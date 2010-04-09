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
#include "GroupBoxEx.h"
#include "BtnST.h"
#include "MyDialog.h"
#include "XTabCtrl.h"

#define BATTLELIST_ANXIETY				0x0001
#define BATTLELIST_PARANOIAM			0x0002
#define MAKE_BLACKLIST					0x0004

#define TRIGGER_MESSAGE					0x0004
	#define MESSAGE_PRIVATE				0x0001
	#define MESSAGE_PUBLIC				0x0002
	#define MESSAGE_IGNORE_SPELLS		0x0004
#define TRIGGER_MOVE					0x0008
#define TRIGGER_HPLOSS					0x0010
#define TRIGGER_HPBELOW					0x0020
#define TRIGGER_SOULPOINT_BELOW			0x0040
#define TRIGGER_BLANK					0x0080
#define TRIGGER_CAPACITY				0x0100
#define TRIGGER_OUTOF					0x0200
#define TRIGGER_HPABOVE					0x0400
#define TRIGGER_MANABELOW				0x0800
#define TRIGGER_MANAABOVE				0x1000
#define TRIGGER_SOULPOINT_ABOVE			0x2000
#define TRIGGER_RUNAWAY_REACHED			0x4000
#define TRIGGER_BATTLELIST_MONSTER      0x08000
#define TRIGGER_BATTLELIST_GM           0x10000
#define TRIGGER_BATTLELIST_PLAYER       0x20000
#define TRIGGER_BATTLELIST_LIST         0x40000
#define TRIGGER_OUTOF_FOOD				0x080000
#define TRIGGER_OUTOF_SPACE				0x100000
#define TRIGGER_OUTOF_CUSTOM			0x200000
#define TRIGGER_SKULL					0x400000
	#define SKULL_NONE					0x0001
	#define SKULL_YELLOW				0x0002
	#define SKULL_GREEN					0x0004
	#define SKULL_WHITE					0x0008
	#define SKULL_RED					0x0010
	#define SKULL_BLACK					0x0020
#define TRIGGER_VIP						0x800000
#define TRIGGER_STAMINA_BELOW			0x1000000
#define TRIGGER_STAMINA_ABOVE			0x2000000



#define ACTION_NONE				0x00
#define ACTION_SUSPEND			0x01
#define ACTION_LOGOUT			0x02
#define ACTION_KILL				0x04
#define ACTION_SHUTDOWN			0x08
#define ACTION_RUNAWAY			0x10
#define ACTION_RUNAWAY_BACK			0x20
#define ACTION_RUNAWAY_CAVEBOT_HALFSLEEP			0x40
#define ACTION_RUNAWAY_CAVEBOT_FULLSLEEP			0x80

#define ACTION_NONE_POS			0
#define ACTION_SUSPEND_POS		1
#define ACTION_LOGOUT_POS		2
#define ACTION_KILL_POS			3
#define ACTION_SHUTDOWN_POS		4
#define ACTION_RUNAWAY_POS		5
#define ACTION_RUNAWAY_BACK_POS		6
#define ACTION_RUNAWAY_CAVEBOT_HALFSLEEP_POS		7
#define ACTION_RUNAWAY_CAVEBOT_FULLSLEEP_POS		8

#define DIR_LEFT	1
#define DIR_RIGHT	2
#define DIR_UP		3
#define DIR_DOWN	4





class CMod_autogoApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
{
// Construction
public:
	char status[201];
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	CConfigDialog(CMod_autogoApp *app,CWnd* pParent = NULL);   // standard constructor
	int m_DialogID[2];
	MyDialog *m_Dialog[2];
	int m_nPageCount;

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CXTabCtrl	m_tabCtrl;
	CStatic	m_status;
	CButtonST	m_enable;
	CButtonST	m_OK;
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int	m_triggerMessage;
	CMod_autogoApp * m_app;
	char memWhiteList[100][32];
	int memMkBlack;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
