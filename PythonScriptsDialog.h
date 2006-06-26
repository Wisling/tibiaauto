#if !defined(AFX_PYTHONSCRIPTSDIALOG_H__3EB59214_FBDE_4247_BAB2_8240E5BB835A__INCLUDED_)
#define AFX_PYTHONSCRIPTSDIALOG_H__3EB59214_FBDE_4247_BAB2_8240E5BB835A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PythonScriptsDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPythonScriptsDialog dialog

class CPythonScriptsDialog : public CDialog
{
// Construction
public:	
	CPythonScriptsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPythonScriptsDialog)
	enum { IDD = IDD_PYTHON_SCRIPTS };
	CListCtrl	m_funlist;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPythonScriptsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPythonScriptsDialog)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickFunlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRunInterpreter();
	afx_msg void OnLoadScript();
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void refreshLoadedScripts();
	void refreshModuleFunctions(int scriptNr);
	CMenu *funListMenu;
	CMenu *modListMenu;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PYTHONSCRIPTSDIALOG_H__3EB59214_FBDE_4247_BAB2_8240E5BB835A__INCLUDED_)
