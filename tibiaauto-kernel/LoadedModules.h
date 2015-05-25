#if !defined(AFX_LOADEDMODULES_H__6B612E74_C0DC_4149_BAF7_B317FF69CA1B__INCLUDED_)
#define AFX_LOADEDMODULES_H__6B612E74_C0DC_4149_BAF7_B317FF69CA1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadedModules.h : header file
//
#include "MyDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CLoadedModules dialog

class CLoadedModules : public MyDialog
{
// Construction
public:
	void DoSetButtonSkin();
	CLoadedModules(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadedModules();
// Dialog Data
	//{{AFX_DATA(CLoadedModules)
	enum { IDD = IDD_LOADED_MODULES };
	CButtonST m_ok;
	CListCtrl m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadedModules)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadedModules)
	afx_msg void OnClose();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRclickLoadedmodulesList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void refreshModulesInformation();
	CMenu * moduleActionsMenu;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADEDMODULES_H__6B612E74_C0DC_4149_BAF7_B317FF69CA1B__INCLUDED_)
