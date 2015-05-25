#if !defined(AFX_MAPBUTTON_H__224FA9B2_E673_436E_85EB_8F35553B880D__INCLUDED_)
#define AFX_MAPBUTTON_H__224FA9B2_E673_436E_85EB_8F35553B880D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MapButton.h : header file
//

enum MapPointType
{
	MAP_POINT_TYPE_SELF        = -2,
	MAP_POINT_TYPE_CLEAR       = -1,
	MAP_POINT_TYPE_AVAILABLE   = 0,
	MAP_POINT_TYPE_OPEN_HOLE   = 101,
	MAP_POINT_TYPE_CLOSED_HOLE = 102,
	MAP_POINT_TYPE_CRATE       = 103,
	MAP_POINT_TYPE_ROPE        = 201,
	MAP_POINT_TYPE_MAGICROPE   = 202,
	MAP_POINT_TYPE_LADDER      = 203,
	MAP_POINT_TYPE_STAIRS      = 204,
	MAP_POINT_TYPE_DEPOT       = 301,
	MAP_POINT_TYPE_TELEPORT    = 302,
	MAP_POINT_TYPE_BLOCK       = 303,
};

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

#endif // !defined(AFX_MAPBUTTON_H__224FA9B2_E673_436E_85EB_8F35553B880D__INCLUDED_)
