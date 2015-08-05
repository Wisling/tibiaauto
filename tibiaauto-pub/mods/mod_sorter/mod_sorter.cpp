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


// mod_sorter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_sorter.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include <MemReader.h>
#include <PackSender.h>
#include <TibiaItem.h>
#include <TileReader.h>
#include <ModuleUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMod_sorterApp

BEGIN_MESSAGE_MAP(CMod_sorterApp, CWinApp)
//{{AFX_MSG_MAP(CMod_sorterApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//int initializeLootBags();
int isStackable(int, int);

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop = 0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc(LPVOID lpParam)
{
	CMemReader& reader = CMemReader::getMemReader();

	
	
	CConfigData *config        = (CConfigData *)lpParam;
	char buf[256];
	while (!toolThreadShouldStop)
	{
		Sleep(200);
		int sorted = 0;
		int j      = 0;
		for (int i = 0; i < 8; i++)
		{
			if (!strlen(config->sortBags[i].slotNr[j].itemName))
				break;
			for (j = 0; j < 32; j++)
			{
				//sprintf(buf, "Sort slotNr (i loop): %d\nSort item (j loop): %d", i + 1, j + 1);
				//AfxMessageBox(buf);
				if (!strlen(config->sortBags[i].slotNr[j].itemName))
					break;
				int sortItem = CTibiaItem::getItemId(config->sortBags[i].slotNr[j].itemName);
				//sprintf(buf, "ObjectID: %d",  sortItem);
				//AfxMessageBox(buf);
				int openContNr  = 0;
				int openContMax = reader.readOpenContainerCount();
				for (int contNr = 0; contNr < reader.m_memMaxContainers && openContNr < openContMax; contNr++)
				{
					if (contNr == i)
						contNr++;
					CTibiaContainer *sortCont = reader.readContainer(contNr);
					CTibiaContainer *destCont = reader.readContainer(i);
					if (sortCont->flagOnOff)
					{
						openContNr++;
						int count = sortCont->itemsInside;
						//sprintf(buf, "Items inside bag %d: %d", contNr, count);
						//AfxMessageBox(buf);
						for (int itemNr = count - 1; itemNr >= 0; itemNr--)
						{
							CTibiaItem *item = (CTibiaItem *)sortCont->items.GetAt(itemNr);
							if (item->objectId == sortItem)
							{
								//sprintf(buf, "Object ID to sort: %d\nObject ID of currently read item: %d", sortItem, item->objectId);
								//AfxMessageBox(buf);
								if (destCont->flagOnOff && destCont->itemsInside < destCont->size || isStackable(sortItem, i))
								{
									//sprintf(buf, "Bag #%d is %sopen and %sfull.", i + 1, destCont->flagOnOff?"":"not ", destCont->itemsInside<destCont->size?"not ":"");
									//AfxMessageBox(buf);
									int sourcePos    = itemNr;
									int targetPos    = destCont->size - 1;
									int moved        = item->quantity ? item->quantity : 1;
									CTibiaTile *tile = CTileReader::getTileReader().getTile(item->objectId);
									CTibiaItem *stackedItem;
									if (tile->stackable)
									{
										for (int stackedItemPos = 0; stackedItemPos < destCont->itemsInside; stackedItemPos++)
										{
											stackedItem = (CTibiaItem *)destCont->items.GetAt(stackedItemPos);
											if (stackedItem->objectId == item->objectId && stackedItem->quantity < 100)
											{
												targetPos = stackedItemPos;
												if ((stackedItem->quantity + item->quantity) > 100)
													itemNr++;
												moved = min(item->quantity, 100 - stackedItem->quantity);
												break;
											}
										}
									}
									//sprintf(buf, "Item: %d  Quantity: %d\nSource Container: %d  Slot: %d\nDestination Container: %d  Slot: %d\n Original item quantity: %d",  sortItem, moved, contNr, sourcePos, i, targetPos, item->quantity);
									//AfxMessageBox(buf);
									CPackSender::moveObjectBetweenContainers(sortItem, 0x40 + contNr, sourcePos, 0x40 + i, targetPos, moved);
									if (item->quantity == moved)
									{
										CModuleUtil::waitForItemsInsideChange(contNr, sortCont->itemsInside);
										if (targetPos != destCont->size - 1 || destCont->itemsInside == destCont->size)
											CModuleUtil::waitForItemsInsideChange(i, targetPos, stackedItem->quantity, destCont->itemsInside);
										else
											CModuleUtil::waitForItemsInsideChange(i, destCont->itemsInside);
										Sleep(CModuleUtil::randomFormula(300, 100));
									}
									else if (item->quantity == 0)
									{
										CModuleUtil::waitForItemsInsideChange(contNr, sortCont->itemsInside);
										CModuleUtil::waitForItemsInsideChange(i, destCont->itemsInside);
									}
									else
									{
										CModuleUtil::waitForItemsInsideChange(contNr, sourcePos, item->quantity, sortCont->itemsInside);
										CModuleUtil::waitForItemsInsideChange(i, targetPos, stackedItem->quantity, destCont->itemsInside);
									}
								}
								else
								{
									int found = 0;
									int findItem;
									CTibiaContainer *fullCont = reader.readContainer(i);
									for (int l = 0; l < fullCont->itemsInside; l++)
									{
										CTibiaItem *item = (CTibiaItem *)fullCont->items.GetAt(l);
										for (int m = 0; m < 8; m++)
										{
											for (int n = 0; n < 32; n++)
											{
												findItem = CTibiaItem::getItemId(config->sortBags[m].slotNr[n].itemName);
												if (item->objectId == findItem && findItem != sortItem && m != i)
													found = 1;
												//break;
											}
										}
									}
									delete fullCont;

									//sprintf(buf, "Full bag %sfound!", found?"":"not ");
									//AfxMessageBox(buf);
									if (!found)
									{
										sprintf(buf, "No more bags configured for %s sorting!!!\nPlease Close bag %d!\nOpen a new bag and press OK", config->sortBags[i].slotNr[j].itemName, i + 1);
										AfxMessageBox(buf);
									}
									else
									{
										contNr = 16;
										itemNr = 0;
									}
								}
								delete sortCont;
								delete destCont;
								sortCont = reader.readContainer(contNr);
								destCont = reader.readContainer(i);
								int slotsChecked = count - itemNr;
								itemNr = (sortCont->itemsInside < count - slotsChecked) ? sortCont->itemsInside : itemNr;
							}
						}
					}
					delete sortCont;
					delete destCont;
				}
			}
			j = 0;
		}
	}
	toolThreadShouldStop = 0;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMod_sorterApp construction

CMod_sorterApp::CMod_sorterApp()
{
	m_configDialog = NULL;
	m_started      = 0;
	m_configData   = new CConfigData();
}

CMod_sorterApp::~CMod_sorterApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * CMod_sorterApp::getName()
{
	return "Auto Sorter";
}

int CMod_sorterApp::isStarted()
{
	return m_started;
}

void CMod_sorterApp::start()
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

void CMod_sorterApp::stop()
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

void CMod_sorterApp::showConfigDialog()
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

void CMod_sorterApp::configToControls()
{
	if (m_configDialog)
		m_configDialog->configToControls(m_configData);
}

void CMod_sorterApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}

