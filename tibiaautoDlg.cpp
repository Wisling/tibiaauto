// tibiaautoDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "vld.h"
#include "tibiaauto.h"
#include "tibiaautoDlg.h"
#include "MemUtil.h" 
#include "ModuleUtil.h"
#include "CharDialog.h"
#include "MemReaderProxy.h"
#include "TibiaMapProxy.h"
#include "TAMiniMapProxy.h"
#include "TibiaItemProxy.h"
#include "TibiaCharacter.h"
#include "ModuleProxy.h"
#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ConfigCreatorUtil.h"
#include "DonationDialog.h"
#include "IPCBackPipeProxy.h"
#include "TibiaItemProxy.h"
#include "EnterCode.h"
#include "md5class.h"
#include "OptionsDialog.h"
#include "detours.h"
#include "PythonEngine.h" 
#include "PythonScriptsDialog.h"
#include "PythonScript.h"
#include "url.h"
#include <iostream>
#include <fstream>


HANDLE hPipe=INVALID_HANDLE_VALUE;


extern int toolAutoResponderRunning;
extern volatile char *checksum;

int globalProcessId;

// other externs
CTibiaMapProxy tibiaMap;


// xerces things
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMWriter.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

class XStr
{
public :
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }

    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }
private:
    XMLCh*   fUnicodeForm;
};

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define X(str) XStr(str).unicodeForm()



/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg dialog

CTibiaautoDlg::CTibiaautoDlg(CWnd* pParent /*=NULL*/)
	: MyDialog(CTibiaautoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTibiaautoDlg)	
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	XMLPlatformUtils::Initialize();		
		 
	parser = new XercesDOMParser();	

	
}

void CTibiaautoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTibiaautoDlg)
	DDX_Control(pDX, IDC_EXIT, m_exit);
	DDX_Control(pDX, IDC_LOADED_MODULES, m_LoadedModules);
	DDX_Control(pDX, IDC_OPTIONS, m_Options);
	DDX_Control(pDX, IDC_PYTHON_SCRIPTS, m_PythonScripts);
	DDX_Control(pDX, IDC_TOOL_MONSERSHOW, m_MonsterShow);
	DDX_Control(pDX, IDC_TOOL_MAPSHOW, m_MapShow);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG, m_ItemConfig);
	DDX_Control(pDX, IDC_TOOL_INJECTMC, m_InjectMC);
	DDX_Control(pDX, IDC_TOOL_CHARINFO, m_CharInfo);
	DDX_Control(pDX, IDC_OTHER_TOOLS, m_OtherTools);
	DDX_Control(pDX, IDC_INFORMATION_TOOLS, m_InformationTools);
	DDX_Control(pDX, IDC_FIGHT_TOOLS, m_FightTools);
	DDX_Control(pDX, IDC_CONFIG, m_Config);
	DDX_Control(pDX, IDC_CAVEBOT_TOOLS, m_CavebotTools);
	DDX_Control(pDX, IDC_AFK_TOOLS, m_AFKTools);
	DDX_Control(pDX, IDC_TOOL_XRAY, m_xray);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK, m_autoAttack);
	DDX_Control(pDX, IDC_TOOL_LOGIN, m_login);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK, m_ammoRestack);
	DDX_Control(pDX, IDC_TOOL_AUTOAIM, m_autoAim);
	DDX_Control(pDX, IDC_TOOL_AUTOFISH, m_autoFish);
	DDX_Control(pDX, IDC_TOOL_AUTOGO, m_autoGo);
	DDX_Control(pDX, IDC_TOOL_AUTOLOOTER, m_autoLooter);
	DDX_Control(pDX, IDC_TOOL_AUTORESPOND, m_autoRespond);
	DDX_Control(pDX, IDC_TOOL_AUTOUH, m_autoUh);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER, m_fluidDrinker);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER, m_runeMaker);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER, m_spellCaster);
	DDX_Control(pDX, IDC_FPS, m_fps);
	DDX_Control(pDX, IDC_TOOL_ANTILOG, m_antilogout);
	DDX_Control(pDX, IDC_TOOL_MAPHACK, m_mapHack);
	DDX_Control(pDX, IDC_TOOL_CREATUREINFO, m_creatureInfo);
	DDX_Control(pDX, IDC_SAVE, m_save);
	DDX_Control(pDX, IDC_LOAD, m_load);
	DDX_Control(pDX, IDC_TOOL_EATER, m_eater);
	DDX_Control(pDX, IDC_TOOL_TRADEMON, m_tradeMon);
	DDX_Control(pDX, IDC_TOOL_AUTOGROUP, m_grouping);
	DDX_Control(pDX, IDC_LIGHT, m_light);
	DDX_Control(pDX, IDC_TOOL_BANKER, m_banker);
	DDX_Control(pDX, IDC_TOOL_SORTER, m_sorter);
	DDX_Control(pDX, IDC_TOOL_SELLER, m_seller);
	DDX_Control(pDX, IDC_LOGINNAME, m_loginName);
	DDX_Control(pDX, IDC_BROWSER_ADS, m_browserAds);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTibiaautoDlg, CDialog)
	//{{AFX_MSG_MAP(CTibiaautoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_ADDRESS, OnChangeEditAddress)
	ON_EN_CHANGE(IDC_EDIT_VALUE, OnChangeEditValue)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_LIGHT, OnLight)
	ON_BN_CLICKED(IDC_TOOL_RUNEMAKER, OnToolRunemaker)
	ON_BN_CLICKED(IDC_TOOL_CHARINFO, OnToolCharinfo)
	ON_BN_CLICKED(IDC_TOOL_MONSERSHOW, OnToolMonsershow)
	ON_BN_CLICKED(IDC_TOOL_AUTOGO, OnToolAutogo)
	ON_BN_CLICKED(IDC_TOOL_AUTOFISH, OnToolAutofish)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_TOOL_AUTOGROUP, OnToolAutogroup)
	ON_BN_CLICKED(IDC_TOOL_AUTOUH, OnToolAutouh)
	ON_BN_CLICKED(IDC_TOOL_MAPSHOW, OnToolMapshow)
	ON_BN_CLICKED(IDC_TOOL_AUTOATTACK, OnToolAutoattack)	
	ON_BN_CLICKED(IDC_TOOL_SPELLCASTER, OnToolSpellcaster)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TOOL_AUTOAIM, OnToolAutoaim)
	ON_BN_CLICKED(IDC_TOOL_MEMDEBUG, OnMemdebug)
	ON_BN_CLICKED(IDC_TOOL_FLUIDDRINKER, OnToolFluiddrinker)
	ON_BN_CLICKED(IDC_TOOL_TRADEMON, OnToolTrademon)
	ON_BN_CLICKED(IDC_TOOL_INJECTMC, OnToolInjectmc)
	ON_BN_CLICKED(IDC_TOOL_ITEMCONFIG, OnToolItemconfig)
	ON_BN_CLICKED(IDC_TOOL_AMMORESTACK, OnToolAmmorestack)
	ON_BN_CLICKED(IDC_TOOL_AUTORESPOND, OnToolAutorespond)
	ON_BN_CLICKED(IDC_LOADED_MODULES, OnLoadedModules)
	ON_BN_CLICKED(IDC_TOOL_AUTOLOOTER, OnToolAutolooter)
	ON_BN_CLICKED(IDC_TOOL_EATER, OnToolEater)
	ON_BN_CLICKED(IDC_TOOL_CREATUREINFO, OnToolCreatureinfo)
	ON_BN_CLICKED(IDC_TOOL_MAPHACK, OnToolMaphack)
	ON_BN_CLICKED(IDC_TOOL_TEAM, OnToolTeam)
	ON_BN_CLICKED(IDC_TOOL_ANTILOG, OnToolAntilog)
	ON_BN_CLICKED(IDC_FPS, OnFps)
	ON_BN_CLICKED(IDC_PYTHON_SCRIPTS, OnPythonScripts)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_TOOL_LOGIN, OnToolLogin)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()	
	ON_BN_CLICKED(IDC_TOOL_XRAY, OnToolXray)
	ON_BN_CLICKED(IDC_TOOL_SORTER, OnToolSorter)
	ON_BN_CLICKED(IDC_TOOL_BANKER, OnToolBanker)
	ON_BN_CLICKED(IDC_TOOL_SELLER, OnToolSeller)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg message handlers
 
