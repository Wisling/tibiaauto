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


// mod_looter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_looter.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "ModuleUtil.h"
#include "TibiaItemProxy.h"
#include "SendStats.h"

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
// CMod_looterApp

BEGIN_MESSAGE_MAP(CMod_looterApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_looterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Tool functions

int containerNotFull(int containerNr)
{
	int ret;
	CMemReaderProxy reader;
	CTibiaContainer *cont = reader.readContainer(containerNr);
	ret=(cont->size>cont->itemsInside&&cont->flagOnOff);
	delete cont;
	return ret;
}

//returns the smallest radius of a square centred at x1,y1 and covers x2,y2
//same equation as finding distance for rods or wands
int maxDist(int x1,int y1,int x2,int y2){ return max(abs(x2-x1),abs(y2-y1));}

//returns the smallest number of steps needed to stand beside x2,y2 starting from x1,y1
//same distance ideally travelled by "Auto Follow"
int taxiDist(int x1,int	y1,int x2,int y2){ return abs(x2-x1)+abs(y2-y1)-2+(x1==x2)+(y1==y2);}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
int lastLootContNr[3]={-1,-1,-1};
HANDLE toolThreadHandle;
CUIntArray lootCreatures;

DWORD WINAPI toolThreadProc(LPVOID lpParam)
{

	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	CPackSenderProxy sender;
	CConfigData *config = (CConfigData *)lpParam;
	int lastAttackedMonster = 0;
	Corpse *corpseQueue;
	corpseQueue = new Corpse();

	int lootCorpseTime = 1;

	while (!toolThreadShouldStop)
	{
		Sleep(50);
		if (!reader.isLoggedIn()) continue; // do not proceed if not connected

		int flags = reader.getSelfEventFlags();
		if (!config->m_lootInDepot && flags & 0x4000) continue;

		CTibiaCharacter *self = reader.readSelfCharacter();
		CTibiaCharacter *attackedCh = reader.getCharacterByTibiaId(lastAttackedMonster);

		//Check if container is closed, then no need to close it
		if(config->m_autoOpen){
			for (int i=0;i<3;i++){
				if (lastLootContNr[i]!=-1){
					CTibiaContainer *cont=reader.readContainer(lastLootContNr[i]);
					if(!cont->flagOnOff) lastLootContNr[i]=-1;
					delete cont;
				}
			}
		}
		/*** killed monster opening part ***/
		if (config->m_autoOpen&&lastAttackedMonster&&(!attackedCh || attackedCh->hpPercLeft==0))
		{
			//::MessageBox(NULL,"x 1","x",0);

			if (attackedCh)
			{
												
				if (!attackedCh->hpPercLeft&&abs(self->x-attackedCh->x)<=1&&abs(self->y-attackedCh->y)<=1&&self->z==attackedCh->z)
				{
					//Sleep(1000);
					// the creature is dead and we can try to open its corpse
					int corpseId = reader.itemOnTopCode(attackedCh->x-self->x,attackedCh->y-self->y);
					
				
					// now close containers 7-9 which we use for auto-opening purposes
					int i;
					for (i=0;i<3;i++){
						if (lastLootContNr[i]!=-1){
							sender.closeContainer(lastLootContNr[i]);
							CModuleUtil::waitForOpenContainer(lastLootContNr[i],0);
							lastLootContNr[i]=-1;
						}
					}

					lastLootContNr[0]=reader.findNextClosedContainer();
					lastLootContNr[1]=reader.findNextClosedContainer(lastLootContNr[0]);
					lastLootContNr[2]=reader.findNextClosedContainer(lastLootContNr[1]);

					CTibiaContainer *cont[3]={NULL,NULL,NULL};
					
					// now open corpse and all its containers

					delete self;
					self = reader.readSelfCharacter();
					delete attackedCh;
					attackedCh=NULL;
					attackedCh = reader.getCharacterByTibiaId(lastAttackedMonster);
					if (attackedCh)
					{
						CModuleUtil::waitForCreatureDisappear(attackedCh->nr);
						int corpseId = reader.itemOnTopCode(attackedCh->x-self->x,attackedCh->y-self->y);
						CTibiaTile *tile=reader.getTibiaTile(corpseId);
						if (corpseId&&tile->isContainer)
						{
							sender.openContainerFromFloor(corpseId,attackedCh->x,attackedCh->y,attackedCh->z,lastLootContNr[0]);
							CModuleUtil::waitForOpenContainer(lastLootContNr[0],1);
							cont[0] = reader.readContainer(lastLootContNr[0]);
							if (cont[0]->flagOnOff)
							{
								
								int currentExtraContainerNr = 1;
								int itemNr;
								for (itemNr=0;itemNr<cont[0]->itemsInside&&currentExtraContainerNr<3;itemNr++)
								{
									CTibiaItem *item = (CTibiaItem *)cont[0]->items.GetAt(itemNr);
									if (item->objectId==itemProxy.getValueForConst("bagbrown"))
									{
										sender.openContainerFromContainer(item->objectId,0x40+lastLootContNr[0],itemNr,lastLootContNr[currentExtraContainerNr]);
										CModuleUtil::waitForOpenContainer(lastLootContNr[currentExtraContainerNr],1);
										cont[currentExtraContainerNr]=reader.readContainer(lastLootContNr[currentExtraContainerNr]);
										currentExtraContainerNr++;
										
									}
								} // for itemNr (open extra containers)
								
								if (corpseId&&cont[0]->flagOnOff)
								{
									FILE *lootStatsFile = NULL;
									// time,rand,creature,name,pos,objectId,count,bagopen,checksum
									int killNr=rand();
									char installPath[1024];
									CModuleUtil::getInstallPath(installPath);
									char pathBuf[2048];
									sprintf(pathBuf,"%s\\tibiaauto-stats-loot.txt",installPath);
									lootStatsFile=fopen(pathBuf,"a+");
									if (lootStatsFile)
									{
										int i,len;
										char statChName[128];
										for (i=0,strcpy(statChName,attackedCh->name),len=strlen(statChName);i<len;i++)
										{
											if (statChName[i]=='[')
												statChName[i]='\0';
										}
										int tm=time(NULL);
										int killNr=rand();
										int checksum;
										
										checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(statChName),-1,corpseId,0,2,attackedCh->x,attackedCh->y,attackedCh->z);
										if (checksum<0) checksum*=-1;
										fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d\n",tm,killNr,statChName,-1,corpseId,0,2,attackedCh->x,attackedCh->y,attackedCh->z,checksum);
										
										CTibiaContainer *lootCont = cont[0];
										int itemNr;
										for (itemNr=0;itemNr<lootCont->itemsInside;itemNr++)
										{
											CTibiaItem *lootItem = (CTibiaItem *)lootCont->items.GetAt(itemNr);
											
											checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(statChName),itemNr,lootItem->objectId,(lootItem->quantity?lootItem->quantity:1),0,attackedCh->x,attackedCh->y,attackedCh->z);
											if (checksum<0) checksum*=-1;
											fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d\n",tm,killNr,statChName,itemNr,lootItem->objectId,lootItem->quantity?lootItem->quantity:1,0,attackedCh->x,attackedCh->y,attackedCh->z,checksum);
										}
										
										
										if (cont[1])
										{
											lootCont = cont[1];
											int itemNr;
											for (itemNr=0;itemNr<lootCont->itemsInside;itemNr++)
											{
												CTibiaItem *lootItem = (CTibiaItem *)lootCont->items.GetAt(itemNr);
												
												checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(statChName),100+itemNr,lootItem->objectId,(lootItem->quantity?lootItem->quantity:1),1,attackedCh->x,attackedCh->y,attackedCh->z);
												if (checksum<0) checksum*=-1;
												fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d\n",tm,killNr,statChName,100+itemNr,lootItem->objectId,lootItem->quantity?lootItem->quantity:1,1,attackedCh->x,attackedCh->y,attackedCh->z,checksum);
											}
										}
										if (cont[2])
										{
											lootCont = cont[2];
											int itemNr;
											for (itemNr=0;itemNr<lootCont->itemsInside;itemNr++)
											{
												CTibiaItem *lootItem = (CTibiaItem *)lootCont->items.GetAt(itemNr);
												
												checksum = CModuleUtil::calcLootChecksum(tm,killNr,strlen(attackedCh->name),100+itemNr,lootItem->objectId,(lootItem->quantity?lootItem->quantity:1),2,attackedCh->x,attackedCh->y,attackedCh->z);
												if (checksum<0) checksum*=-1;
												fprintf(lootStatsFile,"%d,%d,'%s',%d,%d,%d,%d,%d\n",tm,killNr,attackedCh->name,100+itemNr,lootItem->objectId,lootItem->quantity?lootItem->quantity:1,2,attackedCh->x,attackedCh->y,attackedCh->z,checksum);
											}
										}
										
										fclose(lootStatsFile);
									}
								}
								
								
								
							} // if corpseId && cont9 open
						}
					} // if (attackedCh)
					
					
					for(i=0;i<3;i++) {
						if (cont[i]){
							delete cont[i];
							cont[i]=NULL;
						}
					}
				}
			}
			lastAttackedMonster=0;
		}
		if (attackedCh && attackedCh->hpPercLeft!=0 && lastAttackedMonster || !lastAttackedMonster) lastAttackedMonster = reader.getAttackedCreature();
		delete self;
		if (attackedCh) delete attackedCh;//attackedCh is null if not present
		/*** moving part ***/

		/**
		 * mode 0 - ignore
		 * mode 1 - carrying
		 * mode 2 - external
		 */

		int carriedContainers=0;

		// find carrying container
		carriedContainers|=(config->m_mode1==1)*1;
		carriedContainers|=(config->m_mode2==1)*2;
		carriedContainers|=(config->m_mode3==1)*4;
		carriedContainers|=(config->m_mode4==1)*8;
		carriedContainers|=(config->m_mode5==1)*16;
		carriedContainers|=(config->m_mode6==1)*32;
		carriedContainers|=(config->m_mode7==1)*64;
		carriedContainers|=(config->m_mode8==1)*128;
		carriedContainers|=(config->m_mode9==1)*256;
		carriedContainers|=(config->m_mode10==1)*512;


		if (carriedContainers!=0)
		{
			// find items in the other containers
			CUIntArray acceptedItems;
			
			if (config->m_lootFood)
			{
				acceptedItems.Append(*itemProxy.getFoodIdArrayPtr());
			}
			if (config->m_lootGp)
			{
				acceptedItems.Add(itemProxy.getValueForConst("GP"));
			}
			if (config->m_lootWorms)
			{
				acceptedItems.Add(itemProxy.getValueForConst("worms"));
			}
			if (config->m_lootCustom)
			{
				acceptedItems.Append(*itemProxy.getLootItemIdArrayPtr());
			}
			
			
			int extraSleep=0;
			if (config->m_mode1==2&&CModuleUtil::lootItemsToSpecifiedContainers(0,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode2==2&&CModuleUtil::lootItemsToSpecifiedContainers(1,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode3==2&&CModuleUtil::lootItemsToSpecifiedContainers(2,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode4==2&&CModuleUtil::lootItemsToSpecifiedContainers(3,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode5==2&&CModuleUtil::lootItemsToSpecifiedContainers(4,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode6==2&&CModuleUtil::lootItemsToSpecifiedContainers(5,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode7==2&&CModuleUtil::lootItemsToSpecifiedContainers(6,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode8==2&&CModuleUtil::lootItemsToSpecifiedContainers(7,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode9==2&&CModuleUtil::lootItemsToSpecifiedContainers(8,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (config->m_mode10==2&&CModuleUtil::lootItemsToSpecifiedContainers(9,&acceptedItems,carriedContainers)!=-1) extraSleep=1;
			if (extraSleep) Sleep(CModuleUtil::randomFormula(300,100));
		}
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_looterApp construction

CMod_looterApp::CMod_looterApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();
}

CMod_looterApp::~CMod_looterApp()
{
	if (m_configDialog)
	{
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
	delete m_configData;
}

char * CMod_looterApp::getName()
{
	return "Auto looter";
}


int CMod_looterApp::isStarted()
{
	return m_started;
}


void CMod_looterApp::start()
{
	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

		
	char installPath[1024];
	CModuleUtil::getInstallPath(installPath);
	char pathBuf[2048];
	sprintf(pathBuf,"%s\\tibiaauto-stats-loot.txt",installPath);
	FILE *f=fopen(pathBuf,"r");
		
	if (f)
	
	{
		fseek(f,0,SEEK_END);

		int flen=ftell(f);
		fclose(f);
		if (flen>1024*800 && m_configDialog)
		{
			CSendStats info;
			info.DoModal();
		}
	}
	

	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);
	m_started=1;
}

void CMod_looterApp::stop()
{
	toolThreadShouldStop=1;
	while (toolThreadShouldStop) {
		Sleep(50);
	};
	m_started=0;
	
	if (m_configDialog)
	{
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
}

void CMod_looterApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
		if (m_started) disableControls();
		else enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_looterApp::configToControls()
{
	if (m_configDialog)
	{
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_looterApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_looterApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_looterApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_looterApp::getVersion()
{
	return "1.5";
}


int CMod_looterApp::validateConfig(int showAlerts)
{
	return 1;
}

void CMod_looterApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_looterApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"mode/cont1")) m_configData->m_mode1=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont2")) m_configData->m_mode2=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont3")) m_configData->m_mode3=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont4")) m_configData->m_mode4=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont5")) m_configData->m_mode5=atoi(paramValue);

	if (!strcmp(paramName,"mode/cont6")) m_configData->m_mode6=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont7")) m_configData->m_mode7=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont8")) m_configData->m_mode8=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont9")) m_configData->m_mode9=atoi(paramValue);
	if (!strcmp(paramName,"mode/cont10")) m_configData->m_mode10=atoi(paramValue);

	if (!strcmp(paramName,"loot/worms")) m_configData->m_lootWorms=atoi(paramValue);
	if (!strcmp(paramName,"loot/inDepot")) m_configData->m_lootInDepot=atoi(paramValue);
	if (!strcmp(paramName,"loot/gp")) m_configData->m_lootGp=atoi(paramValue);
	if (!strcmp(paramName,"loot/food")) m_configData->m_lootFood=atoi(paramValue);
	if (!strcmp(paramName,"loot/custom")) m_configData->m_lootCustom=atoi(paramValue);
	if (!strcmp(paramName,"other/autoOpen")) m_configData->m_autoOpen=atoi(paramValue);
	if (!strcmp(paramName,"loot/eatFromCorpse")) m_configData->m_eatFromCorpse=atoi(paramValue);
}

char *CMod_looterApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"mode/cont1")) sprintf(buf,"%d",m_configData->m_mode1);
	if (!strcmp(paramName,"mode/cont2")) sprintf(buf,"%d",m_configData->m_mode2);
	if (!strcmp(paramName,"mode/cont3")) sprintf(buf,"%d",m_configData->m_mode3);
	if (!strcmp(paramName,"mode/cont4")) sprintf(buf,"%d",m_configData->m_mode4);
	if (!strcmp(paramName,"mode/cont5")) sprintf(buf,"%d",m_configData->m_mode5);

	if (!strcmp(paramName,"mode/cont6")) sprintf(buf,"%d",m_configData->m_mode6);
	if (!strcmp(paramName,"mode/cont7")) sprintf(buf,"%d",m_configData->m_mode7);
	if (!strcmp(paramName,"mode/cont8")) sprintf(buf,"%d",m_configData->m_mode8);
	if (!strcmp(paramName,"mode/cont9")) sprintf(buf,"%d",m_configData->m_mode9);
	if (!strcmp(paramName,"mode/cont10")) sprintf(buf,"%d",m_configData->m_mode10);

	if (!strcmp(paramName,"loot/worms")) sprintf(buf,"%d",m_configData->m_lootWorms);
	if (!strcmp(paramName,"loot/inDepot")) sprintf(buf,"%d",m_configData->m_lootInDepot);
	if (!strcmp(paramName,"loot/gp")) sprintf(buf,"%d",m_configData->m_lootGp);
	if (!strcmp(paramName,"loot/food")) sprintf(buf,"%d",m_configData->m_lootFood);
	if (!strcmp(paramName,"loot/custom")) sprintf(buf,"%d",m_configData->m_lootCustom);
	if (!strcmp(paramName,"other/autoOpen")) sprintf(buf,"%d",m_configData->m_autoOpen);
	if (!strcmp(paramName,"loot/eatFromCorpse")) sprintf(buf,"%d",m_configData->m_eatFromCorpse);

	return buf;
}

char *CMod_looterApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "mode/cont1";
	case 1: return "mode/cont2";
	case 2: return "mode/cont3";
	case 3: return "mode/cont4";
	case 4: return "mode/cont5";
	case 5: return "mode/cont6";
	case 6: return "mode/cont7";
	case 7: return "mode/cont8";
	case 8: return "mode/cont9";
	case 9: return "mode/cont10";
	case 10: return "loot/worms";
	case 11: return "loot/gp";
	case 12: return "loot/food";
	case 13: return "loot/custom";
	case 14: return "other/autoOpen";
	case 15: return "loot/inDepot";
	case 16: return "loot/eatFromCorpse";
	default:
		return NULL;
	}
}

void CMod_looterApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog){
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
