#if !defined(AFX_HEALLIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
#define AFX_HEALLIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HealList.h : header file
//
#include "MyDialog.h"
#include "BtnST.h"
/////////////////////////////////////////////////////////////////////////////
// CHealList dialog

class CHealList : public MyDialog
{
// Construction
public:
	std::vector<Player> healList;
	CHealList(std::vector<Player> healListParam, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHealList)
	enum { IDD = IDD_HEALLIST };
	CListCtrl	m_list;
	CEdit	m_triggerHP;
	CGroupBoxEx	m_healListFrame;
	CButtonST	m_OK;
	CEdit	m_name;
	CEdit	m_maxHP;
	CButtonST	m_delete;
	CButtonST	m_add;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHealList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHealList)
	afx_msg void OnHealListAdd();
	afx_msg void OnHealListDelete();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Mem2List();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEALLIST_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
