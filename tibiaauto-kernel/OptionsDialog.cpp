// OptionsDialog.cpp : implementation file
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxsock.h>
#include <Tlhelp32.h>


#include <afxinet.h>
#include <queue>
#include <vector>

#include "ipcm.h"

#include "stdafx.h"
#include "tibiaauto.h"
#include "ModuleUtil.h"
#include "OptionsDialog.h"
#include "ColorChooser.h"
#include "zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CUSTOM_SELCHANGE 123456
/////////////////////////////////////////////////////////////////////////////
int fileSendingProgress=1 ;

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog


COptionsDialog::COptionsDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(COptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDialog)
	//}}AFX_DATA_INIT
}

 
void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDialog)
	DDX_Control(pDX, IDC_SKIN, m_Skin);
	DDX_Control(pDX, IDC_USAGE_STATISTICS, m_UsageStatistics);
	DDX_Control(pDX, IDC_TIBIA_MAPS, m_TibiaMaps);
	DDX_Control(pDX, IDC_LOOT_STATISTICS, m_LootStatistics);
	DDX_Control(pDX, IDC_CREATURE_STATISTICS, m_CreatureStatistics);
	DDX_Control(pDX, IDC_LOADING_SCRIPT_OPTIONS,m_loadingScriptOptions);
	DDX_Control(pDX, IDC_SIZE_USAGESTATS, m_sizeUsagestats);
	DDX_Control(pDX, IDC_SEND_USAGESTATS, m_send4);
	DDX_Control(pDX, IDC_SEND_MAPS, m_send3);
	DDX_Control(pDX, IDC_SEND_LOOTSTATS, m_send2);
	DDX_Control(pDX, IDC_SEND_CREATURESTATS, m_send1);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_GATHER_BOT_STATS,m_gatherBotStats);
	DDX_Control(pDX, IDC_LOAD_ON_STARTUP,m_loadOnStartup);
	DDX_Control(pDX, IDC_HIDE_TIBIA,m_hideTibia);
	DDX_Control(pDX, IDC_SCRIPTLOAD_ASK,m_scriptLoadAsk);
	DDX_Control(pDX, IDC_SCRIPTLOAD_START,m_scriptLoadStart);
	DDX_Control(pDX, IDC_SCRIPTLOAD_NOSTART,m_scriptLoadNoStart);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_SIZE_CREATURESTATS, m_sizeCreatureStats);
	DDX_Control(pDX, IDC_SIZE_LOOTSTATS, m_sizeLootstats);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	//{{AFX_MSG_MAP(COptionsDialog)
	ON_BN_CLICKED(IDC_SEND_LOOTSTATS, OnSendLootstats)
	ON_BN_CLICKED(IDC_SEND_CREATURESTATS, OnSendCreaturestats)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_SEND_MAPS, OnSendMaps)
	ON_BN_CLICKED(IDC_SEND_USAGESTATS, OnSendUsagestats)
	ON_BN_CLICKED(IDC_SKIN, OnSkin)
	ON_BN_CLICKED(IDC_GATHER_BOT_STATS, OnGatherBotStats)
	ON_BN_CLICKED(IDC_LOAD_ON_STARTUP, OnLoadOnStartup)
	ON_BN_CLICKED(IDC_SCRIPTLOAD_ASK, OnScriptloadAsk)
	ON_BN_CLICKED(IDC_HIDE_TIBIA, OnHideTibia)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SCRIPTLOAD_NOSTART, OnScriptloadNostart)
	ON_BN_CLICKED(IDC_SCRIPTLOAD_START, OnScriptloadStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
////// GZIP deflate (compress)
#define BUFLEN      16384
#define MAX_NAME_LEN 1024

void gz_compress      OF((FILE   *in, gzFile out));
void file_compress    OF((char  *file, char *mode));


/* ===========================================================================
 * Compress input to output then close both files.
 */

void gz_compress(FILE *in, gzFile out)
{
    char buf[BUFLEN];
    int len;

    for (;;) {
        len = (int)fread(buf, 1, sizeof(buf), in);
        if (ferror(in)) {
			char msgBuf[2048];
			sprintf(msgBuf,"ERROR: Unable to send file. %s",strerror(errno));
			AfxMessageBox(msgBuf);
            return;
        }
        if (len == 0) break;

        if (gzwrite(out, buf, (unsigned)len) != len)
		{
			AfxMessageBox("gz error");
			return;
		}
    }
    fclose(in);
    if (gzclose(out) != Z_OK)
	{
		AfxMessageBox("failed gzclose");
	}
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
    if (in == NULL) {
		char msgBuf[2048];
        sprintf(msgBuf,"ERROR: Unable to send file. %s  %s",strerror(errno), file);
		AfxMessageBox(msgBuf);
		return;
    }
    out = gzopen(outfile, mode);
    if (out == NULL) {
		AfxMessageBox("ERROR: unable to make gzip compression");
        return;
    }
    gz_compress(in, out);

    
}

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog message handlers

void COptionsDialog::OnSendLootstats()
{
	sendFile("tibiaauto-stats-loot.txt");
}

void COptionsDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_ok);
	skin.SetButtonSkin(m_Skin);
	skin.SetButtonSkin(m_send1);
	skin.SetButtonSkin(m_send2);
	skin.SetButtonSkin(m_send3);
	skin.SetButtonSkin(m_send4);
}

