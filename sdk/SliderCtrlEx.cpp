////////////////////////////////////////////////////////////////////////////////
// Copyright 2002 by Harold Bamford
//	You may use this code as long as you keep the copyright in place
//	and do not claim the work as your own.
////////////////////////////////////////////////////////////////////////////////
// SliderCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "SliderCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSliderCtrlEx

CSliderCtrlEx::CSliderCtrlEx()
{
	errResponse = 0;	// respond with retval only, -1 means use VALID, 1 means adjust
	m_Callback = NULL;	// pointer to function to call upon slider update
	m_p2Object = 0;		// no callback function assigned
	m_data1 = 0;		// no misc data value assigned
	m_IsDragging = FALSE;	// mouse is not dragging the slider
	m_oldPosition = -99999999;	// impossible number
	m_dumpBitmaps = FALSE;	// TRUE if we should dump intermediate bitmaps to file for debugging

	// Try and get GradientFill from this library (which may not be available)
	hinst_msimg32 = LoadLibrary( "msimg32.dll" );
	GradientFill = NULL;
	if(hinst_msimg32)
	{
		GradientFill = ((LPFNDLLFUNC1) GetProcAddress( hinst_msimg32, "GradientFill" ));
	}

	m_extendColors = TRUE;	// extend colors set at min and max to edge of control window
}

CSliderCtrlEx::~CSliderCtrlEx()
{
	if(hinst_msimg32)
	{
		FreeLibrary( hinst_msimg32 );
	}
}


BEGIN_MESSAGE_MAP(CSliderCtrlEx, CSliderCtrl)
	//{{AFX_MSG_MAP(CSliderCtrlEx)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSliderCtrlEx message handlers

