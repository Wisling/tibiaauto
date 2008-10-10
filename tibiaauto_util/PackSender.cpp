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

	mess.messageType=method;
	memcpy(mess.payload,buf,len);

	
	mess.send();	
}


void CPackSender::moveObjectBetweenContainers(int objectId, int sourceContNr, int sourcePos, int targetContNr, int targetPos, int qty)
{
	char sendbuf[256];

	sendbuf[0]=19;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x78;

	sendbuf[7]=0xff;
	sendbuf[8]=0xff;
	sendbuf[9]=sourceContNr;
	sendbuf[10]=0;
	sendbuf[11]=sourcePos;
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=sourcePos;

	sendbuf[15]=0xff;
	sendbuf[16]=0xff;
	sendbuf[17]=targetContNr;
	sendbuf[18]=0;
	sendbuf[19]=targetPos;

	sendbuf[20]=qty;

	sendPacket(sendbuf);
}


void CPackSender::say(const char *buf)
{
	// CRC requires packets of type 'say XXX' to be trimmed to exact length
	char *sendbuf = new char [strlen(buf)+10];  

	sendbuf[0]=strlen(buf)+8;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x96;
	sendbuf[7]=0x01;
	sendbuf[8]=strlen(buf);
	sendbuf[9]=0;
	sprintf(sendbuf+10,"%s",buf);

	sendPacket(sendbuf);
}

void CPackSender::sayWhisper(const char *buf)
{
	// CRC requires packets of type 'say XXX' to be trimmed to exact length
	char *sendbuf = new char [strlen(buf)+10];

	sendbuf[0]=strlen(buf)+8;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x96;
	sendbuf[7]=0x02;
	sendbuf[8]=strlen(buf);
	sendbuf[9]=0;
	sprintf(sendbuf+10,"%s",buf);

	sendPacket(sendbuf);
}

void CPackSender::sayYell(const char *buf)
{
	// CRC requires packets of type 'say XXX' to be trimmed to exact length
	char *sendbuf = new char [strlen(buf)+10];

	sendbuf[0]=strlen(buf)+8;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x96;
	sendbuf[7]=0x03;
	sendbuf[8]=strlen(buf);
	sendbuf[9]=0;
	sprintf(sendbuf+10,"%s",buf);

	sendPacket(sendbuf);
}

void CPackSender::useItemInContainer(int objectId, int contNr, int pos)
{
	char sendbuf[256];

	sendbuf[0]=13;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x82;
	sendbuf[7]=0xff;
	sendbuf[8]=0xff;
	sendbuf[9]=contNr;
	sendbuf[10]=0;
	sendbuf[11]=pos;
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=pos;
	if (contNr>=0x40)
	{
		sendbuf[9]=contNr-0x40;
	} else {
		sendbuf[9]=0;
	}	

	sendPacket(sendbuf);
}

void CPackSender::openContainerFromFloor(int objectId,int x,int y,int z, int pos, int targetBag)
{
	char sendbuf[16];

	sendbuf[0]=14;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x82;
	sendbuf[7]=x&0xff;
	sendbuf[8]=(x>>8)&0xff;
	sendbuf[9]=y&0xff;
	sendbuf[10]=(y>>8)&0xff;
	sendbuf[11]=z;	
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=pos;	
	sendbuf[15]=targetBag;	

	sendPacket(sendbuf);
}




void CPackSender::logout()
{
	char sendbuf[7];
	sendbuf[0]=0x5;
	sendbuf[1]=0x00;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x14;
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
	char sendbuf[21];

	sendbuf[0]=21;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x83;
	sendbuf[7]=0xff;
	sendbuf[8]=0xff;
	sendbuf[9]=sourceContNr&0xff;
	sendbuf[10]=(sourceContNr>>8)&0xff;
	sendbuf[11]=sourcePos;
	sendbuf[12]=sourceObjectId&0xff;;
	sendbuf[13]=(sourceObjectId>>8)&0xff;
	sendbuf[14]=sourcePos;

	sendbuf[15]=targetX&0xff;
	sendbuf[16]=(targetX>>8)&0xff;
	sendbuf[17]=targetY&0xff;
	sendbuf[18]=(targetY>>8)&0xff;
	sendbuf[19]=targetZ;
	sendbuf[20]=targetObjectId&0xff;
	sendbuf[21]=(targetObjectId>>8)&0xff;
	sendbuf[22]=extraInfo;

	sendPacket(sendbuf,method);	
}