void CMod_sorterApp::disableControls()
{
	if (m_configDialog)
		m_configDialog->disableControls();
}

void CMod_sorterApp::enableControls()
{
	if (m_configDialog)
		m_configDialog->enableControls();
}

char *CMod_sorterApp::getVersion()
{
	return "1.0";
}

int CMod_sorterApp::validateConfig(int showAlerts)
{
	if (initializeLootBags() == 0)
	{
		AfxMessageBox("Not all bags are open!!\nPlease open all configured bags and restart!");
		return 0;
	}
	if (initializeLootBags() == -1)
	{
		AfxMessageBox("No sortable items configured!!\nPlease configure bags and restart!");
		return 0;
	}
	return 1;
}

void CMod_sorterApp::resetConfig()
{
	if (m_configData)
	{
		delete m_configData;
		m_configData = NULL;
	}
	m_configData = new CConfigData();
}

void CMod_sorterApp::loadConfigParam(char *paramName, char *paramValue)
{
	char tempHold[64];
	sscanf(paramValue, "%d,%d,%[^#]", &m_currentBagEntryNr, &m_currentSlotEntryNr, &tempHold);
	if (!strcmp(paramName, "bag/entry"))
	{
		if (!m_currentBagEntryNr && !m_currentSlotEntryNr)
		{
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 32; j++)
				{
					m_configData->sortBags[i].slotNr[j].itemName[0] = '\0';
				}
			}
		}
		strcpy(m_configData->sortBags[m_currentBagEntryNr].slotNr[m_currentSlotEntryNr].itemName, tempHold);
	}
}