BOOL CTibiaautoDlg::OnInitDialog()
{	
	srand(time(NULL));
	CDialog::OnInitDialog();
	skin.SetButtonSkin(	m_exit);
	skin.SetButtonSkin(	m_LoadedModules);
	skin.SetButtonSkin(	m_Options);
	skin.SetButtonSkin(	m_PythonScripts);
	skin.SetButtonSkin(	m_MonsterShow);
	skin.SetButtonSkin(	m_MapShow);
	skin.SetButtonSkin(	m_ItemConfig);
	skin.SetButtonSkin(	m_InjectMC);
	skin.SetButtonSkin(	m_CharInfo);
	skin.SetButtonSkin(	m_xray);
	skin.SetButtonSkin(	m_autoAttack);
	skin.SetButtonSkin(	m_login);
	skin.SetButtonSkin(	m_ammoRestack);
	skin.SetButtonSkin(	m_autoAim);
	skin.SetButtonSkin(	m_autoFish);
	skin.SetButtonSkin(	m_autoGo);
	skin.SetButtonSkin(	m_autoLooter);
	skin.SetButtonSkin(	m_autoRespond);
	skin.SetButtonSkin(	m_autoUh);
	skin.SetButtonSkin(	m_fluidDrinker);
	skin.SetButtonSkin(	m_runeMaker);
	skin.SetButtonSkin(	m_spellCaster);
	skin.SetButtonSkin(	m_fps);
	skin.SetButtonSkin(	m_antilogout);
	skin.SetButtonSkin(	m_mapHack);
	skin.SetButtonSkin(	m_creatureInfo);
	skin.SetButtonSkin(	m_save);
	skin.SetButtonSkin(	m_load);
	skin.SetButtonSkin(	m_eater);
	skin.SetButtonSkin(	m_tradeMon);
	skin.SetButtonSkin(	m_grouping);
	skin.SetButtonSkin(	m_light);
	skin.SetButtonSkin(	m_banker);
	skin.SetButtonSkin(	m_sorter);
	skin.SetButtonSkin(	m_seller);

	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
		



	CCharDialog *charDialog = new CCharDialog();
	m_processId=charDialog->DoModal();
	globalProcessId=m_processId;
	delete charDialog;
	if (m_processId==-1)
		ExitProcess(0);

	unsigned char buf[5];
	int versionOk=0;
	CTibiaItemProxy itemProxy;
	int m_memAddressRevealCName1=itemProxy.getValueForConst("addrFunRevealCName1");
	buf[0]=buf[1]=0;
	CMemUtil::GetMemRange(m_processId,m_memAddressRevealCName1,m_memAddressRevealCName1+2,(char *)buf);	
	if (buf[0]==0x90&&buf[1]==0x90) versionOk=1;
	if (buf[0]==0x75&&buf[1]==0x40) versionOk=1;
		
	if (!versionOk)
	{		
		char outBuf[32];

		sprintf(outBuf,"tibia.exe version mismatch! Terminating Tibia Auto! (%d  %d)", buf[0], buf[1]);
		AfxMessageBox(outBuf);
		ExitProcess(0);
	}
	

	CMemUtil::setGlobalProcessId(m_processId);
	
				
	InitialiseIPC();

	CMemReaderProxy reader;
	reader.setProcessId(m_processId);
	CPackSenderProxy sender;
	sender.setPipeHandle(hPipe);

	// shutdownCounter is anti-hack protection
	shutdownCounter=rand()%100;
#ifndef _DEBUG
	CEnterCode *enterCode = new CEnterCode(this);
	int res=enterCode->DoModal();
	delete enterCode;

	if (res!=IDOK) ExitProcess(0);
#else
	shutdownCounter=-rand();
#endif


	m_loadedModules = new CLoadedModules();
	m_loadedModules->Create(IDD_LOADED_MODULES);

	m_pythonScriptsDialog = new CPythonScriptsDialog();
	m_pythonScriptsDialog->Create(IDD_PYTHON_SCRIPTS);
	
	m_moduleLooter = new CModuleProxy("mod_looter",0);		
	m_moduleLight = new CModuleProxy("mod_light",0);	
	m_modulePlayerInfo = new CModuleProxy("mod_playerinfo",0);
	m_modulePlayerInfo->start();
	m_moduleUH = new CModuleProxy("mod_uh",0);
	m_moduleGrouping = new CModuleProxy("mod_grouping",0);
	m_moduleRestack = new CModuleProxy("mod_restack",0);
	m_moduleFluid = new CModuleProxy("mod_fluid",0);
	m_moduleAim = new CModuleProxy("mod_aim",0);
	m_moduleFisher = new CModuleProxy("mod_fisher",0);
	m_moduleSpellCaster = new CModuleProxy("mod_spellcaster",0);
	m_moduleTradeMon = new CModuleProxy("mod_trademon",0);
	m_moduleMemDebug = new CModuleProxy("mod_memdebug",0);
	m_moduleMapShow = new CModuleProxy("mod_showmap",0);
	m_moduleMonsterShow = new CModuleProxy("mod_monstershow",0);
	m_moduleItemConfig = new CModuleProxy("mod_itemconfig",0);
	m_moduleAutoGo = new CModuleProxy("mod_autogo",0);
	m_moduleAutoAttack = new CModuleProxy("mod_cavebot",0);
	m_moduleRuneMaker = new CModuleProxy("mod_runemaker",0);
	m_moduleEater = new CModuleProxy("mod_eater",0);
	m_moduleCreatureInfo = new CModuleProxy("mod_creatureinfo",0);
	m_moduleResponder = new CModuleProxy("mod_responder",0);
	m_moduleMapHack = new CModuleProxy("mod_maphack",0);
	m_moduleTeam = new CModuleProxy("mod_team",0);
	m_moduleAntylogout = new CModuleProxy("mod_antylogout",0);
	m_moduleFps = new CModuleProxy("mod_fps",0);
	m_moduleLogin = new CModuleProxy("mod_login",0);
	m_moduleXRay = new CModuleProxy("mod_xray",0);
	m_moduleSorter = new CModuleProxy("mod_sorter",0);
	m_moduleBanker = new CModuleProxy("mod_banker",0);
	m_moduleSeller = new CModuleProxy("mod_seller",0);
	refreshToolInfo();

	if (!CModuleUtil::getTASetting("SeenMapMessage")){
		AfxMessageBox("Please consider submitting your map files to TibiaAuto.net to help us with the new system in place to create a complete Tibia map. \n\nYou will only receive this reminder once. Thank you for any contribution you make.");
		CModuleUtil::setTASetting("SeenMapMessage",1);
		OnOptions();
	}
	if(CModuleUtil::getTASetting("GatherBotStats")){
		COptionsDialog dlg;
		//dlg.Create(IDD_OPTIONS);
		dlg.sendStats();
	}

	SetTimer(1001,100,NULL);
	SetTimer(1002,100,NULL);


	// this is needed to force loading tibiaauto_util.dll	
	CTibiaCharacter *self=reader.readSelfCharacter();
	delete self;


	
	CPythonEngine pythonEngine;
	pythonEngine.init();

	int ffBoxDisplay=1;

	char ffCheckString[1024];
	unsigned long ffCheckLen=1023;
	ffCheckString[0]='\0';
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey))
	{
		RegQueryValueEx(hkey,TEXT("FFcheck"),NULL,NULL,(unsigned char *)ffCheckString,&ffCheckLen);
		
		if (strlen(ffCheckString))
		{
			// found
			if (time(NULL)-atoi(ffCheckString)<60*60*24*3)
			{
				ffBoxDisplay=0;
			}
		}	
		char buf[128];
		sprintf(buf,"%d",time(NULL));
		RegSetValueEx(hkey,TEXT("FFcheck"),0,REG_SZ,(const unsigned char *)buf,strlen(buf)+1);
		RegCloseKey(hkey);	
	}

	if (ffBoxDisplay)
	{
		// now check for firefox	
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Mozilla\\Mozilla Firefox\\",0,KEY_ALL_ACCESS,&hkey))
		{
			RegCloseKey(hkey);
			ffBoxDisplay=0;
		}
	}
	if (ffBoxDisplay)
	{
		// now check for firefox	
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Mozilla\\Mozilla Firefox 2.0\\",0,KEY_ALL_ACCESS,&hkey))
		{
			RegCloseKey(hkey);
			ffBoxDisplay=0;
		}
	}
	
	if (ffBoxDisplay)
	{
		if (AfxMessageBox("Tibia Auto has detected that you are not using Mozilla Firefox. It is recommend that you install it. Do you want to proceed with installation?",MB_YESNO)==IDYES)
		{
			::ShellExecute(NULL, NULL, "http://tibiaauto.net/firefox.html", NULL, NULL, SW_SHOWNORMAL);			
		}		
	}
	
	if (CModuleUtil::getTASetting("LoadScriptOnStartup")){
		char fName[128];
		char *charName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);		
		sprintf(fName,"tibiaAuto.cfg.%s.xml",charName);
		free(charName);
		char pathbuf[2048];
		CModuleUtil::getInstallPath(pathbuf);
		CString pathName=pathbuf;
		if (pathName.Right(1)!="\\") pathName+="\\";
		pathName+=fName;
		loadConfig(pathName);
	}

	refreshAds();

	// set shell tray
	NOTIFYICONDATA data;
	data.cbSize=sizeof(NOTIFYICONDATA);
	data.hWnd=GetSafeHwnd();
	data.uID=1;		
	data.hIcon=AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	char *loggedCharName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);
	snprintf(data.szTip,60,"%s",loggedCharName);
	free(loggedCharName);
	data.uCallbackMessage=WM_APP+1;
	data.uFlags=NIF_ICON|NIF_TIP|NIF_MESSAGE;
	Shell_NotifyIcon(NIM_ADD,&data);
	
	
	SetTimer(1003,1000*60*15,NULL); // once every 15 minutes refresh ads	
	SetTimer(1004,1000*60*5,NULL); // once every 5 minutes refresh ads	
	if (CModuleUtil::getTASetting("GatherBotStats"))
		SetTimer(1005,5000,NULL);//every 5 seconds check and record module stats



		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTibiaautoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTibiaautoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CTibiaautoDlg::OnChangeEditAddress()
{

}

