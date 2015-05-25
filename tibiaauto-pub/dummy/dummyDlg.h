// dummyDlg.h : header file
//

#if !defined(AFX_DUMMYDLG_H__C4D7A80D_171A_4FD5_80B0_6113BA1874B2__INCLUDED_)
#define AFX_DUMMYDLG_H__C4D7A80D_171A_4FD5_80B0_6113BA1874B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDummyDlg dialog

class CDummyDlg : public CDialog
{
// Construction
public:
	CDummyDlg(CWnd* pParent = NULL);        // standard constructor

// Dialog Data
	//{{AFX_DATA(CDummyDlg)
	enum { IDD = IDD_DUMMY_DIALOG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDummyDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDummyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMYDLG_H__C4D7A80D_171A_4FD5_80B0_6113BA1874B2__INCLUDED_)
