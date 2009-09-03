#if !defined(AFX_TOOLITEMCONFIG_H__919303F6_84A9_4D93_834F_7F87C4E0AE47__INCLUDED_)
#define AFX_TOOLITEMCONFIG_H__919303F6_84A9_4D93_834F_7F87C4E0AE47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolItemConfig.h : header file
//

#include "ItemConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig dialog

class CToolItemConfig : public CDialog
{
// Construction
public:
	CToolItemConfig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolItemConfig)
	enum { IDD = IDD_TOOL_ITEMCONFIG };
	CTreeCtrl	m_testTree;
	CListBox	m_lootedList;
	CListBox	m_itemsList;
	CListBox	m_foodList;
	CListBox	m_corpsesList;
	CButton		m_addItem;
	CButton		m_editItem;
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
	char *parseNameFromItemSelected(int type);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLITEMCONFIG_H__919303F6_84A9_4D93_834F_7F87C4E0AE47__INCLUDED_)
