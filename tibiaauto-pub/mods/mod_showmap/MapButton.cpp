// MapButton.cpp : implementation file
//

#include "stdafx.h"
#include "MapButton.h"
#include "TibiaMapPoint.h"
#include "ToolMapShow.h"
#include <MemReader.h>

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
	CMemReader& reader = CMemReader::getMemReader();
	// m_value == MAP_POINT_TYPE_SELF is self point
	if (m_value != MAP_POINT_TYPE_SELF)
	{
		CTibiaCharacter self;
		reader.readSelfCharacter(&self);

		realX = self.x + m_x - 10;
		realY = self.y + m_y - 10;
		realZ = self.z;

		int i;
		/* Get the mouse cursor position */
		DWORD dwPos = GetMessagePos();

		/* Convert the co-ords into a CPoint structure */
		CPoint pt(dwPos & 0xffff, (dwPos >> 16) & 0xffff);

		for (i = 0; i < 13; i++)
		{
			mapActionsMenu->GetSubMenu(0)->CheckMenuItem(i, MF_BYPOSITION | MF_UNCHECKED);
		}
		int itemNum;
		switch (m_value)
		{
		case MAP_POINT_TYPE_CLEAR:
			itemNum = 0;
			break;
		case MAP_POINT_TYPE_AVAILABLE:
			itemNum = 1;
			break;
		case MAP_POINT_TYPE_DEPOT:
			itemNum = 4;
			break;
		case MAP_POINT_TYPE_TELEPORT:
			itemNum = 5;
			break;
		case MAP_POINT_TYPE_USABLE_TELEPORT:
			itemNum = 6;
			break;
		case MAP_POINT_TYPE_BLOCK:
			itemNum = 7;
			break;
		case MAP_POINT_TYPE_ROPE:
		case MAP_POINT_TYPE_MAGICROPE:
		case MAP_POINT_TYPE_LADDER:
		case MAP_POINT_TYPE_STAIRS:
			itemNum = 9;
			break;
		case MAP_POINT_TYPE_OPEN_HOLE:
		case MAP_POINT_TYPE_CLOSED_HOLE:
		case MAP_POINT_TYPE_CRATE:
			itemNum = 11;
			break;
		}

		mapActionsMenu->GetSubMenu(0)->CheckMenuItem(itemNum, MF_BYPOSITION | MF_CHECKED);
		if (m_locked)
			mapActionsMenu->GetSubMenu(0)->CheckMenuItem(2, MF_BYPOSITION | MF_CHECKED);

		mapActionsMenu->GetSubMenu(0)->TrackPopupMenu(0, pt.x, pt.y, this);
	}
	CButton::OnRButtonDown(nFlags, point);
}

BOOL CMapButton::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CMemReader& reader = CMemReader::getMemReader();
	if (wParam == ID_SHOWMAPOPTIONS_LOCKTILE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointToggleLock(realX, realY, realZ);
	}
	else if (wParam == ID_SHOWMAPOPTIONS_CLEAR)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_CLEAR);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_AVAILABLE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_AVAILABLE);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_OPENHOLE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_OPEN_HOLE);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_CLOSEDHOLE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_CLOSED_HOLE);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_CRATE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_CRATE);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_ROPE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_ROPE);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_MAGICROPE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_MAGICROPE);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_LADDER)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_LADDER);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_STAIRS)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_STAIRS);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_DEPOT)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_DEPOT);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_TELE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_TELEPORT);
	}
	else if (wParam == ID_SHOWMAPOPTIONS_USETELE)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_USABLE_TELEPORT);
	}
	else if(wParam == ID_SHOWMAPOPTIONS_BLOCK)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		parent->mapPointClicked(realX, realY, realZ, MAP_POINT_TYPE_BLOCK);
	}
	else if(wParam == IDD_TOOL_MAPSHOW_CONFIG)
	{
		CToolMapShow *parent = (CToolMapShow *)GetParent();
		if (reader.mapGetPointItemId(point(m_x - 10, m_y - 10, 0), 0))
			parent->ShowMapConfig(m_x, m_y);
		else
			AfxMessageBox("You can only configure tiles that have been researched!");
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
