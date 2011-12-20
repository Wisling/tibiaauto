// tibiaautoinject2.cpp : Defines the entry point for the DLL application.
//
// Check MyPlayerNameText funcs

#include "stdafx.h"

#include "tibiaautoinject2.h"
#include "stdio.h"
#include "detours.h"
#include "winsock.h"
#include "time.h"
#include "protocol.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "Tlhelp32.h"
#include "MyMenu.h"
#include "resource.h"
#include "Windows.h"
#include "regex.h"
#include "RegexpProxy.h"

int myInterceptEncrypt(int v1, int v2);
int myInterceptDecrypt(int v1, int v2);
int OUTmyInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
int myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
int myShouldParseRecv();


#define TA_MESSAGE_QLEN 10
#define MAX_CREATUREINFO 1300
#define MAX_PAYLOAD_LEN 1024

#ifndef NDEBUG
const bool OUTPUT_DEBUG_INFO=1;
#else
const bool OUTPUT_DEBUG_INFO=0;
#endif

int tmp1=-1;
char tmp2[256];
char tmp3[256];
int revealFish=0;

char creatureInfoPlayerName[MAX_CREATUREINFO][32];
char creatureInfoPlayerInfo1[MAX_CREATUREINFO][500];
char creatureInfoPlayerInfo2[MAX_CREATUREINFO][500];
int creatureInfoNext=0;

char taMessage[TA_MESSAGE_QLEN][32000];
int taMessageStart=0;
int taMessageEnd=0;


struct actionRegexData{
	int handle;
	int inUse;
	regex_t preg;
	actionRegexData(){
		inUse = 0;
	}
};

const RECV_REGEX_MAX = 500;
actionRegexData recvRegex[RECV_REGEX_MAX];
int recvRegexCount = 0;


int partnerProcessId=0;
int ignoreLookEnd=0;

int autoAimActive=0;
int autoAimOnlyCreatures=0;
int autoAimAimPlayersFromBattle=0;

int revealCNameActive=0;

char lastConnectName[16];

HANDLE hPipe=INVALID_HANDLE_VALUE;
HANDLE hPipeBack=INVALID_HANDLE_VALUE;

HHOOK hook;

SOCKET tibiaSocket=NULL;
FILE *debugFile=NULL;

int COMPLEX=0;
int SENTONLY=1;

time_t debugFileStart;
int lastSendFlags;

// the encryption buffer (to avoid decryption on send)
char encryptBeforeBuf[5000];
char encryptAfterBuf[5000];
int encryptLen;
int encryptPos;
int encryptPrevPtr=0;
int encryptKeyPtr=0;

int privChanBufferPtr=0;

struct packStream {
	char* s;
	int length;
	int pos;
};

struct HUD {
	CPoint pos;
	int redColor;
	int blueColor;
	int greenColor;
	char message[1024];
public:
	HUD() {
		CPoint pos;
		redColor = 0;
		blueColor = 0;
		greenColor = 0;
		message[0] = '\0';
	}
};
HUD HUDisplay[100];

struct tibiaState
{
	int attackedCreature;
	char outbufHmm[100];
	char outbufGfb[100];
	char outbufSd[100];
	char outbufExplo[100];
	char outbufSelfUH[100];
	char outbufFluidMana[100];
	char outbufFluidLife[100];	
}tibiaState;

/*
1. Hi can I have and handle to read from for this regex"%&*%" ?
2. Here is a handle
3. Here is info to read
*/
struct ipcMessage
{
	int messageType;
	char payload[MAX_PAYLOAD_LEN];
	UINT tm;
public:
	ipcMessage()
	{
		messageType = 0;
		memset(payload,0,1024);
		tm = 0;
	}
	void send()
	{
		this->tm = time(NULL);
		DWORD cbWritten;
		BOOL fSuccess = WriteFile(
			hPipeBack,
			this,
			sizeof(struct ipcMessage),
			&cbWritten,
			NULL);
	}
};

/** 
* codes for communication
* 1001: incoming message -> Auto Rsponder
* 1002: middle screen info (you see) -> creature info
* 1003: incoming message -> Autogo message alarm
* 1004: incoming message -> Self info haste/ms timer
* 1005: incoming message -> Mesage commands for unfinished TEAM module
* 1006: incoming message -> python engine
* 1007: %ta messages -> python engine
* 1008: %ta lu/%ta ld -> xray
* 1009: %ta pause -> cavebot pausing
* 1010: matching action found message -> if a recieved packet contains a matching action

* 2001: hooks -> xray
* 2002: hooks -> cavebot pausing
*/

int outExploAvail=0;
int outHmmAvail=0;
int outSdAvail=0;
int outGfbAvail=0;
int outSelfUHAvail=0;
int outFluidManaAvail=0;
int outFluidLifeAvail=0;

DETOUR_TRAMPOLINE(int WINAPI Real_send(SOCKET s,char* buf,int len,int flags),send);
DETOUR_TRAMPOLINE(int WINAPI Real_recv(SOCKET s,char* buf,int len,int flags),recv);
DETOUR_TRAMPOLINE(int WINAPI Real_connect(SOCKET s,const struct sockaddr* name,int namelen),connect);
DETOUR_TRAMPOLINE(SOCKET WINAPI Real_socket(int af,int type,int protocol),socket);
DETOUR_TRAMPOLINE(int Real_select(int nfds,fd_set* readfds,fd_set* writefds,fd_set* exceptfds,const struct timeval* timeout),select);




int WINAPI Mine_send(SOCKET s,char* buf,int len,int flags);
char*  adler(char *data, size_t len);

void InitialiseCommunication();
void InitialiseHooks();
void InitialiseDebugFile();

#define STRBUFLEN 655360
char bufToHexStringRet[STRBUFLEN];

int payloadLen(unsigned char *buf)
{
	int lenL=buf[0];
	int lenH=buf[1];
	int len=buf[0]+buf[1]*256;
	return len;
}

int payloadLen(char buf[])
{
	int lenL=buf[0];
	int lenH=buf[1];
	int len=buf[0]+buf[1]*256;
	return len;
}

void bufToHexString(char *buf,int len)
{	
	if (len>STRBUFLEN/8)
	{
		sprintf(bufToHexStringRet,"[buffer length to big: %d]",len);
		return;
	}
	memset(bufToHexStringRet,0x00,STRBUFLEN);
	for (int pos=0;pos<len;pos++)
	{
		char localBuf[256];
		int val=buf[pos];
		if (val<0) val+=256;
		sprintf(localBuf," %02X",val);
		strcat(bufToHexStringRet,localBuf);
	};
}

#define MOD_ADLER 65521
char outCheck[5]; 
char*  adler(char *data, size_t len) /* data: Pointer to the data to be summed; len is in bytes */
{
	/*
	The is the CRC algorithim. I could not nor would I try to find the actual one Tibia 
	uses. So I have copied this of and modified it to fit the data we use.
	*/
	int count = 0;
    int a = 1;
	int b = 0;
	
    while (len > 0) 
    {
        size_t tlen = len > 5552 ? 5552 : len;
        len -= tlen;
        do 
        {
            a += (int)data[count]>=0?(int)data[count]:256+(int)data[count];
            b += a;
			count++;
        } while (--tlen);
		
        a %= MOD_ADLER;
        b %= MOD_ADLER;
    }
	outCheck[0] = a&0xff;
	outCheck[1] = (a>>8)&0xff;
	outCheck[2] = b&0xff;
	outCheck[3] = (b>>8)&0xff;
	outCheck[4] = '\0';
	
	return outCheck;
	
}

int aaa = 0;
// direction 0 - incoming (recv); 1 outgoing (send)
void parseMessage(char *buf,int realRecvLen,FILE *debugFile, int direction, int depth)
{
	// 45D565, 4F9520 hook for calculating everything
	// stack overflow protecion
	if (depth>20)
	{
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"!!! stack overflow protection run\r\n");
		}
		return;
	}
	
	int packetSizeH=buf[1];
	int packetSizeL=buf[0];
	if (packetSizeH<0)
		packetSizeH+=256;
	if (packetSizeL<0)
		packetSizeL+=256;
	int packetSize=packetSizeH*256+packetSizeL+2;
	
	
	if (packetSize>realRecvLen)
	{	
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"!!! underrun\r\n");
		}
		return;
	} 	
	
	
	int code=buf[2];
	if (code<0)
		code+=256;
	
	
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"### got bytes = %d; packet size = %d; code=0x%x\r\n",realRecvLen,packetSize,code);
	}
	
	if (direction==0)
	{
		// this is 'recv' direction: decrypt and parse message
		if (encryptKeyPtr)
		{
			
			int afterDecryptCode;
			static char decryptedBuf[100000];
			int i;
			
			memcpy(decryptedBuf,buf+6,packetSize-6); // Remember CRC bytes are NOT encrypted either sending or recieving.
			//fprintf(debugFile,"# decrypted content follows #\r\n");
			
			for (i=0;i<packetSize-6;i+=8)	
			{
				myInterceptDecrypt((int)(decryptedBuf+i),encryptKeyPtr);
			}
			afterDecryptCode=decryptedBuf[2];
			if (afterDecryptCode<0) afterDecryptCode+=256;
			int afterDecryptLenH=decryptedBuf[1];
			int afterDecryptLenL=decryptedBuf[0];
			if (afterDecryptLenH<0)
				afterDecryptLenH+=256;
			if (afterDecryptLenL<0)
				afterDecryptLenL+=256;
			int afterDecryptLen=afterDecryptLenH*256+afterDecryptLenL+2;

			if (debugFile&&!SENTONLY){
				fprintf(debugFile,"$$$ len=%d code = 0x%x\r\n",afterDecryptLen,afterDecryptCode);
				bufToHexString(decryptedBuf,afterDecryptLen);
				fprintf(debugFile,"<- [%x] %s\r\n",socket,bufToHexStringRet);					
				fflush(debugFile);	
			}

				
		}	
	}
	
	if (packetSize<realRecvLen)
	{		
		parseMessage(buf+packetSize,realRecvLen-packetSize,debugFile,direction,depth+1);
	} // tail recursion
	
};

