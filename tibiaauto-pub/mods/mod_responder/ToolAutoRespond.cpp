// ToolAutoRespond.cpp : implementation file
//

#include "stdafx.h"
#include "mod_responder.h"
#include "ToolAutoRespond.h"
#include "ModuleUtil.h"

#include "AutoResponderParser.h"
#include "AutoResponderParserContext.h"
#include "ToolAutoResponderThreadConfig.h"
#include "ToolAutoResponderMessage.h"
#include <IPCBackPipe.h>
#include <MemReader.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////

int toolAutoResponderRunning = 0;

CRITICAL_SECTION QueueCriticalSection;


/////////////////////////////////////////////////////////////////////////////
int toolThreadAutoResponderShouldStop = 0;

DWORD WINAPI toolThreadAutoResponderProc(LPVOID lpParam)
{
	srand((unsigned int)time(NULL));
	CMemReader& reader = CMemReader::getMemReader();
	CToolAutoResponderThreadConfig *config = (CToolAutoResponderThreadConfig *)lpParam;
	while (!toolThreadAutoResponderShouldStop)
	{
		config->status = CToolAutoResponderThreadStatus_waitingForMatch;
		Sleep(50);
		if (!reader.isLoggedIn())
			continue;                   // do not proceed if not connected

		if (config->queue.size() > 0)
		{
			EnterCriticalSection(&QueueCriticalSection);
			CToolAutoResponderMessage *msg;
			if (config->queue.size() > 0) //first check is to prevent continual locking, this check is to make sure it still has items

			{
				msg = config->queue.front();
				config->queue.pop();
			}
			else
			{
				LeaveCriticalSection(&QueueCriticalSection);
				continue;
			}
			LeaveCriticalSection(&QueueCriticalSection);

			if (!config->context->isPlayerIgnored(msg->nick))
			{
				// double check for player ignore (to not overload the queue
				// but also to avoid answering when there is message flood)
				CAutoResponderParser parser(config);
				memcpy(config->context->channel, msg->chan, 16384);
				memcpy(config->context->playerName, msg->nick, 16384);
				memcpy(config->context->message, msg->mess, 16384);
				config->status = CToolAutoResponderThreadStatus_generalParse;
				parser.parseThread(config->threadNode, config->context);
			}
			delete msg;
		}
	}
	toolThreadAutoResponderShouldStop--;
	config->status = CToolAutoResponderThreadStatus_notRunning;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CToolAutoRespond dialog


CToolAutoRespond::CToolAutoRespond(CWnd* pParent /*=NULL*/)
	: MyDialog(CToolAutoRespond::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToolAutoRespond)
	//}}AFX_DATA_INIT
	threadCount = 0;
	// enabling
	XMLPlatformUtils::Initialize();

	parser = new XercesDOMParser();


	InitializeCriticalSection(&QueueCriticalSection);
}

void CToolAutoRespond::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolAutoRespond)
	DDX_Control(pDX, IDC_AUTORESPOND_CLEAR, m_ClearLogs);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_FRAME_RESPONDER_LOGS, m_ResponderLogsFrame);
	DDX_Control(pDX, IDC_AUTORESPOND_LOCALECHO, m_localEcho);
	DDX_Control(pDX, IDC_AUTORESPOND_ENABLEDEBUG, m_debug);
	DDX_Control(pDX, IDC_AUTORESPOND_THREADSTATUS, m_threadStatus);
	DDX_Control(pDX, IDC_AUTORESPOND_SCRIPT, m_script);
	DDX_Control(pDX, IDC_AUTORESPOND_ENABLE, m_enable);
	DDX_Control(pDX, IDC_AUTORESPOND_ACTIONLOG, m_actionLog);
	DDX_Control(pDX, IDC_AUTORESPOND_MSGHISTORY, m_msgHistory);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CToolAutoRespond, CDialog)
//{{AFX_MSG_MAP(CToolAutoRespond)
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_WM_CLOSE()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_AUTORESPOND_ENABLE, OnAutorespondEnable)
ON_BN_CLICKED(IDC_AUTORESPOND_CLEAR, OnAutorespondClear)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolAutoRespond message handlers

void CToolAutoRespond::OnOK()
{
	ShowWindow(SW_HIDE);
}

void CToolAutoRespond::OnClose()
{
	ShowWindow(SW_HIDE);
}

