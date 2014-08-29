// tibiaautoinject2.cpp : Defines the entry point for the DLL application.
//
// Check MyPlayerNameText funcs

#include "stdafx.h"

#include "tibiaautoinject2.h"
#include "stdio.h"
#include "detours.h"
#include "winsock2.h"
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
#include "psapi.h"
#include "ipcm.h"

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
		handle = NULL;
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

HHOOK hook;

SOCKET tibiaSocket=NULL;
FILE *debugFile=NULL;

int COMPLEX=0;
int SENTONLY=0;

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
* Located within protocol.cpp to send messages based on incoming packets
* 1101: for poison cure; the number of hp lost from damage caused without monster "You lose x hitpoints."
* 1102: not in use.  for gathering data on damaging creatures
* 1103: detecting "There is no way" when following
* 1104: detecting loot message, if DNE then not ours, if is nothing then don't bother

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

//10.53
int funAddr_tibiaPrintText =			0x4E0180;
int funAddr_tibiaPlayerNameText =		0x4DF510;
int funAddr_tibiaInfoMiddleScreen =		0x58A280;
int funAddr_tibiaIsCreatureVisible =	0x487970;
int funAddr_tibiaEncrypt =				0x59D9A0;
int funAddr_tibiaDecrypt =				0x59DA20;
int funAddr_tibiaShouldParseRecv =		0x533F20;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0xA44A28-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x58E1F0;
int callAddr_PrintText01 =				0x46F54D;//...<addr>.*
int callAddr_PrintText02 =				0x46F592;
int callAddr_PrintText03 =				0x4785E3;
int callAddr_PrintText04 =				0x52115A;
int callAddr_PrintText05 =				0x522360;
int callAddr_PlayerNameText01 =			0x434255;
int callAddr_PlayerNameText02 =			0x4DFE08;
int callAddr_PlayerNameText03 =			0x4E0008;
int callAddr_PlayerNameText04 =			0x4E020C;
int callAddr_PlayerNameText05 =			0x4E040C;
int callAddr_PlayerNameText06 =			0x4E0608;
int callAddr_PlayerNameText07 =			0x4E080A;
int callAddr_PlayerNameText08 =			0x521E9F;
int callAddr_InfoMiddleScreen01 =		0x428A0A;
int callAddr_InfoMiddleScreen02 =		0x4743A4;
int callAddr_InfoMiddleScreen03 =		0x4AE46D;
int callAddr_InfoMiddleScreen04 =		0x51EFAB;
int callAddr_InfoMessageBox01 =			0x428324;
int callAddr_InfoMessageBox02 =			0x4412EF;
int callAddr_InfoMessageBox03 =			0x4C6FEA;
int callAddr_InfoMessageBox04 =			0x515D02;
int callAddr_InfoMessageBox05 =			0x516507;
int callAddr_InfoMessageBox06 =			0x5165AD;
int callAddr_InfoMessageBox07 =			0x58E075;
int callAddr_InfoMessageBox08 =			0x58EA1C;
int callAddr_InfoMessageBox09 =			0x58EB23;
int callAddr_InfoMessageBox10 =			0x58ED22;
int callAddr_InfoMessageBox11 =			0x58F029;
int callAddr_InfoMessageBox12 =			0x58FB24;
int callAddr_InfoMessageBox13 =			0x58FB54;
int callAddr_Encrypt01 =				0x533643;
int callAddr_Decrypt01 =				0x533C49;
int callAddr_ShouldParseRecv01 =		0x47B542;

//10.52
/*
int funAddr_tibiaPrintText =			0x4DE0F0;
int funAddr_tibiaPlayerNameText =		0x4DD480;
int funAddr_tibiaInfoMiddleScreen =		0x5877B0;
int funAddr_tibiaIsCreatureVisible =	0x485B00;
int funAddr_tibiaEncrypt =				0x59AED0;
int funAddr_tibiaDecrypt =				0x59AF50;
int funAddr_tibiaShouldParseRecv =		0x531B80;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0xA3BC28-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x58B720;
int callAddr_PrintText01 =				0x46D55D;//...<addr>.*
int callAddr_PrintText02 =				0x46D5A2;
int callAddr_PrintText03 =				0x4765F3;
int callAddr_PrintText04 =				0x51EDCA;
int callAddr_PrintText05 =				0x51FFD0;
int callAddr_PlayerNameText01 =			0x433C65;
int callAddr_PlayerNameText02 =			0x4DDD78;
int callAddr_PlayerNameText03 =			0x4DDF78;
int callAddr_PlayerNameText04 =			0x4DE17C;
int callAddr_PlayerNameText05 =			0x4DE37C;
int callAddr_PlayerNameText06 =			0x4DE578;
int callAddr_PlayerNameText07 =			0x4DE77A;
int callAddr_PlayerNameText08 =			0x51FB0F;
int callAddr_InfoMiddleScreen01 =		0x4289BA;
int callAddr_InfoMiddleScreen02 =		0x4723B4;
int callAddr_InfoMiddleScreen03 =		0x4AC60D;
int callAddr_InfoMiddleScreen04 =		0x51CC2B;
int callAddr_InfoMessageBox01 =			0x4282D4;
int callAddr_InfoMessageBox02 =			0x440CDF;
int callAddr_InfoMessageBox03 =			0x4C4F7A;
int callAddr_InfoMessageBox04 =			0x5136C2;
int callAddr_InfoMessageBox05 =			0x513EB7;
int callAddr_InfoMessageBox06 =			0x513F5D;
int callAddr_InfoMessageBox07 =			0x58B5A5;
int callAddr_InfoMessageBox08 =			0x58BF4C;
int callAddr_InfoMessageBox09 =			0x58C053;
int callAddr_InfoMessageBox10 =			0x58C252;
int callAddr_InfoMessageBox11 =			0x58C559;
int callAddr_InfoMessageBox12 =			0x58D054;
int callAddr_InfoMessageBox13 =			0x58D084;
int callAddr_Encrypt01 =				0x5312A3;
int callAddr_Decrypt01 =				0x5318A9;
int callAddr_ShouldParseRecv01 =		0x479702;
*/
//10.51
/*
int funAddr_tibiaPrintText =			0x4DE220;
int funAddr_tibiaPlayerNameText =		0x4DD5B0;
int funAddr_tibiaInfoMiddleScreen =		0x5878A0;
int funAddr_tibiaIsCreatureVisible =	0x485BE0;
int funAddr_tibiaEncrypt =				0x59AFB0;
int funAddr_tibiaDecrypt =				0x59B030;
int funAddr_tibiaShouldParseRecv =		0x531CA0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0xA3BC28-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x58B810;
int callAddr_PrintText01 =				0x46D63D;//...<addr>.*
int callAddr_PrintText02 =				0x46D682;
int callAddr_PrintText03 =				0x4766E3;
int callAddr_PrintText04 =				0x51EEEA;
int callAddr_PrintText05 =				0x5200F0;
int callAddr_PlayerNameText01 =			0x433C65;
int callAddr_PlayerNameText02 =			0x4DDEA8;
int callAddr_PlayerNameText03 =			0x4DE0A8;
int callAddr_PlayerNameText04 =			0x4DE2AC;
int callAddr_PlayerNameText05 =			0x4DE4AC;
int callAddr_PlayerNameText06 =			0x4DE6A8;
int callAddr_PlayerNameText07 =			0x4DE8AA;
int callAddr_PlayerNameText08 =			0x51FC2F;
int callAddr_InfoMiddleScreen01 =		0x4289BA;
int callAddr_InfoMiddleScreen02 =		0x4724A4;
int callAddr_InfoMiddleScreen03 =		0x4AC75D;
int callAddr_InfoMiddleScreen04 =		0x51CD4B;
int callAddr_InfoMessageBox01 =			0x4282D4;
int callAddr_InfoMessageBox02 =			0x440CDF;
int callAddr_InfoMessageBox03 =			0x4C507A;
int callAddr_InfoMessageBox04 =			0x513802;
int callAddr_InfoMessageBox05 =			0x513FF7;
int callAddr_InfoMessageBox06 =			0x51409D;
int callAddr_InfoMessageBox07 =			0x58B695;
int callAddr_InfoMessageBox08 =			0x58C03C;
int callAddr_InfoMessageBox09 =			0x58C143;
int callAddr_InfoMessageBox10 =			0x58C342;
int callAddr_InfoMessageBox11 =			0x58C649;
int callAddr_InfoMessageBox12 =			0x58D144;
int callAddr_InfoMessageBox13 =			0x58D174;
int callAddr_Encrypt01 =				0x5313C3;
int callAddr_Decrypt01 =				0x5319C9;
int callAddr_ShouldParseRecv01 =		0x4797F2;
*/
//10.50
/*
int funAddr_tibiaPrintText =			0x4DE180;
int funAddr_tibiaPlayerNameText =		0x4DD510;
int funAddr_tibiaInfoMiddleScreen =		0x5877D0;
int funAddr_tibiaIsCreatureVisible =	0x485C00;
int funAddr_tibiaEncrypt =				0x59AEB0;
int funAddr_tibiaDecrypt =				0x59AF30;
int funAddr_tibiaShouldParseRecv =		0x531C60;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0xA3BC28-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x58B740;
int callAddr_PrintText01 =				0x46D67D;//...<addr>.*
int callAddr_PrintText02 =				0x46D6C2;
int callAddr_PrintText03 =				0x476703;
int callAddr_PrintText04 =				0x51EE88;
int callAddr_PrintText05 =				0x52008F;
int callAddr_PlayerNameText01 =			0x433CB5;
int callAddr_PlayerNameText02 =			0x4DDE08;
int callAddr_PlayerNameText03 =			0x4DE008;
int callAddr_PlayerNameText04 =			0x4DE20C;
int callAddr_PlayerNameText05 =			0x4DE40C;
int callAddr_PlayerNameText06 =			0x4DE608;
int callAddr_PlayerNameText07 =			0x4DE80A;
int callAddr_PlayerNameText08 =			0x51FBCE;
int callAddr_InfoMiddleScreen01 =		0x428A4A;
int callAddr_InfoMiddleScreen02 =		0x4724C4;
int callAddr_InfoMiddleScreen03 =		0x4AC6ED;
int callAddr_InfoMiddleScreen04 =		0x51CCEB;
int callAddr_InfoMessageBox01 =			0x428364;
int callAddr_InfoMessageBox02 =			0x440D1F;
int callAddr_InfoMessageBox03 =			0x4C4FAA;
int callAddr_InfoMessageBox04 =			0x5137A2;
int callAddr_InfoMessageBox05 =			0x513F97;
int callAddr_InfoMessageBox06 =			0x51403D;
int callAddr_InfoMessageBox07 =			0x58B5C5;
int callAddr_InfoMessageBox08 =			0x58BF6C;
int callAddr_InfoMessageBox09 =			0x58C073;
int callAddr_InfoMessageBox10 =			0x58C272;
int callAddr_InfoMessageBox11 =			0x58C579;
int callAddr_InfoMessageBox12 =			0x58D074;
int callAddr_InfoMessageBox13 =			0x58D0A4;
int callAddr_Encrypt01 =				0x531383;
int callAddr_Decrypt01 =				0x531989;
int callAddr_ShouldParseRecv01 =		0x479802;
*/
//10.39 & 10.40 & 10.41
/*
int funAddr_tibiaPrintText =			0x4D7FC0;
int funAddr_tibiaPlayerNameText =		0x4D7310;
int funAddr_tibiaInfoMiddleScreen =		0x580630;
int funAddr_tibiaIsCreatureVisible =	0x47F560;
int funAddr_tibiaEncrypt =				0x5936A0;
int funAddr_tibiaDecrypt =				0x5937D0;
int funAddr_tibiaShouldParseRecv =		0x52AB80;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9F67E4-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x5845D0;
int callAddr_PrintText01 =				0x467DED;//...<addr>.*
int callAddr_PrintText02 =				0x467E37;
int callAddr_PrintText03 =				0x470D86;
int callAddr_PrintText04 =				0x518244;
int callAddr_PrintText05 =				0x519326;
int callAddr_PlayerNameText01 =			0x42D730;
int callAddr_PlayerNameText02 =			0x4D7C44;
int callAddr_PlayerNameText03 =			0x4D7E44;
int callAddr_PlayerNameText04 =			0x4D8043;
int callAddr_PlayerNameText05 =			0x4D8243;
int callAddr_PlayerNameText06 =			0x4D8444;
int callAddr_PlayerNameText07 =			0x4D8642;
int callAddr_PlayerNameText08 =			0x518E93;
int callAddr_InfoMiddleScreen01 =		0x422712;
int callAddr_InfoMiddleScreen02 =		0x46CD55;
int callAddr_InfoMiddleScreen03 =		0x4A50B5;
int callAddr_InfoMiddleScreen04 =		0x516142;
int callAddr_InfoMessageBox01 =			0x422039;
int callAddr_InfoMessageBox02 =			0x43AF77;
int callAddr_InfoMessageBox03 =			0x4BE716;
int callAddr_InfoMessageBox04 =			0x50D0C0;
int callAddr_InfoMessageBox05 =			0x50D89D;
int callAddr_InfoMessageBox06 =			0x50D901;
int callAddr_InfoMessageBox07 =			0x584437;
int callAddr_InfoMessageBox08 =			0x584E40;
int callAddr_InfoMessageBox09 =			0x584F95;
int callAddr_InfoMessageBox10 =			0x58508A;
int callAddr_InfoMessageBox11 =			0x5852D0;
int callAddr_InfoMessageBox12 =			0x585DAD;
int callAddr_Encrypt01 =				0x52A26D;
int callAddr_Decrypt01 =				0x52A879;
int callAddr_ShouldParseRecv01 =		0x473F37;
*/
//10.38
/*
int funAddr_tibiaPrintText =			0x4D7F60;
int funAddr_tibiaPlayerNameText =		0x4D72B0;
int funAddr_tibiaInfoMiddleScreen =		0x5805E0;
int funAddr_tibiaIsCreatureVisible =	0x47F560;
int funAddr_tibiaEncrypt =				0x593630;
int funAddr_tibiaDecrypt =				0x593760;
int funAddr_tibiaShouldParseRecv =		0x52AAF0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9F6744-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x584580;
int callAddr_PrintText01 =				0x467DED;//...<addr>.*
int callAddr_PrintText02 =				0x467E37;
int callAddr_PrintText03 =				0x470D86;
int callAddr_PrintText04 =				0x5181C4;
int callAddr_PrintText05 =				0x5192A6;
int callAddr_PlayerNameText01 =			0x42D730;
int callAddr_PlayerNameText02 =			0x4D7BE4;
int callAddr_PlayerNameText03 =			0x4D7DE4;
int callAddr_PlayerNameText04 =			0x4D7FE3;
int callAddr_PlayerNameText05 =			0x4D81E3;
int callAddr_PlayerNameText06 =			0x4D83E4;
int callAddr_PlayerNameText07 =			0x4D85E2;
int callAddr_PlayerNameText08 =			0x518E13;
int callAddr_InfoMiddleScreen01 =		0x422712;
int callAddr_InfoMiddleScreen02 =		0x46CD55;
int callAddr_InfoMiddleScreen03 =		0x4A50B5;
int callAddr_InfoMiddleScreen04 =		0x5160B2;
int callAddr_InfoMessageBox01 =			0x422039;
int callAddr_InfoMessageBox02 =			0x43AF77;
int callAddr_InfoMessageBox03 =			0x4BE716;
int callAddr_InfoMessageBox04 =			0x50D040;
int callAddr_InfoMessageBox05 =			0x50D81D;
int callAddr_InfoMessageBox06 =			0x50D881;
int callAddr_InfoMessageBox07 =			0x5843E7;
int callAddr_InfoMessageBox08 =			0x584DD0;
int callAddr_InfoMessageBox09 =			0x584F25;
int callAddr_InfoMessageBox10 =			0x58501A;
int callAddr_InfoMessageBox11 =			0x585260;
int callAddr_InfoMessageBox12 =			0x585D3D;
int callAddr_Encrypt01 =				0x52A1DD;
int callAddr_Decrypt01 =				0x52A7E9;
int callAddr_ShouldParseRecv01 =		0x473F37;
*/