void CSliderCtrlEx::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	int loopMax = colorList.GetSize();	// number of color ranges to process
	LPNMCUSTOMDRAW lpCustDraw = (LPNMCUSTOMDRAW)pNMHDR;

	////////////////////////////////////////////////////////////////////////////////
	// OnCustomDraw() is called at many different stages during the painting process
	// of the control. We only care about the PREPAINT state or the ITEMPREPAINT
	// state and not always then.
	//
	// If we want to be notified about subcontrol painting, we have to say so when
	// we get the initial PREPAINT message.
	////////////////////////////////////////////////////////////////////////////////
	if(lpCustDraw->dwDrawStage == CDDS_PREPAINT)
	{
		// should we report slider's position?
		int curVal = GetPos();
		if((m_Callback != NULL)	&& (curVal != m_oldPosition))
		{
			m_oldPosition = curVal;
			m_Callback(m_p2Object, m_data1, curVal, m_IsDragging);
		}

		// If we don't have any special coloring to do, skip all the silliness...
		if(loopMax <= 0)
		{
			*pResult = CDRF_DODEFAULT;
		}
		else
		{
			// We want to be informed when each part of the control is being
			// processed so we can intercept the channel drawing.
			*pResult = CDRF_NOTIFYITEMDRAW;	// send messages for each piece-part
		}
		return;
	}


	///////////////////////////////////////////////////////////////////////////////
	// A slider (track control) is drawn in several steps:
	//	1. Erase
	//	2. Tics
	//	3. Channel
	//	4. Thumb
	//
	// It would be nice to capture when the background has been painted and
	// before the other sub-pieces have been painted. Then we could just
	// substitute our own painting routine. But this doesn't seem to be
	// available.
	//
	// So this routine captures the tics by inserting operations before
	// painting the thumb.
	//
	// Look at the help on NMCUSTOMDRAW for complete details, but the pNMHDR
	// pointer looks at a structure like:
	//
	// typedef struct tagNMCUSTOMDRAWINFO {
    //	 NMHDR  hdr;
    //	 DWORD  dwDrawStage;	// This indicates what stage of the drawing process is involved
    //	 HDC    hdc;			// graphics context of the control (or sub-component)
    //	 RECT   rc;
    //	 DWORD  dwItemSpec;	// This is the particular piece-part of the slider involved
    //	 UINT   uItemState;
    //	 LPARAM lItemlParam;
	// } NMCUSTOMDRAW
	//
	// The stages include CDDS_PREPAINT, which is just before painting of the entire
	// control. However, unless the *pResult parameter is set to CDRF_NOTIFYITEMDRAW,
	// we will get notification for the control as a whole, not for each piece-part.
	// So the first thing to do is set *pResult. Thereafter, we must intercept
	// the sub-parts.
	//
	// We don't care about painting the background (we will re-paint later on). We
	// don't care about PREPAINT on the CHANNEL or the TICS since we will overwrite
	// everything when we get to the THUMB.
	/////////////////////////////////////////////////////////////////////////////////

	if((lpCustDraw->dwDrawStage == CDDS_ITEMPREPAINT) && (lpCustDraw->dwItemSpec != TBCD_THUMB))
	{
		*pResult = CDRF_DODEFAULT;
		return;
	}

	// get channel orientation
	BOOL IsVertical = (TBS_VERT & GetStyle()) ? TRUE : FALSE;

	// Get the coordinates of the control's window
	CRect crect;
	GetClientRect(crect);	// client coordinates (top = left = 0, bottom = height, right = width)

	// Much of this is "paraphrased" from Nic Wilson's work -- see the header file
	//////////////////////////////////////////////////////////////////////////////////
	// This bit does the tics marks transparently.
	// Create a memory dc to hold a copy of the oldbitmap data that includes the tics,
	// because when we add the background in we will lose the tic marks.
	///////////////////////////////////////////////////////////////////////////////////
	CDC *pDC = CDC::FromHandle(lpCustDraw->hdc);
	CDC SaveCDC;
	CBitmap SaveCBmp;

	//set the colours for the monochrome mask bitmap
	COLORREF crOldBack = pDC->SetBkColor(RGB(0,0,0));			// set to Black
	COLORREF crOldText = pDC->SetTextColor(RGB(255,255,255));	// set to White

	int iWidth  = crect.Width();	// channel width
	int iHeight = crect.Height();	// channel height

	////////////////////////////////////////////////////////////////////////////
	// Create an in-memory copy of displayed bitmap, including the tics.
	// This is a monochrome bitmap since it was created from a memory DC.
	// If it had been created from pDC (an actual device DC, not a memory
	// DC) then this would be something with 8, 16, 24, or 32 bits per pixel.
	//
	// This will have a black background, with the tic marks in white.
	//
	// For reasons I don't yet understand, this saves only the tic marks and
	// the channel's centerline (both originally in black), and not the other
	// colors (such as drawn AROUND the channel's centerline). I am not sure
	// what would have happened if the tic marks were not black...
	////////////////////////////////////////////////////////////////////////////
	SaveCDC.CreateCompatibleDC(pDC);
	SaveCBmp.CreateCompatibleBitmap(&SaveCDC, iWidth, iHeight);
	CBitmap* SaveCBmpOld = (CBitmap *)SaveCDC.SelectObject(SaveCBmp);
	SaveCDC.BitBlt(0, 0, iWidth, iHeight, pDC, crect.left, crect.top, SRCCOPY);

	if(m_dumpBitmaps)	// debugging stuff
	{
		SaveBitmap("MonoTicsMask.bmp",SaveCBmp);
	}

	// Do as much of this stuff in memory as possible, then blit it to the screen
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap memBM;
	memBM.CreateCompatibleBitmap(pDC,iWidth,iHeight);	// create from pDC, not memDC
	CBitmap *oldbm = memDC.SelectObject(&memBM);

	////////////////////////////////////////////////////////////////////////////////
	// copy screen bitmap to memory bitmap for manipulation. If this is the very
	// first time the control has been updated, the screen bitmap will show only
	// the tic marks (in black) and the default background color (RGB(214,207,189)).
	// If the control has been updated before, remnants of the previously drawn
	// background color ranges will also show up.
	////////////////////////////////////////////////////////////////////////////////
	memDC.BitBlt(0,0,iWidth,iHeight,pDC,0,0,SRCCOPY);

	if(m_dumpBitmaps)	// debugging
	{
		SaveBitmap("ScrnStart.bmp",memBM);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Color parts of the channel if necessary. It SHOULD be necessary since we
	// don't get notifications unless there are colors to print, but we may have
	// a race condition and it is best to check.
	/////////////////////////////////////////////////////////////////////////////
	if(loopMax)
	{
		/////////////////////////////////////////////////////////////////////////////////
		// We need to draw colors over the subrange of the channel that the center of the
		// thumb traverses, rather than the entire client window. Later on, extend these
		// colors outwards to the ends of the client window (for nicer appearance). This
		// allows for more precise correlation with color and thumb position.
		/////////////////////////////////////////////////////////////////////////////////
		CRect chanRect;
		GetChannelRect(&chanRect);
		CRect thmbRect;
		GetThumbRect(&thmbRect);

		// For unknown reasons, GetChannelRect() returns a rectangle
		// as though it were a horizonally oriented slider, even if it isn't!
		if(IsVertical)
		{
			CRect n;	// could probably just change chanRect directly
			n.left = chanRect.top;
			n.right = chanRect.bottom;
			n.top = chanRect.left;
			n.bottom = chanRect.right;
			n.NormalizeRect();
			chanRect.CopyRect(&n);
		}

		// Offset into client rectangle for beginning of coloring range
		int Offset = chanRect.left + thmbRect.Width()/2;
		if(IsVertical)
		{
			Offset = chanRect.top + thmbRect.Height()/2;
		}

		// Range for center of thumb on the channel
		int ht = chanRect.Height() - thmbRect.Height();
		int wd = chanRect.Width()  - thmbRect.Width();

		// scaling between control range and bitmap
		int min,max;
		GetRange(min,max);	// range of values for the slider
		double scale = (double(max) - double(min))/double(IsVertical ? ht : wd);

		BOOL gotStartColor = FALSE;
		BOOL gotEndColor = FALSE;
		COLORREF startColor = 0, endColor = 0;

		int loop;	// Loop through the array of color ranges
		for(loop = 0; loop < loopMax; loop++)
		{
			clrRange clr;
			clr = colorList[loop];
			
			// Get the good values. If not set, then entire range is good
			int lval = clr.lval;
			int hval = clr.hval;
			if((lval < min) || (lval > max)) lval = min;
			if((hval > max) || (hval < min)) hval = max;

			if(lval == min)
			{
				gotStartColor = TRUE;
				startColor = clr.strColor;
			}
			if(hval == max)
			{
				gotEndColor = TRUE;
				endColor = clr.endColor;
			}

			int minVal = lval - min;	// offset into bitmap for this color
			minVal = int(double(minVal)/scale);

			// width (or height for vertical slider) inside bitmap for this color
			int widthVal = hval - lval;
			widthVal = int((double(widthVal)/scale) + 1.0);
			
			// For drawing a gradient, we need to know the individual RGB values
			int sR,eR,sG,eG,sB,eB;	// start and end R, G, and B values
			sR = GetRValue(clr.strColor);
			eR = GetRValue(clr.endColor);
			sG = GetGValue(clr.strColor);
			eG = GetGValue(clr.endColor);
			sB = GetBValue(clr.strColor);
			eB = GetBValue(clr.endColor);

			if(GradientFill != NULL)
			{
				TRIVERTEX vert[2];	// for specifying range to gradient fill
				GRADIENT_RECT gRect;

// Warning C4244: conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning (push)
#pragma warning (disable : 4244)
				vert[0].Red   = sR<<8;	// expects 16-bit color values!
				vert[0].Green = sG<<8;
				vert[0].Blue  = sB<<8;
				vert[0].Alpha = 0;		// no fading/transparency
				
				vert[1].Red   = eR<<8;
				vert[1].Green = eG<<8;
				vert[1].Blue  = eB<<8;
				vert[1].Alpha = 0;
#pragma warning (pop)

				gRect.UpperLeft = 0;
				gRect.LowerRight = 1;
				
				BOOL retval;
				if(IsVertical)	// vertically oriented?
				{
					vert[0].x = 0;
					vert[0].y = Offset + minVal;
					vert[1].x = iWidth;
					vert[1].y = Offset + minVal + widthVal;
					retval = GradientFill(memDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);
				}
				else
				{
					vert[0].x = Offset + minVal;
					vert[0].y = 0;
					vert[1].x = Offset + minVal + widthVal;
					vert[1].y = iHeight;
					retval = GradientFill(memDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);
				}
			}
			else
			{
				// Homebrew version of GradientFill for rectangles -- works pretty well, sort of.
				int i;
				for(i = 0; i < widthVal; i++)	// for each pixel column in bitmap color range
				{
					int R = sR;
					int G = sG;
					int B = sB;
					if(widthVal)
					{
						R += ::MulDiv(eR - sR, i, widthVal);
						G += ::MulDiv(eG - sG, i, widthVal);
						B += ::MulDiv(eB - sB, i, widthVal);
					}

					if(IsVertical)
					{
						// widthVal really refers to height
						//memDC.FillSolidRect(0,minVal+i,iWidth,widthVal-i,GetNearestColor(memDC,RGB(R,G,B)));
						memDC.FillSolidRect(
							0,						// starting X value
							Offset + minVal + i,	// starting Y value
							iWidth,					// full width
							1,						// one pixel height
							GetNearestColor(memDC,RGB(R,G,B)));
					}
					else
					{
						//memDC.FillSolidRect(minVal+i,0,widthVal-i,iHeight,GetNearestColor(memDC,RGB(R,G,B)));
						memDC.FillSolidRect(
							Offset + minVal + i,	// Starting X value
							0,						// Starting Y value
							1,						// 1 pixel wide
							iHeight,				// full height
							GetNearestColor(memDC,RGB(R,G,B)));
					}
				}
			}
		}

		if(m_extendColors)
		{
			// If we have put in colors at the slider ends, then extend those same
			// colors to the rest of the background. We could try to determine the
			// colors by examining the bitmap, but this is awkward and it is just
			// as easy to grab them on-the-fly in the coloring loop above.
			//
			// If you want to see why this is done, just set m_extendColors to FALSE
			// and take a look at the control. Ugly. But there might be a legitimate
			// reason for it so leave the option to suppress.
			if(IsVertical)
			{
				if(gotStartColor)
				{
					memDC.FillSolidRect(0, 0, iWidth, Offset, startColor);
				}
				if(gotEndColor)
				{
					memDC.FillSolidRect(0, iHeight - Offset - 1, iWidth, Offset, endColor);
				}
			}
			else
			{
				if(gotStartColor)
				{
					memDC.FillSolidRect(0, 0, Offset, iHeight, startColor);
				}
				if(gotEndColor)
				{
					memDC.FillSolidRect(iWidth - Offset - 1, 0, Offset, iHeight, endColor);
				}
			}
		}
	}

	// The screen bitmap should now have only the color ranges filled in, no tic
	// marks should be visible.
	if(m_dumpBitmaps)	// debugging
	{
		SaveBitmap("ScrnColors.bmp",memBM);
	}
	
	//////////////////////////////////////////////////////////////
	// More "paraphrasing" from Nic Wilson's work...
	//////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	// At this point, memDC's bitmap contains just the color ranges drawn above.
	// No tic marks are visible. Doing SRCINVERT with the mask will draw in the
	// tic marks, but all the colors will be backwards. Also, the tics will be
	// whatever color was drawn in the color range stuff, rather than solid,
	// normal tic marks. SRCINVERT means the RGB values are changed by subtracting
	// from 255:
	//
	//		RGB(255,0,0)    --> RGB(0,255,255)
	//		RGB(247,8,0)    --> RGB(8,247,255)
	//		RGB(214,40,255) --> RGB(41,215,0)
	//
	/////////////////////////////////////////////////////////////////////////////
	memDC.SetBkColor(pDC->GetBkColor());
	memDC.SetTextColor(pDC->GetTextColor());
	memDC.BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCINVERT);

	if(m_dumpBitmaps)	// debugging
	{
		SaveBitmap("ScrnInvert.bmp",memBM);
	}

	// Change the tic marks from the color range to the background color. This
	// changes only the tic marks (and the channel centerline) and leaves the
	// rest alone. The tic marks wind up black.
	memDC.BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCAND);

	if(m_dumpBitmaps)	// debugging
	{
		SaveBitmap("ScrnAnd.bmp",memBM);
	}

	// Finally, invert the color ranges to their normal values. Since the tic
	// marks in the SaveCDC bitmap are white, they get inverted to black.
	memDC.BitBlt(0, 0, iWidth, iHeight, &SaveCDC, 0, 0, SRCINVERT);

	if(m_dumpBitmaps)	// debugging
	{
		SaveBitmap("ScrnFinal.bmp",memBM);
	}
	
	// Now copy out to screen
	pDC->BitBlt(0,0,iWidth,iHeight,&memDC,0,0,SRCCOPY);

	// restore and clean up
	pDC->SetBkColor(crOldBack);
	pDC->SetTextColor(crOldText);
	DeleteObject(SelectObject(SaveCDC, SaveCBmpOld));
	DeleteDC(SaveCDC);
	DeleteObject(SelectObject(memDC,oldbm));
	DeleteDC(memDC);

	*pResult = CDRF_DODEFAULT;

	m_dumpBitmaps = FALSE;	// only do this once!
}

