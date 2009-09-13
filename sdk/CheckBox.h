//
//	File:		CCheckBox.h
//
//	Class:		CCheckBox
//
//
//	Version:	1.0
//
//	Created:	December 2002, January 2003
//	Updated:	11/January/2003
//
//	Compiler:	Visual C++ 6.0
//	Tested on:	Win2000, SP2
//
//
//	Author:		Robert Kuster		robert.kuster@email.si
//	Copyright   (C) 2003
//
// 
//	Disclaimer
//	----------
//	This source code (CCheckBox.h and CCheckBox.cpp) is provided "as is" with NO 
//	expressed or implied warranty. The user must assume the entire risk of using it.
//	No responsibilities for possible damages or even functionality can be taken.
//
//
//	Terms of use
//	------------
//	This source code is free for personal use and freeware applications.  
//	For commercial use a simple email would be nice.
//
/////////////////////////////////////////////////////////////////////////////


#if !defined _CHECK_BOX_MFC_RK_
#define _CHECK_BOX_MFC_RK_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CCheckBox window

class CCheckBox : public CButton
{
public:
	CCheckBox	  ();	// rcID		  : bitmap resource ID;		
												// nDimension : bitmap dimension;
	static bool drawFocus;	  // draw focus rectangle?
	virtual ~CCheckBox();

protected:
	enum {
		BOX_ON = 0,			// Column 0 : checked state
		BOX_OFF = 1,		// Column 1 : unchecked state
		BOX_LDOWN_1	 = 2,	// Column 2 : mouse down in checked state
		BOX_LDOWN_2	 = 3,	// Column 3 : mouse down in unchecked state
		BOX_DISABLED_1 = 4,	// Column 4 : CB disabled in checked state
		BOX_DISABLED_2 = 5	// Column 5 : CB disabled in unchecked state
	};	

	bool m_bLeftText;		// BS_LEFTTEXT || BS_RIGHTBUTTON || WS_EX_RIGHT ??
	bool m_bDisabled;
	bool m_bChecked;
	int  m_nState;			// state of check box: BOX_ON, BOX_OFF,...

	int  BOX_SIZE;		    // size of bitmaps (BOX_SIZE X BOX_SIZE)
	int  m_rcID;			// resource ID (m_bSrc)

	DWORD  m_clr;			// background color == COLOR_3DFACE
	HBRUSH m_brush;			// background brush

//	HBITMAP m_bSrc;			// source bitmap
//	HBITMAP m_bitmaps [6];  // bitmaps for all states BOX_ON, BOX_OFF,...BOX_DISABLED_2
	
	bool inRect;			// mouse cursor over CB?
	bool spaceDown;			// space button pressed?
	bool mouseDown;			// mouse button pressed?


	HFONT m_font;			// control font
	char  m_text [64];		// control text
	
	int mHeight;
	int GetMinHeight (HDC);	// calculates the minimal height 'mHeight'
							// based on m_font

//	bool prepare_bitmaps;

	void SetState     (int nState);         // set state (BOX_ON,BOX_OFF,...BOX_DISABLED_2)

	bool hasFocus;							// has focus?
	RECT rcFocus;							// focus rectangle
	void DrawFocus (bool forceDraw=false);
	

	//{{AFX_VIRTUAL(CCheckBox)
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CCheckBox)
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);	
	//}}AFX_MSG
	afx_msg LRESULT OnGetCheck (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetCheck (WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_CHECK_BOX_MFC_RK_)