//10.37
/*
int funAddr_tibiaPrintText =			0x4D57A0;
int funAddr_tibiaPlayerNameText =		0x4D4AF0;
int funAddr_tibiaInfoMiddleScreen =		0x57B980;
int funAddr_tibiaIsCreatureVisible =	0x47D260;
int funAddr_tibiaEncrypt =				0x58EAF0;
int funAddr_tibiaDecrypt =				0x58EC20;
int funAddr_tibiaShouldParseRecv =		0x526AE0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9ECCE4-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57F910;
int callAddr_PrintText01 =				0x465ADD;//...<addr>.*
int callAddr_PrintText02 =				0x465B27;
int callAddr_PrintText03 =				0x46EB16;
int callAddr_PrintText04 =				0x5141C8;
int callAddr_PrintText05 =				0x5152BA;
int callAddr_PlayerNameText01 =			0x42D0C0;
int callAddr_PlayerNameText02 =			0x4D5424;
int callAddr_PlayerNameText03 =			0x4D5624;
int callAddr_PlayerNameText04 =			0x4D5823;
int callAddr_PlayerNameText05 =			0x4D5A23;
int callAddr_PlayerNameText06 =			0x4D5C24;
int callAddr_PlayerNameText07 =			0x4D5E22;
int callAddr_PlayerNameText08 =			0x514E31;
int callAddr_InfoMiddleScreen01 =		0x4223C2;
int callAddr_InfoMiddleScreen02 =		0x46AAE5;
int callAddr_InfoMiddleScreen03 =		0x4A28D5;
int callAddr_InfoMiddleScreen04 =		0x5120B2;
int callAddr_InfoMessageBox01 =			0x421CE9;
int callAddr_InfoMessageBox02 =			0x43A8B7;
int callAddr_InfoMessageBox03 =			0x4BBE26;
int callAddr_InfoMessageBox04 =			0x508F50;
int callAddr_InfoMessageBox05 =			0x50972D;
int callAddr_InfoMessageBox06 =			0x509791;
int callAddr_InfoMessageBox07 =			0x57F777;
int callAddr_InfoMessageBox08 =			0x580160;
int callAddr_InfoMessageBox09 =			0x5802B5;
int callAddr_InfoMessageBox10 =			0x5803AA;
int callAddr_InfoMessageBox11 =			0x5805F0;
int callAddr_InfoMessageBox12 =			0x5810CD;
int callAddr_Encrypt01 =				0x5261CD;
int callAddr_Decrypt01 =				0x5267D9;
int callAddr_ShouldParseRecv01 =		0x471CA7;//10.37
int funAddr_tibiaPrintText =			0x4D57A0;
int funAddr_tibiaPlayerNameText =		0x4D4AF0;
int funAddr_tibiaInfoMiddleScreen =		0x57B980;
int funAddr_tibiaIsCreatureVisible =	0x47D260;
int funAddr_tibiaEncrypt =				0x58EAF0;
int funAddr_tibiaDecrypt =				0x58EC20;
int funAddr_tibiaShouldParseRecv =		0x526AE0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9ECCE4-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57F910;
int callAddr_PrintText01 =				0x465ADD;//...<addr>.*
int callAddr_PrintText02 =				0x465B27;
int callAddr_PrintText03 =				0x46EB16;
int callAddr_PrintText04 =				0x5141C8;
int callAddr_PrintText05 =				0x5152BA;
int callAddr_PlayerNameText01 =			0x42D0C0;
int callAddr_PlayerNameText02 =			0x4D5424;
int callAddr_PlayerNameText03 =			0x4D5624;
int callAddr_PlayerNameText04 =			0x4D5823;
int callAddr_PlayerNameText05 =			0x4D5A23;
int callAddr_PlayerNameText06 =			0x4D5C24;
int callAddr_PlayerNameText07 =			0x4D5E22;
int callAddr_PlayerNameText08 =			0x514E31;
int callAddr_InfoMiddleScreen01 =		0x4223C2;
int callAddr_InfoMiddleScreen02 =		0x46AAE5;
int callAddr_InfoMiddleScreen03 =		0x4A28D5;
int callAddr_InfoMiddleScreen04 =		0x5120B2;
int callAddr_InfoMessageBox01 =			0x421CE9;
int callAddr_InfoMessageBox02 =			0x43A8B7;
int callAddr_InfoMessageBox03 =			0x4BBE26;
int callAddr_InfoMessageBox04 =			0x508F50;
int callAddr_InfoMessageBox05 =			0x50972D;
int callAddr_InfoMessageBox06 =			0x509791;
int callAddr_InfoMessageBox07 =			0x57F777;
int callAddr_InfoMessageBox08 =			0x580160;
int callAddr_InfoMessageBox09 =			0x5802B5;
int callAddr_InfoMessageBox10 =			0x5803AA;
int callAddr_InfoMessageBox11 =			0x5805F0;
int callAddr_InfoMessageBox12 =			0x5810CD;
int callAddr_Encrypt01 =				0x5261CD;
int callAddr_Decrypt01 =				0x5267D9;
int callAddr_ShouldParseRecv01 =		0x471CA7;
*/
//10.36
/*
int funAddr_tibiaPrintText =			0x4D57A0;
int funAddr_tibiaPlayerNameText =		0x4D4AF0;
int funAddr_tibiaInfoMiddleScreen =		0x57B980;
int funAddr_tibiaIsCreatureVisible =	0x47D260;
int funAddr_tibiaEncrypt =				0x58EAC0;
int funAddr_tibiaDecrypt =				0x58EBF0;
int funAddr_tibiaShouldParseRecv =		0x526AE0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9ECCE4-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57F910;
int callAddr_PrintText01 =				0x465ADD;//...<addr>.*
int callAddr_PrintText02 =				0x465B27;
int callAddr_PrintText03 =				0x46EB16;
int callAddr_PrintText04 =				0x5141C8;
int callAddr_PrintText05 =				0x5152BA;
int callAddr_PlayerNameText01 =			0x42D0C0;
int callAddr_PlayerNameText02 =			0x4D5424;
int callAddr_PlayerNameText03 =			0x4D5624;
int callAddr_PlayerNameText04 =			0x4D5823;
int callAddr_PlayerNameText05 =			0x4D5A23;
int callAddr_PlayerNameText06 =			0x4D5C24;
int callAddr_PlayerNameText07 =			0x4D5E22;
int callAddr_PlayerNameText08 =			0x514E31;
int callAddr_InfoMiddleScreen01 =		0x4223C2;
int callAddr_InfoMiddleScreen02 =		0x46AAE5;
int callAddr_InfoMiddleScreen03 =		0x4A28D5;
int callAddr_InfoMiddleScreen04 =		0x5120B2;
int callAddr_InfoMessageBox01 =			0x421CE9;
int callAddr_InfoMessageBox02 =			0x43A8B7;
int callAddr_InfoMessageBox03 =			0x4BBE26;
int callAddr_InfoMessageBox04 =			0x508F50;
int callAddr_InfoMessageBox05 =			0x50972D;
int callAddr_InfoMessageBox06 =			0x509791;
int callAddr_InfoMessageBox07 =			0x57F777;
int callAddr_InfoMessageBox08 =			0x580154;
int callAddr_InfoMessageBox09 =			0x5802A9;
int callAddr_InfoMessageBox10 =			0x58039E;
int callAddr_InfoMessageBox11 =			0x5805E0;
int callAddr_InfoMessageBox12 =			0x5810BD;
int callAddr_Encrypt01 =				0x5261CD;
int callAddr_Decrypt01 =				0x5267D9;
int callAddr_ShouldParseRecv01 =		0x471CA7;
*/
//10.35
/*
int funAddr_tibiaPrintText =			0x4D5090;
int funAddr_tibiaPlayerNameText =		0x4D43E0;
int funAddr_tibiaInfoMiddleScreen =		0x57AC60;
int funAddr_tibiaIsCreatureVisible =	0x47CA70;
int funAddr_tibiaEncrypt =				0x58D920;
int funAddr_tibiaDecrypt =				0x58DA50;
int funAddr_tibiaShouldParseRecv =		0x5261B0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E975C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57EBA0;
int callAddr_PrintText01 =				0x46547D;//...<addr>.*
int callAddr_PrintText02 =				0x4654C7;
int callAddr_PrintText03 =				0x46E3F6;
int callAddr_PrintText04 =				0x5139FB;
int callAddr_PrintText05 =				0x5149A7;
int callAddr_PlayerNameText01 =			0x42C9A0;
int callAddr_PlayerNameText02 =			0x4D4D14;
int callAddr_PlayerNameText03 =			0x4D4F14;
int callAddr_PlayerNameText04 =			0x4D5113;
int callAddr_PlayerNameText05 =			0x4D5313;
int callAddr_PlayerNameText06 =			0x4D5514;
int callAddr_PlayerNameText07 =			0x4D5712;
int callAddr_PlayerNameText08 =			0x514512;
int callAddr_InfoMiddleScreen01 =		0x421EE8;
int callAddr_InfoMiddleScreen02 =		0x46A3C8;
int callAddr_InfoMiddleScreen03 =		0x4A22C5;
int callAddr_InfoMiddleScreen04 =		0x511912;
int callAddr_InfoMessageBox01 =			0x421809;
int callAddr_InfoMessageBox02 =			0x43A237;
int callAddr_InfoMessageBox03 =			0x4BB6B6;
int callAddr_InfoMessageBox04 =			0x508830;
int callAddr_InfoMessageBox05 =			0x50900D;
int callAddr_InfoMessageBox06 =			0x509071;
int callAddr_InfoMessageBox07 =			0x57EA07;
int callAddr_InfoMessageBox08 =			0x57F477;
int callAddr_InfoMessageBox09 =			0x57F547;
int callAddr_InfoMessageBox10 =			0x57F63C;
int callAddr_InfoMessageBox11 =			0x57F880;
int callAddr_InfoMessageBox12 =			0x58035D;
int callAddr_Encrypt01 =				0x52589D;
int callAddr_Decrypt01 =				0x525EA9;
int callAddr_ShouldParseRecv01 =		0x471597;
*/
//10.34
/*
int funAddr_tibiaPrintText =			0x4D5090;
int funAddr_tibiaPlayerNameText =		0x4D43E0;
int funAddr_tibiaInfoMiddleScreen =		0x57AB40;
int funAddr_tibiaIsCreatureVisible =	0x47CBC0;
int funAddr_tibiaEncrypt =				0x58D8C0;
int funAddr_tibiaDecrypt =				0x58D9F0;
int funAddr_tibiaShouldParseRecv =		0x526130;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E975C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57EA90;
int callAddr_PrintText01 =				0x46552D;//...<addr>.*
int callAddr_PrintText02 =				0x465577;
int callAddr_PrintText03 =				0x46E516;
int callAddr_PrintText04 =				0x513936;
int callAddr_PrintText05 =				0x5148E6;
int callAddr_PlayerNameText01 =			0x42C9A0;
int callAddr_PlayerNameText02 =			0x4D4D14;
int callAddr_PlayerNameText03 =			0x4D4F14;
int callAddr_PlayerNameText04 =			0x4D5113;
int callAddr_PlayerNameText05 =			0x4D5313;
int callAddr_PlayerNameText06 =			0x4D5514;
int callAddr_PlayerNameText07 =			0x4D5712;
int callAddr_PlayerNameText08 =			0x51444F;
int callAddr_InfoMiddleScreen01 =		0x421EA8;
int callAddr_InfoMiddleScreen02 =		0x46A488;
int callAddr_InfoMiddleScreen03 =		0x4A2385;
int callAddr_InfoMiddleScreen04 =		0x511862;
int callAddr_InfoMessageBox01 =			0x4217D9;
int callAddr_InfoMessageBox02 =			0x43A167;
int callAddr_InfoMessageBox03 =			0x4BB716;
int callAddr_InfoMessageBox04 =			0x508700;
int callAddr_InfoMessageBox05 =			0x508EDD;
int callAddr_InfoMessageBox06 =			0x508F41;
int callAddr_InfoMessageBox07 =			0x57E8F7;
int callAddr_InfoMessageBox08 =			0x57F367;
int callAddr_InfoMessageBox09 =			0x57F437;
int callAddr_InfoMessageBox10 =			0x57F52C;
int callAddr_InfoMessageBox11 =			0x57F770;
int callAddr_InfoMessageBox12 =			0x58024D;
int callAddr_Encrypt01 =				0x52581D;
int callAddr_Decrypt01 =				0x525E29;
int callAddr_ShouldParseRecv01 =		0x4716E7;
*/

//10.31
/*
int funAddr_tibiaPrintText =			0x4D4BD0;
int funAddr_tibiaPlayerNameText =		0x4D3F20;
int funAddr_tibiaInfoMiddleScreen =		0x57A100;
int funAddr_tibiaIsCreatureVisible =	0x47C7D0;
int funAddr_tibiaEncrypt =				0x58CE70;
int funAddr_tibiaDecrypt =				0x58CFA0;
int funAddr_tibiaShouldParseRecv =		0x51FEA0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E849C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57E050;
int callAddr_PrintText01 =				0x46523D;//...<addr>.*
int callAddr_PrintText02 =				0x465287;
int callAddr_PrintText03 =				0x46E226;
int callAddr_PrintText04 =				0x5133D6;
int callAddr_PrintText05 =				0x514386;
int callAddr_PlayerNameText01 =			0x42C820;
int callAddr_PlayerNameText02 =			0x4D4854;
int callAddr_PlayerNameText03 =			0x4D4A54;
int callAddr_PlayerNameText04 =			0x4D4C53;
int callAddr_PlayerNameText05 =			0x4D4E53;
int callAddr_PlayerNameText06 =			0x4D5054;
int callAddr_PlayerNameText07 =			0x4D5252;
int callAddr_PlayerNameText08 =			0x513EEF;
int callAddr_InfoMiddleScreen01 =		0x421C8C;
int callAddr_InfoMiddleScreen02 =		0x46A188;
int callAddr_InfoMiddleScreen03 =		0x4A2105;
int callAddr_InfoMiddleScreen04 =		0x511302;
int callAddr_InfoMessageBox01 =			0x4217F9;
int callAddr_InfoMessageBox02 =			0x439FE7;
int callAddr_InfoMessageBox03 =			0x4BB536;
int callAddr_InfoMessageBox04 =			0x508000;
int callAddr_InfoMessageBox05 =			0x50896D;
int callAddr_InfoMessageBox06 =			0x5089D1;
int callAddr_InfoMessageBox07 =			0x57DEB7;
int callAddr_InfoMessageBox08 =			0x57E927;
int callAddr_InfoMessageBox09 =			0x57E9F7;
int callAddr_InfoMessageBox10 =			0x57EAEC;
int callAddr_InfoMessageBox11 =			0x57ED30;
int callAddr_InfoMessageBox12 =			0x57F82D;
int callAddr_Encrypt01 =				0x51F58D;
int callAddr_Decrypt01 =				0x51FB99;
int callAddr_ShouldParseRecv01 =		0x4713B7;
*/

