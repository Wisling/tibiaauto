// PackSender.cpp: implementation of the CPackSender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PackSender.h"
#include "MemReader.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
HANDLE hPipe=INVALID_HANDLE_VALUE;
struct ipcMessage
{
	int messageType;
	char payload[1024];
public:
	void send()
	{

		
		DWORD cbWritten;
		BOOL fSuccess = WriteFile( 
			hPipe,
			this,
			sizeof(struct ipcMessage), 
			&cbWritten,
			NULL);				
	}
	
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPackSender::CPackSender()
{

}

CPackSender::~CPackSender()
{

}


void CPackSender::sendPacket(char *buf)
{
	sendPacket(buf,2);
}

void CPackSender::sendPacket(char *buf,int method)
{	
	// check if we are connected
	int lowB=buf[0];
	int hiB=buf[1];
	if (lowB<0) lowB+=256;
	if (hiB<0) hiB+=256;
	int len=lowB+hiB*256+2;

	struct ipcMessage mess;		
	DWORD cbWritten;

	mess.messageType=method;
	memcpy(mess.payload,buf,len);

	
	mess.send();	
}


void CPackSender::moveObjectBetweenContainers(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty)
{
	char retbuf[256];

	retbuf[0]=15;
	retbuf[1]=0;

	retbuf[2]=0x78;

	retbuf[3]=0xff;
	retbuf[4]=0xff;
	retbuf[5]=sourceContNr;
	retbuf[6]=0;
	retbuf[7]=sourcePos;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=sourcePos;

	retbuf[11]=0xff;
	retbuf[12]=0xff;
	retbuf[13]=targetContNr;
	retbuf[14]=0;
	retbuf[15]=targetPos;

	retbuf[16]=qty;

	sendPacket(retbuf);
}


void CPackSender::say(const char *buf)
{
	char retbuf[65536];

	retbuf[0]=strlen(buf)+4;
	retbuf[1]=0;
	retbuf[2]=0x96;
	retbuf[3]=0x01;
	retbuf[4]=strlen(buf);
	retbuf[5]=0;
	sprintf(retbuf+6,"%s",buf);

	sendPacket(retbuf);
}

void CPackSender::sayWhisper(const char *buf)
{
	char retbuf[65536];

	retbuf[0]=strlen(buf)+4;
	retbuf[1]=0;
	retbuf[2]=0x96;
	retbuf[3]=0x02;
	retbuf[4]=strlen(buf);
	retbuf[5]=0;
	sprintf(retbuf+6,"%s",buf);

	sendPacket(retbuf);
}

void CPackSender::sayYell(const char *buf)
{
	char retbuf[65536];

	retbuf[0]=strlen(buf)+4;
	retbuf[1]=0;
	retbuf[2]=0x96;
	retbuf[3]=0x03;
	retbuf[4]=strlen(buf);
	retbuf[5]=0;
	sprintf(retbuf+6,"%s",buf);

	sendPacket(retbuf);
}

void CPackSender::useItemInContainer(int objectId, int contNr, int pos)
{
	char retbuf[256];

	retbuf[0]=10;
	retbuf[1]=0;

	retbuf[2]=0x82;

	retbuf[3]=0xff;
	retbuf[4]=0xff;
	retbuf[5]=contNr;
	retbuf[6]=0;
	retbuf[7]=pos;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=pos;
	if (contNr>=0x40)
	{
		retbuf[11]=contNr-0x40;
	} else {
		retbuf[11]=0;
	}	

	sendPacket(retbuf);
}

void CPackSender::openContainerFromFloor(int objectId,int x,int y,int z,int targetBag)
{
	char retbuf[256];

	retbuf[0]=10;
	retbuf[1]=0;

	retbuf[2]=0x82;

	retbuf[3]=x&0xff;
	retbuf[4]=(x>>8)&0xff;
	retbuf[5]=y&0xff;
	retbuf[6]=(y>>8)&0xff;
	retbuf[7]=z;	
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=2;	
	retbuf[11]=targetBag;	

	sendPacket(retbuf);
}




void CPackSender::logout()
{
	char sendbuf[3];
	sendbuf[0]=0x01;
	sendbuf[1]=0x00;
	sendbuf[2]=0x14;
	sendPacket(sendbuf);
}

void CPackSender::useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ)
{
	useWithObjectFromContainerOnFloor(sourceObjectId,sourceContNr,sourcePos,targetObjectId,targetX,targetY,targetZ,2);
}

