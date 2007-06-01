// tibiaautoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "tibiaautoDlg.h"
#include "MemUtil.h" 
#include "CharDialog.h"
#include "MemReaderProxy.h"
#include "TibiaMapProxy.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg dialog

CTibiaautoDlg::CTibiaautoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTibiaautoDlg::IDD, pParent)
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
	DDX_Control(pDX, IDC_LOGINNAME, m_loginName);
	DDX_Control(pDX, IDC_LIGHT, m_light);
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg message handlers
 
BOOL CTibiaautoDlg::OnInitDialog()
{	
	srand(time(NULL));
	CDialog::OnInitDialog();
	
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

	CMemUtil::setGlobalProcessId(m_processId);
	
				
	InitialiseIPC();

	CMemReaderProxy reader;
	reader.setProcessId(m_processId);	
	CPackSenderProxy sender;
	sender.setPipeHandle(hPipe);

	// shutdownCounter is anti-hack protection
	shutdownCounter=rand()%100;	
	CEnterCode *enterCode = new CEnterCode(this);
	int res=enterCode->DoModal();
	delete enterCode;

	if (res!=IDOK) ExitProcess(0);	

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

	refreshToolInfo();
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
			if (time(NULL)-atoi(ffCheckString)<60*60*24)
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
	

	refreshAds();	

	// set shell tray
	NOTIFYICONDATA data;
	data.cbSize=sizeof(NOTIFYICONDATA);
	data.hWnd=GetSafeHwnd();
	data.uID=1;		
	data.hIcon=AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	snprintf(data.szTip,60,"%s","<Running Tibia Auto>");
	data.uCallbackMessage=WM_APP+1;
	data.uFlags=NIF_ICON|NIF_TIP|NIF_MESSAGE;
	Shell_NotifyIcon(NIM_ADD,&data);
	
	
	SetTimer(1003,1000*60*15,NULL); // once every 15 minutes refresh ads	
	SetTimer(1004,1000*60*5,NULL); // once every 5 minutes refresh ads	
		
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

		SetTimer(1002,100,NULL);
	}
	if (nIDEvent==1003)
	{
		refreshAds();		
	}
	if (nIDEvent==1004)
	{		
		reportUsage();
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
	
	injectDll="tibiaauto_develtest.dll";				
	
	DetourContinueProcessWithDll(procHandle, injectDll);
		
	
	
	
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
	char buf[1000];

	m_runeMaker.SetCheck(m_moduleRuneMaker->isStarted());
	m_autoGo.SetCheck(m_moduleAutoGo->isStarted());
	m_autoFish.SetCheck(m_moduleFisher->isStarted());
	m_autoAttack.SetCheck(m_moduleAutoAttack->isStarted());
	m_autoUh.SetCheck(m_moduleUH->isStarted());
	m_spellCaster.SetCheck(m_moduleSpellCaster->isStarted());
	m_autoAim.SetCheck(m_moduleAim->isStarted());
	m_fluidDrinker.SetCheck(m_moduleFluid->isStarted());
	m_ammoRestack.SetCheck(m_moduleRestack->isStarted());
	m_autoLooter.SetCheck(m_moduleLooter->isStarted());
	m_autoRespond.SetCheck(m_moduleResponder->isStarted());
	m_mapHack.SetCheck(m_moduleMapHack->isStarted());
	m_light.SetCheck(m_moduleLight->isStarted());
	m_grouping.SetCheck(m_moduleGrouping->isStarted());
	m_tradeMon.SetCheck(m_moduleTradeMon->isStarted());
	m_eater.SetCheck(m_moduleEater->isStarted());
	m_creatureInfo.SetCheck(m_moduleCreatureInfo->isStarted());
	m_antilogout.SetCheck(m_moduleAntylogout->isStarted());
	m_fps.SetCheck(m_moduleFps->isStarted());
	m_login.SetCheck(m_moduleLogin->isStarted());
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
	CFileDialog fd(false,"",fName,OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	
	if (fd.DoModal()==IDOK)
	{
		CString pathName = fd.GetPathName();         						
		f=fopen(pathName.GetBuffer(200),"wb");
	}	
		
	if (f)
	{				
		fprintf(f,"<configfile>\n");
		
		// save "normal" modules
		int modNr;
		for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
		{
			CModuleProxy * module = CModuleProxy::allModules[modNr];
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
			CConfigCreatorUtil configCreator;			
			DOMNode *moduleConfig = configCreator.getEmptyConfigForModule(scriptModuleName);
			
			int paramNr=0;
			for (;;)
			{	
				struct paramType *param = pythonScript->getParamDef(paramNr++);
				if (!param) break;
								
				char realParamName[1024];
				sprintf(realParamName,"config/%s",param->name);
				configCreator.addParamToConfig(moduleConfig,realParamName,param->value);
				
			}
			configCreator.serializeConfigForModule(moduleConfig,f);
			configCreator.releaseConfig(moduleConfig);
		} // saving python scripts config
		

		fprintf(f,"\n</configfile>\n");
		fclose(f);
	}
	free(loggedCharName);
}

struct loadThreadParam
{
	CConfigDialogStatus *configDialogStatus;
	char fName[256];
};

DWORD WINAPI loadThread( LPVOID lpParam )
{
	XercesDOMParser *parser;
	parser = new XercesDOMParser();	
	CConfigDialogStatus * m_configDialogStatus = ((struct loadThreadParam *)lpParam)->configDialogStatus;	
	

	CMemReaderProxy reader;
	
	
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
			CConfigCreatorUtil configCreator;
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

	doc->release();
	
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
	
	sprintf(logBuf,"Loading character '%s' finished.",loggedCharName);
	m_configDialogStatus->msgAddToLog(logBuf);
	
	delete []restartedModulesTab;		
	
	free(loggedCharName);	
	free ((struct loadThreadParam *)lpParam);
	return 0;
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
	
	FILE *f=NULL;
	char fName[128];
	char *loggedCharName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);		
	
	
	char szFilters[]=
		"Tibia Auto config (*.xml)|*.xml|All Files (*.*)|*.*||";
	
	sprintf(fName,"tibiaAuto.cfg.%s.xml",loggedCharName);
	CFileDialog fd(true,"",fName,OFN_FILEMUSTEXIST, szFilters, NULL);
	
	
	if (fd.DoModal()==IDOK)
	{		
		CString pathName = fd.GetPathName();         						
		f=fopen(pathName.GetBuffer(200),"rb");	
		
		if (f)
		{
			fclose(f);			
			
			CConfigDialogStatus * configDialogStatus = new CConfigDialogStatus();	
			configDialogStatus->Create(IDD_CONFIGSTATUS);
			configDialogStatus->ShowWindow(SW_SHOW);
			
			struct loadThreadParam *ltp = new struct loadThreadParam();;
			strncpy(ltp->fName,pathName.GetBuffer(200),200);
			ltp->configDialogStatus=configDialogStatus;
			DWORD threadId;
			::CreateThread(NULL,0,loadThread,ltp,0,&threadId);				
			
		}		
	}	
	
	free(loggedCharName);
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
				unsigned char val=0xeb;
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
	m_browserAds.Navigate("http://ads.tibiaauto.net/showad.php?version=1.14.1",NULL,NULL,NULL,NULL);
}

void CTibiaautoDlg::OnToolLogin() 
{
	m_moduleLogin->showConfigDialog();		
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
	FILE *f = fopen("tibiaauto-stats-usage.txt","a+");
	if (f)
	{
		int tm=time(NULL);
		int count=CModuleProxy::allModulesCount;
		int pos;
		int checksum=tm%177;
		fprintf(f,"version=1.14.1,tm=%d,",tm);
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