//10.30
/*
int funAddr_tibiaPrintText =			0x4D4BD0;
int funAddr_tibiaPlayerNameText =		0x4D3F20;
int funAddr_tibiaInfoMiddleScreen =		0x57A100;
int funAddr_tibiaIsCreatureVisible =	0x47C7D0;
int funAddr_tibiaEncrypt =				0x58CE70;
int funAddr_tibiaDecrypt =				0x58CFA0;
int funAddr_tibiaShouldParseRecv =		0x51FEA0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E8A94-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57E050;
int callAddr_PrintText01 =				0x46523D;//...<addr>.*
int callAddr_PrintText02 =				0x465287;
int callAddr_PrintText03 =				0x46E226;
int callAddr_PrintText04 =				0x5133D6;
int callAddr_PrintText05 =				0x514386;
int callAddr_PlayerNameText01 =			0x42C820;
int callAddr_PlayerNameText02 =			0x4D4854;
int callAddr_PlayerNameText03 =			0x4D4A54;
int callAddr_PlayerNameText04 =			0x4D4C53;
int callAddr_PlayerNameText05 =			0x4D4E53;
int callAddr_PlayerNameText06 =			0x4D5054;
int callAddr_PlayerNameText07 =			0x4D5252;
int callAddr_PlayerNameText08 =			0x513EEF;
int callAddr_InfoMiddleScreen01 =		0x421C8C;
int callAddr_InfoMiddleScreen02 =		0x46A188;
int callAddr_InfoMiddleScreen03 =		0x4A2105;
int callAddr_InfoMiddleScreen04 =		0x511302;
int callAddr_InfoMessageBox01 =			0x4217F9;
int callAddr_InfoMessageBox02 =			0x439FE7;
int callAddr_InfoMessageBox03 =			0x4BB536;
int callAddr_InfoMessageBox04 =			0x508000;
int callAddr_InfoMessageBox05 =			0x50896D;
int callAddr_InfoMessageBox06 =			0x5089D1;
int callAddr_InfoMessageBox07 =			0x57DEB7;
int callAddr_InfoMessageBox08 =			0x57E927;
int callAddr_InfoMessageBox09 =			0x57E9F7;
int callAddr_InfoMessageBox10 =			0x57EAEC;
int callAddr_InfoMessageBox11 =			0x57ED30;
int callAddr_InfoMessageBox12 =			0x57F82D;
int callAddr_Encrypt01 =				0x51F58D;
int callAddr_Decrypt01 =				0x51FB99;
int callAddr_ShouldParseRecv01 =		0x4713B7;
*/
//10.22
/*
int funAddr_tibiaPrintText =			0x4D4A60;
int funAddr_tibiaPlayerNameText =		0x4D3DB0;
int funAddr_tibiaInfoMiddleScreen =		0x57A180;
int funAddr_tibiaIsCreatureVisible =	0x47C950;
int funAddr_tibiaEncrypt =				0x58CEC0;
int funAddr_tibiaDecrypt =				0x58CFF0;
int funAddr_tibiaShouldParseRecv =		0x51FBE0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9EA67C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57E0D0;
int callAddr_PrintText01 =				0x46535D;//...<addr>.*
int callAddr_PrintText02 =				0x4653A7;
int callAddr_PrintText03 =				0x46E386;
int callAddr_PrintText04 =				0x5130E6;
int callAddr_PrintText05 =				0x514096;
int callAddr_PlayerNameText01 =			0x42C960;
int callAddr_PlayerNameText02 =			0x4D46E4;
int callAddr_PlayerNameText03 =			0x4D48E4;
int callAddr_PlayerNameText04 =			0x4D4AE3;
int callAddr_PlayerNameText05 =			0x4D4CE3;
int callAddr_PlayerNameText06 =			0x4D4EE4;
int callAddr_PlayerNameText07 =			0x4D50E2;
int callAddr_PlayerNameText08 =			0x513BFF;
int callAddr_InfoMiddleScreen01 =		0x421D9C;
int callAddr_InfoMiddleScreen02 =		0x46A368;
int callAddr_InfoMiddleScreen03 =		0x4A2015;
int callAddr_InfoMiddleScreen04 =		0x511012;
int callAddr_InfoMessageBox01 =			0x421909;
int callAddr_InfoMessageBox02 =			0x43A107;
int callAddr_InfoMessageBox03 =			0x4BB396;
int callAddr_InfoMessageBox04 =			0x508000;
int callAddr_InfoMessageBox05 =			0x50882D;
int callAddr_InfoMessageBox06 =			0x508891;
int callAddr_InfoMessageBox07 =			0x57DF37;
int callAddr_InfoMessageBox08 =			0x57E9A7;
int callAddr_InfoMessageBox09 =			0x57EA77;
int callAddr_InfoMessageBox10 =			0x57EB6C;
int callAddr_InfoMessageBox11 =			0x57EDB0;
int callAddr_InfoMessageBox12 =			0x57F88D;
int callAddr_Encrypt01 =				0x51F2CD;
int callAddr_Decrypt01 =				0x51F8D9;
int callAddr_ShouldParseRecv01 =		0x471507;
*/
//10.21
/*
int funAddr_tibiaPrintText =			0x4D4990;
int funAddr_tibiaPlayerNameText =		0x4D3CE0;
int funAddr_tibiaInfoMiddleScreen =		0x57A090;
int funAddr_tibiaIsCreatureVisible =	0x47C700;
int funAddr_tibiaEncrypt =				0x58CEC0;
int funAddr_tibiaDecrypt =				0x58CFF0;
int funAddr_tibiaShouldParseRecv =		0x51FD20;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9EA67C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57DFE0;
int callAddr_PrintText01 =				0x46517D;//...<addr>.*
int callAddr_PrintText02 =				0x4651C7;
int callAddr_PrintText03 =				0x46E156;
int callAddr_PrintText04 =				0x5131B6;
int callAddr_PrintText05 =				0x514166;
int callAddr_PlayerNameText01 =			0x42C960;
int callAddr_PlayerNameText02 =			0x4D4614;
int callAddr_PlayerNameText03 =			0x4D4814;
int callAddr_PlayerNameText04 =			0x4D4A13;
int callAddr_PlayerNameText05 =			0x4D4C13;
int callAddr_PlayerNameText06 =			0x4D4E14;
int callAddr_PlayerNameText07 =			0x4D5012;
int callAddr_PlayerNameText08 =			0x513CCF;
int callAddr_InfoMiddleScreen01 =		0x421B9C;
int callAddr_InfoMiddleScreen02 =		0x46A128;
int callAddr_InfoMiddleScreen03 =		0x4A1F35;
int callAddr_InfoMiddleScreen04 =		0x5110E2;
int callAddr_InfoMessageBox01 =			0x421709;
int callAddr_InfoMessageBox02 =			0x439EF7;
int callAddr_InfoMessageBox03 =			0x4BB316;
int callAddr_InfoMessageBox04 =			0x508000;
int callAddr_InfoMessageBox05 =			0x5087FD;
int callAddr_InfoMessageBox06 =			0x508861;
int callAddr_InfoMessageBox07 =			0x57DE47;
int callAddr_InfoMessageBox08 =			0x57E8B7;
int callAddr_InfoMessageBox09 =			0x57E987;
int callAddr_InfoMessageBox10 =			0x57EA7C;
int callAddr_InfoMessageBox11 =			0x57ECC0;
int callAddr_InfoMessageBox12 =			0x57F79D;
int callAddr_Encrypt01 =				0x51F40D;
int callAddr_Decrypt01 =				0x51FA19;
int callAddr_ShouldParseRecv01 =		0x4712F7;
*/
//10.20
/*
int funAddr_tibiaPrintText =			0x4D4D00;
int funAddr_tibiaPlayerNameText =		0x4D4050;
int funAddr_tibiaInfoMiddleScreen =		0x57A280;
int funAddr_tibiaIsCreatureVisible =	0x47CE00;
int funAddr_tibiaEncrypt =				0x58CF30;
int funAddr_tibiaDecrypt =				0x58D060;
int funAddr_tibiaShouldParseRecv =		0x5201B0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E93E4-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57E1D0;
int callAddr_PrintText01 =				0x46586D;//...<addr>.*
int callAddr_PrintText02 =				0x4658B7;
int callAddr_PrintText03 =				0x46E846;
int callAddr_PrintText04 =				0x513505;
int callAddr_PrintText05 =				0x5144E8;
int callAddr_PlayerNameText01 =			0x42CCD0;
int callAddr_PlayerNameText02 =			0x4D4984;
int callAddr_PlayerNameText03 =			0x4D4B84;
int callAddr_PlayerNameText04 =			0x4D4D83;
int callAddr_PlayerNameText05 =			0x4D4F83;
int callAddr_PlayerNameText06 =			0x4D5184;
int callAddr_PlayerNameText07 =			0x4D5382;
int callAddr_PlayerNameText08 =			0x51401F;
int callAddr_InfoMiddleScreen01 =		0x42210C;
int callAddr_InfoMiddleScreen02 =		0x46A828;
int callAddr_InfoMiddleScreen03 =		0x4A2605;
int callAddr_InfoMiddleScreen04 =		0x511324;
int callAddr_InfoMessageBox01 =			0x421C79;
int callAddr_InfoMessageBox02 =			0x43A4E7;
int callAddr_InfoMessageBox03 =			0x4BBA26;
int callAddr_InfoMessageBox04 =			0x508000;
int callAddr_InfoMessageBox05 =			0x5089AD;
int callAddr_InfoMessageBox06 =			0x508A11;
int callAddr_InfoMessageBox07 =			0x57E037;
int callAddr_InfoMessageBox08 =			0x57EAA7;
int callAddr_InfoMessageBox09 =			0x57EB77;
int callAddr_InfoMessageBox10 =			0x57EC6C;
int callAddr_InfoMessageBox11 =			0x57EEB0;
int callAddr_InfoMessageBox12 =			0x57F98D;
int callAddr_Encrypt01 =				0x51F88D;
int callAddr_Decrypt01 =				0x51FEA9;
int callAddr_ShouldParseRecv01 =		0x4719C7;
*/
//10.13
/*
int funAddr_tibiaPrintText =			0x4D49E0;
int funAddr_tibiaPlayerNameText =		0x4D3D30;
int funAddr_tibiaInfoMiddleScreen =		0x579BF0;
int funAddr_tibiaIsCreatureVisible =	0x47C8D0;
int funAddr_tibiaEncrypt =				0x58C930;
int funAddr_tibiaDecrypt =				0x58CA60;
int funAddr_tibiaShouldParseRecv =		0x51FE40;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E847C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57DB40;
int callAddr_PrintText01 =				0x4653BD;//...<addr>.*
int callAddr_PrintText02 =				0x465407;
int callAddr_PrintText03 =				0x46E366;
int callAddr_PrintText04 =				0x513255;
int callAddr_PrintText05 =				0x514238;
int callAddr_PlayerNameText01 =			0x42CA40;
int callAddr_PlayerNameText02 =			0x4D4664;
int callAddr_PlayerNameText03 =			0x4D4864;
int callAddr_PlayerNameText04 =			0x4D4A63;
int callAddr_PlayerNameText05 =			0x4D4C63;
int callAddr_PlayerNameText06 =			0x4D4E64;
int callAddr_PlayerNameText07 =			0x4D5062;
int callAddr_PlayerNameText08 =			0x513D6F;
int callAddr_InfoMiddleScreen01 =		0x421EFC;
int callAddr_InfoMiddleScreen02 =		0x46A348;
int callAddr_InfoMiddleScreen03 =		0x4A2255;
int callAddr_InfoMiddleScreen04 =		0x5110A4;
int callAddr_InfoMessageBox01 =			0x421A69;
int callAddr_InfoMessageBox02 =			0x43A237;
int callAddr_InfoMessageBox03 =			0x4BB636;
int callAddr_InfoMessageBox04 =			0x508000;
int callAddr_InfoMessageBox05 =			0x5087D3;
int callAddr_InfoMessageBox06 =			0x508837;
int callAddr_InfoMessageBox07 =			0x57D9A7;
int callAddr_InfoMessageBox08 =			0x57E417;
int callAddr_InfoMessageBox09 =			0x57E4E7;
int callAddr_InfoMessageBox10 =			0x57E5DC;
int callAddr_InfoMessageBox11 =			0x57E820;
int callAddr_InfoMessageBox12 =			0x57F2FD;
int callAddr_Encrypt01 =				0x51F52D;
int callAddr_Decrypt01 =				0x51FB39;
int callAddr_ShouldParseRecv01 =		0x4714A7;
*/
//10.12
/*
int funAddr_tibiaPrintText =			0x4D49E0;
int funAddr_tibiaPlayerNameText =		0x4D3D30;
int funAddr_tibiaInfoMiddleScreen =		0x579BF0;
int funAddr_tibiaIsCreatureVisible =	0x47C8D0;
int funAddr_tibiaEncrypt =				0x58C930;
int funAddr_tibiaDecrypt =				0x58CA60;
int funAddr_tibiaShouldParseRecv =		0x51FE40;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E847C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57DB40;
int callAddr_PrintText01 =				0x4653BD;//...<addr>.*
int callAddr_PrintText02 =				0x465407;
int callAddr_PrintText03 =				0x46E366;
int callAddr_PrintText04 =				0x513255;
int callAddr_PrintText05 =				0x514238;
int callAddr_PlayerNameText01 =			0x42CA40;
int callAddr_PlayerNameText02 =			0x4D4664;
int callAddr_PlayerNameText03 =			0x4D4864;
int callAddr_PlayerNameText04 =			0x4D4A63;
int callAddr_PlayerNameText05 =			0x4D4C63;
int callAddr_PlayerNameText06 =			0x4D4E64;
int callAddr_PlayerNameText07 =			0x4D5062;
int callAddr_PlayerNameText08 =			0x513D6F;
int callAddr_InfoMiddleScreen01 =		0x421EFC;
int callAddr_InfoMiddleScreen02 =		0x46A348;
int callAddr_InfoMiddleScreen03 =		0x4A2255;
int callAddr_InfoMiddleScreen04 =		0x5110A4;
int callAddr_InfoMessageBox01 =			0x421A69;
int callAddr_InfoMessageBox02 =			0x43A237;
int callAddr_InfoMessageBox03 =			0x4BB636;
int callAddr_InfoMessageBox04 =			0x508000;
int callAddr_InfoMessageBox05 =			0x5087D3;
int callAddr_InfoMessageBox06 =			0x508837;
int callAddr_InfoMessageBox07 =			0x57D9A7;
int callAddr_InfoMessageBox08 =			0x57E417;
int callAddr_InfoMessageBox09 =			0x57E4E7;
int callAddr_InfoMessageBox10 =			0x57E5DC;
int callAddr_InfoMessageBox11 =			0x57E820;
int callAddr_InfoMessageBox12 =			0x57F2FD;
int callAddr_Encrypt01 =				0x51F52D;
int callAddr_Decrypt01 =				0x51FB39;
int callAddr_ShouldParseRecv01 =		0x4714A7;
*/
//10.11
/*
int funAddr_tibiaPrintText =			0x4D3710;
int funAddr_tibiaPlayerNameText =		0x4D2A60;
int funAddr_tibiaInfoMiddleScreen =		0x578750;
int funAddr_tibiaIsCreatureVisible =	0x47B790;
int funAddr_tibiaEncrypt =				0x58B4A0;
int funAddr_tibiaDecrypt =				0x58B5D0;
int funAddr_tibiaShouldParseRecv =		0x51E9B0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E748C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57C6A0;
int callAddr_PrintText01 =				0x46423D;//...<addr>.*
int callAddr_PrintText02 =				0x464287;
int callAddr_PrintText03 =				0x46D236;
int callAddr_PrintText04 =				0x511D95;
int callAddr_PrintText05 =				0x512D78;
int callAddr_PlayerNameText01 =			0x42B8C0;
int callAddr_PlayerNameText02 =			0x4D3394;
int callAddr_PlayerNameText03 =			0x4D3594;
int callAddr_PlayerNameText04 =			0x4D3793;
int callAddr_PlayerNameText05 =			0x4D3993;
int callAddr_PlayerNameText06 =			0x4D3B94;
int callAddr_PlayerNameText07 =			0x4D3D92;
int callAddr_PlayerNameText08 =			0x5128AF;
int callAddr_InfoMiddleScreen01 =		0x421BFC;
int callAddr_InfoMiddleScreen02 =		0x469218;
int callAddr_InfoMiddleScreen03 =		0x4A0FB5;
int callAddr_InfoMiddleScreen04 =		0x50FBE4;
int callAddr_InfoMessageBox01 =			0x421759;
int callAddr_InfoMessageBox02 =			0x4390A7;
int callAddr_InfoMessageBox03 =			0x4BA3D6;
int callAddr_InfoMessageBox04 =			0x506C20;
int callAddr_InfoMessageBox05 =			0x5073F3;
int callAddr_InfoMessageBox06 =			0x507457;
int callAddr_InfoMessageBox07 =			0x57C507;
int callAddr_InfoMessageBox08 =			0x57CF77;
int callAddr_InfoMessageBox09 =			0x57D047;
int callAddr_InfoMessageBox10 =			0x57D13C;
int callAddr_InfoMessageBox11 =			0x57D380;
int callAddr_InfoMessageBox12 =			0x57DE6D;
int callAddr_Encrypt01 =				0x51E09D;
int callAddr_Decrypt01 =				0x51E6A9;
int callAddr_ShouldParseRecv01 =		0x470387;
*/
//10.10
/*
int funAddr_tibiaPrintText =			0x4D3020;
int funAddr_tibiaPlayerNameText =		0x4D2370;
int funAddr_tibiaInfoMiddleScreen =		0x578160;
int funAddr_tibiaIsCreatureVisible =	0x47B080;
int funAddr_tibiaEncrypt =				0x58AE70;
int funAddr_tibiaDecrypt =				0x58AFA0;
int funAddr_tibiaShouldParseRecv =		0x51E230;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E657C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57C0B0;
int callAddr_PrintText01 =				0x463DDD;//...<addr>.*
int callAddr_PrintText02 =				0x463E27;
int callAddr_PrintText03 =				0x46CD06;
int callAddr_PrintText04 =				0x5107B5;
int callAddr_PrintText05 =				0x511798;
int callAddr_PlayerNameText01 =			0x42B410;
int callAddr_PlayerNameText02 =			0x4D2CA4;
int callAddr_PlayerNameText03 =			0x4D2EA4;
int callAddr_PlayerNameText04 =			0x4D30A3;
int callAddr_PlayerNameText05 =			0x4D32A3;
int callAddr_PlayerNameText06 =			0x4D34A4;
int callAddr_PlayerNameText07 =			0x4D36A2;
int callAddr_PlayerNameText08 =			0x5112CF;
int callAddr_InfoMiddleScreen01 =		0x424DCC;
int callAddr_InfoMiddleScreen02 =		0x468D78;
int callAddr_InfoMiddleScreen03 =		0x4A0955;
int callAddr_InfoMiddleScreen04 =		0x50E604;
int callAddr_InfoMessageBox01 =			0x424939;
int callAddr_InfoMessageBox02 =			0x438C07;
int callAddr_InfoMessageBox03 =			0x4B9D16;
int callAddr_InfoMessageBox04 =			0x505570;
int callAddr_InfoMessageBox05 =			0x505D43;
int callAddr_InfoMessageBox06 =			0x505DA7;
int callAddr_InfoMessageBox07 =			0x57BF17;
int callAddr_InfoMessageBox08 =			0x57C987;
int callAddr_InfoMessageBox09 =			0x57CA57;
int callAddr_InfoMessageBox10 =			0x57CB4C;
int callAddr_InfoMessageBox11 =			0x57CD90;
int callAddr_InfoMessageBox12 =			0x57D86D;
int callAddr_Encrypt01 =				0x51D91D;
int callAddr_Decrypt01 =				0x51DF29;
int callAddr_ShouldParseRecv01 =		0x46FE27;
*/
//10.01
/*
int funAddr_tibiaPrintText =			0x4D2CD0;
int funAddr_tibiaPlayerNameText =		0x4D2020;
int funAddr_tibiaInfoMiddleScreen =		0x578080;
int funAddr_tibiaIsCreatureVisible =	0x47AE30;
int funAddr_tibiaEncrypt =				0x58AD60;
int funAddr_tibiaDecrypt =				0x58AE90;
int funAddr_tibiaShouldParseRecv =		0x51DED0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E66DC-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57BFD0;
int callAddr_PrintText01 =				0x463C1D;//...<addr>.*
int callAddr_PrintText02 =				0x463C67;
int callAddr_PrintText03 =				0x46CBB6;
int callAddr_PrintText04 =				0x5103A5;
int callAddr_PrintText05 =				0x511388;
int callAddr_PlayerNameText01 =			0x42B3D0;
int callAddr_PlayerNameText02 =			0x4D2954;
int callAddr_PlayerNameText03 =			0x4D2B54;
int callAddr_PlayerNameText04 =			0x4D2D53;
int callAddr_PlayerNameText05 =			0x4D2F53;
int callAddr_PlayerNameText06 =			0x4D3154;
int callAddr_PlayerNameText07 =			0x4D3352;
int callAddr_PlayerNameText08 =			0x510EBF;
int callAddr_InfoMiddleScreen01 =		0x424E7C;
int callAddr_InfoMiddleScreen02 =		0x468C18;
int callAddr_InfoMiddleScreen03 =		0x4A0685;
int callAddr_InfoMiddleScreen04 =		0x50E1F4;
int callAddr_InfoMessageBox01 =			0x4249D9;
int callAddr_InfoMessageBox02 =			0x438C17;
int callAddr_InfoMessageBox03 =			0x4B9A06;
int callAddr_InfoMessageBox04 =			0x505250;
int callAddr_InfoMessageBox05 =			0x505A23;
int callAddr_InfoMessageBox06 =			0x505A87;
int callAddr_InfoMessageBox07 =			0x57BE37;
int callAddr_InfoMessageBox08 =			0x57C8A7;
int callAddr_InfoMessageBox09 =			0x57C977;
int callAddr_InfoMessageBox10 =			0x57CA6C;
int callAddr_InfoMessageBox11 =			0x57CCB0;
int callAddr_InfoMessageBox12 =			0x57D78D;
int callAddr_Encrypt01 =				0x51D5BD;
int callAddr_Decrypt01 =				0x51DBC9;
int callAddr_ShouldParseRecv01 =		0x46FCD7;
*/
//10.00
/*
int funAddr_tibiaPrintText =			0x4D2BE0;
int funAddr_tibiaPlayerNameText =		0x4D1F30;
int funAddr_tibiaInfoMiddleScreen =		0x577EC0;
int funAddr_tibiaIsCreatureVisible =	0x47ABB0;
int funAddr_tibiaEncrypt =				0x58AB70;
int funAddr_tibiaDecrypt =				0x58ACA0;
int funAddr_tibiaShouldParseRecv =		0x51DFB0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E66BC-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57BE10;
int callAddr_PrintText01 =				0x463A2D;//...<addr>.*
int callAddr_PrintText02 =				0x463A77;
int callAddr_PrintText03 =				0x46C936;
int callAddr_PrintText04 =				0x510515;
int callAddr_PrintText05 =				0x5114F8;
int callAddr_PlayerNameText01 =			0x42B3C0;
int callAddr_PlayerNameText02 =			0x4D2864;
int callAddr_PlayerNameText03 =			0x4D2A64;
int callAddr_PlayerNameText04 =			0x4D2C63;
int callAddr_PlayerNameText05 =			0x4D2E63;
int callAddr_PlayerNameText06 =			0x4D3064;
int callAddr_PlayerNameText07 =			0x4D3262;
int callAddr_PlayerNameText08 =			0x51102F;
int callAddr_InfoMiddleScreen01 =		0x424E7C;
int callAddr_InfoMiddleScreen02 =		0x4689B8;
int callAddr_InfoMiddleScreen03 =		0x4A04A5;
int callAddr_InfoMiddleScreen04 =		0x50E364;
int callAddr_InfoMessageBox01 =			0x4249D9;
int callAddr_InfoMessageBox02 =			0x438BC7;
int callAddr_InfoMessageBox03 =			0x4B9876;
int callAddr_InfoMessageBox04 =			0x5052E0;
int callAddr_InfoMessageBox05 =			0x505AB3;
int callAddr_InfoMessageBox06 =			0x505B17;
int callAddr_InfoMessageBox07 =			0x57BC77;
int callAddr_InfoMessageBox08 =			0x57C6E7;
int callAddr_InfoMessageBox09 =			0x57C7B7;
int callAddr_InfoMessageBox10 =			0x57C8AC;
int callAddr_InfoMessageBox11 =			0x57CAF0;
int callAddr_InfoMessageBox12 =			0x57D5CD;
int callAddr_Encrypt01 =				0x51D69D;
int callAddr_Decrypt01 =				0x51DCA9;
int callAddr_ShouldParseRecv01 =		0x46FA57;
*/
// 9.86
/*
int funAddr_tibiaPrintText =			0x4D3BC0;
int funAddr_tibiaPlayerNameText =		0x4D2F10;
int funAddr_tibiaInfoMiddleScreen =		0x577DC0;
int funAddr_tibiaIsCreatureVisible =	0x479450;
int funAddr_tibiaEncrypt =				0x58A930;
int funAddr_tibiaDecrypt =				0x58AA60;
int funAddr_tibiaShouldParseRecv =		0x51DDC0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E1B8C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57BD20;
int callAddr_PrintText01 =				0x4621FD;//...<addr>.*
int callAddr_PrintText02 =				0x462247;
int callAddr_PrintText03 =				0x46AFF6;
int callAddr_PrintText04 =				0x510987;
int callAddr_PrintText05 =				0x51175D;
int callAddr_PlayerNameText01 =			0x42A2B0;
int callAddr_PlayerNameText02 =			0x4D3844;
int callAddr_PlayerNameText03 =			0x4D3A44;
int callAddr_PlayerNameText04 =			0x4D3C43;
int callAddr_PlayerNameText05 =			0x4D3E43;
int callAddr_PlayerNameText06 =			0x4D4044;
int callAddr_PlayerNameText07 =			0x4D4242;
int callAddr_PlayerNameText08 =			0x5112A6;
int callAddr_InfoMiddleScreen01 =		0x423D0C;
int callAddr_InfoMiddleScreen02 =		0x467078;
int callAddr_InfoMiddleScreen03 =		0x4A17D5;
int callAddr_InfoMiddleScreen04 =		0x50E8D4;
int callAddr_InfoMessageBox01 =			0x423879;
int callAddr_InfoMessageBox02 =			0x437A77;
int callAddr_InfoMessageBox03 =			0x4BA9B6;
int callAddr_InfoMessageBox04 =			0x5058F0;
int callAddr_InfoMessageBox05 =			0x5060BC;
int callAddr_InfoMessageBox06 =			0x506120;
int callAddr_InfoMessageBox07 =			0x57BB87;
int callAddr_InfoMessageBox08 =			0x57C5F7;
int callAddr_InfoMessageBox09 =			0x57C6C7;
int callAddr_InfoMessageBox10 =			0x57C7BC;
int callAddr_InfoMessageBox11 =			0x57CA00;
int callAddr_InfoMessageBox12 =			0x57D4DD;
int callAddr_Encrypt01 =				0x51D4AD;
int callAddr_Decrypt01 =				0x51DAB9;
int callAddr_ShouldParseRecv01 =		0x46E127;
*/

