#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_spellcaster.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "BtnST.h"
#include "GroupBoxEx.h"
#include "XTabCtrl.h"

class CMod_spellcasterApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
{
// Construction
public:
//	virtual void controlsToConfig(CConfigData *) {};
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	void DoSetButtonSkin();
	CConfigDialog(CMod_spellcasterApp *app, CWnd* pParent = NULL);   // standard constructor
	//Array to hold the list of dialog boxes/tab pages for CTabCtrl
	int m_DialogID[6];

	//CDialog Array Variable to hold the dialogs
	MyDialog *m_Dialog[6];

	int m_nPageCount;
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CXTabCtrl m_tabCtrl;
	CButton m_disableWarning;
	CButton m_randomCast;
	CButtonST m_enable;
	CButtonST m_OK;
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
	CMod_spellcasterApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
