// MyDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MyDialog.h"
#include "Skin.h"
#include "BtnST.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MyDialog dialog


MyDialog::MyDialog(UINT nIDTemplate, CWnd* pParent)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(MyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	CMemReaderProxy reader;
	char installPath[1024] = {'\0'};
	unsigned long installPathLen=1023;
	HKEY hkey=NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Tibia Auto\\",0,KEY_ALL_ACCESS,&hkey)) {
		RegQueryValueEx(hkey,TEXT("Install_Dir"),NULL,NULL,(unsigned char *)installPath,&installPathLen );
		RegCloseKey(hkey);
	}
	if (!strlen(installPath)) {
		AfxMessageBox("ERROR! Unable to read TA install directory! Please reinstall!");
		exit(1);
	}
	char buf[2048];
	sprintf(buf,"%s\\skins\\CurrentSkin.skin",installPath);
	CString currentPathBuf = buf;

	skin = reader.loadCurrentSkin(currentPathBuf);

	backBrush.CreateSolidBrush(RGB(skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue));
	foreBrush.CreateSolidBrush(RGB(skin.m_SecondaryBackgroundRedValue, skin.m_SecondaryBackgroundGreenValue, skin.m_SecondaryBackgroundBlueValue));
}

MyDialog::~MyDialog()
{
	//{{AFX_DATA_INIT(MyDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	backBrush.DeleteObject();
	foreBrush.DeleteObject();
	editBrush.DeleteObject();
}


void MyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MyDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL MyDialog::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);
	CBrush bkbr;
	bkbr.CreateSolidBrush(RGB(skin.m_PrimaryBackgroundRedValue, skin.m_PrimaryBackgroundGreenValue, skin.m_PrimaryBackgroundBlueValue));
	pDC->FillRect(rect,&bkbr);
	DeleteObject(bkbr);
	return true;
}

BEGIN_MESSAGE_MAP(MyDialog, CDialog)
	//{{AFX_MSG_MAP(MyDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MyDialog message handlers

HBRUSH MyDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// TODO: Change any attributes of the DC here

	// TODO: Return a different brush if the default is not desired
	foreBrush.DeleteObject();
	foreBrush.CreateSolidBrush(RGB(skin.m_SecondaryBackgroundRedValue, skin.m_SecondaryBackgroundGreenValue, skin.m_SecondaryBackgroundBlueValue));
	
	switch (nCtlColor) {
 
	case CTLCOLOR_EDIT:
		pDC->SetTextColor(RGB(skin.m_EditboxRedValue, skin.m_EditboxGreenValue, skin.m_EditboxBlueValue));
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
		break;
	case CTLCOLOR_STATIC:
		pDC->SetTextColor(RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
		pDC->SetBkMode(TRANSPARENT);
		if (pWnd->GetExStyle() & WS_EX_TRANSPARENT) {
			return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
		}
		return (HBRUSH)foreBrush;
		break;
	default:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
		break;
 	}
}

/*void MyDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	CPen newPen;
	CPen *oldPen;
	CDC	*pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rc = lpDrawItemStruct->rcItem;
	// This code only works with buttons.
	//ASSERT(lpDrawItemStruct->CtlType == ODT_);
	HWND mine = lpDrawItemStruct->hwndItem;
 
	HBRUSH myBrush = CreateSolidBrush(RGB(skin.m_ButtonFaceRedValue,skin.m_ButtonFaceGreenValue,skin.m_ButtonFaceBlueValue));
	FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, myBrush);
	// If drawing selected, add the pushed style to DrawFrameControl.
	if (lpDrawItemStruct->itemState & ODS_SELECTED) {

		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonDSRedValue, skin.m_ButtonDSGreenValue, skin.m_ButtonDSBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom);
		pDC->LineTo(rc.left, rc.top);
		pDC->LineTo(rc.right, rc.top);
		newPen.DeleteObject();

		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonHighLightRedValue, skin.m_ButtonHighLightGreenValue, skin.m_ButtonHighLightBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.top);
		newPen.DeleteObject();
		
		rc.InflateRect(-1, -1);

		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonLSRedValue, skin.m_ButtonLSGreenValue, skin.m_ButtonLSBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom);
		pDC->LineTo(rc.left, rc.top);
		pDC->LineTo(rc.right, rc.top);
		newPen.DeleteObject();

		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonLightRedValue, skin.m_ButtonLightGreenValue, skin.m_ButtonLightBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.top);
		newPen.DeleteObject();

		rc = lpDrawItemStruct->rcItem;
		rc.top += 2;
		rc.left += 2;

		// Get the 's text.
		char strText[32];
		::GetWindowText(lpDrawItemStruct->hwndItem, strText, 31);
		CString text = strText;
		// Draw the  text using the text color red.
		::SetBkColor(lpDrawItemStruct->hDC, RGB(skin.m_ButtonFaceRedValue,skin.m_ButtonFaceGreenValue,skin.m_ButtonFaceBlueValue));
		COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
		::DrawText(lpDrawItemStruct->hDC, text, text.GetLength(),
			&rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
	}
	else {
		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonHighLightRedValue, skin.m_ButtonHighLightGreenValue, skin.m_ButtonHighLightBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom);
		pDC->LineTo(rc.left, rc.top);
		pDC->LineTo(rc.right, rc.top);
		newPen.DeleteObject();
		
		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonDSRedValue, skin.m_ButtonDSGreenValue, skin.m_ButtonDSBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.top);
		newPen.DeleteObject();
		
		rc.InflateRect(-1, -1);
		
		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonLightRedValue, skin.m_ButtonLightGreenValue, skin.m_ButtonLightBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom);
		pDC->LineTo(rc.left, rc.top);
		pDC->LineTo(rc.right, rc.top);
		newPen.DeleteObject();
		
		newPen.CreatePen(PS_SOLID, 1, RGB(skin.m_ButtonLSRedValue, skin.m_ButtonLSGreenValue, skin.m_ButtonLSBlueValue));
		oldPen = pDC->SelectObject(&newPen);
		
		pDC->MoveTo(rc.left, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.bottom - 1);
		pDC->LineTo(rc.right - 1, rc.top);
		newPen.DeleteObject();

		// Get the 's text.
		char strText[32];
		::GetWindowText(lpDrawItemStruct->hwndItem, strText, 31);
		CString text = strText;
		// Draw the  text using the text color red.
		::SetBkColor(lpDrawItemStruct->hDC, RGB(skin.m_ButtonFaceRedValue,skin.m_ButtonFaceGreenValue,skin.m_ButtonFaceBlueValue));
		COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(skin.m_TextRedValue, skin.m_TextGreenValue, skin.m_TextBlueValue));
		::DrawText(lpDrawItemStruct->hDC, text, text.GetLength(),
			&lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
	}
	DeleteObject(myBrush);
	newPen.DeleteObject();
	oldPen->DeleteObject();
}*/