int lastAction=0;
void sendBufferViaSocket(char *buffer)
{	
	// if we don't yet have key pointer then don't do anything
	if (!encryptKeyPtr)
		return;
	
	int i;
	char outbufHeader[1006];
	char* outbuf = outbufHeader+6;
	int lowB=buffer[0];
	int hiB=buffer[1];
	if (lowB<0) lowB+=256;
	if (hiB<0) hiB+=256;
	int len=lowB+hiB*256+2;

	int outbuflen=len;
	if (len%8!=0) outbuflen+=8-(len%8); // packet length is now 8-btye encryptions + 4 CRC bytes + 2 byes packet header
	outbufHeader[0]=outbuflen%256;
	outbufHeader[1]=outbuflen/256;

	//before encryption
	if (debugFile && COMPLEX) {
		bufToHexString(buffer,len);	
		fprintf(debugFile,"Before Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		fprintf(debugFile,"-> [%x] %s\r\n",socket,bufToHexStringRet);
		fprintf(debugFile, "outbuflen = %d\r\n", outbuflen);
		fflush(debugFile);	
	}

	for (i=0;i<outbuflen;i+=8)
	{		
		memcpy(outbuf+i,buffer+i,8);
		myInterceptEncrypt((int)(outbuf+i),encryptKeyPtr);		
		memcpy(outbuf+i,buffer+i,8); 		
		myInterceptEncrypt((int)(outbuf+i),encryptKeyPtr);		
		memcpy(outbuf+i,buffer+i,8); 		
		myInterceptEncrypt((int)(outbuf+i),encryptKeyPtr);		
	}	
	//after encryption
	if (debugFile && COMPLEX) {
		fprintf(debugFile,"After Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		bufToHexString(outbuf, outbuflen);	
		fprintf(debugFile,"-> [%x] %s\r\n",socket,bufToHexStringRet);	
		fprintf(debugFile, "outbuflen = %d\r\n", outbuflen);
		fflush(debugFile);
	}
	int test = outbuflen;
	char *check = adler(outbuf, outbuflen);
	memcpy(outbufHeader + 2 , check, 4);
	outbufHeader[0] += 4;
	test += 4;

	// make sure that packets go at most once every minDistance ms
	int minDistance=175;
	minDistance=1;
	int nowAction=GetTickCount();
	if (nowAction-lastAction<minDistance) Sleep(minDistance-(nowAction-lastAction));
	if (debugFile&&COMPLEX)
	{				
		fprintf(debugFile,"sending; waited %dms delta=%dms [%d]\r\n",minDistance-(nowAction-lastAction),nowAction-lastAction,time(NULL));
		fprintf(debugFile, "outbuflen = %d\r\n", outbuflen);
	}	
	lastAction=GetTickCount();
	

	
	int ret=send(tibiaSocket, outbufHeader,test+2,0);
	
	if (debugFile&&COMPLEX)
	{		
		fprintf(debugFile,"sent %d bytes, ret=%d, lastSendFlags=%d\r\n",outbuflen+2,ret,lastSendFlags);
		fprintf(debugFile, "outbuflen = %d\r\n", outbuflen);
	}
	//delete check;
}


void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ)
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
	
	//Mine_send(tibiaSocket,sendbuf,19,lastSendFlags);
	sendBufferViaSocket(sendbuf);
	
}
void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId)
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
	
	//Mine_send(tibiaSocket,sendbuf,17,lastSendFlags);
	sendBufferViaSocket(sendbuf);
	
	
}

void autoAimAttack(int runeId)
{
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	int attackedCreature = reader.getAttackedCreature();
	
	CTibiaCharacter *ch = reader.getCharacterByTibiaId(attackedCreature);
	
	if (ch)
	{
		int chX=ch->x;
		int chY=ch->y;
		int chZ=ch->z;
		int contNr;
		for (contNr=0;contNr<memConstData.m_memMaxContainers;contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);
			
			if (cont->flagOnOff)
			{				
				CUIntArray acceptedItems;
				CTibiaItem *runeItem;
				
				acceptedItems.Add(runeId);
				runeItem = CModuleUtil::lookupItem(contNr,&acceptedItems);		
				if (runeItem->objectId)
				{			
					if (autoAimOnlyCreatures)
					{
						castRuneAgainstCreature(0x40+contNr,runeItem->pos,runeId,ch->tibiaId);
					} else {
						castRuneAgainstHuman(0x40+contNr,runeItem->pos,runeId,ch->x,ch->y,ch->z);							
					}
				}
				delete runeItem;
				
			};
			
			delete cont;
		}
		delete ch;
	}
}

struct parcelRecvActionData{
	int handle;
	int countLeft;
	int len;
	char actionData[MAX_PAYLOAD_LEN-4*3];
	parcelRecvActionData(){
		memset(actionData,0,sizeof(actionData));
	}
};

void parseRecvActionData(int handle, char* data, int len){
#ifdef _DEBUG
	if (len == 0){
		AfxMessageBox("parseRecvActionData: length is 0.");
	}
#endif
	struct ipcMessage mess;
	mess.messageType = 1010;

	parcelRecvActionData* p = ((parcelRecvActionData*)mess.payload);
	int maxlen = sizeof(p->actionData);
	int splits = len/maxlen;

	p->handle = handle;
	while (splits>=0) {
		p->countLeft = splits;
		p->len = min(len,maxlen);
		memcpy(p->actionData,data,p->len);
		memset(p->actionData+p->len,0,maxlen-p->len);
		mess.send();
		splits--;
		data += p->len;
		len -= p->len;
	}
#ifdef _DEBUG
	if (len != 0){
		AfxMessageBox("parseRecvActionData: part of message not sent.");
	}
#endif

}

void parseMessageSay(char *sayBuf)
{
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	
	struct ipcMessage mess;
	
	int len=strlen(sayBuf);
	if (len<900)
	{
		mess.messageType=1007;
		memcpy(mess.payload,&len,sizeof(int));		
		memcpy(mess.payload+4,sayBuf,len);		
		mess.send();
		mess.messageType=1008;
		mess.send();
		mess.messageType=1009;
		mess.send();
	}
	
	
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"$$$ ta command - '%s'\r\n",sayBuf);
	}
	
	
	if (!strcmp(sayBuf,"%ta hmm")&&autoAimActive)
	{			
		if (reader.getAttackedCreature())
		{
			autoAimAttack(itemProxy.getValueForConst("runeHMM"));					
		}
		
	}
	if (!strcmp(sayBuf,"%ta gfb")&&autoAimActive)
	{			
		if (reader.getAttackedCreature())
		{
			autoAimAttack(itemProxy.getValueForConst("runeGFB"));					
		}
		
	}
	if (!strcmp(sayBuf,"%ta sd")&&autoAimActive)
	{			
		if (reader.getAttackedCreature())
		{
			autoAimAttack(itemProxy.getValueForConst("runeSD"));					
		}
		
	}
	if (!strcmp(sayBuf,"%ta explo")&&autoAimActive)
	{			
		if (reader.getAttackedCreature())
		{
			autoAimAttack(itemProxy.getValueForConst("runeExplo"));
		}
		
	}
	if (!strcmp(sayBuf,"%ta selfuh")&&outSelfUHAvail)
	{							
		//Mine_send(tibiaSocket,tibiaState.outbufSelfUH,payloadLen(tibiaState.outbufSelfUH)+2,lastSendFlags);
		sendBufferViaSocket(tibiaState.outbufSelfUH);
	}
	if (!strcmp(sayBuf,"%ta fluidlife")&&outFluidLifeAvail)
	{
		//Mine_send(tibiaSocket,tibiaState.outbufFluidLife,payloadLen(tibiaState.outbufFluidLife)+2,lastSendFlags);
		sendBufferViaSocket(tibiaState.outbufFluidLife);
	}
	if (!strcmp(sayBuf,"%ta fluidmana")&&outFluidManaAvail)
	{
		//Mine_send(tibiaSocket,tibiaState.outbufFluidMana,payloadLen(tibiaState.outbufFluidMana)+2,lastSendFlags);
		sendBufferViaSocket(tibiaState.outbufFluidMana);
	}
}

//wis:working
int parseMessageForTibiaAction(char *buf,int len)
{	
	//char buf2[1111]="hi ";
	//for (int i=0;i<min(len,1110);i++){ sprintf(buf2,"%s %x",buf2,(char)buf[i]); }
	//AfxMessageBox(buf2);
	static int gatherStats=CModuleUtil::getTASetting("GatherBotStats");
	static int removeStatsMessage=CModuleUtil::getTASetting("RemoveBotStatsMessage");
	if(gatherStats){
		Protocol::outputPacket(NetworkMessage(buf));
	} else if (removeStatsMessage){
		CModuleUtil::setTASetting("RemoveBotStatsMessage",0);
	}
	Protocol::parsePacketIn(NetworkMessage(buf));
	//char buf3[1111]="bye ";
	//for (i=0;i<min(len,1110);i++){sprintf(buf2,"%s %2x",buf2,buf[i]);}
	//AfxMessageBox(buf2);
	int code=buf[2];
	if (code<0)
		code+=256;
	int buf3=buf[3];
	int buf4=buf[4];
	if (buf3<0) buf3+=256;
	if (buf4<0) buf4+=256;
	if (autoAimActive&&!autoAimOnlyCreatures&&code==0x84&&buf[0]==13&&buf3==0xff&&buf4==0xff)
	{
		CTibiaItemProxy itemProxy;
		// cast rune against monster
		int contNr=buf[5];
		int itemPos=buf[7];
		int v1=buf[8];
		int v2=buf[9];
		if (v1<0) v1+=256;
		if (v2<0) v2+=256;
		int objectId=v1+v2*256;
		v1=buf[11];
		v2=buf[12];
		int v3=buf[13];
		int v4=buf[14];
		if (v1<0) v1+=256;
		if (v2<0) v2+=256;
		if (v3<0) v3+=256;
		if (v4<0) v4+=256;
		
		unsigned int playerId=v1+v2*256+v3*256*256+v4*256*256*256;
		if (debugFile)
		{
			fprintf(debugFile,"### %x, %x\r\n",objectId,playerId);
		}
		if ((objectId==itemProxy.getValueForConst("runeHMM")||
			objectId==itemProxy.getValueForConst("runeGFB")||
			objectId==itemProxy.getValueForConst("runeSD")||
			objectId==itemProxy.getValueForConst("runeExplo"))&&
			playerId<0x40000000)
		{
			CMemReaderProxy reader;
			CTibiaCharacter *ch = reader.getCharacterByTibiaId(playerId);
			if (ch)
			{
				castRuneAgainstHuman(contNr,itemPos,objectId,ch->x,ch->y,ch->z);
				delete ch;
				return 1;
			}
		}
	}
	/**
	*/
	
	if (code==0x96&&(buf[3]==1 || buf[3]==0xb))
	{		
		// "say"
		char sayBuf[1000];
		int sayV1=buf[4];
		int sayV2=buf[5];
		if (sayV1<0) sayV1+=256;
		if (sayV2<0) sayV2+=256;
		int sayLen=sayV1+256*sayV2;
		
		if (sayLen>500) return 0;
		
		memset(sayBuf,0,1000);
		memcpy(sayBuf,buf+6,sayLen);
		if (!strncmp(sayBuf,"%ta ",3))
		{
			parseMessageSay(sayBuf);			
			return 1;
		}
	}
	
	if (code==0x96&&buf[3]==7)
	{		
		// "channel"
		char sayBuf[1000];
		int sayV1=buf[6];
		int sayV2=buf[7];
		if (sayV1<0) sayV1+=256;
		if (sayV2<0) sayV2+=256;
		int sayLen=sayV1+256*sayV2;
		
		if (sayLen>500) return 0;
		
		memset(sayBuf,0,1000);
		memcpy(sayBuf,buf+8,sayLen);
		if (!strncmp(sayBuf,"%ta ",3))
		{
			parseMessageSay(sayBuf);			
			return 1;
		}
	}
	
	if (code==0x96&&buf[3]==6)
	{		
		// "private"
		char sayBuf[1000];
		unsigned char nameV1=buf[4];
		unsigned char nameV2=buf[5];
		int nameLen=nameV1+256*nameV2;
		
		unsigned char sayV1=buf[6+nameLen];
		unsigned char sayV2=buf[7+nameLen];
		int sayLen=sayV1+256*sayV2;

		if (sayLen>500) return 0;
		
		memset(sayBuf,0,1000);
		memcpy(sayBuf,buf+8+nameLen,sayLen);
		if (!strncmp(sayBuf,"%ta ",3))
		{
			parseMessageSay(sayBuf);			
			return 1;
		}
	}
	
	if (code==0x96&&buf[3]==27)
	{
		CTibiaItemProxy itemProxy;
		// "private/NPC outgoing"
		char sayBuf[1000];
		char nickBuf[1000];
		int nickV1=buf[4];
		int nickV2=buf[5];
		if (nickV1<0) nickV1+=256;
		if (nickV2<0) nickV2+=256;
		int nickLen=nickV1+256*nickV2;
		
		if (nickLen>500) return 0;
		
		memset(nickBuf,0,1000);
		memcpy(nickBuf,buf+6,nickLen);
		
		int sayV1=buf[6+nickLen];
		int sayV2=buf[7+nickLen];
		if (sayV1<0) sayV1+=256;
		if (sayV2<0) sayV2+=256;
		int sayLen=sayV1+256*sayV2;
		
		if (sayLen>500) return 0;
		memset(sayBuf,0,1000);
		memcpy(sayBuf,buf+8+nickLen,sayLen);
		
		if (!strncmp(sayBuf,"%ta ",3))
		{
			parseMessageSay(sayBuf);		
			return 1;
		}
	}
	return 0;
}



