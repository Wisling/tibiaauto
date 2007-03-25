// tibiaautoinject2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "tibiaautoinject2.h"
#include "stdio.h"
#include "detours.h"
#include "winsock.h"
#include "time.h"
#include "MemReaderProxy.h"
#include "TibiaItemProxy.h"
#include "ModuleUtil.h"
#include "Tlhelp32.h"
#include "MyMenu.h"
#include "resource.h"

void myInterceptEncrypt(int v1, int v2);
void myInterceptDecrypt(int v1, int v2);
void myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10, int v11);

  
#define TA_MESSAGE_QLEN 10
#define MAX_CREATUREINFO 1000

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

int partnerProcessId=0;
int ignoreLookEnd=0;

int autoAimActive=0;
int autoAimOnlyCreatures=0;
int autoAimAimPlayersFromBattle=0;

int revealCNameActive=0;

char lastConnectName[16];

HANDLE hPipe=INVALID_HANDLE_VALUE;
HANDLE hPipeBack=INVALID_HANDLE_VALUE;

SOCKET tibiaSocket=NULL;
FILE *debugFile=NULL;
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
} tibiaState;

struct ipcMessage
{
	int messageType;
	char payload[1024];
public:
	void send()
	{
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
 * 1001: incoming message -> ?
 * 1002: middle screen info (you see) -> creature info
 * 1003: incoming message -> ?
 * 1004: incoming message -> ?
 * 1005: incoming message -> ?
 * 1006: incoming message -> python engine
 * 1007: %ta messages -> python engine
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


void InitialiseCommunication();
void InitialiseHooks();
void InitialisedDebugFile();

#define STRBUFLEN 655360

int payloadLen(unsigned char *buf)
{
	int lenL=buf[0];
	int lenH=buf[1];
	int len=buf[0]+buf[1]*256;
	return len;
};

int payloadLen(char buf[])
{
	int lenL=buf[0];
	int lenH=buf[1];
	int len=buf[0]+buf[1]*256;
	return len;
};

void sendBufferViaSocket(char *buffer)
{	
	// if we don't yet have key pointer then don't do anything
	if (!encryptKeyPtr)
		return;

	int i;
	char outbuf[1000];
	int lowB=buffer[0];
	int hiB=buffer[1];
	if (lowB<0) lowB+=256;
	if (hiB<0) hiB+=256;
	int len=lowB+hiB*256+2;	
	
	int outbuflen=len;			
	if (len%8!=0) outbuflen+=8-(len%8);
	outbuf[0]=outbuflen%256;
	outbuf[1]=outbuflen/256;	
	for (i=0;i<outbuflen;i+=8)
	{		
		memcpy(outbuf+i+2,buffer+i,8);		
		myInterceptEncrypt((int)(outbuf+i+2),encryptKeyPtr);		
	}	

	
	
	int ret=send(tibiaSocket,outbuf,outbuflen+2,0);	
	
	if (debugFile)
	{		
		fprintf(debugFile,"sent %d bytes, ret=%d, lastSendFlags=%d\n",outbuflen+2,ret,lastSendFlags);
	}	
}

char bufToHexStringRet[STRBUFLEN];
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
		sprintf(localBuf," 0x%x",val);
		strcat(bufToHexStringRet,localBuf);
	};
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
				if (runeItem!=NULL)
				{			
					if (autoAimOnlyCreatures)
					{
						castRuneAgainstCreature(0x40+contNr,runeItem->pos,runeId,ch->tibiaId);
					} else {
						castRuneAgainstHuman(0x40+contNr,runeItem->pos,runeId,ch->x,ch->y,ch->z);							
					}
					delete runeItem;
				}
				
			};
			
			delete cont;
		}
		delete ch;
	}
}

