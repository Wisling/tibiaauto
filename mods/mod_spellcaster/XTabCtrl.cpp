// XTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "XTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl

CXTabCtrl::CXTabCtrl()
{
	m_iSelectedTab = 0;
	m_ptTabs.x = 3;
	m_ptTabs.y = 39;

	m_crSelected = 0;
	m_crDisabled = 0;
	m_crNormal = 0;
	m_crMouseOver = 0;
	m_crTab = 0;

	m_bColorSelected  = false;
	m_bColorDisabled  = false;
	m_bColorNormal    = false;
	m_bColorMouseOver = false;
	m_bColorTab = false;


	m_iIndexMouseOver = -1;

	m_bMouseOver = false;
}

CXTabCtrl::~CXTabCtrl()
{
	m_arrayStatusTab.RemoveAll();
}


BEGIN_MESSAGE_MAP(CXTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CXTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl message handlers

void CXTabCtrl::SetTopLeftCorner(CPoint pt)
{
	m_ptTabs.x = pt.x;
	m_ptTabs.y = pt.y;
}

void CXTabCtrl::SetMouseOverColor(COLORREF cr) 
{
	m_bColorMouseOver = true;
	m_crMouseOver = cr;
}


void CXTabCtrl::SetDisabledColor(COLORREF cr) 
{
	m_bColorDisabled = true;
	m_crDisabled = cr;
}

void CXTabCtrl::SetSelectedColor(COLORREF cr)
{
	m_bColorSelected = true;
	m_crSelected = cr;
}


void CXTabCtrl::SetNormalColor(COLORREF cr)
{
	m_bColorNormal = true;
	m_crNormal = cr;
}

void CXTabCtrl::SetTabColor(COLORREF cr)
{
	m_bColorTab = true;
	m_crTab = cr;
}

void CXTabCtrl::AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage)
{
	ASSERT_VALID(pWnd);

	TCITEM item;
	item.mask = TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE;
	item.lParam = (LPARAM) pWnd;
	item.pszText = lpszCaption;
	item.iImage = iImage;

	int iIndex = m_arrayStatusTab.GetSize();
	InsertItem(iIndex, &item);

	pWnd->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y , 0, 0,
						SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER);
	
	pWnd->ShowWindow(iIndex ? SW_HIDE : SW_SHOW);

	//** the initial status is enabled
	m_arrayStatusTab.Add(TRUE);
}

void CXTabCtrl::EnableTab(int iIndex, BOOL bEnable)
{
	ASSERT(iIndex < m_arrayStatusTab.GetSize());

	//** if it should change the status ----
	if (m_arrayStatusTab[iIndex] != bEnable)
	{
		m_arrayStatusTab[iIndex] = bEnable;

		//** redraw the item -------
		CRect rect;

		GetItemRect(iIndex, &rect);
		InvalidateRect(rect);
	}
}

void CXTabCtrl::DeleteAllTabs()
{
	m_arrayStatusTab.RemoveAll();
	
	DeleteAllItems();

}

void CXTabCtrl::DeleteTab(int iIndex)
{
	ASSERT(iIndex < m_arrayStatusTab.GetSize());

	m_arrayStatusTab.RemoveAt(iIndex);

	DeleteItem(iIndex);

}


BOOL CXTabCtrl::IsTabEnabled(int iIndex)
{
	ASSERT(iIndex < m_arrayStatusTab.GetSize());
	
	return m_arrayStatusTab[iIndex];
}


BOOL CXTabCtrl::SelectTab(int iIndex)
{
	ASSERT(iIndex < m_arrayStatusTab.GetSize());

	if (GetCurSel() == iIndex)
		return TRUE;

	//** check if the tab is enabled --------
	if (m_arrayStatusTab[iIndex])
	{
		TCITEM item;
		CWnd* pWnd;
		item.mask = TCIF_PARAM;

		GetItem(GetCurSel(), &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_HIDE);

		SetCurSel(iIndex);
		GetItem(iIndex, &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_SHOW);

		return TRUE;
	}

	return FALSE;
}

void CXTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iNewTab = GetCurSel();

	if (!IsTabEnabled(iNewTab))
	{
		SetCurSel(m_iSelectedTab);
	}
	else
	{
		TCITEM item;
		CWnd* pWnd;

		item.mask = TCIF_PARAM;
		
		//** hide the current tab ---------
		GetItem(m_iSelectedTab, &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_HIDE);

		//** show the selected tab --------
		GetItem(iNewTab, &item);
		pWnd = reinterpret_cast<CWnd*> (item.lParam);
		ASSERT_VALID(pWnd);
		pWnd->ShowWindow(SW_SHOW);

	}
	*pResult = 0;
}

void CXTabCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_iSelectedTab = GetCurSel();

	*pResult = 0;
}



void CXTabCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CTabCtrl::PreSubclassWindow();
	ModifyStyle(0, TCS_OWNERDRAWFIXED);
}


void CXTabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{	
	CRect rect;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC)
		return;

	rect = lpDrawItemStruct->rcItem;
	rect.top += ::GetSystemMetrics(SM_CYEDGE);

	int nTabIndex = lpDrawItemStruct->itemID;
	
	if (nTabIndex < 0) return;

	BOOL bSelected = (nTabIndex == GetCurSel());

	COLORREF crSelected = m_bColorSelected ? m_crSelected : GetSysColor(COLOR_BTNTEXT);
	COLORREF crNormal  = m_bColorNormal   ? m_crNormal   : GetSysColor(COLOR_BTNTEXT);
	COLORREF crDisabled = m_bColorDisabled ? m_crDisabled : GetSysColor(COLOR_GRAYTEXT);
	COLORREF crTab = m_bColorTab ? m_crTab : GetSysColor(COLOR_BTNFACE);


	char label[64];
	TC_ITEM item;
	item.mask = TCIF_TEXT|TCIF_IMAGE;
	item.pszText = label;     
	item.cchTextMax = 63;    	
	if (!GetItem(nTabIndex, &item))
		return;

	int nSavedDC = pDC->SaveDC();

	CRect rectItem;
	POINT pt;

	GetItemRect(nTabIndex, &rectItem);
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if (rectItem.PtInRect(pt))
		m_iIndexMouseOver = nTabIndex;



	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rect, crTab);

	//**  Draw the image
	CImageList* pImageList = GetImageList();
	if (pImageList && item.iImage >= 0) 
	{

		rect.left += pDC->GetTextExtent(_T(" ")).cx;

		IMAGEINFO info;
		pImageList->GetImageInfo(item.iImage, &info);
		CRect ImageRect(info.rcImage);
		int nYpos = rect.top;

		pImageList->Draw(pDC, item.iImage, CPoint(rect.left, nYpos), ILD_TRANSPARENT);
		rect.left += ImageRect.Width();
	}

	if (!IsTabEnabled(nTabIndex))
	{
		pDC->SetTextColor(crDisabled);
		rect.top -= ::GetSystemMetrics(SM_CYEDGE);
		pDC->DrawText(label, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
	else
	{
		//** selected item -----
		if (bSelected)
			pDC->SetTextColor(crSelected);
		else //** other item ---
		{
			if (m_bColorMouseOver && nTabIndex == m_iIndexMouseOver) 
			{
				pDC->SetTextColor(m_crMouseOver);	
			}
			else
			{
				pDC->SetTextColor(crNormal);
			}
		}

		rect.top -= ::GetSystemMetrics(SM_CYEDGE);
		pDC->DrawText(label, rect, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}

	pDC->RestoreDC(nSavedDC);
}

void CXTabCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//** if we should change the color of the tab ctrl ---
	if (m_bColorMouseOver)
	{
		SetTimer(1,10,NULL);

		if (m_iIndexMouseOver != -1)
		{
			CRect rectItem;
			GetItemRect(m_iIndexMouseOver, rectItem);
			if (!rectItem.PtInRect(point))
			{
				CRect rectOldItem;
				GetItemRect(m_iIndexMouseOver, rectOldItem);
				m_iIndexMouseOver = -1;
				InvalidateRect(rectOldItem);
				return;
			}
		}
	

		if (!m_bMouseOver)
		{
			TCHITTESTINFO hitTest;

			m_bMouseOver = true;
			hitTest.pt = point;

			int iItem = HitTest(&hitTest);
			if (iItem != -1 && m_arrayStatusTab[iItem])
			{
				RECT rectItem;
				GetItemRect(iItem, &rectItem);

				InvalidateRect(&rectItem);
			}
		}
	}
	
	CTabCtrl::OnMouseMove(nFlags, point);
}

void CXTabCtrl::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	POINT pt;
	GetCursorPos(&pt);
	CRect rectItem, rectScreen;

	GetItemRect(m_iIndexMouseOver, rectItem);
	rectScreen = rectItem;
	ClientToScreen(rectScreen);
	
	// If mouse leaves, show normal
	if (!rectScreen.PtInRect(pt)) 
	{
		KillTimer (1);
		m_bMouseOver = false;
		m_iIndexMouseOver = -1;
		InvalidateRect(rectItem);
	}


	CTabCtrl::OnTimer(nIDEvent);
}

void CXTabCtrl::ChangeTab(int iIndex, CWnd *pNewTab, LPTSTR lpszCaption, int iImage)
{
	ASSERT_VALID(pNewTab);

	TCITEM item;
	item.mask = TCIF_TEXT|TCIF_PARAM|TCIF_IMAGE;
	item.lParam = (LPARAM) pNewTab;
	item.pszText = lpszCaption;
	item.iImage = iImage;

	//** update item 
	VERIFY(SetItem(iIndex, &item));

	pNewTab->SetWindowPos(NULL, m_ptTabs.x, m_ptTabs.y , 0, 0,
						SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER);
	
	pNewTab->ShowWindow(SW_HIDE);

	//** the initial status is enabled
	m_arrayStatusTab[iIndex] = TRUE; 

	//** updates the screen
	GetItem(GetCurSel(), &item);
	((CWnd*)item.lParam)->ShowWindow(SW_SHOW);
}

BOOL CXTabCtrl::SelectTabByName(LPCTSTR lpszTabCaption)
{
	TCHAR szName[64];
	TCITEM item;
	
	memset(&item, 0, sizeof(item));

	item.mask       = TCIF_TEXT;
	item.cchTextMax = sizeof(szName);
	item.pszText    = szName;

	for (int i=0; i < GetItemCount(); i++)
	{
		GetItem(i, &item);

		if (strcmp(lpszTabCaption, item.pszText) == 0)
			return SelectTab(i);
	}

	return FALSE;
}

BOOL CXTabCtrl::SelectNextTab(BOOL bForward)
{
	int iSum = bForward ? 1 : -1;

	int iCurSel = GetCurSel();
	int iCurSelSave(iCurSel);

	do
	{
		iCurSel += iSum;

		if (iCurSel < 0 || iCurSel == GetItemCount())
			return FALSE;

		if (IsTabEnabled(iCurSel))
		{
			if (iCurSel < GetItemCount())
			{
				SelectTab(iCurSel);

				return TRUE;
			}

			return FALSE;
		}

	}
	while (iCurSel != iCurSelSave);
	
	return FALSE;
}