void hookCallback(int value)
{
	struct ipcMessage mess;
	char *message=NULL;
	if (value==0x13)
	{
		message="%ta pause";
	}
	if (value==0x21)
	{		
		message="%ta lu";
	}
	if (value==0x22)
	{
		message="%ta ld";
	}
	if (message)
	{
		int len=strlen(message);
		memcpy(mess.payload,&len,sizeof(int));		
		memcpy(mess.payload+4,message,len);	
		if (value==0x21||value==0x22){
			mess.messageType=2001;			
			mess.send();
		}
		if (value==0x13){
			mess.messageType=2002;
			mess.send();
		}
	}
}
typedef void (*Proto_callback)(int value);
volatile Proto_callback hookCallbackFun=hookCallback;

LPCTSTR hooksFile=NULL;

void ActivateHookCallback()
{
	int size = 128;
	char mapFileBuf[1024];
	HANDLE hMapFile;
	
	
	wsprintf(mapFileBuf,"Global\\tibiaauto-mapfile-%d",::GetCurrentProcessId());	
		
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security 
		PAGE_READWRITE,          // read/write access
		0,                       // max. object size 
		size,                // buffer size  
		mapFileBuf);                 // name of mapping object	
		
	if (hMapFile == NULL) 
	{       
		return;
	}	
	
	hooksFile = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,                   
		0,                   
		size);           
		
	if (hooksFile == NULL) 
	{       
		return;
	}	
		
	CopyMemory((PVOID)hooksFile, (PVOID)&hookCallbackFun, sizeof(void *));   	
	
	
	
	
	
	
}



int WINAPI Mine_send(SOCKET s,char* buf,int len,int flags)
{
	int i;
	int identical=1;
	if (len!=encryptPos+8+6)
	{
		identical=0;
	} else {
		for (i=0;i<encryptPos+8;i++)
		{
			if (buf[6+i]!=encryptAfterBuf[i]) {
				identical=0;
			}
		}
	}
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"!!! send !!! [identical=%d]\r\n",identical);
		fflush(debugFile);
	}
	
	if (debugFile&&!SENTONLY)
	{	
		bufToHexString(buf,len);	
		fprintf(debugFile,"E> [%x] %s\r\n",socket,bufToHexStringRet);
		fflush(debugFile);
	}
	if (identical)
	{
		// pass for further processing only when we know content before encryption
		
		parseMessage(encryptBeforeBuf,encryptLen,debugFile,1,1);
		if (debugFile)
		{	
			bufToHexString(encryptBeforeBuf,encryptLen);	
			fprintf(debugFile,"-> [%x] %s\r\n",socket,bufToHexStringRet);	
			fflush(debugFile);
		}

		if (parseMessageForTibiaAction(encryptBeforeBuf,encryptLen))
			return len;
		
	}
	
	if (debugFile)
	{
		fflush(debugFile);
	}
	
    
	tibiaSocket=s;
	lastSendFlags=flags;
	
	
    int ret=0;
	ret=Real_send(s,buf,len,flags);	
	
	return ret;
}


int WINAPI Mine_recv(SOCKET s,char* buf,int len,int flags)
{
	if (debugFile && COMPLEX && !SENTONLY)
	{
		fprintf(debugFile,"!!! recv !!!\r\n");
		fflush(debugFile);
	}
	int offset=0;
	
	int realRecvLen=0;
	
	// use "standard" tibia own socket
	if (strncmp(buf+15,"Aprem",5) == 0) {
		(buf+15)[0] = 'o';
	}
	realRecvLen=Real_recv(s,buf+offset,len-offset,flags);
	if (realRecvLen!=-1)
	{
		if (debugFile&&!SENTONLY)
		{
			if (COMPLEX){
			fprintf(debugFile,"realRecvLen=%d\r\n",realRecvLen);
			fflush(debugFile);					
			}
			
			bufToHexString(buf,realRecvLen);
			fprintf(debugFile,"<- [%x] %s\r\n",socket,bufToHexStringRet);
			
			parseMessage(buf,realRecvLen,debugFile,0,1);
			fflush(debugFile);
		}
	}
	
	
	
	if (debugFile)
	{
		fflush(debugFile);
	}
    
	tibiaSocket=s;
	
	
    return realRecvLen+offset;
}

SOCKET WINAPI Mine_socket(int af,int type,int protocol)
{
	SOCKET s = Real_socket(af,type,protocol);
	
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"got socket: %d, %d, %d -> %d\r\n",af,type,protocol,s);
		fflush(debugFile);
	}	
    
	tibiaSocket=s;
	
    return s;
}

int WINAPI Mine_connect(SOCKET s,const struct sockaddr* name,int namelen) 
{
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"connect [pre]: %d, %x, %d\r\n",s,name,namelen);
		fflush(debugFile);
	}
	
	
	int ret = Real_connect(s,name,namelen);	
	
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"connect: %d, %x, %d\r\n",s,name,namelen);
		fflush(debugFile);
	}
	
	memcpy(lastConnectName,name,16);
	
	return ret;
};

int WINAPI Mine_select(
					   int nfds,
					   fd_set* readfds,
					   fd_set* writefds,
					   fd_set* exceptfds,
					   const struct timeval* timeout
					   )
{
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"select: %x, %x, %x\r\n",readfds,writefds,exceptfds);
		fflush(debugFile);
	}
	return Real_select(nfds,readfds,writefds,exceptfds,timeout);
}




void InitialiseIPC()
{	
	HMODULE aa = GetModuleHandle(0);
	char buf[128];	
	char lpszPipename[1024];
	int pid = ::GetCurrentProcessId();
	sprintf(lpszPipename,"\\\\.\\pipe\\tibiaAutoPipe-%d",pid);	
	
	
	while (1)
	{		
		
		hPipe = CreateFile( 
			lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE, 
			0,              // no sharing 
			NULL,           // no security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 
		
		
		// pipe handle is invalid - proceed
		if (hPipe != INVALID_HANDLE_VALUE) 	
		{
			break;
		}
		
		
		if (GetLastError() != ERROR_PIPE_BUSY) 
		{
			
			sprintf(buf,"Could not open pipe (not busy): %d",GetLastError());
			//MessageBox(NULL,"","error 3",0); 
			Sleep(50);
			continue;
		}
		
		// All pipe instances are busy, so wait for 10 seconds. 
		
		if (! WaitNamedPipe(lpszPipename, 1000) ) 
		{
			sprintf(buf,"Could not open pipe (busy too long): %d",GetLastError());
			//MessageBox(NULL,buf,"error",0); 
			Sleep(50);
			continue;
		}
	}
	
	DWORD dwMode = PIPE_READMODE_MESSAGE; 
	BOOL fSuccess = SetNamedPipeHandleState( 
		hPipe,    // pipe handle
		&dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes
		NULL);    // don't set maximum time
	if (!fSuccess) 
	{	
		InitialiseIPC();
	} else {
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"[debug] straight IPC initialised ok\r\n");
		}
	}
}

void InitialiseIPCback()
{
	char buf[1024];
	char lpszPipename[1024];
	sprintf(lpszPipename,"\\\\.\\pipe\\tibiaAutoPipe-back-%d",partnerProcessId);
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"[debug] IPC queue is %s\r\n",lpszPipename);
	}
	
	
	hPipeBack = CreateNamedPipe(
		lpszPipename,             // pipe name 
		PIPE_ACCESS_DUPLEX,       // read/write access 
		PIPE_TYPE_MESSAGE |       // message type pipe 
		PIPE_READMODE_MESSAGE |   // message-read mode 
		PIPE_WAIT,                // blocking mode 
		PIPE_UNLIMITED_INSTANCES, // max. instances  
		163840,                  // output buffer size 
		163840,                  // input buffer size 
		1000,                        // client time-out 
		NULL);                    // no security attribute 
	
	
	if (hPipeBack == INVALID_HANDLE_VALUE) 
	{		
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"[ipcback] Invalid pipe handle: %d\r\n",GetLastError());
			return;
        }
	}			
	
	
	BOOL fConnected = ConnectNamedPipe(hPipeBack, NULL) ?  true : (GetLastError() == ERROR_PIPE_CONNECTED); 
	
	
	if (!fConnected)
	{
		sprintf(buf,"client not connected via pipe: %d",GetLastError());
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"[ipcback] client not connected via pipe: %d\r\n",GetLastError());
			return;
        }
	}
	
	
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"[debug] back IPC initialised ok\r\n");
		fflush(debugFile);
	}	
	
}




void InitialiseHooks()
{	
	
	DetourFunctionWithTrampoline((PBYTE)Real_send,(PBYTE)Mine_send);
	DetourFunctionWithTrampoline((PBYTE)Real_recv,(PBYTE)Mine_recv);	
	DetourFunctionWithTrampoline((PBYTE)Real_connect,(PBYTE)Mine_connect);	
	DetourFunctionWithTrampoline((PBYTE)Real_socket,(PBYTE)Mine_socket);		
	//DetourFunctionWithTrampoline((PBYTE)Real_select,(PBYTE)Mine_select);
};





void InitialiseDebugFile()
{
	if (OUTPUT_DEBUG_INFO)
		debugFile=fopen("C:\\temp\\tibiaDebug.txt","wb");
	else
		debugFile=NULL;

	if (debugFile) fprintf(debugFile,"Start\r\n");
	fflush(debugFile);
	debugFileStart=time(NULL);

}

void InitialiseTibiaState()
{
	tibiaState.attackedCreature=0;
	memset(tmp3,0x00,256);
}

void InitialiseCreatureInfo()
{
	int i;
	for (i=0;i<MAX_CREATUREINFO;i++)
	{
		creatureInfoPlayerName[i][0]=0;
	}
}


//(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<edx>, int v9)
int OUTmyPrintText(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9)
{
	int ret;
	typedef void (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9);
	//Proto_fun fun = (Proto_fun)(0x4B4B20); // 8.74
	//Proto_fun fun = (Proto_fun)(0x4B4E70); // 9.00
	//Proto_fun fun = (Proto_fun)(0x4B6E20); // 9.10
	//Proto_fun fun = (Proto_fun)(0x4B6F10); // 9.20
	//Proto_fun fun = (Proto_fun)(0x4B7090); // 9.31
	Proto_fun fun = (Proto_fun)(0x4BFFD0); // 9.40

	__asm{
		push [ebp + 0x28]
		mov edx, [ebp + 0x24]
		push [ebp + 0x20]
		push [ebp + 0x1C]
		push [ebp + 0x18]
		push [ebp + 0x14]
		push [ebp + 0x10]
		push [ebp + 0x0C]
		push [ebp + 0x08]
		call fun
		add esp, 0x20
		mov [ebp - 4],eax
	}
	return ret;
}
int myPrintText(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign)
{
	//myPrintText runs continuously, so this is a good place to check if the TA Message pipe has anything for us
	if (taMessageStart!=taMessageEnd)
	{
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"privChanBufferPtr=%x\r\n",privChanBufferPtr);
			fflush(debugFile);
		}
		if (privChanBufferPtr)
		{			
			OUTmyInterceptInfoMessageBox(privChanBufferPtr,0,(int)taMessage[taMessageEnd],4,(int)"Tibia Auto",0,0,0,0);
		}
		taMessageEnd++;
		if (taMessageEnd==TA_MESSAGE_QLEN)
			taMessageEnd=0;
		
	}

	int titleOffset=0;	

	int ret = OUTmyPrintText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
	for (int loop = 0; loop < 100; loop++ ) {
		if (HUDisplay[loop].pos.x && HUDisplay[loop].pos.y && HUDisplay[loop].message != NULL && HUDisplay[loop].message[0] != '\0')
			OUTmyPrintText(1, HUDisplay[loop].pos.x, HUDisplay[loop].pos.y, nFont, HUDisplay[loop].redColor, HUDisplay[loop].greenColor, HUDisplay[loop].blueColor, HUDisplay[loop].message, 0);
	}
	return ret;
}
__declspec(naked) void INmyPrintText() //(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<edx>, int v9)
{__asm{
	push ebp
	mov ebp, esp
	push [ebp + 0x24]
	push edx
	push [ebp + 0x20]
	push [ebp + 0x1C]
	push [ebp + 0x18]
	push [ebp + 0x14]
	push [ebp + 0x10]
	push [ebp + 0x0C]
	push [ebp + 0x08]
	call myPrintText
	leave
	ret
}}


