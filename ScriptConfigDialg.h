#if !defined(AFX_SCRIPTCONFIGDIALG_H__D11DB0EB_B5B8_4115_B6DF_0A2BDD4B77B3__INCLUDED_)
#define AFX_SCRIPTCONFIGDIALG_H__D11DB0EB_B5B8_4115_B6DF_0A2BDD4B77B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptConfigDialg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScriptConfigDialg dialog

class CScriptConfigDialg : public CDialog
{
// Construction
public:
	CScriptConfigDialg(int scriptNr,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScriptConfigDialg)
	enum { IDD = IDD_SCRIPT_CONFIG };
	CListCtrl	m_list;
	CEdit	m_edit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptConfigDialg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScriptConfigDialg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickScriptConfigParamList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void saveParamValue(int paramNr);
	int scriptNr;
	int lastParamNr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTCONFIGDIALG_H__D11DB0EB_B5B8_4115_B6DF_0A2BDD4B77B3__INCLUDED_)
