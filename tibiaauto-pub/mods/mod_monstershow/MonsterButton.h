#if !defined(AFX_MONSTERBUTTON_H__FFD71AFB_7139_4133_851E_2CBA642EA8DC__INCLUDED_)
#define AFX_MONSTERBUTTON_H__FFD71AFB_7139_4133_851E_2CBA642EA8DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonsterButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonsterButton window

class CMonsterButton : public CButton
{
// Construction
public:
	CMonsterButton(int x,int y);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonsterButton)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMonsterButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMonsterButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_x;
	int m_y;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONSTERBUTTON_H__FFD71AFB_7139_4133_851E_2CBA642EA8DC__INCLUDED_)