void CTibiaautoDlg::OnChangeEditValue()
{
	
	
}


void CTibiaautoDlg::OnTimer(UINT nIDEvent) 
{	
	CMemReaderProxy reader;
	
	if (nIDEvent==1001)
	{
		KillTimer(1001);
		refreshToolInfo();
		char buf[1024];
		char *loggedCharName = reader.GetLoggedChar(CMemUtil::m_globalProcessId);
		sprintf(buf,"Logged as: %s",loggedCharName);
		free(loggedCharName);
		CString currentDisplay;
		m_loginName.GetWindowText(currentDisplay);
		if (currentDisplay != buf)
			m_loginName.SetWindowText(buf);
		shutdownCounter--;						
		if (shutdownCounter<-100000) shutdownCounter=-123;
		if (shutdownCounter==0)
		{
			// ups ops some's hacking TA :/
			shutdownCounter++;
			delete this;
		}
		SetTimer(1001,150,NULL);
	}	
	if (nIDEvent==1002)
	{
		KillTimer(1002);

		CPythonEngine::periodicalTick();
		CPythonEngine::backpipeMsgTick();
		CPythonEngine::backpipeTamsgTick();

		SetTimer(1002,25,NULL);
	}
	if (nIDEvent==1003)
	{
		refreshAds();
	}
	if (nIDEvent==1004)
	{		
		reportUsage();
	}
	if (nIDEvent==1005)
	{		
		static unsigned int enabledModules=0xffffffff;
		unsigned int modCheck=0;
		for (int i=0;i<CModuleProxy::allModulesCount && i<32;i++){
			modCheck|=CModuleProxy::allModules[i]->isStarted()<<i;
		}
		if (modCheck!=enabledModules){
			//char buf[111];
			//sprintf(buf,"%x",enabledModules);
			//AfxMessageBox(buf);
			enabledModules=modCheck;
			char path[1024];
			CModuleUtil::getInstallPath(path);
			char pathBuf[2048];
			sprintf(pathBuf,"%s\\tascripts\\module %d statistics.txt",path,reader.getProcessId());
			std::ofstream fout(pathBuf,std::ios::out|std::ios::app|std::ios::binary);
			int tm=time(NULL);
			fout.write((char*)&tm,4);
			fout.write((char*)&enabledModules,4);
			fout.close();
		}
	}


	
	
	CDialog::OnTimer(nIDEvent);
}



