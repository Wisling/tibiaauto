// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "OptionsDialog.h"
#include "ColorChooser.h"
#include "zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CUSTOM_SELCHANGE 1
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
	DDX_Control(pDX, IDC_USAGE_STATISTICS, m_UsageStatistics);
	DDX_Control(pDX, IDC_TIBIA_MAPS, m_TibiaMaps);
	DDX_Control(pDX, IDC_LOOT_STATISTICS, m_LootStatistics);
	DDX_Control(pDX, IDC_CREATURE_STATISTICS, m_CreatureStatistics);
	DDX_Control(pDX, IDC_SIZE_USAGESTATS, m_sizeUsagestats);
	DDX_Control(pDX, IDC_SEND_USAGESTATS, m_send4);
	DDX_Control(pDX, IDC_SEND_MAPS, m_send3);
	DDX_Control(pDX, IDC_SEND_LOOTSTATS, m_send2);
	DDX_Control(pDX, IDC_SEND_CREATURESTATS, m_send1);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
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
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
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
            perror("fread");
            exit(1);
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
        perror(file);
        exit(1);
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

BOOL COptionsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	refreshStatFiles();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDialog::refreshStatFiles()
{

	char buf[128];
	FILE *f=NULL;
	int flen=0;
	
	f = fopen("tibiaauto-stats-loot.txt","a+");
	fseek(f,0,SEEK_END);
	flen=ftell(f);
	fclose(f);		
	sprintf(buf,"%dk",flen/1024);
	m_sizeLootstats.SetWindowText(buf);	

	f = fopen("tibiaauto-stats-creatures.txt","a+");
	fseek(f,0,SEEK_END);
	flen=ftell(f);
	fclose(f);		
	sprintf(buf,"%dk",flen/1024);
	m_sizeCreatureStats.SetWindowText(buf);	

	f = fopen("tibiaauto-stats-usage.txt","a+");
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
	char *fname=(char *)lpParam;

	try
	{
		char fnameGz[128];
		sprintf(fnameGz,"%s.gz",fname);
		char remoteFileName[128];
		sprintf(remoteFileName,"incoming/%s-%d-%d.gz",fname,time(NULL),rand());
		file_compress(fname,"wb");		
		CInternetSession session;	
		CFtpConnection *ftpConnection = session.GetFtpConnection("upload.tibiaauto.net","anonymous","tibiaauto@tibiaauto.net",21,true);
		ftpConnection->PutFile(fnameGz,remoteFileName);
		ftpConnection->Close();
		delete ftpConnection;
		
		
		unlink(fname);
		unlink(fnameGz);		

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
			int t=time(NULL);
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
								
				unlink(fnameGz);	 	
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
		this->RedrawWindow();
NMHDR nmh;
nmh.code = CUSTOM_SELCHANGE;    // Message type defined by control.
nmh.idFrom = this->GetDlgCtrlID();
nmh.hwndFrom = this->GetSafeHwnd();
GetParent()->SendMessage(WM_NOTIFY, 
    (WPARAM)this->GetSafeHwnd(), 
    (LPARAM)&nmh);	}

	//delete dlg;
}
