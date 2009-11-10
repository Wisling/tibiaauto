#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_runemaker.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MyDialog.h"
#include "BtnST.h"
#include "GroupBoxEx.h"

class CMod_runemakerApp;

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
	CConfigDialog(CMod_runemakerApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CGroupBoxEx	m_StatisticsFrame;
	CGroupBoxEx	m_RuneOptionsFrame;
	CGroupBoxEx	m_GeneralOptionsFrame;
	CGroupBoxEx	m_BackpackViewerFrame;
	CButton	m_useBackpack;
	CButton	m_maxUse;
	CButton	m_premium;
	CButton	m_randomCast;
	CEdit	m_manaLimit;
	CEdit	m_mana;
	CListCtrl	m_safe;
	CListCtrl	m_spells;
	CEdit	m_soulPoints;
	CButton	m_useArrow;
	CEdit	m_spell;
	CButton	m_makeTwo;
	CStatic	m_infoRuneTime;
	CStatic	m_infoSoulpointsTime;
	CStatic	m_infoFood;
	CStatic	m_infoBlanks;
	CButtonST m_addSpell;
	CButtonST m_deleteSpell;
	CButtonST m_loadSpell;
	CButtonST m_makeNow;
	CButtonST	m_OK;
	CButtonST	m_enable;
	CButton	m_useSpear;
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
	afx_msg void OnDblclkToolRunemakerCont(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddspelltolist();
	afx_msg void OnLoadspellfromlist();
	afx_msg void OnDeletefromlist();
	afx_msg void OnMakenow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void updateListItem(CListCtrl *list, CTibiaContainer *container, int itemNr);
	void initialiseContainers();
	CMod_runemakerApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
