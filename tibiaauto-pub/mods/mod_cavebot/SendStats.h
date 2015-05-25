#if !defined(AFX_SENDSTATS_H__C36E8DD4_22AC_4489_AE02_2677F256E082__INCLUDED_)
#define AFX_SENDSTATS_H__C36E8DD4_22AC_4489_AE02_2677F256E082__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsDialog.h : header file
//
#include "MyDialog.h"
#include "BtnST.h"
/////////////////////////////////////////////////////////////////////////////
// CSendStats dialog

class CSendStats : public MyDialog
{
// Construction
public:
	CSendStats(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendStats)
	enum { IDD = IDD_SENDSTATS };
	CButtonST m_send2;
	CButtonST m_send1;
	CButtonST m_ok;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendStats)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendStats)
	afx_msg void OnSendLootstats();
	virtual BOOL OnInitDialog();
	afx_msg void OnSendCreaturestats();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void sendFile(char *fname);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDSTATS_H__C36E8DD4_22AC_4489_AE02_2677F256E082__INCLUDED_)