// Add a solid color range to the list
BOOL CSliderCtrlEx::AddColor(int nLow, int nHigh, COLORREF color)
{
	return AddColor(nLow,nHigh,color,color);
}

BOOL CSliderCtrlEx::AddColor(int nLow, int nHigh, COLORREF strColor, COLORREF endColor)
{
	BOOL retval = TRUE;
	int l,h;
	GetRange(l,h);

	if((nLow < l) || (nHigh > h))	// sanity check
	{
		if(!errResponse)
		{
			return FALSE;	// merely indicate failure
		}
		if(errResponse > 0)	// force in range and continue (but indicate a problem)
		{
			if(nLow < l) nLow = l;
			if(nHigh > h) nHigh = h;
			retval = FALSE;	// there was an error, but it has been sort-of corrected
		}
		else
		{
			CString msg;
			msg.Format("Bad range (%d,%d) given, must be within (%d,%d)",
				nLow,nHigh,l,h);
			MessageBox(msg,"Bad Color Range given to SliderEx Control");
			return FALSE;
		}
	}

	clrRange tmp;
	tmp.lval = nLow;
	tmp.hval = nHigh;
	tmp.strColor = strColor;
	tmp.endColor = endColor;
	colorList.Add(tmp);

	return TRUE;
}

void CSliderCtrlEx::clearColors()
{
	colorList.RemoveAll();
}