void CPackSender::useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method)
{
	useWithObjectFromContainerOnFloor(sourceObjectId,sourceContNr,sourcePos,targetObjectId,targetX,targetY,targetZ,method,0);
}

void CPackSender::useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method, int extraInfo)
{
	char sendbuf[19];

	sendbuf[0]=17;
	sendbuf[1]=0;
	sendbuf[2]=0x83;

	sendbuf[3]=0xff;
	sendbuf[4]=0xff;
	sendbuf[5]=sourceContNr&0xff;
	sendbuf[6]=(sourceContNr>>8)&0xff;
	sendbuf[7]=sourcePos;
	sendbuf[8]=sourceObjectId&0xff;;
	sendbuf[9]=(sourceObjectId>>8)&0xff;
	sendbuf[10]=sourcePos;

	sendbuf[11]=targetX&0xff;
	sendbuf[12]=(targetX>>8)&0xff;
	sendbuf[13]=targetY&0xff;
	sendbuf[14]=(targetY>>8)&0xff;
	sendbuf[15]=targetZ;
	sendbuf[16]=targetObjectId&0xff;
	sendbuf[17]=(targetObjectId>>8)&0xff;
	sendbuf[18]=extraInfo;

	sendPacket(sendbuf,method);	
}

void CPackSender::stepUp()
{
	char sendbuf[3];

	sendbuf[0]=0x01;
	sendbuf[1]=0x00;
	sendbuf[2]=0x65;

	sendPacket(sendbuf);
}

void CPackSender::stepDown()
{
	char sendbuf[3];

	sendbuf[0]=0x01;
	sendbuf[1]=0x00;
	sendbuf[2]=0x67;

	sendPacket(sendbuf);
}

void CPackSender::stepLeft()
{
	char sendbuf[3];

	sendbuf[0]=0x01;
	sendbuf[1]=0x00;
	sendbuf[2]=0x68;

	sendPacket(sendbuf);
}

void CPackSender::stepRight()
{
	char sendbuf[3];

	sendbuf[0]=0x01;
	sendbuf[1]=0x00;
	sendbuf[2]=0x66;

	sendPacket(sendbuf);
}

void CPackSender::stepMulti(int *direction, int size)
{
	char sendbuf[1000];
	int i;

	sendbuf[0]=size+2;
	sendbuf[1]=0;
	sendbuf[2]=0x64;
	sendbuf[3]=size;
	for (i=0;i<size;i++)
		sendbuf[4+i]=(char)direction[i];
	
	sendPacket(sendbuf);
}

void CPackSender::attack(int tibiaCharId)
{
	char sendbuf[7];
	sendbuf[0]=5;
	sendbuf[1]=0;
	sendbuf[2]=0xa1;
	sendbuf[3]=tibiaCharId&0xff;
	sendbuf[4]=(tibiaCharId>>8)&0xff;
	sendbuf[5]=(tibiaCharId>>16)&0xff;
	sendbuf[6]=(tibiaCharId>>24)&0xff;
	
	sendPacket(sendbuf);
}

//DEL void CPackSender::attackModeFollow()
//DEL {
//DEL 	char sendbuf[6];
//DEL 	sendbuf[0]=4;
//DEL 	sendbuf[1]=0;
//DEL 	sendbuf[2]=0xa0;
//DEL 	sendbuf[3]=1;
//DEL 	sendbuf[4]=1;
//DEL 	sendbuf[5]=1;
//DEL 
//DEL 	sendPacket(sendbuf);
//DEL }


void CPackSender::closeContainer(int contNr)
{
	char sendbuf[4];
	sendbuf[0]=2;
	sendbuf[1]=0;
	sendbuf[2]=0x87;
	sendbuf[3]=contNr;	

	sendPacket(sendbuf);
}

