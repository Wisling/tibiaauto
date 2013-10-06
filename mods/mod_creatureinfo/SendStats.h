#if !defined(AFX_SENDSTATS_H__C599E8D1_4397_4E82_B03D_9EB4E430D1EC__INCLUDED_)
#define AFX_SENDSTATS_H__C599E8D1_4397_4E82_B03D_9EB4E430D1EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendStats.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSendStats dialog

class CSendStats : public CDialog
{
// Construction
public:
	CSendStats(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendStats)
	enum { IDD = IDD_SENDSTATS };
		// NOTE: the ClassWizard will add data members here
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
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDSTATS_H__C599E8D1_4397_4E82_B03D_9EB4E430D1EC__INCLUDED_)
