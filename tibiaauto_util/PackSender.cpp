// PackSender.cpp: implementation of the CPackSender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PackSender.h"
#include "MemReader.h"
#include "MemUtil.h"
#include "Util.h"
#include "commons2.h"
#include <stdio.h>

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

	mess.messageType=method;
	memcpy(mess.payload,buf,len);

	
	mess.send();
}

#pragma warning (push)
#pragma warning(disable:4305 )
#pragma warning(disable:4309 )
void CPackSender::say(const char *buf)
{
	char retbuf[65536];

	retbuf[0]=strlen(buf)+4;
	retbuf[1]=(strlen(buf)+4)>>8;
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
	retbuf[1]=(strlen(buf)+4)>>8;
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
	retbuf[1]=(strlen(buf)+4)>>8;
	retbuf[2]=0x96;
	retbuf[3]=0x03;
	retbuf[4]=strlen(buf);
	retbuf[5]=0;
	sprintf(retbuf+6,"%s",buf);

	sendPacket(retbuf);
}

void CPackSender::useItem(int objectId)
{
	char retbuf[256];

	retbuf[0]=10;
	retbuf[1]=0;

	retbuf[2]=0x82;

	retbuf[3]=0xff;
	retbuf[4]=0xff;
	retbuf[5]=0;
	retbuf[6]=0;
	retbuf[7]=0;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=0;
	retbuf[11]=0;

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
	retbuf[11]=findNextClosedContainer();

	sendPacket(retbuf);
}
int CPackSender::openAutoContainerFromFloor(int objectId,int x,int y,int z)
{
	int targetBag=findNextClosedContainer();
	openContainerFromFloor(objectId,x,y,z,targetBag);
	return targetBag;

}
void CPackSender::openContainerFromFloor(int objectId,int x,int y,int z, int targetBag)
{
	char retbuf[256];
	int targetInd=getItemIndex(x,y,objectId);

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
	retbuf[10]=max(0,targetInd);	
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

void CPackSender::useWithObjectOnFloor(int sourceObjectId,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	useWithObjectSend(sourceObjectId,0xffff,0,0,targetObjectId,targetX,targetY,targetZ,method);
}

void CPackSender::useWithObjectInContainer(int sourceObjectId,int targetObjectId,int contNr,int itemPos, int method/*=2*/)
{
	useWithObjectSend(sourceObjectId,0xffff,0,0,targetObjectId,0xffff,contNr,itemPos,method);
}

void CPackSender::useWithObjectFromFloorOnFloor(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	useWithObjectSend(sourceObjectId,sourceX,sourceY,sourceZ,targetObjectId,targetX,targetY,targetZ,method);
}

void CPackSender::useWithObjectFromFloorInContainer(int sourceObjectId,int sourceX,int sourceY,int sourceZ,int targetObjectId,int targetContNr,int targetPos, int method/*=2*/)
{
	useWithObjectSend(sourceObjectId,sourceX,sourceY,sourceZ,targetObjectId,0xffff,targetContNr,targetPos,method);
}

void CPackSender::useWithObjectFromContainerInContainer(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetContNr,int targetPos, int method/*=2*/)
{
	useWithObjectSend(sourceObjectId,0xffff,sourceContNr,sourcePos,targetObjectId,0xffff,targetContNr,targetPos,method);
}

void CPackSender::useWithObjectFromContainerOnFloor(int sourceObjectId,int sourceContNr,int sourcePos,int targetObjectId,int targetX,int targetY,int targetZ, int method/*=2*/)
{
	useWithObjectSend(sourceObjectId,0xffff,sourceContNr,sourcePos,targetObjectId,targetX,targetY,targetZ,method);
}

void CPackSender::useWithObjectSend(int sourceObjectId,int sourceX,int sourceY_Cont,int sourceZ_Pos,int targetObjectId,int targetX,int targetY_Cont,int targetZ_Pos, int method/*=2*/)
{
	char sendbuf[19];

	int sourceInd_Pos=sourceZ_Pos;
	int targetInd_Pos=targetZ_Pos;

	//if using a floor location then use Ind_Pos=getItemIndex(x,y,z)
	if (sourceX!=0xffff || targetX!=0xffff){
		CMemReader reader;
		CTibiaCharacter *self = reader.readSelfCharacter();
		if (sourceX!=0xffff) sourceInd_Pos=getItemIndex(sourceX-self->x,sourceY_Cont-self->y,sourceObjectId);
		if (targetX!=0xffff) targetInd_Pos=getItemIndex(targetX-self->x,targetY_Cont-self->y,targetObjectId);
		delete self;
	}

	sendbuf[0]=17;
	sendbuf[1]=0;
	sendbuf[2]=0x83;

	sendbuf[3]=sourceX&0xff;
	sendbuf[4]=(sourceX>>8)&0xff;
	sendbuf[5]=sourceY_Cont&0xff;
	sendbuf[6]=(sourceY_Cont>>8)&0xff;
	sendbuf[7]=sourceZ_Pos;
	sendbuf[8]=sourceObjectId&0xff;;
	sendbuf[9]=(sourceObjectId>>8)&0xff;
	sendbuf[10]=max(0,sourceInd_Pos);

	sendbuf[11]=targetX&0xff;
	sendbuf[12]=(targetX>>8)&0xff;
	sendbuf[13]=targetY_Cont&0xff;
	sendbuf[14]=(targetY_Cont>>8)&0xff;
	sendbuf[15]=targetZ_Pos;
	sendbuf[16]=targetObjectId&0xff;
	sendbuf[17]=(targetObjectId>>8)&0xff;
	sendbuf[18]=max(0,targetInd_Pos);

	sendPacket(sendbuf,method);	
}

void CPackSender::stepRight()
{
	int i[1]={1};
	stepMulti(i,1);
}
void CPackSender::stepUpRight()
{
	int i[1]={2};
	stepMulti(i,1);
}
void CPackSender::stepUp()
{
	int i[1]={3};
	stepMulti(i,1);
}
void CPackSender::stepUpLeft()
{
	int i[1]={4};
	stepMulti(i,1);
}
void CPackSender::stepLeft()
{
	int i[1]={5};
	stepMulti(i,1);
}
void CPackSender::stepDownLeft()
{
	int i[1]={6};
	stepMulti(i,1);
}
void CPackSender::stepDown()
{
	int i[1]={7};
	stepMulti(i,1);
}
void CPackSender::stepDownRight()
{
	int i[1]={8};
	stepMulti(i,1);
}



// Tibia client sends single steps instead of multisteps of distance 1
void CPackSender::stepMulti(int *direction, int size)
{
	CMemReader reader;
	int i;

	//Manage Tibia's memory
	int pathIndAddr=reader.m_memAddressCurrentTileToGo;
	int pathLenAddr=reader.m_memAddressTilesToGo;
	int pathStartAddr=pathLenAddr+4;
	CMemUtil::SetMemIntValue(pathIndAddr,0);
	CMemUtil::SetMemIntValue(pathLenAddr,size);
	for (i=0;i<size && i<10;i++){
		CMemUtil::SetMemIntValue(pathStartAddr+4*i,direction[i]);
	}
	CMemUtil::SetMemIntValue(reader.m_memAddressFirstContainer+reader.m_memLengthCreature*reader.getLoggedCharNr()+76,1);
	
	char sendbuf[1000];

	if (size==1){
		sendbuf[0]=1;
		sendbuf[1]=0;
		switch(direction[0]){
		case 3: sendbuf[2]=0x65; break;
		case 1: sendbuf[2]=0x66; break;
		case 7: sendbuf[2]=0x67; break;
		case 5: sendbuf[2]=0x68; break;
		case 2: sendbuf[2]=0x6A; break;
		case 8: sendbuf[2]=0x6B; break;
		case 6: sendbuf[2]=0x6C; break;
		case 4: sendbuf[2]=0x6D; break;
		}
	} else {
		sendbuf[0]=size+2;
		sendbuf[1]=0;
		sendbuf[2]=0x64;
		sendbuf[3]=size;
		for (i=0;i<size;i++)
			sendbuf[4+i]=(char)direction[i];
	}
	sendPacket(sendbuf);
}

void CPackSender::attack(int tibiaCharId)
{
	CMemReader reader;
	reader.setAttackedCreature(tibiaCharId);

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

void CPackSender::follow(int tibiaCharId)
{
	CMemReader reader;
	reader.setFollowedCreature(tibiaCharId);

	char sendbuf[7];
	sendbuf[0]=5;
	sendbuf[1]=0;
	sendbuf[2]=0xa2;
	sendbuf[3]=tibiaCharId&0xff;
	sendbuf[4]=(tibiaCharId>>8)&0xff;
	sendbuf[5]=(tibiaCharId>>16)&0xff;
	sendbuf[6]=(tibiaCharId>>24)&0xff;

	sendPacket(sendbuf);
}


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

void CPackSender::useItemOnCreature(int objectId, int creatureId)
{
	useItemOnCreatureSend(objectId,0xffff,0,0,creatureId);
}

void CPackSender::useItemFromContainerOnCreature(int objectId, int contNr, int itemPos, int creatureId)
{
	useItemOnCreatureSend(objectId,0xffff,contNr,itemPos,creatureId);
}

void CPackSender::useItemFromFloorOnCreature(int objectId, int x, int y, int z, int creatureId)
{
	useItemOnCreatureSend(objectId,x,y,z,creatureId);
}

void CPackSender::useItemOnCreatureSend(int objectId,int x,int y_Cont,int z_Pos, int creatureId)
{
	char retbuf[256];
	int targetInd_Pos=((x&0xffff)==x)?z_Pos:getItemIndex(x,y_Cont,objectId);//Decide if using floor or bag

	retbuf[0]=13;
	retbuf[1]=0;

	retbuf[2]=0x84;

	retbuf[3]=x&0xff;
	retbuf[4]=(x>>8)&0xff;
	retbuf[5]=y_Cont&0xff;
	retbuf[6]=(y_Cont>>8)&0xff;
	retbuf[7]=z_Pos;
	retbuf[8]=objectId&0xff;
	retbuf[9]=(objectId>>8)&0xff;	
	retbuf[10]=max(0,targetInd_Pos);	
	retbuf[11]=creatureId&0xff;
	retbuf[12]=(creatureId>>8)&0xff;
	retbuf[13]=(creatureId>>16)&0xff;
	retbuf[14]=(creatureId>>24)&0xff;

	sendPacket(retbuf);
}


void CPackSender::attackMode(int attack,int follow,int attLock)
{
	char sendbuf[6];

	if(attack!=2 && attack!=1 && attack!=3){ sendTAMessage("Packet error attack"); return;}
	if(follow!=0 && follow!=1){ sendTAMessage("Packet error follow"); return;}
	if(attLock!=0 && attLock!=1){ sendTAMessage("Packet error attklock"); return;}

	sendbuf[0]=4;
	sendbuf[1]=0;
	sendbuf[2]=0xa0;
	sendbuf[3]=attack;
	sendbuf[4]=follow;
	sendbuf[5]=attLock;

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

	mess.messageType=3;
	strcpy(mess.payload,msg);
	
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
	retbuf[10]=max(0,getItemIndex(srcX,srcY,objectId));

	retbuf[11]=destX&0xff;
	retbuf[12]=(destX>>8)&0xff;
	retbuf[13]=destY&0xff;
	retbuf[14]=(destY>>8)&0xff;
	retbuf[15]=destZ;

	retbuf[16]=quantity;

	sendPacket(retbuf);
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
	retbuf[10]=max(0,getItemIndex(sourceX,sourceY,objectId));

	retbuf[11]=0xff;
	retbuf[12]=0xff;
	retbuf[13]=targetContNr;
	retbuf[14]=0;
	retbuf[15]=targetPos;

	retbuf[16]=quantity;

	sendPacket(retbuf);
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

int CPackSender::openAutoContainerFromContainer(int objectId, int contNrFrom, int contPosFrom)
{
	int targetBag=findNextClosedContainer();
	openContainerFromContainer(objectId, contNrFrom, contPosFrom,targetBag);
	return targetBag;

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
	retbuf[10]=max(0,getItemIndex(x,y,objectId));
	retbuf[11]=findNextClosedContainer();

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
	retbuf[1]=(strlen(buf)+4)>>8;
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

void CPackSender::npcBuy(int objectId,int qty, int ignoreCap, int withBackpack)
{
	char retbuf[256];

	retbuf[0]=0x7;
	retbuf[1]=0x0;

	retbuf[2]=0x7a;
	retbuf[3]=objectId&0xff;
	retbuf[4]=(objectId>>8)&0xff;
	retbuf[5]=0;	
	retbuf[6]=qty;	
	retbuf[7]=ignoreCap ? 1 : 0; //ignore capacity
	retbuf[8]=withBackpack ? 1 : 0; //buy with backpack	

	sendPacket(retbuf);
}

void CPackSender::npcSell(int objectId,int qty)
{
	char retbuf[256];

	retbuf[0]=0x6;
	retbuf[1]=0x0;

	retbuf[2]=0x7b;
	retbuf[3]=objectId&0xff;
	retbuf[4]=(objectId>>8)&0xff;	
	retbuf[5]=0;	
	retbuf[6]=qty;
	retbuf[7]=1;

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

void CPackSender::sendCreatureInfo(char *name, char *info1, char *info2)
{
	struct ipcMessage mess;		

	if (strlen(info1)>499) info1[499]='\0';
	if (strlen(info2)>499) info2[499]='\0';

	mess.messageType=301;
	memset(mess.payload,0x0,1024);
	strcpy(mess.payload,name);
	strcpy(mess.payload+32,info1);
	strcpy(mess.payload+500+32,info2);
	
	mess.send();

}

void CPackSender::printText(CPoint pos, int red, int green, int blue, char* message)
{
	struct ipcMessage mess;		
	int messLen = strlen(message);
	mess.messageType=307;
	memset(mess.payload,0x0,1024);
	memcpy(mess.payload, &pos.x, sizeof(int));
	memcpy(mess.payload + 4, &pos.y, sizeof(int));
	memcpy(mess.payload + 8, &red, sizeof(int));
	memcpy(mess.payload + 12, &green, sizeof(int));
	memcpy(mess.payload + 16, &green, sizeof(int));
	memcpy(mess.payload + 20, &messLen, sizeof(int));
	strcpy(mess.payload + 24, message);
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
	retbuf[10]=max(0,getItemIndex(x,y,objectId));

	sendPacket(retbuf);
}

void CPackSender::ignoreLook(int end)
{
	struct ipcMessage mess;		

	mess.messageType=302;
	memset(mess.payload,0x0,1024);
	memcpy(mess.payload,&end,4);
	
	mess.send();
}


void CPackSender::sendAutoAimConfig(int active, int onlyCreatures, int aimPlayersFromBattle)
{
	struct ipcMessage mess;
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

#pragma warning (pop)

void CPackSender::sendDirectPacket(const char* buf,int len)
{
	char retbuf[16384];

	retbuf[0]=len%0xFF;
	retbuf[1]=(len>>8)%0xFF;
	memcpy(retbuf+2,buf,min(len,16384-2));

	sendPacket(retbuf);
}