// 9.85
/*
int funAddr_tibiaPrintText =			0x4D3BC0;
int funAddr_tibiaPlayerNameText =		0x4D2F10;
int funAddr_tibiaInfoMiddleScreen =		0x577DC0;
int funAddr_tibiaIsCreatureVisible =	0x479450;
int funAddr_tibiaEncrypt =				0x58A930;
int funAddr_tibiaDecrypt =				0x58AA60;
int funAddr_tibiaShouldParseRecv =		0x51DDC0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9E1B8C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57BD20;
int callAddr_PrintText01 =				0x4621FD;//...<addr>.*
int callAddr_PrintText02 =				0x462247;
int callAddr_PrintText03 =				0x46AFF6;
int callAddr_PrintText04 =				0x510987;
int callAddr_PrintText05 =				0x51175D;
int callAddr_PlayerNameText01 =			0x42A2B0;
int callAddr_PlayerNameText02 =			0x4D3844;
int callAddr_PlayerNameText03 =			0x4D3A44;
int callAddr_PlayerNameText04 =			0x4D3C43;
int callAddr_PlayerNameText05 =			0x4D3E43;
int callAddr_PlayerNameText06 =			0x4D4044;
int callAddr_PlayerNameText07 =			0x4D4242;
int callAddr_PlayerNameText08 =			0x5112A6;
int callAddr_InfoMiddleScreen01 =		0x423D0C;
int callAddr_InfoMiddleScreen02 =		0x467078;
int callAddr_InfoMiddleScreen03 =		0x4A17D5;
int callAddr_InfoMiddleScreen04 =		0x50E8D4;
int callAddr_InfoMessageBox01 =			0x423879;
int callAddr_InfoMessageBox02 =			0x437A77;
int callAddr_InfoMessageBox03 =			0x4BA9B6;
int callAddr_InfoMessageBox04 =			0x5058F0;
int callAddr_InfoMessageBox05 =			0x5060BC;
int callAddr_InfoMessageBox06 =			0x506120;
int callAddr_InfoMessageBox07 =			0x57BB87;
int callAddr_InfoMessageBox08 =			0x57C5F7;
int callAddr_InfoMessageBox09 =			0x57C6C7;
int callAddr_InfoMessageBox10 =			0x57C7BC;
int callAddr_InfoMessageBox11 =			0x57CA00;
int callAddr_InfoMessageBox12 =			0x57D4DD;
int callAddr_Encrypt01 =				0x51D4AD;
int callAddr_Decrypt01 =				0x51DAB9;
int callAddr_ShouldParseRecv01 =		0x46E127;
*/

