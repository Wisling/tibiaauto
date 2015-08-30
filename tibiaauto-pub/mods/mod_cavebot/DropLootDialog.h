#if !defined(AFX_DROPLOOTDIALOG_H__2038F219_52A8_4DF9_8CB3_1DD8F184260B__INCLUDED_)
#define AFX_DROPLOOTDIALOG_H__2038F219_52A8_4DF9_8CB3_1DD8F184260B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DropLootDialog.h : header file
//
#include "MyDialog.h"
#include "BtnSt.h"

/////////////////////////////////////////////////////////////////////////////
// DropLootDialog dialog

class CDropLootDialog : public MyDialog
{
// Construction
public:
	CDropLootDialog(char dropListParam[MAX_DROPLISTCOUNT][64], int& dropListCountParam, int& dropWhenCapacityLimitReachedParam, int& dropOnlyLootedParam, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(DropLootDialog)
	enum { IDD = IDD_DROPLIST };
	CGroupBoxEx m_DropLootFrame;
	CComboBox m_itemList;
	CListBox m_dropList;
	CButton m_dropWhenCapacityLimitReached;
	CButton m_dropOnlyLooted;
	CButtonST m_add;
	CButtonST m_delete;
	CButtonST m_OK;
	CButtonST m_cancel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DropLootDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char* dropList;
	int* dropListCount;
	int* dropWhenCapacityLimitReached;
	int* dropOnlyLooted;

	// Generated message map functions
	//{{AFX_MSG(DropLootDialog)
	afx_msg void OnDroplistAdd();
	afx_msg void OnDroplistDelete();
	afx_msg void OnDroplistDropOnlyLooted();
	afx_msg void OnDroplistDropWhenCapacityLimitReached();
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void LoadItemList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DROPLOOTDIALOG_H__2038F219_52A8_4DF9_8CB3_1DD8F184260B__INCLUDED_)
