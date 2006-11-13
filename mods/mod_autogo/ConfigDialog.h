#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_autogo.h"
#include "ConfigData.h"
#include "ImageButtonWithStyle.h"

#define TRIGGER_BATTLELIST		0x0001
	#define BATTLELIST_PLAYER		0x0001
	#define BATTLELIST_MONSTER		0x0002
	#define BATTLELIST_GM			0x0004	
	#define BATTLELIST_PARANOIAM	0x0008
	#define BATTLELIST_BATTLELIST	0x0010
#define TRIGGER_SIGN			0x0002
	#define SIGN_BATTLE			0x0080
	#define SIGN_POISON			0x0001
	#define SIGN_FIRE			0x0002
	#define SIGN_ENERGY			0x0004
#define TRIGGER_MESSAGE			0x0004
	#define MESSAGE_PRIVATE		0x0001
	#define MESSAGE_PUBLIC		0x0002
#define TRIGGER_MOVE			0x0008
#define TRIGGER_HPLOSS			0x0010
#define TRIGGER_HPBELOW			0x0020
#define TRIGGER_SOULPOINT		0x0040
#define TRIGGER_BLANK			0x0080
#define TRIGGER_CAPACITY		0x0100
#define TRIGGER_OUTOF			0x0200
	#define OUTOF_FOOD			0x0001
	#define OUTOF_WORMS			0x0002
	#define OUTOF_SPACE			0x0004


#define ACTION_NONE				0x00
#define ACTION_SUSPEND			0x01
#define ACTION_LOGOUT			0x02
#define ACTION_KILL				0x04
#define ACTION_SHUTDOWN			0x08
#define ACTION_RUNAWAY			0x10

#define ACTION_NONE_POS			0
#define ACTION_SUSPEND_POS		1
#define ACTION_LOGOUT_POS		2
#define ACTION_KILL_POS			3
#define ACTION_SHUTDOWN_POS		4
#define ACTION_RUNAWAY_POS		5

#define DIR_LEFT	1
#define DIR_RIGHT	2
#define DIR_UP		3
#define DIR_DOWN	4

class CMod_autogoApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public CDialog
{
// Construction
public:
	char status[128];
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	CConfigDialog(CMod_autogoApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CButton	m_battleBattlelist;
	CButton	m_battleParanoiaM;
	CComboBox	m_actDirection;
	CButton	m_soundHpBelow;
	CButton	m_triggerHpBelow;
	CEdit	m_hpBelow;
	CComboBox	m_actionHpBelow;
	CButton	m_outOfSpace;
	CEdit	m_actX;
	CEdit	m_actZ;
	CEdit	m_actY;
	CEdit	m_soulPoint;
	CButton	m_soundSign;
	CButton	m_triggerSoulPoint;
	CButton	m_triggerSign;
	CButton	m_triggerOutOf;
	CButton	m_triggerMessage;
	CButton	m_triggerMove;
	CButton	m_triggerHpLoss;
	CButton	m_triggerCapacity;
	CButton	m_triggerBlank;
	CButton	m_triggerBattleList;
	CButton	m_soundSoulPoint;
	CButton	m_soundOutOf;
	CButton	m_soundMove;
	CButton	m_soundMessage;
	CButton	m_soundHpLoss;
	CButton	m_soundCapacity;
	CButton	m_soundBlank;
	CButton	m_soundBattleList;
	CImageButtonWithStyle	m_signPoison;
	CImageButtonWithStyle	m_signFire;
	CImageButtonWithStyle	m_signEnergy;
	CImageButtonWithStyle	m_signBattle;
	CButton	m_outOfWorms;
	CButton	m_messagePublic;
	CButton	m_outOfFood;
	CButton	m_messagePrivate;
	CEdit	m_capacity;
	CEdit	m_blank;
	CButton	m_battleWhiteList;
	CButton	m_battlePlayer;
	CButton	m_battleMonster;
	CButton	m_battleGM;
	CComboBox	m_actionSoulPoint;
	CComboBox	m_actionSign;
	CComboBox	m_actionOutOf;
	CComboBox	m_actionMove;
	CComboBox	m_actionMessage;
	CComboBox	m_actionHpLoss;
	CComboBox	m_actionCapacity;
	CComboBox	m_actionBlank;
	CComboBox	m_actionBattleList;
	CStatic	m_status;
	CEdit	m_curZ;
	CEdit	m_curY;
	CEdit	m_curX;
	CEdit	m_runawayZ;	
	CEdit	m_runawayY;
	CEdit	m_runawayX;
	CButton	m_enable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDialog)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnEnable();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnAutogoTorunaway();
	afx_msg void OnAutogoTostart();
	afx_msg void OnAutogoWhitelist();
	afx_msg void OnAutogoTriggerbattlelist();
	afx_msg void OnAutogoTriggersign();
	afx_msg void OnAutogoTriggermessage();
	afx_msg void OnAutogoTriggerhploss();
	afx_msg void OnAutogoTriggermove();
	afx_msg void OnAutogoTriggersoulpoints();
	afx_msg void OnAutogoTriggerblank();
	afx_msg void OnAutogoTriggercapacity();
	afx_msg void OnAutogoTriggeroutof();
	afx_msg void OnAutogoTriggerhpbelow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CMod_autogoApp * m_app;
	char memWhiteList[100][64];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
