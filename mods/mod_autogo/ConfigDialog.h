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
	char* status;
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
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnDestroy();
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
