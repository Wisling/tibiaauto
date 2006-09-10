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


// mod_autogo.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_autogo.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "TibiaMapProxy.h"
#include "ModuleUtil.h"
#include <MMSystem.h>
#include <Tlhelp32.h>

#include "IPCBackPipeProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EWX_FORCEIFHUNG      0x00000010


/////////////////////////////////////////////////////////////////////////////
// CMod_autogoApp

BEGIN_MESSAGE_MAP(CMod_autogoApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_autogoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;
char suspendedModules[20][64];
int suspendedCount = 0;


int actionPos2ID(int pos){
	switch (pos){
		case ACTION_NONE_POS:		return ACTION_NONE;
		case ACTION_SUSPEND_POS:	return ACTION_SUSPEND;
		case ACTION_LOGOUT_POS:		return ACTION_LOGOUT;
		case ACTION_KILL_POS:		return ACTION_KILL;
		case ACTION_SHUTDOWN_POS:	return ACTION_SHUTDOWN;
		case ACTION_RUNAWAY_POS:	return ACTION_RUNAWAY;
		default:					return 0;
	}

}

void actionTerminate(){
	CMemReaderProxy reader;
	HANDLE hTibiaProc;

	hTibiaProc = OpenProcess(PROCESS_TERMINATE,true,reader.getProcessId());

	TerminateProcess(hTibiaProc,0);

	CloseHandle(hTibiaProc);
}

int actionShutdownSystem(){
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
	//T4: Privilages are needed for XP & 2000
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken)){
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
		
        tkp.PrivilegeCount = 1; 
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

        if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)){
			ExitWindowsEx(EWX_SHUTDOWN|EWX_POWEROFF|EWX_FORCEIFHUNG,0);
			return 1;
		}
	}
	return 0;
}

int actionSuspend(int lSuspend){
	if (lSuspend){
		HANDLE hSnap;
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
		if (hSnap){
			MODULEENTRY32 lpModule;
			lpModule.dwSize = sizeof(MODULEENTRY32);

			Module32First(hSnap,&lpModule);
			do {
				if (strcmpi(lpModule.szModule,"mod_autogo.dll")!=0){
					FARPROC isStarted;
					isStarted = GetProcAddress(lpModule.hModule,"isStarted");
					if (isStarted){
						if (isStarted()){
							/*FARPROC stop;
							stop = GetProcAddress(lpModule.hModule,"stop");
							stop();*/
							GetProcAddress(lpModule.hModule,"stop")();
							lstrcpyn(suspendedModules[suspendedCount++],lpModule.szModule,63);
						}
					}
				}
			}while (Module32Next(hSnap,&lpModule));

			CloseHandle(hSnap);
		}
	}else{
		int i;
		for (i=0;i<suspendedCount;i++){
			HMODULE hModule;
			hModule = GetModuleHandle(suspendedModules[i]);
			if (hModule){
				FARPROC start;
				start = GetProcAddress(hModule,"start");
				start();
			}
		}
		suspendedCount=0;
	}
	return 0;
}

int OnList(char whiteList[][32],char name[]){
	int i=0;
	while (IsCharAlphaNumeric(whiteList[i][0])){
		if (!strcmpi(whiteList[i],name)){
			return 1;
		}
		i++;
	}

	return 0;
}

int triggerBattleList(int options, char whiteList[][32]){
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	CTibiaCharacter *self = reader.readSelfCharacter();
	int creatureNr;
	int iRet=0;
			
	for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++){
		CTibiaCharacter *ch=reader.readVisibleCreature(creatureNr);
		int found = 0;
		
		if (ch->visible){		
			if ((ch->x != self->x || ch->y != self->y) &&  !OnList(whiteList,(char*)ch->name)){			
				if ((options&BATTLELIST_PARANOIAM)) {
					found=1;
				}
				if ((options&BATTLELIST_PARANOIA)&&ch->z==self->z){
					found=1;
				};
				if(reader.readBattleListMax()>=0||reader.readBattleListMin()>=0){
					found=1;
				}
			
				if (found){
					if (ch->name[0]=='G'&&ch->name[1]=='M' && options&BATTLELIST_GM)
						iRet=iRet|BATTLELIST_GM;
					else if (options&BATTLELIST_PLAYER && ch->tibiaId < 0x4000000)
						iRet=iRet|BATTLELIST_PLAYER;
					else if (options&BATTLELIST_MONSTER && ch->tibiaId >= 0x4000000)
						iRet=iRet|BATTLELIST_MONSTER;
				}
			}
		};	
		delete ch;
	};
	delete self;

	return iRet;
}

