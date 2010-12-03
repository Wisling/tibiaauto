#if !defined(AFX_GENERALCONFIGDIALOG_H__085305C8_6965_4E43_AFAE_D3D81918831F__INCLUDED_)
#define AFX_GENERALCONFIGDIALOG_H__085305C8_6965_4E43_AFAE_D3D81918831F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeneralConfigDialog.h : header file
//

#include "GroupBoxEx.h"
#include "BtnST.h"
#include "MyDialog.h"

/////////////////////////////////////////////////////////////////////////////
// GeneralConfigDialog dialog

class GeneralConfigDialog : public MyDialog
{
// Construction
public:
	void controlsToConfig(CConfigData *);
	void configToControls(CConfigData *);
	void enableControls();
	void disableControls();
	GeneralConfigDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(GeneralConfigDialog)
	enum { IDD = IDD_GENERAL_CONFIG_DIALOG };
	CStatic	m_runawayPicture;
	CStatic	m_startPicture;
	CButton	m_maintainStart;
	CEdit	m_curY;
	CButton	m_proximityFrame;
	CButton	m_postionFrame;
	CButton	m_messageFrame;
	CButtonST	m_SetStart;
	CButtonST	m_SetRunaway;
	CEdit	m_runawayZ;
	CEdit	m_runawayY;
	CEdit	m_runawayX;
	CEdit	m_curZ;
	CEdit	m_curX;
	CEdit	m_actZ;
	CEdit	m_actY;
	CEdit	m_actX;
	CComboBox	m_actDirection;
	CButton	m_ignoreSpells;
	CButton	m_battleParanoia;
	CButton	m_battleAnxiety;
	CGroupBoxEx m_GeneralConfigFrame;
	CButtonST	m_battleWhiteList;
	int		lastX;
	int		lastY;
	int		lastZ;
	CButton	m_flashOnAlarm;
	CComboBox m_modPriority;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GeneralConfigDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(GeneralConfigDialog)
	afx_msg void OnAutogoWhitelist();
	virtual BOOL OnInitDialog();
	afx_msg void OnAutogoBattleparanoia();
	afx_msg void OnAutogoBattleanxiety();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAutogoTorunaway();
	afx_msg void OnAutogoTostart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString RunawayBMP;
	CString StartBMP;
	char memWhiteList[100][32];
	int memMkBlack;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALCONFIGDIALOG_H__085305C8_6965_4E43_AFAE_D3D81918831F__INCLUDED_)
