#if !defined(AFX_WHITELIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
#define AFX_WHITELIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WhiteList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWhiteList dialog

class CWhiteList : public CDialog
{
// Construction
public:
	char* whiteList;
	CWhiteList(char whiteListParam[100][32], CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWhiteList)
	enum { IDD = IDD_WHITELIST };
	CEdit	m_name;
	CListBox	m_list;
	CButton	m_delete;
	CButton	m_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhiteList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWhiteList)
	afx_msg void OnWhitelistAdd();
	afx_msg void OnWhitelistDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Mem2List();
	void ListToMem();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WHITELIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
