#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_fluid.h"
#include "ConfigData.h"
#include "GroupBoxEx.h"
#include "BtnST.h"
#include "MyDialog.h"

class CMod_fluidApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public MyDialog
{
// Construction
public:
	void reloadCustomItems();
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	void DoSetButtonSkin();
	CConfigDialog(CMod_fluidApp *app, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CGroupBoxEx m_GeneralOptionsFrame;
	CGroupBoxEx m_ManaRecoveryFrame;
	CGroupBoxEx m_HealthRecoveryFrame;
	CEdit m_manaBelowS;
	CEdit m_manaBelowN;
	CEdit m_manaBelowG;
	CEdit m_hpBelowH;
	CEdit m_hpBelowS;
	CEdit m_hpBelowN;
	CEdit m_hpBelowG;
	CEdit m_hpBelowU;
	CButton m_drinkManaS;
	CButton m_drinkManaN;
	CButton m_drinkManaG;
	CButton m_drinkHpH;
	CButton m_drinkHpS;
	CButton m_drinkHpN;
	CButton m_drinkHpG;
	CButton m_drinkHpU;
	CButton m_customItem2Use;
	CEdit m_customItem2Below;
	CComboBox m_customItem2List;
	CButton m_customItem1Use;
	CEdit m_customItem1Below;
	CComboBox m_customItem1List;
	CButton m_dropEmpty;
	CEdit m_sleep;
	CEdit m_manaBelow;
	CEdit m_hpBelow;
	CEdit m_flaskMoreThan;
	CButton m_drinkMana;
	CButton m_drinkHp;
	CButton m_randomCast;
	CButton m_useHotkey;
	CButtonST m_OK;
	CButtonST m_enable;
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
	CMod_fluidApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
