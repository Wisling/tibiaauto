#if !defined(AFX_AOEDIALOG_H__DA58E0D7_F60C_422C_A441_2D764FC8C47A__INCLUDED_)
#define AFX_AOEDIALOG_H__DA58E0D7_F60C_422C_A441_2D764FC8C47A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyDialog.h"
#include "GroupBoxEx.h"
// AOEDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AOEDialog dialog

class AOEDialog : public MyDialog
{
// Construction
public:
	void controlsToConfig(CConfigData *);
	void configToControls(CConfigData *);
	void enableControls();
	void disableControls();
	AOEDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AOEDialog)
	enum { IDD = IDD_AOE_DIALOG };
	CButton m_exoriMas;
	CButton m_exoriGran;
	CButton m_exori;
	CButton m_exevoVisLux;
	CButton m_exevoTeraHur;
	CButton m_exevoVisHur;
	CButton m_exevoMasSan;
	CButton m_exevoGranVisLux;
	CButton m_exevoGranMasVis;
	CButton m_exevoGranMasTera;
	CButton m_exevoGranMasFrigo;
	CButton m_exevoGranMasFlam;
	CButton m_exevoFrigoHur;
	CButton m_exevoFlamHur;
	CEdit m_aoeAffect;
	CButton m_aoe;
	CGroupBoxEx m_SorcererAOEFrame;
	CGroupBoxEx m_PaladinAOEFrame;
	CGroupBoxEx m_KnightAOEFrame;
	CGroupBoxEx m_DruidAOEFrame;
	CGroupBoxEx m_AOESpellsFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AOEDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AOEDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnToolSpellcasterAOE();
	afx_msg void OnToolSpellcasterSorcererAOE();
	afx_msg void OnToolSpellcasterDruidAOE();
	afx_msg void OnToolSpellcasterPaladinAOE();
	afx_msg void OnToolSpellcasterKnightAOE();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AOEDIALOG_H__DA58E0D7_F60C_422C_A441_2D764FC8C47A__INCLUDED_)
