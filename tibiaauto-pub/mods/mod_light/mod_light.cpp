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

// mod_light.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_light.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include <MemReader.h>
#include <PackSender.h>
#include <ModuleUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG


/////////////////////////////////////////////////////////////////////////////
// Cmod_lightApp

BEGIN_MESSAGE_MAP(Cmod_lightApp, CWinApp)
//{{AFX_MSG_MAP(Cmod_lightApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop = 0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc(LPVOID lpParam)
{
	CMemReader& reader = CMemReader::getMemReader();
	CConfigData *config = (CConfigData *)lpParam;

	int initialLightPower = reader.readSelfLightPower();
	int initialLightColor = reader.readSelfLightColor();

/*	char buff[256];
        sprintf(buff,"Power: %d\n Color: %d",initialLightPower,initialLightColor);
        AfxMessageBox(buff,MB_OK);*/

	while (!toolThreadShouldStop)
	{
		Sleep(500);
		if (!reader.isLoggedIn())
			continue;                   // do not proceed if not connected

		reader.writeSelfLightPower(config->lightPower);
		reader.writeSelfLightColor(config->lightColor);
		//Blue C=23
	}
	reader.writeSelfLightPower(initialLightPower);
	reader.writeSelfLightColor(initialLightColor);

	toolThreadShouldStop = 0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Cmod_lightApp construction

Cmod_lightApp::Cmod_lightApp()
{
	m_configDialog = NULL;
	m_started      = 0;
	m_configData   = new CConfigData();
}

Cmod_lightApp::~Cmod_lightApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * Cmod_lightApp::getName()
{
	return "LightHack";
}

int Cmod_lightApp::isStarted()
{
	return m_started;
}

void Cmod_lightApp::start()
{
	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	DWORD threadId;

	toolThreadShouldStop = 0;
	toolThreadHandle     = ::CreateThread(NULL, 0, toolThreadProc, m_configData, 0, &threadId);
	m_started            = 1;
}

void Cmod_lightApp::stop()
{
	toolThreadShouldStop = 1;
	while (toolThreadShouldStop)
	{
		Sleep(50);
	};
	m_started = 0;

	if (m_configDialog)
	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
}

void Cmod_lightApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
		if (m_started)
			disableControls();
		else
			enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}

void Cmod_lightApp::configToControls()
{
	if (m_configDialog)

		m_configDialog->configToControls(m_configData);
}

void Cmod_lightApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void Cmod_lightApp::disableControls()
{
	if (m_configDialog)
		m_configDialog->disableControls();
}

void Cmod_lightApp::enableControls()
{
	if (m_configDialog)
		m_configDialog->enableControls();
}

char *Cmod_lightApp::getVersion()
{
	return "1.1";
}

int Cmod_lightApp::validateConfig(int showAlerts)
{
	return 1;
}

void Cmod_lightApp::resetConfig()
{
	if (m_configData)
	{
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void Cmod_lightApp::loadConfigParam(const char *paramName, char *paramValue)
{
	if (!strcmp(paramName, "lightPower"))
		m_configData->lightPower = atoi(paramValue);
	if (!strcmp(paramName, "lightColor"))
		m_configData->lightColor = atoi(paramValue);
}

char *Cmod_lightApp::saveConfigParam(const char *paramName)
{
	static char buf[1024];
	buf[0] = 0;

	if (!strcmp(paramName, "lightPower"))
		sprintf(buf, "%d", m_configData->lightPower);
	if (!strcmp(paramName, "lightColor"))
		sprintf(buf, "%d", m_configData->lightColor);

	return buf;
}

static const char *configParamNames[] =
{
	"lightPower",
	"lightColor",
	NULL,
};

const char **Cmod_lightApp::getConfigParamNames()
{
	return configParamNames;
}

void Cmod_lightApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog)
	{
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