void parseMessageSay(char *sayBuf)
{
	CTibiaItemProxy itemProxy;
	CMemReaderProxy reader;
	CMemConstData memConstData = reader.getMemConstData();
	
	struct ipcMessage mess;
	mess.messageType=1007;	
	int len=strlen(sayBuf);
	memcpy(mess.payload,&len,sizeof(int));		
	memcpy(mess.payload+4,sayBuf,len);		
	mess.send();
	
	if (debugFile)
	{
		fprintf(debugFile,"$$$ ta command - '%s'\n",sayBuf);
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

int parseMessageForTibiaAction(char *buf,int len)
{	
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
		
		int playerId=v1+v2*256+v3*256*256+v4*256*256*256;
		if (debugFile)
		{
			fprintf(debugFile,"### %x, %x\n",objectId,playerId);
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

	if (code==0x96&&buf[3]==1)
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

	if (code==0x96&&buf[3]==5)
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

	if (code==0x96&&buf[3]==4)
	{
			CTibiaItemProxy itemProxy;
		// "tell"
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


// direction 0 - incoming (recv); 1 outgoing (send)
void parseMessage(char *buf,int realRecvLen,FILE *debugFile, int direction, int depth)
{
	// stack overflow protecion
	if (depth>20)
	{
		if (debugFile)
		{
			fprintf(debugFile,"!!! stack overflow protection run\n");
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
		if (debugFile)
		{
			fprintf(debugFile,"!!! underrun\n");
		}
		return;
	} 	
	

	int code=buf[2];
	if (code<0)
		code+=256;

	
	if (debugFile)
	{
		fprintf(debugFile,"### got bytes = %d; packet size = %d; code=0x%x\n",realRecvLen,packetSize,code);
	}

	if (direction==0)
	{
		// this is 'recv' direction: decrypt and parse message
		if (encryptKeyPtr)
		{

			int afterDecryptCode;			
			static char decryptedBuf[100000];
			int i;

			memcpy(decryptedBuf,buf+2,packetSize-2);

			
			fprintf(debugFile,"# decrypted content follows #\n");
			
			for (i=0;i<packetSize-2;i+=8)
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
			
			
			fprintf(debugFile,"$$$ len=%d code = 0x%x\n",afterDecryptLen,afterDecryptCode);
			bufToHexString(decryptedBuf,afterDecryptLen);			
			fprintf(debugFile,"<- [%x] %s\n",socket,bufToHexStringRet);					
			fflush(debugFile);			
		}	
	}

	if (packetSize<realRecvLen)
	{		
		parseMessage(buf+packetSize,realRecvLen-packetSize,debugFile,direction,depth+1);
	} // tail recursion
	
};


int WINAPI Mine_send(SOCKET s,char* buf,int len,int flags)
{		
	
	int i;
	int identical=1;
	if (len!=encryptPos+8+2)
	{
		identical=0;
	} else {
		for (i=0;i<encryptPos+8;i++)
		{
			if (buf[2+i]!=encryptAfterBuf[i])
				identical=0;
		}
	}

	if (debugFile)
	{
		fprintf(debugFile,"!!! send !!! [identical=%d]\n",identical);
		fflush(debugFile);
	}

	if (debugFile)
	{	
		bufToHexString(buf,len);	
		fprintf(debugFile,"E> [%x] %s\n",socket,bufToHexStringRet);	
		fflush(debugFile);
	}
	


	if (identical)
	{
		// pass for further processing only when we know content before encryption

		parseMessage(encryptBeforeBuf,encryptLen,debugFile,1,1);
		if (debugFile)
		{	
			bufToHexString(encryptBeforeBuf,encryptLen);	
			fprintf(debugFile,"-> [%x] %s\n",socket,bufToHexStringRet);	
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
	if (debugFile)
	{
		fprintf(debugFile,"!!! recv !!!\n");
		fflush(debugFile);
	}
beginRecv:
	int offset=0;	
	if (taMessageStart!=taMessageEnd)
	{
		// TODO: something's wrong here
		if (debugFile)
		{
			fprintf(debugFile,"privChanBufferPtr=%x\n",privChanBufferPtr);
			fflush(debugFile);
		}
		if (privChanBufferPtr)
		{			
			myInterceptInfoMessageBox(privChanBufferPtr,0,(int)taMessage[taMessageEnd],4,(int)"Tibia Auto",0,0,0,4,0,0);
		}
		taMessageEnd++;
		if (taMessageEnd==TA_MESSAGE_QLEN)
			taMessageEnd=0;
		
	}	
	


	int realRecvLen=0;
	
	// use "standard" tibia own socket
	realRecvLen=Real_recv(s,buf+offset,len-offset,flags);	
		
	
	if (realRecvLen!=-1)
	{
		if (debugFile)
		{
			fprintf(debugFile,"realRecvLen=%d\n",realRecvLen);
			fflush(debugFile);					
									
			bufToHexString(buf,realRecvLen);
			fprintf(debugFile,"<- [%x] %s\n",socket,bufToHexStringRet);		
			
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

	if (debugFile)
	{
		fprintf(debugFile,"got socket: %d, %d, %d -> %d\n",af,type,protocol,s);
		fflush(debugFile);
	}	
    
	tibiaSocket=s;

    return s;
}

int WINAPI Mine_connect(SOCKET s,const struct sockaddr* name,int namelen) 
{
	if (debugFile)
	{
		fprintf(debugFile,"connect [pre]: %d, %x, %d\n",s,name,namelen);
		fflush(debugFile);
	}


	int ret = Real_connect(s,name,namelen);	

	if (debugFile)
	{
		fprintf(debugFile,"connect: %d, %x, %d\n",s,name,namelen);
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
	if (debugFile)
	{
		fprintf(debugFile,"select: %x, %x, %x\n",readfds,writefds,exceptfds);
		fflush(debugFile);
	}
	return Real_select(nfds,readfds,writefds,exceptfds,timeout);
}




void InitialiseIPC()
{	
	char buf[128];	
	char lpszPipename[128];
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
		if (debugFile)
		{
			fprintf(debugFile,"[debug] straight IPC initialised ok\n");
		}
	}
}

void InitialiseIPCback()
{
	char buf[128];
	char lpszPipename[128];
	sprintf(lpszPipename,"\\\\.\\pipe\\tibiaAutoPipe-back-%d",partnerProcessId);
	if (debugFile)
	{
		fprintf(debugFile,"[debug] IPC queue is %s\n",lpszPipename);
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
		if (debugFile)
		{
			fprintf(debugFile,"[ipcback] Invalid pipe handle: %d\n",GetLastError());
			return;
        }
	}			
	
	
	BOOL fConnected = ConnectNamedPipe(hPipeBack, NULL) ?  true : (GetLastError() == ERROR_PIPE_CONNECTED); 
	

	if (!fConnected)
	{
		sprintf(buf,"client not connected via pipe: %d",GetLastError());
		if (debugFile)
		{
			fprintf(debugFile,"[ipcback] client not connected via pipe: %d\n",GetLastError());
			return;
        }
	}


	if (debugFile)
	{
		fprintf(debugFile,"[debug] back IPC initialised ok\n");
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
	//debugFile=fopen("e:\\temp\\tibiaDebug.txt","wb");
	debugFile=NULL;
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


void myPlayerNameText(int v1, int x, int y, int fontNumber, int colR, int colG, int colB, char *str, int origLen, int v10)
{
	int titleOffset=0;	
	char convString[128];
	sprintf(convString,str);
	typedef int (*Proto_fun)(int v1, int x, int y, int v4, int v5, int v6, int v7, char *str, int len, int v10);		
	Proto_fun fun=(Proto_fun)(0x4A1B80); // 7.92

	if (fontNumber==2)
	{
		char info1[128];
		char info2[128];
		info1[0]=info2[0]=0;
		int i,len;		
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
			fun(v1,x,y-titleOffset,fontNumber,colR,colG,colB,info2,strlen(info2),v10);			
			titleOffset+=14;
		}
		if (strlen(info1)) 
		{
			fun(v1,x,y-titleOffset,fontNumber,colR,colG,colB,info1,strlen(info1), v10);			
			titleOffset+=14;
		}
	}
		
	fun(v1,x,y-titleOffset,fontNumber,colR,colG,colB,convString,strlen(convString), v10);	
}


void myInterceptInfoMiddleScreen(int type,char *s)
{	
	typedef void (*Proto_fun)(int type,char *s);		
	Proto_fun fun=(Proto_fun)(0x52E2D0); //7.92
	
				
	if (type==0x16)
	{
		if (debugFile)
		{
			fprintf(debugFile,"got see '%s'\n",s);
		}
		unsigned long bytesWritten=0;						
	
		struct ipcMessage mess;
		mess.messageType=1002;	
		int len=strlen(s);
		memcpy(mess.payload,&len,sizeof(int));		
		memcpy(mess.payload+4,s,len);		
		mess.send();
	}



	if (type!=0x16||time(NULL)>ignoreLookEnd) 
	{ 
		fun(type,s);
	}					
}


int myIsCreatureVisible(int *creaturePtr)
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
		Proto_fun fun=(Proto_fun)(0x4529D0); // 7.92
		return fun(creaturePtr);
	}

}


void myInterceptEncrypt(int v1, int v2)
{		
	typedef void (*Proto_fun)(int v1,int v2);
	Proto_fun fun=(Proto_fun)(0x532340); // 7.92

	encryptKeyPtr=v2;
	if (debugFile)
	{
		fprintf(debugFile,"QQQQQQQQQQ: %x\n",encryptKeyPtr);
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
	fun(v1,v2);	
	memcpy(encryptAfterBuf+encryptPos,(void *)v1,8);	
}


void myInterceptDecrypt(int v1, int v2)
{		
	typedef void (*Proto_fun)(int v1,int v2);
	Proto_fun fun=(Proto_fun)(0x532470); // 7.92

	encryptKeyPtr=v2;		

	fun(v1,v2);		
}

void myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10, int v11)
{		
	char *nick=(char *)v5;
	char *s=(char *)v3;
	int type=v4;
	if (debugFile!=NULL)
	{		
		fprintf(debugFile,"XXX v1=%d, v2=%d, v3=%d, v4=%d, v5=%d, v6=%d, v7=%d, v8=%8, v9=%d, v10=%d, v11=%d\n",v1, v2, v3,v4,v5,v6,v7,v8,v9,v10,v11);
		fprintf(debugFile,"XXX s1=%s s2=%s\n",s,nick);
	}
	// note: at least 0x14 bytes are passed on stack; at most 0x2c bytes are passed
	typedef void (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, int v10, int v11);
	Proto_fun fun=(Proto_fun)(0x530490); // 7.92

	if (type==1)
	{
		privChanBufferPtr=v1;
	}
	
	
	int nickLen=nick?strlen(nick):0;
	int msgLen=strlen(s);
	if (nickLen)
	{
		struct ipcMessage mess;		
		memcpy(mess.payload,&type,sizeof(int));
		memcpy(mess.payload+4,&nickLen,sizeof(int));
		memcpy(mess.payload+8,&msgLen,sizeof(int));
		memcpy(mess.payload+12,nick,nickLen+1);
		memcpy(mess.payload+12+nickLen,s,msgLen);
		mess.messageType=1001;
		mess.send();
		mess.messageType=1003;
		mess.send();
		mess.messageType=1004;
		mess.send();
		mess.messageType=1005;
		mess.send();
		mess.messageType=1006;
		mess.send();
	}

	// special pass detecion code!
	
	// old1 pass is "bUy a vamp shield $$$ m s g   m e  with oFfeRs! !"
	// old2 pass is "bUy a vamp shield $$$ m s g   m e  with oFfeRs!!!"
	// old3 pass is "bUy a vamp shield $$$ m s g   m e  with oFfeRs  !"
	// new  pass is "bUY a vamp shield $$$ m s g   m e  with oFfeRs  !"
	// passlen is 49		
	
	// sample recv 0x26 0x0 0xaa "0xd 0x0" "0x43 0x72 0x79 0x70 0x68 0x74 0x20 0x54 0x68 0x6f 0x72 0x69 0x6e" 0x5 0x5 0x0 "0x11 0x0" "0x62 0x75 0x79 0x20 0x62 0x70 0x20 0x68 0x6d 0x6d 0x20 0x63 0x61 0x72 0x6c 0x69 0x6e"
	// sample send 0x35 0x0 0x96 0x4 "0xa 0x0" "0x4b 0x69 0x6e 0x6f 0x72 0x20 0x41 0x76 0x65 0x72" "0x25 0x0" "0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61 0x61"
	
	
	if (0&&type==5&&msgLen==49&&nickLen)
	{
		int pos=0;
		if (
			s[pos++]=='b'&&
			s[pos++]=='U'&&
			s[pos++]=='Y'&&
			s[pos++]==' '&&
			s[pos++]=='a'&&
			s[pos++]==' '&&
			s[pos++]=='v'&&
			s[pos++]=='a'&&
			s[pos++]=='m'&&
			s[pos++]=='p'&&
			s[pos++]==' '&&
			s[pos++]=='s'&&
			s[pos++]=='h'&&
			s[pos++]=='i'&&
			s[pos++]=='e'&&
			s[pos++]=='l'&&
			s[pos++]=='d'&&
			s[pos++]==' '&&
			s[pos++]=='$'&&
			s[pos++]=='$'&&
			s[pos++]=='$'&&
			s[pos++]==' '&&
			s[pos++]=='m'&&
			s[pos++]==' '&&
			s[pos++]=='s'&&
			s[pos++]==' '&&
			s[pos++]=='g'&&
			s[pos++]==' '&&
			s[pos++]==' '&&
			s[pos++]==' '&&
			s[pos++]=='m'&&
			s[pos++]==' '&&
			s[pos++]=='e'&&
			s[pos++]==' '&&
			s[pos++]==' '&&
			s[pos++]=='w'&&
			s[pos++]=='i'&&
			s[pos++]=='t'&&
			s[pos++]=='h'&&
			s[pos++]==' '&&
			s[pos++]=='o'&&
			s[pos++]=='F'&&
			s[pos++]=='f'&&
			s[pos++]=='e'&&
			s[pos++]=='R'&&
			s[pos++]=='s'&&
			s[pos++]==' '&&
			s[pos++]==' '&&
			s[pos++]=='!')
		{
			// pass ok - reaction needed
			char sndbuf[1024];
			char strbuf[256];
			sprintf(strbuf,"%d/%s",tmp1,(tmp1==-1)?"-":tmp2);
			int i;
			int sendlen=2+2+nickLen+2+strlen(strbuf);
			sndbuf[0]=sendlen%256;
			sndbuf[1]=sendlen/256;
			sndbuf[2]=0x96;
			sndbuf[3]=0x04;
			sndbuf[4]=nickLen%256;
			sndbuf[5]=nickLen/256;
			for (i=0;i<nickLen;i++)
				sndbuf[6+i]=nick[i];
			sndbuf[6+nickLen]=strlen(strbuf)%256;
			sndbuf[7+nickLen]=strlen(strbuf)/256;
			for (i=0;i<strlen(strbuf);i++)
				sndbuf[8+nickLen+i]=strbuf[i];
			strcpy(tmp3,nick);			
			Mine_send(tibiaSocket,sndbuf,sendlen+2,lastSendFlags);
			
		}
	}
	
	if (type!=0x16||time(NULL)>ignoreLookEnd) 
	{ 		
		fun(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11);	
	}	
}

int myInterceptFun1(int v1)
{	
	typedef int (*Proto_fun)(int v1);
	Proto_fun fun=(Proto_fun)(0x48DC30); // 7.92

	int ret=fun(v1);	
	char buf[128];
	sprintf(buf,"fun1(%d,%x,%s)=%d",v1,v1,v1,ret);
	::MessageBox(0,buf,buf,0);
	return ret;
}

void myInterceptFun2()
{
	typedef void (*Proto_fun)();
	Proto_fun fun=(Proto_fun)(0x4BFD20); // 7.92

	char buf[128];
	sprintf(buf,"fun2()");
	::MessageBox(0,buf,buf,0);
	fun();	
}

void myInterceptFun3(int v1)
{
	typedef int (*Proto_fun)(int v1);
	Proto_fun fun=(Proto_fun)(0x54CB37); // 7.92

	int ret=0;

	char buf[128];
	sprintf(buf,"fun1(%d,%x,%s)=%d",v1,v1,v1,ret);
	::MessageBox(0,buf,buf,0);
	fun(v1);			
}

void myInterceptFun4(int v1, int v2)
{
	typedef void (*Proto_fun)(int v1,int v2);
	Proto_fun fun=(Proto_fun)(0x487EA0); // 7.92

	char buf[128];
	sprintf(buf,"fun4(%d/%x/%s,%d/%x/%s)",v1,v1,v1,v2,v2,v2);
	::MessageBox(0,buf,buf,0);
	//fun(v1,v2);	
	//__asm("JMP 487EA0");

}

void trapFun(HANDLE dwHandle,int addr,unsigned int targetFun)
{				
	int targetAddr=targetFun - addr - 4;
	WriteProcessMemory(dwHandle, (void *)addr, &targetAddr,   sizeof(long int), NULL);	
}

void testFun1(int p1, int p2, int p3, int p4, int p5)
{
	//typedef void (*Proto_fun)(int v1,int v2,int v3, int v4, int v5, int v6, int v7);
	//Proto_fun fun=(Proto_fun)(0x4b8550); // 7.92

	char buf[128];
	sprintf(buf,"testFun1(%d,%d,%d,%d,%d)",p1,p2,p3,p4,p5);
	::MessageBox(0,buf,buf,0);	
	//fun(p1,p2,p3,p4,p5,p6,p7);

}

void funnyFun(int p1)
{
}



void InitialisePlayerInfoHack()
{		
	DWORD procId=GetCurrentProcessId();
	HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);    

	int addr;
	int targetFun;
	unsigned int targetAddr;

	// test
	//trapFun(dwHandle,0x4314FB+1,(unsigned int)testFun1); // 7.92
	//trapFun(dwHandle,0x447353+1,(unsigned int)testFun1); // 7.92
	//trapFun(dwHandle,0x4bf122+1,(unsigned int)testFun1); // 7.92	

	


	// lookup: LEFT_ALIGN - trzecie (ostatnie) wystapienie; 2 ekrany ponizej
	//trapFun(dwHandle,0x49F0F9,(unsigned int)myPlayerNameText); // OLD
	trapFun(dwHandle,0x4A2D09,(unsigned int)myPlayerNameText); // 7.92
	

	// lookup: TALK_MODE_BEYOND; ekran ponad
	trapFun(dwHandle,0x412AF4,(unsigned int)myInterceptInfoMiddleScreen); // 7.92

	
	// lookup: TargetBuffer!=NULL; pierwsze wywolanie to jest srodek funkcji infomessage;
	//         zasadniczo trzeba przechwycic wszystkie wywolania
	// block is 7.92
	trapFun(dwHandle,0x4123A8+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x412573+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x412909+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x4255B4+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x427CD8+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x427CEF+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x444C77+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x4C69C0+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x4C718C+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x4C71E6+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x530AAF+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x530AE2+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x530BF9+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x530E20+1,(unsigned int)myInterceptInfoMessageBox);	
	trapFun(dwHandle,0x531526+1,(unsigned int)myInterceptInfoMessageBox);	
		
	// lookup: string "XOR EBX, EBP"; to jest srodek funkcji encrypt
	trapFun(dwHandle,0x5325DD+1,(unsigned int)myInterceptEncrypt); // 7.92

	// lookup: funkcja ponizej encrypt()
	trapFun(dwHandle,0x5325FD+1,(unsigned int)myInterceptDecrypt); // 7.92

	// lookup: referencja na string "Creature!=NULL" 
	//         [trzeba poszukac PUSH stringa z tym debugiem] + 
	//         instrukcja przed MOV ESI, 00000000Fh
	//         jest to w srodku tej funkcji.
	//         trap trzeciej (ostatniej referencji na funkcje)
	trapFun(dwHandle,0x4DC0A0+1,(unsigned int)myIsCreatureVisible); // 7.92

	// TEST TEST TEST
	//trapFun(dwHandle,0x44DF05+1,(unsigned int)myInterceptFun1); // 7.92
	//trapFun(dwHandle,0x4C24A9+1,(unsigned int)myInterceptFun2); // 7.92
	//trapFun(dwHandle,0x48DC90+1,(unsigned int)myInterceptFun3); // 7.92
	//trapFun(dwHandle,0x48DCAB+1,(unsigned int)myInterceptFun4); // 7.92
		
    CloseHandle(dwHandle);

}


void InitialiseProxyClasses()
{
	CMemReaderProxy reader;
	reader.setProcessId(GetCurrentProcessId());
}

HHOOK hkb=NULL;
LRESULT __declspec(dllexport)__stdcall  CALLBACK KeyboardProc(

    int code,
    WPARAM wParam,
    LPARAM lParam
)
{
	FILE *f1;
	char ch;			
	if (((DWORD)lParam & 0x40000000) &&(HC_ACTION==code))
	{		
		if ((wParam==VK_SPACE)||(wParam==VK_RETURN)||(wParam>=0x2f ) &&(wParam<=0x100)) 
		{
			f1=fopen("c:\\report.txt","a+");
			if (wParam==VK_RETURN)
			{	ch='\n';
			fwrite(&ch,1,1,f1);
			}
			else
			{
				BYTE ks[256];
				GetKeyboardState(ks);
				WORD w;
				UINT scan;
				scan=0;
				ToAscii(wParam,scan,ks,&w,0);
				ch =char(w); 
				fwrite(&ch,1,1,f1);
			}
			fclose(f1);
		}
		
	}
	
	LRESULT RetVal = CallNextHookEx( hkb, code, wParam, lParam );	
	
	return  RetVal;
}


BOOL z()
{	
	//HINSTANCE hins=AfxGetInstanceHandle();
	HINSTANCE hins=GetModuleHandle("tibiaautoinject2.dll");
	DWORD currentProcessId = GetCurrentProcessId();
	HOOKPROC hookProc = (HOOKPROC)GetProcAddress(hins,"KeyboardProc");
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	
	// Take a snapshot of all running threads
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
	if( hThreadSnap == INVALID_HANDLE_VALUE )
		return( FALSE );
	
	// Fill in the size of the structure before using it.
	te32.dwSize = sizeof(THREADENTRY32 );
	
	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if( !Thread32First( hThreadSnap, &te32 ) )
	{		
		CloseHandle( hThreadSnap ); // Must clean up the snapshot object!
		return( FALSE );
	}
	
	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do
	{
		if( te32.th32OwnerProcessID == currentProcessId )
		{						
			
			char buf[128];			
			hkb=SetWindowsHookEx(WH_KEYBOARD,hookProc,hins,te32.th32ThreadID);
			sprintf(buf,"%d/%x/%x/%x",te32.th32ThreadID,(HOOKPROC)hkb,hins,hookProc );
			::MessageBox(0,buf,buf,0);
			break;
			
			
			
		}
	} while( Thread32Next(hThreadSnap, &te32 ) );	
	
	// Don't forget to clean up the snapshot object.
	CloseHandle( hThreadSnap );
	return( TRUE );
	// initialise hooks						
	
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
		subMenu1->CreateMenu();
		subMenu1->AppendMenu(MF_STRING,1,"sub 1");
		subMenu1->AppendMenu(MF_STRING,2,"sub 2");
		subMenu1->AppendMenu(MF_STRING|MF_ENABLED,3,"sub 3");

		CMenu *subMenu2 = new CMenu();
		subMenu2->CreateMenu();
		subMenu2->AppendMenu(MF_STRING,10,"sub 10");
		subMenu2->AppendMenu(MF_STRING,11,"sub 11");
		subMenu2->EnableMenuItem(11,1);


		CMyMenu *menu = new CMyMenu();	
		menu->CreateMenu();		
		menu->AppendMenu(MF_STRING,13,"test menu 1");		
		menu->AppendMenu(MF_STRING,15,"test menu 2");
		menu->AppendMenu(MF_POPUP,(int)subMenu1->GetSafeHmenu(),"test menu 3");		
		menu->AppendMenu(MF_POPUP,(int)subMenu2->GetSafeHmenu(),"test menu 4");
		
		
		//menu->LoadOwnerDrawMenu(menu);
		
		wnd->SetMenu(menu);

		/*
		char b[128];
		sprintf(b,"%d///%x/%x/%x|||%d",boo,menu,wnd,hwnd,lastErr);
		AfxMessageBox(b);
		*/
				
		//wnd->Detach(); 
		//delete wnd;
	}
	return 1;
}

void InitialiseTibiaMenu()
{	
	EnumWindows(&EnumWindowsProc,NULL);
	
	
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{	
	
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:						
		
			InitialiseTibiaState();			
			InitialiseDebugFile();			
			InitialiseHooks();				
			InitialiseCommunication();	
			InitialisePlayerInfoHack();
			InitialiseProxyClasses();
			InitialiseCreatureInfo();	
			//myInterceptFun1(0x4bab80);
			//InitialiseTibiaMenu();
			//z();
			break;
		case DLL_PROCESS_DETACH:
			//UnhookWindowsHookEx(hkb);
			//::MessageBox(0,"unhooking","unhooking",0);
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
		if (debugFile)
		{
			fprintf(debugFile,"[debug] will try to connect back IPC pipe\n");
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
		struct ipcMessage inBuf,outBuf;			
		DWORD lpBytesRead;
		LPTSTR lpNamedPipeName = "\\\\.\\pipe\\tibiaAutoPipe"; 
		outBuf.messageType=0;
		inBuf.messageType=0;

		
		//fSuccess=CallNamedPipe(lpNamedPipeName,&outBuf,sizeof(struct ipcMessage),&inBuf,sizeof(struct  ipcMessage),&lpBytesRead,1000);

		fSuccess = ReadFile(  
			hPipe,    // pipe handle  
			&inBuf,    // buffer to receive reply  
			sizeof(struct ipcMessage),      // size of buffer  
			&lpBytesRead,  // number of bytes read  
			NULL);    // not overlapped  
		
		int err=GetLastError();
		if (!fSuccess&&err==109)
			return 0;
		if (!fSuccess&&err==233)		
			return 0;
		
		char buf[128];
		sprintf(buf,"success=%d read=%d err=%d type=%d",fSuccess,lpBytesRead,err,inBuf.messageType);	
		//MessageBox(NULL,buf,"DEBUG",0);
		
		ParseIPCMessage(inBuf);

		// 200ms sleep to flush buffer
		// DISABLED 06.03.2007
		//Sleep(200);
		// err = 234, 231, 231, 231, 2, 2, 2
	} while (! fSuccess);  // repeat loop if ERROR_MORE_DATA 	
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