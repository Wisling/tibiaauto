// tibiaautoDlg.h : header file
//
//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES

#if !defined(AFX_TIBIAAUTODLG_H__27D9EDA8_C837_406F_9952_9B3288BD9A18__INCLUDED_)
#define AFX_TIBIAAUTODLG_H__27D9EDA8_C837_406F_9952_9B3288BD9A18__INCLUDED_

#include "LoadedModules.h"

#include "IModuleInterface.h"
#include "ConfigDialogStatus.h"
#include "PythonScriptsDialog.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"

#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

XERCES_CPP_NAMESPACE_USE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg dialog

class CTibiaautoDlg : public MyDialog
{
// Construction
public:
	void DoSetButtonSkin();
	void refreshToolInfo();
	CTibiaautoDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
	//{{AFX_DATA(CTibiaautoDlg)
	enum { IDD = IDD_TIBIAAUTO_DIALOG };
	CGroupBoxEx m_OtherTools;
	CGroupBoxEx m_InformationTools;
	CGroupBoxEx m_FightTools;
	CGroupBoxEx m_Config;
	CGroupBoxEx m_CavebotTools;
	CGroupBoxEx m_AFKTools;
	CButtonST m_exit;
	CButtonST m_LoadedModules;
	CButtonST m_Options;
	CButtonST m_PythonScripts;
	CButtonST m_MonsterShow;
	CButtonST m_MapShow;
	CButtonST m_ItemConfig;
	CButtonST m_InjectMC;
	CButtonST m_CharInfo;
	CButtonST m_xray;
	CButtonST m_autoAttack;
	CButtonST m_login;
	CButtonST m_ammoRestack;
	CButtonST m_autoAim;
	CButtonST m_autoFish;
	CButtonST m_autoGo;
	CButtonST m_autoLooter;
	CButtonST m_autoRespond;
	CButtonST m_autoUh;
	CButtonST m_fluidDrinker;
	CButtonST m_runeMaker;
	CButtonST m_spellCaster;
	CButtonST m_fps;
	CButtonST m_antilogout;
	CButtonST m_mapHack;
	CButtonST m_creatureInfo;
	CButtonST m_save;
	CButtonST m_load;
	CButtonST m_eater;
	CButtonST m_tradeMon;
	CButtonST m_grouping;
	CButtonST m_light;
	CButtonST m_banker;
	CButtonST m_sorter;
	CButtonST m_seller;
	CStatic m_infoMapShow;
	CStatic m_loginName;
	CWebBrowser2 m_browserAds;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTibiaautoDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFinalRelease();
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeEditAddress();
	afx_msg void OnChangeEditValue();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLight();
	afx_msg void OnToolRunemaker();
	afx_msg void OnToolCharinfo();
	afx_msg void OnToolMonsershow();
	afx_msg void OnToolAutogo();
	afx_msg void OnToolAutofish();
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnToolAutogroup();
	afx_msg void OnToolAutouh();
	afx_msg void OnToolMapshow();
	afx_msg void OnToolAutoattack();
	afx_msg void OnToolSpellcaster();
	afx_msg void OnExit();
	afx_msg void OnClose();
	afx_msg void OnToolAutoaim();
	afx_msg void OnMemdebug();
	afx_msg void OnToolFluiddrinker();
	afx_msg void OnToolTrademon();
	afx_msg void OnToolInjectmc();
	afx_msg void OnToolItemconfig();
	afx_msg void OnToolAmmorestack();
	afx_msg void OnToolAutorespond();
	afx_msg void OnLoadedModules();
	afx_msg void OnToolAutolooter();
	afx_msg void OnToolEater();
	afx_msg void OnToolCreatureinfo();
	afx_msg void OnToolMaphack();
	afx_msg void OnToolTeam();
	afx_msg void OnToolAntilog();
	afx_msg void OnFps();
	afx_msg void OnPythonScripts();
	afx_msg void OnOptions();
	afx_msg void OnToolLogin();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnToolXray();
	afx_msg void OnToolSorter();
	afx_msg void OnToolBanker();
	afx_msg void OnToolSeller();
	afx_msg void OnButton1();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
private:
	int injectDll(HANDLE process, char* path);
	void InitialiseIPC();

	long m_processId;


public:
	~CTibiaautoDlg();
	void passSecurityInfo(int value);
	int shutdownCounter;
	LRESULT DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
private:
	void reportUsage();
	void loadConfig(CString);
	void refreshAds();
	void sendStats();
	void setShellTray();
	XercesDOMParser *parser;
	CLoadedModules *m_loadedModules;
	CPythonScriptsDialog *m_pythonScriptsDialog;
	CConfigDialogStatus * configDialogStatus;


	IModuleInterface *m_moduleLooter;
	IModuleInterface *m_moduleLight;
	IModuleInterface *m_modulePlayerInfo;
	IModuleInterface *m_moduleUH;
	IModuleInterface *m_moduleGrouping;
	IModuleInterface *m_moduleRestack;
	IModuleInterface *m_moduleFluid;
	IModuleInterface *m_moduleAim;
	IModuleInterface *m_moduleFisher;
	IModuleInterface *m_moduleSpellCaster;
	IModuleInterface *m_moduleTradeMon;
	IModuleInterface *m_moduleMemDebug;
	IModuleInterface *m_moduleMapShow;
	IModuleInterface *m_moduleMonsterShow;
	IModuleInterface *m_moduleItemConfig;
	IModuleInterface *m_moduleAutoGo;
	IModuleInterface *m_moduleAutoAttack;
	IModuleInterface *m_moduleRuneMaker;
	IModuleInterface *m_moduleEater;
	IModuleInterface *m_moduleCreatureInfo;
	IModuleInterface *m_moduleResponder;
	IModuleInterface *m_moduleMapHack;
	IModuleInterface *m_moduleTeam;
	IModuleInterface *m_moduleAntylogout;
	IModuleInterface *m_moduleFps;
	IModuleInterface *m_moduleLogin;
	IModuleInterface *m_moduleXRay;
	IModuleInterface *m_moduleSorter;
	IModuleInterface *m_moduleBanker;
	IModuleInterface *m_moduleSeller;

	HHOOK hhookKeyb;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIBIAAUTODLG_H__27D9EDA8_C837_406F_9952_9B3288BD9A18__INCLUDED_)
