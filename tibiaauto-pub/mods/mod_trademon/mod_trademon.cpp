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


// mod_trademon.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_trademon.h"

#include "ConfigData.h"
#include "TibiaContainer.h"

#include <MemReader.h>
#include <PackSender.h>
#include <TibiaItem.h>
#include <ModuleUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// Tool functions

int RandomTimeTrademon(int interval)
{
	return interval + rand() % max((int)(.1 * interval), 5);
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop = 0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc(LPVOID lpParam)
{
	CMemReader& reader = CMemReader::getMemReader();

	
	CConfigData *config = (CConfigData *)lpParam;

	int offeredItems = 0;

	time_t channelTime = 0;
	time_t yellTime    = 0;
	time_t sayTime     = 0;
	srand((unsigned int)time(NULL));

	while (!toolThreadShouldStop)
	{
		Sleep(1000);
		if (!reader.isLoggedIn())
			continue;                   // do not proceed if not connected

		if (config->tradeTalk)
		{
			if (config->channel)
			{
				static int randTimeTrade = RandomTimeTrademon(config->channelInterval);
				if (channelTime + randTimeTrade < time(NULL))
				{
					randTimeTrade = RandomTimeTrademon(config->channelInterval);
					channelTime   = time(NULL);
					CPackSender::sayOnChan(config->message, 7, 5);
				}
			}
			if (config->yell)
			{
				static int randTimeYell = RandomTimeTrademon(config->yellInterval);
				if (yellTime + randTimeYell < time(NULL))
				{
					randTimeYell = RandomTimeTrademon(config->yellInterval);
					yellTime     = time(NULL);
					CPackSender::sayYell(config->message);
				}
			}

			if (config->say)
			{
				static int randTimeSay = RandomTimeTrademon(config->sayInterval);
				if (sayTime + randTimeSay < time(NULL))
				{
					randTimeSay = RandomTimeTrademon(config->sayInterval);
					sayTime     = time(NULL);
					CPackSender::say(config->message);
				}
			}
		}
		if (config->tradeMon)
		{
			if (offeredItems != reader.getTradeCountPartner())
			{
				offeredItems = reader.getTradeCountPartner();
				// if offered items count=0 then continue (means - no more trade open)
				if (!offeredItems)
					continue;

				char *strbuf = (char *)malloc(65536);
				memset(strbuf, 0, 65536);
				int *itemsCount = (int *)malloc(sizeof(int) * 65536);
				int i;

				memset(itemsCount, 0, 65536 * sizeof(int));


				for (i = 0; i < offeredItems; i++)
				{
					CTibiaItem *item = reader.getTradeItemPartner(i);
					itemsCount[item->objectId] += item->quantity ? item->quantity : 1;
					delete item;
				}
				sprintf(strbuf, "[trade] ");
				for (i = 0; i < 65536; i++)
				{
					if (itemsCount[i])
					{
						char buf[500];
						sprintf(buf, "%s [%d]", CTibiaItem::getItemName(i), itemsCount[i]);
						if (i)
							strcat(strbuf, ", ");
						strcat(strbuf, buf);
						strbuf[1000] = 0;
					}
				}
				CPackSender::sendTAMessage(strbuf);
				free(strbuf);
				free(itemsCount);
			}
		}
	}

	toolThreadShouldStop = 0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMod_trademonApp construction

CMod_trademonApp::CMod_trademonApp()
{
	m_configDialog = NULL;
	m_started      = 0;
	m_configData   = new CConfigData();
}

CMod_trademonApp::~CMod_trademonApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * CMod_trademonApp::getName()
{
	return "Trade tool";
}

int CMod_trademonApp::isStarted()
{
	return m_started;
}

void CMod_trademonApp::start()
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

void CMod_trademonApp::stop()
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

void CMod_trademonApp::showConfigDialog()
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

void CMod_trademonApp::configToControls()
{
	if (m_configDialog)

		m_configDialog->configToControls(m_configData);
}

void CMod_trademonApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_trademonApp::disableControls()
{
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_trademonApp::enableControls()
{
	if (m_configDialog)
		m_configDialog->enableControls();
}

char *CMod_trademonApp::getVersion()
{
	return "2.0";
}

int CMod_trademonApp::validateConfig(int showAlerts)
{
	if (m_configData)
	{
		if (m_configData->tradeTalk)
		{
			if (strlen(m_configData->message) == 0)
			{
				if (showAlerts)
					AfxMessageBox("You have to specify a message text");
				return 0;
			}
			if (!(m_configData->channel || m_configData->yell || m_configData->say))
			{
				if (showAlerts)
					AfxMessageBox("You have to select at least one way of showing your message");
				return 0;
			}
			if (m_configData->channel && m_configData->channelInterval < 1)
			{
				if (showAlerts)
					AfxMessageBox("Channel interval must be >= 1!");
				return 0;
			}
			if (m_configData->yell && m_configData->yellInterval < 1)
			{
				if (showAlerts)
					AfxMessageBox("Yell interaval must be >= 1!");
				return 0;
			}
			if (m_configData->say && m_configData->sayInterval < 1)
			{
				if (showAlerts)
					AfxMessageBox("Say interval must be >= 1!");
				return 0;
			}
		}
	}
	return 1;
}

void CMod_trademonApp::resetConfig()
{
	if (m_configData)
	{
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void CMod_trademonApp::loadConfigParam(const char *paramName, char *paramValue)
{
	if (!strcmp(paramName, "tradeTalker"))
		m_configData->tradeTalk = atoi(paramValue);
	if (!strcmp(paramName, "message"))
	{
		lstrcpyn(m_configData->message, paramValue, 255);
		m_configData->message[255] = 0;
	}
	if (!strcmp(paramName, "channel"))
		m_configData->channel = atoi(paramValue);
	if (!strcmp(paramName, "channelInterval"))
		m_configData->channelInterval = atoi(paramValue);
	if (!strcmp(paramName, "yell"))
		m_configData->yell = atoi(paramValue);
	if (!strcmp(paramName, "yellInterval"))
		m_configData->yellInterval = atoi(paramValue);
	if (!strcmp(paramName, "say"))
		m_configData->say = atoi(paramValue);
	if (!strcmp(paramName, "sayInterval"))
		m_configData->sayInterval = atoi(paramValue);
	if (!strcmp(paramName, "tradeMon"))
		m_configData->tradeMon = atoi(paramValue);
}

char *CMod_trademonApp::saveConfigParam(const char *paramName)
{
	static char buf[1024];
	buf[0] = 0;

	if (!strcmp(paramName, "tradeTalker"))
		sprintf(buf, "%d", m_configData->tradeTalk);
	if (!strcmp(paramName, "message"))
		sprintf(buf, "%s", m_configData->message);
	if (!strcmp(paramName, "channel"))
		sprintf(buf, "%d", m_configData->channel);
	if (!strcmp(paramName, "channelInterval"))
		sprintf(buf, "%d", m_configData->channelInterval);
	if (!strcmp(paramName, "yell"))
		sprintf(buf, "%d", m_configData->yell);
	if (!strcmp(paramName, "yellInterval"))
		sprintf(buf, "%d", m_configData->yellInterval);
	if (!strcmp(paramName, "say"))
		sprintf(buf, "%d", m_configData->say);
	if (!strcmp(paramName, "sayInterval"))
		sprintf(buf, "%d", m_configData->sayInterval);
	if (!strcmp(paramName, "tradeMon"))
		sprintf(buf, "%d", m_configData->tradeMon);

	return buf;
}

static const char *configParamNames[] =
{
	"tradeTalker",
	"message",
	"channel",
	"channelInterval",
	"yell",
	"yellInterval",
	"say",
	"sayInterval",
	"tradeMon",
	NULL,
};

const char **CMod_trademonApp::getConfigParamNames()
{
	return configParamNames;
}

void CMod_trademonApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog)
	{
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