char *CMod_sorterApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0] = 0;
	if (!strcmp(paramName, "bag/entry") && strlen(m_configData->sortBags[m_currentBagEntryNr].slotNr[m_currentSlotEntryNr].itemName) && m_currentBagEntryNr < 8 && m_currentSlotEntryNr < 32)
	{
		sprintf(buf, "%d,%d,%s", m_currentBagEntryNr, m_currentSlotEntryNr, m_configData->sortBags[m_currentBagEntryNr].slotNr[m_currentSlotEntryNr].itemName);
		m_currentSlotEntryNr++;
		if (m_currentSlotEntryNr == 32)
		{
			m_currentSlotEntryNr = 0;
			m_currentBagEntryNr++;
		}
	}
	else if (!strcmp(paramName, "bag/entry") && m_currentBagEntryNr < 7 && m_currentSlotEntryNr < 32)
	{
		m_currentSlotEntryNr = 0;
		m_currentBagEntryNr++;
		if (!strcmp(paramName, "bag/entry") && strlen(m_configData->sortBags[m_currentBagEntryNr].slotNr[m_currentSlotEntryNr].itemName))
		{
			sprintf(buf, "%d,%d,%s", m_currentBagEntryNr, m_currentSlotEntryNr, m_configData->sortBags[m_currentBagEntryNr].slotNr[m_currentSlotEntryNr].itemName);
			m_currentSlotEntryNr++;
			if (m_currentSlotEntryNr == 32)
			{
				m_currentSlotEntryNr = 0;
				m_currentBagEntryNr++;
			}
		}
	}
	return buf;
}

char *CMod_sorterApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0:
		return "bag/entry";
	default:
		return NULL;
	}
}

int CMod_sorterApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName, "bag/entry"))
		return 1;
	return 0;
}

void CMod_sorterApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName, "bag/entry"))
	{
		m_currentBagEntryNr  = 0;
		m_currentSlotEntryNr = 0;
	}
}

int CMod_sorterApp::initializeLootBags()
{
	for (int i = 0; i < 8; i++)
	{
		if (m_configData->sortBags[i].slotNr[0].itemName[0] != '\0')
		{
			//AfxMessageBox("At least one sortable item found");
			if (CModuleUtil::waitForOpenContainer(i, true))
				continue;
			else
				return 0;
		}
		else if (i == 0)
		{
			return -1;
		}
		else
		{
			break;
		}
	}
	return 1;
}

int isStackable(int sortItem, int contNr)
{
	CMemReader& reader = CMemReader::getMemReader();
	CTibiaContainer *cont = reader.readContainer(contNr);
	CTibiaTile *tile      = CTileReader::getTileReader().getTile(sortItem);
	if (tile->stackable)
	{
		for (int stackedItemPos = 0; stackedItemPos < cont->itemsInside; stackedItemPos++)
		{
			CTibiaItem *stackedItem = (CTibiaItem *)cont->items.GetAt(stackedItemPos);
			if (stackedItem->objectId == sortItem && stackedItem->quantity < 100)
			{
				delete cont;
				return 1;
			}
		}
	}
	delete cont;
	return 0;
}

void CMod_sorterApp::getNewSkin(CSkin newSkin)
{
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog)
	{
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
