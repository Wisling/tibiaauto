#if !defined(AFX_DONATIONDIALOG_H__1399C346_306D_46E6_90F1_97CDAE9E2FBB__INCLUDED_)
#define AFX_DONATIONDIALOG_H__1399C346_306D_46E6_90F1_97CDAE9E2FBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DonationDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDonationDialog dialog

class CDonationDialog : public CDialog
{
// Construction
public:
	CDonationDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDonationDialog)
	enum { IDD = IDD_DONATION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDonationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDonationDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DONATIONDIALOG_H__1399C346_306D_46E6_90F1_97CDAE9E2FBB__INCLUDED_)