int CSliderCtrlEx::setErrResponse(int resp)
{
	int retval = errResponse;
	errResponse = resp;
	return retval;
}

void CSliderCtrlEx::SetRangeMin(int nMin, BOOL bRedraw)
{
	int l = GetRangeMin();
	if(nMin < l)
	{
		clearColors();	// old color ranges are almost certainly bogus
	}
	CSliderCtrl::SetRangeMin(nMin,bRedraw);
}

void CSliderCtrlEx::SetRange(int nMin, int nMax, BOOL bRedraw)
{
	int l,h;
	GetRange(l,h);
	if((nMin < l) || (nMax > h))
	{
		clearColors();
	}
	CSliderCtrl::SetRange(nMin,nMax,bRedraw);
}

void CSliderCtrlEx::SetRangeMax(int nMax, BOOL bRedraw)
{
	int h = GetRangeMax();
	if(nMax < h)
	{
		clearColors();
	}
	CSliderCtrl::SetRangeMax(nMax,bRedraw);
}

//////////////////////////////////////////////////////////////////////////////
// set callback function for reporting slider position.
// Typically, p2Object is the caller's 'this' value (class instance pointer).
// Typically, data1 is a control ID or a control pointer.
//////////////////////////////////////////////////////////////////////////////
void CSliderCtrlEx::setCallback(void *funcptr, void *p2Object, LPARAM data1)
{
	m_p2Object = p2Object;	// pointer to class instance of caller, could be NULL
	m_data1 = data1;
	m_Callback = (ptr2Func)funcptr;
}

