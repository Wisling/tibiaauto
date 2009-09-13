#if !defined(AFX_TOOLITEMCONFIG_H__919303F6_84A9_4D93_834F_7F87C4E0AE47__INCLUDED_)
#define AFX_TOOLITEMCONFIG_H__919303F6_84A9_4D93_834F_7F87C4E0AE47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolItemConfig.h : header file
//

#include "ItemConfig.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig dialog

class CToolItemConfig : public MyDialog
{
// Construction
public:
	CToolItemConfig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolItemConfig)
	enum { IDD = IDD_TOOL_ITEMCONFIG };
	CGroupBoxEx	m_LootFrame;
	CGroupBoxEx	m_ItemsFrame;
	CGroupBoxEx	m_FoodFrame;
	CButtonST	m_OK;
	CButtonST	m_RefreshItems;
	CButtonST	m_EditLoot;
	CButtonST	m_EditItem;
	CButtonST	m_EditFood;
	CButtonST	m_DeleteLoot;
	CButtonST	m_DeleteItem;
	CButtonST	m_DeleteFood;
	CButtonST	m_AddLoot;
	CButtonST	m_AddItem;
	CButtonST	m_AddFood;
	CTreeCtrl	m_testTree;
	CListBox	m_lootedList;
	CListBox	m_itemsList;
	CListBox	m_foodList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolItemConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	char outbuf[64];
	char *parseNameFromItemSelected(int type);
	// Generated message map functions
	//{{AFX_MSG(CToolItemConfig)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnToolItemconfigRefresh();
	virtual BOOL OnInitDialog();
	virtual void OnItemAdd();
	virtual void OnItemEdit();
	virtual void OnItemDelete();
	virtual void OnFoodAdd();
	virtual void OnFoodEdit();
	virtual void OnFoodDelete();
	virtual void OnLootAdd();
	virtual void OnLootEdit();
	virtual void OnLootDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLITEMCONFIG_H__919303F6_84A9_4D93_834F_7F87C4E0AE47__INCLUDED_)