void CToolAutoRespond::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1001)
	{
		KillTimer(1001);

		readInfo();
		refreshThreadStatus();

		if (!toolThreadAutoResponderShouldStop && toolThreadAutoResponderShouldStopWaiting)
		{
			int i;
			if (!m_enable.IsWindowEnabled())
				m_enable.EnableWindow(true);

			if (doc)
			{
				delete doc;
				doc = NULL;
			}
			for (i = 0; i < threadCount; i++)
			{
				delete threadConfig[i].context;
				EnterCriticalSection(&QueueCriticalSection);
				while (!threadConfig[i].queue.empty())
				{
					delete threadConfig[i].queue.front();
					threadConfig[i].queue.pop();
				}
				LeaveCriticalSection(&QueueCriticalSection);
			}
			m_script.SetWindowText("");
			m_threadStatus.DeleteAllItems();

			threadCount = 0;

			toolThreadAutoResponderShouldStopWaiting = 0;
		}


		SetTimer(1001, 100, NULL);
	}

	CDialog::OnTimer(nIDEvent);
}

void CToolAutoRespond::readInfo()
{
	CIpcMessage mess;
	if (CIPCBackPipe::readFromPipe(&mess, 1001))
	{
		int infoType;
		int chanType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];
		char chanBuf[16384];
		char timeBuf[128];
		int i;

		memset(nickBuf, 0, 16384);
		memset(msgBuf, 0, 16384);
		memcpy(&infoType, mess.payload, sizeof(int));
		memcpy(&chanType, mess.payload + 4, sizeof(int));
		memcpy(&nickLen, mess.payload + 8, sizeof(int));
		memcpy(&msgLen, mess.payload + 12, sizeof(int));
		memcpy(nickBuf, mess.payload + 16, nickLen);
		memcpy(msgBuf, mess.payload + 16 + nickLen, msgLen);
		switch (infoType)//received message types
		{
		case 1:
			sprintf(chanBuf, "say[%d]", infoType);
			break;
		case 2:
			sprintf(chanBuf, "whisper[%d]", infoType);
			break;
		case 3:
			sprintf(chanBuf, "yell[%d]", infoType);
			break;
		case 5:
			sprintf(chanBuf, "NPC[%d]", infoType);
			break;
		case 6:
			sprintf(chanBuf, "private[%d]", infoType);
			break;
		case 7:
			sprintf(chanBuf, "channel[%d,%d]", infoType, chanType);
			break;
		default:
			sprintf(chanBuf, "other[%d]", infoType);
			break;
		}
		//Channel IDs
		//1-Party
		//2-Own Chat Channel(premium)
		//3-Game Chat
		//4-Trade
		//5-Rook Trade
		//6-RL chat
		//7-Help
		time_t nowSec  = time(NULL);
		struct tm *now = localtime(&nowSec);
		sprintf(timeBuf, "%d:%d:%d", now->tm_hour, now->tm_min, now->tm_sec);

		m_msgHistory.InsertItem(0, "");
		m_msgHistory.SetItemText(0, 0, timeBuf);
		m_msgHistory.SetItemText(0, 1, chanBuf);
		m_msgHistory.SetItemText(0, 2, nickBuf);
		m_msgHistory.SetItemText(0, 3, msgBuf);

		if (m_msgHistory.GetItemCount() > 500)
			m_msgHistory.DeleteItem(500);

		for (i = 0; i < threadCount; i++)
		{
			if (!threadConfig[i].context->isPlayerIgnored(nickBuf))
			{
				EnterCriticalSection(&QueueCriticalSection);
				threadConfig[i].queue.push(new CToolAutoResponderMessage(chanBuf, msgBuf, nickBuf));
				LeaveCriticalSection(&QueueCriticalSection);
			}
		}
	}
}

void CToolAutoRespond::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_enable);
	skin.SetButtonSkin(m_ClearLogs);
	skin.SetButtonSkin(m_OK);
}

