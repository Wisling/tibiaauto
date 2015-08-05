/*
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#include "stdafx.h"
#include "mod_itemconfig.h"

#include "ConfigData.h"
#include "TibiaContainer.h"
#include <TibiaItem.h>

#include <MemReader.h>
#include <PackSender.h>
#include "ModuleUtil.h"
#include "TibiaMapPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CMod_itemconfigApp

BEGIN_MESSAGE_MAP(CMod_itemconfigApp, CWinApp)
//{{AFX_MSG_MAP(CMod_itemconfigApp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions


/////////////////////////////////////////////////////////////////////////////
// CMod_itemconfigApp construction

CMod_itemconfigApp::CMod_itemconfigApp()
{
	m_infoDialog = NULL;
}

CMod_itemconfigApp::~CMod_itemconfigApp()
{
	if (m_infoDialog)
	{
		m_infoDialog->cleanup();
		delete m_infoDialog;
	}
	delete m_configData;
}

char * CMod_itemconfigApp::getName()
{
	return "Item Configuration";
}

int CMod_itemconfigApp::isStarted()
{
	return 0;
}

void CMod_itemconfigApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!m_infoDialog)
	{
		m_infoDialog = new CToolItemConfig();
		m_infoDialog->Create(IDD_TOOL_ITEMCONFIG);
	}
	m_infoDialog->ShowWindow(SW_SHOW);
}

void CMod_itemconfigApp::enableControls()
{
}

char *CMod_itemconfigApp::getVersion()
{
	return "2.1";
}

void CMod_itemconfigApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_infoDialog)
	{
		m_infoDialog->DoSetButtonSkin();
		m_infoDialog->Invalidate();
	}
}

void CMod_itemconfigApp::configToControls()
{
	if (m_infoDialog)
		m_infoDialog->ConfigToControls();
}

void CMod_itemconfigApp::controlsToConfig()
{
	if (m_infoDialog)
		m_infoDialog->ControlsToConfig();
}

void CMod_itemconfigApp::loadConfigParam(char *paramName, char *paramValue)
{
	
	if (!strcmp(paramName, "lootedItems"))
	{
		if (currentPos == 0)
			CTibiaItem::clearLootItems();
		CTibiaItem::setItemAsLooted(atoi(paramValue));
		currentPos++;
	}
}

char *CMod_itemconfigApp::saveConfigParam(char *paramName)
{
	
	static char buf[1024];
	buf[0] = 0;

	if (!strcmp(paramName, "lootedItems") && currentPos < CTibiaItem::getLootItemCount())
		sprintf(buf, "%d", CTibiaItem::getLootItemIdAtIndex(currentPos++));
	return buf;
}

char *CMod_itemconfigApp::getConfigParamName(int nr)
{
	//Removed since config can be changed even when not opened
	//if (!m_infoDialog) return NULL;//special case where user has not opened the itemConfig

	switch (nr)
	{
	case 0:
		return "lootedItems";
	default:
		return NULL;
	}
}

int CMod_itemconfigApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName, "lootedItems"))
		return 1;
	return 0;
}

void CMod_itemconfigApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName, "lootedItems"))
		currentPos = 0;
}
