// CheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "CheckBox.h"
#include "Skin.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// //
// Initialisation of static members

bool CCheckBox::drawFocus = true;


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// //
// CCheckBox construction/destruction

//-----------------------------------------------------------------------------
// Name:		CCheckBox
// Description:	Constructor
//
// Entry:
//				int rcID		- resource ID with state bitmaps
//
//				int dimension	== width == height of a state bitmap
//-----------------------------------------------------------------------------
CCheckBox::CCheckBox ()
{
	// load state bitmaps
	//
	m_bLeftText = false;
	m_bDisabled = false;
	m_bChecked  = false;
	m_nState    = BOX_OFF;

	inRect    = false;
	spaceDown = false;
	mouseDown = false;

	m_brush = NULL;

	hasFocus = false;

//	prepare_bitmaps = true;

//	for (int i=0; i<6; i++)
//		m_bitmaps [i] = 0;
}

//-----------------------------------------------------------------------------
// Name:		~CCheckBox
// Description:	Destructor
//-----------------------------------------------------------------------------
CCheckBox::~CCheckBox()
{
	DeleteObject (m_font);
}


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// //
// CCheckBox overrides

//-----------------------------------------------------------------------------
// Name:		PreSubclassWindow
// Description:	Makes some initialisations before the actuall
//				subclassing occurs
//-----------------------------------------------------------------------------
void CCheckBox::PreSubclassWindow()
{
	// text on left side?
	//
	unsigned style    = GetStyle   ();
	unsigned ex_style = GetExStyle ();

	if ((style & BS_LEFTTEXT) ||
	    (style & BS_RIGHTBUTTON) ||
	    (ex_style & WS_EX_RIGHT) )
		m_bLeftText = true;


	GetWindowText (m_text, 64);                                                                             // get text
	m_font = (HFONT) GetParent()->SendMessage (WM_GETFONT, 0, 0);   // get font


	if (!IsWindowEnabled ())
	{
		m_bDisabled = true;
		m_nState    = BOX_DISABLED_2;
	}

	// prevent any drawing by the control itself
	//
	ModifyStyle (0, BS_OWNERDRAW, 0);


	CButton::PreSubclassWindow();
}

//-----------------------------------------------------------------------------
// Name:		PreTranslateMessage
// Description:	WM_LBUTTONDBLCLK => WM_LBUTTONDOWN
//					- to ensure proper state repainting of the control
//					- to prevent BN_DBLCLK notifications to the parent
//					  (BS_OWNERDRAW buttons send them, but CBs don't)
//-----------------------------------------------------------------------------
BOOL CCheckBox::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
}


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// //
// CCheckBox message handlers

BEGIN_MESSAGE_MAP(CCheckBox, CButton)
//{{AFX_MSG_MAP(CCheckBox)
ON_WM_ENABLE()
ON_WM_KEYDOWN()
ON_WM_KEYUP()
ON_WM_LBUTTONDBLCLK()
ON_WM_LBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_LBUTTONUP()
ON_WM_SETFOCUS()
ON_WM_KILLFOCUS()
//}}AFX_MSG_MAP
ON_MESSAGE (BM_GETCHECK, OnGetCheck)
ON_MESSAGE (BM_SETCHECK, OnSetCheck)
END_MESSAGE_MAP()


//-----------------------------------------------------------------------------
// Name:		OnEnable
// Description:	Enable or disable CB
//-----------------------------------------------------------------------------
void CCheckBox::OnEnable(BOOL bEnable)
{
	if (bEnable)
	{
		m_bDisabled = false;

		if (m_nState == BOX_DISABLED_1)
			SetState (BOX_ON);
		else
			SetState (BOX_OFF);
	}
	else
	{
		m_bDisabled = true;

		if (m_bChecked)
			SetState (BOX_DISABLED_1);
		else
			SetState (BOX_DISABLED_2);
	}

	CButton::OnEnable(bEnable);
}


//-----------------------------------------------------------------------------
// Name:		OnKeyDown
//-----------------------------------------------------------------------------
void CCheckBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE && !spaceDown)
	{
		spaceDown = true;

		if (!mouseDown)
		{
			if (m_bChecked)
				SetState (BOX_LDOWN_1);
			else
				SetState (BOX_LDOWN_2);
		}
	}
	else if (nChar != VK_SPACE && spaceDown)        // if any other key pressed
	{                                                                                       // while space down
		spaceDown = false;

		if (!mouseDown)
		{
			if (m_bChecked)
				SetState (BOX_ON);
			else
				SetState (BOX_OFF);
		}
	}

	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}


