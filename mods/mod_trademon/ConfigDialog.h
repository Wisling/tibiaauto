#if !defined(AFX_CONFIGDIALOG_H__8B4F2B48_CD8B_4829_B028_78578EE0D55E__INCLUDED_)
#define AFX_CONFIGDIALOG_H__8B4F2B48_CD8B_4829_B028_78578EE0D55E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_trademon.h"
#include "ConfigData.h"

class CMod_trademonApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public CDialog
{
// Construction
public:
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	CConfigDialog(CMod_trademonApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CButton	m_channel;
	CEdit	m_channelInterval;
	CButton	m_tradeTalker;
	CEdit	m_yellInterval;
	CButton	m_yell;
	CButton	m_tradeMon;
	CEdit	m_sayInterval;
	CButton	m_say;
	CEdit	m_message;
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
	virtual BOOL OnInitDialog();
	afx_msg void OnTradetoolChannel();
	afx_msg void OnTradetoolSay();
	afx_msg void OnTradetoolYell();
	afx_msg void OnTradetoolTradetalk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CMod_trademonApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__8B4F2B48_CD8B_4829_B028_78578EE0D55E__INCLUDED_)