int triggerMessage(){
	CMemReaderProxy reader;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	if (backPipe.readFromPipe(&mess,1003)){
		int infoType;
		int nickLen;
		char nickBuf[32];

		memset(nickBuf,0,32);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&nickLen,mess.payload+4,sizeof(int));
		memcpy(nickBuf,mess.payload+12,nickLen);
		
		CTibiaCharacter *temp = reader.readSelfCharacter();
		//T4: Name in temp structure is empty, bug?
		CTibiaCharacter *self = reader.getCharacterByTibiaId(temp->tibiaId);
		delete temp;

		if (strcmpi(nickBuf,self->name)!=0 && strcmpi(nickBuf,"Tibia Auto")!=0){
			delete self;
			return infoType;
		}
		delete self;
	}
	return -1;
}

int triggerBlank(){
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	int ret=0;
	int pos;
	for (pos=0;pos<10;pos++){
		int itemNr;
		CTibiaContainer *container = reader.readContainer(pos);
		//T4: Update missing OnOff flag checking
		for (itemNr=0;itemNr<container->itemsInside && container->flagOnOff;itemNr++){
			CTibiaItem *item = (CTibiaItem *)container->items.GetAt(itemNr);
			if (item->objectId==itemProxy.getValueForConst("runeBlank")){
				// blank rune found!
				ret++;
			};
		};								
		delete container;
	};
	return ret;
}

int triggerNoSpace(){
	CMemReaderProxy reader;
	int ret=1;
	int pos;
	for (pos=0;pos<10;pos++){
		CTibiaContainer *container = reader.readContainer(pos);
		if (container->flagOnOff && container->itemsInside < container->size){
			ret = 0;
			break;
		}
		delete container;
	};
	return ret;
}

int triggerOutOf(int options){
	CMemReaderProxy reader;
	CTibiaItemProxy itemProxy;
	CTibiaItem *Item = NULL;
	CUIntArray itemArray;
			
	itemArray.Add(itemProxy.getValueForConst("worms"));
	int pos;
	int ret=0;	//Bitfield
	
	for (pos=0;pos<10&&!ret;pos++){
		//T4: Missing check if container is opened
		CTibiaContainer *container = reader.readContainer(pos);
		if (container->flagOnOff){
			if (options&OUTOF_FOOD && !(ret&OUTOF_FOOD)){
				Item = CModuleUtil::lookupItem(pos,itemProxy.getItemsFoodArray());
				if (Item != NULL){
					ret = ret|OUTOF_FOOD;
					delete Item;
				}
			}
			if (options&OUTOF_WORMS && !(ret&OUTOF_WORMS)){
				Item = CModuleUtil::lookupItem(pos,&itemArray);
				if (Item != NULL){
					ret = ret|OUTOF_WORMS;
					delete Item;
				}
			}
			if (options&OUTOF_SPACE && !(ret&OUTOF_SPACE)){
				if (container->itemsInside < container->size)
					ret = ret|OUTOF_SPACE;
			}
		}
		delete container;
	}
	//T4: As in code ret saves is certain thing in backpack, at the end I revers bits
	return (ret^options);
}

