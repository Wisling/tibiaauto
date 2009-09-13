#if !defined(AFX_OPTIONSDIALOG_H__2DDA3824_BDCE_4603_AA0A_8BD1BB5A60D1__INCLUDED_)
#define AFX_OPTIONSDIALOG_H__2DDA3824_BDCE_4603_AA0A_8BD1BB5A60D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDialog.h : header file
//

#include "MyDialog.h"
#include "GroupBoxEx.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog

class COptionsDialog : public MyDialog
{
// Construction
public:
	COptionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDialog)
	enum { IDD = IDD_OPTIONS };
	CButtonST	m_Skin;
	CGroupBoxEx	m_UsageStatistics;
	CGroupBoxEx	m_TibiaMaps;
	CGroupBoxEx	m_LootStatistics;
	CGroupBoxEx	m_CreatureStatistics;
	CStatic	m_sizeUsagestats;
	CButtonST	m_send4;
	CButtonST	m_send3;
	CButtonST	m_send2;
	CButtonST	m_send1;
	CProgressCtrl	m_progress;
	CButtonST	m_ok;
	CStatic	m_sizeCreatureStats;
	CStatic	m_sizeLootstats;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDialog)
	afx_msg void OnSendLootstats();
	virtual BOOL OnInitDialog();
	afx_msg void OnSendCreaturestats();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnSendMaps();
	afx_msg void OnSendUsagestats();
	afx_msg void OnSkin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void sendMaps(char *path);
	void sendFile(char *fname);
	void refreshStatFiles();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDIALOG_H__2DDA3824_BDCE_4603_AA0A_8BD1BB5A60D1__INCLUDED_)