//(int align<ecx>, char *str<edx>, int visible, int x, int y, int fontNumber, int colR, int colG, int colB, int showFormatting, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, int v16<ebx>, int v17<edi>)
int OUTmyPlayerNameText(int align, char *str, int visible, int x, int y, int fontNumber, int colR, int colG, int colB, int showFormatting, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, int v16, int v17)
{
	int ret;
	typedef int (*Proto_fun)(int align, char *str, int visible, int x, int y, int fontNumber, int colR, int colG, int colB, int showFormatting, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, int v16, int v17);

	//Proto_fun fun=(Proto_fun)(0x4B3FA0); // 8.74
	//Proto_fun fun=(Proto_fun)(0x4B42F0); // 9.00
	//Proto_fun fun=(Proto_fun)(0x4B6290); // 9.10
	//Proto_fun fun=(Proto_fun)(0x4B6380); // 9.20
	//Proto_fun fun=(Proto_fun)(0x4B6500); // 9.31
	Proto_fun fun=(Proto_fun)(0x4BF440); // 9.40
	
	__asm{
		mov edi, [ebp + 0x48]
		mov ebx, [ebp + 0x44]
		push [ebp + 0x40]
		push [ebp + 0x3C]
		push [ebp + 0x38]
		push [ebp + 0x34]
		push [ebp + 0x30]
		push [ebp + 0x2C]
		push [ebp + 0x28]
		push [ebp + 0x24]
		push [ebp + 0x20]
		push [ebp + 0x1C]
		push [ebp + 0x18]
		push [ebp + 0x14]
		push [ebp + 0x10]
		mov edx, [ebp + 0x0C]
		mov ecx, [ebp + 0x08]
		call fun
		add esp, 0x34
		mov [ebp - 4],eax
	}
	return ret;
}
int myPlayerNameText(int align, char *str, int visible, int x, int y, int fontNumber, int colR, int colG, int colB, int showFormatting, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, int v16, int v17)
{
	int ret;
	int titleOffset=0;
	/*
	char buf[1111];
	sprintf(buf,"myPlayerNameText(%x,%s,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",align, str, visible,x,y,fontNumber,colR,colG,colB,showFormatting, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
	static int aa = 0;
	int barn=0;
	if (privChanBufferPtr && (colR!=0xa0 || colG!=0xa0 || colB!=0xff) && aa%1 == 0){
		if (colR==0xf0 && colG==0xf0 && colB==0 && fontNumber==4){
			if (aa>10000000) aa=0;
			OUTmyInterceptInfoMessageBox(privChanBufferPtr,0,(int)buf,4,(int)"Tibia Auto",0,0,0,0);
			barn=1;
		}
	}
	aa++;
	if (debugFile) {		
		fprintf(debugFile,"myPlayerNameText(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)\r\n",align,str,visible,x,y,fontNumber,colR,colG,colB,showFormatting, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
		fflush(debugFile);
	}
	*/
	if (str!=NULL&&fontNumber==2&&strlen(str)<1000)
	{
		char info1[1024];
		char info2[1024];
		info1[0]=info2[0]=0;
		int i,len;		
		char convString[1024];

		sprintf(convString,"%s",str);	
		for (i=0,len=strlen(str);i<len;i++)
		{
			if (convString[i]=='[')
				convString[i]='\0';
		}
		
		for (i=0;i<MAX_CREATUREINFO;i++)
		{
			if (!strcmp(str,creatureInfoPlayerName[i]))
			{
				strncpy(info1,creatureInfoPlayerInfo1[i],128);
				strncpy(info2,creatureInfoPlayerInfo2[i],128);
				break;
			}
		}				
		
		if (strlen(info2)) 
		{
			OUTmyPlayerNameText(align,info2,visible,x,y-titleOffset,fontNumber,colR,colG,colB,10, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
			titleOffset+=14;
		}		
		if (strlen(info1)) 
		{
			OUTmyPlayerNameText(align,info1,visible,x,y-titleOffset,fontNumber,colR,colG,colB,showFormatting, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
			titleOffset+=14;
		}
		ret = OUTmyPlayerNameText(align,convString,visible,x,y-titleOffset,fontNumber,colR,colG,colB, showFormatting, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
	} else {
		ret = OUTmyPlayerNameText(align,str, visible,x,y-titleOffset,fontNumber,colR,colG,colB,showFormatting, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
	}
	return ret;
}

__declspec(naked) void INmyPlayerNameText() //()
{__asm{
	push ebp
	mov ebp, esp
	push edi
	push ebx
	push [ebp + 0x38]
	push [ebp + 0x34]
	push [ebp + 0x30]
	push [ebp + 0x2C]
	push [ebp + 0x28]
	push [ebp + 0x24]
	push [ebp + 0x20]
	push [ebp + 0x1C]
	push [ebp + 0x18]
	push [ebp + 0x14]
	push [ebp + 0x10]
	push [ebp + 0x0C]
	push [ebp + 0x08]
	push edx
	push ecx
	call myPlayerNameText
	leave
	ret
}}


void myInterceptInfoMiddleScreen(int type,char *s)
{	
	typedef void (*Proto_fun)(int type,char *s);
	//Proto_fun fun=(Proto_fun)(0x54C1B0); // 8.74
	//Proto_fun fun=(Proto_fun)(0x54D480); // 9.00
	//Proto_fun fun=(Proto_fun)(0x54BD20); // 9.10
	//Proto_fun fun=(Proto_fun)(0x54BEA0); // 9.20
	//Proto_fun fun=(Proto_fun)(0x54C200); // 9.31
	Proto_fun fun=(Proto_fun)(0x55D2F0); // 9.40

	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"got middle screen %d/%s\r\n",type,s);
	}
	
	char buf[1111];
	if (s) sprintf(buf,"XXY v1Type=%x, v2Text=%s",type,s);
	else sprintf(buf,"XXY v1Type=%x, v2Text=",type);

	//OUTmyInterceptInfoMessageBox(privChanBufferPtr,0,(int)buf,4,(int)"Tibia Auto",0,0,0);
	if (type==0x13)
	{
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"got see '%s'\r\n",s);
		}
		unsigned long bytesWritten=0;						
		
		struct ipcMessage mess;
		mess.messageType=1002;	
		int len=strlen(s);
		if (len<900)
		{
			memcpy(mess.payload,&len,sizeof(int));		
			memcpy(mess.payload+4,s,len);
			mess.send();
		}
	}
	
	
	
	if (type!=0x13||time(NULL)>ignoreLookEnd)
	{
		/*
		if (true && !strncmp(s,"Loot",4)){
			int len=strlen(s);
			for (int i=0;i<len;i++){
				if (s[i]==':'){
					s[++i]='\n';
				}
				if (s[i]==','){
					s[i]=' ';
					s[++i]='\n';
				}
			}
		}*/
		fun(type,s);
	} else {
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"ignoring look");
		}
	}
}


int myIsCreatureVisible(int *creaturePtr) //Should Draw Creature(not used)
{	
	if (revealCNameActive)
	{
		int ret=0;
		
		static CMemReaderProxy *reader=NULL;
		
		if (!reader)
			reader = new CMemReaderProxy();
		static CMemConstData memConstData = reader->getMemConstData();
		static int firstAddr = memConstData.m_memAddressFirstCreature;
		static int crSize = memConstData.m_memLengthCreature;
		
		
		
		if (creaturePtr)
		{		
			int *coordPtr;
			coordPtr=creaturePtr+36/4;
			int coordX=*coordPtr;	
			coordPtr=creaturePtr+40/4;
			int coordY=*coordPtr;
			int loggedCharNr = reader->getLoggedCharNr();
			coordPtr=(int *)(firstAddr+loggedCharNr*crSize+36);
			int selfX=*coordPtr;
			coordPtr=(int *)(firstAddr+loggedCharNr*crSize+40);
			int selfY=*coordPtr;
			if (abs(coordX-selfX)<=8&&abs(coordY-selfY)<=6) ret=1;
		}	
		return ret;
	} else {
		typedef int (*Proto_fun)(int *creaturePtr);
		//Proto_fun fun=(Proto_fun)(0x462660); // 8.74
		//Proto_fun fun=(Proto_fun)(0x462880); // 9.00
		//Proto_fun fun=(Proto_fun)(0x465AD0); // 9.10
		//Proto_fun fun=(Proto_fun)(0x465BD0); // 9.20
		//Proto_fun fun=(Proto_fun)(0x465CD0); // 9.31
		Proto_fun fun=(Proto_fun)(0x467CC0); // 9.40


		return fun(creaturePtr);
	}
	
}

int OUTmyInterceptEncrypt(int v1, int v2) //<eax>(int v1, int v2<esi>)
{
	int ret;
	typedef void (*Proto_fun)(int v1, int v2);
	//Proto_fun fun=(Proto_fun)(0x555E80); // 8.74
	//Proto_fun fun=(Proto_fun)(0x557160); // 9.00
	//Proto_fun fun=(Proto_fun)(0x55BA70); // 9.10
	//Proto_fun fun=(Proto_fun)(0x55BC10); // 9.20
	//Proto_fun fun=(Proto_fun)(0x55BFB0); // 9.31
	Proto_fun fun=(Proto_fun)(0x56D030); // 9.40

	__asm{
		push [ebp + 0x08]
		mov esi, [ebp + 0x0C]
		call fun
		add esp, 4
		mov [ebp - 4],eax
	}
	return ret;
}
int myInterceptEncrypt(int v1, int v2)
{
	encryptKeyPtr=v2;
	if (debugFile&&COMPLEX)
	{
		fprintf(debugFile,"QQQQQQQQQQ: %x,%x,%x\r\n",encryptKeyPtr,v1,v2);
		fflush(debugFile);
	}
	
	if (v1!=encryptPrevPtr+8)
	{
		// means: next packet is being encrypted
		encryptPrevPtr=v1;
		int lowB=((char *)v1)[0];
		int hiB=((char *)v1)[1];
		if (lowB<0) lowB+=256;
		if (hiB<0) hiB+=256;
		encryptLen=lowB+hiB*256+2;
		encryptPos=0;
	} else {
		encryptPos+=8;
		encryptPrevPtr+=8;
	}
	
	memcpy(encryptBeforeBuf+encryptPos,(void *)v1,8);	
	int ret = OUTmyInterceptEncrypt(v1,v2);
	memcpy(encryptAfterBuf+encryptPos,(void *)v1,8);	
	return ret;
}
__declspec(naked) void INmyInterceptEncrypt() //<eax>(int v1, int v2<esi>)
{__asm{
	push ebp
	mov ebp, esp
	push esi
	push [ebp + 8]
	call myInterceptEncrypt
	leave
	ret
}}