//-----------------------------------------------------------------------------
// Name:		OnKeyUp
//-----------------------------------------------------------------------------
void CCheckBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE && spaceDown)
	{
		spaceDown = false;

		if (!mouseDown)
		{
			m_bChecked = (m_bChecked == true ? false : true);

			if (m_bChecked)
				SetState (BOX_ON);
			else
				SetState (BOX_OFF);
		}
	}

	CButton::OnKeyUp(nChar, nRepCnt, nFlags);
}


//-----------------------------------------------------------------------------
// Name:		OnLButtonDown
//-----------------------------------------------------------------------------
void CCheckBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	mouseDown = true;
	inRect    = true;

	if (m_bChecked)
		SetState (BOX_LDOWN_1);
	else
		SetState (BOX_LDOWN_2);

	CButton::OnLButtonDown(nFlags, point);  // also calls "SetCapture" & "SetFocus"
}


//-----------------------------------------------------------------------------
// Name:		OnMouseMove
//-----------------------------------------------------------------------------
void CCheckBox::OnMouseMove(UINT nFlags, CPoint pt)
{
	RECT rc;
	GetClientRect (&rc);

	if (mouseDown)
	{
		if (!PtInRect (&rc, pt))
		{
			if (m_bChecked)
				SetState (BOX_ON);
			else
				SetState (BOX_OFF);

			inRect = false;
		}
		else if (!inRect)  // again over control
		{
			inRect = true;

			if (m_bChecked)
				SetState (BOX_LDOWN_1);
			else
				SetState (BOX_LDOWN_2);
		}
	}

	CButton::OnMouseMove(nFlags, pt);
}


//-----------------------------------------------------------------------------
// Name:		OnLButtonUp
//-----------------------------------------------------------------------------
void CCheckBox::OnLButtonUp(UINT nFlags, CPoint pt)
{
	mouseDown = false;

	RECT rc;
	GetClientRect (&rc);

	if (PtInRect (&rc, pt))
	{
		m_bChecked = (m_bChecked == true ? false : true);

		if (!spaceDown)
		{
			if (m_bChecked)
				SetState (BOX_ON);
			else
				SetState (BOX_OFF);
		}
		else
		{
			if (m_bChecked)
				SetState (BOX_LDOWN_1);
			else
				SetState (BOX_LDOWN_2);
		}
	}
	else if (spaceDown)  // mouse released outside of control, but "space" still pressed
	{
		if (m_bChecked)
			SetState (BOX_LDOWN_1);
		else
			SetState (BOX_LDOWN_2);
	}

	CButton::OnLButtonUp(nFlags, pt);       // also calls "ReleaseCapture"
}


//-----------------------------------------------------------------------------
// Name:		OnSetFocus
//-----------------------------------------------------------------------------
void CCheckBox::OnSetFocus(CWnd* pOldWnd)
{
	hasFocus = drawFocus;
	DrawFocus ();

	CButton::OnSetFocus(pOldWnd);
}


//-----------------------------------------------------------------------------
// Name:		OnKillFocus
//-----------------------------------------------------------------------------
void CCheckBox::OnKillFocus(CWnd* pNewWnd)
{
	if (spaceDown)                                  // mouse pressed in another window,
	{
		spaceDown = false;                      // while spaceDown
		if (m_bChecked)
			SetState (BOX_ON);
		else
			SetState (BOX_OFF);
	}
	hasFocus = false;
	DrawFocus ();

	CButton::OnKillFocus(pNewWnd);
}


//-----------------------------------------------------------------------------
// Name:		OnGetCheck
// Description:	recieved BM_GETCHECK - either directly or through:
//						- CWnd::IsDlgButtonChecked or
//						- CButton::GetCheck wrapper
//-----------------------------------------------------------------------------
LRESULT CCheckBox::OnGetCheck(WPARAM wParam, LPARAM lParam)
{
	if (m_bChecked)
		return BST_CHECKED;
	else
		return BST_UNCHECKED;
}


