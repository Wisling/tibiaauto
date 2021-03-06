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

// mod_fps.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_fps.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include <MemReader.h>
#include <PackSender.h>
#include <TibiaItem.h>
#include <ModuleUtil.h>
#include <MemUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop = 0;
HANDLE toolThreadHandle;
HWND hTibiaWnd;
DWORD addrFps;

extern BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam); //in ConfigDialog.cpp


void SetFPS(double iFps)
{
	CMemReader& reader = CMemReader::getMemReader();
	double *fpsVal;
	int ifpsVal[2];

	fpsVal = (double*)ifpsVal;

	(*fpsVal) = 1000 / iFps;

	//T4: Fps value can be set to almost 0

	CMemUtil::getMemUtil().SetMemIntValue(addrFps + 88, ifpsVal[0], 0);//this address comes from Tibia itself and need not be shifted
	CMemUtil::getMemUtil().SetMemIntValue(addrFps + 88 + 4, ifpsVal[1], 0);//this address comes from Tibia itself and need not be shifted
}

double GetFPS()
{
	CMemReader& reader = CMemReader::getMemReader();
	
	if (!addrFps)
		addrFps = CMemUtil::getMemUtil().GetMemIntValue(CTibiaItem::getValueForConst("addrFps"));

	double *fpsVal;
	int ifpsVal[2];

	fpsVal = (double*)ifpsVal;

	ifpsVal[0] = CMemUtil::getMemUtil().GetMemIntValue(addrFps + 88, 0);//this address comes from Tibia itself and need not be shifted
	ifpsVal[1] = CMemUtil::getMemUtil().GetMemIntValue(addrFps + 88 + 4, 0);//this address comes from Tibia itself and need not be shifted


	return 1000 / (*fpsVal);
}

DWORD WINAPI toolThreadProc(LPVOID lpParam)
{
	CMemReader& reader = CMemReader::getMemReader();
	CConfigData *config = (CConfigData *)lpParam;

	double oldFps;

	oldFps = max(GetFPS(), 20);
	if (!hTibiaWnd)
		EnumWindows(EnumWindowsProc, CMemUtil::getMemUtil().getGlobalProcessId());


	while (!toolThreadShouldStop)
	{
		if (config->minimized && (IsIconic(hTibiaWnd) || !IsWindowVisible(hTibiaWnd)))
			SetFPS(config->minimizedVal);
		else if (config->inactive && GetForegroundWindow() != hTibiaWnd)
			SetFPS(config->inactiveVal);
		else
			SetFPS(config->activeVal);
		double currFps = GetFPS();
		Sleep(1000);
	}

	SetFPS(oldFps);

	toolThreadShouldStop = 0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Cmod_fpsApp construction

Cmod_fpsApp::Cmod_fpsApp()
{
	m_configDialog = NULL;
	m_started      = 0;
	m_configData   = new CConfigData();
}

Cmod_fpsApp::~Cmod_fpsApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * Cmod_fpsApp::getName()
{
	return "FPS";
}

int Cmod_fpsApp::isStarted()
{
	return m_started;
}

void Cmod_fpsApp::start()
{
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

void Cmod_fpsApp::stop()
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

void Cmod_fpsApp::showConfigDialog()
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

void Cmod_fpsApp::configToControls()
{
	if (m_configDialog)

		m_configDialog->configToControls(m_configData);
}

void Cmod_fpsApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void Cmod_fpsApp::disableControls()
{
	if (m_configDialog)
		m_configDialog->disableControls();
}

void Cmod_fpsApp::enableControls()
{
	if (m_configDialog)
		m_configDialog->enableControls();
}

char *Cmod_fpsApp::getVersion()
{
	return "1.0";
}

int Cmod_fpsApp::validateConfig(int showAlerts)
{
	return 1;
}

void Cmod_fpsApp::resetConfig()
{
	if (m_configData)
	{
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void Cmod_fpsApp::loadConfigParam(const char *paramName, char *paramValue)
{
	if (!strcmp(paramName, "activeVal"))
		m_configData->activeVal = atof(paramValue);
	if (!strcmp(paramName, "inactive"))
		m_configData->inactive = atoi(paramValue);
	if (!strcmp(paramName, "inactiveVal"))
		m_configData->inactiveVal = atof(paramValue);
	if (!strcmp(paramName, "minimized"))
		m_configData->minimized = atoi(paramValue);
	if (!strcmp(paramName, "minimizedVal"))
		m_configData->minimizedVal = atof(paramValue);
}

char *Cmod_fpsApp::saveConfigParam(const char *paramName)
{
	static char buf[1024];
	buf[0] = 0;

	if (!strcmp(paramName, "activeVal"))
		sprintf(buf, "%.2f", m_configData->activeVal);
	if (!strcmp(paramName, "inactive"))
		sprintf(buf, "%d", m_configData->inactive);
	if (!strcmp(paramName, "inactiveVal"))
		sprintf(buf, "%.2f", m_configData->inactiveVal);
	if (!strcmp(paramName, "minimized"))
		sprintf(buf, "%d", m_configData->minimized);
	if (!strcmp(paramName, "minimizedVal"))
		sprintf(buf, "%.2f", m_configData->minimizedVal);

	return buf;
}

static const char *configParamNames[] =
{
	"activeVal",
	"inactive",
	"inactiveVal",
	"minimized",
	"minimizedVal",
	NULL,
};

const char **Cmod_fpsApp::getConfigParamNames()
{
	return configParamNames;
}

void Cmod_fpsApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog)
	{
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
