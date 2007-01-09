#if !defined(AFX_CONFIGDIALOG_H__FE1F0F71_AC4E_4432_AAF5_2473A4C04D9C__INCLUDED_)
#define AFX_CONFIGDIALOG_H__FE1F0F71_AC4E_4432_AAF5_2473A4C04D9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_creatureinfo.h"
#include "ConfigData.h"

class CMod_creatureinfoApp;

/////////////////////////////////////////////////////////////////////////////
// CConfigDialog dialog

class CConfigDialog : public CDialog
{
// Construction
public:
	void activateEnableButton(int enable);
	CConfigData * controlsToConfig();
	void configToControls(CConfigData *configData);
	void enableControls();
	void disableControls();
	CConfigDialog(CMod_creatureinfoApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CEdit	m_selfWindow;
	CEdit	m_selfTray;
	CEdit	m_rangeZ;
	CEdit	m_rangeXY;
	CButton	m_showCreaturesInArea;
	CButton	m_collectStats;
	CButton	m_knownInfo;
	CButton	m_addRequest;
	CButton	m_self;
	CButton	m_player;
	CButton	m_monster;
	CButton	m_allFloorInfo;
	CButton	m_uniqueMonsterNames;
	CEdit	m_self2;
	CEdit	m_self1;
	CEdit	m_player2;
	CEdit	m_player1;
	CEdit	m_monster2;
	CEdit	m_monster1;
	CButton	m_enable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDialog)
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
	afx_msg void OnToolcreaturinfoNamechanger();
	afx_msg void OnToolcreaturinfoPlayer();
	afx_msg void OnToolcreaturinfoMonster();
	afx_msg void OnToolcreaturinfoSelf();
	afx_msg void OnToolcreaturinfoKnowninfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CMod_creatureinfoApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__FE1F0F71_AC4E_4432_AAF5_2473A4C04D9C__INCLUDED_)
