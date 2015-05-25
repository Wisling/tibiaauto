#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_banker.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnSt.h"

struct banker {
	char name[40];
	int xPos;
	int yPos;
	int zPos;
};

enum CToolBankerState {
	CToolBankerState_notRunning,
	CToolBankerState_halfSleep,
	CToolBankerState_noPathFound,
	CToolBankerState_walking,
	CToolBankerState_talking
};
class CMod_bankerApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
{
// Construction
public:
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	void DoSetButtonSkin();
	CConfigDialog(CMod_bankerApp *app, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CGroupBoxEx m_BankerFrame;
	CComboBox m_Banker;
	CEdit m_MinGold;
	CEdit m_OnHand;
	CComboBox m_modPriority;
	CButton m_changeGold;
	CEdit m_capsLimit;
	CButton m_stopByBanker;
	CButton m_drawUpTo;
	CButtonST m_OK;
	CButtonST m_enable;
	CStatic m_stateBanker;
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
	afx_msg void OnChangeGold();
	afx_msg void OnEnable();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void reloadBankers();
	CMod_bankerApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
