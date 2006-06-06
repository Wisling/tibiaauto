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

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"
#include "TibiaMapProxy.h"
#include "TibiaMapPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	m_infoDialog=NULL;
	currentPointNr=0;
}

CMod_showmapApp::~CMod_showmapApp()
{
	
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
		m_infoDialog=new CToolMapShow();
		m_infoDialog->Create(IDD_TOOL_MAPSHOW);
	}
	m_infoDialog->ShowWindow(SW_SHOW);
}


void CMod_showmapApp::configToControls()
{	
}


void CMod_showmapApp::controlsToConfig()
{
	
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

void CMod_showmapApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"map/point"))
	{
		CTibiaMapProxy tibiaMap;
		if (currentPointNr==0)
		{			
			// first call - reset map
			tibiaMap.clear();
		}
		int x,y,z,updown;
		sscanf(paramValue,"%d,%d,%d,%d",&x,&y,&z,&updown);
		tibiaMap.setPointAsAvailable(x,y,z);
		tibiaMap.setPointUpDown(x,y,z,updown);
		currentPointNr++;
	}
}

char *CMod_showmapApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	memset(buf,0,1024);
	if (!strcmp(paramName,"map/point"))
	{
		CTibiaMapProxy tibiaMap;
getNextCurrentPoint:
		
		if (currentPointNr<tibiaMap.size())
		{
			struct point p=tibiaMap.getPointByNr(currentPointNr++);						
			if (tibiaMap.isPointAvailableNoProh(p.x,p.y,p.z))
			{
				sprintf(buf,"%d,%d,%d,%d",p.x,p.y,p.z,tibiaMap.getPointUpDown(p.x,p.y,p.z));
			} else {
				goto getNextCurrentPoint;
			}
		}
	}
	return buf;
}

char *CMod_showmapApp::getConfigParamName(int nr)
{
	if (nr==0) return "map/point";
	return NULL;
}

int CMod_showmapApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName,"map/point")) return 1;
	return 0;
}

void CMod_showmapApp::resetMultiParamAccess(char *paramName)
{
	currentPointNr=0;
}