void CTibiaautoDlg::OnLight() 
{	
	m_moduleLight->showConfigDialog();	
}


void CTibiaautoDlg::InitialiseIPC()
{
	char buf[128];
	char lpszPipename[128];
	sprintf(lpszPipename,"\\\\.\\pipe\\tibiaAutoPipe-%d",m_processId);
	

	 hPipe = CreateNamedPipe(
          lpszPipename,             // pipe name 
          PIPE_ACCESS_DUPLEX,       // read/write access 
          PIPE_TYPE_MESSAGE |       // message type pipe 
          PIPE_READMODE_MESSAGE |   // message-read mode 
          PIPE_WAIT,                // blocking mode 
          PIPE_UNLIMITED_INSTANCES, // max. instances  
          163840,                  // output buffer size 
          163840,                  // input buffer size 
          1000,                        // client time-out 
          NULL);                    // no security attribute 


	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		sprintf(buf,"Invalid pipe handle: %d",GetLastError());
        AfxMessageBox(buf);
	}
	

	
	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS,true,m_processId);
	
	char *injectDll="tibiaautoinject2.dll";				
	
	if (!DetourContinueProcessWithDll(procHandle, injectDll)) {
		sprintf(buf,"dll injection failed: %d",GetLastError());
		AfxMessageBox(buf);		
		ExitProcess(1);
	}	
	
	//injectDll="tibiaauto_develtest.dll";			
	
	//DetourContinueProcessWithDll(procHandle, injectDll);
	
	
	CloseHandle(procHandle);
	
	BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? 
TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
	

	if (!fConnected)
	{
		sprintf(buf,"client not connected via pipe: %d",GetLastError());
		AfxMessageBox(buf);
		ExitProcess(1);
	}

	m_lightPower=-1;

	// send my pid to the dll
	int myProcessId=GetCurrentProcessId();
	struct ipcMessage mess;
	mess.messageType=4;
	memcpy(mess.payload,&myProcessId,sizeof(int));
	mess.send();

	// wait for incoming IPC connection from the dll
	
	CIPCBackPipeProxy backPipe;
	backPipe.InitialiseIPC();
	
	// now hook keys	
	HINSTANCE hinstDLL=NULL;
	hhookKeyb=NULL;
	
	hinstDLL = LoadLibrary((LPCTSTR) "tibiaautoinject3.dll");
	
	if (hinstDLL)
	{				
		typedef LRESULT (CALLBACK *KeyboardProc_fun)(int nCode, WPARAM wParam, LPARAM lParam);
		static KeyboardProc_fun fun=(KeyboardProc_fun)GetProcAddress(hinstDLL,"KeyboardProc");				
		if (fun)
		{									
			
			HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
			THREADENTRY32 te32;
			
			// Take a snapshot of all running threads
			hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
			if( hThreadSnap == INVALID_HANDLE_VALUE )
				goto noKeybHook;
			
			// Fill in the size of the structure before using it.
			te32.dwSize = sizeof(THREADENTRY32 );
			
			// Retrieve information about the first thread,
			// and exit if unsuccessful
			if( !Thread32First( hThreadSnap, &te32 ) )
			{		
				CloseHandle( hThreadSnap ); // Must clean up the snapshot object!
				goto noKeybHook;
			}
			
			// Now walk the thread list of the system,
			// and display information about each thread
			// associated with the specified process				
			do
			{
				
				if( te32.th32OwnerProcessID == m_processId )
				{								
					hhookKeyb=SetWindowsHookEx(WH_KEYBOARD,fun,hinstDLL,te32.th32ThreadID);
					break;
				}
			} while( Thread32Next(hThreadSnap, &te32 ) );								
			
			// Don't forget to clean up the snapshot object.
			CloseHandle( hThreadSnap );				
			// initialise hooks								
		}						
	}	
noKeybHook:
	{
	}
	
	
}



void CTibiaautoDlg::OnToolRunemaker() 
{	
	m_moduleRuneMaker->showConfigDialog();
}

void CTibiaautoDlg::OnToolCharinfo() 
{	
	m_modulePlayerInfo->showConfigDialog();
}

void CTibiaautoDlg::OnToolMonsershow() 
{
	m_moduleMonsterShow->showConfigDialog();
}

void CTibiaautoDlg::OnToolAutogo() 
{
	m_moduleAutoGo->showConfigDialog();
}

void CTibiaautoDlg::OnToolAutofish() 
{
	m_moduleFisher->showConfigDialog();
}