//-----------------------------------------------------------------------------
// Name:		OnSetCheck
// Description:	recieved BM_SETCHECK - either directly or through
//				CButton::SetCheck wrapper
//-----------------------------------------------------------------------------
LRESULT CCheckBox::OnSetCheck(WPARAM wParam, LPARAM lParam)
{
	if (wParam == BST_CHECKED)
	{
		m_bChecked = true;

		if (m_bDisabled)                // the state of a disabled CB
		{
			SetState (BOX_DISABLED_1);  // can also be changed
			return 0;
		}

		if (mouseDown)                                  // if mouse down => space doesn't matter
		{
			if (inRect)
				SetState (BOX_LDOWN_1);
			else
				SetState (BOX_ON);
		}
		else if (spaceDown)
		{
			SetState (BOX_LDOWN_1);
		}
		else
		{
			SetState (BOX_ON);
		}
	}
	else
	{
		m_bChecked = false;

		if (m_bDisabled)
		{
			SetState (BOX_DISABLED_2);
			return 0;
		}

		if (mouseDown)
		{
			if (inRect)
				SetState (BOX_LDOWN_2);
			else
				SetState (BOX_OFF);
		}
		else if (spaceDown)
		{
			SetState (BOX_LDOWN_2);
		}
		else
		{
			SetState (BOX_OFF);
		}
	}

	return 0;
}


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// //
// CCheckBox drawing methods

//-----------------------------------------------------------------------------
// Name:		DrawFocus
// Description:	Draws a focus rectangle
//
// Entry:
//				bool forceDraw - this parameter can be one of the following:
//					 false : draw focus rect only if "hasFocus" has changed
//					 true  : draw focus rect regardles of previous value
//							 of "hasFocus"
//
//					hasFocus - has the following meaning:
//					true  : check box has the keyboard focus at the moment
//					false : check box is not focused at the moment
//-----------------------------------------------------------------------------
void CCheckBox::DrawFocus (bool forceDraw)
{
	if (!drawFocus)   // draw focus rectangle?
		return;

	static bool prev = false;
	if (prev != hasFocus || forceDraw)
	{
		prev = hasFocus;

		CDC* pdc = GetDC ();
		DrawFocusRect (pdc->m_hDC, &rcFocus);
		ReleaseDC (pdc);
	}
}


//-----------------------------------------------------------------------------
// Name:		SetState
// Description:	Sets the new state and then blits the CB to the screen
//
// Entry:
//				int nState - specifies the state of the CB and can be:
//					BOX_ON			== checked state
//					BOX_OFF			== unchecked state
//					BOX_LDOWN_1		== mouse button down in checked state
//					BOX_LDOWN_2		== mouse button down in unchecked state
//					BOX_DISABLED_1	== CB disabled in checked state
//					BOX_DISABLED_2	== CB disabled in unchecked state
//-----------------------------------------------------------------------------
void CCheckBox::SetState (int state)
{
	if (state != m_nState)
	{
		m_nState = state;

//		CDC* pdc = GetDC ();
//		BlitToScreen(pdc->m_hDC,state);
//		ReleaseDC (pdc);

		if (hasFocus)
			DrawFocus (true);
	}
}


//-----------------------------------------------------------------------------
// Name:		BlitToScreen
// Description:	Blits (=draw, copy,..) "m_bitmaps [nState]" to the screen
//
// Entry:
//				HDC hdc - DC to blit to
//
//				int nState - specifies the state of the CB and can be:
//					BOX_ON
//					BOX_OFF
//					BOX_LDOWN_1
//					BOX_LDOWN_2
//					BOX_DISABLED_1
//					BOX_DISABLED_2
//-----------------------------------------------------------------------------
//DEL void CCheckBox::BlitToScreen (HDC hdc, int nState)
//DEL {
//DEL   HDC     destDC = ::CreateCompatibleDC (hdc);
//DEL   HBITMAP oDest;
//DEL   oDest = (HBITMAP) ::SelectObject (destDC,m_bitmaps [nState]);
//DEL
//DEL   RECT rect;
//DEL   GetClientRect(&rect);
//DEL
//DEL   int width  = rect.right  - rect.left;
//DEL   int height = rect.bottom - rect.top;
//DEL
//DEL   if(height >= mHeight) {
//DEL           rect.top    += (height  - mHeight)/2;
//DEL           rect.bottom -= (height - mHeight)/2;
//DEL           height = rect.bottom - rect.top;
//DEL   }
//DEL
//DEL   // Blit pre-prepared bitmap to screen
//DEL   //
//DEL   BitBlt(hdc, rect.left, rect.top, width, height, destDC, 0, 0, SRCCOPY);
//DEL
//DEL
//DEL   // Clean up
//DEL   //
//DEL   SelectObject(destDC, oDest);
//DEL
//DEL   DeleteDC(destDC);
//DEL }


