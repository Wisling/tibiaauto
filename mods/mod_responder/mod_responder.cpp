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
#include "mod_responder.h"

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

/////////////////////////////////////////////////////////////////////////////
// CMod_responderApp

BEGIN_MESSAGE_MAP(CMod_responderApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_responderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions

extern int toolAutoResponderRunning;

/////////////////////////////////////////////////////////////////////////////
// CMod_responderApp construction

CMod_responderApp::CMod_responderApp()
{	
	m_infoDialog=NULL;
	currentPointNr=0;
}

CMod_responderApp::~CMod_responderApp()
{
	
	delete m_configData;	
}

char * CMod_responderApp::getName()
{
	return "Auto responder";
}


int CMod_responderApp::isStarted()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (!m_infoDialog)
	{ 
		m_infoDialog=new CToolAutoRespond();
		m_infoDialog->Create(IDD_TOOL_AUTORESPOND);
	}
	return toolAutoResponderRunning;
}

void CMod_responderApp::showConfigDialog()
{
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (!m_infoDialog)
	{ 
		m_infoDialog=new CToolAutoRespond();
		m_infoDialog->Create(IDD_TOOL_AUTORESPOND);
	}
	m_infoDialog->ShowWindow(SW_SHOW);
}


void CMod_responderApp::enableControls()
{
	
}


char *CMod_responderApp::getVersion()
{
	return "1.1";
}

void CMod_responderApp::start()
{
	superStart();
	if (!toolAutoResponderRunning)
	{
		if (!m_infoDialog)
		{
			if (!m_infoDialog)
			{ 
				AFX_MANAGE_STATE(AfxGetStaticModuleState());			
				m_infoDialog=new CToolAutoRespond();
				m_infoDialog->Create(IDD_TOOL_AUTORESPOND);
			}
		}
		m_infoDialog->start();
	}
}

void CMod_responderApp::stop()
{
	if (toolAutoResponderRunning&&m_infoDialog)
	{
		m_infoDialog->stop();
	}
}