void CPackSender::stepUp()
{
	char sendbuf[7];

	sendbuf[0]=0x5;
	sendbuf[1]=0x00;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x65;

	sendPacket(sendbuf);
}

void CPackSender::stepDown()
{
	char sendbuf[7];

	sendbuf[0]=0x5;
	sendbuf[1]=0x00;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x67;

	sendPacket(sendbuf);
}

void CPackSender::stepLeft()
{
	char sendbuf[7];

	sendbuf[0]=0x5;
	sendbuf[1]=0x00;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x68;

	sendPacket(sendbuf);
}

void CPackSender::stepRight()
{
	char sendbuf[7];

	sendbuf[0]=0x5;
	sendbuf[1]=0x00;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x66;

	sendPacket(sendbuf);
}

void CPackSender::stepMulti(int *direction, int size)
{
	int i;
	int count = 0;
	for (i = 0; i < size; i++) {
		if (direction[i] == 0)
			break;
		count++;
	}
	char *sendbuf = new char[count + 9];

	sendbuf[0]=count+6;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x64;
	sendbuf[7]=count;
	for (i=0;i<count;i++)
		sendbuf[8+i]=(char)direction[i];
char buf[32];
sprintf(buf, "Size: %d\nCount: %d\n", size, count);
AfxMessageBox(buf);
sprintf(buf, "%d %d %d %d", sendbuf[8], sendbuf[9], sendbuf[10], sendbuf[11]);
AfxMessageBox(buf);
	
	sendPacket(sendbuf);
}

void CPackSender::attack(int tibiaCharId)
{
	char sendbuf[11];
	sendbuf[0]=9;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0xa1;
	sendbuf[7]=tibiaCharId&0xff;
	sendbuf[8]=(tibiaCharId>>8)&0xff;
	sendbuf[9]=(tibiaCharId>>16)&0xff;
	sendbuf[10]=(tibiaCharId>>24)&0xff;
	
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
	char sendbuf[8];
	sendbuf[0]=6;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x87;
	sendbuf[7]=contNr;	

	sendPacket(sendbuf);
}

void CPackSender::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId)
{
	castRuneAgainstCreature(contNr,itemPos,runeObjectId,creatureId,2);
}

void CPackSender::castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId, int method)
{
	char sendbuf[19];
	sendbuf[0]=17;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x84;

	sendbuf[7]=0xff;
	sendbuf[8]=0xff;
	sendbuf[9]=contNr&0xff;
	sendbuf[10]=(contNr>>8)&0xff;
	sendbuf[11]=itemPos;
	sendbuf[12]=runeObjectId&0xff;;
	sendbuf[13]=(runeObjectId>>8)&0xff;
	sendbuf[14]=itemPos;

	sendbuf[15]=creatureId&0xff;
	sendbuf[16]=(creatureId>>8)&0xff;
	sendbuf[17]=(creatureId>>16)&0xff;
	sendbuf[18]=(creatureId>>24)&0xff;

	sendPacket(sendbuf,method);
}

void CPackSender::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ)
{
	castRuneAgainstHuman(contNr,itemPos,runeObjectId,targetX,targetY,targetZ,2);
}

void CPackSender::castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ, int method)
{
	char sendbuf[23];
	//int targetObjectId=CTibiaItem::m_itemTypeRopespot;
	int targetObjectId=0x63;

	sendbuf[0]=21;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x83;

	sendbuf[7]=0xff;
	sendbuf[8]=0xff;
	sendbuf[9]=contNr&0xff;
	sendbuf[10]=(contNr>>8)&0xff;
	sendbuf[11]=itemPos;
	sendbuf[12]=runeObjectId&0xff;;
	sendbuf[13]=(runeObjectId>>8)&0xff;
	sendbuf[14]=itemPos;

	sendbuf[15]=targetX&0xff;
	sendbuf[16]=(targetX>>8)&0xff;
	sendbuf[17]=targetY&0xff;
	sendbuf[18]=(targetY>>8)&0xff;
	sendbuf[19]=targetZ;
	sendbuf[20]=targetObjectId&0xff;
	sendbuf[21]=(targetObjectId>>8)&0xff;
	sendbuf[22]=0x01;

	sendPacket(sendbuf,method);
}

