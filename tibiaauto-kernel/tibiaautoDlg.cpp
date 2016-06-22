// tibiaautoDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "vld.h"
#include "tibiaauto.h"
#include "tibiaautoDlg.h"
#include "MemUtil.h"
#include <ModuleUtil.h>
#include "CharDialog.h"
#include <MemReader.h>
#include <TibiaItem.h>
#include "TibiaCharacter.h"
#include "ModuleLoader.h"
#include <MemReader.h>
#include <PackSender.h>
#include "ConfigCreatorUtil.h"
#include "DonationDialog.h"
#include <IPCBackPipe.h>
#include <TibiaItem.h>
#include "md5class.h"
#include "OptionsDialog.h"
#include "detours.h"
#include "PythonEngine.h"
#include "PythonScriptsDialog.h"
#include "PythonScript.h"
#include "url.h"
#include <iostream>
#include <fstream>
#include <TibiaMap.h>
#include <TAMiniMap.h>
#include <TileReader.h>


HANDLE hPipe   = INVALID_HANDLE_VALUE;
HWND tibiaHWND = NULL;


extern int toolAutoResponderRunning;
extern volatile char *checksum;

int globalProcessId;
NOTIFYICONDATA currentIconData;

// xerces things
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

class XStr
{
public:
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
#endif // ifdef _DEBUG

#define X(str) XStr(str).unicodeForm()


void InitTibiaHandle()
{
	CMemReader& reader = CMemReader::getMemReader();
	tibiaHWND = FindWindowEx(NULL, NULL, "TibiaClient", NULL);
	while (tibiaHWND)
	{
		DWORD pid;
		DWORD dwThreadId = ::GetWindowThreadProcessId(tibiaHWND, &pid);

		if (pid == CMemUtil::getMemUtil().getGlobalProcessId())
			break;
		tibiaHWND = FindWindowEx(NULL, tibiaHWND, "TibiaClient", NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg dialog

LRESULT CTibiaautoDlg::DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskbarRestart;

	if (uMessage == WM_CREATE)
	{
		s_uTaskbarRestart = RegisterWindowMessage(TEXT("TaskbarCreated"));
	}
	else
	{
		if (uMessage == s_uTaskbarRestart)
		{
			CMemReader& reader = CMemReader::getMemReader();
			setShellTray();
		}
	}

	return CWnd::DefWindowProc(uMessage, wParam, lParam);
}

CTibiaautoDlg::CTibiaautoDlg(CWnd* pParent /*=NULL*/)
	: MyDialog(CTibiaautoDlg::IDD, pParent)
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	XMLPlatformUtils::Initialize();

	parser = new XercesDOMParser();
}

void CTibiaautoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXIT, m_exit);
	DDX_Control(pDX, IDC_LOADED_MODULES, m_LoadedModules);
	DDX_Control(pDX, IDC_OPTIONS, m_Options);
	DDX_Control(pDX, IDC_PYTHON_SCRIPTS, m_PythonScripts);
	DDX_Control(pDX, IDC_TOOL_MONSERSHOW, m_MonsterShow);
	DDX_Control(pDX, IDC_TOOL_MAPSHOW, m_MapShow);
	DDX_Control(pDX, IDC_TOOL_ITEMCONFIG, m_ItemConfig);
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
}

BEGIN_MESSAGE_MAP(CTibiaautoDlg, CDialog)
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
ON_WM_CREATE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg message handlers

void CTibiaautoDlg::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_exit);
	skin.SetButtonSkin(m_LoadedModules);
	skin.SetButtonSkin(m_Options);
	skin.SetButtonSkin(m_PythonScripts);
	skin.SetButtonSkin(m_MonsterShow);
	skin.SetButtonSkin(m_MapShow);
	skin.SetButtonSkin(m_ItemConfig);
	skin.SetButtonSkin(m_CharInfo);
	skin.SetButtonSkin(m_xray);
	skin.SetButtonSkin(m_autoAttack);
	skin.SetButtonSkin(m_login);
	skin.SetButtonSkin(m_ammoRestack);
	skin.SetButtonSkin(m_autoAim);
	skin.SetButtonSkin(m_autoFish);
	skin.SetButtonSkin(m_autoGo);
	skin.SetButtonSkin(m_autoLooter);
	skin.SetButtonSkin(m_autoRespond);
	skin.SetButtonSkin(m_autoUh);
	skin.SetButtonSkin(m_fluidDrinker);
	skin.SetButtonSkin(m_runeMaker);
	skin.SetButtonSkin(m_spellCaster);
	skin.SetButtonSkin(m_fps);
	skin.SetButtonSkin(m_antilogout);
	skin.SetButtonSkin(m_mapHack);
	skin.SetButtonSkin(m_creatureInfo);
	skin.SetButtonSkin(m_save);
	skin.SetButtonSkin(m_load);
	skin.SetButtonSkin(m_eater);
	skin.SetButtonSkin(m_tradeMon);
	skin.SetButtonSkin(m_grouping);
	skin.SetButtonSkin(m_light);
	skin.SetButtonSkin(m_banker);
	skin.SetButtonSkin(m_sorter);
	skin.SetButtonSkin(m_seller);
}