void CTibiaautoDlg::refreshToolInfo()
{	
	if (m_runeMaker.GetCheck() != m_moduleRuneMaker->isStarted())
		m_runeMaker.SetCheck(m_moduleRuneMaker->isStarted());
	if (m_autoGo.GetCheck() != m_moduleAutoGo->isStarted())
		m_autoGo.SetCheck(m_moduleAutoGo->isStarted());
	if (m_autoFish.GetCheck() != m_moduleFisher->isStarted())
		m_autoFish.SetCheck(m_moduleFisher->isStarted());
	if (m_autoAttack.GetCheck() != m_moduleAutoAttack->isStarted())
		m_autoAttack.SetCheck(m_moduleAutoAttack->isStarted());
	if (m_autoUh.GetCheck() != m_moduleUH->isStarted())
		m_autoUh.SetCheck(m_moduleUH->isStarted());
	if (m_spellCaster.GetCheck() != m_moduleSpellCaster->isStarted())
		m_spellCaster.SetCheck(m_moduleSpellCaster->isStarted());
	if (m_autoAim.GetCheck() != m_moduleAim->isStarted())
		m_autoAim.SetCheck(m_moduleAim->isStarted());
	if (m_fluidDrinker.GetCheck() != m_moduleFluid->isStarted())
		m_fluidDrinker.SetCheck(m_moduleFluid->isStarted());
	if (m_ammoRestack.GetCheck() != m_moduleRestack->isStarted())
		m_ammoRestack.SetCheck(m_moduleRestack->isStarted());
	if (m_autoLooter.GetCheck() != m_moduleLooter->isStarted())
		m_autoLooter.SetCheck(m_moduleLooter->isStarted());
	if (m_autoRespond.GetCheck() != m_moduleResponder->isStarted())
		m_autoRespond.SetCheck(m_moduleResponder->isStarted());
	if (m_mapHack.GetCheck() != m_moduleMapHack->isStarted())
		m_mapHack.SetCheck(m_moduleMapHack->isStarted());
	if (m_light.GetCheck() != m_moduleLight->isStarted())
		m_light.SetCheck(m_moduleLight->isStarted());
	if (m_grouping.GetCheck() != m_moduleGrouping->isStarted())
		m_grouping.SetCheck(m_moduleGrouping->isStarted());
	if (m_tradeMon.GetCheck() != m_moduleTradeMon->isStarted())
		m_tradeMon.SetCheck(m_moduleTradeMon->isStarted());
	if (m_eater.GetCheck() != m_moduleEater->isStarted())
		m_eater.SetCheck(m_moduleEater->isStarted());
	if (m_creatureInfo.GetCheck() != m_moduleCreatureInfo->isStarted())
		m_creatureInfo.SetCheck(m_moduleCreatureInfo->isStarted());
	if (m_antilogout.GetCheck() != m_moduleAntylogout->isStarted())
		m_antilogout.SetCheck(m_moduleAntylogout->isStarted());
	if (m_fps.GetCheck() != m_moduleFps->isStarted())
		m_fps.SetCheck(m_moduleFps->isStarted());
	if (m_login.GetCheck() != m_moduleLogin->isStarted())
		m_login.SetCheck(m_moduleLogin->isStarted());
	if (m_xray.GetCheck() != m_moduleXRay->isStarted())
		m_xray.SetCheck(m_moduleXRay->isStarted());
	if (m_sorter.GetCheck() != m_moduleSorter->isStarted())
		m_sorter.SetCheck(m_moduleSorter->isStarted());
	if (m_banker.GetCheck() != m_moduleBanker->isStarted())
		m_banker.SetCheck(m_moduleBanker->isStarted());
	if (m_seller.GetCheck() != m_moduleSeller->isStarted())
		m_seller.SetCheck(m_moduleSeller->isStarted());
}

void CTibiaautoDlg::OnSave()
{
	CMemReaderProxy reader;		
	char fName[128];
	char *loggedCharName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);
	FILE *f=NULL;

	char szFilters[]=
      "Tibia Auto config (*.xml)|*.xml|All Files (*.*)|*.*||";

	sprintf(fName,"tibiaAuto.cfg.%s.xml",loggedCharName);
	free(loggedCharName);
	CFileDialog fd(false,"",fName,OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	
	if (fd.DoModal()==IDOK)
	{
		CString pathName = fd.GetPathName();
		f=fopen(pathName.GetBuffer(200),"wb");
	}	
		
	if (f)
	{				
		fprintf(f,"<configfile>\n");
		int modNr;
		int someModuleRunning=0;
		for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
		{
			CModuleProxy * module = CModuleProxy::allModules[modNr];
			if (module->isStarted())
			{
				someModuleRunning=1;
				break;
			}			
		}
		//Save a list of started scripts so they can be started on load
		if (someModuleRunning){
			if (AfxMessageBox("Some modules are running.\nWould you like to save a list of running modules?",MB_YESNO)==IDYES){
				fprintf(f,"<startedModules>\n");
				for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
				{
					CModuleProxy * module = CModuleProxy::allModules[modNr];
					if (module->isStarted())
					{
						fprintf(f,"<module name = \"%s\"/>",module->getModuleName());
					}
				}
				fprintf(f,"</startedModules>\n");
				fprintf(f,"<startedScripts>\n");
				for (modNr=0;modNr<CPythonScript::getScriptCount();modNr++)
				{
					CPythonScript * script = CPythonScript::getScriptByNr(modNr);
					if (script->isEnabled())
					{
						fprintf(f,"<module name = \"%s\"/>",script->getName());
					}
				}
				fprintf(f,"</startedScripts>\n");
			}
		}

		// save "normal" modules
		for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
		{
			CModuleProxy * module = CModuleProxy::allModules[modNr];
			char* nme=module->getName();
			if (module->isLoaded())
			{
				CConfigCreatorUtil configCreator;
				if (!module->isStarted())
				{
					module->controlsToConfig();
				}
				DOMNode *moduleConfig = configCreator.getEmptyConfigForModule(module->getModuleName());

				int paramNr;
				for (paramNr=0;module->getConfigParamName(paramNr);paramNr++)
				{									
					char *paramName=module->getConfigParamName(paramNr);
					if (module->isMultiParam(paramName))
					{
						module->resetMultiParamAccess(paramName);
						char *paramValue=module->saveConfigParam(paramName);
						while (strlen(paramValue))
						{													 						
							char realParamName[1024];
							sprintf(realParamName,"config/%s",paramName);
							configCreator.addParamToConfig(moduleConfig,realParamName,paramValue);
							paramValue=module->saveConfigParam(paramName);
						};
					} else {
						char *paramValue=module->saveConfigParam(paramName);
						
						if (!paramValue)
							break;
						char realParamName[1024];
						sprintf(realParamName,"config/%s",paramName);
						configCreator.addParamToConfig(moduleConfig,realParamName,paramValue);
					}
				}
				configCreator.serializeConfigForModule(moduleConfig,f);
				configCreator.releaseConfig(moduleConfig);
			}
		} // saving normal modules config

		// python scripts
		int scriptNr=0;		
		for (;;)
		{		
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr++);
			if (!pythonScript) break;
			
			char scriptModuleName[128];
			snprintf(scriptModuleName,127,"Script: %s",pythonScript->getName());
			if (!strcmp("Script: Stub",scriptModuleName))
				int a=1;
			CConfigCreatorUtil configCreator;			
			DOMNode *moduleConfig;
			try {
				moduleConfig = configCreator.getEmptyConfigForModule(scriptModuleName);
			} catch (...){
				char buf [1024];
				sprintf(buf,"The python module name \"%s\" is invalid", pythonScript->getName());
				AfxMessageBox(buf);
				continue;
			}
			
			int paramNr=0;
			for (;;)
			{	
				struct paramType *param = pythonScript->getParamDef(paramNr++);
				if (!param) break;
								
				char realParamName[1024];
				sprintf(realParamName,"config/%s",param->name);
				try {
					configCreator.addParamToConfig(moduleConfig,realParamName,param->value);
				} catch (...){
					char buf [1024];
					sprintf(buf,"Parameter name \"%s\" for the python module \"%s\" is invalid", param->name,pythonScript->getName());
					AfxMessageBox(buf);
					continue;
				}
				
			}
			configCreator.serializeConfigForModule(moduleConfig,f);
			configCreator.releaseConfig(moduleConfig);
		} // saving python scripts config
		

		fprintf(f,"\n</configfile>\n");
		fclose(f);
	}
}

