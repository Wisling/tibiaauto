#if !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
#define AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDialog.h : header file
//

#include "mod_addressfinder.h"
#include "ConfigData.h"
#include "MyDialog.h"
#include "GroupBoxEx.h"
#include "BtnSt.h"
#include "AddressFinder.h"


class CMod_addressfinderApp;

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
	CConfigDialog(CMod_addressfinderApp *app,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigDialog)
	enum { IDD = IDD_CONFIG };
	CStatic	m_staminaAddress;
	CStatic	m_stamina;
	CStatic	m_capacityAddress;
	CStatic	m_capacity;
	CStatic	m_battleListMax;
	CStatic	m_tilesLeftAddress;
	CStatic	m_tilesLeft;
	CStatic	m_soulPointsAddress;
	CStatic	m_soulPoints;
	CStatic	m_selfID;
	CStatic	m_selfIDAddress;
	CStatic	m_manaMaxAddress;
	CStatic	m_manaMax;
	CStatic	m_manaAddress;
	CStatic	m_mana;
	CStatic	m_magicLevelPercentLeft;
	CStatic	m_magicLevelPrecentLeftAddress;
	CStatic	m_magicLevelAddress;
	CStatic	m_magicLevel;
	CStatic	m_levelAddress;
	CStatic	m_level;
	CStatic	m_HPMaxAddress;
	CStatic	m_HPAddress;
	CStatic	m_HP;
	CStatic	m_HPMax;
	CStatic	m_goZAddress;
	CStatic	m_goZ;
	CStatic	m_goYAddress;
	CStatic	m_goY;
	CStatic	m_goX;
	CStatic	m_goXAddress;
	CStatic	m_goToTileAddress;
	CStatic	m_goToTile;
	CStatic	m_flagsAddress;
	CStatic	m_flags;
	CStatic	m_fistSkillAddress;
	CStatic	m_fistSkill;
	CStatic	m_fistPercentLeftAddress;
	CStatic	m_fistPercentLeft;
	CStatic	m_fishSkillAddress;
	CStatic	m_fishSkill;
	CStatic	m_firstCreatureAddress;
	CStatic	m_experienceAddress;
	CStatic	m_firstCreature;
	CStatic	m_experience;
	CStatic	m_battleListMinAddress;
	CStatic	m_battleListMin;
	CStatic	m_battleListMaxAddress;
	CStatic	m_attackedCreatureAddress;
	CStatic	m_attackedCreature;
	CButtonST	m_beginOutput;
	CGroupBoxEx	m_addressOutput;
	CGroupBoxEx	m_AddressFinderFrame;
	CButtonST	m_OK;
	CButtonST	m_enable;
	int		m_outputStartByte;
	int		m_outputEndByte;
	CButtonST		m_searchString;
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
	afx_msg void OnBeginOutput();
	afx_msg void OnCreateSearchString();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	AddressFinder m_addressFinder;
	CMod_addressfinderApp * m_app;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDIALOG_H__A38E1144_4FC4_4130_9D8B_A22DE0571AB2__INCLUDED_)
