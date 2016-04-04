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


// mod_fisher.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_fisher.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

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
// CMod_fisherApp

BEGIN_MESSAGE_MAP(CMod_fisherApp, CWinApp)
//{{AFX_MSG_MAP(CMod_fisherApp)
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

	
	
	CConfigData *config        = (CConfigData *)lpParam;

	while (!toolThreadShouldStop)
	{
		Sleep(CModuleUtil::randomFormula(500, 200));
		if (!reader.isLoggedIn())
			continue;                   // do not proceed if not connected
		int continueFishing   = 1;
		CTibiaCharacter self;
		reader.readSelfCharacter(&self);

		if (config->moveFromHandToCont)
		{
			// left hand
			CTibiaItem *item = reader.readItem(reader.m_memAddressLeftHand);
			int contNr;
			CTibiaContainer *cont;
			if (item->objectId == CTibiaItem::getValueForConst("fish"))
			{
				// fish in left hand - move to first open container
				int openContNr  = 0;
				int openContMax = reader.readOpenContainerCount();
				for (contNr = 0; contNr < reader.m_memMaxContainers && openContNr < openContMax; contNr++)
				{
					cont = reader.readContainer(contNr);
					if (cont->flagOnOff)
					{
						openContNr++;
						if (cont->itemsInside < cont->size)
						{
							CPackSender::moveObjectBetweenContainers(item->objectId, 0x06, 0, 0x40 + contNr, 0, item->quantity ? item->quantity : 1);
							contNr = reader.m_memMaxContainers;
						}
					}
					delete cont;
				}
			}

			// right hand
			delete item;
			item = reader.readItem(reader.m_memAddressRightHand);
			if (item->objectId == CTibiaItem::getValueForConst("fish"))
			{
				// fish in right hand - move to first open container
				int openContNr  = 0;
				int openContMax = reader.readOpenContainerCount();
				for (contNr = 0; contNr < reader.m_memMaxContainers && openContNr < openContMax; contNr++)
				{
					cont = reader.readContainer(contNr);
					if (cont->flagOnOff)
					{
						openContNr++;
						if (cont->itemsInside < cont->size)
						{
							CPackSender::moveObjectBetweenContainers(item->objectId, 0x05, 0, 0x40 + contNr, 0, item->quantity ? item->quantity : 1);
							contNr = reader.m_memMaxContainers;
						}
					}
					delete cont;
				}
			}
			delete item;
		}
		// refresh self to have correct cap
		reader.readSelfCharacter(&self);
		// if cap check enabled,
		if (self.cap < config->fishOnlyWhenCap)
			continueFishing = 0;

		//New only fish when worms available
		if (config->fishOnlyWhenWorms)
		{
			CUIntArray itemsAccepted;
			itemsAccepted.Add(CTibiaItem::getValueForConst("worms"));
			for (int contNr = 0; contNr < reader.m_memMaxContainers; contNr++)
			{
				CTibiaItem *item = CModuleUtil::lookupItem(contNr, &itemsAccepted);
				if (item->objectId)
				{
					delete item;
					break;
				}
				delete item;
				if (contNr == reader.m_memMaxContainers - 1)
					continueFishing = 0;
			}
		}

		int fishingRodCont    = 0xa;
		int fishingRodPos     = 0;
		CTibiaItem *itemArrow = reader.readItem(reader.m_memAddressSlotArrow);
		if (itemArrow->objectId != CTibiaItem::getValueForConst("fishingRod"))
		{
			CUIntArray itemsAccepted;
			int contNr;
			itemsAccepted.Add(CTibiaItem::getValueForConst("fishingRod"));
			for (contNr = 0; contNr < reader.m_memMaxContainers; contNr++)
			{
				CTibiaItem *item = CModuleUtil::lookupItem(contNr, &itemsAccepted);
				if (item->objectId)
				{
					fishingRodCont = 0x40 + contNr;
					fishingRodPos  = item->pos;
					delete item;
					break;
				}
				delete item;
				if (contNr == reader.m_memMaxContainers - 1)
					continueFishing = 0;
			}
		}
		delete itemArrow;
		// now find "random" water field with a fish
		if (continueFishing)
		{
			int offsetX, offsetY = 0;
			int randLoopCount = 20;
			int randomMiss    = rand() % 100 < 20;// 20% chance it looks for a tile without fish
			while (randLoopCount--)
			{
				offsetX = rand() % 15 - 7;
				offsetY = rand() % 11 - 5;
				int tileId = reader.mapGetPointItemId(point(offsetX, offsetY, 0), 0);

				if (randomMiss && tileId >= CTibiaItem::getValueForConst("waterWithFishStart") + 12 && tileId <= CTibiaItem::getValueForConst("waterWithFishEnd") + 12)
					break;
				else if (!randomMiss && tileId >= CTibiaItem::getValueForConst("waterWithFishStart") && tileId <= CTibiaItem::getValueForConst("waterWithFishEnd"))
					break;
			}
			if (randLoopCount > 0)
			{
				int tileId = reader.mapGetPointItemId(point(offsetX, offsetY, 0), 0);
				CPackSender::useWithObjectFromContainerOnFloor(
				        CTibiaItem::getValueForConst("fishingRod"), fishingRodCont, fishingRodPos, tileId, self.x + offsetX, self.y + offsetY, self.z);
			}
		}


	}
	toolThreadShouldStop = 0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMod_fisherApp construction