BOOL COptionsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_gatherBotStats.SetCheck(CModuleUtil::getTASetting("GatherBotStats"));
	m_loadOnStartup.SetCheck(CModuleUtil::getTASetting("LoadScriptOnStartup"));
	m_hideTibia.SetCheck(CModuleUtil::getTASetting("HideTibiaWithTA"));
	m_scriptLoadAsk.SetCheck(CModuleUtil::getTASetting("StartModulesOnLoad")==0);
	m_scriptLoadStart.SetCheck(CModuleUtil::getTASetting("StartModulesOnLoad")==1);
	m_scriptLoadNoStart.SetCheck(CModuleUtil::getTASetting("StartModulesOnLoad")==2);
	DoSetButtonSkin();

	refreshStatFiles();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDialog::refreshStatFiles()
{

	char buf[128];
	FILE *f=NULL;
	int flen=0;
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);

	char pathBuf[2048];
	sprintf(pathBuf,"%s\\tibiaauto-stats-loot.txt",installPath);
	f = fopen(pathBuf,"a+");
	fseek(f,0,SEEK_END);
	flen=ftell(f);
	fclose(f);
	sprintf(buf,"%dk",flen/1024);
	m_sizeLootstats.SetWindowText(buf);

	sprintf(pathBuf,"%s\\tibiaauto-stats-creatures.txt",installPath);
	f = fopen(pathBuf,"a+");
	fseek(f,0,SEEK_END);
	flen=ftell(f);
	fclose(f);
	sprintf(buf,"%dk",flen/1024);
	m_sizeCreatureStats.SetWindowText(buf);

	sprintf(pathBuf,"%s\\tibiaauto-stats-usage.txt",installPath);
	f = fopen(pathBuf,"a+");
	fseek(f,0,SEEK_END);
	flen=ftell(f);
	fclose(f);
	sprintf(buf,"%dk",flen/1024);
	m_sizeUsagestats.SetWindowText(buf);

}

void COptionsDialog::OnSendCreaturestats()
{
	sendFile("tibiaauto-stats-creatures.txt");
}