//-----------------------------------------------------------------------------
// Name:		GetMinHeight
// Description:	Calculates minimal height of CB based on m_font
//
// Entry:
//				HDC hdc - DC into which m_font is selected
//
//-----------------------------------------------------------------------------
int CCheckBox::GetMinHeight (HDC hdc)
{
	HFONT oFont = (HFONT) SelectObject (hdc, m_font);
	SIZE sz; GetTextExtentPoint32 (hdc, "Dummy", 5, &sz);
	SelectObject (hdc, oFont);

	return sz.cy + 1;
}


//-----------------------------------------------------------------------------
// Name:		PrepareBitmaps & PrepareState
// Description:	This methods prepare "m_bitmaps[???]" for all posible states
//
// Entry:
//				HDC     hdc - DC used to create memory DCs for bitmap preparation
//
//				HBITMAP &bDest - this parameter is one of the following:
//						m_bitmaps [0] : BOX_ON  bitmap
//						m_bitmaps [1] : BOX_OFF bitmap
//						...
//						m_bitmaps [5] : BOX_DISABLED_2 bitmap
//
//				int nState - specifies the state of the CB and can be:
//					BOX_ON
//					BOX_OFF
//					BOX_LDOWN_1
//					BOX_LDOWN_2
//					BOX_DISABLED_1
//					BOX_DISABLED_2
//-----------------------------------------------------------------------------
//DEL void CCheckBox::PrepareBitmaps (HDC hdc)
//DEL {
//DEL   mHeight = GetMinHeight (hdc);
//DEL   if (mHeight < BOX_SIZE)
//DEL           mHeight = BOX_SIZE;
//DEL
//DEL
//DEL /*	PrepareState (hdc, m_bitmaps [0], BOX_ON);
//DEL   PrepareState (hdc, m_bitmaps [1], BOX_OFF);
//DEL   PrepareState (hdc, m_bitmaps [2], BOX_LDOWN_1);
//DEL   PrepareState (hdc, m_bitmaps [3], BOX_LDOWN_2);
//DEL   PrepareState (hdc, m_bitmaps [4], BOX_DISABLED_1);
//DEL   PrepareState (hdc, m_bitmaps [5], BOX_DISABLED_2);*/
//DEL }

//DEL void CCheckBox::PrepareState (HDC hdc, HBITMAP & bDest, int nState)
//DEL {
//DEL   RECT rect;
//DEL   GetClientRect(&rect);
//DEL
//DEL   int width  = rect.right  - rect.left;
//DEL   int height = rect.bottom - rect.top;
//DEL
//DEL   if(height >= mHeight) {
//DEL           rect.top    += (height - mHeight)/2;
//DEL           rect.bottom -= (height - mHeight)/2;
//DEL           height = rect.bottom - rect.top;
//DEL   }
//DEL   int o = (mHeight - BOX_SIZE) / 2;
//DEL
//DEL   //
//DEL   // Create memory DCs and bitmaps to prepare the image
//DEL   //
//DEL   HDC srcDC  = ::CreateCompatibleDC (hdc);
//DEL   HDC maskDC = ::CreateCompatibleDC (hdc);
//DEL   HDC destDC = ::CreateCompatibleDC (hdc);
//DEL
//DEL
//DEL   HBITMAP bMask = ::CreateBitmap (BOX_SIZE,BOX_SIZE,1,1,NULL);
//DEL                   bDest = ::CreateCompatibleBitmap (hdc,width,mHeight);
//DEL
//DEL   HBITMAP oSrc  = (HBITMAP) ::SelectObject (srcDC, m_bSrc);
//DEL   HBITMAP oMask = (HBITMAP) ::SelectObject (maskDC,bMask);
//DEL   HBITMAP oDest = (HBITMAP) ::SelectObject (destDC,bDest);
//DEL
//DEL
//DEL   // Create mask - mask color: RGB(255,0,255)
//DEL   //
//DEL   COLORREF color = SetBkColor(srcDC, RGB(255,0,255));
//DEL
//DEL
//DEL   RECT rc;				// rect = drawing rectangle inside client area of CB
//DEL   rc.left	  = rc.top = 0;	// rc   = drawing rectangle inside destDC
//DEL   rc.right  = width;		// right of rc = (width) of rect
//DEL   rc.bottom = height;     // bottom of rc = (height) of rect
//DEL
//DEL
//DEL   // copy the display surface where the CheckBox will be to destDC
//DEL   //
//DEL #if TRANSPARENT_CB
//DEL   BitBlt (destDC,0,0,width,height,hdc, rect.left, rect.top, SRCCOPY);
//DEL #else
//DEL   FillRect (destDC, &rc, m_brush);
//DEL #endif
//DEL
//DEL   int l = 0;
//DEL   if (m_bLeftText) l = width - BOX_SIZE;
//DEL
//DEL   int x = BOX_SIZE*nState;
//DEL   BitBlt(maskDC, 0, 0, BOX_SIZE, BOX_SIZE, srcDC, x, 0, SRCCOPY);
//DEL   BitBlt(destDC, l, o, BOX_SIZE, BOX_SIZE, srcDC, x, 0, SRCINVERT);
//DEL   BitBlt(destDC, l, o, BOX_SIZE, BOX_SIZE,maskDC, 0, 0, SRCAND);
//DEL   BitBlt(destDC, l, o, BOX_SIZE, BOX_SIZE, srcDC, x, 0, SRCINVERT);
//DEL
//DEL
//DEL   // Draw control text
//DEL   //
//DEL   DrawText_ (destDC,rc,nState);
//DEL   rcFocus.top    += rect.top;
//DEL   rcFocus.bottom += rect.top;
//DEL   rcFocus.left  -= 1;
//DEL   rcFocus.right += 1;
//DEL
//DEL   // Clean up
//DEL   //
//DEL   SelectObject(srcDC,  oSrc);
//DEL   SelectObject(maskDC, oMask);
//DEL   SelectObject(destDC, oDest);
//DEL
//DEL   DeleteDC(srcDC);
//DEL   DeleteDC(maskDC);
//DEL   DeleteDC(destDC);
//DEL
//DEL   DeleteObject(bMask);
//DEL }