CMod_fisherApp::CMod_fisherApp()
{
	m_configDialog = NULL;
	m_started      = 0;
	m_configData   = new CConfigData();
}

CMod_fisherApp::~CMod_fisherApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * CMod_fisherApp::getName()
{
	return "Auto fisher";
}

int CMod_fisherApp::isStarted()
{
	return m_started;
}

void CMod_fisherApp::start()
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

void CMod_fisherApp::stop()
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

void CMod_fisherApp::showConfigDialog()
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

void CMod_fisherApp::configToControls()
{
	if (m_configDialog)

		m_configDialog->configToControls(m_configData);
}

void CMod_fisherApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_fisherApp::disableControls()
{
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_fisherApp::enableControls()
{
	if (m_configDialog)
		m_configDialog->enableControls();
}

char *CMod_fisherApp::getVersion()
{
	return "2.4";
}

int CMod_fisherApp::validateConfig(int showAlerts)
{
	if (m_configData->fishOnlyWhenCap < 0)
	{
		if (showAlerts)
			AfxMessageBox("Minimum fishing capacity must be >=0!");
		return 0;
	}

	return 1;
}

void CMod_fisherApp::resetConfig()
{
	if (m_configData)
	{
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void CMod_fisherApp::loadConfigParam(const char *paramName, char *paramValue)
{
	if (!strcmp(paramName, "other/fishOnlyWhenCap"))
		m_configData->fishOnlyWhenCap = atoi(paramValue);
	if (!strcmp(paramName, "move/fromHandToCont"))
		m_configData->moveFromHandToCont = atoi(paramValue);
	if (!strcmp(paramName, "other/fishOnlyWhenWorms"))
		m_configData->fishOnlyWhenWorms = atoi(paramValue);
}

char *CMod_fisherApp::saveConfigParam(const char *paramName)
{
	static char buf[1024];
	buf[0] = 0;

	if (!strcmp(paramName, "other/fishOnlyWhenCap"))
		sprintf(buf, "%d", m_configData->fishOnlyWhenCap);
	if (!strcmp(paramName, "move/fromHandToCont"))
		sprintf(buf, "%d", m_configData->moveFromHandToCont);
	if (!strcmp(paramName, "other/fishOnlyWhenWorms"))
		sprintf(buf, "%d", m_configData->fishOnlyWhenWorms);


	return buf;
}

static const char *configParamNames[] =
{
	"other/fishOnlyWhenCap", // old: fishCap
	"move/fromHandToCont",
	"other/fishOnlyWhenWorms",
	NULL,
};

const char **CMod_fisherApp::getConfigParamNames()
{
	return configParamNames;
}

void CMod_fisherApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog)
	{
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