BOOL CTibiaautoDlg::OnInitDialog()
{
	configDialogStatus = NULL;
	srand((unsigned int)time(NULL));
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);                 // Set big icon
	SetIcon(m_hIcon, FALSE);                // Set small icon

	CCharDialog *charDialog = new CCharDialog();
	m_processId     = charDialog->DoModal();
	globalProcessId = m_processId;
	delete charDialog;
	if (m_processId == -1)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}

	unsigned char buf[5];
	int versionOk = 0;
	
	int m_memAddressRevealCName1 = CTibiaItem::getValueForConst("addrFunRevealCName1");
	buf[0] = buf[1] = 0;
	CMemUtil::getMemUtil().GetMemRange(m_processId, m_memAddressRevealCName1, m_memAddressRevealCName1 + 2, (char *)buf, 1);
	if (buf[0] == 0x90 && buf[1] == 0x90)
		versionOk = 1;
	if ((buf[0] == 0x75 || buf[0] == 0xEB) && (buf[1] == 0x0A || buf[1] == 0x10 || buf[1] == 0x0E))
		versionOk = 1;

	if (!versionOk)
	{
		char outBuf[100];

		sprintf(outBuf, "tibia.exe version mismatch! Terminating Tibia Auto! (%x  %x)", buf[0], buf[1]);
		AfxMessageBox(outBuf);

		EndDialog(IDCANCEL);
		return TRUE;
	}


	CMemUtil::getMemUtil().setGlobalProcessId(m_processId);

	InitialiseIPC();

	CMemReader& reader = CMemReader::getMemReader();
	CMemUtil::getMemUtil().setGlobalProcessId(m_processId);
	
	m_loadedModules = new CLoadedModules();
	m_loadedModules->Create(IDD_LOADED_MODULES);

	m_pythonScriptsDialog = new CPythonScriptsDialog();
	m_pythonScriptsDialog->Create(IDD_PYTHON_SCRIPTS);

	m_moduleLooter     = CModuleLoader::LoadModule("mod_looter", 0);
	m_moduleLight      = CModuleLoader::LoadModule("mod_light", 0);
	m_modulePlayerInfo = CModuleLoader::LoadModule("mod_playerinfo", 0);
	m_modulePlayerInfo->start();
	m_moduleUH           = CModuleLoader::LoadModule("mod_uh", 0);
	m_moduleGrouping     = CModuleLoader::LoadModule("mod_grouping", 0);
	m_moduleRestack      = CModuleLoader::LoadModule("mod_restack", 0);
	m_moduleFluid        = CModuleLoader::LoadModule("mod_fluid", 0);
	m_moduleAim          = CModuleLoader::LoadModule("mod_aim", 0);
	m_moduleFisher       = CModuleLoader::LoadModule("mod_fisher", 0);
	m_moduleSpellCaster  = CModuleLoader::LoadModule("mod_spellcaster", 0);
	m_moduleTradeMon     = CModuleLoader::LoadModule("mod_trademon", 0);
	m_moduleMemDebug     = CModuleLoader::LoadModule("mod_memdebug", 0);
	m_moduleMapShow      = CModuleLoader::LoadModule("mod_showmap", 0);
	m_moduleMonsterShow  = CModuleLoader::LoadModule("mod_monstershow", 0);
	m_moduleItemConfig   = CModuleLoader::LoadModule("mod_itemconfig", 0);
	m_moduleAutoGo       = CModuleLoader::LoadModule("mod_autogo", 0);
	m_moduleAutoAttack   = CModuleLoader::LoadModule("mod_cavebot", 0);
	m_moduleRuneMaker    = CModuleLoader::LoadModule("mod_runemaker", 0);
	m_moduleEater        = CModuleLoader::LoadModule("mod_eater", 0);
	m_moduleCreatureInfo = CModuleLoader::LoadModule("mod_creatureinfo", 0);
	m_moduleResponder    = CModuleLoader::LoadModule("mod_responder", 0);
	m_moduleMapHack      = CModuleLoader::LoadModule("mod_maphack", 0);
	m_moduleTeam         = CModuleLoader::LoadModule("mod_team", 0);
	m_moduleAntylogout   = CModuleLoader::LoadModule("mod_antylogout", 0);
	m_moduleFps          = CModuleLoader::LoadModule("mod_fps", 0);
	m_moduleLogin        = CModuleLoader::LoadModule("mod_login", 0);
	m_moduleXRay         = CModuleLoader::LoadModule("mod_xray", 0);
	m_moduleSorter       = CModuleLoader::LoadModule("mod_sorter", 0);
	m_moduleBanker       = CModuleLoader::LoadModule("mod_banker", 0);
	m_moduleSeller       = CModuleLoader::LoadModule("mod_seller", 0);
	refreshToolInfo();


	if (!CModuleUtil::getTASetting("SeenMapMessage"))  // Disabled Dec 19/11 until these map files will actually be used
	{       //AfxMessageBox("Please consider submitting your map files to TibiaAuto.net to help us with the new system in place to create a complete Tibia map. \n\nYou will only receive this reminder once. Thank you for any contribution you make.");
		//CModuleUtil::setTASetting("SeenMapMessage",1);
		//OnOptions();
	}
	if (CModuleUtil::getTASetting("GatherBotStats"))
	{
		COptionsDialog dlg;
		//dlg.Create(IDD_OPTIONS);
		dlg.sendStats();
	}


	SetTimer(1001, 100, NULL);
	SetTimer(1002, 100, NULL);
	
	CPythonEngine pythonEngine;
	pythonEngine.init();

	int ffBoxDisplay = 1;

	char ffCheckString[1024];
	unsigned long ffCheckLen = 1023;
	ffCheckString[0] = '\0';
	HKEY hkey = NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_ALL_ACCESS, &hkey))
	{
		RegQueryValueEx(hkey, TEXT("FFcheck"), NULL, NULL, (unsigned char *)ffCheckString, &ffCheckLen);

		if (strlen(ffCheckString))
		{
			// found
			if (time(NULL) - atoi(ffCheckString) < 60 * 60 * 24 * 3)
				ffBoxDisplay = 0;
		}
		char buf[128];
		sprintf(buf, "%d", time(NULL));
		RegSetValueEx(hkey, TEXT("FFcheck"), 0, REG_SZ, (const unsigned char *)buf, strlen(buf) + 1);
		RegCloseKey(hkey);
	}

	if (ffBoxDisplay)
	{
		// now check for firefox
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Mozilla\\Mozilla Firefox\\", 0, KEY_READ, &hkey))
		{
			RegCloseKey(hkey);
			ffBoxDisplay = 0;
		}
	}
	if (ffBoxDisplay)
	{
		// now check for firefox
		if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Mozilla\\Mozilla Firefox 2.0\\", 0, KEY_READ, &hkey))
		{
			RegCloseKey(hkey);
			ffBoxDisplay = 0;
		}
	}

	if (ffBoxDisplay)
		if (AfxMessageBox("Tibia Auto has detected that you are not using Mozilla Firefox. It is recommend that you install it. Do you want to proceed with installation?", MB_YESNO) == IDYES)
			::ShellExecute(NULL, NULL, "http://tibiaauto.net/firefox.html", NULL, NULL, SW_SHOWNORMAL);

	m_moduleMapShow->showConfigDialog(); // As showmap modules runs its calculations in the window instead of a non-window thread, loading requires this to already be created.

	if (CModuleUtil::getTASetting("LoadScriptOnStartup"))
	{
		char fName[128];
		char charName[65];
		reader.GetLoggedChar(CMemUtil::getMemUtil().getGlobalProcessId(), charName, sizeof(charName));
		sprintf(fName, "tibiaAuto.cfg.%s.xml", charName);
		char pathbuf[2048];
		CModuleUtil::getInstallPath(pathbuf);
		CString pathName = pathbuf;
		if (pathName.Right(1) != "\\")
			pathName += "\\";
		pathName += fName;
		loadConfig(pathName);
	}

	refreshAds();

	// set shell tray
	setShellTray();

	SetTimer(1003, 1000 * 60 * 15, NULL); // once every 15 minutes refresh ads
	SetTimer(1004, 1000 * 60 * 5, NULL); // once every 5 minutes report usage
	if (CModuleUtil::getTASetting("GatherBotStats"))
		SetTimer(1005, 5000, NULL);  //every 5 seconds check and record module stats
	SetTimer(1006, 5000, NULL); //refresh tray icon name if changed

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
	CMemReader& reader = CMemReader::getMemReader();

	if (nIDEvent == 1001)
	{
		KillTimer(1001);
		refreshToolInfo();
		char buf[1024];
		char loggedCharName[65];
		reader.GetLoggedChar(CMemUtil::getMemUtil().getGlobalProcessId(), loggedCharName, sizeof(loggedCharName));
		sprintf(buf, "Logged as: %s", loggedCharName);
		CString currentDisplay;
		m_loginName.GetWindowText(currentDisplay);
		if (currentDisplay != buf)
			m_loginName.SetWindowText(buf);
		SetTimer(1001, 150, NULL);
	}
	if (nIDEvent == 1002)
	{
		KillTimer(1002);

		CPythonEngine::periodicalTick();
		CPythonEngine::backpipeMsgTick();
		CPythonEngine::backpipeTamsgTick(); // call allows the pipe between TAinject2 and TA to flush itself into a TA-managed pipe even if there is no message waiting for the function
		CPythonEngine::backpipeInpacketTick();

		CPythonEngine::threadGILAcquire(1); // allows python bound threads to still have cycles to run or terminate on when module is disabled

		SetTimer(1002, 25, NULL);
	}
	if (nIDEvent == 1003)
		refreshAds();
	if (nIDEvent == 1004)
		reportUsage();
	if (nIDEvent == 1005)
	{
		static unsigned int enabledModules = 0xffffffff;
		unsigned int modCheck              = 0;
		int i = 0;
		for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end() && i < 32; ++it, ++i)
		{
			modCheck |= it->second->isStarted() << i;
		}
		if (modCheck != enabledModules)
		{
			enabledModules = modCheck;
			char path[1024];
			CModuleUtil::getInstallPath(path);
			char pathBuf[2048];
			sprintf(pathBuf, "%s\\tascripts\\module %d statistics.txt", path, CMemUtil::getMemUtil().getGlobalProcessId());
			std::ofstream fout(pathBuf, std::ios::out | std::ios::app | std::ios::binary);
			time_t tm = time(NULL);
			fout.write((char*)&tm, 4);
			fout.write((char*)&enabledModules, 4);
			fout.close();
		}
	}
	if (nIDEvent == 1006)
	{
		char loggedCharName[65];
		reader.GetLoggedChar(CMemUtil::getMemUtil().getGlobalProcessId(), loggedCharName, sizeof(loggedCharName));
		if (strcmp(loggedCharName, currentIconData.szTip) != 0)
		{
			snprintf(currentIconData.szTip, 60, "%s", loggedCharName);
			Shell_NotifyIcon(NIM_MODIFY, &currentIconData);
		}
	}

	//New timers must be killed in OnExit
	CDialog::OnTimer(nIDEvent);
}

