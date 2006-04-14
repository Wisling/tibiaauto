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

#include "detours.h"


HANDLE hPipe=INVALID_HANDLE_VALUE;


extern int toolAutoResponderRunning;


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
	DDX_Control(pDX, IDC_TOOL_ANTILOG, m_antilogout);
	DDX_Control(pDX, IDC_TOOL_MAPHACK_INFO, m_mapHackInfo);
	DDX_Control(pDX, IDC_TOOL_MAPHACK, m_mapHack);
	DDX_Control(pDX, IDC_TOOL_CREATUREINFO, m_creatureInfo);
	DDX_Control(pDX, IDC_SAVE, m_save);
	DDX_Control(pDX, IDC_LOAD, m_load);
	DDX_Control(pDX, IDC_TOOL_EATER, m_eater);
	DDX_Control(pDX, IDC_TOOL_TRADEMON, m_tradeMon);
	DDX_Control(pDX, IDC_TOOL_AUTOGROUP, m_grouping);
	DDX_Control(pDX, IDC_TOOL_AUTORESPOND_INFO, m_infoAutoRespond);
	DDX_Control(pDX, IDC_TOOL_AUTOLOOTER_INFO, m_infoModuleLooter);
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK, m_buttonAutoAttack);
	DDX_Control(pDX, IDC_TOOL_AMMORESTACK_INFO, m_infoAmmoRestack);
	DDX_Control(pDX, IDC_TOOL_FLUIDDRINKER_INFO, m_infoFluidDrinker);
	DDX_Control(pDX, IDC_TOOL_AUTOAIM_INFO, m_infoAutoAim);
	DDX_Control(pDX, IDC_TOOL_SPELLCASTER_INFO, m_infoSpellCaster);	
	DDX_Control(pDX, IDC_TOOL_AUTOATTACK_INFO, m_infoAutoAttack);
	DDX_Control(pDX, IDC_TOOL_AUTOUH_INFO, m_infoAutoUH);
	DDX_Control(pDX, IDC_LOGINNAME, m_loginName);
	DDX_Control(pDX, IDC_LIGHT, m_light);
	DDX_Control(pDX, IDC_TOOL_AUTOFISH_INFO, m_infoAutoFish);
	DDX_Control(pDX, IDC_TOOL_AUTOGO_INFO, m_infoAutoGo);
	DDX_Control(pDX, IDC_TOOL_RUNEMAKER_INFO, m_infoRuneMaker);	
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
	ON_BN_CLICKED(IDC_DONATION, OnDonation)
	ON_BN_CLICKED(IDC_TOOL_CREATUREINFO, OnToolCreatureinfo)
	ON_BN_CLICKED(IDC_TOOL_MAPHACK, OnToolMaphack)
	ON_BN_CLICKED(IDC_TOOL_TEAM, OnToolTeam)
	ON_BN_CLICKED(IDC_TOOL_ANTILOG, OnToolAntilog)
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
		

	m_valueTab=(long int *)malloc(sizeof(long int)*1024*1024);
	m_memory=(long int *)malloc(sizeof(long int)*1024*1024);


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

	m_loadedModules = new CLoadedModules();
	m_loadedModules->Create(IDD_LOADED_MODULES);
	
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

	refreshToolInfo();
	SetTimer(1001,100,NULL);	

	CDonationDialog donDialog;
	donDialog.DoModal();	




		
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
		SetTimer(1001,150,NULL);
	
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

	sprintf(buf,"%s",
		m_moduleRuneMaker->isStarted()?"active":"-");
	m_infoRuneMaker.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleAutoGo->isStarted()?"active":"-");
	m_infoAutoGo.SetWindowText(buf);
	
	sprintf(buf,"%s",
		m_moduleFisher->isStarted()?"active":"-");
	m_infoAutoFish.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleAutoAttack->isStarted()?"active":"-");
	m_infoAutoAttack.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleUH->isStarted()?"active":"-");
	m_infoAutoUH.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleSpellCaster->isStarted()?"active":"-");
	m_infoSpellCaster.SetWindowText(buf);	

	sprintf(buf,"%s",
	m_moduleAim->isStarted()?"active":"-");
	m_infoAutoAim.SetWindowText(buf);		

	sprintf(buf,"%s",
		m_moduleFluid->isStarted()?"active":"-");
	m_infoFluidDrinker.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleRestack->isStarted()?"active":"-");
	m_infoAmmoRestack.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleLooter->isStarted()?"active":"-");
	m_infoModuleLooter.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleResponder->isStarted()?"active":"-");
	m_infoAutoRespond.SetWindowText(buf);

	sprintf(buf,"%s",
		m_moduleMapHack->isStarted()?"active":"-");
	m_mapHackInfo.SetWindowText(buf);

	// refresh push-like modules
	m_light.SetCheck(m_moduleLight->isStarted());
	m_grouping.SetCheck(m_moduleGrouping->isStarted());
	m_tradeMon.SetCheck(m_moduleTradeMon->isStarted());
	m_eater.SetCheck(m_moduleEater->isStarted());
	m_creatureInfo.SetCheck(m_moduleCreatureInfo->isStarted());
	m_antilogout.SetCheck(m_moduleAntylogout->isStarted());
}

