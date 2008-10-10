// MyMenu.cpp: implementation of the CMyMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyMenu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyMenu::CMyMenu()
{

}

CMyMenu::~CMyMenu()
{

} 
void CMyMenu::LoadOwnerDrawMenu(CMenu *baseMenu)
{	
	/*
	int iMenuCount = baseMenu->GetMenuItemCount();
	for (int i=0;i<iMenuCount;i++)
	{
		CString str;		
		CMyMenuData* pMenuData = new CMyMenuData();
		baseMenu->GetMenuString(i, str, MF_BYPOSITION);
		pMenuData->m_strCaption=str;
		baseMenu->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, 0, (LPCTSTR)pMenuData);		
		CMenu *subMenu = baseMenu->GetSubMenu(i);
		if (subMenu)
		{
			CMenu *newSubMenu = new CMenu();
			newSubMenu->Attach(subMenu->GetSafeHmenu());
			LoadOwnerDrawMenu(newSubMenu);			
			newSubMenu->Detach();
		}		
		
		
	}


	return;
	*/
	
	CMenu *pMyMenu = baseMenu;
	CString str;	//use to hold the caption temporarily
	CMyMenu* pMenu;	//use to hold the sub menu
	CMyMenuData* pMenuData;	//use to hold the menu data
	//get the number of the menu items of the parent menu
	int iMenuCount = pMyMenu->GetMenuItemCount();
	UINT nID;	//use to hold the identifier of the menu items
	for (int i=0; i<iMenuCount; i++)
	{
		//get the caption of the menu item
   		pMyMenu->GetMenuString(i, str, MF_BYPOSITION);
		pMenu = 0;	//reset pointer for safety
		pMenuData = 0; //reset pointer for safety
		pMenuData = new CMyMenuData;
		pMenuData->m_strCaption = str;
		rgpMyMenuData.push_back(pMenuData);
		
		nID = pMyMenu->GetMenuItemID(i);
		if (nID>=65535)
		{
			nID=0;
		}
		if (pMyMenu->GetSubMenu(i))	//if the parent menu has sub menu
		{
			pMyMenu->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, nID , (LPCTSTR)pMenuData);
			pMenu = new CMyMenu;
			rgpMyMenu.push_back(pMenu);
			HMENU hMenu = pMyMenu->GetSubMenu(i)->GetSafeHmenu();
			pMenu->Attach(hMenu);
			LoadOwnerDrawMenu(pMenu);			
		}
		else
		{
			
			pMyMenu->ModifyMenu(i, MF_BYPOSITION | MF_OWNERDRAW, nID , (LPCTSTR)pMenuData);
		}
	}

	
}


void CMyMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{	
	CMyMenuData* pMyMenuData = (CMyMenuData*)lpMeasureItemStruct->itemData;
	//get the caption of the menu item
	CString str = pMyMenuData->m_strCaption;
	//assign the height of the menu item
	lpMeasureItemStruct->itemHeight = 23;
	//assign the width of the menu item
	lpMeasureItemStruct->itemWidth = str.GetLength()*5;				
}

void CMyMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	//draw an orange background
	pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0,0,0));	
	//if the menu item is selected	
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		//draw a blue background
		pDC->FillSolidRect(&lpDrawItemStruct->rcItem, RGB(0,150,255));
	}
		
	CMyMenuData* pMyMenuData = (CMyMenuData*)lpDrawItemStruct->itemData;

	CString str = pMyMenuData->m_strCaption;	
	
	//draw the caption of the menu item	
	pDC->SetTextColor(RGB(255,255,255));
	pDC->TextOut(lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top, str);
	
}