void alarmSound(int alarmId,int alarmSubId){
	OFSTRUCT lpOpen;
	char wavFile[1024];

	switch (alarmId){
		case TRIGGER_BATTLELIST:strcpy(wavFile,".\\mods\\sound\\battlelist.wav");break;
		case TRIGGER_SIGN:		strcpy(wavFile,".\\mods\\sound\\sign.wav");break;
		case TRIGGER_MESSAGE:	strcpy(wavFile,".\\mods\\sound\\message.wav");break;
		case TRIGGER_MOVE:		strcpy(wavFile,".\\mods\\sound\\move.wav");break;
		case TRIGGER_HPLOSS:	strcpy(wavFile,".\\mods\\sound\\hploss.wav");break;
		case TRIGGER_HPBELOW:	strcpy(wavFile,".\\mods\\sound\\hpbelow.wav");break;
		case TRIGGER_SOULPOINT:	strcpy(wavFile,".\\mods\\sound\\soulpoint.wav");break;
		case TRIGGER_BLANK:		strcpy(wavFile,".\\mods\\sound\\blank.wav");break;
		case TRIGGER_CAPACITY:	strcpy(wavFile,".\\mods\\sound\\capacity.wav");break;
		case TRIGGER_OUTOF:		strcpy(wavFile,".\\mods\\sound\\outof.wav");break;
		default:				strcpy(wavFile,".\\mods\\sound\\alarm.wav");break;
	}

	if (OpenFile(wavFile,&lpOpen,OF_EXIST) != HFILE_ERROR){
		PlaySound(wavFile,NULL,SND_FILENAME|SND_ASYNC);
		// isn't some CloseFile() needed? -- vanitas
	}else if(OpenFile(".\\mods\\sound\\alarm.wav",&lpOpen,OF_EXIST) != HFILE_ERROR){
		PlaySound(".\\mods\\sound\\alarm.wav",NULL,SND_FILENAME|SND_ASYNC);
		// isn't some wait needed? cause multiple async plays will kill CPU --vanitas
	}else{
		MessageBeep(MB_OK);
	}
}
char *alarmStatus(int alarmId, int alarmSubId){
	//TODO: SubId support
	if (alarmId&TRIGGER_BATTLELIST) return "BattleList alarm";
	if (alarmId&TRIGGER_SIGN)		return "A sign appeared";
	if (alarmId&TRIGGER_MESSAGE)	return "New message";
	if (alarmId&TRIGGER_MOVE)		return "Your moving";
	if (alarmId&TRIGGER_HPLOSS)		return "You have lost HP";
	if (alarmId&TRIGGER_HPBELOW)	return "Your HP is below certain value";
	if (alarmId&TRIGGER_SOULPOINT)	return "Too few soul points";
	if (alarmId&TRIGGER_BLANK)		return "Too few blank runes";
	if (alarmId&TRIGGER_CAPACITY)	return "Capacity is too small";
	if (alarmId&TRIGGER_OUTOF)		return "You run out of food/worms/space";
	return "";
}