// 9.82
/*
int funAddr_tibiaPrintText =			0x4D0C80;
int funAddr_tibiaPlayerNameText =		0x4CFFD0;
int funAddr_tibiaInfoMiddleScreen =		0x578090;
int funAddr_tibiaIsCreatureVisible =	0x4766B0;
int funAddr_tibiaEncrypt =				0x588550;
int funAddr_tibiaDecrypt =				0x588680;
int funAddr_tibiaShouldParseRecv =		0x51DFF0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9DE6E4-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x57BFE0;
int callAddr_PrintText01 =				0x45F54D;//...<addr>.*
int callAddr_PrintText02 =				0x45F597;
int callAddr_PrintText03 =				0x4681E6;
int callAddr_PrintText04 =				0x510BC7;
int callAddr_PrintText05 =				0x51199D;
int callAddr_PlayerNameText01 =			0x429760;
int callAddr_PlayerNameText02 =			0x4D0904;
int callAddr_PlayerNameText03 =			0x4D0B04;
int callAddr_PlayerNameText04 =			0x4D0D03;
int callAddr_PlayerNameText05 =			0x4D0F03;
int callAddr_PlayerNameText06 =			0x4D1104;
int callAddr_PlayerNameText07 =			0x4D1302;
int callAddr_PlayerNameText08 =			0x5114E6;
int callAddr_InfoMiddleScreen01 =		0x423BAC;
int callAddr_InfoMiddleScreen02 =		0x464268;
int callAddr_InfoMiddleScreen03 =		0x49E845;
int callAddr_InfoMiddleScreen04 =		0x50EB14;
int callAddr_InfoMessageBox01 =			0x423719;
int callAddr_InfoMessageBox02 =			0x436F27;
int callAddr_InfoMessageBox03 =			0x4B7A16;
int callAddr_InfoMessageBox04 =			0x505AE0;
int callAddr_InfoMessageBox05 =			0x5062AC;
int callAddr_InfoMessageBox06 =			0x506310;
int callAddr_InfoMessageBox07 =			0x57BE47;
int callAddr_InfoMessageBox08 =			0x57C8B7;
int callAddr_InfoMessageBox09 =			0x57C987;
int callAddr_InfoMessageBox10 =			0x57CA7C;
int callAddr_InfoMessageBox11 =			0x57CCC0;
int callAddr_InfoMessageBox12 =			0x57D79D;
int callAddr_Encrypt01 =				0x51D6DD;
int callAddr_Decrypt01 =				0x51DCE9;
int callAddr_ShouldParseRecv01 =		0x46B337;
*/

// 9.80
/*
int funAddr_tibiaPrintText =			0x4CE800;
int funAddr_tibiaPlayerNameText =		0x4CDB50;
int funAddr_tibiaInfoMiddleScreen =		0x575E00;
int funAddr_tibiaIsCreatureVisible =	0x473F80;
int funAddr_tibiaEncrypt =				0x586360;
int funAddr_tibiaDecrypt =				0x586490;
int funAddr_tibiaShouldParseRecv =		0x51BEE0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9DA5FC-8; //look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x579D50;
int callAddr_PrintText01 =				0x45CE4D; //...<addr>.*
int callAddr_PrintText02 =				0x45CE97;
int callAddr_PrintText03 =				0x465AE6;
int callAddr_PrintText04 =				0x50E7F1;
int callAddr_PrintText05 =				0x50F5BA;
int callAddr_PlayerNameText01 =			0x426FD0;
int callAddr_PlayerNameText02 =			0x4CE484;
int callAddr_PlayerNameText03 =			0x4CE684;
int callAddr_PlayerNameText04 =			0x4CE883;
int callAddr_PlayerNameText05 =			0x4CEA83;
int callAddr_PlayerNameText06 =			0x4CEC84;
int callAddr_PlayerNameText07 =			0x4CEE82;
int callAddr_PlayerNameText08 =			0x50F110;
int callAddr_InfoMiddleScreen01 =		0x4213BC;
int callAddr_InfoMiddleScreen02 =		0x461B08;
int callAddr_InfoMiddleScreen03 =		0x49C2B5;
int callAddr_InfoMiddleScreen04 =		0x50C6E4;
int callAddr_InfoMessageBox01 =			0x420F29;
int callAddr_InfoMessageBox02 =			0x434787;
int callAddr_InfoMessageBox03 =			0x4B54B6;
int callAddr_InfoMessageBox04 =			0x503610;
int callAddr_InfoMessageBox05 =			0x503DE3;
int callAddr_InfoMessageBox06 =			0x503E47;
int callAddr_InfoMessageBox07 =			0x579BB7;
int callAddr_InfoMessageBox08 =			0x57A627;
int callAddr_InfoMessageBox09 =			0x57A6F7;
int callAddr_InfoMessageBox10 =			0x57A7EC;
int callAddr_InfoMessageBox11 =			0x57AA30;
int callAddr_InfoMessageBox12 =			0x57B50D;
int callAddr_Encrypt01 =				0x51B5CD;
int callAddr_Decrypt01 =				0x51BBD9;
int callAddr_ShouldParseRecv01 =		0x468C17;
*/
// 9.71
/*
int funAddr_tibiaPrintText =			0x4CD750;
int funAddr_tibiaPlayerNameText =		0x4CCAA0;
int funAddr_tibiaInfoMiddleScreen =		0x574210;
int funAddr_tibiaIsCreatureVisible =	0x473180;
int funAddr_tibiaEncrypt =				0x5847B0;
int funAddr_tibiaDecrypt =				0x5848E0;
int funAddr_tibiaShouldParseRecv =		0x51ACC0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9D539C-8; //look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x578160;
int callAddr_PrintText01 =				0x45C5DD; //...<addr>.*
int callAddr_PrintText02 =				0x45C627;
int callAddr_PrintText03 =				0x4652D6;
int callAddr_PrintText04 =				0x50D5E1;
int callAddr_PrintText05 =				0x50E3AA;
int callAddr_PlayerNameText01 =			0x426A30;
int callAddr_PlayerNameText02 =			0x4CD3D4;
int callAddr_PlayerNameText03 =			0x4CD5D4;
int callAddr_PlayerNameText04 =			0x4CD7D3;
int callAddr_PlayerNameText05 =			0x4CD9D3;
int callAddr_PlayerNameText06 =			0x4CDBD4;
int callAddr_PlayerNameText07 =			0x4CDDD2;
int callAddr_PlayerNameText08 =			0x50DF00;
int callAddr_InfoMiddleScreen01 =		0x420C8C;
int callAddr_InfoMiddleScreen02 =		0x461348;
int callAddr_InfoMiddleScreen03 =		0x49B3F5;
int callAddr_InfoMiddleScreen04 =		0x50B4E4;
int callAddr_InfoMessageBox01 =			0x4207F9;
int callAddr_InfoMessageBox02 =			0x4341D7;
int callAddr_InfoMessageBox03 =			0x4B4592;
int callAddr_InfoMessageBox04 =			0x502340;
int callAddr_InfoMessageBox05 =			0x502B0C;
int callAddr_InfoMessageBox06 =			0x502B70;
int callAddr_InfoMessageBox07 =			0x577FC7;
int callAddr_InfoMessageBox08 =			0x578A37;
int callAddr_InfoMessageBox09 =			0x578B07;
int callAddr_InfoMessageBox10 =			0x578BFC;
int callAddr_InfoMessageBox11 =			0x578E40;
int callAddr_InfoMessageBox12 =			0x57991D;
int callAddr_Encrypt01 =				0x51A3AD;
int callAddr_Decrypt01 =				0x51A9B8;
int callAddr_ShouldParseRecv01 =		0x467FD7;
*/
// 9.70
/*
int funAddr_tibiaPrintText =			0x4CB8B0;
int funAddr_tibiaPlayerNameText =		0x4CAC00;
int funAddr_tibiaInfoMiddleScreen =		0x573AA0;
int funAddr_tibiaIsCreatureVisible =	0x471610;
int funAddr_tibiaEncrypt =				0x584020;
int funAddr_tibiaDecrypt =				0x584150;
int funAddr_tibiaShouldParseRecv =		0x518D20;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9D45CC-8; //look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x577A20;
int callAddr_PrintText01 =				0x45A93D; //...<addr>.*
int callAddr_PrintText02 =				0x45A987;
int callAddr_PrintText03 =				0x463696;
int callAddr_PrintText04 =				0x50B5C1;
int callAddr_PrintText05 =				0x50C38A;
int callAddr_PlayerNameText01 =			0x424D80;
int callAddr_PlayerNameText02 =			0x4CB534;
int callAddr_PlayerNameText03 =			0x4CB734;
int callAddr_PlayerNameText04 =			0x4CB933;
int callAddr_PlayerNameText05 =			0x4CBB33;
int callAddr_PlayerNameText06 =			0x4CBD34;
int callAddr_PlayerNameText07 =			0x4CBF32;
int callAddr_PlayerNameText08 =			0x50BEE0;
int callAddr_InfoMiddleScreen01 =		0x41B83C;
int callAddr_InfoMiddleScreen02 =		0x45F658;
int callAddr_InfoMiddleScreen03 =		0x499845;
int callAddr_InfoMiddleScreen04 =		0x5094C4;
int callAddr_InfoMessageBox01 =			0x41B3A9;
int callAddr_InfoMessageBox02 =			0x432537;
int callAddr_InfoMessageBox03 =			0x4B2782;
int callAddr_InfoMessageBox04 =			0x5003C0;
int callAddr_InfoMessageBox05 =			0x500B8C;
int callAddr_InfoMessageBox06 =			0x500BF0;
int callAddr_InfoMessageBox07 =			0x577887;
int callAddr_InfoMessageBox08 =			0x5782F7;
int callAddr_InfoMessageBox09 =			0x5783C7;
int callAddr_InfoMessageBox10 =			0x5784BC;
int callAddr_InfoMessageBox11 =			0x578700;
int callAddr_InfoMessageBox12 =			0x5791DD;
int callAddr_Encrypt01 =				0x51840D;
int callAddr_Decrypt01 =				0x518A18;
int callAddr_ShouldParseRecv01 =		0x466330;
*/
// 9.63
/*
int funAddr_tibiaPrintText =			0x4CB5A0;
int funAddr_tibiaPlayerNameText =		0x4CA8F0;
int funAddr_tibiaInfoMiddleScreen =		0x5736D0;
int funAddr_tibiaIsCreatureVisible =	0x4715F0;
int funAddr_tibiaEncrypt =				0x583BD0;
int funAddr_tibiaDecrypt =				0x583D00;
int funAddr_tibiaShouldParseRecv =		0x5187E0;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0x9D457C-8; //look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x577620;
int callAddr_PrintText01 =				0x45A97D; //...<addr>.*
int callAddr_PrintText02 =				0x45A9C7;
int callAddr_PrintText03 =				0x463666;
int callAddr_PrintText04 =				0x50B101;
int callAddr_PrintText05 =				0x50BECA;
int callAddr_PlayerNameText01 =			0x424D60;
int callAddr_PlayerNameText02 =			0x4CB224;
int callAddr_PlayerNameText03 =			0x4CB424;
int callAddr_PlayerNameText04 =			0x4CB623;
int callAddr_PlayerNameText05 =			0x4CB823;
int callAddr_PlayerNameText06 =			0x4CBA24;
int callAddr_PlayerNameText07 =			0x4CBC22;
int callAddr_PlayerNameText08 =			0x50BA20;
int callAddr_InfoMiddleScreen01 =		0x41B7B7;
int callAddr_InfoMiddleScreen02 =		0x45F638;
int callAddr_InfoMiddleScreen03 =		0x4997A5;
int callAddr_InfoMiddleScreen04 =		0x509004;
int callAddr_InfoMessageBox01 =			0x41B329;
int callAddr_InfoMessageBox02 =			0x4325B7;
int callAddr_InfoMessageBox03 =			0x4B2712;
int callAddr_InfoMessageBox04 =			0x500010;
int callAddr_InfoMessageBox05 =			0x5007DC;
int callAddr_InfoMessageBox06 =			0x500840;
int callAddr_InfoMessageBox07 =			0x577487;
int callAddr_InfoMessageBox08 =			0x577EF7;
int callAddr_InfoMessageBox09 =			0x577FC7;
int callAddr_InfoMessageBox10 =			0x5780BC;
int callAddr_InfoMessageBox11 =			0x578300;
int callAddr_InfoMessageBox12 =			0x578DDD;
int callAddr_Encrypt01 =				0x517ECD;
int callAddr_Decrypt01 =				0x514FF8;
int callAddr_ShouldParseRecv01 =		0x4662C0;
*/

