#if !defined(AFX_ENTERCODE_H__8FACC4CE_5E2F_4E72_8848_D8E377B54C5B__INCLUDED_)
#define AFX_ENTERCODE_H__8FACC4CE_5E2F_4E72_8848_D8E377B54C5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnterCode.h : header file
//

#include "TibiaautoDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CEnterCode dialog

class CEnterCode : public CDialog
{
// Construction
public:
	int auth();
	CEnterCode(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnterCode)
	enum { IDD = IDD_ENTERCODE };
	CEdit	m_runtimeId;
	CEdit	m_code;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterCode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnterCode)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CTibiaautoDlg * parent;
	int randTab[8];	
	int testsum;
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTERCODE_H__8FACC4CE_5E2F_4E72_8848_D8E377B54C5B__INCLUDED_)