void alarmAction(int alarmId, int alarmSubId,CConfigData *config){
	CMemReaderProxy reader;
	CPackSenderProxy sender;

	strcpy(config->status,alarmStatus(alarmId,alarmSubId));

	//sender.sendTAMessage(statusInfo(alarmId));
	/*if (config->sound&alarmId){
		playSound(alarmId);
	}*/

	int iAction=0;

	if (alarmId&TRIGGER_BATTLELIST)	{iAction|=actionPos2ID(config->actionBattleList);}
	if (alarmId&TRIGGER_SIGN)		{iAction|=actionPos2ID(config->actionSign);}
	if (alarmId&TRIGGER_MESSAGE)	{iAction|=actionPos2ID(config->actionMessage);}
	if (alarmId&TRIGGER_MOVE)		{iAction|=actionPos2ID(config->actionMove);}
	if (alarmId&TRIGGER_HPLOSS)		{iAction|=actionPos2ID(config->actionHpLoss);}
	if (alarmId&TRIGGER_HPBELOW)	{iAction|=actionPos2ID(config->actionHpBelow);}
	if (alarmId&TRIGGER_SOULPOINT)	{iAction|=actionPos2ID(config->actionSoulPoint);}
	if (alarmId&TRIGGER_BLANK)		{iAction|=actionPos2ID(config->actionBlank);}
	if (alarmId&TRIGGER_CAPACITY)	{iAction|=actionPos2ID(config->actionCapacity);}
	if (alarmId&TRIGGER_OUTOF)		{iAction|=actionPos2ID(config->actionOutOf);}

	//if (iAction&ACTION_SUSPEND){
		actionSuspend(iAction&ACTION_SUSPEND);
	//}
	if (iAction&ACTION_LOGOUT){				
		//sender.sendTAMessage("Logout");
		sender.logout();
	}
	if (iAction&ACTION_KILL){
		//sender.sendTAMessage("Kill");
		sender.logout();
		Sleep(1500);

		actionTerminate();
	}
	if (iAction&ACTION_SHUTDOWN){
		sender.logout();
		Sleep(1500);			
		if (!actionShutdownSystem()){
			actionTerminate();
		};
		Sleep(2000);
			
	}
	if (iAction&ACTION_RUNAWAY){
		CTibiaCharacter *self = reader.readSelfCharacter();
		CMemConstData memConstData = reader.getMemConstData();
		CTibiaMapProxy tibiaMap;
		
		int path[10000];
		delete self;
		self = reader.readSelfCharacter();

		if (self->x!=config->runawayX || self->y!=config->runawayY || self->z!=config->runawayZ){
		
			// proceed with path searching									
			CModuleUtil::findPathOnMap(self->x,self->y,self->z,config->runawayX,config->runawayY,config->runawayZ,path);											
			int pathSize;
			for (pathSize=0;pathSize<10000&&path[pathSize];pathSize++){}										
			if (pathSize){
				CModuleUtil::executeWalk(path);
				CModuleUtil::sleepWithStop(500,&toolThreadShouldStop);
			}
		}
		delete self;
	}else if (config->action&ACTION_RUNAWAY){
		CTibiaCharacter *self = reader.readSelfCharacter();
		CMemConstData memConstData = reader.getMemConstData();
		CTibiaMapProxy tibiaMap;

		int path[10000];
		delete self;
		self = reader.readSelfCharacter();

		if (self->x!=config->actX || self->y!=config->actY || self->z!=config->actZ){

			CModuleUtil::sleepWithStop(2000,&toolThreadShouldStop);
			
			// proceed with path searching									
			CModuleUtil::findPathOnMap(self->x,self->y,self->z,config->actX,config->actY,config->actZ,path);											
			int pathSize;
			for (pathSize=0;pathSize<10000&&path[pathSize];pathSize++){}										
			if (pathSize){
				CModuleUtil::executeWalk(path);											
			}
		}else{
			if (config->actDirection){
				CPackSenderProxy sender;
				if (config->actDirection==DIR_LEFT){
					sender.turnLeft();
				}else if(config->actDirection==DIR_RIGHT){
					sender.turnRight();
				}else if(config->actDirection==DIR_UP){
					sender.turnUp();
				}else if(config->actDirection==DIR_DOWN){
					sender.turnDown();
				}
			}
		}
		delete self;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;

	CTibiaCharacter *self = reader.readSelfCharacter();

	int lastX = self->x;
	int lastY = self->y;
	int lastZ = self->z;
	int lastHp = self->hp;

	int iAlarm;
	int alarm;

	delete self;

	PlaySound(0, 0, 0);

	//while (triggerMessage()>0){};//Remove all msg in queue
	
	while (!toolThreadShouldStop)
	{			
		Sleep(100);	
		alarm = 0;
		
		if ((config->trigger&TRIGGER_BATTLELIST)){
			iAlarm = triggerBattleList(config->optionsBattleList,config->whiteList);
			if (iAlarm){
				alarm |= TRIGGER_BATTLELIST;
				if (config->sound&TRIGGER_BATTLELIST)
					alarmSound(TRIGGER_BATTLELIST,iAlarm);
			}
		}

		if (config->trigger&TRIGGER_SIGN){
			int flags = reader.getSelfEventFlags()&config->optionsSign;
			if (flags){
				alarm |= TRIGGER_SIGN;
				if (config->sound&TRIGGER_SIGN)
					alarmSound(TRIGGER_SIGN,flags);
			}
			//0x00000001 - poison;
			//0x00000002 - fire;
			//0x00000004 - energy;
			//0x00000008 - drunk;
			//0x00000010 - shield;
			//0x00000020 - paralize;
			//0x00000040 - haste
			//0x00000080 - attack;
			/*char buffer[1024];
			sprintf(buffer,"0x%x",reader.getSelfEventFlags());
			sender.sendTAMessage(buffer);
			Sleep(1000);*/
		}
		if (config->trigger&TRIGGER_MESSAGE){
			int iType = triggerMessage();
			if (config->optionsMessage&MESSAGE_PUBLIC && (iType == 1 || iType == 2) /*|| iType == 3*/){
				alarm |= TRIGGER_MESSAGE;
				if (config->sound&TRIGGER_MESSAGE)
					alarmSound(TRIGGER_MESSAGE,MESSAGE_PUBLIC);
			}
			if (config->optionsMessage&MESSAGE_PRIVATE && iType == 4){
				alarm |= TRIGGER_MESSAGE;
				if (config->sound&TRIGGER_MESSAGE)
					alarmSound(TRIGGER_MESSAGE,MESSAGE_PRIVATE);
			}

		}
		if (config->trigger&TRIGGER_MOVE){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (lastX != self->x || lastY != self->y || lastZ != self->z){
				alarm |= TRIGGER_MOVE;
				if (config->sound&TRIGGER_MOVE)
					alarmSound(TRIGGER_MOVE,0);
			}
			lastX = self->x;
			lastY = self->y;
			lastZ = self->z;
			delete self;
		}
		if (config->trigger&TRIGGER_HPLOSS){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (lastHp > self->hp ){
				alarm |= TRIGGER_HPLOSS;
				if (config->sound&TRIGGER_HPLOSS)
					alarmSound(TRIGGER_HPLOSS,lastHp-self->hp);
			}
			lastHp = self->hp;
			delete self;
		}
		if (config->trigger&TRIGGER_HPBELOW){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->hp < config->optionsHpBelow){
				alarm |= TRIGGER_HPBELOW;
				if (config->sound&TRIGGER_HPBELOW)
					alarmSound(TRIGGER_HPBELOW,config->optionsHpBelow);
			}
			lastHp = self->hp;
			delete self;
		}
		if (config->trigger&TRIGGER_SOULPOINT){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->soulPoints < config->optionsSoulPoint){
				alarm |= TRIGGER_SOULPOINT;
				if (config->sound&TRIGGER_SOULPOINT)
					alarmSound(TRIGGER_SOULPOINT,0);
			}
			delete self;
		}
		if (config->trigger&TRIGGER_BLANK){
			if (triggerBlank() < config->optionsBlank){
				alarm |= TRIGGER_BLANK;
				if (config->sound&TRIGGER_BLANK)
					alarmSound(TRIGGER_BLANK,0);
			}
		}
		if (config->trigger&TRIGGER_CAPACITY){
			CTibiaCharacter *self = reader.readSelfCharacter();
			if (self->cap < config->optionsCapacity){
				alarm |= TRIGGER_CAPACITY;
				if (config->sound&TRIGGER_CAPACITY)
					alarmSound(TRIGGER_CAPACITY,0);
			}
			delete self;
		}
		if (config->trigger&TRIGGER_OUTOF){
			iAlarm = triggerOutOf(config->optionsOutOf);
			if (iAlarm&config->optionsOutOf){
				alarm |= TRIGGER_OUTOF;
				if (config->sound&TRIGGER_OUTOF)
					alarmSound(TRIGGER_OUTOF,iAlarm);
			}
		}

		alarmAction(alarm,0,config);
	}
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_autogoApp construction