struct loadThreadParam
{
	CConfigDialogStatus *configDialogStatus;
	char fName[1024];
};

DWORD WINAPI loadThread( LPVOID lpParam )
{
	XercesDOMParser *parser;
	parser = new XercesDOMParser();	
	CConfigDialogStatus * m_configDialogStatus = ((struct loadThreadParam *)lpParam)->configDialogStatus;	
	

	CMemReaderProxy reader;
	CConfigCreatorUtil configCreator;
	
	
	char logBuf[16384];		
	
	int modNr;
	int *restartedModulesTab = new int[CModuleProxy::allModulesCount];
	for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
	{
		CModuleProxy * module = CModuleProxy::allModules[modNr];			
		restartedModulesTab[modNr]=module->isStarted();
	}
	
	
	char *loggedCharName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);
	sprintf(logBuf,"Loading character '%s' ... started",loggedCharName);
	m_configDialogStatus->msgAddToLog(logBuf);
	
	for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
	{
		CModuleProxy * module = CModuleProxy::allModules[modNr];
		if (restartedModulesTab[modNr])
		{
			sprintf(logBuf,"Stopping module %s ...",module->getModuleName());
			m_configDialogStatus->msgAddToLog(logBuf);
			module->stop();
		}
	}
				
	
	parser->parse(((struct loadThreadParam *)lpParam)->fName);		
	
	DOMDocument *doc = parser->getDocument();
	DOMElement *root = doc->getDocumentElement();
	
	for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
	{
		CModuleProxy * module = CModuleProxy::allModules[modNr];
		if (module->isLoaded())
		{
			char *moduleName = module->getModuleName();
			DOMNode *moduleConfig = configCreator.getConfigForModule(root,moduleName);
			if (moduleConfig)
			{
				sprintf(logBuf,"Loading config for module %s ...",module->getModuleName());
				m_configDialogStatus->msgAddToLog(logBuf);
				configCreator.parseConfigFromNode(module,moduleConfig,"");
				module->configToControls();
			}				
		}
	}	

	int scriptNr=0;
	for (;;)
	{
		CConfigCreatorUtil configCreator;
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr++);
		if (!pythonScript) break;

		char scriptConfigName[128];
		snprintf(scriptConfigName,127,"Script: %s",pythonScript->getName());
		DOMNode *scriptConfig = configCreator.getConfigForModule(root,scriptConfigName);
		if (scriptConfig)
		{
			sprintf(logBuf,"Loading config for script %s",pythonScript->getName());
			m_configDialogStatus->msgAddToLog(logBuf);
			configCreator.parseConfigFromNode(pythonScript,scriptConfig,"");
		}
	}
	int otherModulesLoaded=0;
	DOMNodeList *childNodes = root->getElementsByTagName(X("startedModules"));
	DOMNode *moduleConfig;
	if (childNodes->getLength() && (moduleConfig = childNodes->item(0)))
	{
		if (AfxMessageBox("This config contains saved started modules. Do you wish to start them?",MB_YESNO)==IDYES)
		{
			DOMNode *subNode = moduleConfig->getFirstChild();
			if (subNode)
			{
				do {
					if (subNode->getNodeType()==DOMNode::ELEMENT_NODE)
					{				
						char subNodeName[1024];
						wcstombs(subNodeName,subNode->getNodeName(),1024);

						DOMNode *attrNode=subNode->getAttributes()->getNamedItem(_L("name"));
						if (!strcmp(subNodeName,"module")&&attrNode)
						{
							char nodeValue[1024];
							wcstombs(nodeValue,attrNode->getNodeValue(),1024);
							for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
							{
								CModuleProxy * module = CModuleProxy::allModules[modNr];
								if (!strcmp(nodeValue,module->getModuleName()))
								{
									sprintf(logBuf,"Starting module %s ...",module->getModuleName());
									m_configDialogStatus->msgAddToLog(logBuf);
									module->start();
								}
							}
						}
					}
				} while ((subNode=subNode->getNextSibling())!=NULL);
			}
			childNodes = root->getElementsByTagName(X("startedScripts"));
			DOMNode *scriptConfig;
			if (childNodes->getLength() && (scriptConfig = childNodes->item(0)))
			{
				DOMNode *subNode = scriptConfig->getFirstChild();
				if (subNode)
				{
					do {
						if (subNode->getNodeType()==DOMNode::ELEMENT_NODE)
						{				
							char subNodeName[1024];
							wcstombs(subNodeName,subNode->getNodeName(),1024);

							DOMNode *attrNode=subNode->getAttributes()->getNamedItem(_L("name"));
								
							if (!strcmp(subNodeName,"module")&&attrNode)
							{
								char nodeValue[1024];
								wcstombs(nodeValue,attrNode->getNodeValue(),1024);
								for (modNr=0;modNr<CPythonScript::getScriptCount();modNr++)
								{
									CPythonScript * script = CPythonScript::getScriptByNr(modNr);
									if (!strcmp(nodeValue,script->getName()))
									{
										sprintf(logBuf,"Starting script %s ...",script->getName());
										m_configDialogStatus->msgAddToLog(logBuf);
										script->setEnabled(1);
									}
								}
							}		
						}
					} while ((subNode=subNode->getNextSibling())!=NULL);
				}
			}
			otherModulesLoaded=1;
		}
	}
	if (!otherModulesLoaded){	
		for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
		{
			CModuleProxy * module = CModuleProxy::allModules[modNr];
			if (restartedModulesTab[modNr])
			{
				sprintf(logBuf,"Starting module %s ...",module->getModuleName());
				m_configDialogStatus->msgAddToLog(logBuf);
				module->start();				
			}
		}	
	}
	doc->release();
	
	sprintf(logBuf,"Loading character '%s' finished.",loggedCharName);
	free(loggedCharName);
	m_configDialogStatus->msgAddToLog(logBuf);
	
	delete []restartedModulesTab;		
	
	free ((struct loadThreadParam *)lpParam);
	return 0;
}
	
