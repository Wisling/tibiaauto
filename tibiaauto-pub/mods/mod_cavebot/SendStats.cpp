// SendStats.cpp : implementation file
//

#include "stdafx.h"
#include "mod_cavebot.h"
#include "ModuleUtil.h"
#include "SendStats.h"
#include "zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG


/////////////////////////////////////////////////////////////////////////////
int fileSendingProgress = 1;

/////////////////////////////////////////////////////////////////////////////
// CSendStats dialog


CSendStats::CSendStats(CWnd* pParent /*=NULL*/)
	: MyDialog(CSendStats::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendStats)
	//}}AFX_DATA_INIT
}

void CSendStats::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendStats)
	DDX_Control(pDX, IDC_SEND_LOOTSTATS_CAVEBOT, m_send2);
	DDX_Control(pDX, IDC_SEND_CREATURESTATS_CAVEBOT, m_send1);
	DDX_Control(pDX, IDOK, m_ok);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendStats, CDialog)
//{{AFX_MSG_MAP(CSendStats)
ON_BN_CLICKED(IDC_SEND_LOOTSTATS_CAVEBOT, OnSendLootstats)
ON_BN_CLICKED(IDC_SEND_CREATURESTATS_CAVEBOT, OnSendCreaturestats)
ON_WM_TIMER()
ON_WM_CLOSE()
ON_WM_ERASEBKGND()
ON_WM_DRAWITEM()
ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
////// GZIP deflate (compress)
#define BUFLEN      16384
#define MAX_NAME_LEN 1024

void gz_compress      OF((FILE * in, gzFile out));
void file_compress    OF((char  *file, char *mode));


/* ===========================================================================
 * Compress input to output then close both files.
 */

void gz_compress(FILE *in, gzFile out)
{
	char buf[BUFLEN];
	int len;

	for (;; )
	{
		len = (int)fread(buf, 1, sizeof(buf), in);
		if (ferror(in))
		{
			perror("fread");
			PostQuitMessage(-1);
			return;
		}
		if (len == 0)
			break;

		if (gzwrite(out, buf, (unsigned)len) != len)
		{
			AfxMessageBox("gz error");
			return;
		}
	}
	fclose(in);
	if (gzclose(out) != Z_OK)
		AfxMessageBox("failed gzclose");
}

/* ===========================================================================
 * Compress the given file: create a corresponding .gz file and remove the
 * original.
 */
void file_compress(char *file, char *mode)
{
	char outfile[MAX_NAME_LEN];
	FILE  *in;
	gzFile out;

	strcpy(outfile, file);
	strcat(outfile, ".gz");

	in = fopen(file, "rb");
	if (in == NULL)
	{
		perror(file);
		PostQuitMessage(-1);
		return;
	}
	out = gzopen(outfile, mode);
	if (out == NULL)
	{
		AfxMessageBox("ERROR: unable to make gzip compression");
		return;
	}
	gz_compress(in, out);
}

/////////////////////////////////////////////////////////////////////////////
// CSendStats message handlers

void CSendStats::OnSendLootstats()
{
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf, "%s\\tibiaauto-stats-loot.txt", installPath);
	sendFile(pathBuf);
}

BOOL CSendStats::OnInitDialog()
{
	CDialog::OnInitDialog();
	skin.SetButtonSkin(m_send2);
	skin.SetButtonSkin(m_send1);
	skin.SetButtonSkin(m_ok);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendStats::OnSendCreaturestats()
{
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf, "%s\\tibiaauto-stats-creatures.txt", installPath);
	sendFile(pathBuf);
}

DWORD WINAPI sendFileThread(LPVOID lpParam)
{
	char *fname = (char *)lpParam;

	try
	{
		char fnameGz[128];
		sprintf(fnameGz, "%s.gz", fname);
		char remoteFileName[128];
		sprintf(remoteFileName, "incoming/%s-%d-%d.gz", fname, time(NULL), rand());
		file_compress(fname, "wb");
		CInternetSession session;
		CFtpConnection *ftpConnection = session.GetFtpConnection("upload.tibiaauto.net", "anonymous", "tibiaauto@tibiaauto.net", 21, true);
		ftpConnection->PutFile(fnameGz, remoteFileName);
		ftpConnection->Close();
		delete ftpConnection;


		_unlink(fname);
		_unlink(fnameGz);

		fileSendingProgress = 1;
	}
	catch (CInternetException *e)
	{
		e                   = 0;
		fileSendingProgress = -1;
	}
	return 0;
}

void CSendStats::sendFile(char *fname)
{
	if (fileSendingProgress)
	{
		DWORD threadId;
		fileSendingProgress = 0;
		::CreateThread(NULL, 0, sendFileThread, fname, 0, &threadId);
		m_ok.EnableWindow(false);
		m_send1.EnableWindow(false);
		m_send2.EnableWindow(false);
		SetTimer(10001, 100, NULL);
	}
}

void CSendStats::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 10001)
	{
		// timer for waiting for upload to finish
		KillTimer(10001);

		if (fileSendingProgress)
		{
			if (fileSendingProgress == 1)
				AfxMessageBox("File sent ok. Thank you for support of Tibia Auto!");
			if (fileSendingProgress == -1)
				AfxMessageBox("ERROR during sending a file!");
			m_ok.EnableWindow(true);
			m_send1.EnableWindow(true);
			m_send2.EnableWindow(true);
		}
		else
		{
			SetTimer(10001, 100, NULL);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CSendStats::OnClose()
{
	if (fileSendingProgress)
		CDialog::OnClose();
}