CMod_autogoApp::CMod_autogoApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_autogoApp::~CMod_autogoApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_autogoApp::getName()
{
	return "Auto go/logout";
}


int CMod_autogoApp::isStarted()
{
	if (!m_started)
	{
		// if not started then regularry consume 1003 messages from the queue
		CIPCBackPipeProxy backPipe;
		struct ipcMessage mess;	

		backPipe.readFromPipe(&mess,1003);
	}
	return m_started;
}


void CMod_autogoApp::start()
{	

	superStart();
	if (m_configDialog)
	{
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_autogoApp::stop()
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

void CMod_autogoApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_autogoApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_autogoApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_autogoApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_autogoApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_autogoApp::getVersion()
{	
	return "2.1";
}


int CMod_autogoApp::validateConfig(int showAlerts)
{
	if ((m_configData->optionsBattleList&BATTLELIST_PARANOIA)||(m_configData->optionsBattleList&BATTLELIST_PARANOIAM))
	{
		if (!(m_configData->optionsBattleList&BATTLELIST_GM)&&
			!(m_configData->optionsBattleList&BATTLELIST_PLAYER)&&
			!(m_configData->optionsBattleList&BATTLELIST_MONSTER))
		{
			if (showAlerts) AfxMessageBox("When using paranoia modes, one of 'gm', 'player','monster' must always be selected!");
			return 0;
		}
	}
	return 1;
}

void CMod_autogoApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_autogoApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"act/x"))				m_configData->actX				= atoi(paramValue);
	if (!strcmp(paramName,"act/y"))				m_configData->actY				= atoi(paramValue);
	if (!strcmp(paramName,"act/z"))				m_configData->actZ				= atoi(paramValue);
	if (!strcmp(paramName,"act/direction"))		m_configData->actDirection		= atoi(paramValue);
	if (!strcmp(paramName,"runaway/x"))			m_configData->runawayX			= atoi(paramValue);
	if (!strcmp(paramName,"runaway/y"))			m_configData->runawayY			= atoi(paramValue);
	if (!strcmp(paramName,"runaway/z"))			m_configData->runawayZ			= atoi(paramValue);
	if (!strcmp(paramName,"trigger"))			m_configData->trigger			= atoi(paramValue);
	if (!strcmp(paramName,"action/BattleList"))	m_configData->actionBattleList	= atoi(paramValue);
	if (!strcmp(paramName,"action/Sign"))		m_configData->actionSign		= atoi(paramValue);
	if (!strcmp(paramName,"action/Message"))	m_configData->actionMessage		= atoi(paramValue);
	if (!strcmp(paramName,"action/HpLoss"))		m_configData->actionHpLoss		= atoi(paramValue);
	if (!strcmp(paramName,"action/HpBelow"))	m_configData->actionHpBelow		= atoi(paramValue);
	if (!strcmp(paramName,"action/Move"))		m_configData->actionMove		= atoi(paramValue);
	if (!strcmp(paramName,"action/SoulPoint"))	m_configData->actionSoulPoint	= atoi(paramValue);
	if (!strcmp(paramName,"action/Blank"))		m_configData->actionBlank		= atoi(paramValue);
	if (!strcmp(paramName,"action/Capacity"))	m_configData->actionCapacity	= atoi(paramValue);
	if (!strcmp(paramName,"action/OutOf"))		m_configData->actionOutOf		= atoi(paramValue);
	if (!strcmp(paramName,"options/BattleList"))m_configData->optionsBattleList	= atoi(paramValue);
	if (!strcmp(paramName,"options/Sign"))		m_configData->optionsSign		= atoi(paramValue);
	if (!strcmp(paramName,"options/Message"))	m_configData->optionsMessage	= atoi(paramValue);
	if (!strcmp(paramName,"options/HpBelow"))	m_configData->optionsHpBelow	= atoi(paramValue);
	if (!strcmp(paramName,"options/SoulPoint"))	m_configData->optionsSoulPoint	= atoi(paramValue);
	if (!strcmp(paramName,"options/Blank"))		m_configData->optionsBlank		= atoi(paramValue);
	if (!strcmp(paramName,"options/Capacity"))	m_configData->optionsCapacity	= atoi(paramValue);
	if (!strcmp(paramName,"options/OutOf"))		m_configData->optionsOutOf		= atoi(paramValue);
	if (!strcmp(paramName,"sound"))				m_configData->sound				= atoi(paramValue);
	if (!strcmp(paramName,"whiteList")){
		if (currentPos>99)
			return;
		lstrcpyn(m_configData->whiteList[currentPos++],paramValue,32);
	}
}