void CTibiaautoDlg::loadConfig(CString pathName){

	FILE *f=fopen(pathName.GetBuffer(1024),"rb");	
	
	if (f)
	{
		fclose(f);
		
		CConfigDialogStatus * configDialogStatus = new CConfigDialogStatus();	
		configDialogStatus->Create(IDD_CONFIGSTATUS);
		configDialogStatus->ShowWindow(SW_SHOW);
		
		struct loadThreadParam *ltp = new struct loadThreadParam();;
		strncpy(ltp->fName,pathName.GetBuffer(1023),1023);
		ltp->configDialogStatus=configDialogStatus;
		DWORD threadId;
		::CreateThread(NULL,0,loadThread,ltp,0,&threadId);				
		
	}
}

void CTibiaautoDlg::OnLoad()
{	
	int someModuleRunning=0;
	int modNr;
	for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
	{
		CModuleProxy * module = CModuleProxy::allModules[modNr];
		if (module->isStarted())
		{
			someModuleRunning=1;
		}			
	}
	if (someModuleRunning)
	{
		if (AfxMessageBox("Some modules are running. Are you sure you want to load new configuration?",MB_YESNO)==IDNO)
		{				
			return;
		}
	}
	CMemReaderProxy reader;
	
	char fName[128];
	char *loggedCharName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);		
	
	
	char szFilters[]=
		"Tibia Auto config (*.xml)|*.xml|All Files (*.*)|*.*||";
	
	sprintf(fName,"tibiaAuto.cfg.%s.xml",loggedCharName);
	free(loggedCharName);
	CFileDialog fd(true,"",fName,OFN_FILEMUSTEXIST, szFilters, NULL);
	if (fd.DoModal()==IDOK){
		CString pathName;
		pathName = fd.GetPathName();
		loadConfig(pathName);
	}
}	

void CTibiaautoDlg::OnToolAutogroup() 
{
	if (m_grouping.GetCheck())
	{
		m_moduleGrouping->start();
	} else {
		m_moduleGrouping->stop();
	}	
}

void CTibiaautoDlg::OnToolAutouh() 
{
	m_moduleUH->showConfigDialog();
}

void CTibiaautoDlg::OnToolMapshow() 
{
	m_moduleMapShow->showConfigDialog();
}

void CTibiaautoDlg::OnToolAutoattack() 
{
	m_moduleAutoAttack->showConfigDialog();
}



void CTibiaautoDlg::OnToolSpellcaster() 
{
	m_moduleSpellCaster->showConfigDialog();
}

void CTibiaautoDlg::OnExit() 
{
	DisconnectNamedPipe(hPipe);
	NOTIFYICONDATA data;
	data.cbSize=sizeof(NOTIFYICONDATA);
	data.hWnd=GetSafeHwnd();
	data.uID=1;				
	data.uFlags=0;
	Shell_NotifyIcon(NIM_DELETE,&data);

	CTAMiniMapProxy taMiniMap;
	taMiniMap.unloadMiniMaps();

	CTibiaItemProxy tibiaItem;
	tibiaItem.cleanup();

	CTibiaMapProxy tibiaMap;
	tibiaMap.clear();

	CMemReaderProxy reader;
	reader.cleanupTibiaTiles();

	delete m_loadedModules;	
	delete m_pythonScriptsDialog;
	m_moduleLooter->stop();
	delete m_moduleLooter;
	delete m_moduleLight;
	delete m_modulePlayerInfo;
	delete m_moduleUH;
	delete m_moduleGrouping;
	delete m_moduleRestack;
	delete m_moduleFluid;
	delete m_moduleAim;
	delete m_moduleFisher;
	delete m_moduleSpellCaster;
	delete m_moduleTradeMon;
	delete m_moduleMemDebug;
	delete m_moduleMapShow;
	delete m_moduleMonsterShow;
	delete m_moduleItemConfig;
	delete m_moduleAutoGo;
	delete m_moduleAutoAttack;
	delete m_moduleRuneMaker;
	delete m_moduleEater;
	delete m_moduleCreatureInfo;
	delete m_moduleResponder;
	delete m_moduleMapHack;
	delete m_moduleTeam;
	delete m_moduleAntylogout;
	delete m_moduleFps;
	delete m_moduleLogin;
	delete m_moduleXRay;
	delete m_moduleBanker;
	delete m_moduleSorter;
	delete m_moduleSeller;
	
	if (hhookKeyb)
	{
		UnhookWindowsHookEx(hhookKeyb);
	}	

	for (int scriptNr=0;;scriptNr++){
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
		if (!pythonScript) break;
		delete pythonScript;
	}


	ExitProcess(0);	
}

void CTibiaautoDlg::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void CTibiaautoDlg::OnToolAutoaim() 
{
	m_moduleAim->showConfigDialog();	
}

void CTibiaautoDlg::OnMemdebug() 
{
	m_moduleMemDebug->showConfigDialog();
}

