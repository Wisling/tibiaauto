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
#include "mod_monstershow.h"

#include "ConfigData.h"
#include "TibiaContainer.h"

#include <MemReader.h>
#include <PackSender.h>
#include <ModuleUtil.h>
#include "TibiaMapPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Tool functions

/////////////////////////////////////////////////////////////////////////////
// CMod_monstershowApp construction

CMod_monstershowApp::CMod_monstershowApp()
{
	m_infoDialog = NULL;
	m_configData = new CConfigData();
}

CMod_monstershowApp::~CMod_monstershowApp()
{
	if (m_infoDialog)
	{
		m_infoDialog->DestroyWindow();
		delete m_infoDialog;
	}
	delete m_configData;
}

char * CMod_monstershowApp::getName()
{
	return "Monster show";
}

void CMod_monstershowApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!m_infoDialog)
	{
		m_infoDialog = new CToolMonsterShow();
		m_infoDialog->Create(IDD_TOOL_MONSTERSHOW);
	}
	m_infoDialog->ShowWindow(SW_SHOW);
}

char *CMod_monstershowApp::getVersion()
{
	return "1.1";
}

void CMod_monstershowApp::resetConfig()
{
}

static const char *configParamNames[] =
{
	NULL,
};

const char **CMod_monstershowApp::getConfigParamNames()
{
	return configParamNames;
}

void CMod_monstershowApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_infoDialog)
	{
		m_infoDialog->DoSetButtonSkin();
		m_infoDialog->Invalidate();
	}
}