int OUTmyInterceptDecrypt(int v1, int v2) //<eax>(int v1<esi>, int v2)
{
	int ret; //create [ebp - 4] variable
	typedef void (*Proto_fun)();

	//Proto_fun fun=(Proto_fun)(0x555FB0); // 8.74
	//Proto_fun fun=(Proto_fun)(0x557290); // 9.00
	//Proto_fun fun=(Proto_fun)(0x55BBA0); // 9.10
	//Proto_fun fun=(Proto_fun)(0x55BD40); // 9.20
	//Proto_fun fun=(Proto_fun)(0x55C0E0); // 9.31
	Proto_fun fun=(Proto_fun)(0x56D160); // 9.40

	__asm{
		mov esi, [ebp + 0x0C] // move v2 to ESI
		push [ebp + 0x08] // push v1 onto the stack
		call fun // jump to the real Tibia function
		add esp, 4 // "pop" v2 from the stack
		mov [ebp - 4],eax //store value returned from real function in ret
	}
	return ret;
}
int myInterceptDecrypt(int v1, int v2) //<eax>(int v1<esi>, int v2)
{
	encryptKeyPtr=v2;
	return OUTmyInterceptDecrypt(v1,v2);
}
__declspec(naked) void INmyInterceptDecrypt() //<eax>(int v1, int v2<esi>)
{__asm{
	push ebp //store extended base pointer for prev func
	mov ebp, esp //use current stack pointer as base pointer
	push esi //Push on v2 stored in esi
	push [ebp + 8] //Push on v1 from first passed parameter
	call myInterceptDecrypt //jump to actuar function
	leave //move EBP to ESP and pop EBP, the extended base pointer on the stack gets restored
	ret //jump back to location on stack
}}

/*int OUTmyInterceptAllRegs(int v1, int v2, int v3, int v4, int v5, int v6, int va, int vb, int vc, int vd, int vdi, int vsi)
{
	int ret;
	typedef void (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int va, int vb, int vc, int vd, int vdi, int vsi);	

	Proto_fun fun=(Proto_fun)(0x);

	__asm{
		mov edi, [ebp + 0x34]
		mov esi, [ebp + 0x30]
		mov edx, [ebp + 0x2C]
		mov ecx, [ebp + 0x28]
		mov ebx, [ebp + 0x24]
		mov eax, [ebp + 0x20]
		push [ebp + 0x1C]
		push [ebp + 0x18]
		push [ebp + 0x14]
		push [ebp + 0x10]
		push [ebp + 0x0C]
		push [ebp + 0x08]
		call fun
		add esp, 24
		mov [ebp - 4],eax
	}
	return ret;

}
int myInterceptAllRegs(int v1, int v2, int v3, int v4, int v5, int v6, int va, int vb, int vc, int vd, int vdi, int vsi)
{
	char buf[1111];
	sprintf(buf,"XXX v1=%x, v2=%x, v3=%x, v4=%x, v5=%x, v6=%x, va=%x, vb=%x, vc=%x, vd=%x, vdi=%x, vsi=%x\r\n", v1, v2, v3, v4, v5, v6, va, vb, vc, vd, vdi, vsi);
	AfxMessageBox(buf);

	return OUTmyInterceptAllRegs(v1, v2, v3, v4, v5, v6, va, vb, vc, vd, vdi, vsi);
}
__declspec(naked) void INmyInterceptAllRegs() //<eax>(int v1, int v2<esi>)
{__asm{
	push ebp
	mov ebp, esp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax
	push [ebp + 0x1C]
	push [ebp + 0x18]
	push [ebp + 0x14]
	push [ebp + 0x10]
	push [ebp + 0x0C]
	push [ebp + 8]
	call myInterceptAllRegs
	leave
	ret
}}
*/

//wiz
//This captured function is what Tibia uses to decrypt the packet
// and determine which action types it contains in order to parse it properly
// The function returns the action type and Tibia then parses the stream.
// If the stream has not yet been used up, it will continually run this function to 
// determine the type of and parse the next action.
// We can interfere with this process, but this requires knowing packet structures
// and error handling for incomplete packets which takes much more work than payoff.
int prevRecvStreamPos = 0;
int Runtest=0;
int myShouldParseRecv(){
	CMemReaderProxy reader;
	CRegexpProxy regexpProxy;

	if (Runtest){
		Runtest = 0;
		recvRegex[0].inUse = 1;
		regexpProxy.regncomp(&(recvRegex[0].preg),"^\x6b",2,REG_NOSUB|REG_EXTENDED);
		recvRegexCount = 1;
	}
	typedef int (*Proto_fun)();
	//Proto_fun fun=(Proto_fun)(0x4F9520); // 8.74
	//Proto_fun fun=(Proto_fun)(0x4FA0A0); // 9.00
	//Proto_fun fun=(Proto_fun)(0x4FC0C0); // 9.10
	//Proto_fun fun=(Proto_fun)(0x4FC2E0); // 9.20
	//Proto_fun fun=(Proto_fun)(0x4FC460); // 9.31
	Proto_fun fun=(Proto_fun)(0x505D40); // 9.40
	int ret = fun();

	//look for this address near above location, it will be off by 8
	//packStream* recvStream = (packStream*)(0x7C6CEC); //8.74
	//packStream* recvStream = (packStream*)(0x7C8D0C); //9.00
	//packStream* recvStream = (packStream*)(0x9B2690); //9.10
	//packStream* recvStream = (packStream*)(0x9B2880); //9.20
	//packStream* recvStream = (packStream*)(0x9B4880); //9.31
	packStream* recvStream = (packStream*)(0x9D2B90); //9.40
	//int packLen = ((unsigned char)recvStream->s[6]) + ((unsigned char)recvStream->s[7])*256;
	if (prevRecvStreamPos){ // If there is an action we can read
		int actionStart = prevRecvStreamPos - 1;
		int actionEnd = (ret==-1?recvStream->pos-1:recvStream->pos-2);
		int actionLen = actionEnd - actionStart + 1;
		{for (int i=0;i<recvRegexCount;i++){
			if (recvRegex[i].inUse == 1){
				int match = regexpProxy.regnexec(&(recvRegex[i].preg),((char*)recvStream->s+actionStart),actionLen,0,NULL,0);
				if (match == 0){
					bufToHexString(((char*)recvStream->s+actionStart),actionLen);
					if (privChanBufferPtr){
						OUTmyInterceptInfoMessageBox(privChanBufferPtr,0,(int)bufToHexStringRet,4,(int)"Tibia Auto",0,0,0,0);
					}
					int a=0;
					parseRecvActionData	(recvRegex[i].handle,(char*)recvStream->s+actionStart,actionLen);
				}
			}
		}}
		/*
		int i;
		for (i=0;i<recvRegexCount;i++){
			if (recvRegex[i].inUse == 0) break;
		}
		if (i<RECV_REGEX_MAX){
			recvRegex[i] = actionRegexData();
		}
		*/
		
		
	}
	prevRecvStreamPos = (ret==-1 ? 0 : recvStream->pos);
	//char buf[1111];
	//sprintf(bufToHexStringRet,"%x:%d, %d  %x%x",ret, recvStream->pos, recvStream->length,((char)recvStream->s[recvStream->pos-2]),((char)recvStream->s[recvStream->pos-2-1]));
	//AfxMessageBox(buf);
	//bufToHexString(recvStream->s,30);
	//if (privChanBufferPtr){
	//	myInterceptInfoMessageBox(privChanBufferPtr,0,(int)bufToHexStringRet,4,(int)"Tibia Auto",0,0);
	//}
	if (0&&reader.getConnectionState() == 8){
		if (ret == 107){
			ret = -1;
		}
		if (ret == -1) {
			recvStream->pos += 10;
		}
	}
	return ret;
}

//<eax>(int v1<edx>, int v2, int v3, int v4, int v5, int v6, int v7, int v8<ecx>, int v9<esi>)
int OUTmyInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9)
{
	int ret;
	typedef int (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);

	//Proto_fun fun=(Proto_fun)(0x54F0E0); // 8.74
	//Proto_fun fun=(Proto_fun)(0x5503C0); // 9.00
	//Proto_fun fun=(Proto_fun)(0x54FB60); // 9.10
	//Proto_fun fun=(Proto_fun)(0x54FCE0); // 9.20
	//Proto_fun fun=(Proto_fun)(0x550040); // 9.31
	Proto_fun fun=(Proto_fun)(0x561130); // 9.40

	__asm{
		mov esi, [ebp + 0x28]
		mov ecx, [ebp + 0x24]
		push [ebp + 0x20]
		push [ebp + 0x1C]
		push [ebp + 0x18]
		push [ebp + 0x14]
		push [ebp + 0x10]
		push [ebp + 0x0C]
		mov edx, [ebp + 0x08]
		call fun
		add esp, 24
		mov [ebp - 4],eax
	}
	return ret;

}
//<eax>(int v1<edx>, int v2, int v3, int v4, int v5, int v6, int v7, int v8<ecx>, int v9<esi>)
int myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9)
{
	char buf[1111];
	sprintf(buf,"XXX v1ConsolePointer=%x, v2Time=%x, v3Text=%x, v4Type=%x, v5Name=%x, v6=%x, v7Type2=%x, v8Lvl=%d, v9=%x\r\n", v1, v2, v3, v4, v5, v6, v7, v8, v9);
	//Types 1 2 3 7 12-yellow,4 21-dark blue, 5 6-light blue, 8 16 17 18 19-white,9 10 11 22 23-red, 13 14-orange, 15-pink, 19-green
	//1-say
	//2-whisper
	//3-yell
	//4-
	//5-NPC to me
	//6-Private to me
	//7-channel messages
	//8-
	//9-
	//10-
	//11-
	//12-
	//13-
	//14-
	//15-
	//16-rule report messages
	//17-white client messages, login, permanent ignore, 
	//18-white game info messages, attack, experience, depot volume
	//19-all green message
	//20-
	//21-to myself in dark blue
	//22-
	//23-
	//AfxMessageBox(buf);
	//OUTmyInterceptInfoMessageBox(v1, v2, (int)buf, 4, (int)"Tibia Auto", v6, v7, 0, v9);
	//if (v9!=0 && privChanBufferPtr) OUTmyInterceptInfoMessageBox(privChanBufferPtr,0,(int)buf,4,(int)"Tibia Auto", v6, v7, 0, v9);

	char *nick=(char *)v5;
	char *s=(char *)v3;

	int type=v4;
	if (debugFile)
	{
		fprintf(debugFile,"XXX s1=%s s2=%s\r\n",s,nick);
	}
	//char* happy2="dddddddddddddddddddd";
	//memcpy(*(char**)(*(char**)v1),happy2,strlen(happy2));
	// note: at least 0x14 bytes are passed on stack; at most 0x2c bytes are passed
	
	if (type==1)
	{
		privChanBufferPtr=v1;
	}

	
	
	int nickLen=nick?strlen(nick):0;
	int msgLen=strlen(s);
	if (nickLen+msgLen<900)
	{
		struct ipcMessage mess;
		memcpy(mess.payload,&type,sizeof(int));
		memcpy(mess.payload+4,&nickLen,sizeof(int));
		memcpy(mess.payload+8,&msgLen,sizeof(int));
		if (nickLen) memcpy(mess.payload+12,nick,nickLen);
		memcpy(mess.payload+12+nickLen,s,msgLen);
		mess.messageType=1001;
		mess.send();
		mess.messageType=1003;
		mess.send();
		mess.messageType=1004;
		mess.send();
		//mess.messageType=1005;
		//mess.send();
		mess.messageType=1006;
		mess.send();
	}
	
	if (type!=0x13||time(NULL)>ignoreLookEnd)
	{ 		
		//int happy=*(int*)v1;
		//happy+=0x20;
		//*(int***)v1+=0x20;

		return OUTmyInterceptInfoMessageBox(v1, v2, v3, v4, v5, v6, v7, v8, v9);
	}
	return 2;
}
__declspec(naked) void INmyInterceptInfoMessageBox() //<eax>(int v1<edx>, int v2, int v3, int v4, int v5, int v6, int v7, int v8<ecx>, int v9<esi>)
{__asm{
	push ebp
	mov ebp, esp
	push esi
	push ecx
	push [ebp + 0x1C]
	push [ebp + 0x18]
	push [ebp + 0x14]
	push [ebp + 0x10]
	push [ebp + 0x0C]
	push [ebp + 8]
	push edx
//	mov edi, 0xBD6D6173
//	mov esi, 0xBD6D6174
//	mov edx, 0xBD6D6175
//	mov ecx, 0xBD6D6176
//	mov ebx, 0xBD6D6177
//	mov eax, 0xBD6D6178
	call myInterceptInfoMessageBox
	leave
	ret
}}