void CTibiaautoDlg::OnLight()
{
	m_moduleLight->showConfigDialog();
}

void CTibiaautoDlg::setShellTray()
{
	CMemReader& reader = CMemReader::getMemReader();
	ZeroMemory(&currentIconData, sizeof(NOTIFYICONDATA));
	currentIconData.cbSize = sizeof(NOTIFYICONDATA);
	currentIconData.hWnd   = GetSafeHwnd();
	currentIconData.uID    = 1;
	currentIconData.hIcon  = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
	char loggedCharName[65];
	reader.GetLoggedChar(CMemUtil::getMemUtil().getGlobalProcessId(), loggedCharName, sizeof(loggedCharName));
	snprintf(currentIconData.szTip, 60, "%s", loggedCharName);
	currentIconData.uCallbackMessage = WM_APP + 1;
	currentIconData.uFlags           = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	Shell_NotifyIcon(NIM_ADD, &currentIconData);
}

int CTibiaautoDlg::setDllLoadDir(HANDLE process, const char* dllLoadDir)
{
	LPVOID addrSetDllDir = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "SetDllDirectoryA");
	if (addrSetDllDir == NULL)
		return -1;

	LPVOID setDllDirArg = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllLoadDir) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (setDllDirArg == NULL)
		return -2;

	int n = WriteProcessMemory(process, setDllDirArg, dllLoadDir, strlen(dllLoadDir) + 1, NULL);
	if (n == 0)
		return -3;

	HANDLE threadID = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)addrSetDllDir, setDllDirArg, NULL, NULL);
	if (threadID == NULL)
		return -4;
	WaitForSingleObject(threadID, -1);
	CloseHandle(threadID);

	return 0;
}