BOOL CToolAutoRespond::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	m_msgHistory.InsertColumn(0, "time", LVCFMT_LEFT, 50);
	m_msgHistory.InsertColumn(1, "channel", LVCFMT_LEFT, 70);
	m_msgHistory.InsertColumn(2, "author", LVCFMT_LEFT, 100);
	m_msgHistory.InsertColumn(3, "message", LVCFMT_LEFT, 330);

	m_actionLog.InsertColumn(0, "log message", LVCFMT_LEFT, 550);

	m_threadStatus.InsertColumn(0, "id", LVCFMT_LEFT, 50);
	m_threadStatus.InsertColumn(1, "status", LVCFMT_LEFT, 100);
	m_threadStatus.InsertColumn(2, "backlog", LVCFMT_LEFT, 50);

	toolThreadAutoResponderShouldStop        = 0;
	threadCount                              = 0;
	doc                                      = NULL;
	toolThreadAutoResponderShouldStopWaiting = 0;


	SetTimer(1001, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CToolAutoRespond::OnAutorespondEnable()
{
	if (m_enable.GetCheck())
		start();
	else
		stop();
}

void CToolAutoRespond::OnAutorespondClear()
{
	m_actionLog.DeleteAllItems();
}

void CToolAutoRespond::refreshThreadStatus()
{
	int i;
	if (m_threadStatus.GetItemCount() != threadCount)
	{
		// thread list not up to date - refresh it
		m_threadStatus.DeleteAllItems();
		for (i = 0; i < threadCount; i++)
		{
			char buf[128];
			sprintf(buf, "%d", i + 1);
			m_threadStatus.InsertItem(i, buf);
		}
	}
	for (i = 0; i < threadCount; i++)
	{
		char buf[128];
		char prevBuf[128];
		switch (threadConfig[i].status)
		{
		case CToolAutoResponderThreadStatus_notRunning:
			sprintf(buf, "not running");
			break;
		case CToolAutoResponderThreadStatus_waitingForMatch:
			sprintf(buf, "waiting");
			break;
		case CToolAutoResponderThreadStatus_generalParse:
			sprintf(buf, "parsing");
			break;
		case CToolAutoResponderThreadStatus_sleep:
			sprintf(buf, "sleeping");
			break;
		default:
			sprintf(buf, "UNKNOWN");
			break;
		}
		m_threadStatus.GetItemText(i, 1, prevBuf, 127);
		if (strcmp(buf, prevBuf))
			m_threadStatus.SetItemText(i, 1, buf);
		sprintf(buf, "%d", threadConfig[i].queue.size());
		m_threadStatus.GetItemText(i, 2, prevBuf, 127);
		if (strcmp(buf, prevBuf))
			m_threadStatus.SetItemText(i, 2, buf);
	};
}

CToolAutoRespond::~CToolAutoRespond()
{
	DeleteCriticalSection(&QueueCriticalSection);
}

void CToolAutoRespond::start()
{
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);

	char pathBuf[2048];

	m_enable.SetCheck(1);
	threadCount = 0;
	try
	{
		sprintf(pathBuf, "%s\\mods\\tibiaauto-responder.xml", installPath);
		parser->parse(pathBuf);
		doc = parser->getDocument();
		for (size_t rootNr = 0; rootNr < doc->getChildNodes()->getLength(); rootNr++)
		{
			DOMNode *root = doc->getChildNodes()->item(rootNr);
			if (wcscmp(root->getNodeName(), L"responder"))
				continue;
			for (size_t threadNr = 0; threadNr < root->getChildNodes()->getLength(); threadNr++)
			{
				DOMNode *threadNode = root->getChildNodes()->item(threadNr);
				if (wcscmp(threadNode->getNodeName(), L"thread"))
					continue;


				if (wcscmp(threadNode->getNodeName(), L"thread"))
				{
					char buf[1024];
					sprintf(buf, "Responder parse error: unknown tag '%s'", _T(threadNode->getNodeName()));
					AfxMessageBox(buf);
					continue;
				}

				threadConfig[threadCount].threadNode = threadNode;
				threadConfig[threadCount].context    = new CAutoResponderParserContext(m_debug.GetCheck() ? &m_actionLog : NULL, m_localEcho.GetCheck());

				threadCount++;
				if (threadCount > MAX_THREADS + 5)
				{
					AfxMessageBox("Auto responder: too many threads!");
					threadCount--;
				}
			}
		}
		char scriptBuf[1024 * 200];
		memset(scriptBuf, 0, 1024 * 200);
		sprintf(pathBuf, "%s\\mods\\tibiaauto-responder.xml", installPath);
		FILE *f = fopen(pathBuf, "rb");
		fread(scriptBuf, 1, 1024 * 200 - 1, f);
		fclose(f);
		m_script.SetWindowText(scriptBuf);
	}
	catch (...)
	{
		doc = NULL;
		AfxMessageBox("Unable to initialise auto responder!");
	}

	if (doc)
	{
		int i;
		for (i = 0; i < threadCount; i++)
		{
			CToolAutoResponderThreadConfig *config = &threadConfig[i];

			DWORD threadId;
			CreateThread(NULL, 0, toolThreadAutoResponderProc, config, 0, &threadId);
		}
		toolAutoResponderRunning = 1;
	}
}

void CToolAutoRespond::stop()
{
	m_enable.SetCheck(0);
	// disabling
	// step 1 -> uninitialised everything
	toolThreadAutoResponderShouldStop        = threadCount;
	toolThreadAutoResponderShouldStopWaiting = 1;

	m_enable.EnableWindow(true);
	//m_debug.EnableWindow(true);  //mysteriously hangs TA when disabling then enabling button
	toolAutoResponderRunning = 0;
}