void CPackSender::attackMode(int mode,int follow)
{
	char sendbuf[10];

	sendbuf[0]=8;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0xa0;
	sendbuf[7]=mode;
	sendbuf[8]=follow;
	sendbuf[9]=1;

	sendPacket(sendbuf,2);
}

void CPackSender::revealFish(int enable)
{
	char sendbuf[6];
	sendbuf[0]=4;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

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

void CPackSender::moveObjectFromFloorToContainer(int objectId, int sourceX, int sourceY, int sourceZ, int targetContNr, int targetPos, int quantity)
{
	char sendbuf[256];

	sendbuf[0]=19;
	sendbuf[1]=0;


	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x78;
	sendbuf[7]=sourceX&0xff;
	sendbuf[8]=(sourceX>>8)&0xff;
	sendbuf[9]=sourceY&0xff;
	sendbuf[10]=(sourceY>>8)&0xff;
	sendbuf[11]=sourceZ;
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=0x01;

	sendbuf[15]=0xff;
	sendbuf[16]=0xff;
	sendbuf[17]=targetContNr;
	sendbuf[18]=0;
	sendbuf[19]=targetPos;

	sendbuf[20]=quantity;

	sendPacket(sendbuf);
}

void CPackSender::openContainerFromContainer(int objectId, int contNrFrom, int contPosFrom, int targetBag)
{
	char sendbuf[256];

	sendbuf[0]=14;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x82;
	sendbuf[7]=0xff;
	sendbuf[8]=0xff;
	sendbuf[9]=contNrFrom;
	sendbuf[10]=0;
	sendbuf[11]=contPosFrom;	
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=contPosFrom;	
	sendbuf[15]=targetBag;	

	sendPacket(sendbuf);
}

void CPackSender::moveObjectFromContainerToFloor(int objectId, int contNr, int pos, int x, int y, int z,int quantity)
{
	char sendbuf[256];

	sendbuf[0]=19;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte

	sendbuf[6]=0x78;
	sendbuf[7]=0xff;
	sendbuf[8]=0xff;
	sendbuf[9]=contNr;
	sendbuf[10]=0;
	sendbuf[11]=pos;
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=pos;

	sendbuf[15]=x&0xff;
	sendbuf[16]=(x>>8)&0xff;
	sendbuf[17]=y&0xff;
	sendbuf[18]=(y>>8)&0xff;
	sendbuf[19]=z;

	sendbuf[20]=quantity;

	sendPacket(sendbuf);
}

void CPackSender::useItemOnFloor(int objectId, int x, int y, int z)
{
	char sendbuf[256];

	sendbuf[0]=14;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x82;
	sendbuf[7]=x&0xff;
	sendbuf[8]=(x>>8)&0xff;
	sendbuf[9]=y&0xff;
	sendbuf[10]=(y>>8)&0xff;
	sendbuf[11]=z;
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=1;	
	sendbuf[15]=1;	

	sendPacket(sendbuf);
}


void CPackSender::tell(char *msg, char *playerName)
{
	// CRC requires packets of type 'say XXX' to be trimmed to exact length
	char *sendbuf = new char [strlen(msg)+strlen(playerName)+13];
	int l=strlen(msg)+strlen(playerName)+6+2;
	
	sendbuf[0]=l%256;
	sendbuf[1]=l/256;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x96;
	sendbuf[7]=0x06;
	sendbuf[8]=strlen(playerName)%256;
	sendbuf[9]=strlen(playerName)/256;	
	sprintf(sendbuf+10,"%s",playerName);	
	sendbuf[10+strlen(playerName)]=strlen(msg)%256;
	sendbuf[11+strlen(playerName)]=strlen(msg)/256;
	memcpy(sendbuf+12+strlen(playerName),msg,strlen(msg));		
	
	sendPacket(sendbuf);	
}

void CPackSender::sayNPC(char *buf)
{
	// CRC requires packets of type 'say XXX' to be trimmed to exact length
	char *sendbuf = new char [strlen(buf)+10];

	sendbuf[0]=strlen(buf)+8;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x96;
	sendbuf[7]=0x04;
	sendbuf[8]=strlen(buf);
	sendbuf[9]=0;
	sprintf(sendbuf+10,"%s",buf);

	sendPacket(sendbuf);
}

void CPackSender::sayOnChan(char *msg, int channelId1,int channelId2)
{
	// CRC requires packets of type 'say XXX' to be trimmed to exact length
	char *sendbuf = new char [strlen(msg)+12];
	int l=strlen(msg)+10;
	
	sendbuf[0]=l%256;
	sendbuf[1]=l/256;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x96;
	sendbuf[7]=channelId1%256;
	sendbuf[8]=channelId2%256;
	sendbuf[9]=0;
	sendbuf[10]=strlen(msg)%256;
	sendbuf[11]=strlen(msg)/256;	
	sprintf(sendbuf+12,"%s",msg);			

	sendPacket(sendbuf);
}

void CPackSender::npcBuy(int objectId,int qty)
{
	char sendbuf[256];

	sendbuf[0]=0x9;
	sendbuf[1]=0x0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x7a;
	sendbuf[7]=objectId&0xff;
	sendbuf[8]=(objectId>>8)&0xff;
	sendbuf[9]=0;	
	sendbuf[10]=qty;	

	sendPacket(sendbuf);
}

void CPackSender::npcSell(int objectId,int qty)
{
	char sendbuf[256];

	sendbuf[0]=0x9;
	sendbuf[1]=0x0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x7b;
	sendbuf[7]=objectId&0xff;
	sendbuf[8]=(objectId>>8)&0xff;	
	sendbuf[9]=0;
	sendbuf[10]=qty;

	sendPacket(sendbuf);
}

void CPackSender::turnLeft()
{
	char sendbuf[7];

	sendbuf[0]=5;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x72;

	sendPacket(sendbuf);

}

void CPackSender::turnRight()
{
	char sendbuf[7];

	sendbuf[0]=5;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[2]=0x70;

	sendPacket(sendbuf);
}

void CPackSender::turnUp()
{
	char sendbuf[7];

	sendbuf[0]=5;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x6f;

	sendPacket(sendbuf);
}

void CPackSender::turnDown()
{
	char sendbuf[7];

	sendbuf[0]=5	;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x71;

	sendPacket(sendbuf);
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
	char sendbuf[256];

	sendbuf[0]=19;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x78;
	sendbuf[7]=srcX&0xff;
	sendbuf[8]=(srcX>>8)&0xff;
	sendbuf[9]=srcY&0xff;
	sendbuf[10]=(srcY>>8)&0xff;
	sendbuf[11]=srcZ;
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=0x01;

	sendbuf[15]=destX&0xff;
	sendbuf[16]=(destX>>8)&0xff;
	sendbuf[17]=destY&0xff;
	sendbuf[18]=(destY>>8)&0xff;
	sendbuf[19]=destZ;

	sendbuf[20]=quantity;

	sendPacket(sendbuf);
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

void CPackSender::look(int x, int y, int z, int objectId)
{
	char sendbuf[256];

	sendbuf[0]=13;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0x8c;
	sendbuf[7]=x&0xff;
	sendbuf[8]=(x>>8)&0xff;
	sendbuf[9]=y&0xff;
	sendbuf[10]=(y>>8)&0xff;
	sendbuf[11]=z;
	sendbuf[12]=objectId&0xff;
	sendbuf[13]=(objectId>>8)&0xff;	
	sendbuf[14]=0x01;

	sendPacket(sendbuf);
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
	char sendbuf[6];
	sendbuf[0]=4;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	if (enable)
		sendPacket(sendbuf,305); else
		sendPacket(sendbuf,306);
}

void CPackSender::stopAll()
{
	char sendbuf[7];

	sendbuf[0]=5	;
	sendbuf[1]=0;

	sendbuf[2]=0;  //CRC byte (to be filled later in program execution)
	sendbuf[3]=0;  //CRC byte
	sendbuf[4]=0;  //CRC byte
	sendbuf[5]=0;  //CRC byte

	sendbuf[6]=0xbe;

	sendPacket(sendbuf);
}
