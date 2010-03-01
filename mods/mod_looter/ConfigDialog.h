#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_looter.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnST.h"

class CMod_looterApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
{
// Construction
public:
	void setButtonSkins();
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	CConfigDialog(CMod_looterApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CButton	m_lootInDepot;
	CGroupBoxEx	m_LootOptionsFrame;
	CGroupBoxEx	m_ContainerSetupFrame;
	CStatic	m_status10;
	CComboBox	m_mode10;
	CButton	m_lootWorms;
	CButton	m_lootGp;
	CButton	m_lootFood;
	CButton	m_lootCustom;
	CButton	m_autoOpen;
	CStatic	m_status9;
	CStatic	m_status8;
	CStatic	m_status7;
	CStatic	m_status6;
	CStatic	m_status5;
	CStatic	m_status4;
	CStatic	m_status3;
	CStatic	m_status2;
	CStatic	m_status1;
	CComboBox	m_mode9;
	CComboBox	m_mode8;
	CComboBox	m_mode7;
	CComboBox	m_mode6;
	CComboBox	m_mode5;
	CComboBox	m_mode4;
	CComboBox	m_mode3;
	CComboBox	m_mode2;
	CComboBox	m_mode1;
	CButtonST	m_enable;
	CButtonST	m_OK;
	//}}AFX_DATA


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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void refreshContainerStatus();
	CMod_looterApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