char *CMod_autogoApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"act/x"))				sprintf(buf,"%d",m_configData->actX);
	if (!strcmp(paramName,"act/y"))				sprintf(buf,"%d",m_configData->actY);
	if (!strcmp(paramName,"act/z"))				sprintf(buf,"%d",m_configData->actZ);
	if (!strcmp(paramName,"act/direction"))		sprintf(buf,"%d",m_configData->actDirection);
	if (!strcmp(paramName,"runaway/x"))			sprintf(buf,"%d",m_configData->runawayX);
	if (!strcmp(paramName,"runaway/y"))			sprintf(buf,"%d",m_configData->runawayY);
	if (!strcmp(paramName,"runaway/z"))			sprintf(buf,"%d",m_configData->runawayZ);
	if (!strcmp(paramName,"trigger"))			sprintf(buf,"%d",m_configData->trigger);
	if (!strcmp(paramName,"action/BattleList"))	sprintf(buf,"%d",m_configData->actionBattleList);
	if (!strcmp(paramName,"action/Sign"))		sprintf(buf,"%d",m_configData->actionSign);
	if (!strcmp(paramName,"action/Message"))	sprintf(buf,"%d",m_configData->actionMessage);
	if (!strcmp(paramName,"action/HpLoss"))		sprintf(buf,"%d",m_configData->actionHpLoss);
	if (!strcmp(paramName,"action/HpBelow"))	sprintf(buf,"%d",m_configData->actionHpBelow);
	if (!strcmp(paramName,"action/Move"))		sprintf(buf,"%d",m_configData->actionMove);
	if (!strcmp(paramName,"action/SoulPoint"))	sprintf(buf,"%d",m_configData->actionSoulPoint);
	if (!strcmp(paramName,"action/Blank"))		sprintf(buf,"%d",m_configData->actionBlank);
	if (!strcmp(paramName,"action/Capacity"))	sprintf(buf,"%d",m_configData->actionCapacity);
	if (!strcmp(paramName,"action/OutOf"))		sprintf(buf,"%d",m_configData->actionOutOf);
	if (!strcmp(paramName,"options/BattleList"))sprintf(buf,"%d",m_configData->optionsBattleList);
	if (!strcmp(paramName,"options/Sign"))		sprintf(buf,"%d",m_configData->optionsSign);
	if (!strcmp(paramName,"options/Message"))	sprintf(buf,"%d",m_configData->optionsMessage);
	if (!strcmp(paramName,"options/HpBelow"))	sprintf(buf,"%d",m_configData->optionsHpBelow);
	if (!strcmp(paramName,"options/SoulPoint"))	sprintf(buf,"%d",m_configData->optionsSoulPoint);
	if (!strcmp(paramName,"options/Blank"))		sprintf(buf,"%d",m_configData->optionsBlank);
	if (!strcmp(paramName,"options/Capacity"))	sprintf(buf,"%d",m_configData->optionsCapacity);
	if (!strcmp(paramName,"options/OutOf"))		sprintf(buf,"%d",m_configData->optionsOutOf);
	if (!strcmp(paramName,"sound"))				sprintf(buf,"%d",m_configData->sound);
	if (!strcmp(paramName,"whiteList")){		
		if (currentPos<100){				
			if (IsCharAlphaNumeric(m_configData->whiteList[currentPos][0])){				
				lstrcpyn(buf,m_configData->whiteList[currentPos++],32);
			}
		}		
	}

	return buf;
}