DWORD WINAPI sendFileThread( LPVOID lpParam )
{
	char *filename=(char *)lpParam;
	try
	{
		char installPath[1024];
		CModuleUtil::getInstallPath(installPath);

		char fname[1024];
		sprintf(fname,"%s\\%s",installPath,filename);
		char fnameGz[1024];
		sprintf(fnameGz,"%s\\%s.gz",installPath,filename);
		char remoteFileName[128];
		sprintf(remoteFileName,"incoming/%s-%d-%d.gz",filename,time(NULL),rand());
		file_compress(fname,"wb");
		CInternetSession session;
		CFtpConnection *ftpConnection = session.GetFtpConnection("upload.tibiaauto.net","anonymous","tibiaauto@tibiaauto.net",21,true);
		ftpConnection->PutFile(fnameGz,remoteFileName);
		ftpConnection->Close();
		delete ftpConnection;
		
		
		_unlink(fname);
		_unlink(fnameGz);

		fileSendingProgress=1;
	} catch (CInternetException *e)
	{
		fileSendingProgress=-1;
	}
	return 0;
}

DWORD WINAPI sendMapsThread( LPVOID lpParam )
{
	char *path=(char *)lpParam;
	
	try
	{
		char fullMask[1024];
		sprintf(fullMask,"%s\\*.map",path);
		WIN32_FIND_DATA data;
		HANDLE hFind = FindFirstFile(fullMask,&data);
		if (hFind!=INVALID_HANDLE_VALUE)
		{
			CInternetSession session;
			CFtpConnection *ftpConnection = session.GetFtpConnection("upload.tibiaauto.net","anonymous","tibiaauto@tibiaauto.net",21,true);
			time_t t = time(NULL);
			int r=rand();
			int lastfile=1;
			while(lastfile)
			{
				char fname[128];
				char fnameGz[128];
				sprintf(fname,"%s\\%s",path,data.cFileName);
				sprintf(fnameGz,"%s\\%s.gz",path,data.cFileName);
				char remoteFileName[128];
				sprintf(remoteFileName,"incoming/%s-%d-%d.gz",data.cFileName,t,r);
				file_compress(fname,"wb");
								
				ftpConnection->PutFile(fnameGz,remoteFileName);
								
				_unlink(fnameGz);
				lastfile=FindNextFile(hFind,&data);
			}
			ftpConnection->Close();
			delete ftpConnection;
		}
		
		fileSendingProgress=1;
	} catch (CInternetException *e)
	{
		fileSendingProgress=-1;
	}
	// allocated before starting a thread
	free(path);
	return 0;
}



void COptionsDialog::sendFile(char *fname)
{
	if (fileSendingProgress)
	{
		DWORD threadId;
		fileSendingProgress=0;
		::CreateThread(NULL,0,sendFileThread,fname,0,&threadId);
		m_ok.EnableWindow(false);
		m_send1.EnableWindow(false);
		m_send2.EnableWindow(false);
		m_send3.EnableWindow(false);
		m_send4.EnableWindow(false);
		m_progress.SetPos(0);
		m_progress.SetRange(0,100);
		m_progress.ShowWindow(SW_SHOW);
		SetTimer(10001,100,NULL);
	}
}

void COptionsDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==10001)
	{
		// timer for waiting for upload to finish
		KillTimer(10001);

		m_progress.SetPos((m_progress.GetPos()+1)%100);
		if (fileSendingProgress)
		{
			if (fileSendingProgress==1)
			{
				AfxMessageBox("File sent ok. Thank you for support of Tibia Auto!");
			}
			if (fileSendingProgress==-1)
			{
				AfxMessageBox("ERROR during sending a file!");
			}
			m_ok.EnableWindow(true);
			m_progress.ShowWindow(SW_HIDE);
			m_send1.EnableWindow(true);
			m_send2.EnableWindow(true);
			m_send3.EnableWindow(true);
			m_send4.EnableWindow(true);
			refreshStatFiles();
		} else {
			SetTimer(10001,100,NULL);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void COptionsDialog::OnClose()
{
	if (fileSendingProgress)
	{
		CDialog::OnClose();
	}
}

void COptionsDialog::OnSendMaps()
{
	char *tibiaPath=(char *)malloc(1024);
	unsigned long tibiaPathLen=1023;
	tibiaPath[0]='\0';
	/* Maps now stored in absolute directory
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
	{
		RegQueryValueEx(hkey,TEXT("TibiaClientHome"),NULL,NULL,(unsigned char *)tibiaPath,&tibiaPathLen);
		RegCloseKey(hkey);
	}*/
	sprintf(tibiaPath,"%s%s",getenv("USERPROFILE"),"/Application Data/Tibia/Automap/");
	if (!strlen(tibiaPath))
	{
		AfxMessageBox("ERROR! Unable to read Tibia client directory! Please reinstall!");
		exit(1);
	}

	sendMaps(tibiaPath);
	
}

void COptionsDialog::sendMaps(char *path)
{
	if (fileSendingProgress)
	{
		DWORD threadId;
		fileSendingProgress=0;
		::CreateThread(NULL,0,sendMapsThread,path,0,&threadId);
		m_ok.EnableWindow(false);
		m_send1.EnableWindow(false);
		m_send2.EnableWindow(false);
		m_send3.EnableWindow(false);
		m_send4.EnableWindow(false);
		m_progress.SetPos(0);
		m_progress.SetRange(0,100);
		m_progress.ShowWindow(SW_SHOW);
		SetTimer(10001,100,NULL);
	}
}

void COptionsDialog::OnSendUsagestats()
{
	sendFile("tibiaauto-stats-usage.txt");
}

void COptionsDialog::OnSkin()
{
	// TODO: Add your control notification handler code here
	CColorChooser *dlg = new CColorChooser();
	if (dlg->DoModal() == IDOK) {
		DoSetButtonSkin();
		this->Invalidate();

		NMHDR nmh;
		nmh.code = CUSTOM_SELCHANGE;    // Message type defined by control.
		nmh.idFrom = this->GetDlgCtrlID();
		nmh.hwndFrom = this->GetSafeHwnd();
		GetParent()->SendMessage(WM_NOTIFY, (WPARAM)this->GetSafeHwnd(), (LPARAM)&nmh);	}

	delete dlg;
}

DWORD WINAPI sendPacketLogThread( LPVOID lpParam )
{
	char path[1024];
	CModuleUtil::getInstallPath(path);
	char fullMask[1024];
	sprintf(fullMask,"%s\\tascripts\\* statistics.txt",path);
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(fullMask,&data);
	if (hFind!=INVALID_HANDLE_VALUE){
		char fname[128];
		int lastfile=1;
		while(lastfile)
		{
			sprintf(fname,"%s\\tascripts\\%s",path,data.cFileName);
			FILE* f = fopen(fname,"a+");
			if (f){
				fseek(f,0,SEEK_END);
				int flen=ftell(f);
				fclose(f);
				if (flen>1000)
					goto sendFiles;
			}
			lastfile=FindNextFile(hFind,&data);
		}
	}
	{
		hFind = FindFirstFile(fullMask,&data);
		if (hFind!=INVALID_HANDLE_VALUE){
			char fname[128];
			int lastfile=1;
			while(lastfile)
			{
				sprintf(fname,"%s\\tascripts\\%s",path,data.cFileName);
				_unlink(fname);
				lastfile=FindNextFile(hFind,&data);
			}
		}
		fileSendingProgress=1;
		return 0;
	}
sendFiles:
	int msgboxID=-1;
	if(!CModuleUtil::getTASetting("RemoveBotStatsMessage")){
		msgboxID = MessageBox(NULL,"TA is about to send your botting statistics to TA.net. You can edit this option in \"General options and statistics\".\n\nWould you like to receive this message every time before TA sends this data?\nUse cancel to stop the operation.","Submit Botting Data",MB_YESNOCANCEL);
	}
	if (msgboxID==IDCANCEL) return 0;
	try
	{
		hFind = FindFirstFile(fullMask,&data);
		if (hFind!=INVALID_HANDLE_VALUE)
		{
			CInternetSession session;
			CFtpConnection *ftpConnection = session.GetFtpConnection("upload.tibiaauto.net","anonymous","tibiaauto@tibiaauto.net",21,true);
			time_t t = time(NULL);
			unsigned long serialNumber;
			GetVolumeInformation(NULL,NULL,0,&serialNumber,NULL,NULL,NULL,0);
			unsigned int r=serialNumber%0x1000000;
			int lastfile=1;
			while(lastfile)
			{
				char fname[128];
				char fnameGz[128];
				sprintf(fname,"%s\\tascripts\\%s",path,data.cFileName);
				sprintf(fnameGz,"%s\\tascripts\\%s.gz",path,data.cFileName);
				char remoteFileName[128];
				sprintf(remoteFileName,"incoming/%s-%d-%u.gz",data.cFileName,t,r);
				file_compress(fname,"wb");
								
				ftpConnection->PutFile(fnameGz,remoteFileName);
								
				_unlink(fnameGz);
				lastfile=FindNextFile(hFind,&data);
				_unlink(fname);
			}
			ftpConnection->Close();
			delete ftpConnection;
		}

		fileSendingProgress=1;
		if(!CModuleUtil::getTASetting("RemoveBotStatsMessage")){
			AfxMessageBox("Thank you for submitting \"botting statistics.txt\" and \"module statistics.txt\".\n\nNo personally identifiable information was sent.\nThis botting information will be analysed and used to help prevent CIPSoft from automatically detecting TA in the future. You can change this setting in \"General Options and Statistics\".\n\nTA users thank you for helping us towards this end.\n~TA Team");
		}

	} catch (CInternetException *e)
	{
		if(!CModuleUtil::getTASetting("RemoveBotStatsMessage")){
			AfxMessageBox("Failed to send file. Check your connection to the internet.");
		}
		fileSendingProgress=-1;
	}
	if(msgboxID==IDNO){
		CModuleUtil::setTASetting("RemoveBotStatsMessage",1);
	}
	return 0;
}

void COptionsDialog::sendStats()
{
	if (fileSendingProgress)
	{
		fileSendingProgress=0;
		DWORD threadId;
		::CreateThread(NULL,0,sendPacketLogThread,NULL,0,&threadId);
	}
	OnClose();
}

void COptionsDialog::OnGatherBotStats()
{
	CModuleUtil::setTASetting("GatherBotStats",m_gatherBotStats.GetCheck());
}

void COptionsDialog::OnLoadOnStartup()
{
	CModuleUtil::setTASetting("LoadScriptOnStartup",m_loadOnStartup.GetCheck());
	if (m_loadOnStartup.GetCheck()){
		char pathbuf[2048];
		CModuleUtil::getInstallPath(pathbuf);
		char message[5000];
		sprintf(message,"Save files to your TA directory in the form of\n%s\\tibiaAuto.cfg.<char name>.xml",pathbuf);
		AfxMessageBox(message);
	}
}

void COptionsDialog::OnHideTibia()
{
	CModuleUtil::setTASetting("HideTibiaWithTA",m_hideTibia.GetCheck());
}

void COptionsDialog::OnScriptloadAsk()
{
	CModuleUtil::setTASetting("StartModulesOnLoad",0);
	m_scriptLoadStart.SetCheck(0);
	m_scriptLoadNoStart.SetCheck(0);
}

void COptionsDialog::OnScriptloadStart()
{
	CModuleUtil::setTASetting("StartModulesOnLoad",1);
	m_scriptLoadAsk.SetCheck(0);
	m_scriptLoadNoStart.SetCheck(0);
}

void COptionsDialog::OnScriptloadNostart()
{
	CModuleUtil::setTASetting("StartModulesOnLoad",2);
	m_scriptLoadAsk.SetCheck(0);
	m_scriptLoadStart.SetCheck(0);
}