void trapFun(HANDLE dwHandle,int addr,unsigned int targetFun)
{				
	int targetAddr=targetFun - addr - 4;
	WriteProcessMemory(dwHandle, (void *)addr, &targetAddr,   sizeof(long int), NULL);	
}

void InitialisePlayerInfoHack()
{		
	DWORD procId=GetCurrentProcessId();
	HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);    
	
	//trapFun(dwHandle,0x44F1DC+1,(unsigned int)myPrintText); // 8.74
	//trapFun(dwHandle,0x44F22C+1,(unsigned int)myPrintText); // 8.74
	//trapFun(dwHandle,0x45B568+1,(unsigned int)myPrintText); // 8.74
	//trapFun(dwHandle,0x4F5493+1,(unsigned int)myPrintText); // 8.74
	//trapFun(dwHandle,0x4F6257+1,(unsigned int)myPrintText); // 8.74

	//trapFun(dwHandle,0x44F2FC+1,(unsigned int)myPrintText); // 9.00
	//trapFun(dwHandle,0x44F34C+1,(unsigned int)myPrintText); // 9.00
	//trapFun(dwHandle,0x45B778+1,(unsigned int)myPrintText); // 9.00
	//trapFun(dwHandle,0x4F6013+1,(unsigned int)myPrintText); // 9.00
	//trapFun(dwHandle,0x4F6DD7+1,(unsigned int)myPrintText); // 9.00

	//trapFun(dwHandle,0x45248C+1,(unsigned int)INmyPrintText); // 9.10
	//trapFun(dwHandle,0x4524D6+1,(unsigned int)INmyPrintText); // 9.10
	//trapFun(dwHandle,0x45A793+1,(unsigned int)INmyPrintText); // 9.10
	//trapFun(dwHandle,0x4F010F+1,(unsigned int)INmyPrintText); // 9.10
	//trapFun(dwHandle,0x4F0EB9+1,(unsigned int)INmyPrintText); // 9.10

	//trapFun(dwHandle,0x45257C+1,(unsigned int)INmyPrintText); // 9.20
	//trapFun(dwHandle,0x4525C6+1,(unsigned int)INmyPrintText); // 
	//trapFun(dwHandle,0x45A883+1,(unsigned int)INmyPrintText); // 
	//trapFun(dwHandle,0x4F032F+1,(unsigned int)INmyPrintText); // 
	//trapFun(dwHandle,0x4F10D9+1,(unsigned int)INmyPrintText); // 

	//trapFun(dwHandle,0x45261C+1,(unsigned int)INmyPrintText); // 9.31
	//trapFun(dwHandle,0x452666+1,(unsigned int)INmyPrintText); // 
	//trapFun(dwHandle,0x45A993+1,(unsigned int)INmyPrintText); // 
	//trapFun(dwHandle,0x4F04AF+1,(unsigned int)INmyPrintText); // 
	//trapFun(dwHandle,0x4F1259+1,(unsigned int)INmyPrintText); // 

	trapFun(dwHandle,0x45456C+1,(unsigned int)INmyPrintText); // 9.40
	trapFun(dwHandle,0x4545B6+1,(unsigned int)INmyPrintText); // 
	trapFun(dwHandle,0x45C953+1,(unsigned int)INmyPrintText); // 
	trapFun(dwHandle,0x4F9D6F+1,(unsigned int)INmyPrintText); // 
	trapFun(dwHandle,0x4FAB19+1,(unsigned int)INmyPrintText); // 

	// lookup: find string In(FontNumber,1 [6th match is in the middle of the function]
	
	//trapFun(dwHandle,0x42700A+1,(unsigned int)myPlayerNameText); // 8.74
	//trapFun(dwHandle,0x4B47B1+1,(unsigned int)myPlayerNameText); // 8.74
	//trapFun(dwHandle,0x4B49AC+1,(unsigned int)myPlayerNameText); // 8.74
	//trapFun(dwHandle,0x4B4BA2+1,(unsigned int)myPlayerNameText); // 8.74
	//trapFun(dwHandle,0x4B4DA2+1,(unsigned int)myPlayerNameText); // 8.74
	//trapFun(dwHandle,0x4B4F9C+1,(unsigned int)myPlayerNameText); // 8.74
	//trapFun(dwHandle,0x4B5191+1,(unsigned int)myPlayerNameText); // 8.74
	//trapFun(dwHandle,0x4F5DBA+1,(unsigned int)myPlayerNameText); // 8.74

	/*
	trapFun(dwHandle,0x42716A+1,(unsigned int)myPlayerNameText); // 9.00
	trapFun(dwHandle,0x4B4B01+1,(unsigned int)myPlayerNameText); // 9.00
	trapFun(dwHandle,0x4B4CFC+1,(unsigned int)myPlayerNameText); // 9.00
	trapFun(dwHandle,0x4B4EF2+1,(unsigned int)myPlayerNameText); // 9.00
	trapFun(dwHandle,0x4B50F2+1,(unsigned int)myPlayerNameText); // 9.00
	trapFun(dwHandle,0x4B52EC+1,(unsigned int)myPlayerNameText); // 9.00
	trapFun(dwHandle,0x4B54E1+1,(unsigned int)myPlayerNameText); // 9.00
	trapFun(dwHandle,0x4F693A+1,(unsigned int)myPlayerNameText); // 9.00
	*/
	
	/*
	trapFun(dwHandle,0x41E323+1,(unsigned int)INmyPlayerNameText); // 9.10
	trapFun(dwHandle,0x4B6A9E+1,(unsigned int)INmyPlayerNameText); // 9.10
	trapFun(dwHandle,0x4B6CA4+1,(unsigned int)INmyPlayerNameText); // 9.10
	trapFun(dwHandle,0x4B6EA1+1,(unsigned int)INmyPlayerNameText); // 9.10
	trapFun(dwHandle,0x4B70A1+1,(unsigned int)INmyPlayerNameText); // 9.10
	trapFun(dwHandle,0x4B72A4+1,(unsigned int)INmyPlayerNameText); // 9.10
	trapFun(dwHandle,0x4B74A0+1,(unsigned int)INmyPlayerNameText); // 9.10
	trapFun(dwHandle,0x4F0A1E+1,(unsigned int)INmyPlayerNameText); // 9.10
	*/

	//trapFun(dwHandle,0x41E333+1,(unsigned int)INmyPlayerNameText); // 9.20
	//trapFun(dwHandle,0x4B6B8E+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B6D94+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B6F91+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B7191+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B7394+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B7590+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4F0C3E+1,(unsigned int)INmyPlayerNameText); // 

	//trapFun(dwHandle,0x41E4D3+1,(unsigned int)INmyPlayerNameText); // 9.31
	//trapFun(dwHandle,0x4B6D0E+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B6F14+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B7111+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B7311+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B7514+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4B7710+1,(unsigned int)INmyPlayerNameText); // 
	//trapFun(dwHandle,0x4F0DBE+1,(unsigned int)INmyPlayerNameText); // 

	trapFun(dwHandle,0x4203C3+1,(unsigned int)INmyPlayerNameText); // 9.40
	trapFun(dwHandle,0x4BFC4E+1,(unsigned int)INmyPlayerNameText); // 
	trapFun(dwHandle,0x4BFE54+1,(unsigned int)INmyPlayerNameText); // 
	trapFun(dwHandle,0x4C0051+1,(unsigned int)INmyPlayerNameText); // 
	trapFun(dwHandle,0x4C0251+1,(unsigned int)INmyPlayerNameText); // 
	trapFun(dwHandle,0x4C0454+1,(unsigned int)INmyPlayerNameText); // 
	trapFun(dwHandle,0x4C0650+1,(unsigned int)INmyPlayerNameText); // 
	trapFun(dwHandle,0x4FA67E+1,(unsigned int)INmyPlayerNameText); // 

	// lookup: TALK_INFO_MESSAGE; this is inside of the function
	//trapFun(dwHandle,0x413A69+1,(unsigned int)myInterceptInfoMiddleScreen); // 8.74
	//trapFun(dwHandle,0x4A4A63+1,(unsigned int)myInterceptInfoMiddleScreen); // 8.74
	//trapFun(dwHandle,0x4EE5A8+1,(unsigned int)myInterceptInfoMiddleScreen); // 8.74

	//trapFun(dwHandle,0x413A39+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.00
	//trapFun(dwHandle,0x4A4DB3+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.00
	//trapFun(dwHandle,0x4EF128+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.00

	//trapFun(dwHandle,0x4163EB+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.10
	//trapFun(dwHandle,0x48C6B5+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.10
	//trapFun(dwHandle,0x4EE064+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.10

	//trapFun(dwHandle,0x4163EB+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.20
	//trapFun(dwHandle,0x48C765+1,(unsigned int)myInterceptInfoMiddleScreen); // 
	//trapFun(dwHandle,0x4EE284+1,(unsigned int)myInterceptInfoMiddleScreen); // 

	//trapFun(dwHandle,0x4165CB+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.31
	//trapFun(dwHandle,0x4570ED+1,(unsigned int)myInterceptInfoMiddleScreen); // 
	//trapFun(dwHandle,0x48C915+1,(unsigned int)myInterceptInfoMiddleScreen); // 
	//trapFun(dwHandle,0x4EE3F4+1,(unsigned int)myInterceptInfoMiddleScreen); // 

	trapFun(dwHandle,0x417698+1,(unsigned int)myInterceptInfoMiddleScreen); // 9.40
	trapFun(dwHandle,0x45904D+1,(unsigned int)myInterceptInfoMiddleScreen); // 
	trapFun(dwHandle,0x490D85+1,(unsigned int)myInterceptInfoMiddleScreen); // 
	trapFun(dwHandle,0x4F7CC4+1,(unsigned int)myInterceptInfoMiddleScreen); // 

	// lookup: TargetBuffer!=NULL; first call is in the middle of the infomessage function
	//         in fact you need to increcept all calls
	

	// BLOCK is 8.74
	/*
	trapFun(dwHandle,0x4136C9+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x42A4FE+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x42A532+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x42D434+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x498154+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x4DB432+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x4DBC00+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x4DBC69+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x54F98E+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x54F9C4+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x54FACF+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x54FD00+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x550256+1,(unsigned int)myInterceptInfoMessageBox);
	*/

	// BLOCK is 9.00
	/*
	trapFun(dwHandle,0x413699+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x42A65E+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x42A692+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x42D594+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x4984E4+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x4DB7E2+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x4DBFB0+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x4DC019+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x550C6E+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x550CA4+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x550DAF+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x550FE0+1,(unsigned int)myInterceptInfoMessageBox);
	trapFun(dwHandle,0x551536+1,(unsigned int)myInterceptInfoMessageBox);
	*/

	// BLOCK is 9.10
	/*
	trapFun(dwHandle,0x415FBA+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42B738+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42BB4C+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42BB88+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4A4351+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E633B+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E6B0D+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E6B71+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x54F9C7+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x55036B+1,(unsigned int)INmyInterceptInfoMessageBox);//BADBAD
	trapFun(dwHandle,0x55043C+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x550531+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x550770+1,(unsigned int)INmyInterceptInfoMessageBox);
	*/

	// BLOCK is 9.20
	/*
	trapFun(dwHandle,0x415FBA+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42B7E8+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42BBFC+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42BC38+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4A4421+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E655B+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E6D2D+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E6D91+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x54FB47+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x5504EB+1,(unsigned int)INmyInterceptInfoMessageBox);//BADBAD
	trapFun(dwHandle,0x5505BC+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x5506B1+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x5508F0+1,(unsigned int)INmyInterceptInfoMessageBox);
	*/

	// BLOCK is 9.31
	/*
	trapFun(dwHandle,0x41619A+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42B8C8+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4A4571+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E66CB+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E6E9D+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4E6F01+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x54FEA7+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x55084B+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x55091C+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x550A11+1,(unsigned int)INmyInterceptInfoMessageBox);//BADBAD
	trapFun(dwHandle,0x550C50+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x5516D6+1,(unsigned int)INmyInterceptInfoMessageBox);
	*/

	// BLOCK is 9.40
	trapFun(dwHandle,0x4171FA+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x42D7E8+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4A9171+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4EFF7B+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4F074D+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x4F07B1+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x560F97+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x56193B+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x561A0C+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x561B01+1,(unsigned int)INmyInterceptInfoMessageBox);//BADBAD
	trapFun(dwHandle,0x561D40+1,(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,0x5627D6+1,(unsigned int)INmyInterceptInfoMessageBox);

	// lookup: manually match around previous address by assembly similarity
	//trapFun(dwHandle,0x55611D+1,(unsigned int)myInterceptEncrypt); // 8.74
	//trapFun(dwHandle,0x5573FD+1,(unsigned int)myInterceptEncrypt); // 9.00
	//trapFun(dwHandle,0x4FB7BA+1,(unsigned int)INmyInterceptEncrypt); // 9.10
	//trapFun(dwHandle,0x4FB9DA+1,(unsigned int)INmyInterceptEncrypt); // 9.20
	//trapFun(dwHandle,0x4FBB5A+1,(unsigned int)INmyInterceptEncrypt); // 9.31
	trapFun(dwHandle,0x50543A+1,(unsigned int)INmyInterceptEncrypt); // 9.40
		
	// lookup: function below encrypt is the decrypt function
	//trapFun(dwHandle,0x55613D+1,(unsigned int)myInterceptDecrypt); // 8.74
	//trapFun(dwHandle,0x55741D+1,(unsigned int)myInterceptDecrypt); // 9.00
	//trapFun(dwHandle,0x4FBDB4+1,(unsigned int)INmyInterceptDecrypt); // 9.10
	//trapFun(dwHandle,0x4FBFD4+1,(unsigned int)INmyInterceptDecrypt); // 9.20
	//trapFun(dwHandle,0x4FC154+1,(unsigned int)INmyInterceptDecrypt); // 9.31
	trapFun(dwHandle,0x505A34+1,(unsigned int)INmyInterceptDecrypt); // 9.40

	
	// WARNING: decrypt function is not trapped since 8.22 as I did not track it down in the soureccode
	
	// lookup: reference to  string "Creature!=NULL" 
	//         [you need to look for PUSH string with this debug] + 
	//         instruction before PUSH XXX must be MOV ESI, 00000000Fh
	//         it's in the middle of the function
	//         trap of the second (last one) reference to the function	
	//trapFun(dwHandle,0x4F243E+1,(unsigned int)myIsCreatureVisible); // 8.74
	//trapFun(dwHandle,0x4F2FBE+1,(unsigned int)myIsCreatureVisible); // 9.00
	//trapFun(dwHandle,0x4F309A+1,(unsigned int)myIsCreatureVisible); // 9.10
//	trapFun(dwHandle,0x4F32BA+1,(unsigned int)myIsCreatureVisible); // 9.20

	// lookup: reference to  string "unknown packet type during login" 
	//         it's about 65 instuctions before it and 300 bytes
	//         look for CMP ESI,-1 since the function returns -1 when there are no packets to parse
	//trapFun(dwHandle,0x45D565+1,(unsigned int)myShouldParseRecv); // 8.74
	//trapFun(dwHandle,0x45D775+1,(unsigned int)myShouldParseRecv); // 9.00
	//trapFun(dwHandle,0x45D230+1,(unsigned int)myShouldParseRecv); // 9.10
	//trapFun(dwHandle,0x45D320+1,(unsigned int)myShouldParseRecv); // 9.20
	//trapFun(dwHandle,0x45D430+1,(unsigned int)myShouldParseRecv); // 9.31
	trapFun(dwHandle,0x45F3F0+1,(unsigned int)myShouldParseRecv); // 9.40

    CloseHandle(dwHandle);
	
}