// 9.61
/*
int funAddr_tibiaPrintText =			0x4C8D40;
int funAddr_tibiaPlayerNameText =		0x4C8090;
int funAddr_tibiaInfoMiddleScreen =		0x570A10;
int funAddr_tibiaIsCreatureVisible =	0x46C540;
int funAddr_tibiaEncrypt =				0x580FB0;
int funAddr_tibiaDecrypt =				0x5810E0;
int funAddr_tibiaShouldParseRecv =		0x515300;
int arrayPtr_recvStream =				0x9D1FCC-8; //look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x574960;
int callAddr_PrintText01 =				0x45813D;
int callAddr_PrintText02 =				0x458187;
int callAddr_PrintText03 =				0x460726;
int callAddr_PrintText04 =				0x507C21;
int callAddr_PrintText05 =				0x5089EA;
int callAddr_PlayerNameText01 =			0x4225A0;
int callAddr_PlayerNameText02 =			0x4C89C4;
int callAddr_PlayerNameText03 =			0x4C8BC4;
int callAddr_PlayerNameText04 =			0x4C8DC3;
int callAddr_PlayerNameText05 =			0x4C8FC3;
int callAddr_PlayerNameText06 =			0x4C91C4;
int callAddr_PlayerNameText07 =			0x4C93C2;
int callAddr_PlayerNameText08 =			0x508540;
int callAddr_InfoMiddleScreen01 =		0x418BB7;
int callAddr_InfoMiddleScreen02 =		0x45CD98;
int callAddr_InfoMiddleScreen03 =		0x4979C5;
int callAddr_InfoMiddleScreen04 =		0x505B24;
int callAddr_InfoMessageBox01 =			0x418729;
int callAddr_InfoMessageBox02 =			0x42FD57;
int callAddr_InfoMessageBox03 =			0x4B0842;
int callAddr_InfoMessageBox04 =			0x4FD540;
int callAddr_InfoMessageBox05 =			0x4FDD0C;
int callAddr_InfoMessageBox06 =			0x4FDD70;
int callAddr_InfoMessageBox07 =			0x5747C7;
int callAddr_InfoMessageBox08 =			0x575237;
int callAddr_InfoMessageBox09 =			0x575307;
int callAddr_InfoMessageBox10 =			0x5753FC;
int callAddr_InfoMessageBox11 =			0x575640;
int callAddr_InfoMessageBox12 =			0x57611D;
int callAddr_Encrypt01 =				0x5149DD;
int callAddr_Decrypt01 =				0x514FF8;
int callAddr_ShouldParseRecv01 =		0x463380;
*/
/*
// 9.46
int funAddr_tibiaPrintText = 0x4C5BD0;
int funAddr_tibiaPlayerNameText = 0x4C4F20;
int funAddr_tibiaInfoMiddleScreen = 0x56CAB0;
int funAddr_tibiaIsCreatureVisible = 0x46A220;
int funAddr_tibiaEncrypt = 0x57D060;
int funAddr_tibiaDecrypt = 0x57D190;
int funAddr_tibiaShouldParseRecv = 0x510AA0;
int arrayPtr_recvStream = 0x9E6C30-8; //look for this address near above location, it will be off by 8
int funAddr_tibiaInfoMessageBox = 0x570A20;
int callAddr_PrintText01 = 0x455D01;
int callAddr_PrintText02 = 0x455D4B;
int callAddr_PrintText03 = 0x45E390;
int callAddr_PrintText04 = 0x504507;
int callAddr_PrintText05 = 0x5052CA;
int callAddr_PlayerNameText01 = 0x420C4B;
int callAddr_PlayerNameText02 = 0x4C5854;
int callAddr_PlayerNameText03 = 0x4C5A54;
int callAddr_PlayerNameText04 = 0x4C5C53;
int callAddr_PlayerNameText05 = 0x4C5E53;
int callAddr_PlayerNameText06 = 0x4C6054;
int callAddr_PlayerNameText07 = 0x4C6252;
int callAddr_PlayerNameText08 = 0x504E1A;
int callAddr_InfoMiddleScreen01 = 0x417DAA;
int callAddr_InfoMiddleScreen02 = 0x45A9AA;
int callAddr_InfoMiddleScreen03 = 0x4953F5;
int callAddr_InfoMiddleScreen04 = 0x502414;
int callAddr_InfoMessageBox01 = 0x417919;
int callAddr_InfoMessageBox02 = 0x42E397;
int callAddr_InfoMessageBox03 = 0x4ADDE2;
int callAddr_InfoMessageBox04 = 0x4F9E20;
int callAddr_InfoMessageBox05 = 0x4FA5EC;
int callAddr_InfoMessageBox06 = 0x4FA650;
int callAddr_InfoMessageBox07 = 0x570887;
int callAddr_InfoMessageBox08 = 0x5712F7;
int callAddr_InfoMessageBox09 = 0x5713C7;
int callAddr_InfoMessageBox10 = 0x5714BC;
int callAddr_InfoMessageBox11 = 0x571700;
int callAddr_InfoMessageBox12 = 0x5721DD;
int callAddr_Encrypt01 = 0x51018D;
int callAddr_Decrypt01 = 0x510798;
int callAddr_ShouldParseRecv01 = 0x460E23;
*/
/*
// 9.50
int funAddr_tibiaPrintText =			0x4C6620;
int funAddr_tibiaPlayerNameText =		0x4C5970;
int funAddr_tibiaInfoMiddleScreen =		0x56D890;
int funAddr_tibiaIsCreatureVisible =	0x46AC90;
int funAddr_tibiaEncrypt =				0x57DE30;
int funAddr_tibiaDecrypt =				0x57DF60;
int funAddr_tibiaShouldParseRecv =		0x5115D0;
int arrayPtr_recvStream =				0x9E8178-8; //look for this address near above location, it will be off by 8
int funAddr_tibiaInfoMessageBox =		0x5717E0;
int callAddr_PrintText01 =				0x456581;
int callAddr_PrintText02 =				0x4565CB;
int callAddr_PrintText03 =				0x45ECC6;
int callAddr_PrintText04 =				0x504FD7;
int callAddr_PrintText05 =				0x505D9A;
int callAddr_PlayerNameText01 =			0x42145B;
int callAddr_PlayerNameText02 =			0x4C62A4;
int callAddr_PlayerNameText03 =			0x4C64A4;
int callAddr_PlayerNameText04 =			0x4C66A3;
int callAddr_PlayerNameText05 =			0x4C68A3;
int callAddr_PlayerNameText06 =			0x4C6AA4;
int callAddr_PlayerNameText07 =			0x4C6CA2;
int callAddr_PlayerNameText08 =			0x5058EA;
int callAddr_InfoMiddleScreen01 =		0x4185BA;
int callAddr_InfoMiddleScreen02 =		0x45B1FA;
int callAddr_InfoMiddleScreen03 =		0x495E85;
int callAddr_InfoMiddleScreen04 =		0x502ED4;
int callAddr_InfoMessageBox01 =			0x418129;
int callAddr_InfoMessageBox02 =			0x42EC27;
int callAddr_InfoMessageBox03 =			0x4AE842;
int callAddr_InfoMessageBox04 =			0x4FA8B0;
int callAddr_InfoMessageBox05 =			0x4FB07C;
int callAddr_InfoMessageBox06 =			0x4FB0E0;
int callAddr_InfoMessageBox07 =			0x571647;
int callAddr_InfoMessageBox08 =			0x5720B7;
int callAddr_InfoMessageBox09 =			0x572187;
int callAddr_InfoMessageBox10 =			0x57227C;
int callAddr_InfoMessageBox11 =			0x5724C0;
int callAddr_InfoMessageBox12 =			0x572FAD;
int callAddr_Encrypt01 =				0x510CBD;
int callAddr_Decrypt01 =				0x5112C8;
int callAddr_ShouldParseRecv01 =		0x461783;
*/
/*
// 9.51
int funAddr_tibiaPrintText =			0x4C6620;
int funAddr_tibiaPlayerNameText =		0x4C5970;
int funAddr_tibiaInfoMiddleScreen =		0x56D890;
int funAddr_tibiaIsCreatureVisible =	0x46AC90;
int funAddr_tibiaEncrypt =				0x57DE30;
int funAddr_tibiaDecrypt =				0x57DF60;
int funAddr_tibiaShouldParseRecv =		0x5115D0;
int arrayPtr_recvStream =				0x9E82B8-8; //look for this address near above location, it will be off by 8
int funAddr_tibiaInfoMessageBox =		0x5717E0;
int callAddr_PrintText01 =				0x456581;
int callAddr_PrintText02 =				0x4565CB;
int callAddr_PrintText03 =				0x45ECC6;
int callAddr_PrintText04 =				0x504FD7;
int callAddr_PrintText05 =				0x505D9A;
int callAddr_PlayerNameText01 =			0x42145B;
int callAddr_PlayerNameText02 =			0x4C62A4;
int callAddr_PlayerNameText03 =			0x4C64A4;
int callAddr_PlayerNameText04 =			0x4C66A3;
int callAddr_PlayerNameText05 =			0x4C68A3;
int callAddr_PlayerNameText06 =			0x4C6AA4;
int callAddr_PlayerNameText07 =			0x4C6CA2;
int callAddr_PlayerNameText08 =			0x5058EA;
int callAddr_InfoMiddleScreen01 =		0x4185BA;
int callAddr_InfoMiddleScreen02 =		0x45B1FA;
int callAddr_InfoMiddleScreen03 =		0x495E85;
int callAddr_InfoMiddleScreen04 =		0x502ED4;
int callAddr_InfoMessageBox01 =			0x418129;
int callAddr_InfoMessageBox02 =			0x42EC27;
int callAddr_InfoMessageBox03 =			0x4AE842;
int callAddr_InfoMessageBox04 =			0x4FA8B0;
int callAddr_InfoMessageBox05 =			0x4FB07C;
int callAddr_InfoMessageBox06 =			0x4FB0E0;
int callAddr_InfoMessageBox07 =			0x571647;
int callAddr_InfoMessageBox08 =			0x5720B7;
int callAddr_InfoMessageBox09 =			0x572187;
int callAddr_InfoMessageBox10 =			0x57227C;
int callAddr_InfoMessageBox11 =			0x5724C0;
int callAddr_InfoMessageBox12 =			0x572FAD;
int callAddr_Encrypt01 =				0x510CBD;
int callAddr_Decrypt01 =				0x5112C8;
int callAddr_ShouldParseRecv01 =		0x461783;
*/

// 9.52
/*
int funAddr_tibiaPrintText =			0x4C6620;
int funAddr_tibiaPlayerNameText =		0x4C5970;
int funAddr_tibiaInfoMiddleScreen =		0x56D890;
int funAddr_tibiaIsCreatureVisible =	0x46AC90;
int funAddr_tibiaEncrypt =				0x57DE30;
int funAddr_tibiaDecrypt =				0x57DF60;
int funAddr_tibiaShouldParseRecv =		0x5115D0;
int arrayPtr_recvStream =				0x9E7F00-8; //look for this address near above location, it will be off by 8
int funAddr_tibiaInfoMessageBox =		0x5717E0;
int callAddr_PrintText01 =				0x456581;
int callAddr_PrintText02 =				0x4565CB;
int callAddr_PrintText03 =				0x45ECC6;
int callAddr_PrintText04 =				0x504FD7;
int callAddr_PrintText05 =				0x505D9A;
int callAddr_PlayerNameText01 =			0x42145B;
int callAddr_PlayerNameText02 =			0x4C62A4;
int callAddr_PlayerNameText03 =			0x4C64A4;
int callAddr_PlayerNameText04 =			0x4C66A3;
int callAddr_PlayerNameText05 =			0x4C68A3;
int callAddr_PlayerNameText06 =			0x4C6AA4;
int callAddr_PlayerNameText07 =			0x4C6CA2;
int callAddr_PlayerNameText08 =			0x5058EA;
int callAddr_InfoMiddleScreen01 =		0x4185BA;
int callAddr_InfoMiddleScreen02 =		0x45B1FA;
int callAddr_InfoMiddleScreen03 =		0x495E85;
int callAddr_InfoMiddleScreen04 =		0x502ED4;
int callAddr_InfoMessageBox01 =			0x418129;
int callAddr_InfoMessageBox02 =			0x42EC27;
int callAddr_InfoMessageBox03 =			0x4AE842;
int callAddr_InfoMessageBox04 =			0x4FA8B0;
int callAddr_InfoMessageBox05 =			0x4FB07C;
int callAddr_InfoMessageBox06 =			0x4FB0E0;
int callAddr_InfoMessageBox07 =			0x571647;
int callAddr_InfoMessageBox08 =			0x5720B7;
int callAddr_InfoMessageBox09 =			0x572187;
int callAddr_InfoMessageBox10 =			0x57227C;
int callAddr_InfoMessageBox11 =			0x5724C0;
int callAddr_InfoMessageBox12 =			0x572FAD;
int callAddr_Encrypt01 =				0x510CBD;
int callAddr_Decrypt01 =				0x5112C8;
int callAddr_ShouldParseRecv01 =		0x461783;
*/

// 9.53
/*
int funAddr_tibiaPrintText =			0x4C6620;
int funAddr_tibiaPlayerNameText =		0x4C5970;
int funAddr_tibiaInfoMiddleScreen =		0x56DAD0;
int funAddr_tibiaIsCreatureVisible =	0x46AC60;
int funAddr_tibiaEncrypt =				0x57E0C0;
int funAddr_tibiaDecrypt =				0x57E1F0;
int funAddr_tibiaShouldParseRecv =		0x5115D0;
int arrayPtr_recvStream =				0x9E9518-8; //look for this address near above location, it will be off by 8
int funAddr_tibiaInfoMessageBox =		0x571A20;
int callAddr_PrintText01 =				0x456571;
int callAddr_PrintText02 =				0x4565BB;
int callAddr_PrintText03 =				0x45ECA6;
int callAddr_PrintText04 =				0x504FD7;
int callAddr_PrintText05 =				0x505D9A;
int callAddr_PlayerNameText01 =			0x42145B;
int callAddr_PlayerNameText02 =			0x4C62A4;
int callAddr_PlayerNameText03 =			0x4C64A4;
int callAddr_PlayerNameText04 =			0x4C66A3;
int callAddr_PlayerNameText05 =			0x4C68A3;
int callAddr_PlayerNameText06 =			0x4C6AA4;
int callAddr_PlayerNameText07 =			0x4C6CA2;
int callAddr_PlayerNameText08 =			0x5058EA;
int callAddr_InfoMiddleScreen01 =		0x4185BA;
int callAddr_InfoMiddleScreen02 =		0x45B17A;
int callAddr_InfoMiddleScreen03 =		0x495E15;
int callAddr_InfoMiddleScreen04 =		0x502ED4;
int callAddr_InfoMessageBox01 =			0x418129;
int callAddr_InfoMessageBox02 =			0x42EC27;
int callAddr_InfoMessageBox03 =			0x4AE822;
int callAddr_InfoMessageBox04 =			0x4FA8B0;
int callAddr_InfoMessageBox05 =			0x4FB07C;
int callAddr_InfoMessageBox06 =			0x4FB0E0;
int callAddr_InfoMessageBox07 =			0x571887;
int callAddr_InfoMessageBox08 =			0x5722F7;
int callAddr_InfoMessageBox09 =			0x5723C7;
int callAddr_InfoMessageBox10 =			0x5724BC;
int callAddr_InfoMessageBox11 =			0x572700;
int callAddr_InfoMessageBox12 =			0x5731DD;
int callAddr_Encrypt01 =				0x510CBD;
int callAddr_Decrypt01 =				0x5112C8;
int callAddr_ShouldParseRecv01 =		0x461763;
*/

// 9.54
/*
int funAddr_tibiaPrintText =			0x4C8890;
int funAddr_tibiaPlayerNameText =		0x4C7BE0;
int funAddr_tibiaInfoMiddleScreen =		0x570240;
int funAddr_tibiaIsCreatureVisible =	0x46C2D0;
int funAddr_tibiaEncrypt =				0x580840;
int funAddr_tibiaDecrypt =				0x580970;
int funAddr_tibiaShouldParseRecv =		0x513910;
int arrayPtr_recvStream =				0x9EDA6C-8; //look for this address near above location, it will be off by 8
int funAddr_tibiaInfoMessageBox =		0x574190;
int callAddr_PrintText01 =				0x457B8D;
int callAddr_PrintText02 =				0x457BD7;
int callAddr_PrintText03 =				0x460101;
int callAddr_PrintText04 =				0x507301;
int callAddr_PrintText05 =				0x5080CA;
int callAddr_PlayerNameText01 =			0x422110;
int callAddr_PlayerNameText02 =			0x4C8514;
int callAddr_PlayerNameText03 =			0x4C8714;
int callAddr_PlayerNameText04 =			0x4C8913;
int callAddr_PlayerNameText05 =			0x4C8B13;
int callAddr_PlayerNameText06 =			0x4C8D14;
int callAddr_PlayerNameText07 =			0x4C8F22;
int callAddr_PlayerNameText08 =			0x507C20;
int callAddr_InfoMiddleScreen01 =		0x41873A;
int callAddr_InfoMiddleScreen02 =		0x45C78A;
int callAddr_InfoMiddleScreen03 =		0x4975E5;
int callAddr_InfoMiddleScreen04 =		0x5051F4;
int callAddr_InfoMessageBox01 =			0x4182A9;
int callAddr_InfoMessageBox02 =			0x42F8D7;
int callAddr_InfoMessageBox03 =			0x4B0452;
int callAddr_InfoMessageBox04 =			0x4FCBF0;
int callAddr_InfoMessageBox05 =			0x4FD3BC;
int callAddr_InfoMessageBox06 =			0x4FD420;
int callAddr_InfoMessageBox07 =			0x573FF7;
int callAddr_InfoMessageBox08 =			0x574A67;
int callAddr_InfoMessageBox09 =			0x574B37;
int callAddr_InfoMessageBox10 =			0x574C2C;
int callAddr_InfoMessageBox11 =			0x574E70;
int callAddr_InfoMessageBox12 =			0x57594D;
int callAddr_Encrypt01 =				0x512FFD;
int callAddr_Decrypt01 =				0x513608;
int callAddr_ShouldParseRecv01 =		0x462D93;
*/

