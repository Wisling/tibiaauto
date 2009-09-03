// GroupBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "GroupBoxEx.h"
#include "Skin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupBoxEx

CGroupBoxEx::CGroupBoxEx(){
}

CGroupBoxEx::~CGroupBoxEx()
{
}


BEGIN_MESSAGE_MAP(CGroupBoxEx, CWnd)
	//{{AFX_MSG_MAP(CGroupBoxEx)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGroupBoxEx message handlers

void CGroupBoxEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	CPen newPen;
	CPen *oldPen;
	CRect rc = NULL;
	GetClientRect(rc);
	rc.top += 4;
	CString text;
	this->GetWindowText(text);

	int textSize = 0;
	for (int loop = 0; loop < text.GetLength(); loop++) {
		int buffer = 0;
		dc.GetCharWidth(text[loop], text[loop], &buffer);
		textSize += buffer;
	}
	
	LOGFONT logFont;
	dc.GetCurrentFont()->GetLogFont(&logFont);

	newPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	oldPen = dc.SelectObject(&newPen);

	dc.MoveTo(10, rc.top);
	dc.LineTo(rc.left, rc.top);
	dc.LineTo(rc.left, rc.bottom);
	dc.LineTo(rc.right, rc.bottom);
	dc.LineTo(rc.right, rc.top);
	dc.LineTo(12 + textSize, rc.top);

	dc.SelectObject(oldPen);
	newPen.DeleteObject();

	rc.InflateRect(-1, -1);
	newPen.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	oldPen = dc.SelectObject(&newPen);

	dc.MoveTo(10, rc.top);
	dc.LineTo(rc.left, rc.top);
	dc.LineTo(rc.left, rc.bottom);
	dc.LineTo(rc.right, rc.bottom);
	dc.LineTo(rc.right, rc.top);
	dc.LineTo(12 + textSize, rc.top);

	dc.SelectObject(oldPen);
	newPen.DeleteObject();

/*	newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_SecondaryBackgroundRedValue,skin.m_SecondaryBackgroundGreenValue,skin.m_SecondaryBackgroundBlueValue));
	oldPen = dc.SelectObject(&newPen);

	dc.MoveTo(11, rc.top);
	dc.LineTo(16 + (logFont.lfWidth * text.GetLength()), rc.top);
	dc.LineTo(16 + (logFont.lfWidth * text.GetLength()), rc.top - 1);
	dc.LineTo(11, rc.top - 1);

	dc.SelectObject(oldPen);
	newPen.DeleteObject();*/

	rc.top += 1;
	rc.left += 1;
	CBrush myBrush;
	myBrush.CreateSolidBrush(RGB(skin.m_SecondaryBackgroundRedValue,skin.m_SecondaryBackgroundGreenValue,skin.m_SecondaryBackgroundBlueValue));
	dc.FillRect(rc, &myBrush);

	rc.top = -1;
	rc.left = 12; 
	rc.right = 13 + textSize;
	rc.bottom = 16;
	logFont.lfWeight = FW_BOLD;
	CFont font;
	font.CreateFontIndirect(&logFont);
	CFont* def_font = dc.SelectObject(&font);
	dc.SetTextColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(::GetStockObject(HOLLOW_BRUSH));

	dc.DrawText(text, rc, NULL);

	dc.SelectObject(def_font);
	font.DeleteObject(); 
}