void CPackSender::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId)
{
	castRuneAgainstCreature(contNr,itemPos,runeObjectId,creatureId,2);
}

void CPackSender::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method)
{
	char sendbuf[15];
	sendbuf[0]=13;
	sendbuf[1]=0;
	sendbuf[2]=0x84;

	sendbuf[3]=0xff;
	sendbuf[4]=0xff;
	sendbuf[5]=contNr&0xff;
	sendbuf[6]=(contNr>>8)&0xff;
	sendbuf[7]=itemPos;
	sendbuf[8]=runeObjectId&0xff;;
	sendbuf[9]=(runeObjectId>>8)&0xff;
	sendbuf[10]=itemPos;

	sendbuf[11]=creatureId&0xff;
	sendbuf[12]=(creatureId>>8)&0xff;
	sendbuf[13]=(creatureId>>16)&0xff;
	sendbuf[14]=(creatureId>>24)&0xff;

	sendPacket(sendbuf,method);
}

void CPackSender::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ)
{
	castRuneAgainstHuman(contNr,itemPos,runeObjectId,targetX,targetY,targetZ,2);
}

void CPackSender::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method)
{
	char sendbuf[19];
	//int targetObjectId=CTibiaItem::m_itemTypeRopespot;
	int targetObjectId=0x63;

	sendbuf[0]=17;
	sendbuf[1]=0;
	sendbuf[2]=0x83;

	sendbuf[3]=0xff;
	sendbuf[4]=0xff;
	sendbuf[5]=contNr&0xff;
	sendbuf[6]=(contNr>>8)&0xff;
	sendbuf[7]=itemPos;
	sendbuf[8]=runeObjectId&0xff;;
	sendbuf[9]=(runeObjectId>>8)&0xff;
	sendbuf[10]=itemPos;

	sendbuf[11]=targetX&0xff;
	sendbuf[12]=(targetX>>8)&0xff;
	sendbuf[13]=targetY&0xff;
	sendbuf[14]=(targetY>>8)&0xff;
	sendbuf[15]=targetZ;
	sendbuf[16]=targetObjectId&0xff;
	sendbuf[17]=(targetObjectId>>8)&0xff;
	sendbuf[18]=0x01;

	sendPacket(sendbuf,method);
}

void CPackSender::attackMode(int mode,int follow)
{
	char sendbuf[6];

	sendbuf[0]=4;
	sendbuf[1]=0;
	sendbuf[2]=0xa0;
	sendbuf[3]=mode;
	sendbuf[4]=follow;
	sendbuf[5]=1;

	sendPacket(sendbuf,2);
}

void CPackSender::revealFish(int enable)
{
	char sendbuf[2];
	sendbuf[0]=0;
	sendbuf[1]=0;
	if (enable)
		sendPacket(sendbuf,201); else
		sendPacket(sendbuf,202);
}


void CPackSender::sendTAMessage(char *msg)
{
	struct ipcMessage mess;		
	DWORD cbWritten;

	mess.messageType=3;
	strcpy(mess.payload,msg);	

	
	mess.send();
}

void CPackSender::moveObjectFromFloorToContainer(int objectId, int sourceX, int sourceY, int sourceZ, int targetContNr, int targetPos, int quantity)
{
	char retbuf[256];

	retbuf[0]=15;
	retbuf[1]=0;

	retbuf[2]=0x78;

	retbuf[3]=sourceX&0xff;
	retbuf[4]=(sourceX>>8)&0xff;
	retbuf[5]=sourceY&0xff;
	retbuf[6]=(sourceY>>8)&0xff;
	retbuf[7]=sourceZ;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=0x01;

	retbuf[11]=0xff;
	retbuf[12]=0xff;
	retbuf[13]=targetContNr;
	retbuf[14]=0;
	retbuf[15]=targetPos;

	retbuf[16]=quantity;

	sendPacket(retbuf);
}

