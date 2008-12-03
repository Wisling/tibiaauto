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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoDlg dialog

class CTibiaautoDlg : public CDialog
{
// Construction
public:	
	void refreshToolInfo();
	CTibiaautoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTibiaautoDlg)
	enum { IDD = IDD_TIBIAAUTO_DIALOG };
	CButton	m_xray;
	CButton	m_autoAttack;
	CButton	m_login;
	CButton	m_ammoRestack;
	CButton	m_autoAim;
	CButton	m_autoFish;
	CButton	m_autoGo;
	CButton	m_autoLooter;
	CButton	m_autoRespond;
	CButton	m_autoUh;
	CButton	m_fluidDrinker;
	CButton	m_runeMaker;
	CButton	m_spellCaster;
	CButton	m_fps;
	CButton	m_antilogout;
	CButton	m_mapHack;
	CButton	m_creatureInfo;
	CButton	m_save;
	CButton	m_load;
	CButton	m_eater;
	CButton	m_tradeMon;
	CButton	m_grouping;
	CStatic	m_infoMapShow;
	CStatic	m_loginName;
	CButton	m_light;
	CButton	m_banker;
	CButton	m_sorter;
	CButton	m_seller;
	CWebBrowser2	m_browserAds;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTibiaautoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFinalRelease();
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTibiaautoDlg)
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:	
	void InitialiseIPC();

	long m_processId;
	

public:
	~CTibiaautoDlg();
	void passSecurityInfo(int value);
		int shutdownCounter;
private:
	void reportUsage();
	void refreshAds();
	XercesDOMParser *parser;
	int m_lightPower;
	CLoadedModules *m_loadedModules;
	CPythonScriptsDialog *m_pythonScriptsDialog;


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

