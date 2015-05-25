#if !defined(AFX_ITEMCONFIG_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
#define AFX_ITEMCONFIG_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemConfig.h : header file
//
#include "resource.h"
#include "MyDialog.h"
#include "BtnST.h"
#include "ComboBoxSuper.h"

/////////////////////////////////////////////////////////////////////////////
// CItemAdd dialog

class CItemAdd : public MyDialog {
// Construction
public:
	char name[1024];
	int itemId;
	int itemType;
	CTreeCtrl* tree;
	HTREEITEM item;
	CItemAdd(CTreeCtrl* treeIn, HTREEITEM itemIn);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CItemAdd)
	enum { IDD = IDD_ITEM_ADD };
	CComboBoxSuper m_ItemType;
	CButtonST m_OK;
	CButtonST m_Cancel;
	CEdit m_ItemName;
	CEdit m_ItemID;
	CButton m_ItemBranch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemAdd)
protected:
	virtual void OnCommit();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_itemImg;

	// Generated message map functions
	//{{AFX_MSG(CItemAdd)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddBranch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////
// CItemEdit dialog

class CItemEdit : public MyDialog {
// Construction
public:
	char name[1024];
	int itemId;
	int itemType;
	CTreeCtrl* tree;
	HTREEITEM item;
	CItemEdit(CTreeCtrl* treeIn, HTREEITEM itemIn);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CItemEdit)
	enum { IDD = IDD_ITEM_EDIT };
	CComboBoxSuper m_ItemType;
	CButtonST m_OK;
	CButtonST m_Cancel;
	CEdit m_ItemName;
	CEdit m_ItemID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemEdit)
protected:
	virtual void OnCommit();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_itemImg;

	// Generated message map functions
	//{{AFX_MSG(CItemEdit)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////
// CFoodAdd dialog

class CFoodAdd : public MyDialog
{
// Construction
public:
	char name[512];
	int itemId;
	int eatTime;
	CListBox* list;
	CFoodAdd(CListBox*);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFoodAdd)
	enum { IDD = IDD_FOOD_ADD };
	CButtonST m_OK;
	CButtonST m_Cancel;
	CEdit m_ItemName;
	CEdit m_ItemID;
	CEdit m_EatTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFoodAdd)
protected:
	virtual void OnCommit();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFoodAdd)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////
// CFoodEdit dialog

class CFoodEdit : public MyDialog
{
// Construction
public:
	char name[512];
	int itemId;
	int eatTime;
	CListBox* list;
	int index;
	CFoodEdit(int, char*, int, CListBox*, int);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFoodEdit)
	enum { IDD = IDD_FOOD_EDIT };
	CButtonST m_OK;
	CButtonST m_Cancel;
	CEdit m_ItemName;
	CEdit m_ItemID;
	CEdit m_EatTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFoodEdit)
protected:
	virtual void OnCommit();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFoodEdit)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMCONFIG_H__2B7D4FF7_3CD1_48AC_9D96_98AFC9020887__INCLUDED_)