// 9.60
/*
int funAddr_tibiaPrintText =			0x4C8A60;
int funAddr_tibiaPlayerNameText =		0x4C7DB0;
int funAddr_tibiaInfoMiddleScreen =		0x570890;
int funAddr_tibiaIsCreatureVisible =	0x46C310;
int funAddr_tibiaEncrypt =				0x580DE0;
int funAddr_tibiaDecrypt =				0x580F10;
int funAddr_tibiaShouldParseRecv =		0x5150B0;
int arrayPtr_recvStream =				0x9D1D2C-8; //look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x5747E0;
int callAddr_PrintText01 =				0x457EED;
int callAddr_PrintText02 =				0x457F37;
int callAddr_PrintText03 =				0x4604D6;
int callAddr_PrintText04 =				0x507951;
int callAddr_PrintText05 =				0x50871A;
int callAddr_PlayerNameText01 =			0x422410;
int callAddr_PlayerNameText02 =			0x4C86E4;
int callAddr_PlayerNameText03 =			0x4C88E4;
int callAddr_PlayerNameText04 =			0x4C8AE3;
int callAddr_PlayerNameText05 =			0x4C8CE3;
int callAddr_PlayerNameText06 =			0x4C8EE4;
int callAddr_PlayerNameText07 =			0x4C90E2;
int callAddr_PlayerNameText08 =			0x508270;
int callAddr_InfoMiddleScreen01 =		0x418A07;
int callAddr_InfoMiddleScreen02 =		0x45CAF8;
int callAddr_InfoMiddleScreen03 =		0x497755;
int callAddr_InfoMiddleScreen04 =		0x505834;
int callAddr_InfoMessageBox01 =			0x418579;
int callAddr_InfoMessageBox02 =			0x42FBC7;
int callAddr_InfoMessageBox03 =			0x4B05F2;
int callAddr_InfoMessageBox04 =			0x4FD250;
int callAddr_InfoMessageBox05 =			0x4FDA1C;
int callAddr_InfoMessageBox06 =			0x4FDA80;
int callAddr_InfoMessageBox07 =			0x574647;
int callAddr_InfoMessageBox08 =			0x5750B7;
int callAddr_InfoMessageBox09 =			0x575187;
int callAddr_InfoMessageBox10 =			0x57527C;
int callAddr_InfoMessageBox11 =			0x5754C0;
int callAddr_InfoMessageBox12 =			0x575FAD;
int callAddr_Encrypt01 =				0x51479D;
int callAddr_Decrypt01 =				0x514DA8;
int callAddr_ShouldParseRecv01 =		0x463130;
*/


//int recv2 = (void (*)())(*DetourFindFunction("wsock32.dll","recv"));
DETOUR_TRAMPOLINE(int WINAPI Real_send(SOCKET s,char* buf,int len,int flags),send);
//DETOUR_TRAMPOLINE(int WINAPI Real_recv(SOCKET s,char* buf,int len,int flags),recv);
DETOUR_TRAMPOLINE(int WINAPI Real_connect(SOCKET s,const struct sockaddr* name,int namelen),connect);
DETOUR_TRAMPOLINE(SOCKET WINAPI Real_socket(int af,int type,int protocol),socket);
DETOUR_TRAMPOLINE(int Real_select(int nfds,fd_set* readfds,fd_set* writefds,fd_set* exceptfds,const struct timeval* timeout),select);

//there are two recv functions and "recv" specifies the wrong one
static PVOID __fastcall _Detours_GetVA_recv(VOID)
{
    return (PVOID)DetourFindFunction("wsock32.dll","recv");
}

__declspec(naked) int WINAPI Real_recv(SOCKET s,char* buf,int len,int flags)
{
    __asm { nop };
    __asm { nop };
    __asm { call _Detours_GetVA_recv };
    __asm { jmp eax };
    __asm { ret };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
    __asm { nop };
}

int WINAPI Mine_send(SOCKET s,char* buf,int len,int flags);
char*  adler(char *data, size_t len);

void InitialiseCommunication();
void InitialiseHooks();
void InitialiseDebugFile();

void WriteOutDebug(const char * format,...){
	if(debugFile){
		va_list args;
		va_start(args,format);
		vfprintf(debugFile,format,args);
		va_end(args);
		fflush(debugFile);
	}

}

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

int GetProcessBaseAddr()
{
	static HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE,::GetCurrentProcessId());
	if (dwHandle==NULL){
		CloseHandle(dwHandle);
		AfxMessageBox("Unable to read Tibia memory!");
	}


	int ret=0;
	int isNotFromNormalScan = 0;
	if (dwHandle){
		unsigned long moduleCount=0;
		EnumProcessModules(dwHandle,NULL,0,&moduleCount);
		moduleCount = moduleCount/sizeof(HMODULE);

		HMODULE *modules=(HMODULE*)calloc(moduleCount,sizeof(HMODULE));
		char moduleName[64];
		EnumProcessModules(dwHandle,modules,moduleCount*sizeof(HMODULE),NULL);
		for (int i=0;i<moduleCount;i++){
			GetModuleBaseName(dwHandle,modules[i],moduleName,sizeof(moduleName));
			if(strcmpi(moduleName,"Tibia.exe")==0){
				MODULEINFO moduleInfo;
				GetModuleInformation(dwHandle, modules[i], &moduleInfo, sizeof(moduleInfo));
				isNotFromNormalScan=0; // commented to see if Tibia.exe in sometimes not first
				ret = (int)moduleInfo.lpBaseOfDll;
				break;
			}
			if(i==0){ // catches first module in case Tibia.exe does not exist
				MODULEINFO moduleInfo;
				GetModuleInformation(dwHandle, modules[i], &moduleInfo, sizeof(moduleInfo));
				isNotFromNormalScan=1;
				ret = (int)moduleInfo.lpBaseOfDll;
			}
		}
		free(modules); modules = NULL;
	}
	if(isNotFromNormalScan) AfxMessageBox("While finding base address, main module was no first or was not named \"Tibia.exe\".");
	return ret;
}

int baseAdjust(int addr){
	CMemReaderProxy reader;
	return addr-0x400000+GetProcessBaseAddr();
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

void sendTAMessage(char* message){
	if(privChanBufferPtr){
		WriteOutDebug("sendTAMessage %X %s",privChanBufferPtr,message);
		OUTmyInterceptInfoMessageBox(privChanBufferPtr,0,(int)message,4,(int)"Tibia Auto",0,0,0,0);
	}
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
			WriteOutDebug("!!! stack overflow protection run\r\n");
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
			WriteOutDebug("!!! underrun\r\n");
		}
		return;
	}
	
	
	int code=buf[2];
	if (code<0)
		code+=256;
	
	
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("### got bytes = %d; packet size = %d; code=0x%x\r\n",realRecvLen,packetSize,code);
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
			//WriteOutDebug("# decrypted content follows #\r\n");
			
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
				//WriteOutDebug("$$$ len=%d code = 0x%x\r\n",afterDecryptLen,afterDecryptCode);
				bufToHexString(decryptedBuf,afterDecryptLen);
				WriteOutDebug("<- [%x] %s\r\n",socket,bufToHexStringRet);
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
	if (!encryptKeyPtr){
		return;
	}
	int i;
	char outbufHeader[1006];
	char* outbuf = outbufHeader+6;
	int len= (((int)buffer[0])&0xFF) + (((int)buffer[1])&0xFF<<8) + 2;

	int outbuflen=len;
	if (len%8!=0) outbuflen+=8-(len%8); // packet length is now 8-btye encryptions + 4 CRC bytes + 2 byes packet header
	outbufHeader[0]=outbuflen%256;
	outbufHeader[1]=outbuflen/256;

	//before encryption
	if (debugFile && COMPLEX) {
		bufToHexString(buffer,len);
		WriteOutDebug("Before Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		WriteOutDebug("-> [%x] %s\r\n",socket,bufToHexStringRet);
		WriteOutDebug("outbuflen = %d\r\n", outbuflen);
	}

	for (i=0;i<outbuflen;i+=8)
	{
		memcpy(outbuf+i,buffer+i,8);
		myInterceptEncrypt((int)(outbuf+i),encryptKeyPtr);// wiz: was repeated 3 times. 2 Removed!
	}
	//after encryption
	if (debugFile && COMPLEX){
		WriteOutDebug("After Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		bufToHexString(outbuf, outbuflen);
		WriteOutDebug("-> [%x] %s\r\n",socket,bufToHexStringRet);
		WriteOutDebug("outbuflen = %d\r\n", outbuflen);
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
	if (nowAction-lastAction<minDistance && nowAction-lastAction>0){
		Sleep(minDistance-(nowAction-lastAction));
	}
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("sending; waited %dms delta=%dms [%d]\r\n",minDistance-(nowAction-lastAction),nowAction-lastAction,time(NULL));
		WriteOutDebug("outbuflen = %d\r\n", outbuflen);
	}
	lastAction=GetTickCount();
	
	
	int ret=Mine_send(tibiaSocket, outbufHeader,test+2,0); //wiz:changed from "send"
	
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("sent %d bytes, ret=%d, lastSendFlags=%d\r\n",outbuflen+2,ret,lastSendFlags);
		WriteOutDebug("outbuflen = %d\r\n", outbuflen);
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
		int openContNr=0;
		int openContMax=reader.readOpenContainerCount();
		for (contNr=0;contNr<memConstData.m_memMaxContainers && openContNr<openContMax;contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);
			
			if (cont->flagOnOff)
			{
				openContNr++;
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
					delete runeItem;
					delete cont;
					delete ch;
					return;
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
	int totalCount;
	int countLeft;
	int len;
	char actionData[MAX_PAYLOAD_LEN-sizeof(int)*4];
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
	int messageCounts = splits+1;

	p->handle = handle;
	while (splits>=0) {
		p->totalCount = messageCounts;
		p->countLeft = splits;
		p->len = min(len,maxlen);
		memcpy(p->actionData,data,p->len);
		char buf[1111];
		sprintf(buf,"%d, %s", p->len,p->actionData);
		//AfxMessageBox(buf);
		ipcPipeBack.send(mess);
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
		ipcPipeBack.send(mess);
		mess.messageType=1008;
		ipcPipeBack.send(mess);
		mess.messageType=1009;
		ipcPipeBack.send(mess);
	}
	
	
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("$$$ ta command - '%s'\r\n",sayBuf);
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
	//fwrite(buf,len);
	//Protocol::parsePacketOut(NetworkMessage(buf));
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
			WriteOutDebug("### %x, %x\r\n",objectId,playerId);
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
			ipcPipeBack.send(mess);
		}
		if (value==0x13){
			mess.messageType=2002;
			ipcPipeBack.send(mess);
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
		WriteOutDebug("!!! send !!! [identical=%d]\r\n",identical);
	}
	
	if (debugFile&&!SENTONLY)
	{
		bufToHexString(buf,len);
		//WriteOutDebug("E> [%x] %s\r\n",socket,bufToHexStringRet);
	}
	if (identical)
	{
		// pass for further processing only when we know content before encryption
		
		parseMessage(encryptBeforeBuf,encryptLen,debugFile,1,1);
		if (debugFile)
		{
			bufToHexString(encryptBeforeBuf,encryptLen);
			WriteOutDebug("-> [%x] %s\r\n",socket,bufToHexStringRet);
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
		WriteOutDebug("!!! recv !!!\r\n");
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
			WriteOutDebug("realRecvLen=%d\r\n",realRecvLen);
			}
			
			bufToHexString(buf,realRecvLen);
			WriteOutDebug("<- [%x] %s\r\n",socket,bufToHexStringRet);
			
			parseMessage(buf,realRecvLen,debugFile,0,1);
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
		WriteOutDebug("got socket: %d, %d, %d -> %d\r\n",af,type,protocol,s);
	}
    
	tibiaSocket=s;
	
    return s;
}

int WINAPI Mine_connect(SOCKET s,const struct sockaddr* name,int namelen)
{
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("connect [pre]: %d, %x, %d\r\n",s,name,namelen);
	}
	
	
	int ret = Real_connect(s,name,namelen);
	
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("connect: %d, %x, %d\r\n",s,name,namelen);
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
		WriteOutDebug("select: %x, %x, %x\r\n",readfds,writefds,exceptfds);
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
			WriteOutDebug("[debug] straight IPC initialised ok\r\n");
		}
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

	WriteOutDebug("Start\r\n");
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


