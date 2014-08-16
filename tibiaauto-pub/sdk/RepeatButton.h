#if !defined(AFX_REPEATBUTTON_H__BC90DEBD_C42C_4475_A658_05228E9710BF__INCLUDED_)
#define AFX_REPEATBUTTON_H__BC90DEBD_C42C_4475_A658_05228E9710BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RepeatButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RepeatButton allows for a typomatic-style repeat of the button just by
// holding down the mouse button.  Also known in some circles as a
// "Fire Control" button.
/////////////////////////////////////////////////////////////////////////////
// This control is based upon ideas and code presented in:
//
//	http://www.codeproject.com/miscctrl/subclassdemo.asp
//	http://www.codeproject.com/cpp/mouserepeat.asp
//
// The "subclass" page was invaluable in getting a clean subclassed button
// along with how to use it. It might be obvious for the expert but I
// needed the use-small-words-to-explain language to finally "get it"
//
// The "mouserepeat" page was the starting point for the concept.
//
// CRepeatButton is just an ordinary button but if you hold the Left Mouse
// button down, it acts like it was being repeatedly clicked! The initial
// delay and the repeat rate are identical to that of the keyboard repeat
// delay and rate.
//
// The only gotchas are that unlike ordinary buttons, the BN_CLICKED message
// is sent to the dialog upon Left Mouse Button DOWN rather than UP. And the
// background of the button turns yellow when it is repeating (so you know
// that something non-standard is going on). There is also an audible clicking
// sound during the auto-repeat to give further indication to the user that
// something non-standard is happening.
//
// The trick to using this control is to use an ordinary button control on your
// dialog and then (here is the snazzy part) associate a dialog member variable,
// Category: Control, Variable type: CRepeatButton
//
// Now that button, as displayed in the dialog editor, is really a CRepeatButton
// instead of a CButton!
//
/////////////////////////////////////////////////////////////////////////////
// CRepeatButton window

class CRepeatButton : public CButton
{
// Construction
public:
	CRepeatButton();

// Attributes
private:
	BOOL TimerID;
	int TimerStep;

	// To minimize churn, get the ControlID and construct the message parameters
	// upon LBDOWN and save them for each time the timer fires.
	WPARAM wParam;	// combined ControlID and BN_CLICKED message
	HWND lParam;	// handle to control's window


// Operations
public:
	// Only need the DrawItem and PreSubclassWindow if you want the background
	// color to change when in repeat mode.
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRepeatButton)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsOverClient(void);	// See if mouse is hovering over the button's client window.
	virtual ~CRepeatButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRepeatButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPEATBUTTON_H__BC90DEBD_C42C_4475_A658_05228E9710BF__INCLUDED_)
