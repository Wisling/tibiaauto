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

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CToolItemConfig dialog

class CToolItemConfig : public MyDialog
{
// Construction
public:
	CToolItemConfig(CWnd* pParent = NULL);   // standard constructor
	void ConfigToControls();
	void ControlsToConfig();

	//For dragging
	HTREEITEM itemOrigin;
	TVHITTESTINFO htDest;
	int insertAfter;
	CPoint offsetPt;
	CPoint lastInsertMark;
	bool Dragging;

	//For Copy, Cut and Paste
	int actionIndicator;
	HTREEITEM actionItem;

	CImageList* checkImgList;

// Dialog Data
	//{{AFX_DATA(CToolItemConfig)
	enum { IDD = IDD_TOOL_ITEMCONFIG };
	CTreeCtrl	m_itemsTree;
	CButtonST	m_AddChild;
	CGroupBoxEx	m_ItemsFrame;
	CGroupBoxEx	m_FoodFrame;
	CButtonST	m_OK;
	CButtonST	m_Cancel;
	CButtonST	m_RefreshItems;
	CButtonST	m_DeleteFood;
	CButtonST	m_EditFood;
	CButtonST	m_AddFood;
	CButtonST	m_DeleteItem;
	CButtonST	m_EditItem;
	CButtonST	m_AddItem;
	CButtonST	m_itemSort;
	CButtonST	m_helpInfo;
	CButtonST	m_selectedToBranch;
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
	char *parseName(char*);
	int parseId(char*);
	void CancelTwoStepOperations();
	// Generated message map functions
	//{{AFX_MSG(CToolItemConfig)
	afx_msg void OnClose();
	afx_msg void OnToolItemconfigRefresh();
	virtual BOOL OnInitDialog();
	virtual void OnItemEdit();
	virtual void OnItemDelete();
	virtual void OnFoodAdd();
	virtual void OnFoodEdit();
	virtual void OnFoodDelete();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnAddItem();
	afx_msg void OnBegindragTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemSort();
	afx_msg void OnKeydownTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelpInfo();
	afx_msg void OnSelectedToBranch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLITEMCONFIG_H__919303F6_84A9_4D93_834F_7F87C4E0AE47__INCLUDED_)