int CTibiaautoDlg::injectDll(HANDLE process, const char* dllPath)
{
	LPVOID addrLibLoad = (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if (addrLibLoad == NULL)
		return -1;
	
	LPVOID libLoadArg = (LPVOID)VirtualAllocEx(process, NULL, strlen(dllPath) + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (libLoadArg == NULL)
		return -2;

	int n = WriteProcessMemory(process, libLoadArg, dllPath, strlen(dllPath) + 1, NULL);
	if (n == 0)
		return -3;
	
	HANDLE threadID = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)addrLibLoad, libLoadArg, NULL, NULL);
	if (threadID == NULL)
		return -4;
	WaitForSingleObject(threadID, -1);
	CloseHandle(threadID);

	return 0;
}

void CTibiaautoDlg::InitialiseIPC()
{
	char buf[128];
	char lpszPipename[128];
	sprintf(lpszPipename, "\\\\.\\pipe\\tibiaAutoPipe-%d", m_processId);


	hPipe = CreateNamedPipe(
	        lpszPipename,             // pipe name
	        PIPE_ACCESS_DUPLEX,       // read/write access
	        PIPE_TYPE_MESSAGE |       // message type pipe
	        PIPE_READMODE_MESSAGE |   // message-read mode
	        PIPE_WAIT,          // blocking mode
	        PIPE_UNLIMITED_INSTANCES, // max. instances
	        163840,                  // output buffer size
	        163840,                  // input buffer size
	        1000,                        // client time-out
	        NULL);                    // no security attribute


	if (hPipe == INVALID_HANDLE_VALUE)
	{
		sprintf(buf, "Invalid pipe handle: %d", GetLastError());
		AfxMessageBox(buf);
	}

	HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, true, m_processId);
	int status = setDllLoadDir(procHandle, CInstallPath::getInstallPath().c_str());
	if (status != 0)
	{
		sprintf(buf, "DLL load dir setup failed. Ret: %d, lastError: %d", status, GetLastError());
		AfxMessageBox(buf);
		PostQuitMessage(1);
		return;
	}
	status = injectDll(procHandle, "tibiaautoinject2.dll");
	if (status != 0)
	{
		sprintf(buf, "DLL injection failed. Ret: %d, lastError: %d", status, GetLastError());
		AfxMessageBox(buf);
		PostQuitMessage(1);
		return;
	}
	CloseHandle(procHandle);

	BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);


	if (!fConnected)
	{
		sprintf(buf, "Cannot connect properly to Tibia client: %d", GetLastError());
		AfxMessageBox(buf);
		PostQuitMessage(1);
		return;
	}

	CIpcMessage::hPipe = hPipe;
	// send my pid to the dll
	int myProcessId = GetCurrentProcessId();
	CIpcMessage mess;
	mess.messageType = 4;
	memcpy(mess.payload, &myProcessId, sizeof(int));
	mess.send();

	// wait for incoming IPC connection from the dll

	CIPCBackPipe::InitialiseIPC();

	// now hook keys
	HINSTANCE hinstDLL = NULL;
	hhookKeyb = NULL;

	hinstDLL = LoadLibrary((LPCTSTR) "tibiaautoinject3.dll");

	if (hinstDLL)
	{
		typedef LRESULT (CALLBACK * KeyboardProc_fun)(int nCode, WPARAM wParam, LPARAM lParam);
		static KeyboardProc_fun fun = (KeyboardProc_fun)GetProcAddress(hinstDLL, "KeyboardProc");
		if (fun)
		{
			HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
			THREADENTRY32 te32;

			// Take a snapshot of all running threads
			hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
			if (hThreadSnap == INVALID_HANDLE_VALUE)
				goto noKeybHook;

			// Fill in the size of the structure before using it.
			te32.dwSize = sizeof(THREADENTRY32);

			// Retrieve information about the first thread,
			// and exit if unsuccessful
			if (!Thread32First(hThreadSnap, &te32))
			{
				CloseHandle(hThreadSnap);   // Must clean up the snapshot object!
				goto noKeybHook;
			}

			// Now walk the thread list of the system,
			// and display information about each thread
			// associated with the specified process
			do
			{
				if (te32.th32OwnerProcessID == m_processId)
				{
					hhookKeyb = SetWindowsHookEx(WH_KEYBOARD, fun, hinstDLL, te32.th32ThreadID);
					break;
				}
			}
			while (Thread32Next(hThreadSnap, &te32));

			// Don't forget to clean up the snapshot object.
			CloseHandle(hThreadSnap);
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
	CMemReader& reader = CMemReader::getMemReader();
	char fName[128];
	char loggedCharName[65];
	reader.GetLoggedChar(CMemUtil::getMemUtil().getGlobalProcessId(), loggedCharName, sizeof(loggedCharName));
	FILE *f = NULL;

	char szFilters[] =
	        "Tibia Auto config (*.xml)|*.xml|All Files (*.*)|*.*||";

	sprintf(fName, "tibiaAuto.cfg.%s.xml", loggedCharName);
	CFileDialog fd(false, "", fName, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);

	if (fd.DoModal() == IDOK)
	{
		CString pathName = fd.GetPathName();
		f = fopen(pathName.GetBuffer(200), "wb");
	}

	if (f)
	{
		fprintf(f, "<configfile>\n");
		int modNr;
		int someModuleRunning = 0;
		for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
		{
			if (it->second->isStarted())
			{
				someModuleRunning = 1;
				break;
			}
		}
		//Save a list of started scripts so they can be started on load
		if (someModuleRunning)
		{
			if (AfxMessageBox("Some modules are running.\nWould you like to save a list of running modules?", MB_YESNO) == IDYES)
			{
				fprintf(f, "<startedModules>\n");
				for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
				{
					IModuleInterface* module = it->second;
					if (module->isStarted())
						fprintf(f, "<module name = \"%s\"/>", module->getModuleName());
				}
				fprintf(f, "</startedModules>\n");
				fprintf(f, "<startedScripts>\n");
				for (modNr = 0; modNr < CPythonScript::getScriptCount(); modNr++)
				{
					CPythonScript * script = CPythonScript::getScriptByNr(modNr);
					if (script->isEnabled())
						fprintf(f, "<module name = \"%s\"/>", script->getName());
				}
				fprintf(f, "</startedScripts>\n");
			}
		}

		// save "normal" modules
		for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
		{
			IModuleInterface* module = it->second;
			if (module != NULL)
			{
				char* nme = module->getName();
				CConfigCreatorUtil configCreator;
				if (!module->isStarted())
					module->controlsToConfig();
				DOMNode *moduleConfig = configCreator.getEmptyConfigForModule(module->getModuleName());

				int paramNr;
				const char** paramList = module->getConfigParamNames();
				for (paramNr = 0; paramList[paramNr] != NULL; ++paramNr)
				{
					const char *paramName = paramList[paramNr];
					if (module->isMultiParam(paramName))
					{
						module->resetMultiParamAccess(paramName);
						char *paramValue = module->saveConfigParam(paramName);
						while (strlen(paramValue))
						{
							char realParamName[1024];
							sprintf(realParamName, "config/%s", paramName);
							configCreator.addParamToConfig(moduleConfig, realParamName, paramValue);
							paramValue = module->saveConfigParam(paramName);
						};
					}
					else
					{
						char *paramValue = module->saveConfigParam(paramName);

						if (!paramValue)
							break;
						char realParamName[1024];
						sprintf(realParamName, "config/%s", paramName);
						configCreator.addParamToConfig(moduleConfig, realParamName, paramValue);
					}
				}
				configCreator.serializeConfigForModule(moduleConfig, f);
				configCreator.releaseConfig(moduleConfig);
			}
		} // saving normal modules config

		// python scripts
		int scriptNr = 0;
		for (;; )
		{
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr++);
			if (!pythonScript)
				break;

			char scriptModuleName[128];
			snprintf(scriptModuleName, 127, "Script: %s", pythonScript->getName());
			if (!strcmp("Script: Stub", scriptModuleName))
				int a = 1;
			CConfigCreatorUtil configCreator;
			DOMNode *moduleConfig;
			try
			{
				moduleConfig = configCreator.getEmptyConfigForModule(scriptModuleName);
			}
			catch (...)
			{
				char buf [1024];
				sprintf(buf, "The python module name \"%s\" is invalid", pythonScript->getName());
				AfxMessageBox(buf);
				continue;
			}

			int paramNr = 0;
			for (;; )
			{
				struct paramType *param = pythonScript->getParamDef(paramNr++);
				if (!param)
					break;

				char realParamName[1024];
				sprintf(realParamName, "config/%s", param->name);
				try
				{
					configCreator.addParamToConfig(moduleConfig, realParamName, param->value);
				}
				catch (...)
				{
					char buf [1024];
					sprintf(buf, "Parameter name \"%s\" for the python module \"%s\" is invalid", param->name, pythonScript->getName());
					AfxMessageBox(buf);
					continue;
				}
			}
			configCreator.serializeConfigForModule(moduleConfig, f);
			configCreator.releaseConfig(moduleConfig);
		} // saving python scripts config


		fprintf(f, "\n</configfile>\n");
		fclose(f);
	}
}

