// MyMenu.h: interface for the CMyMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYMENU_H__EDDCCDA7_D18F_4846_831D_E2F113ACA81D__INCLUDED_)
#define AFX_MYMENU_H__EDDCCDA7_D18F_4846_831D_E2F113ACA81D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyMenuData.h"
#include <vector>
using namespace std;

class CMyMenu : public CMenu  
{
public:
	CMyMenu();
	virtual ~CMyMenu();
	void LoadOwnerDrawMenu(CMenu *baseMenu);
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
private:
	vector<CMyMenu*> rgpMyMenu;
	vector<CMyMenuData*> rgpMyMenuData;
};


#endif // !defined(AFX_MYMENU_H__EDDCCDA7_D18F_4846_831D_E2F113ACA81D__INCLUDED_)
