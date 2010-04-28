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


// mod_maphack.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <sys/stat.h> 
#include "mod_maphack.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "TAMiniMapProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMod_maphackApp

BEGIN_MESSAGE_MAP(CMod_maphackApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_maphackApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




/////////////////////////////////////////////////////////////////////////////
// Tool thread function

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;

DWORD WINAPI toolThreadProc( LPVOID lpParam )
{		
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CTAMiniMapProxy taMiniMap;
	CTibiaItemProxy itemProxy;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	int iter=0;

	sender.enableCName(1);
	while (!toolThreadShouldStop)
	{			
		iter++;
		Sleep(200);	
		if (reader.getConnectionState()!=8) continue; // do not proceed if not connected
		
		if (config->revealNoFish&&iter%4==0)//Checks every 0.8 seconds
		{			
			CTibiaCharacter* self=reader.readSelfCharacter();

			static int lastFishX=0,lastFishY=0,lastFishZ=0;
			if (lastFishX!=self->x || lastFishY!=self->y || lastFishZ!=self->z || iter%(4*2)==0){//runs at least every 1.6 seconds if not moved
				int x,y;
				for (x=-7;x<=7;x++)
				{
					for (y=-5;y<=5;y++)
					{
						int tileId = reader.mapGetPointItemId(point(x,y,0),0);
						if (tileId>=itemProxy.getValueForConst("waterWithFishStart")+12&&tileId<=itemProxy.getValueForConst("waterWithFishEnd")+12)
						{
							reader.mapSetPointItemId(point(x,y,0),0,727);
						}
					}
				}
				lastFishX=self->x;
				lastFishY=self->y;
				lastFishZ=self->z;
			}
			delete self;
		}
		if (config->revealCName&&iter%5==0)
		{
			reader.writeEnableRevealCName();
		}
		if (config->revealInvisible)
		{
			/*
			int creatureNr=0;
			for (creatureNr=0;creatureNr<memConstData.m_memMaxCreatures;creatureNr++)
			{
				CTibiaCharacter *cr=reader.readVisibleCreature(creatureNr);
				if (cr->monsterType==
				delete cr;
			}
			*/

		}
		if (config->smoothWalking&&iter%5==2)
		{
			
		}
		if (config->minimapResearch&&iter%2==0)
		{
			char buf[1111];
			CTibiaCharacter* self=reader.readSelfCharacter();

			static int lastX=0,lastY=0,lastZ=0,lastMinZ=0,lastMaxZ=0;
			if (lastX!=self->x || lastY!=self->y || lastZ!=self->z){
				//
				char mapArrCol[18][14][8];
				memset(mapArrCol,0,18*14*8);

				char mapArrSpd[18][14][8];
				memset(mapArrSpd,0,18*14*8);

				int x,y,z;
				int sigX=max(-1,min(1,self->x-lastX));
				int sigY=max(-1,min(1,self->y-lastY));
				int sigZ=max(-1,min(1,self->z-lastZ));

				int minZ=(self->z<=7?-self->z:-2);
				int maxZ=(self->z<=7?7-self->z:min(2,15-self->z));
				int minY=-6;
				int maxY=7;
				int minX=-8;
				int maxX=9;

				int minNewZ=max(minZ,min(maxZ, (lastZ-self->z)+(sigZ==1?lastMaxZ:lastMinZ)+sigZ ));
				int maxNewZ=minNewZ+max(minZ-maxZ-1,min(maxZ-minZ+1, (self->z-lastZ)+(sigZ==1?maxZ-lastMaxZ:minZ-lastMinZ) ));
				int minNewY=max(minY,min(maxY, (lastY-self->y)+(sigY==1?maxY:minY)+sigY ));
				int maxNewY=minNewY+max(minY-maxY-1,min(maxY-minY+1,self->y-lastY));
				int minNewX=max(minX,min( maxX,(lastX-self->x)+(sigX==1?maxX:minX)+sigX ));
				int maxNewX=minNewX+max(minX-maxX-1,min(maxX-minX+1,self->x-lastX));


				sprintf(buf,"%x looping sigs(%d,%d,%d) x:%d to %d  y:%d to %d  z:%d to %d minmax's %d,%d  %d,%d  %d,%d",&mapArrCol,sigX,sigY,sigZ,minNewX,maxNewX,minNewY,maxNewY,minNewZ,maxNewZ,minX,maxX,minY,maxY,minZ,maxZ);
				//AfxMessageBox(buf);

				int relToCell=reader.mapGetSelfCellNr();// the present location of self in map memory range 0-2016


				for (x=minNewX;x*sigX<maxNewX*sigX;x+=sigX){
					for (y=minY;y<=maxY;y++){
						for(z=minZ;z<=maxZ;z++){
							int i;
							int mapColour=-1;
							int mapSpeed=-1;
							int count=reader.mapGetPointItemsCount(point(x,y,z),relToCell);
							sprintf(buf,"Looking (%d,%d,%d) i=%d count=%d",x,y,z,i,count);
							//AfxMessageBox(buf);
							//start from top and go to bottom until found both a speed and a colour
							for (i=count-1;i>=0 && (mapColour==-1 || mapSpeed==-1);i--){
								int tileId = reader.mapGetPointItemId(point(x,y,z),i,relToCell);
								if (tileId==99) {mapSpeed=255; continue;}//reached top of relevant tiles

								CTibiaTile *tileData = reader.getTibiaTile(tileId);
								if(tileData->minimapColor && mapColour==-1) mapColour=tileData->minimapColor;
								if(tileData->speed && tileData->speed<256&& mapSpeed==-1) mapSpeed=tileData->speed;
								if(tileData->blocking || tileData->isTeleporter || tileData->goDown || tileData->goUp) mapSpeed=255;
							}
							if (mapColour==-1 || mapSpeed==-1){
								mapColour=0; mapSpeed=100; continue;
							}
							sprintf(buf,"Drawing (%d,%d,%d) %d %d",self->x+x,self->y+y,self->z+z,mapColour,mapSpeed);
							//AfxMessageBox(buf); 
							mapArrCol[x-minX][y-minY][z-minZ]=mapColour;
							mapArrSpd[x-minX][y-minY][z-minZ]=mapSpeed;
						}
					}
				}
				for (x=minX;x<=maxX;x++){
					for (y=minNewY;y*sigY<maxNewY*sigY;y+=sigY){
						for(z=minZ;z<=maxZ;z++){
							if(x>=minNewX && x<maxNewX) continue;//already did these points
							int i;
							int mapColour=-1;
							int mapSpeed=-1;
							int count=reader.mapGetPointItemsCount(point(x,y,z),relToCell);
							sprintf(buf,"Looking (%d,%d,%d) i=%d count=%d",x,y,z,i,count);
							//AfxMessageBox(buf);
							//start from top and go to bottom until found both a speed and a colour
							for (i=count-1;i>=0 && (mapColour==-1 || mapSpeed==-1);i--){
								int tileId = reader.mapGetPointItemId(point(x,y,z),i,relToCell);
								if (tileId==99) {mapSpeed=255; continue;}//reached top of relevant tiles

								CTibiaTile *tileData = reader.getTibiaTile(tileId);
								if(tileData->minimapColor && mapColour==-1)
									mapColour=tileData->minimapColor;
								if(tileData->speed && tileData->speed<256&& mapSpeed==-1) mapSpeed=tileData->speed;
								if(tileData->blocking || tileData->isTeleporter || tileData->goDown || tileData->goUp) mapSpeed=255;
							}
							if (mapColour==-1 || mapSpeed==-1){
								mapColour=0; mapSpeed=100; continue;
							}
							sprintf(buf,"Drawing (%d,%d,%d) %d %d",self->x+x,self->y+y,self->z+z,mapColour,mapSpeed);
							//AfxMessageBox(buf); 
							mapArrCol[x-minX][y-minY][z-minZ]=mapColour;
							mapArrSpd[x-minX][y-minY][z-minZ]=mapSpeed;
						}
					}
				}
				for (x=minX;x<=maxX;x++){
					for (y=minY;y<=maxY;y++){
						for(z=minNewZ;z*sigZ<maxNewZ*sigZ;z+=sigZ){
							if(x>=minNewX && x<maxNewX) continue;//already did these points
							if(y>=minNewY && y<maxNewY) continue;//already did these points
							int i;
							int mapColour=-1;
							int mapSpeed=-1;
							int count=reader.mapGetPointItemsCount(point(x,y,z),relToCell);
							sprintf(buf,"Looking (%d,%d,%d) i=%d count=%d",x,y,z,i,count);
							//AfxMessageBox(buf);
							//start from top and go to bottom until found both a speed and a colour
							for (i=count-1;i>=0 && (mapColour==-1 || mapSpeed==-1);i--){
								int tileId = reader.mapGetPointItemId(point(x,y,z),i,relToCell);
								if (tileId==99) {mapSpeed=255; continue;}//reached top of relevant tiles

								CTibiaTile *tileData = reader.getTibiaTile(tileId);
								if(tileData->minimapColor && mapColour==-1) mapColour=tileData->minimapColor;
								if(tileData->speed && tileData->speed<256 && mapSpeed==-1) mapSpeed=tileData->speed;
								if(tileData->blocking || tileData->isTeleporter || tileData->goDown || tileData->goUp) mapSpeed=255;
							}
							if (mapColour==-1 || mapSpeed==-1){
								mapColour=0; mapSpeed=100; continue;
							}
							sprintf(buf,"Drawing (%d,%d,%d) %d %d",self->x+x,self->y+y,self->z+z,mapColour,mapSpeed);
							//AfxMessageBox(buf); 
							mapArrCol[x-minX][y-minY][z-minZ]=mapColour;
							mapArrSpd[x-minX][y-minY][z-minZ]=mapSpeed;
						}
					}
				}
				CTibiaCharacter* newSelf=reader.readSelfCharacter();
				int fileExists;
				int last = 0;
				if (newSelf->z==self->z){//since we used relToCell only floorchanges drastically matter
					// tiles changed between read from tibia and write to map are excluded
					for(z=minZ;z<=maxZ;z++){
						for (x=-8+max(0,newSelf->x-self->x);x<=9+min(0,newSelf->x-self->x);x++){
							for (y=-6+max(0,newSelf->y-self->y);y<=7+min(0,newSelf->y-self->y);y++){
/*
								if (last !=(int)((self->x+x-z)/256)*256*30+(int)((self->y+y-z)/256)*30+self->z+z){
									char filename[1024+20];
									sprintf(filename,"%s%s%03d%03d%02d.map",getenv("USERPROFILE"),"/Application Data/Tibia/Automap/",(int)((self->x+x-z)/256),(int)((self->y+y-z)/256),self->z+z);
									FILE *f=fopen(filename,"r");
									fileExists=(f?1:0);
									if (f) fclose(f);
									last =(int)((self->x+x-z)/256)*256*30+(int)((self->y+y-z)/256)*30+self->z+z;
								}
*/
								//if(mapArrSpd[x-(-8)][y-(-6)][z-minZ]!=0 && (mapArrCol[x-(-8)][y-(-6)][z-minZ]!=0 || fileExists)){
								if(mapArrSpd[x-(-8)][y-(-6)][z-minZ]!=0){
									reader.writeMiniMapPoint(self->x+x-z,self->y+y-z,self->z+z,mapArrCol[x-(-8)][y-(-6)][z-minZ],mapArrSpd[x-(-8)][y-(-6)][z-minZ]);
								}
							}
						}
					}
					lastX=self->x;
					lastY=self->y;
					lastZ=self->z;

					lastMinZ=minZ;
					lastMaxZ=maxZ;
				}
				delete newSelf;


			}
			delete self;

		}
				
	}

	sender.enableCName(0);
	reader.writeDisableRevealCName();
	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_maphackApp construction

CMod_maphackApp::CMod_maphackApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();	
}

CMod_maphackApp::~CMod_maphackApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_maphackApp::getName()
{
	return "Map hack";
}


int CMod_maphackApp::isStarted()
{
	return m_started;
}


void CMod_maphackApp::start()
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

void CMod_maphackApp::stop()
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

void CMod_maphackApp::showConfigDialog()
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


void CMod_maphackApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_maphackApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_maphackApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_maphackApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_maphackApp::getVersion()
{
	return "1.4";
}


int CMod_maphackApp::validateConfig(int showAlerts)
{
	
	return 1;
}

void CMod_maphackApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_maphackApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"reveal/noFish")) m_configData->revealNoFish=atoi(paramValue);
	if (!strcmp(paramName,"reveal/cName")) m_configData->revealCName=atoi(paramValue);	
	if (!strcmp(paramName,"reveal/invisible")) m_configData->revealInvisible=atoi(paramValue);
	if (!strcmp(paramName,"reveal/minimap")) m_configData->minimapResearch=atoi(paramValue);
	if (!strcmp(paramName,"smoothWalking")) m_configData->smoothWalking=atoi(paramValue);
}

char *CMod_maphackApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"reveal/noFish")) sprintf(buf,"%d",m_configData->revealNoFish);
	if (!strcmp(paramName,"reveal/cName")) sprintf(buf,"%d",m_configData->revealCName);	
	if (!strcmp(paramName,"reveal/invisible")) sprintf(buf,"%d",m_configData->revealInvisible);
	if (!strcmp(paramName,"reveal/minimap")) sprintf(buf,"%d",m_configData->minimapResearch);
	if (!strcmp(paramName,"smoothWalking")) sprintf(buf,"%d",m_configData->smoothWalking);

	return buf;
}

char *CMod_maphackApp::getConfigParamName(int nr)
{
	switch (nr)
	{
	case 0: return "reveal/noFish";
	case 1: return "reveal/cName";
	case 2: return "reveal/invisible";
	case 3: return "reveal/minimap";
	case 4: return "smoothWalking";
	default:
		return NULL;
	}
}

void CMod_maphackApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin(	m_configDialog->m_OK);
	skin.SetButtonSkin(	m_configDialog->m_enable);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog)
		m_configDialog->Invalidate();
}