//-----------------------------------------------------------------------------
// Name:		DrawText_
// Description:	Draws text for the CB to destDC
//				(into which one of the "m_bitmaps [???]" is selected)
//
// Entry:
//				HDC destDC - DC to which text is drawed
//							 ("m_bitmaps [???]" is selected into it)
//
//				RECT rc    - part of client area (of the CB) to which
//							 text is output
//
//-----------------------------------------------------------------------------
//DEL void CCheckBox::DrawText_ (HDC destDC, RECT rc, int nState)
//DEL {
//DEL   int oState = SaveDC (destDC);
//DEL
//DEL   SetBkMode (destDC, TRANSPARENT);
//DEL   SelectObject (destDC,m_font);
//DEL
//DEL
//DEL   unsigned style = 0;
//DEL   if (m_bLeftText) {
//DEL           rc.right -= BOX_SIZE+6;  style = DT_RIGHT;
//DEL   }
//DEL   else {
//DEL           rc.left  += BOX_SIZE+5;  style = DT_LEFT;
//DEL   }
//DEL   rc.bottom -= 1;				// rc   = drawing rectangle inside destDC
//DEL
//DEL
//DEL   if (nState == BOX_DISABLED_1 || nState == BOX_DISABLED_2)
//DEL   {
//DEL           SetTextColor (destDC, GetSysColor (COLOR_3DHILIGHT));
//DEL           rc.left  += 1;	rc.top   += 1;
//DEL           rc.right += 1;	rc.bottom+= 1;
//DEL           DrawText (destDC, m_text, -1, &rc, DT_SINGLELINE | DT_VCENTER | style);
//DEL
//DEL           SetTextColor (destDC, GetSysColor (COLOR_3DSHADOW));
//DEL           rc.left  -= 1;	rc.top   -= 1;
//DEL           rc.right -= 1;	rc.bottom-= 1;
//DEL           DrawText (destDC, m_text, -1, &rc, DT_SINGLELINE | DT_VCENTER | style);
//DEL   }
//DEL   else
//DEL           DrawText (destDC, m_text, -1, &rc, DT_SINGLELINE | DT_VCENTER | style);
//DEL
//DEL
//DEL   // calculate rcFocus
//DEL   //
//DEL   int r_t = rc.right;				// right side of text rectangle
//DEL
//DEL   DrawText (destDC, m_text, -1, &rc, DT_SINGLELINE | DT_VCENTER | style | DT_CALCRECT);
//DEL
//DEL   if (m_bLeftText) {
//DEL           int d = rc.right - rc.left; // text width
//DEL           d = r_t - d;
//DEL           rc.left  += d;
//DEL           rc.right += d;
//DEL   }
//DEL   rcFocus = rc;
//DEL
//DEL   RestoreDC (destDC, oState);
//DEL }
