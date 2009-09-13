#if !defined(AFX_WHITELIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
#define AFX_WHITELIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WhiteList.h : header file
//
#include "MyDialog.h"
#include "BtnSt.h"
/////////////////////////////////////////////////////////////////////////////
// CWhiteList dialog

class CWhiteList : public MyDialog
{
// Construction
public:
	char* whiteList;
	int * mkBlack;
	CWhiteList(char whiteListParam[100][32], int * mkBlackParam, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWhiteList)
	enum { IDD = IDD_WHITELIST };
	CGroupBoxEx	m_WhiteListFrame;
	CEdit	m_name;
	CListBox	m_list;
	CButton	m_mkblack;
	CButtonST	m_delete;
	CButtonST	m_add;
	CButtonST	m_OK;
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
	afx_msg void OnWhitelistMkBlack();
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
