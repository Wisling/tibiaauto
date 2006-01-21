#if !defined(AFX_CONFIGDIALOGSTATUS_H__86FABF49_AFAE_4B1E_9377_4FBA69E686C0__INCLUDED_)
#define AFX_CONFIGDIALOGSTATUS_H__86FABF49_AFAE_4B1E_9377_4FBA69E686C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialogStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigDialogStatus dialog

#define MY_WM_MESSAGE_ADDTOLOG  (WM_APP + 1)

class CConfigDialogStatus : public CDialog
{
// Construction
public:
	void msgAddToLog(char *message);
	void enableOk();
	void disableOk();
	void addToLog(char *message);
	void resetLog();
	CConfigDialogStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialogStatus)
	enum { IDD = IDD_CONFIGSTATUS };
	CButton	m_ok;
	CListCtrl	m_log;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDialogStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDialogStatus)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
    afx_msg LRESULT OnMyMessageAddToLog(UINT wParam, LONG lParam);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOGSTATUS_H__86FABF49_AFAE_4B1E_9377_4FBA69E686C0__INCLUDED_)