struct loadThreadParam
{
	CConfigDialogStatus *configDialogStatus;
	char fName[1024];
};

DWORD WINAPI loadThread(LPVOID lpParam)
{
	XercesDOMParser *parser;
	parser = new XercesDOMParser();
	CConfigDialogStatus * m_configDialogStatus = ((struct loadThreadParam *)lpParam)->configDialogStatus;

	CMemReader& reader = CMemReader::getMemReader();
	CConfigCreatorUtil configCreator;


	char logBuf[16384];

	int modNr;
	map<string, bool> restartedModulesTab;
	for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
	{
		restartedModulesTab[it->first] = (it->second->isStarted() != 0);
	}


	char loggedCharName[65];
	reader.GetLoggedChar(CMemUtil::getMemUtil().getGlobalProcessId(), loggedCharName, sizeof(loggedCharName));
	sprintf(logBuf, "Loading character '%s' ... started", loggedCharName);
	m_configDialogStatus->msgAddToLog(logBuf);

	//Modules
	for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
	{
		IModuleInterface* module = it->second;
		if (restartedModulesTab[it->first])
		{
			sprintf(logBuf, "Stopping module %s ...", module->getModuleName());
			m_configDialogStatus->msgAddToLog(logBuf);
			module->stop();
		}
	}

	//Scripts
	for (modNr = 0; modNr < CPythonScript::getScriptCount(); modNr++)
	{
		CPythonScript * script = CPythonScript::getScriptByNr(modNr);
		if (script->isEnabled())
			script->setEnabled(0);
	}


	parser->parse(((struct loadThreadParam *)lpParam)->fName);

	xercesc::DOMDocument *doc = parser->getDocument();
	DOMElement *root          = doc->getDocumentElement();

	for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
	{
		IModuleInterface* module = it->second;
		DOMNode *moduleConfig = configCreator.getConfigForModule(root, it->first.c_str());
		if (moduleConfig)
		{
			sprintf(logBuf, "Loading config for module %s ...", it->first.c_str());
			module->resetConfig();
			m_configDialogStatus->msgAddToLog(logBuf);
			configCreator.parseConfigFromNode(module, moduleConfig, "");
			module->configToControls();
		}
}

	int scriptNr = 0;
	for (;; )
	{
		CConfigCreatorUtil configCreator;
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr++);
		if (!pythonScript)
			break;

		char scriptConfigName[128];
		snprintf(scriptConfigName, 127, "Script: %s", pythonScript->getName());
		DOMNode *scriptConfig = configCreator.getConfigForModule(root, scriptConfigName);
		if (scriptConfig)
		{
			sprintf(logBuf, "Loading config for script %s", pythonScript->getName());
			m_configDialogStatus->msgAddToLog(logBuf);
			configCreator.parseConfigFromNode(pythonScript, scriptConfig, "");
		}
	}
	int otherModulesLoaded  = 0;
	DOMNodeList *childNodes = root->getElementsByTagName(X("startedModules"));
	DOMNode *moduleConfig;
	if (childNodes->getLength() && (moduleConfig = childNodes->item(0)))
	{
		int startOnLoad = CModuleUtil::getTASetting("StartModulesOnLoad");
		if (startOnLoad != 2 && (startOnLoad == 1 || AfxMessageBox("This config contains saved started modules. Do you wish to start them?", MB_YESNO) == IDYES))
		{
			DOMNode *subNode = moduleConfig->getFirstChild();
			if (subNode)
			{
				do
				{
					if (subNode->getNodeType() == DOMNode::ELEMENT_NODE)
					{
						char subNodeName[1024];
						wcstombs(subNodeName, subNode->getNodeName(), 1024);

						DOMNode *attrNode = subNode->getAttributes()->getNamedItem(L"name");
						if (!strcmp(subNodeName, "module") && attrNode)
						{
							char nodeValue[1024];
							wcstombs(nodeValue, attrNode->getNodeValue(), 1024);
							for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
							{
								IModuleInterface* module = it->second;
								if (!strcmp(nodeValue, it->first.c_str()))
								{
									sprintf(logBuf, "Starting module %s ...", it->first.c_str());
									m_configDialogStatus->msgAddToLog(logBuf);
									module->start();
								}
							}
						}
					}
				}
				while ((subNode = subNode->getNextSibling()) != NULL);
			}
			childNodes = root->getElementsByTagName(X("startedScripts"));
			DOMNode *scriptConfig;
			if (childNodes->getLength() && (scriptConfig = childNodes->item(0)))
			{
				DOMNode *subNode = scriptConfig->getFirstChild();
				if (subNode)
				{
					do
					{
						if (subNode->getNodeType() == DOMNode::ELEMENT_NODE)
						{
							char subNodeName[1024];
							wcstombs(subNodeName, subNode->getNodeName(), 1024);

							DOMNode *attrNode = subNode->getAttributes()->getNamedItem(L"name");

							if (!strcmp(subNodeName, "module") && attrNode)
							{
								char nodeValue[1024];
								wcstombs(nodeValue, attrNode->getNodeValue(), 1024);
								for (modNr = 0; modNr < CPythonScript::getScriptCount(); modNr++)
								{
									CPythonScript * script = CPythonScript::getScriptByNr(modNr);
									if (!strcmp(nodeValue, script->getName()))
									{
										sprintf(logBuf, "Starting script %s ...", script->getName());
										m_configDialogStatus->msgAddToLog(logBuf);
										script->setEnabled(1);
									}
								}
							}
						}
					}
					while ((subNode = subNode->getNextSibling()) != NULL);
				}
			}
			otherModulesLoaded = 1;
		}
	}
	if (!otherModulesLoaded)
	{
		for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
		{
			IModuleInterface * module = it->second;
			if (restartedModulesTab[it->first])
			{
				sprintf(logBuf, "Starting module %s ...", module->getModuleName());
				m_configDialogStatus->msgAddToLog(logBuf);
				module->start();
			}
		}
	}
	doc->release();

	sprintf(logBuf, "Loading character '%s' finished.", loggedCharName);
	m_configDialogStatus->msgAddToLog(logBuf);

	delete (struct loadThreadParam *)lpParam;
	return 0;
}

