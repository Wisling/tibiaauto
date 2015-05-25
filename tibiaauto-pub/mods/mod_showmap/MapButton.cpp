// MapButton.cpp : implementation file
//

#include "stdafx.h"
#include "MapButton.h"
#include "TibiaMapPoint.h"
#include "ToolMapShow.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapButton

CMenu *CMapButton::mapActionsMenu = NULL;

CMapButton::CMapButton(int x, int y)
{
	m_x   = x;
	m_y   = y;
	realX = realY = realZ = 0;
	if (mapActionsMenu == NULL)
	{
		// if menu object is not yet created then just create it
		mapActionsMenu = new CMenu();
		mapActionsMenu->LoadMenu(IDR_SHOWMAP_MENU);
	}
}

CMapButton::~CMapButton()
{
	if (mapActionsMenu != NULL)
	{
		delete mapActionsMenu;
		mapActionsMenu = NULL;
	}
}


BEGIN_MESSAGE_MAP(CMapButton, CButton)
//{{AFX_MSG_MAP(CMapButton)
ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
ON_WM_CTLCOLOR_REFLECT()
ON_WM_RBUTTONDOWN()
ON_WM_MOUSEMOVE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapButton message handlers

void CMapButton::OnClicked()
{
}

HBRUSH CMapButton::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return NULL;
}


void CMapButton::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMemReaderProxy reader;
	// m_value == -2 is self point
	if (m_value != -2)
	{
		CTibiaCharacter *self = reader.readSelfCharacter();

		realX = self->x + m_x - 10;
		realY = self->y + m_y - 10;
		realZ = self->z;

		delete self;
		int i;
		/* Get the mouse cursor position */
		DWORD dwPos = GetMessagePos();

		/* Convert the co-ords into a CPoint structure */
		CPoint pt( dwPos & 0xffff, (dwPos >> 16) & 0xffff );

		for (i = 0; i < 13; i++)
		{
			mapActionsMenu->GetSubMenu(0)->CheckMenuItem(i, MF_BYPOSITION | MF_UNCHECKED);
		}
		int itemNum;
		switch(m_value)
		{
		case -1: itemNum  = 0; break;
		case 0: itemNum   = 1; break;
		case 301: itemNum = 4; break;
		case 302: itemNum = 5; break;
		case 303: itemNum = 6; break;
		case 201:
		case 202:
		case 203:
		case 204: itemNum = 8; break;
		case 101:
		case 102:
		case 103: itemNum = 10; break;
		}

		mapActionsMenu->GetSubMenu(0)->CheckMenuItem(itemNum, MF_BYPOSITION | MF_CHECKED);
		if(m_locked)
			mapActionsMenu->GetSubMenu(0)->CheckMenuItem(2, MF_BYPOSITION | MF_CHECKED);

		mapActionsMenu->GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, this);
	}
	CButton::OnRButtonDown(nFlags, point);
}

BOOL CMapButton::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CMemReaderProxy reader;
	if (wParam == ID_SHOWMAPOPTIONS_LOCKTILE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointToggleLock(realX, realY, realZ);
	}
	if (wParam == ID_SHOWMAPOPTIONS_CLEAR)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, -1);
	}
	if (wParam == ID_SHOWMAPOPTIONS_AVAILABLE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 0);
	}
	if (wParam == ID_SHOWMAPOPTIONS_OPENHOLE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 101);
	}
	if (wParam == ID_SHOWMAPOPTIONS_CLOSEDHOLE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 102);
	}
	if (wParam == ID_SHOWMAPOPTIONS_CRATE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 103);
	}
	if (wParam == ID_SHOWMAPOPTIONS_ROPE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 201);
	}
	if (wParam == ID_SHOWMAPOPTIONS_MAGICROPE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 202);
	}
	if (wParam == ID_SHOWMAPOPTIONS_LADDER)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 203);
	}
	if (wParam == ID_SHOWMAPOPTIONS_STAIRS)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 204);
	}
	if (wParam == ID_SHOWMAPOPTIONS_DEPOT)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 301);
	}
	if (wParam == ID_SHOWMAPOPTIONS_TELE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 302);
	}
	if (wParam == ID_SHOWMAPOPTIONS_BLOCK)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, 303);
	}
	if (wParam == IDD_TOOL_MAPSHOW_CONFIG)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		if (parent->m_extendedResearch.GetCheck())
		{
			if (reader.mapGetPointItemId(point(m_x, m_y, 0), 0))
				parent->ShowMapConfig(m_x, m_y);
			else
				AfxMessageBox("You can only configure tiles that have been researched!");
		}
		else
		{
			AfxMessageBox("You must enable Extended Research first!");
		}
	}
	CToolMapShow *parent = (CToolMapShow *)GetParent();
	parent->refreshVisibleMap();

	return CButton::OnCommand(wParam, lParam);
}

void CMapButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CToolMapShow *parent = (CToolMapShow *)GetParent();

	parent->showTileDetails(m_x, m_y);

	CButton::OnMouseMove(nFlags, point);
}
