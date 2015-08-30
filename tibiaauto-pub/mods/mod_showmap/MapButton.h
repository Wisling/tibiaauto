#pragma once
#include <TibiaMap.h>

/////////////////////////////////////////////////////////////////////////////
// CMapButton window

class CMapButton : public CBitmapButton
{
// Construction
public:
	CMapButton(int x, int y);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapButton)
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMapButton();
	MapPointType m_value;
	int m_locked;

private:
	static CMenu * mapActionsMenu;
	int m_x;
	int m_y;
	int realX, realY, realZ;

	// Generated message map functions
protected:
	void OnMouseMove(UINT nFlags, CPoint point);
	//{{AFX_MSG(CMapButton)
	afx_msg void OnClicked();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