void InitialiseProxyClasses()
{
	CMemReaderProxy reader;
	reader.setProcessId(GetCurrentProcessId());
}

WNDPROC wndProcOriginal = NULL;

LRESULT APIENTRY FilterProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
				AfxMessageBox("I did it!");
if (uMsg == WM_COMMAND)
	{
		if (LOWORD(wParam) == 4001)
		{
			AfxMessageBox("I did it!");
		}
	}

	return CallWindowProc(wndProcOriginal, hwnd, uMsg, wParam, lParam);
}

void GetDebugPrivs()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tp;

	if (::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if ( !::LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
		{
			::CloseHandle( hToken );
		}

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = sedebugnameValue;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if ( !::AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(tp), NULL, NULL ) )
		{
			::CloseHandle( hToken );
		}

		::CloseHandle( hToken );
	}
}

BOOL CALLBACK EnumWindowsProc(      
							  
							  HWND hwnd,
							  LPARAM lParam
							  )
{
	
	DWORD procId;
	GetWindowThreadProcessId(hwnd,&procId);
	if (procId==::GetCurrentProcessId())		
	{
		
		CWnd *wnd = new CWnd();
		wnd->Attach(hwnd);
		
		
		
		CMenu *subMenu1 = new CMenu();
		subMenu1->CreatePopupMenu();
		subMenu1->AppendMenu(MF_STRING,4001,"sub 1");
		subMenu1->AppendMenu(MF_STRING,4002,"sub 2");
		subMenu1->AppendMenu(MF_STRING|MF_ENABLED,4003,"Sub 3");
		
		CMenu *subMenu2 = new CMenu();
		subMenu2->CreatePopupMenu();
		subMenu2->AppendMenu(MF_STRING,4010,"sub 10");
		subMenu2->AppendMenu(MF_STRING,4011,"sub 11");
		subMenu2->EnableMenuItem(4011,1);
		
		
		CMyMenu *menu = new CMyMenu();	
		menu->CreateMenu();		
		menu->AppendMenu(MF_STRING,4013,"test menu 1");		
		menu->AppendMenu(MF_STRING,4015,"test menu 2");
		menu->AppendMenu(MF_POPUP|MF_DISABLED,(int)subMenu1->GetSafeHmenu(),"test menu 3");		
		menu->AppendMenu(MF_POPUP,(int)subMenu2->GetSafeHmenu(),"test menu 4");
		//menu->LoadOwnerDrawMenu(menu);
		

		wnd->SetMenu(menu);
		wndProcOriginal = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)(WNDPROC)FilterProc);
		if(wndProcOriginal)
			AfxMessageBox("Window Procedure hijacked!");
		//GetDebugPrivs();
		/*
		char b[128];
		sprintf(b,"%d///%x/%x/%x|||%d",boo,menu,wnd,hwnd,lastErr);
		AfxMessageBox(b);
		*/
		
		//wnd->Detach(); 
		delete wnd;
	}
	return 1;
}

void InitialiseTibiaMenu()
{	
	EnumWindows(&EnumWindowsProc,NULL);
	
	
}






BOOL APIENTRY DllMain( HINSTANCE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{	
	
	static HINSTANCE hinstDLL = hModule;
	
	
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:						
		
		InitialiseDebugFile();
		InitialiseTibiaState();
		InitialiseHooks();
		//InitialiseKBHooks();
		InitialiseCommunication();
		InitialisePlayerInfoHack();
		InitialiseProxyClasses();
		InitialiseCreatureInfo();				
		//InitialiseTibiaMenu();
		ActivateHookCallback();
		
		
		break;
	case DLL_PROCESS_DETACH:
		{			
			if (hooksFile) UnmapViewOfFile(hooksFile);
		}
		
		break;		
	case DLL_THREAD_ATTACH:						
		break;
	case DLL_THREAD_DETACH:						
		break;
		
    }
	
	
    return TRUE;
}

