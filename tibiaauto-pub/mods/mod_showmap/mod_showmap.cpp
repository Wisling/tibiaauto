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
#include "mod_showmap.h"

#include "ConfigData.h"
#include "TibiaContainer.h"

#include <MemReader.h>
#include <PackSender.h>
#include <ModuleUtil.h>
#include "TibiaMapPoint.h"
#include <TibiaMap.h>

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
// CMod_showmapApp

BEGIN_MESSAGE_MAP(CMod_showmapApp, CWinApp)
//{{AFX_MSG_MAP(CMod_showmapApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions


/////////////////////////////////////////////////////////////////////////////
// CMod_showmapApp construction

CMod_showmapApp::CMod_showmapApp()
{
	m_infoDialog   = NULL;
	currentPointNr = 0;
	m_configData   = new CConfigData();
}

CMod_showmapApp::~CMod_showmapApp()
{
	delete m_infoDialog;
	delete m_configData;
}

char * CMod_showmapApp::getName()
{
	return "Show map";
}

int CMod_showmapApp::isStarted()
{
	return 0;
}

void CMod_showmapApp::start()
{
	superStart();
}

void CMod_showmapApp::stop()
{
}

void CMod_showmapApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!m_infoDialog)
	{
		// ShowMap has no toolthreadproc and infinite loop is set up in OnTimer() which requires a window
		// This function is called when TA starts to create the window to prevent the load log window from creating, and thus owning, the window.
		m_infoDialog = new CToolMapShow();
		m_infoDialog->Create(IDD_TOOL_MAPSHOW);
	}
	else
	{
		m_infoDialog->ShowWindow(SW_SHOW);
	}
}

void CMod_showmapApp::configToControls()
{
	m_infoDialog->m_extendedResearch.SetCheck(m_configData->extendedResearch);
	m_infoDialog->RefreshExtendedResearchMap();
}

void CMod_showmapApp::controlsToConfig()
{
	m_configData->extendedResearch = m_infoDialog->m_extendedResearch.GetCheck();
}

void CMod_showmapApp::disableControls()
{
}

void CMod_showmapApp::enableControls()
{
}

char *CMod_showmapApp::getVersion()
{
	return "1.0";
}

int CMod_showmapApp::validateConfig(int showAlerts)
{
	return 1;
}

void CMod_showmapApp::resetConfig()
{
}

void CMod_showmapApp::loadConfigParam(char *paramName, char *paramValue)
{
	if (!strcmp(paramName, "map/point"))
	{
		CTibiaMap& tibiaMap = CTibiaMap::getTibiaMap();
		if (currentPointNr == 0)
			// first call - reset map
			tibiaMap.clear();
		int x, y, z;
		MapPointType pointType;
		int speed, altX, altY, altZ;
		int locked  = 0;
		int numData = sscanf(paramValue, "%d,%d,%d,%d,%d,%d,%d,%d,%d", &x, &y, &z, &pointType, &speed, &locked, &altX, &altY, &altZ);
		tibiaMap.setPointAsAvailable(x, y, z);
		tibiaMap.setPointType(x, y, z, pointType);

		//is new point for 2.0
		if (numData >= 5)
		{
			tibiaMap.setPointSpeed(x, y, z, speed);
			//has destination data
			if (numData >= 8)
			{
				if (numData == 8)
				{
					//has no "locked" property
					numData = sscanf(paramValue, "%d,%d,%d,%d,%d,%d,%d", &x, &y, &z, &pointType, &speed, &altX, &altY, &altZ);
					locked  = 0;
				}
				tibiaMap.setDestPoint(x, y, z, altX, altY, altZ);
			}
			tibiaMap.setPointLocked(x, y, z, locked);
		}
		currentPointNr++;
	}
	else if (!strcmp(paramName, "extendedResearch"))
	{
		m_configData->extendedResearch = atoi(paramValue);
	}
	configToControls(); // showmap is always on so any changes in the config parameters need to be made to the GUI
}

char *CMod_showmapApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	memset(buf, 0, 1024);
	if (!strcmp(paramName, "map/point"))
	{
		CTibiaMap& tibiaMap = CTibiaMap::getTibiaMap();
getNextCurrentPoint:

		if (currentPointNr < tibiaMap.size())
		{
			struct point p = tibiaMap.getPointByNr(currentPointNr++);
			if (tibiaMap.isPointAvailableNoProh(p.x, p.y, p.z))
			{
				point dest = tibiaMap.getDestPoint(p.x, p.y, p.z);
				if (dest.x == 0)
					sprintf(buf, "%d,%d,%d,%d,%d,%d", p.x, p.y, p.z, tibiaMap.getPointType(p.x, p.y, p.z), tibiaMap.getPointSpeed(p.x, p.y, p.z), tibiaMap.isPointLocked(p.x, p.y, p.z));
				else
					sprintf(buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d", p.x, p.y, p.z, tibiaMap.getPointType(p.x, p.y, p.z), tibiaMap.getPointSpeed(p.x, p.y, p.z), tibiaMap.isPointLocked(p.x, p.y, p.z), dest.x, dest.y, dest.z);
			}
			else
			{
				goto getNextCurrentPoint;
			}
		}
	}
	else if (!strcmp(paramName, "extendedResearch"))
	{
		sprintf(buf, "%d", m_configData->extendedResearch);
	}
	return buf;
}

char *CMod_showmapApp::getConfigParamName(int nr)
{
	if (nr == 0)
		return "map/point";
	if (nr == 1)
		return "extendedResearch";
	return NULL;
}

int CMod_showmapApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName, "map/point"))
		return 1;
	return 0;
}

void CMod_showmapApp::resetMultiParamAccess(char *paramName)
{
	currentPointNr = 0;
}

void CMod_showmapApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_infoDialog)
	{
		m_infoDialog->DoSetButtonSkin();
		m_infoDialog->Invalidate();
	}
}
