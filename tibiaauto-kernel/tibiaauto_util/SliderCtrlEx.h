#pragma once

#include "tibiaauto_util.h"

#include <afxtempl.h>


/////////////////////////////////////////////////////////////////////////////
// This control is partially based upon work by Nic Wilson in the article:
//
//		http://www.codeproject.com/miscctrl/transparentslider.asp
//
// The technique of saving the TIC marks is used (massively modified) but
// the code for making the slider transparent is not used.
//
// An array of slider ranges and associated colors is used to draw the
// channel area. Each color range is drawn and succeeding ones overwrite
// as needed. This is useful for visually indicating good vs. bad control
// positions, or even degrees of "badness" (via the gradient).
//
//	int min = 100, max = 1200
//	m_Slider1.SetRange(min,max);
//	m_Slider1.AddColor(min,max,colRed);	// entire background is marked in red
//	m_Slider1.AddColor(min/3, (2*max)/3, colGreen);	// middle 1/3 is "Good"
//
// If you change the slider range to something outside the color ranges, then
// all the color ranges are erased.
//
// SetErrResponse() can be used to control how errors in AddColor() are
// handled. The default is 0, which just returns FALSE and does not put the
// given range into the array.
//
//
/////////////////////////////////////////////////////////////////////////////
// CSliderCtrlEx window

// Pointer to a static function, class instance is first argument.
// The expectation is that the function pointer will be to a static
// call member function, and that the p2Object argument will be a
// pointer to the class instance.
typedef void (*ptr2Func)(void *p2Object, LPARAM data1, int sValue, BOOL IsDragging);


// Dynamically attempt to load in msimg32.dll and if available, load in the
// GradientFill() function. Otherwise use a homebrew version.
//
// This technique shameless stolen from an article by Irek Zielinski in
// http://www.codeproject.com/staticctrl/gradient_static.asp.
//
// However, the homebrew version is my own.
typedef UINT (CALLBACK * LPFNDLLFUNC1)(HDC, CONST PTRIVERTEX, DWORD, CONST PVOID, DWORD, DWORD);


class TIBIAAUTOUTIL_API CSliderCtrlEx : public CSliderCtrl
{
// Construction
public:
	CSliderCtrlEx();

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSliderCtrlEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Refresh(void);             // force update of displayed slider -- user after all range and color changes
	BOOL m_extendColors;    // TRUE if background colors should be extended to edge of client rectangle
	BOOL m_dumpBitmaps;             // debugging variable to dump intermediate bitmaps to file

	// Debugging routine used to look at intermediate bitmaps
	BOOL SaveBitmap(LPCSTR lpFileName, CBitmap &hBitmap, HPALETTE hPal = NULL);

	/////////////////////////////////////////////////////////////////////////
	// When using a callback to update another control, you get much faster
	// response in the update if the TAB ORDER is such that the slider is
	// before the other control. Otherwise, when you update the other control,
	// the current control gets repainted and that causes another update
	// message, etc. The result is "stickiness" or erratic updates.
	//
	// So, if IDC_SLIDER1 has a callback to update IDC_EDIT1, you should make
	// sure that the tab order for IDC_SLIDER1 is, say 5, and IDC_EDIT1 is, say,
	// 6.
	///////////////////////////////////////////////////////////////////////////
	void setCallback(void *funcptr, void *p2Object, LPARAM data1);  // set callback function for reporting slider position
	ptr2Func m_Callback;            // call this to report slider position
	void *m_p2Object;                       // user-supplied value (typically pointer to class instance (this))
	LPARAM m_data1;                         // user-supplied value (typically control ID or pointer)
	void clearCallback(void);       // clear out the callback function

	// Overrides for standard CSliderCtrl
	void SetRangeMax(int nMax, BOOL bRedraw = FALSE);                       // set max value for slider
	void SetRange(int nMin, int nMax, BOOL bRedraw = FALSE);        // set min and max value for slider
	void SetRangeMin(int nMin, BOOL bRedraw = FALSE);                       // min min value for slider

	BOOL AddColor(int nLow, int nHigh, COLORREF color);     // add a new range and color (TRUE if no errors)
	BOOL AddColor(int nLow, int nHigh, COLORREF strColor, COLORREF endColor); // add a new range and color gradient
	int setErrResponse(int resp);   // Control handling of AddColor() errors
	void clearColors(void);                 // clear out all color ranges
	virtual ~CSliderCtrlEx();

	// Generated message map functions
protected:
	struct clrRange {
		int lval;       // start new color at this slider value
		int hval;       // finish end color at this slider value
		COLORREF strColor;      // starting color (use named values or RGB() macro)
		COLORREF endColor;      // ending color (same as strColor if no gradient)
	};
	CArray<clrRange, clrRange> colorList;    // ordered list of ranges and associated colors
	int errResponse;        // how to handle errors in the AddColor() functions
	int m_oldPosition;      // used to detect if the slider has actually moved
	BOOL m_IsDragging;      // Set if the mouse button is DOWN

	// for using GradientFill from msimg32.dll
	HINSTANCE hinst_msimg32;        // DLL handle for msimg32.dll
	LPFNDLLFUNC1 GradientFill;      // function pointer for GradientFill from DLL

	//{{AFX_MSG(CSliderCtrlEx)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.