void CTibiaautoDlg::OnSave() 
{
	CMemReaderProxy reader;		
	char fName[128];
	char *loggedCharName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);		
	FILE *f;

	sprintf(fName,"tibiaAuto.cfg.%s.xml",loggedCharName);
	f = fopen(fName,"wb");
	if (f)
	{				
		fprintf(f,"<configfile>\n");
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
		}
		fprintf(f,"\n</configfile>\n");
		fclose(f);
	}
	free(loggedCharName);
}

DWORD WINAPI loadThread( LPVOID lpParam )
{
	XercesDOMParser *parser;
	parser = new XercesDOMParser();	
	CConfigDialogStatus * m_configDialogStatus = (CConfigDialogStatus *)lpParam;
	

	CMemReaderProxy reader;
	char fName[128];
	char *loggedCharName=reader.GetLoggedChar(CMemUtil::m_globalProcessId);	
	FILE *f;
	

	sprintf(fName,"tibiaAuto.cfg.%s.xml",loggedCharName);	
	f = fopen(fName,"rb");
	if (f)
	{		
		char logBuf[16384];
		fclose(f);
		
		int modNr;
		int *restartedModulesTab = new int[CModuleProxy::allModulesCount];
		for (modNr=0;modNr<CModuleProxy::allModulesCount;modNr++)
		{
			CModuleProxy * module = CModuleProxy::allModules[modNr];			
			restartedModulesTab[modNr]=module->isStarted();
		}
	
		

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
				
		
		parser->parse(fName);		
		
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
	}
	free(loggedCharName);	
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

	CConfigDialogStatus * configDialogStatus = new CConfigDialogStatus();	
	configDialogStatus->Create(IDD_CONFIGSTATUS);
	configDialogStatus->ShowWindow(SW_SHOW);

	DWORD threadId;
	::CreateThread(NULL,0,loadThread,configDialogStatus,0,&threadId);						
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
	ExitProcess(0);	
}

void CTibiaautoDlg::OnClose() 
{
	ShowWindow(SW_MINIMIZE);
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
		ShowWindow(SW_MINIMIZE);
		eventStop=1;
	} 
	if (pMsg->message==WM_KEYDOWN&&pMsg->wParam==VK_RETURN)
	{
		eventStop=1;
	} 
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
	if (m_tradeMon.GetCheck())
	{
		m_moduleTradeMon->start();
	} else {
		m_moduleTradeMon->stop();
	}	
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
				
		char maxNo=10;
		FILE *f = fopen(pathName,"r+b");
		if (f)
		{
			
			
			if (!fseek(f,itemProxy.getValueForConst("addrMCInject")+2,SEEK_SET))
			{
				fwrite(&maxNo,1,1,f);
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

void CTibiaautoDlg::OnDonation() 
{
	CDonationDialog donDialog;
	donDialog.DoModal();
	
}

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
