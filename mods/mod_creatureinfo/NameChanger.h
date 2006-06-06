#if !defined(AFX_NAMECHANGER_H__1724A70C_82BE_4AB6_8F24_F88A206B2FD0__INCLUDED_)
#define AFX_NAMECHANGER_H__1724A70C_82BE_4AB6_8F24_F88A206B2FD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NameChanger.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNameChanger dialog

class CNameChanger : public CDialog
{
// Construction
public:
	CNameChanger(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNameChanger)
	enum { IDD = IDD_NAME };
	CButton	m_battleOnly;
	CButton	m_advanced;
	CButton	m_relpos;
	CButton	m_refresh;
	CListCtrl	m_list;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameChanger)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNameChanger)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnToolcreaturinfoRefresh();
	afx_msg void OnBeginlabeleditToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolcreaturinfoAdvanced();
	afx_msg void OnDblclkToolcreaturinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitList();
	void RefreshInfo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMECHANGER_H__1724A70C_82BE_4AB6_8F24_F88A206B2FD0__INCLUDED_)