char *CMod_autogoApp::getConfigParamName(int nr)
{

	switch (nr)
	{
	case 0: return "act/x";
	case 1: return "act/y";
	case 2: return "act/z";
	case 3: return "act/direction";
	case 4: return "runaway/x";
	case 5: return "runaway/y";
	case 6: return "runaway/z";
	case 7: return "trigger";
	case 8: return "action/BattleList";
	case 9: return "action/Sign";
	case 10: return "action/Message";
	case 11: return "action/HpLoss";
	case 12: return "action/HpBelow";
	case 13: return "action/Move";
	case 14: return "action/SoulPoint";
	case 15: return "action/Blank";
	case 16: return "action/Capacity";
	case 17: return "action/NoSpace";
	case 18: return "action/OutOf";
	case 19: return "options/BattleList";
	case 20: return "options/Sign";
	case 21: return "options/Message";
	case 22: return "options/HpBelow";
	case 23: return "options/SoulPoint";
	case 24: return "options/Blank";
	case 25: return "options/Capacity";
	case 26: return "options/OutOf";;
	case 27: return "sound";
	case 28: return "whiteList";
	default:
		return NULL;
	}
}
int CMod_autogoApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName,"whiteList")) return 1;
	return 0;
}

void CMod_autogoApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName,"whiteList")) currentPos=0;
}