//(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<ecx>, int v9)
int OUTmyPrintText(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9)
{
	int ret;
	typedef void (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9);

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaPrintText);
	

	__asm{
		push [ebp + 0x28]
		mov ecx, [ebp + 0x24]
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
	//EXCEPT when display creature names is off
	if (taMessageStart!=taMessageEnd)
	{
		if (debugFile&&COMPLEX)
		{
			WriteOutDebug("privChanBufferPtr=%x\r\n",privChanBufferPtr);
		}
		sendTAMessage(taMessage[taMessageEnd]);
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
__declspec(naked) void INmyPrintText() //(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<ecx>, int v9)
{__asm{
	push ebp
	mov ebp, esp
	push [ebp + 0x24]
	push ecx
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
int OUTmyPlayerNameText(int align, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char *str, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17)
{
	int ret;
	typedef int (*Proto_fun)(int align, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char *str, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17);

	Proto_fun fun=(Proto_fun)baseAdjust(funAddr_tibiaPlayerNameText);
	
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

int myPlayerNameText(int align, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char* str , int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17)
{
	int ret;
	int titleOffset=0;
	/*
	char buf[1111];
	sprintf(buf,"myPlayerNameText(%x,%s,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)",align, str, visible,x,y,fontNumber,colR,colG,colB,showFormatting, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
	static int aa = 0;
	int barn=0;
	if ((colR!=0xa0 || colG!=0xa0 || colB!=0xff) && aa%1 == 0){
		if (colR==0xf0 && colG==0xf0 && colB==0 && fontNumber==4){
			if (aa>10000000) aa=0;
			sendTAMessage(buf);
			barn=1;
		}
	}
	aa++;
	if (debugFile) {
		WriteOutDebug("myPlayerNameText(%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)\r\n",align,str,visible,x,y,fontNumber,colR,colG,colB,showFormatting, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
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
			OUTmyPlayerNameText(align,fontNumber,visible,x,y-titleOffset,10,colR,colG,colB,info2, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
			titleOffset+=14;
		}
		if (strlen(info1))
		{
			OUTmyPlayerNameText(align,fontNumber,visible,x,y-titleOffset,showFormatting,colR,colG,colB,info1, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
			titleOffset+=14;
		}
		ret = OUTmyPlayerNameText(align,fontNumber,visible,x,y-titleOffset,showFormatting,colR,colG,colB, convString, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
	} else {
		ret = OUTmyPlayerNameText(align,fontNumber, visible,x,y-titleOffset,showFormatting,colR,colG,colB,str, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
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


void OUTmyInterceptInfoMiddleScreen(int type, char* s)
{
	typedef void (*Proto_fun)(int type,char *s);

	Proto_fun fun=(Proto_fun)baseAdjust(funAddr_tibiaInfoMiddleScreen);

	__asm{
		mov edx, [ebp + 0x0C]
		mov ecx, [ebp + 0x08]
		call fun
	}
}
void myInterceptInfoMiddleScreen(int type,char *s)
{
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("got middle screen %d/%s\r\n",type,s);
	}
	
	char buf[1111];
	if (s) sprintf(buf,"XXY v1Type=%x, v2Text=%s",type,s);
	else sprintf(buf,"XXY v1Type=%x, v2Text=",type);

	//sendTAMessage(buf);
	if (type==0x13)
	{
		if (debugFile&&COMPLEX)
		{
			WriteOutDebug("got see '%s'\r\n",s);
		}
		unsigned long bytesWritten=0;
		
		struct ipcMessage mess;
		mess.messageType=1002;
		int len=strlen(s);
		if (len<900)
		{
			memcpy(mess.payload,&len,sizeof(int));
			memcpy(mess.payload+4,s,len);
			ipcPipeBack.send(mess);
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
		OUTmyInterceptInfoMiddleScreen(type,s);
	} else {
		if (debugFile&&COMPLEX)
		{
			WriteOutDebug("ignoring look");
		}
	}
}
// void(int type<ecx>, char* text<edx>)
__declspec(naked) void INmyInterceptInfoMiddleScreen() //()
{__asm{
	push ebp
	mov ebp, esp
	push edx
	push ecx
	call myInterceptInfoMiddleScreen
	leave
	ret
}}


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

		Proto_fun fun=(Proto_fun)baseAdjust(funAddr_tibiaIsCreatureVisible);


		return fun(creaturePtr);
	}
	
}

int OUTmyInterceptEncrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	int ret;
	typedef void (*Proto_fun)(int v1, int v2);

	Proto_fun fun=(Proto_fun)baseAdjust(funAddr_tibiaEncrypt);

	__asm{
		mov edx, [ebp + 0x0C] // move v2 to edx
		mov ecx, [ebp + 0x08] // move v1 to ecx
		call fun
		mov [ebp - 4],eax
	}
	return ret;
}
int myInterceptEncrypt(int v1, int v2)
{
	encryptKeyPtr=v2;
	if (debugFile&&COMPLEX)
	{
		WriteOutDebug("QQQQQQQQQQ: %x,%x,%x\r\n",encryptKeyPtr,v1,v2);
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
__declspec(naked) void INmyInterceptEncrypt() //<eax>(int v1<ecx>, int v2<edx>)
{__asm{
	push ebp
	mov ebp, esp
	push edx // push v2 to stack
	push ecx // push v1 to stack
	call myInterceptEncrypt
	leave
	ret
}}


int OUTmyInterceptDecrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	int ret; //create [ebp - 4] variable
	typedef void (*Proto_fun)();

	Proto_fun fun=(Proto_fun)baseAdjust(funAddr_tibiaDecrypt);

	__asm{
		mov edx, [ebp + 0x0C] // move v2 to edx
		mov ecx, [ebp + 0x08] // move v1 to ecx
		call fun // jump to the real Tibia function
		mov [ebp - 4],eax //store value returned from real function in ret
	}
	return ret;
}
int myInterceptDecrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	if (taMessageStart!=taMessageEnd)
	{
		if (debugFile&&COMPLEX)
		{
			WriteOutDebug("privChanBufferPtr=%x\r\n",privChanBufferPtr);
		}
		sendTAMessage(taMessage[taMessageEnd]);
		taMessageEnd++;
		if (taMessageEnd==TA_MESSAGE_QLEN)
			taMessageEnd=0;
		
	}

	encryptKeyPtr=v2;
	return OUTmyInterceptDecrypt(v1,v2);
}
__declspec(naked) void INmyInterceptDecrypt() //<eax>(int v1<ecx>, int v2<edx>)
{__asm{
	push ebp //store extended base pointer for prev func
	mov ebp, esp //use current stack pointer as base pointer
	push edx // push v2 to stack
	push ecx // push v1 to stack
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
int prevRecvStreamLen = 0;
int prevNextRet = 0;
char prevRecvStream[32768];
int myShouldParseRecv(){
	CMemReaderProxy reader;
	CRegexpProxy regexpProxy;

	typedef int (*Proto_fun)();

	Proto_fun fun=(Proto_fun)baseAdjust(funAddr_tibiaShouldParseRecv);
	//This function looks ahead, it returns the next packet type. It will return -1 if no next packet.
	int ret = fun();

	//look for this address near above location, it will be off by 8

	packStream* recvStream =	(packStream*)baseAdjust(arrayPtr_recvStream);
	//int packLen = ((unsigned char)recvStream->s[6]) + ((unsigned char)recvStream->s[7])*256;

	if (prevNextRet != -1){
		int actionStart = prevRecvStreamPos - 1; //Tibia eats 1 byte to return "ret" every call it is not -1
		int actionEnd; // index of last byte
		if (recvStream->pos <= prevRecvStreamPos || recvStream->pos > prevRecvStreamLen){ // when stream restarts, use previous length instead of current pos
			actionEnd = prevRecvStreamLen-1;
		} else if (ret == -1){
			actionEnd = recvStream->pos-1; // did not eat an extra byte
		} else {
			actionEnd = recvStream->pos-2; // ate an extra byte
		}
		int actionLen = actionEnd - actionStart + 1;
		//Parse packet and perform any needed actions
		Protocol::parsePacketIn(NetworkMessage((char*)(prevRecvStream+actionStart),actionLen));
		/*
		int packtype = ((char*)prevRecvStream+actionStart)[0]&0xff;
		int wanted = 0xb4;
		if(packtype == wanted){
			Protocol::parsePacketIn(NetworkMessage((char*)(prevRecvStream+actionStart),actionLen),ipcPipeBack);
			bufToHexString(((char*)prevRecvStream+actionStart),actionLen);
			sendTAMessage(bufToHexStringRet);
		}*/
		{for (int i=0;i<recvRegexCount;i++){
			if (recvRegex[i].inUse == 1){
				int match = regexpProxy.regnexec(&(recvRegex[i].preg),((char*)prevRecvStream+actionStart),actionLen,0,NULL,0);
				if (match == 0){
					if (0){
						bufToHexString(((char*)prevRecvStream+actionStart),actionLen);
						sendTAMessage(bufToHexStringRet);
					}
					parseRecvActionData(recvRegex[i].handle,(char*)prevRecvStream+actionStart,actionLen);
				}
			}
		}}
	}
	if (ret != -1 && (recvStream->pos <= prevRecvStreamPos || recvStream->pos > prevRecvStreamLen)){
		prevRecvStreamLen = recvStream->length;
		memcpy(prevRecvStream, recvStream->s, prevRecvStreamLen);
	}
	prevRecvStreamPos = recvStream->pos;
	prevNextRet = ret;

	//char buf[1111];
	//sprintf(bufToHexStringRet,"%x:%d, %d  %x%x",ret, recvStream->pos, recvStream->length,((char)recvStream->s[recvStream->pos-2]),((char)recvStream->s[recvStream->pos-2-1]));
	//AfxMessageBox(buf);
	//bufToHexString(recvStream->s,30);
	//sendTAMessage(bufToHexStringRet);
	if (0&&reader.isLoggedIn()){ // see what happens when packets are ignored
		if (ret == 107){
			ret = -1;
		}
		if (ret == -1) {
			recvStream->pos += 10;
		}
	}
	return ret;
}

//<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
int OUTmyInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9)
{
	int ret;
	typedef int (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);

	Proto_fun fun=(Proto_fun)baseAdjust(funAddr_tibiaInfoMessageBox);

	__asm{
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
		add esp, 0x1C
		mov [ebp - 4],eax
	}
	return ret;

}
//<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
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
	//16-
	//17-rule report messages
	//18-white client messages, login, permanent ignore,
	//19-white game info messages, attack, experience, depot volume
	//20-all green message
	//21-
	//22-to myself in dark blue
	//23-hitpoint loss message
	//AfxMessageBox(buf);
	//OUTmyInterceptInfoMessageBox(v1, v2, (int)buf, 4, (int)"Tibia Auto", v6, v7, 0, v9);
	//if (v9!=0 && privChanBufferPtr) OUTmyInterceptInfoMessageBox(privChanBufferPtr,0,(int)buf,4,(int)"Tibia Auto", v6, v7, 0, v9);

	char *nick=(char *)v5;
	char *s=(char *)v3;

	int type=v4;
	int chantype=v9;
	if (debugFile)
	{
		WriteOutDebug("XXX s1=%s s2=%s\r\n",s,nick);
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
		memcpy(mess.payload+4,&chantype,sizeof(int));
		memcpy(mess.payload+8,&nickLen,sizeof(int));
		memcpy(mess.payload+12,&msgLen,sizeof(int));
		if (nickLen) memcpy(mess.payload+16,nick,nickLen);
		memcpy(mess.payload+16+nickLen,s,msgLen);
		mess.messageType=1001;
		ipcPipeBack.send(mess);
		mess.messageType=1003;
		ipcPipeBack.send(mess);
		mess.messageType=1004;
		ipcPipeBack.send(mess);
		//mess.messageType=1005;
		//ipcPipeBack.send(mess);
		mess.messageType=1006;
		ipcPipeBack.send(mess);
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
__declspec(naked) void INmyInterceptInfoMessageBox() //<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
{__asm{
	push ebp
	mov ebp, esp
	push [ebp + 0x20]
	push [ebp + 0x1C]//lvl
	push [ebp + 0x18]//type2
	push [ebp + 0x14]//unk0x76d43ef5,5f6ec11c,754f5e89
	push [ebp + 0x10]//name
	push [ebp + 0x0C]//type
	push [ebp + 8]//text
	push edx//6988,7e8e,0
	push ecx//consoleID
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

	// lookup: find string In(FontNumber,1 [6th match is in the middle of the function]

	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText01+1),(unsigned int)INmyPlayerNameText);
	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText02+1),(unsigned int)INmyPlayerNameText);
	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText03+1),(unsigned int)INmyPlayerNameText);
	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText04+1),(unsigned int)INmyPlayerNameText);
	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText05+1),(unsigned int)INmyPlayerNameText);
	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText06+1),(unsigned int)INmyPlayerNameText);
	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText07+1),(unsigned int)INmyPlayerNameText);
	trapFun(dwHandle,baseAdjust(callAddr_PlayerNameText08+1),(unsigned int)INmyPlayerNameText);

	// lookup: TALK_INFO_MESSAGE; this is inside of the function

	trapFun(dwHandle,baseAdjust(callAddr_InfoMiddleScreen01+1),(unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMiddleScreen02+1),(unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMiddleScreen03+1),(unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMiddleScreen04+1),(unsigned int)INmyInterceptInfoMiddleScreen);

	// lookup: TargetBuffer!=NULL; first call is in the middle of the infomessage function
	//         in fact you need to increcept all calls
	
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox01+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox02+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox03+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox04+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox05+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox06+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox07+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox08+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox09+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox10+1),(unsigned int)INmyInterceptInfoMessageBox);//BADBAD
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox11+1),(unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox12+1),(unsigned int)INmyInterceptInfoMessageBox);//
	trapFun(dwHandle,baseAdjust(callAddr_InfoMessageBox13+1),(unsigned int)INmyInterceptInfoMessageBox);//
	// lookup: manually match around previous address by assembly similarity
	trapFun(dwHandle,baseAdjust(callAddr_Encrypt01+1),(unsigned int)INmyInterceptEncrypt);

	// lookup: function below encrypt is the decrypt function
	trapFun(dwHandle,baseAdjust(callAddr_Decrypt01+1),(unsigned int)INmyInterceptDecrypt);

	
	// WARNING: decrypt function is not trapped since 8.22 as I did not track it down in the soureccode
	
	// lookup: reference to  string "Creature!=NULL"
	//         [you need to look for PUSH string with this debug] +
	//         instruction before PUSH XXX must be MOV ESI, 00000000Fh
	//         it's in the middle of the function
	//         trap of the second (last one) reference to the function
	//trapFun(dwHandle,0x4F32BA+1,(unsigned int)myIsCreatureVisible); // 9.20

	// lookup: reference to  string "unknown packet type during login"
	//         it's about 65 instuctions before it and 300 bytes
	//         look for CMP ESI,-1 since the function returns -1 when there are no packets to parse
	trapFun(dwHandle,baseAdjust(callAddr_ShouldParseRecv01+1),(unsigned int)myShouldParseRecv);

	//This allows TA ta handle any errors before they are sent to Tibia
	//trapFun(dwHandle,baseAdjust(0x460EB3+1),(unsigned int)myShouldParseRecv); // 9.43 // need to find stuff for this

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
			Mine_send(tibiaSocket,buf,3,0);
			
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
			WriteOutDebug("[debug] will try to connect back IPC pipe\r\n");
		}
		if (!ipcPipeBack.hasInvalidHandle())
		{
			// close back pipe
			ipcPipeBack.closePipe();
		}
		memcpy(&partnerProcessId,mess.payload,sizeof(int));
		ipcPipeBack.InitialiseIPCback(partnerProcessId,debugFile,COMPLEX);
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
					found=1;
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
	case 307:
		{
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
	case 308: // adds given regex to list
		{
			switch(*(int*)mess.payload){
			case 1:
				{
					CRegexpProxy regexpProxy;
					int handle, regLen;
					memcpy(&handle, mess.payload + 4, sizeof(int));
					// Since we are using the array at the same time we are creating it, we do not move around items in use to fill empty spaces
					// this is run only once when the python script is loaded so this inefficioncy is acceptable.
					// Handles are the same when TA restarts so disable duplicate handles
					int i;
					for (i=0;i<RECV_REGEX_MAX && recvRegex[i].inUse != 0;i++){ //find first empty space
						if (recvRegex[i].handle == handle){ // disable duplicate handles as TA has restarted
							recvRegex[i].inUse = 0;
						}
					}
					if (i<RECV_REGEX_MAX) {
						memcpy(&regLen, mess.payload + 8, sizeof(int));
						char* regExp = (char*)malloc(regLen);
						memcpy(regExp, mess.payload + 12, regLen);

						recvRegex[i].handle = handle;
						if(regexpProxy.regncomp(&(recvRegex[i].preg),regExp,regLen,REG_NOSUB|REG_EXTENDED)){
							//Failed to compile expression, send one message back to user
							char base[] = "Error: Failed to compile regular expression ";
							char* errorMsg = (char*)malloc(strlen(base)+regLen+1);
							strcpy(errorMsg,base);
							strncpy(errorMsg+strlen(base),regExp,regLen);
							errorMsg[strlen(base)+regLen]=0;
							sendTAMessage(errorMsg);
							break;
						}
						free(regExp);

						recvRegex[i].inUse = 1; //activates regex once everything is in order
						if (i >= recvRegexCount) recvRegexCount++;
					}
					break;
				}
			case 2: //sets use of given regex to 0
				{
					// it is a (almost absolute) guarantee that the time between the regex being set to 0 and
					// being set to 1 again will be long enough for the reader to have finished with the entry
					int handle;
					memcpy(&handle, mess.payload + 4, sizeof(int));
					for (int i=0;i<recvRegexCount && recvRegex[i].inUse != 0;i++){
						if(recvRegex[i].handle == handle){
							recvRegex[i].inUse = 0;
						}
					}
				}
				break;
			default:
				break;
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