void CTibiaautoDlg::loadConfig(CString pathName)
{
	FILE *f = fopen(pathName.GetBuffer(1024), "rb");

	if (f)
	{
		fclose(f);

		if (configDialogStatus)
		{
			configDialogStatus->DestroyWindow();
			delete configDialogStatus;
		}

		configDialogStatus = new CConfigDialogStatus();
		configDialogStatus->Create(IDD_CONFIGSTATUS);
		configDialogStatus->ShowWindow(SW_SHOW);

		struct loadThreadParam *ltp = new struct loadThreadParam ();
		strncpy(ltp->fName, pathName.GetBuffer(1023), 1023);
		ltp->configDialogStatus = configDialogStatus;
		DWORD threadId;
		::CreateThread(NULL, 0, loadThread, ltp, 0, &threadId);
	}
}

void CTibiaautoDlg::OnLoad()
{
	int someModuleRunning = 0;
	for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
	{
		if (it->second->isStarted())
			someModuleRunning = 1;
	}
	if (someModuleRunning)
		if (AfxMessageBox("Some modules are running. Are you sure you want to load new configuration?", MB_YESNO) == IDNO)
			return;
	CMemReader& reader = CMemReader::getMemReader();

	char fName[128];
	char loggedCharName[65];
	reader.GetLoggedChar(CMemUtil::getMemUtil().getGlobalProcessId(), loggedCharName, sizeof(loggedCharName));

	char szFilters[] =
	        "Tibia Auto config (*.xml)|*.xml|All Files (*.*)|*.*||";

	sprintf(fName, "tibiaAuto.cfg.%s.xml", loggedCharName);
	CFileDialog fd(true, "", fName, OFN_FILEMUSTEXIST, szFilters, NULL);
	if (fd.DoModal() == IDOK)
	{
		CString pathName;
		pathName = fd.GetPathName();
		loadConfig(pathName);
	}
}

