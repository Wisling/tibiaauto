// RepeatButton.cpp : implementation file
//
// RepeatButton allows typomatic-style repeating of a button by holding down
// the mouse button. Also known in some circles as a "Fire Control" button.

#include "stdafx.h"
#include "RepeatButton.h"

#include <mmsystem.h>   // for playing a "click" wav sound using sndPlaySound()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRepeatButton

CRepeatButton::CRepeatButton()
{
	TimerID   = 0;  // No timer in use
	TimerStep = 0;  // Not yet using the button yet
}

CRepeatButton::~CRepeatButton()
{
}

BEGIN_MESSAGE_MAP(CRepeatButton, CButton)
//{{AFX_MSG_MAP(CRepeatButton)
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_TIMER()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRepeatButton message handlers

void CRepeatButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Set up for notifying parent window
	int ControlID = GetDlgCtrlID();
	wParam = ControlID | (BN_CLICKED << 16);
	lParam = GetSafeHwnd();

	// Unlike an ordinary button, this sends the BN_CLICKED notification upon
	// Left Mouse Button DOWN rather than UP.
	GetParent()->PostMessage(WM_COMMAND, wParam, (LPARAM)lParam);

	// initial timer interval = keyboard repeat delay
	int setting = 0;
	SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &setting, 0);
	int InitialDelay = (setting + 1) * 250;
	TimerID   = SetTimer(99, InitialDelay, NULL);
	TimerStep = 1;  // waiting for initial delay before starting auto-repeat

	// If you pass the message along to the base class then when you get a button up
	// you get an extra BN_CLICKED action. If you don't pass along the message
	// then the button state is not handled. So explicitly call SetState()
	// CButton::OnLButtonDown(nFlags, point);
	CButton::SetState(TRUE);
}

void CRepeatButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	if (TimerStep)   // We're done with this button
	{
		KillTimer(TimerID);
		TimerStep = 0;
	}

	// Don't pass along the Button Up or you may get an extra BN_CLICKED message
	// CButton::OnLButtonUp(nFlags, point);
	CButton::SetState(FALSE);       // clear the depressed pushbutton appearence
}

void CRepeatButton::OnTimer(UINT nIDEvent)
{
	// We may have moved the mouse during the timer interval so double-check it first!
	if (!IsOverClient())
	{
		KillTimer(TimerID);
		TimerStep = 0;  // No longer doing auto-repeat
		CButton::SetState(FALSE);       // clear the depressed pushbutton appearence
	}
	else
	{
		if (TimerStep == 1)      // initial delay before auto-repeat?
		{
			KillTimer(TimerID);     // set up new timer interval

			// set timer interval per keyboard repeat rate
			int setting = 0;
			SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &setting, 0);
			int RepeatInterval = 400 - (setting * 12);
			TimerID   = SetTimer(100, RepeatInterval, NULL);
			TimerStep = 2;  // Now we are in auto-repeat mode
		}

		if (TimerStep > 1)
			GetParent()->PostMessage(WM_COMMAND, wParam, (LPARAM)lParam);
	}

	CButton::OnTimer(nIDEvent);
}

// Want to ensure that we are not repeating if the mouse has wandered off the
// button.
BOOL CRepeatButton::IsOverClient()
{
	CPoint p(GetMessagePos());
	ScreenToClient(&p);

	CRect rect;
	GetClientRect(rect);

	return rect.PtInRect(p) ? TRUE : FALSE;
}