BOOL CTibiaautoDlg::PreTranslateMessage(MSG* pMsg) 
{
	int eventStop=0;
	if (pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_ESCAPE)
	{
		ShowWindow(SW_HIDE);
		eventStop=1;
	} 
	if (pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
	{
		eventStop=1;
	} 
	if (pMsg->message==WM_CHAR) eventStop=1;
	if (!eventStop) {
		return CDialog::PreTranslateMessage(pMsg);
	} else {
		return 0;
	}
}

void CTibiaautoDlg::OnFinalRelease() 
{
	CDialog::OnFinalRelease();

}

void CTibiaautoDlg::OnToolFluiddrinker() 
{
	m_moduleFluid->showConfigDialog();
}

void CTibiaautoDlg::OnToolTrademon() 
{
	m_moduleTradeMon->showConfigDialog();
}

void CTibiaautoDlg::OnToolInjectmc() 
{
	CTibiaItemProxy itemProxy;
	char szFilters[]=
      "Tibia client (tibia.exe)|tibia.exe|All Files (*.*)|*.*||";

	CFileDialog fd(true,"tibia","tibia.exe",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
	if (fd.DoModal()==IDOK)
	{
		CString pathName = fd.GetPathName();         
						
		FILE *f = fopen(pathName,"r+b");
		if (f)
		{		
			if (!fseek(f,itemProxy.getValueForConst("addrMCInject"),SEEK_SET))
			{
				unsigned char val=0xff;
				fwrite(&val,1,1,f);
				AfxMessageBox("Tibia client has been successfully modified to MC mode!");
			} else {
				AfxMessageBox("Unable to seek correct position in file!");
			}
			fclose(f);
		} else {
			AfxMessageBox("Unable to open file for writting!");
		}
	}
}

void CTibiaautoDlg::OnToolItemconfig() 
{
	m_moduleItemConfig->showConfigDialog();
}

void CTibiaautoDlg::OnToolAmmorestack() 
{
	m_moduleRestack->showConfigDialog();
}

void CTibiaautoDlg::OnToolAutorespond() 
{
	m_moduleResponder->showConfigDialog();
}

void CTibiaautoDlg::OnLoadedModules() 
{
	m_loadedModules->ShowWindow(SW_SHOW);	
}

void CTibiaautoDlg::OnToolAutolooter() 
{	
	m_moduleLooter->showConfigDialog();
}

void CTibiaautoDlg::OnToolEater() 
{
	if (m_eater.GetCheck())
	{
		m_moduleEater->start();
	} else {
		m_moduleEater->stop();
	}	
}

//void CTibiaautoDlg::OnDonation() 
//{	
//	CDonationDialog donDialog;
//	donDialog.DoModal();							
//}

void CTibiaautoDlg::OnToolCreatureinfo() 
{
	m_moduleCreatureInfo->showConfigDialog();	
}

void CTibiaautoDlg::OnToolMaphack() 
{
	m_moduleMapHack->showConfigDialog();	
}

void CTibiaautoDlg::OnToolTeam() 
{
	m_moduleTeam->showConfigDialog();
}

void CTibiaautoDlg::OnToolAntilog() 
{	
	if (m_antilogout.GetCheck())
	{
		m_moduleAntylogout->start();
	} else {
		m_moduleAntylogout->stop();
	}	
}

void CTibiaautoDlg::OnFps() 
{
	m_moduleFps->showConfigDialog();	
}

void CTibiaautoDlg::passSecurityInfo(int value)
{
	if (!value)
	{
		delete this;
	}
}

void CTibiaautoDlg::OnPythonScripts() 
{
	m_pythonScriptsDialog->ShowWindow(SW_SHOW);	
}

void CTibiaautoDlg::OnOptions() 
{
	COptionsDialog dlg;
	dlg.DoModal();
}

void CTibiaautoDlg::refreshAds()
{
	m_browserAds.Navigate("http://ads.tibiaauto.net/showad.php?version=1.19.1",NULL,NULL,NULL,NULL);
}

void CTibiaautoDlg::OnToolLogin() 
{
	m_moduleLogin->showConfigDialog();		
}

void CTibiaautoDlg::OnToolSorter() 
{
	m_moduleSorter->showConfigDialog();		
}

void CTibiaautoDlg::OnToolBanker() 
{
	m_moduleBanker->showConfigDialog();		
}

void CTibiaautoDlg::OnToolSeller() 
{
	m_moduleSeller->showConfigDialog();		
}

void CTibiaautoDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);		
}

BOOL CTibiaautoDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{		
	return CDialog::OnCommand(wParam, lParam);
}

void CTibiaautoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
}

LRESULT CTibiaautoDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_APP+1&&lParam==WM_LBUTTONDOWN)
	{
		if (IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
		} else {
			ShowWindow(SW_SHOW);
			SetForegroundWindow();			
		}
		
	}
	return CDialog::WindowProc(message, wParam, lParam);	
}

void CTibiaautoDlg::reportUsage()
{
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\tibiaauto-stats-usage.txt",installPath);
	FILE *f = fopen(pathBuf,"a+");
	if (f)
	{
		int tm=time(NULL);
		int count=CModuleProxy::allModulesCount;
		int pos;
		int checksum=tm%177;
		fprintf(f,"version=2.5.0,tm=%d,",tm);
		for (pos=0;pos<count;pos++)
		{
			CModuleProxy *mod=CModuleProxy::allModules[pos];
			fprintf(f,"%s=%d,",mod->getName(),mod->isStarted());
			checksum=checksum*3+strlen(mod->getName())*9+mod->isStarted()*13;
		}
		fprintf(f,"checksum=%d\n",checksum%1000000);
		fclose(f);
	}
}

void CTibiaautoDlg::OnToolXray() 
{
	m_moduleXRay->showConfigDialog();	
}

void CTibiaautoDlg::OnButton1() 
{
	CMemReaderProxy reader;	

	int i;
	for (i=0;i<100000;i++)
	{
		CTibiaCharacter *self=reader.readSelfCharacter();
		delete self;
	}
}

BOOL CTibiaautoDlg::DestroyWindow() 
{

	
	return CDialog::DestroyWindow();
}

void CTibiaautoDlg::PostNcDestroy() 
{
	
	CDialog::PostNcDestroy();
}

CTibiaautoDlg::~CTibiaautoDlg()
{

	CDialog::~CDialog();
}

BOOL CTibiaautoDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (((LPNMHDR) lParam)->code == 123456) {
		Invalidate();
		m_browserAds.Refresh();
		skin.SetButtonSkin(	m_exit);
		skin.SetButtonSkin(	m_LoadedModules);
		skin.SetButtonSkin(	m_Options);
		skin.SetButtonSkin(	m_PythonScripts);
		skin.SetButtonSkin(	m_MonsterShow);
		skin.SetButtonSkin(	m_MapShow);
		skin.SetButtonSkin(	m_ItemConfig);
		skin.SetButtonSkin(	m_InjectMC);
		skin.SetButtonSkin(	m_CharInfo);
		skin.SetButtonSkin(	m_xray);
		skin.SetButtonSkin(	m_autoAttack);
		skin.SetButtonSkin(	m_login);
		skin.SetButtonSkin(	m_ammoRestack);
		skin.SetButtonSkin(	m_autoAim);
		skin.SetButtonSkin(	m_autoFish);
		skin.SetButtonSkin(	m_autoGo);
		skin.SetButtonSkin(	m_autoLooter);
		skin.SetButtonSkin(	m_autoRespond);
		skin.SetButtonSkin(	m_autoUh);
		skin.SetButtonSkin(	m_fluidDrinker);
		skin.SetButtonSkin(	m_runeMaker);
		skin.SetButtonSkin(	m_spellCaster);
		skin.SetButtonSkin(	m_fps);
		skin.SetButtonSkin(	m_antilogout);
		skin.SetButtonSkin(	m_mapHack);
		skin.SetButtonSkin(	m_creatureInfo);
		skin.SetButtonSkin(	m_save);
		skin.SetButtonSkin(	m_load);
		skin.SetButtonSkin(	m_eater);
		skin.SetButtonSkin(	m_tradeMon);
		skin.SetButtonSkin(	m_grouping);
		skin.SetButtonSkin(	m_light);
		skin.SetButtonSkin(	m_banker);
		skin.SetButtonSkin(	m_sorter);
		skin.SetButtonSkin(	m_seller);
		
		m_pythonScriptsDialog->Invalidate();
		m_loadedModules->Invalidate();
		
		int count=CModuleProxy::allModulesCount;
		int pos;
		for (pos=0;pos<count;pos++)
		{
			CModuleProxy *mod=CModuleProxy::allModules[pos];
			mod->getNewSkin(skin);
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}
