#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_restack.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnST.h"

class CMod_restackApp;

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
	CConfigDialog(CMod_restackApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CGroupBoxEx	m_ThrowablesFrame;
	CGroupBoxEx	m_PickupAreaFrame;
	CGroupBoxEx	m_OptionsFrame;
	CGroupBoxEx	m_AmmunitionFrame;
	CEdit	m_capLimit;
	CButton	m_pickupCC;
	CEdit	m_periodTo;
	CEdit	m_periodFrom;
	CButton	m_pickupToHand;
	CButton	m_restackToRight;
	CButton	m_moveCovering;
	CButton	m_pickupUR;
	CButton	m_pickupUL;
	CButton	m_pickupUC;
	CButton	m_pickupCR;
	CButton	m_pickupCL;
	CButton	m_pickupBR;
	CButton	m_pickupBL;
	CButton	m_pickupBC;
	CButton	m_pickupSpears;
	CEdit	m_throwableTo;
	CEdit	m_throwableAt;
	CComboBox	m_throwableType;	
	CEdit	m_ammoTo;
	CEdit	m_ammoAt;
	CComboBox	m_ammoType;
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
	CMod_restackApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