// clear out the callback function
void CSliderCtrlEx::clearCallback(void)
{
	m_Callback = NULL;
	m_p2Object = 0;
	m_data1 = 0;
}

// When reporting slider position, we also indicate if it is being dragged by the mouse
void CSliderCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_IsDragging = TRUE;
	CSliderCtrl::OnLButtonDown(nFlags, point);
}

void CSliderCtrlEx::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_IsDragging = FALSE;
	CSliderCtrl::OnLButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Utility to save a bitmap to file for later examination. Used for debugging only.
// Based on an article by "gelbert" on www.experts-exchange.com at
// http://www.experts-exchange.com/Programming/Programming_Languages/MFC/Q_20193761.html
//
// This version is a member function and instead of taking an HBITMAP as the 2nd
// parameter, it take a reference to a CBitmap. Otherwise, it is identical.
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSliderCtrlEx::SaveBitmap(LPCSTR lpFileName, CBitmap &hBitmap, HPALETTE hPal)
{
	BOOL bResult = FALSE;
	
	PICTDESC stPictDesc;
	stPictDesc.cbSizeofstruct = sizeof(PICTDESC);
	stPictDesc.picType = PICTYPE_BITMAP;
	stPictDesc.bmp.hbitmap = hBitmap;
	stPictDesc.bmp.hpal = hPal;
	
	LPPICTURE pPicture;
	HRESULT hr = OleCreatePictureIndirect( &stPictDesc, IID_IPicture, FALSE,
		reinterpret_cast<void**>(&pPicture) );
	if ( SUCCEEDED(hr) )
	{
		LPSTREAM pStream;
		hr = CreateStreamOnHGlobal( NULL, TRUE, &pStream );
		if ( SUCCEEDED(hr) )
		{
			long lBytesStreamed = 0;
			hr = pPicture->SaveAsFile( pStream, TRUE, &lBytesStreamed );
			if ( SUCCEEDED(hr) )
			{
				HANDLE hFile = CreateFile( lpFileName,
					GENERIC_WRITE,
					FILE_SHARE_READ,
					NULL,
					CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL );
				if ( hFile )
				{
					HGLOBAL hMem = NULL;
					GetHGlobalFromStream( pStream, &hMem );
					LPVOID lpData = GlobalLock( hMem );
					
					DWORD dwBytesWritten;
					bResult = WriteFile( hFile, lpData, lBytesStreamed, &dwBytesWritten, NULL );
					bResult &= ( dwBytesWritten == (DWORD)lBytesStreamed );
					
					// clean up
					GlobalUnlock(hMem);
					CloseHandle(hFile);
				}
			}
			// clean up
			pStream->Release();
		}
		// clean up
		pPicture->Release();
	}
	
	return bResult;
}

void CSliderCtrlEx::Refresh()
{
	// This is typically called after changing colors or ranges or the like.
	// It forces a complete and immediate redrawing of the control
	int min,max;
	GetRange(min,max);
	CSliderCtrl::SetRange(min,max,TRUE);
}