void CPackSender::openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag)
{
	char retbuf[256];

	retbuf[0]=10;
	retbuf[1]=0;

	retbuf[2]=0x82;

	retbuf[3]=0xff;
	retbuf[4]=0xff;
	retbuf[5]=contNrFrom;
	retbuf[6]=0;
	retbuf[7]=contPosFrom;	
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=contPosFrom;	
	retbuf[11]=targetBag;	

	sendPacket(retbuf);
}

void CPackSender::moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x, int y, int z,int quantity)
{
	char retbuf[256];

	retbuf[0]=15;
	retbuf[1]=0;

	retbuf[2]=0x78;

	retbuf[3]=0xff;
	retbuf[4]=0xff;
	retbuf[5]=contNr;
	retbuf[6]=0;
	retbuf[7]=pos;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=pos;

	retbuf[11]=x&0xff;
	retbuf[12]=(x>>8)&0xff;
	retbuf[13]=y&0xff;
	retbuf[14]=(y>>8)&0xff;
	retbuf[15]=z;

	retbuf[16]=quantity;

	sendPacket(retbuf);
}

void CPackSender::useItemOnFloor(int objectId, int x, int y, int z)
{
	char retbuf[256];

	retbuf[0]=10;
	retbuf[1]=0;

	retbuf[2]=0x82;

	retbuf[3]=x&0xff;
	retbuf[4]=(x>>8)&0xff;
	retbuf[5]=y&0xff;
	retbuf[6]=(y>>8)&0xff;
	retbuf[7]=z;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=1;	
	retbuf[11]=1;	

	sendPacket(retbuf);
}


void CPackSender::tell(char *msg, char *playerName)
{
	char retbuf[65536];
	int l=strlen(msg)+strlen(playerName)+4+2;
	
	retbuf[0]=l%256;
	retbuf[1]=l/256;
	retbuf[2]=0x96;
	retbuf[3]=0x06;
	retbuf[4]=strlen(playerName)%256;
	retbuf[5]=strlen(playerName)/256;	
	sprintf(retbuf+6,"%s",playerName);	
	retbuf[6+strlen(playerName)]=strlen(msg)%256;
	retbuf[7+strlen(playerName)]=strlen(msg)/256;
	memcpy(retbuf+8+strlen(playerName),msg,strlen(msg));		
	
	sendPacket(retbuf);	
}

void CPackSender::sayNPC(char *buf)
{
	char retbuf[65536];

	retbuf[0]=strlen(buf)+4;
	retbuf[1]=0;
	retbuf[2]=0x96;
	retbuf[3]=0x04;
	retbuf[4]=strlen(buf);
	retbuf[5]=0;
	sprintf(retbuf+6,"%s",buf);

	sendPacket(retbuf);
}

void CPackSender::sayOnChan(char *msg, int channelId1,int channelId2)
{
	char retbuf[65536];
	int l=strlen(msg)+2+2+2;
	
	retbuf[0]=l%256;
	retbuf[1]=l/256;
	retbuf[2]=0x96;
	retbuf[3]=channelId1%256;
	retbuf[4]=channelId2%256;
	retbuf[5]=0;
	retbuf[6]=strlen(msg)%256;
	retbuf[7]=strlen(msg)/256;	
	sprintf(retbuf+8,"%s",msg);			

	sendPacket(retbuf);
}

void CPackSender::npcBuy(int objectId,int qty)
{
	char retbuf[256];

	retbuf[0]=0x5;
	retbuf[1]=0x0;

	retbuf[2]=0x7a;
	retbuf[3]=objectId&0xff;
	retbuf[4]=(objectId>>8)&0xff;
	retbuf[5]=(qty>>8)&0xff;	
	retbuf[6]=0;	

	sendPacket(retbuf);
}

void CPackSender::npcSell(int objectId,int qty)
{
	char retbuf[256];

	retbuf[0]=0x5;
	retbuf[1]=0x0;

	retbuf[2]=0x7b;
	retbuf[3]=objectId&0xff;
	retbuf[4]=(objectId>>8)&0xff;	
	retbuf[5]=(qty>>8)&0xff;	
	retbuf[6]=0;

	sendPacket(retbuf);
}