void CTibiaautoDlg::OnToolAutogroup()
{
	if (m_grouping.GetCheck())
		m_moduleGrouping->start();
	else
		m_moduleGrouping->stop();
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
	for (int i = 1001; i <= 1006; KillTimer(i++))
		;

	DisconnectNamedPipe(hPipe);
	Shell_NotifyIcon(NIM_DELETE, &currentIconData);

	CTAMiniMap::getTAMiniMap().unloadMiniMaps();
	CTibiaItem::cleanup();
	CTibiaMap::getTibiaMap().clear();
	CTileReader::getTileReader().cleanup();

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
	if (configDialogStatus)
	{
		configDialogStatus->DestroyWindow();
		delete configDialogStatus;
	}

	if (hhookKeyb)
		UnhookWindowsHookEx(hhookKeyb);

	for (int scriptNr = 0;; scriptNr++)
	{
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
		if (!pythonScript)
			break;
		delete pythonScript;
	}
	PostQuitMessage(0);
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

const UINT RWM_PRETRANSLATEMSG = ::RegisterWindowMessage(_T("RWM_PRETRANSLATEMSG"));
// If it has a Modeless parent windows then it returns the modeless window just before the parents become modal again.
// We want this as modal windows do not send all messages to their child modeless windows
HWND AppGetTopParent(HWND hWnd)
{
	if (hWnd == NULL)
		return NULL;
	int isModeless = CWnd::FromHandlePermanent(hWnd) == NULL;

	HWND hWndParent = hWnd;
	while (::GetParent(hWndParent) != NULL && !(isModeless && CWnd::FromHandlePermanent(::GetParent(hWndParent)) != NULL))
	{
		isModeless = CWnd::FromHandlePermanent(hWndParent) == NULL;
		hWndParent = ::GetParent(hWndParent);
	}
	return hWndParent;
}

BOOL CTibiaautoDlg::PreTranslateMessage(MSG* pMsg)
{
	//read http://www.codeproject.com/Articles/211862/PreTranslateMessage-support-in-modeless-dialogs-in
	//Modeless dialogs used for modules need special call to be able to send 'tab key' to them
	int eventStop = 0;
	HWND hWnd     = AppGetTopParent(pMsg->hwnd);
	if (this->m_hWnd != hWnd && hWnd && pMsg->message == 0x100)
	{
		if (::SendMessage(hWnd, RWM_PRETRANSLATEMSG, 0, (LPARAM)pMsg) == TRUE)
			return TRUE;
		return ::IsDialogMessage(hWnd, pMsg);
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		ShowWindow(SW_HIDE);
		eventStop = 1;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		eventStop = 1;
	}
	else if (pMsg->message == WM_CHAR)
	{
		eventStop = 1;
	}
	if (!eventStop)
		return CDialog::PreTranslateMessage(pMsg);
	else
		return 0;
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
		m_moduleEater->start();
	else
		m_moduleEater->stop();
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
		m_moduleAntylogout->start();
	else
		m_moduleAntylogout->stop();
}

void CTibiaautoDlg::OnFps()
{
	m_moduleFps->showConfigDialog();
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
	if (time(NULL) < 132882315 + 3600 * 24 * 14)
	{
		m_browserAds.Navigate("127.0.0.1", NULL, NULL, NULL, NULL);
		return;
	}
	m_browserAds.Navigate("http://ads.tibiaauto.net/showad.php?version=1.19.1", NULL, NULL, NULL, NULL);
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
	CMemReader& reader = CMemReader::getMemReader();
	if (message == WM_APP + 1 && lParam == WM_LBUTTONDOWN)
	{
		if (!tibiaHWND)
			InitTibiaHandle();
		if (IsWindowVisible())
		{
			if (!CModuleUtil::getTASetting("SeenHideTibiaMessage"))
			{
				CModuleUtil::setTASetting("SeenHideTibiaMessage", 1);
				if (AfxMessageBox("Both Tibia and TA can be hidden by clicking on the Tray icon.  Would you like to enable this?\nThis setting can be changed in \"General Options and Statistics\"", MB_YESNO) == IDYES)
					CModuleUtil::setTASetting("HideTibiaWithTA", 1);
				else
					CModuleUtil::setTASetting("HideTibiaWithTA", 0);
			}
			currentIconData.hIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME_HIDDEN));
			Shell_NotifyIcon(NIM_MODIFY, &currentIconData);
			ShowWindow(SW_HIDE);
			if (CModuleUtil::getTASetting("HideTibiaWithTA"))
				::ShowWindow(tibiaHWND, SW_HIDE);
		}
		else
		{
			currentIconData.hIcon = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDR_MAINFRAME));
			Shell_NotifyIcon(NIM_MODIFY, &currentIconData);
			ShowWindow(SW_SHOW);
			SetForegroundWindow();
			if (!::IsWindowVisible(tibiaHWND) && CModuleUtil::getTASetting("HideTibiaWithTA"))
			{
				::ShowWindow(tibiaHWND, SW_SHOW);
				if (::IsIconic(tibiaHWND))
					::ShowWindow(tibiaHWND, SW_RESTORE);
				::SetForegroundWindow(tibiaHWND);
			}
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CTibiaautoDlg::reportUsage()
{
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf, "%s\\tibiaauto-stats-usage.txt", installPath);
	FILE *f = fopen(pathBuf, "a+");
	if (f)
	{
		time_t tm = time(NULL);
		int checksum = tm % 177;
		fprintf(f, "version=2.67.0 tm=%d,", tm);
		for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
		{
			IModuleInterface* mod = it->second;
			fprintf(f, "%s=%d,", mod->getName(), mod->isStarted());
			checksum = checksum * 3 + strlen(mod->getName()) * 9 + mod->isStarted() * 13;
		}
		fprintf(f, "checksum=%d\n", checksum % 1000000);
		fclose(f);
	}
}

void CTibiaautoDlg::OnToolXray()
{
	m_moduleXRay->showConfigDialog();
}

void CTibiaautoDlg::OnButton1()
{
	CMemReader& reader = CMemReader::getMemReader();

	int i;
	for (i = 0; i < 100000; i++)
	{
		CTibiaCharacter self;
		reader.readSelfCharacter(&self);
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
	if (((LPNMHDR) lParam)->code == 123456)
	{
		Invalidate();
		m_browserAds.Refresh();
		DoSetButtonSkin();

		m_pythonScriptsDialog->Invalidate();
		m_loadedModules->Invalidate();

		for (ModuleMap::iterator it = CModuleLoader::loadedModules.begin(); it != CModuleLoader::loadedModules.end(); ++it)
		{
			it->second->getNewSkin(skin);
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

int CTibiaautoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) != 0)
		return -1;
	return 0;
}
