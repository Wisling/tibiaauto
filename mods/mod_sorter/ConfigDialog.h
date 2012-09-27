#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_sorter.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "BtnST.h"
#include "GroupBoxEx.h"

class CMod_sorterApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
{
// Construction
public:
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	void DoSetButtonSkin();
	CConfigDialog(CMod_sorterApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CGroupBoxEx	m_Bag8Frame;
	CGroupBoxEx	m_Bag7Frame;
	CGroupBoxEx	m_Bag6Frame;
	CGroupBoxEx	m_Bag5Frame;
	CGroupBoxEx	m_Bag4Frame;
	CGroupBoxEx	m_Bag3Frame;
	CGroupBoxEx	m_Bag2Frame;
	CGroupBoxEx	m_Bag1Frame;
	CComboBox	m_sortItemList;
	CButtonST		m_enable;
	CButtonST	m_OK;
	//}}AFX_DATA
	CButtonST		m_BagIn[8];
	CButtonST		m_BagOut[8];
	CListBox	m_Bag[8];

	void reloadSortItems();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDialog)
	virtual void OnOK();
	afx_msg void OnClose();
	afx_msg void OnEnable();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnSortEntryAdd1();
	afx_msg void OnSortEntryRemove1();
	afx_msg void OnSortEntryAdd2();
	afx_msg void OnSortEntryRemove2();
	afx_msg void OnSortEntryAdd3();
	afx_msg void OnSortEntryRemove3();
	afx_msg void OnSortEntryAdd4();
	afx_msg void OnSortEntryRemove4();
	afx_msg void OnSortEntryAdd5();
	afx_msg void OnSortEntryRemove5();
	afx_msg void OnSortEntryAdd6();
	afx_msg void OnSortEntryRemove6();
	afx_msg void OnSortEntryAdd7();
	afx_msg void OnSortEntryRemove7();
	afx_msg void OnSortEntryAdd8();
	afx_msg void OnSortEntryRemove8();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CMod_sorterApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