void CPackSender::turnLeft()
{
	char retbuf[3];

	retbuf[0]=1;
	retbuf[1]=0;

	retbuf[2]=0x72;

	sendPacket(retbuf);

}

void CPackSender::turnRight()
{
	char retbuf[3];

	retbuf[0]=1;
	retbuf[1]=0;

	retbuf[2]=0x70;

	sendPacket(retbuf);
}

void CPackSender::turnUp()
{
	char retbuf[3];

	retbuf[0]=1;
	retbuf[1]=0;

	retbuf[2]=0x6f;

	sendPacket(retbuf);
}

void CPackSender::turnDown()
{
	char retbuf[3];

	retbuf[0]=1	;
	retbuf[1]=0;

	retbuf[2]=0x71;

	sendPacket(retbuf);
}


void CPackSender::sendAttackedCreatureToAutoAim(int attackedCreature)
{
	struct ipcMessage mess;
	mess.messageType=100;
	memcpy(mess.payload,&attackedCreature,sizeof(int));
	mess.send();	
}

void CPackSender::moveObjectFromFloorToFloor(int objectId, int srcX, int srcY, int srcZ, int destX, int destY, int destZ, int quantity)
{
	char retbuf[256];

	retbuf[0]=15;
	retbuf[1]=0;

	retbuf[2]=0x78;

	retbuf[3]=srcX&0xff;
	retbuf[4]=(srcX>>8)&0xff;
	retbuf[5]=srcY&0xff;
	retbuf[6]=(srcY>>8)&0xff;
	retbuf[7]=srcZ;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=0x01;

	retbuf[11]=destX&0xff;
	retbuf[12]=(destX>>8)&0xff;
	retbuf[13]=destY&0xff;
	retbuf[14]=(destY>>8)&0xff;
	retbuf[15]=destZ;

	retbuf[16]=quantity;

	sendPacket(retbuf);
}

void CPackSender::sendCreatureInfo(char *name, char *info1, char *info2)
{
	struct ipcMessage mess;		
	DWORD cbWritten;

	if (strlen(info1)>499) info1[499]='\0';
	if (strlen(info2)>499) info2[499]='\0';

	mess.messageType=301;
	memset(mess.payload,0x0,1024);
	strcpy(mess.payload,name);
	strcpy(mess.payload+32,info1);
	strcpy(mess.payload+500+32,info2);
	
	mess.send();

}

void CPackSender::look(int x, int y, int z, int objectId)
{
	char retbuf[256];

	retbuf[0]=9;
	retbuf[1]=0;

	retbuf[2]=0x8c;
	retbuf[3]=x&0xff;
	retbuf[4]=(x>>8)&0xff;
	retbuf[5]=y&0xff;
	retbuf[6]=(y>>8)&0xff;
	retbuf[7]=z;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=0x01;

	sendPacket(retbuf);
}

void CPackSender::ignoreLook(int end)
{
	struct ipcMessage mess;		
	DWORD cbWritten;

	mess.messageType=302;
	memset(mess.payload,0x0,1024);
	memcpy(mess.payload,&end,4);	
	
	mess.send();
}


void CPackSender::sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle)
{
	struct ipcMessage mess;
	DWORD cbWritten;
	mess.messageType=303;
	memset(mess.payload,0,1024);
	memcpy(mess.payload,&active,4);
	memcpy(mess.payload+4,&onlyCreatures,4);
	memcpy(mess.payload+8,&aimPlayersFromBattle,4);

	mess.send();
}

void CPackSender::sendClearCreatureInfo()
{
	struct ipcMessage mess;
	DWORD cbWritten;
	mess.messageType=304;
	mess.send();
}

void CPackSender::enableCName(int enable)
{
	char sendbuf[2];
	sendbuf[0]=0;
	sendbuf[1]=0;
	if (enable)
		sendPacket(sendbuf,305); else
		sendPacket(sendbuf,306);
}

void CPackSender::stopAll()
{
	char retbuf[3];

	retbuf[0]=1	;
	retbuf[1]=0;

	retbuf[2]=0xbe;

	sendPacket(retbuf);
}