void ParseIPCMessage(struct ipcMessage mess)
{
	
	switch (mess.messageType)
	{
	case 1:
		if (tibiaSocket!=NULL)
		{
			
			
			char buf[3];
			buf[0]=0x1;
			buf[1]=0x0;
			buf[2]=0x67;
			send(tibiaSocket,buf,3,0);
			
			break;
		};
	case 2:
		if (tibiaSocket!=NULL)
		{			
			sendBufferViaSocket(mess.payload);
			
		};
		break;
	case 3:
		if (taMessageStart!=taMessageEnd-1&&!(taMessageStart==TA_MESSAGE_QLEN-1&&taMessageEnd==0))
		{
			strcpy(taMessage[taMessageStart],mess.payload);
			taMessageStart++;
			if (taMessageStart==TA_MESSAGE_QLEN)
			{
				taMessageStart=0;
			}
		}
		break;
	case 4:
		if (debugFile&&COMPLEX)
		{
			fprintf(debugFile,"[debug] will try to connect back IPC pipe\r\n");
		}
		if (hPipeBack!=INVALID_HANDLE_VALUE)
		{
			// close back pipe
			DisconnectNamedPipe(hPipeBack);
			hPipeBack=INVALID_HANDLE_VALUE;
		}
		memcpy(&partnerProcessId,mess.payload,sizeof(int));
		InitialiseIPCback();
		break;
	case 100:
		memcpy(&tibiaState.attackedCreature,mess.payload,sizeof(int));
		break;
	case 101:
		outHmmAvail=1;		
		memcpy(&tibiaState.outbufHmm,mess.payload,payloadLen(mess.payload)+2);
		break;
	case 102:
		outGfbAvail=1;
		memcpy(&tibiaState.outbufGfb,mess.payload,payloadLen(mess.payload)+2);
		break;
	case 103:
		outSdAvail=1;
		memcpy(&tibiaState.outbufSd,mess.payload,payloadLen(mess.payload)+2);
		break;
	case 104:
		outExploAvail=1;
		memcpy(&tibiaState.outbufExplo,mess.payload,payloadLen(mess.payload)+2);		
		break;
	case 105:		
		outSelfUHAvail=1;
		memcpy(&tibiaState.outbufSelfUH,mess.payload,payloadLen(mess.payload)+2);		
		break;
	case 106:
		outFluidManaAvail=1;
		memcpy(&tibiaState.outbufFluidMana,mess.payload,payloadLen(mess.payload)+2);		
		break;
	case 107:
		outFluidLifeAvail=1;
		memcpy(&tibiaState.outbufFluidLife,mess.payload,payloadLen(mess.payload)+2);		
		break;
	case 201:
		revealFish=1;
		break; 
	case 202:
		revealFish=0;
		break;
	case 301:
		{
			int found=0;
			int i;
			for (i=0;i<MAX_CREATUREINFO;i++)
			{
				if (!strcmp(creatureInfoPlayerName[i],mess.payload))
				{
					found=0;
					memcpy(creatureInfoPlayerInfo1[i],mess.payload+32,500);
					memcpy(creatureInfoPlayerInfo2[i],mess.payload+32+500,500);
					break;
				}
			}
			if (!found)
			{
				memcpy(creatureInfoPlayerName[creatureInfoNext],mess.payload,32);
				memcpy(creatureInfoPlayerInfo1[creatureInfoNext],mess.payload+32,500);
				memcpy(creatureInfoPlayerInfo2[creatureInfoNext],mess.payload+32+500,500);			
				creatureInfoNext++;
				// roll creatureInfoText if we are out of limit
				if (creatureInfoNext==MAX_CREATUREINFO)
					creatureInfoNext=0;
			}
		}
		break;
	case 302:
		{
			int tmpignoreLookEnd=0;
			memcpy(&tmpignoreLookEnd,mess.payload,4);
			ignoreLookEnd=tmpignoreLookEnd;
			break;
		}
	case 303:
		{
			memcpy(&autoAimActive,mess.payload,4);
			memcpy(&autoAimOnlyCreatures,mess.payload+4,4);
			memcpy(&autoAimAimPlayersFromBattle,mess.payload+8,4);
			/*
			* as of tibia 7.8 this is obsolete (handled by the client itself
			
			  if (autoAimAimPlayersFromBattle)
			  {								
			  unsigned char val=0xEB;
			  unsigned char *addr=(unsigned char *)0x42BBAB+0xF430+0x30; //7.72
			  DWORD procId=GetCurrentProcessId();
			  HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
			  WriteProcessMemory(dwHandle, (void *)addr, &val,   sizeof(char), NULL);				
			  CloseHandle(dwHandle);
			  } else {
			  unsigned char val=0x74;
			  //unsigned char *addr=(unsigned char *)0x411073; // 7.6
			  unsigned char *addr=(unsigned char *)0x42BBAB+0xF430+0x30; //7.72
			  DWORD procId=GetCurrentProcessId();
			  HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
			  WriteProcessMemory(dwHandle, (void *)addr, &val,   sizeof(char), NULL);				
			  CloseHandle(dwHandle);
			  }
			*/
			break;
		}
	case 304:
		{
			InitialiseCreatureInfo();
			break;
		}
	case 305:
		{
			revealCNameActive=1;
			break;
		};
	case 306:
		{
			revealCNameActive=0;
			break;
		}	
	case 307: {
		/*CString buf;
		buf.Format("(%d, %d) rgb(%d, %d, %d) '%s'", mess.data[0], mess.data[1], mess.data[2], mess.data[3], mess.data[4], mess.payload);
		AfxMessageBox(buf);*/
		int x, y, red, green, blue, messLen;
		char message[1000];
		memcpy(&x, mess.payload, sizeof(int));
		memcpy(&y, mess.payload + 4, sizeof(int));
		memcpy(&red, mess.payload + 8, sizeof(int));
		memcpy(&green, mess.payload + 12, sizeof(int));
		memcpy(&blue, mess.payload + 16, sizeof(int));
		memcpy(&messLen, mess.payload + 20, sizeof(int));
		memcpy(&message, mess.payload + 24, messLen+1);

		bool actionComplete = false;
		int found = -1;
		for (int loop = 0; loop < 100; loop++) {
			//buf.Format("(%d, %d), %d", HUDisplay[loop].pos.x, HUDisplay[loop].pos.y, found);
			//AfxMessageBox(buf);
			if (HUDisplay[loop].pos.x == x && HUDisplay[loop].pos.y == y) {
				if (message != "") {// Update message on screen at point (mess.data[0], mess.data[1]):
					HUDisplay[loop].redColor = red;
					HUDisplay[loop].greenColor = green;
					HUDisplay[loop].blueColor = blue;
					memcpy(HUDisplay[loop].message, message, messLen);
					//AfxMessageBox("update");
					actionComplete = true;
					break;
				}
				else {// "Delete" message from screen
					HUDisplay[loop].pos.x = 0;
					HUDisplay[loop].pos.y = 0;
					HUDisplay[loop].redColor = 0;
					HUDisplay[loop].greenColor = 0;
					HUDisplay[loop].blueColor = 0;
					HUDisplay[loop].message[0] = '\0';
					//AfxMessageBox("delete");
					actionComplete = true;
					break;
				}
			}
			else if  (!HUDisplay[loop].pos.x && !HUDisplay[loop].pos.y && found < 0) 
				found = loop;
		}
		if (!actionComplete && found > -1) { // Add a message to the screen if there is room to store it and no delete or update action took place
			HUDisplay[found].pos.x = x;
			HUDisplay[found].pos.y = y;
			HUDisplay[found].redColor = red;
			HUDisplay[found].greenColor = green;
			HUDisplay[found].blueColor = blue;
			memcpy(HUDisplay[loop].message, message, messLen);
			//AfxMessageBox("add");
		}
		break;
		}
	default:		
		break;
	};
};

int ReadFromPipe()
{

	BOOL fSuccess=false;
	do 
	{ 
		// Read from the pipe.
		struct ipcMessage inBuf;
		DWORD lpBytesRead;
		inBuf.messageType=0;
		
		fSuccess = ReadFile(
			hPipe,    // pipe handle
			&inBuf,    // buffer to receive reply
			sizeof(struct ipcMessage),      // size of buffer
			&lpBytesRead,  // number of bytes read
			NULL);    // not overlapped
		
		int err=GetLastError();
		
		if (!fSuccess&&err==ERROR_BROKEN_PIPE)
			return 0;
		if (!fSuccess&&err==ERROR_PIPE_NOT_CONNECTED)
			return 0;

		if (!fSuccess&&err!=ERROR_MORE_DATA){
			char buf[128];
			sprintf(buf,"success=%d read=%d err=%d type=%d",fSuccess,lpBytesRead,err,inBuf.messageType);	
			MessageBox(NULL,buf,"DEBUG",0);
		}
		
		ParseIPCMessage(inBuf);
		
		// err = 234, 231, 231, 231, 2, 2, 2
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA
	return 1;
}



DWORD WINAPI CommunicationThread( LPVOID lpParam ) 
{
	InitialiseIPC();
	for (;;)
	{
		if (!ReadFromPipe())
		{
			// communcation closed - reestablish communication pipe
			InitialiseIPC();
		}
		Sleep(10);
	}; 
}

void InitialiseCommunication()
{
	DWORD dwThread;
	
	HANDLE hThread = CreateThread( 
		NULL,              // default security attributes
		0,                 // use default stack size  
		CommunicationThread,        // thread function 
		NULL,             // argument to thread function 
		0,                 // use default creation flags 
		&dwThread);   // returns the thread identifier 
}; 

/*

  
	
	  #define INCLUDE_PROXY_CLASSES
	  #include "ddraw.h"
	  #include "NewIDirectDrawSurface4.cpp"
	  #include "NewIDirectDraw4.cpp"
	  #include "NewIDirectDraw.cpp"
	  
		
		  extern "C" 
		  {
		  
			typedef HRESULT (WINAPI *RealDirectDrawCreate)(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
			
			  
				HRESULT WINAPI DirectDrawCreate(
				GUID FAR *lpGUID, 
				LPDIRECTDRAW FAR *lplpDD, 
				IUnknown FAR *pUnkOuter)
				{
				
				  
					RealDirectDrawCreate pDirectDrawCreate;
					
					  HMODULE hddraw=NULL;
					  char szFileName[512];
					  UINT ret;
					  
						ret = GetSystemDirectory(szFileName, 256);
						if(ret <= 0)
						{
						return FALSE;
						}
						
						  strcat(szFileName, "\\ddraw.dll");
						  hddraw = LoadLibrary(szFileName);
						  if(hddraw == NULL)
						  {
						  return FALSE;
						  }
						  
							pDirectDrawCreate = (RealDirectDrawCreate)GetProcAddress(hddraw, "DirectDrawCreate");
							
							  if(pDirectDrawCreate == NULL)
							  {
							  return FALSE;
							  } 
							  IDirectDraw *realIDirectDraw;
							  pDirectDrawCreate(lpGUID,&realIDirectDraw,pUnkOuter);		
							  *lplpDD = new NewIDirectDraw(realIDirectDraw,"IDirectDraw");		
							  
								return FALSE;
								
								  }
								  } // extern "C"
								  
									
									  
										extern "C" 
										{
										
										  typedef PVOID (WINAPI *RealAllocMemEx)(DWORD dwSize, HANDLE hProcess);
										  typedef BOOL (WINAPI *RealFreeMemEx)(PVOID  pMem, HANDLE hProcess);
										  typedef BOOL (WINAPI *RealHookAPI)(LPCSTR pszModule,LPCSTR pszFuncName,PVOID  pCallbackFunc,PVOID  *pNextHook,DWORD  dwFlags);
										  typedef HANDLE (WINAPI *RealCreateRemoteThreadEx)(HANDLE hProcess,LPSECURITY_ATTRIBUTES  lpThreadAttributes,DWORD dwStackSize,LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter,DWORD dwCreationFlags,LPDWORD lpThreadId);
										  
											HANDLE WINAPI CreateRemoteThreadEx(
											HANDLE                 hProcess,
											LPSECURITY_ATTRIBUTES  lpThreadAttributes,
											DWORD                  dwStackSize,
											LPTHREAD_START_ROUTINE lpStartAddress,
											LPVOID                 lpParameter,
											DWORD                  dwCreationFlags,
											LPDWORD                lpThreadId
											)
											{
											MessageBox(NULL,"CreateRemoteThreadEx","",0);
											RealCreateRemoteThreadEx fun;
											fun = (RealCreateRemoteThreadEx)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "CreateRemoteThreadEx");		
											return fun(hProcess,lpThreadAttributes,dwStackSize,lpStartAddress,lpParameter,dwCreationFlags,lpThreadId);				
											}
											
											  
												BOOL WINAPI HookAPI(
												LPCSTR pszModule,
												LPCSTR pszFuncName,
												PVOID  pCallbackFunc,
												PVOID  *pNextHook,  
												DWORD  dwFlags  
												)
												{
												char b[1024];
												sprintf(b,"HookApi: %s/%s",pszModule,pszFuncName);
												MessageBox(NULL,b,"",0);
												RealHookAPI fun;
												fun = (RealHookAPI)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "HookAPI");		
												return fun(pszModule,pszFuncName,pCallbackFunc,pNextHook,dwFlags);
												}
												
												  
													
													  PVOID WINAPI AllocMemEx(
													  DWORD  dwSize,  
													  HANDLE hProcess  
													  )
													  {
													  MessageBox(NULL,"AllocMemEx","",0);
													  RealAllocMemEx fun;
													  fun = (RealAllocMemEx)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "AllocMemEx");		
													  return fun(dwSize,hProcess);				
													  }
													  BOOL WINAPI FreeMemEx(
													  PVOID  pMem,  
													  HANDLE hProcess
													  
														)
														{
														MessageBox(NULL,"FreeMemEx","",0);
														RealFreeMemEx fun;
														fun = (RealFreeMemEx)GetProcAddress(LoadLibrary("madCHookOrig.dll"), "FreeMemEx");		
														return fun(pMem,hProcess);				
														}
														
														  BOOL WINAPI HookCode(
														  PVOID  pCode,
														  PVOID  pCallbackFunc,
														  PVOID  *pNextHook,
														  DWORD  dwFlags
														  )
														  {
														  MessageBox(NULL,"HookCode","",0);
														  return 0;
														  }
														  
															
															  } // extern "C"
*/

