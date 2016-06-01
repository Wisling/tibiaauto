// tibiaautoinject2.cpp : Defines the entry point for the DLL application.
//
// Check MyPlayerNameText funcs

#include "stdafx.h"

#include "stdio.h"
#include "detours.h"
#include "winsock2.h"
#include "time.h"
#include "protocol.h"
#include "Tlhelp32.h"
#include "MyMenu.h"
#include "resource.h"
#include "Windows.h"
#include "regex.h"
#include "psapi.h"
#include <ModuleUtil.h>
#include <MemReader.h>
#include <MemUtil.h>
#include "IPCPipeBack.h"

CIPCPipeBack ipcPipeBack;

int myInterceptEncrypt(int v1, int v2);
int myInterceptDecrypt(int v1, int v2);
__declspec(noinline) int OUTmyInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
int myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
int myShouldParseRecv();


#define TA_MESSAGE_QLEN 10
#define MAX_CREATUREINFO 1300
#define MAX_PAYLOAD_LEN 1024
#define MAX_PRINTEXT_LEN 1024

#ifndef NDEBUG
const bool OUTPUT_DEBUG_INFO = 1;
#else // ifndef NDEBUG
const bool OUTPUT_DEBUG_INFO = 0;
#endif // ifndef NDEBUG

int tmp1 = -1;
char tmp2[256];
char tmp3[256];
int revealFish = 0;

char creatureInfoPlayerName[MAX_CREATUREINFO][32];
char creatureInfoPlayerInfo1[MAX_CREATUREINFO][500];
char creatureInfoPlayerInfo2[MAX_CREATUREINFO][500];
int creatureInfoNext = 0;

char taMessage[TA_MESSAGE_QLEN][32000];
int taMessageStart = 0;
int taMessageEnd   = 0;


struct actionRegexData {
	int handle;
	int inUse;
	regex_t preg;
	actionRegexData()
	{
		inUse  = 0;
		handle = NULL;
	}
};

const int RECV_REGEX_MAX = 500;
actionRegexData recvRegex[RECV_REGEX_MAX];
int recvRegexCount = 0;


int partnerProcessId = 0;
int ignoreLookEnd    = 0;

int autoAimActive               = 0;
int autoAimOnlyCreatures        = 0;
int autoAimAimPlayersFromBattle = 0;

int revealCNameActive = 0;

char lastConnectName[16];

HANDLE hPipe = INVALID_HANDLE_VALUE;

HHOOK hook;

SOCKET tibiaSocket = NULL;
FILE *debugFile    = NULL;

int COMPLEX  = 0;
int SENTONLY = 0;

time_t debugFileStart;
int lastSendFlags;

// the encryption buffer (to avoid decryption on send)
char encryptBeforeBuf[5000];
char encryptAfterBuf[5000];
int encryptLen;
int encryptPos;
int encryptPrevPtr = 0;
int encryptKeyPtr  = 0;

int privChanBufferPtr = 0;

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
	string message;
public:
	HUD()
	{
		CPoint pos;
		redColor   = 0;
		blueColor  = 0;
		greenColor = 0;
		message[0] = '\0';
	}
};
vector<HUD> vecHUD;

bool showManaBar = 0;

struct tibiaState
{
	int attackedCreature;
	unsigned char outbufHmm[100];
	unsigned char outbufGfb[100];
	unsigned char outbufSd[100];
	unsigned char outbufExplo[100];
	unsigned char outbufSelfUH[100];
	unsigned char outbufFluidMana[100];
	unsigned char outbufFluidLife[100];
} tibiaState;

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

int outExploAvail     = 0;
int outHmmAvail       = 0;
int outSdAvail        = 0;
int outGfbAvail       = 0;
int outSelfUHAvail    = 0;
int outFluidManaAvail = 0;
int outFluidLifeAvail = 0;

//10.94
int funAddr_tibiaPrintText =			0x498B60;
int funAddr_tibiaPlayerNameText =		0x52E310;
int funAddr_tibiaInfoMiddleScreen =		0x5F56A0;
int funAddr_tibiaIsCreatureVisible =	0x4AC820;
int funAddr_tibiaEncrypt =				0x60BBD0;
int funAddr_tibiaDecrypt =				0x60BC50;
int funAddr_tibiaShouldParseRecv =		0x599510;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0xB8013C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x5F9610;
int callAddr_DrawRect =                 0x584FE2;
int callAddr_DrawBlackRect =            0x584F53;
int callAddr_PrintText01 =				0x4911BD;//...<addr>.*
int callAddr_PrintText02 =				0x491202;
int callAddr_PrintText03 =				0x49AD39;
int callAddr_PrintText04 =				0x58502F;
int callAddr_PrintText05 =				0x586587;
int callAddr_PlayerNameText01 =			0x450825;
int callAddr_PlayerNameText02 =			0x4626EC;
int callAddr_PlayerNameText03 =			0x468248;
int callAddr_PlayerNameText04 =			0x468438;
int callAddr_PlayerNameText05 =			0x46E9A8;
int callAddr_PlayerNameText06 =			0x47A52C;
int callAddr_PlayerNameText07 =			0x498BEC;
int callAddr_PlayerNameText08 =			0x4D22FA;
int callAddr_PlayerNameText09 =			0x585E6F;
int callAddr_PlayerNameText10 =			0x59036A;
int callAddr_InfoMiddleScreen01 =		0x435048;
int callAddr_InfoMiddleScreen02 =		0x4962C9;
int callAddr_InfoMiddleScreen03 =		0x4D91DD;
int callAddr_InfoMiddleScreen04 =		0x582CEB;
int callAddr_InfoMessageBox01 =			0x4348D4;
int callAddr_InfoMessageBox02 =			0x45DADF;
int callAddr_InfoMessageBox03 =			0x51488A;
int callAddr_InfoMessageBox04 =			0x5799E2;
int callAddr_InfoMessageBox05 =			0x57A1D7;
int callAddr_InfoMessageBox06 =			0x57A27D;
int callAddr_InfoMessageBox07 =			0x5F9495;
int callAddr_InfoMessageBox08 =			0x5F9E5C;
int callAddr_InfoMessageBox09 =			0x5F9F4B;
int callAddr_InfoMessageBox10 =			0x5FA132;
int callAddr_InfoMessageBox11 =			0x5FA3F9;
int callAddr_InfoMessageBox12 =			0x5FAF15;
int callAddr_InfoMessageBox13 =			0x5FAF45;
int callAddr_Encrypt01 =				0x598C43;
int callAddr_Decrypt01 =				0x599239;
int callAddr_ShouldParseRecv01 =		0x49DE22;

//10.93
/*
int funAddr_tibiaPrintText =			0x496560;
int funAddr_tibiaPlayerNameText =		0x52B8A0;
int funAddr_tibiaInfoMiddleScreen =		0x5F2C30;
int funAddr_tibiaIsCreatureVisible =	0x4A9F30;
int funAddr_tibiaEncrypt =				0x609190;
int funAddr_tibiaDecrypt =				0x609210;
int funAddr_tibiaShouldParseRecv =		0x596A60;//switch table contains "Are you sure you want to leave Tibia"
int arrayPtr_recvStream =				0xB7A45C-8;//look for this address near above location
int funAddr_tibiaInfoMessageBox =		0x5F6BD0;
int callAddr_DrawRect =                 0x582542;
int callAddr_DrawBlackRect =            0x5824B3;
int callAddr_PrintText01 =				0x48EB9D;//...<addr>.*
int callAddr_PrintText02 =				0x48EBE2;
int callAddr_PrintText03 =				0x498739;
int callAddr_PrintText04 =				0x58258F;
int callAddr_PrintText05 =				0x583AE7;
int callAddr_PlayerNameText01 =			0x44FBB5;
int callAddr_PlayerNameText02 =			0x461A8C;
int callAddr_PlayerNameText03 =			0x4675F8;
int callAddr_PlayerNameText04 =			0x4677E8;
int callAddr_PlayerNameText05 =			0x46DD58;
int callAddr_PlayerNameText06 =			0x47992C;
int callAddr_PlayerNameText07 =			0x4965EC;
int callAddr_PlayerNameText08 =			0x4CF95A;
int callAddr_PlayerNameText09 =			0x5833CF;
int callAddr_PlayerNameText10 =			0x58D8BA;
int callAddr_InfoMiddleScreen01 =		0x434EE8;
int callAddr_InfoMiddleScreen02 =		0x493CB9;
int callAddr_InfoMiddleScreen03 =		0x4D683D;
int callAddr_InfoMiddleScreen04 =		0x58024B;
int callAddr_InfoMessageBox01 =			0x434774;
int callAddr_InfoMessageBox02 =			0x45CE7F;
int callAddr_InfoMessageBox03 =			0x511E1A;
int callAddr_InfoMessageBox04 =			0x576F12;
int callAddr_InfoMessageBox05 =			0x577707;
int callAddr_InfoMessageBox06 =			0x5777AD;
int callAddr_InfoMessageBox07 =			0x5F6A55;
int callAddr_InfoMessageBox08 =			0x5F741C;
int callAddr_InfoMessageBox09 =			0x5F750B;
int callAddr_InfoMessageBox10 =			0x5F76F2;
int callAddr_InfoMessageBox11 =			0x5F79B9;
int callAddr_InfoMessageBox12 =			0x5F84C5;
int callAddr_InfoMessageBox13 =			0x5F84F5;
int callAddr_Encrypt01 =				0x596193;
int callAddr_Decrypt01 =				0x596789;
int callAddr_ShouldParseRecv01 =		0x49B812;
*/

int(WINAPI * Real_send)(SOCKET s, char* buf, int len, int flags)               = NULL;
int(WINAPI * Real_recv)(SOCKET s, char* buf, int len, int flags)               = NULL;
int(WINAPI * Real_connect)(SOCKET s, const struct sockaddr* name, int namelen) = NULL;
SOCKET(WINAPI * Real_socket)(int af, int type, int protocol)                   = NULL;

int WINAPI Mine_send(SOCKET s, char* buf, int len, int flags);
void adler(unsigned char *data, size_t len, unsigned char* outBuf);

void InitialiseCommunication();
void InitialiseHooks();
void InitialiseDebugFile();

void WriteOutDebug(const char * format, ...)
{
	if (debugFile)
	{
		va_list args;
		va_start(args, format);
		vfprintf(debugFile, format, args);
		va_end(args);
		fflush(debugFile);
	}
}

#define STRBUFLEN 655360
char bufToHexStringRet[STRBUFLEN];

int payloadLen(unsigned char *buf)
{
	int lenL = buf[0];
	int lenH = buf[1];
	int len  = buf[0] + buf[1] * 256;
	return len;
}

int payloadLen(char buf[])
{
	int lenL = buf[0];
	int lenH = buf[1];
	int len  = buf[0] + buf[1] * 256;
	return len;
}

void bufToHexString(unsigned char *buf, int len)
{
	if (len > STRBUFLEN / 8)
	{
		sprintf(bufToHexStringRet, "[buffer length to big: %d]", len);
		return;
	}
	memset(bufToHexStringRet, 0x00, STRBUFLEN);
	for (int pos = 0; pos < len; pos++)
	{
		char localBuf[256];
		int val = buf[pos];
		if (val < 0)
			val += 256;
		sprintf(localBuf, " %02X", val);
		strcat(bufToHexStringRet, localBuf);
	};
}

void bufToHexString(char *buf, int len)
{
	bufToHexString((unsigned char*)buf, len);
}

int GetProcessBaseAddr()
{
	static HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());
	if (dwHandle == NULL)
	{
		CloseHandle(dwHandle);
		AfxMessageBox("Unable to read Tibia memory!");
	}


	int ret                 = 0;
	int isNotFromNormalScan = 0;
	if (dwHandle)
	{
		unsigned long moduleCount = 0;
		EnumProcessModules(dwHandle, NULL, 0, &moduleCount);
		moduleCount = moduleCount / sizeof(HMODULE);

		HMODULE *modules = (HMODULE*)calloc(moduleCount, sizeof(HMODULE));
		char moduleName[64];
		unsigned long dummy;
		EnumProcessModules(dwHandle, modules, moduleCount * sizeof(HMODULE), &dummy);
		for (size_t i = 0; i < moduleCount; i++)
		{
			GetModuleBaseName(dwHandle, modules[i], moduleName, sizeof(moduleName));
			if (_strcmpi(moduleName, "Tibia.exe") == 0)
			{
				MODULEINFO moduleInfo;
				GetModuleInformation(dwHandle, modules[i], &moduleInfo, sizeof(moduleInfo));
				isNotFromNormalScan = 0; // commented to see if Tibia.exe in sometimes not first
				ret                 = (int)moduleInfo.lpBaseOfDll;
				break;
			}
			if (i == 0) // catches first module in case Tibia.exe does not exist
			{
				MODULEINFO moduleInfo;
				GetModuleInformation(dwHandle, modules[i], &moduleInfo, sizeof(moduleInfo));
				isNotFromNormalScan = 1;
				ret                 = (int)moduleInfo.lpBaseOfDll;
			}
		}
		free(modules);
		modules = NULL;
	}
	if (isNotFromNormalScan)
		AfxMessageBox("While finding base address, main module was no first or was not named \"Tibia.exe\".");
	return ret;
}

int baseAdjust(int addr)
{
	CMemReader& reader = CMemReader::getMemReader();
	return addr - 0x400000 + GetProcessBaseAddr();
}

#define MOD_ADLER 65521
void adler(unsigned char *data, size_t len, unsigned char *outBuf)
{
	/*
	   The is the CRC algorithim. I could not nor would I try to find the actual one Tibia
	   uses. So I have copied this of and modified it to fit the data we use.
	 */
	int count = 0;
	int a     = 1;
	int b     = 0;

	while (len > 0)
	{
		size_t tlen = len > 5552 ? 5552 : len;
		len -= tlen;
		do
		{
			a += (int)data[count] >= 0 ? (int)data[count] : 256 + (int)data[count];
			b += a;
			count++;
		}
		while (--tlen);

		a %= MOD_ADLER;
		b %= MOD_ADLER;
	}
	outBuf[0] = a & 0xff;
	outBuf[1] = (a >> 8) & 0xff;
	outBuf[2] = b & 0xff;
	outBuf[3] = (b >> 8) & 0xff;
}

void sendTAMessage(char* message)
{
	if (privChanBufferPtr)
	{
		WriteOutDebug("sendTAMessage %X %s", privChanBufferPtr, message);
		OUTmyInterceptInfoMessageBox(privChanBufferPtr, 0, (int)message, 4, (int)"Tibia Auto", 0, 0, 0, 0);
	}
}

int aaa = 0;
// direction 0 - incoming (recv); 1 outgoing (send)
void parseMessage(char *buf, int realRecvLen, FILE *debugFile, int direction, int depth)
{
	// 45D565, 4F9520 hook for calculating everything
	// stack overflow protecion
	if (depth > 20)
	{
		if (debugFile && COMPLEX)
			WriteOutDebug("!!! stack overflow protection run\r\n");
		return;
	}

	int packetSizeH = buf[1];
	int packetSizeL = buf[0];
	if (packetSizeH < 0)
		packetSizeH += 256;
	if (packetSizeL < 0)
		packetSizeL += 256;
	int packetSize = packetSizeH * 256 + packetSizeL + 2;


	if (packetSize > realRecvLen)
	{
		if (debugFile && COMPLEX)
			WriteOutDebug("!!! underrun\r\n");
		return;
	}


	int code = buf[2];
	if (code < 0)
		code += 256;


	if (debugFile && COMPLEX)
		WriteOutDebug("### got bytes = %d; packet size = %d; code=0x%x\r\n", realRecvLen, packetSize, code);

	if (direction == 0)
	{
		// this is 'recv' direction: decrypt and parse message
		if (encryptKeyPtr)
		{
			int afterDecryptCode;
			static char decryptedBuf[100000];
			int i;

			memcpy(decryptedBuf, buf + 6, packetSize - 6); // Remember CRC bytes are NOT encrypted either sending or recieving.
			//WriteOutDebug("# decrypted content follows #\r\n");

			for (i = 0; i < packetSize - 6; i += 8)
			{
				myInterceptDecrypt((int)(decryptedBuf + i), encryptKeyPtr);
			}
			afterDecryptCode = decryptedBuf[2];
			if (afterDecryptCode < 0)
				afterDecryptCode += 256;
			int afterDecryptLenH = decryptedBuf[1];
			int afterDecryptLenL = decryptedBuf[0];
			if (afterDecryptLenH < 0)
				afterDecryptLenH += 256;
			if (afterDecryptLenL < 0)
				afterDecryptLenL += 256;
			int afterDecryptLen = afterDecryptLenH * 256 + afterDecryptLenL + 2;

			if (debugFile && !SENTONLY)
			{
				//WriteOutDebug("$$$ len=%d code = 0x%x\r\n",afterDecryptLen,afterDecryptCode);
				bufToHexString(decryptedBuf, afterDecryptLen);
				WriteOutDebug("<- [%x] %s\r\n", socket, bufToHexStringRet);
			}
		}
	}

	if (packetSize < realRecvLen)
		parseMessage(buf + packetSize, realRecvLen - packetSize, debugFile, direction, depth + 1);
	// tail recursion
};

int lastAction = 0;
void sendBufferViaSocket(unsigned char *buffer)
{
	// if we don't yet have key pointer then don't do anything
	if (!encryptKeyPtr)
		return;
	int i;
	unsigned char outBuf[1006];
	unsigned char* outBufHeader  = outBuf;
	unsigned char* outBufPayload = outBuf + 6;
	int payloadLen               = *((unsigned short*)buffer) + 2; // length stored on first 2 bytes of payload doesn't include these 2 bytes
	if (payloadLen > 8 * 125) // this is to check to see if outBuf is long enough
		//this should never be reached, but just in case 1000 is not arbitrarily high enough we can catch this error
		MessageBox(NULL, "Trying to send packet > 1000 characters!", "", 0);
	memcpy(outBufPayload, buffer, payloadLen);
	int outBufLen = payloadLen;
	if (payloadLen % 8 != 0)
		outBufLen += 8 - (payloadLen % 8); // packet length is now 8-byte encryptions + 4 CRC bytes + 2 bytes packet header, align payload to encryption size

	//before encryption
	if (debugFile && COMPLEX)
	{
		bufToHexString(buffer, payloadLen);
		WriteOutDebug("Before Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		WriteOutDebug("-> [%x] %s\r\n", socket, bufToHexStringRet);
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}

	for (i = 0; i < outBufLen; i += 8)
	{
		myInterceptEncrypt((int)(outBufPayload + i), encryptKeyPtr);
	}
	//after encryption
	if (debugFile && COMPLEX)
	{
		WriteOutDebug("After Encryption: ~~~~~~~~~~~~~~~~~~~~~~\r\n");
		bufToHexString(outBufPayload, outBufLen);
		WriteOutDebug("-> [%x] %s\r\n", socket, bufToHexStringRet);
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}
	adler(outBufPayload, outBufLen, outBufHeader + 2);
	int lengthWithChecksum = outBufLen + 4;
	*((unsigned short*)outBufHeader) = lengthWithChecksum;

	// make sure that packets go at most once every minDistance ms
	int minDistance = 175;
	minDistance = 1;
	int nowAction = GetTickCount();
	if (nowAction - lastAction < minDistance && nowAction - lastAction > 0)
		Sleep(minDistance - (nowAction - lastAction));
	if (debugFile && COMPLEX)
	{
		WriteOutDebug("sending; waited %dms delta=%dms [%d]\r\n", minDistance - (nowAction - lastAction), nowAction - lastAction, time(NULL));
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}
	lastAction = GetTickCount();

	int ret = Mine_send(tibiaSocket, (char*)outBuf, lengthWithChecksum + 2, 0);

	if (debugFile && COMPLEX)
	{
		WriteOutDebug("sent %d bytes, ret=%d, lastSendFlags=%d\r\n", lengthWithChecksum + 2, ret, lastSendFlags);
		WriteOutDebug("outBufLen = %d\r\n", outBufLen);
	}
}

void castRuneAgainstHuman(int contNr, int itemPos, int runeObjectId, int targetX, int targetY, int targetZ)
{
	unsigned char sendbuf[19];
	//int targetObjectId=CTibiaItem::m_itemTypeRopespot;
	int targetObjectId = 0x63;

	sendbuf[0] = 17;
	sendbuf[1] = 0;
	sendbuf[2] = 0x83;

	sendbuf[3] = 0xff;
	sendbuf[4] = 0xff;
	sendbuf[5] = contNr & 0xff;
	sendbuf[6] = (contNr >> 8) & 0xff;
	sendbuf[7] = itemPos;
	sendbuf[8] = runeObjectId & 0xff;
	;
	sendbuf[9]  = (runeObjectId >> 8) & 0xff;
	sendbuf[10] = itemPos;

	sendbuf[11] = targetX & 0xff;
	sendbuf[12] = (targetX >> 8) & 0xff;
	sendbuf[13] = targetY & 0xff;
	sendbuf[14] = (targetY >> 8) & 0xff;
	sendbuf[15] = targetZ;
	sendbuf[16] = targetObjectId & 0xff;
	sendbuf[17] = (targetObjectId >> 8) & 0xff;
	sendbuf[18] = 0x01;

	//Mine_send(tibiaSocket,sendbuf,19,lastSendFlags);
	sendBufferViaSocket(sendbuf);
}

void castRuneAgainstCreature(int contNr, int itemPos, int runeObjectId, int creatureId)
{
	unsigned char sendbuf[15];
	sendbuf[0] = 13;
	sendbuf[1] = 0;
	sendbuf[2] = 0x84;

	sendbuf[3] = 0xff;
	sendbuf[4] = 0xff;
	sendbuf[5] = contNr & 0xff;
	sendbuf[6] = (contNr >> 8) & 0xff;
	sendbuf[7] = itemPos;
	sendbuf[8] = runeObjectId & 0xff;
	;
	sendbuf[9]  = (runeObjectId >> 8) & 0xff;
	sendbuf[10] = itemPos;

	sendbuf[11] = creatureId & 0xff;
	sendbuf[12] = (creatureId >> 8) & 0xff;
	sendbuf[13] = (creatureId >> 16) & 0xff;
	sendbuf[14] = (creatureId >> 24) & 0xff;

	//Mine_send(tibiaSocket,sendbuf,17,lastSendFlags);
	sendBufferViaSocket(sendbuf);
}

void autoAimAttack(int runeId)
{
	CMemReader& reader = CMemReader::getMemReader();	
	int attackedCreature       = reader.getAttackedCreature();
	CTibiaCharacter ch;

	if (reader.getCharacterByTibiaId(&ch, attackedCreature))
	{
		int chX = ch.x;
		int chY = ch.y;
		int chZ = ch.z;
		int contNr;
		int openContNr  = 0;
		int openContMax = reader.readOpenContainerCount();
		for (contNr = 0; contNr < reader.m_memMaxContainers && openContNr < openContMax; contNr++)
		{
			CTibiaContainer *cont = reader.readContainer(contNr);

			if (cont->flagOnOff)
			{
				openContNr++;
				CUIntArray acceptedItems;
				CTibiaItem *runeItem;

				acceptedItems.Add(runeId);
				runeItem = CModuleUtil::lookupItem(contNr, &acceptedItems);
				if (runeItem->objectId)
				{
					if (autoAimOnlyCreatures)
						castRuneAgainstCreature(0x40 + contNr, runeItem->pos, runeId, ch.tibiaId);
					else
						castRuneAgainstHuman(0x40 + contNr, runeItem->pos, runeId, ch.x, ch.y, ch.z);
					delete runeItem;
					delete cont;
					return;
				}
				delete runeItem;
			};
			delete cont;
		}
	}
}

struct parcelRecvActionData {
	int handle;
	int totalCount;
	int countLeft;
	int len;
	char actionData[MAX_PAYLOAD_LEN - sizeof(int) * 4];
	parcelRecvActionData()
	{
		memset(actionData, 0, sizeof(actionData));
	}
};

void parseRecvActionData(int handle, char* data, int len)
{
#ifdef _DEBUG
	if (len == 0)
		AfxMessageBox("parseRecvActionData: length is 0.");

#endif // ifdef _DEBUG
	CIpcMessage mess;
	mess.messageType = 1010;

	parcelRecvActionData* p = ((parcelRecvActionData*)mess.payload);
	int maxlen              = sizeof(p->actionData);
	int splits              = len / maxlen;
	int messageCounts       = splits + 1;

	p->handle = handle;
	while (splits >= 0)
	{
		p->totalCount = messageCounts;
		p->countLeft  = splits;
		p->len        = min(len, maxlen);
		memcpy(p->actionData, data, p->len);
		char buf[1111];
		sprintf(buf, "%d, %s", p->len, p->actionData);
		//AfxMessageBox(buf);
		ipcPipeBack.send(mess);
		splits--;
		data += p->len;
		len  -= p->len;
	}
#ifdef _DEBUG
	if (len != 0)
		AfxMessageBox("parseRecvActionData: part of message not sent.");

#endif // ifdef _DEBUG
}

void parseMessageSay(char *sayBuf)
{
	
	CMemReader& reader = CMemReader::getMemReader();
	

	CIpcMessage mess;

	int len = strlen(sayBuf);
	if (len < 900)
	{
		mess.messageType = 1007;
		memcpy(mess.payload, &len, sizeof(int));
		memcpy(mess.payload + 4, sayBuf, len);
		ipcPipeBack.send(mess);
		mess.messageType = 1008;
		ipcPipeBack.send(mess);
		mess.messageType = 1009;
		ipcPipeBack.send(mess);
	}


	if (debugFile && COMPLEX)
		WriteOutDebug("$$$ ta command - '%s'\r\n", sayBuf);


	if (!strcmp(sayBuf, "%ta hmm") && autoAimActive)
		if (reader.getAttackedCreature())
			autoAimAttack(CTibiaItem::getValueForConst("runeHMM"));
	if (!strcmp(sayBuf, "%ta gfb") && autoAimActive)
		if (reader.getAttackedCreature())
			autoAimAttack(CTibiaItem::getValueForConst("runeGFB"));
	if (!strcmp(sayBuf, "%ta sd") && autoAimActive)
		if (reader.getAttackedCreature())
			autoAimAttack(CTibiaItem::getValueForConst("runeSD"));
	if (!strcmp(sayBuf, "%ta explo") && autoAimActive)
		if (reader.getAttackedCreature())
			autoAimAttack(CTibiaItem::getValueForConst("runeExplo"));
	if (!strcmp(sayBuf, "%ta selfuh") && outSelfUHAvail)
		//Mine_send(tibiaSocket,tibiaState.outbufSelfUH,payloadLen(tibiaState.outbufSelfUH)+2,lastSendFlags);
		sendBufferViaSocket(tibiaState.outbufSelfUH);
	if (!strcmp(sayBuf, "%ta fluidlife") && outFluidLifeAvail)
		//Mine_send(tibiaSocket,tibiaState.outbufFluidLife,payloadLen(tibiaState.outbufFluidLife)+2,lastSendFlags);
		sendBufferViaSocket(tibiaState.outbufFluidLife);
	if (!strcmp(sayBuf, "%ta fluidmana") && outFluidManaAvail)
		//Mine_send(tibiaSocket,tibiaState.outbufFluidMana,payloadLen(tibiaState.outbufFluidMana)+2,lastSendFlags);
		sendBufferViaSocket(tibiaState.outbufFluidMana);
}

int parseMessageForTibiaAction(char *buf, int len)
{
	//char buf2[1111]="hi ";
	//for (int i=0;i<min(len,1110);i++){ sprintf(buf2,"%s %x",buf2,(char)buf[i]); }
	//AfxMessageBox(buf2);
	static int gatherStats        = CModuleUtil::getTASetting("GatherBotStats");
	static int removeStatsMessage = CModuleUtil::getTASetting("RemoveBotStatsMessage");
	if (gatherStats)
		Protocol::outputPacket(NetworkMessage(buf));
	else if (removeStatsMessage)
		CModuleUtil::setTASetting("RemoveBotStatsMessage", 0);
	//fwrite(buf,len);
	//Protocol::parsePacketOut(NetworkMessage(buf));
	//char buf3[1111]="bye ";
	//for (i=0;i<min(len,1110);i++){sprintf(buf2,"%s %2x",buf2,buf[i]);}
	//AfxMessageBox(buf2);
	int code = buf[2];
	if (code < 0)
		code += 256;
	int buf3 = buf[3];
	int buf4 = buf[4];
	if (buf3 < 0)
		buf3 += 256;
	if (buf4 < 0)
		buf4 += 256;
	if (autoAimActive && !autoAimOnlyCreatures && code == 0x84 && buf[0] == 13 && buf3 == 0xff && buf4 == 0xff)
	{
		
		// cast rune against monster
		int contNr  = buf[5];
		int itemPos = buf[7];
		int v1      = buf[8];
		int v2      = buf[9];
		if (v1 < 0)
			v1 += 256;
		if (v2 < 0)
			v2 += 256;
		int objectId = v1 + v2 * 256;
		v1 = buf[11];
		v2 = buf[12];
		int v3 = buf[13];
		int v4 = buf[14];
		if (v1 < 0)
			v1 += 256;
		if (v2 < 0)
			v2 += 256;
		if (v3 < 0)
			v3 += 256;
		if (v4 < 0)
			v4 += 256;

		unsigned int playerId = v1 + v2 * 256 + v3 * 256 * 256 + v4 * 256 * 256 * 256;
		if (debugFile)
			WriteOutDebug("### %x, %x\r\n", objectId, playerId);
		if ((objectId == CTibiaItem::getValueForConst("runeHMM") ||
		     objectId == CTibiaItem::getValueForConst("runeGFB") ||
		     objectId == CTibiaItem::getValueForConst("runeSD") ||
		     objectId == CTibiaItem::getValueForConst("runeExplo")) &&
		    playerId < 0x40000000)
		{
			CMemReader& reader = CMemReader::getMemReader();
			CTibiaCharacter ch;
			if (reader.getCharacterByTibiaId(&ch, playerId))
			{
				castRuneAgainstHuman(contNr, itemPos, objectId, ch.x, ch.y, ch.z);
				return 1;
			}
		}
	}
	/**
	 */

	if (code == 0x96 && (buf[3] == 1 || buf[3] == 0xb))
	{
		// "say"
		char sayBuf[1000];
		int sayV1 = buf[4];
		int sayV2 = buf[5];
		if (sayV1 < 0)
			sayV1 += 256;
		if (sayV2 < 0)
			sayV2 += 256;
		int sayLen = sayV1 + 256 * sayV2;

		if (sayLen > 500)
			return 0;

		memset(sayBuf, 0, 1000);
		memcpy(sayBuf, buf + 6, sayLen);
		if (!strncmp(sayBuf, "%ta ", 3))
		{
			parseMessageSay(sayBuf);
			return 1;
		}
	}

	if (code == 0x96 && buf[3] == 7)
	{
		// "channel"
		char sayBuf[1000];
		int sayV1 = buf[6];
		int sayV2 = buf[7];
		if (sayV1 < 0)
			sayV1 += 256;
		if (sayV2 < 0)
			sayV2 += 256;
		int sayLen = sayV1 + 256 * sayV2;

		if (sayLen > 500)
			return 0;

		memset(sayBuf, 0, 1000);
		memcpy(sayBuf, buf + 8, sayLen);
		if (!strncmp(sayBuf, "%ta ", 3))
		{
			parseMessageSay(sayBuf);
			return 1;
		}
	}

	if (code == 0x96 && buf[3] == 6)
	{
		// "private"
		char sayBuf[1000];
		unsigned char nameV1 = buf[4];
		unsigned char nameV2 = buf[5];
		int nameLen          = nameV1 + 256 * nameV2;

		unsigned char sayV1 = buf[6 + nameLen];
		unsigned char sayV2 = buf[7 + nameLen];
		int sayLen          = sayV1 + 256 * sayV2;

		if (sayLen > 500)
			return 0;

		memset(sayBuf, 0, 1000);
		memcpy(sayBuf, buf + 8 + nameLen, sayLen);
		if (!strncmp(sayBuf, "%ta ", 3))
		{
			parseMessageSay(sayBuf);
			return 1;
		}
	}

	if (code == 0x96 && buf[3] == 27)
	{
		
		// "private/NPC outgoing"
		char sayBuf[1000];
		char nickBuf[1000];
		int nickV1 = buf[4];
		int nickV2 = buf[5];
		if (nickV1 < 0)
			nickV1 += 256;
		if (nickV2 < 0)
			nickV2 += 256;
		int nickLen = nickV1 + 256 * nickV2;

		if (nickLen > 500)
			return 0;

		memset(nickBuf, 0, 1000);
		memcpy(nickBuf, buf + 6, nickLen);

		int sayV1 = buf[6 + nickLen];
		int sayV2 = buf[7 + nickLen];
		if (sayV1 < 0)
			sayV1 += 256;
		if (sayV2 < 0)
			sayV2 += 256;
		int sayLen = sayV1 + 256 * sayV2;

		if (sayLen > 500)
			return 0;
		memset(sayBuf, 0, 1000);
		memcpy(sayBuf, buf + 8 + nickLen, sayLen);

		if (!strncmp(sayBuf, "%ta ", 3))
		{
			parseMessageSay(sayBuf);
			return 1;
		}
	}
	return 0;
}

void hookCallback(int value)
{
	CIpcMessage mess;
	char *message = NULL;
	if (value == 0x13)
		message = "%ta pause";
	if (value == 0x21)
		message = "%ta lu";
	if (value == 0x22)
		message = "%ta ld";
	if (message)
	{
		int len = strlen(message);
		memcpy(mess.payload, &len, sizeof(int));
		memcpy(mess.payload + 4, message, len);
		if (value == 0x21 || value == 0x22)
		{
			mess.messageType = 2001;
			ipcPipeBack.send(mess);
		}
		if (value == 0x13)
		{
			mess.messageType = 2002;
			ipcPipeBack.send(mess);
		}
	}
}

int WINAPI Mine_send(SOCKET s, char* buf, int len, int flags)
{
	int i;
	int identical = 1;
	if (len != encryptPos + 8 + 6)
	{
		identical = 0;
	}
	else
	{
		for (i = 0; i < encryptPos + 8; i++)
		{
			if (buf[6 + i] != encryptAfterBuf[i])
				identical = 0;
		}
	}
	if (debugFile && COMPLEX)
		WriteOutDebug("!!! send !!! [identical=%d]\r\n", identical);

	if (debugFile && !SENTONLY)
	{
		//bufToHexString(buf,len);
		//WriteOutDebug("E> [%x] %s\r\n",socket,bufToHexStringRet);
	}
	if (identical)
	{
		// pass for further processing only when we know content before encryption

		parseMessage(encryptBeforeBuf, encryptLen, debugFile, 1, 1);
		if (debugFile)
		{
			bufToHexString(encryptBeforeBuf, encryptLen);
			WriteOutDebug("-> [%x] %s\r\n", socket, bufToHexStringRet);
		}

		if (parseMessageForTibiaAction(encryptBeforeBuf, encryptLen))
			return len;
	}

	if (debugFile)
		fflush(debugFile);


	tibiaSocket   = s;
	lastSendFlags = flags;


	int ret = 0;
	ret = Real_send(s, buf, len, flags);

	return ret;
}

int WINAPI Mine_recv(SOCKET s, char* buf, int len, int flags)
{
	if (debugFile && COMPLEX && !SENTONLY)
		WriteOutDebug("!!! recv !!!\r\n");
	int offset = 0;

	int realRecvLen = 0;

	// use "standard" tibia own socket
	if (strncmp(buf + 15, "Aprem", 5) == 0)
		(buf + 15)[0] = 'o';
	realRecvLen = Real_recv(s, buf + offset, len - offset, flags);
	if (realRecvLen != -1)
	{
		if (debugFile && !SENTONLY)
		{
			if (COMPLEX)
				WriteOutDebug("realRecvLen=%d\r\n", realRecvLen);

			bufToHexString(buf, realRecvLen);
			WriteOutDebug("<- [%x] %s\r\n", socket, bufToHexStringRet);

			parseMessage(buf, realRecvLen, debugFile, 0, 1);
		}
	}


	if (debugFile)
		fflush(debugFile);

	tibiaSocket = s;


	return realRecvLen + offset;
}

SOCKET WINAPI Mine_socket(int af, int type, int protocol)
{
	SOCKET s = Real_socket(af, type, protocol);

	if (debugFile && COMPLEX)
		WriteOutDebug("got socket: %d, %d, %d -> %d\r\n", af, type, protocol, s);

	tibiaSocket = s;

	return s;
}

int WINAPI Mine_connect(SOCKET s, const struct sockaddr* name, int namelen)
{
	if (debugFile && COMPLEX)
		WriteOutDebug("connect [pre]: %d, %x, %d\r\n", s, name, namelen);


	int ret = Real_connect(s, name, namelen);

	if (debugFile && COMPLEX)
		WriteOutDebug("connect: %d, %x, %d\r\n", s, name, namelen);

	memcpy(lastConnectName, name, 16);

	return ret;
};


void InitialiseIPC()
{
	HMODULE aa = GetModuleHandle(0);
	char buf[128];
	char lpszPipename[1024];
	int pid = ::GetCurrentProcessId();
	sprintf(lpszPipename, "\\\\.\\pipe\\tibiaAutoPipe-%d", pid);


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
			break;


		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			sprintf(buf, "Could not open pipe (not busy): %d", GetLastError());
			//MessageBox(NULL,"","error 3",0);
			Sleep(50);
			continue;
		}

		// All pipe instances are busy, so wait for 10 seconds.

		if (!WaitNamedPipe(lpszPipename, 1000))
		{
			sprintf(buf, "Could not open pipe (busy too long): %d", GetLastError());
			//MessageBox(NULL,buf,"error",0);
			Sleep(50);
			continue;
		}
	}

	DWORD dwMode  = PIPE_READMODE_MESSAGE;
	BOOL fSuccess = SetNamedPipeHandleState(
	        hPipe,    // pipe handle
	        &dwMode,  // new pipe mode
	        NULL,     // don't set maximum bytes
	        NULL);    // don't set maximum time
	if (!fSuccess)
		InitialiseIPC();
	else if (debugFile && COMPLEX)
		WriteOutDebug("[debug] straight IPC initialised ok\r\n");
}

void InitialiseHooks()
{
	Real_send    = (int(WINAPI *)(SOCKET, char*, int, int))DetourFindFunction("wsock32.dll", "send");
	Real_recv    = (int(WINAPI *)(SOCKET, char*, int, int))DetourFindFunction("wsock32.dll", "recv");
	Real_connect = (int(WINAPI *)(SOCKET, const struct sockaddr*, int))DetourFindFunction("wsock32.dll", "connect");
	Real_socket  = (SOCKET(WINAPI *)(int, int, int))DetourFindFunction("wsock32.dll", "socket");
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)Real_send, Mine_send);
	DetourAttach(&(PVOID&)Real_recv, Mine_recv);
	DetourAttach(&(PVOID&)Real_connect, Mine_connect);
	DetourAttach(&(PVOID&)Real_socket, Mine_socket);
	DetourTransactionCommit();
};


void InitialiseDebugFile()
{
	if (OUTPUT_DEBUG_INFO)
		debugFile = fopen("C:\\temp\\tibiaDebug.txt", "wb");
	else
		debugFile = NULL;

	WriteOutDebug("Start\r\n");
	debugFileStart = time(NULL);
}

void InitialiseTibiaState()
{
	tibiaState.attackedCreature = 0;
	memset(tmp3, 0x00, 256);
}

void InitialiseCreatureInfo()
{
	int i;
	for (i = 0; i < MAX_CREATUREINFO; i++)
	{
		creatureInfoPlayerName[i][0] = 0;
	}
}

__declspec(noinline) int OUTmyDrawRect(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9, DWORD drawRectPtr)
{
	int retvar;
	typedef void(*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);

	Proto_fun fun = (Proto_fun)(drawRectPtr);

	__asm {
		push v9 //nBlue
		push v8 //nGreen
		push v7 //nRed
		push v6 //nHeight
		push v5 //nWeight
		push v4 //nY
		push v3 //nX
		push v2 //nSurface
		mov ecx, v1 //unknown
		call fun
		add esp, 0x20
		mov retvar, eax
	}
	return retvar;

}

void myDrawRect(int ebp, int ecx, int nSurface, int nX, int nY, int nWeight, int nHeight, int nRed, int nGreen, int nBlue, DWORD drawRectPtr)
{
	int creaturePointer = *(int*)(ebp - 0x5D6C);
	char *creatureID = (char*)(creaturePointer);

	CMemReader& reader = CMemReader::getMemReader();
	CTibiaCharacter self;
	reader.readSelfCharacter(&self);

	if (strcmp(self.name, creatureID) == 0 && showManaBar)
	{
		OUTmyDrawRect(ecx, nSurface, nX, nY-5, nWeight, nHeight, nRed, nGreen, nBlue, drawRectPtr); //draw hp bar 5 pixels higher

		if (!(!nRed && !nGreen && !nBlue)) //if it is not black bar being drawn
		{
			float myRed = ((1 - ((float)self.mana / (float)self.maxMana)) * 0xC0);
			float myBlue = ((float)self.mana / (float)self.maxMana) * 0xC0;
			float hpPorc = ((float)self.hp / (float)self.maxHp);

			int luminosity;

			if (hpPorc > 0.30) // hp bar change from yellow to red at 30%
			{
				luminosity = 0xC0 - nGreen;
			}
			else // when hp< 30%, green color change from 0xC0 to 0x30
			{
				luminosity = 0xC0 - nRed;
			}
					
			if (myBlue < 0x30) // minimum blue color
			{
				myBlue = 0x30;
			}

			if (myRed < 0x20) // minimum red color
			{
				myRed = 0x20;
			}


			if ((myBlue - luminosity) > 0) //adjust blue color to light
				myBlue = myBlue - (float)luminosity / (float)1.5;
			else
				myBlue = 0;

			myRed = myRed - (float)luminosity/(float)4; //adjust red color to light
			if (myRed < 0)
				myRed = 0;

			float myWeight = ((float)self.mana / (float)self.maxMana) * 0x1A; // "0x1A" is the maxWeight of colored bar

			OUTmyDrawRect(ecx, nSurface, nX, nY, int(myWeight), nHeight, int(myRed), 0, int(myBlue), drawRectPtr);
		}
		else
			OUTmyDrawRect(ecx, nSurface, nX, nY, nWeight, nHeight, nRed, nGreen, nBlue, drawRectPtr);
			
	}
	else
		OUTmyDrawRect(ecx, nSurface, nX, nY, nWeight, nHeight, nRed, nGreen, nBlue, drawRectPtr);

}

__declspec(naked) void INmyDrawRect() //(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8)
{
	__asm {
		mov ecx, dword ptr[ebp - 0x5D8C]
		push ebp
		mov ebp, esp
		push[edx + 0x14]
		push[ebp + 0x24]
		push[ebp + 0x20]
		push[ebp + 0x1C]
		push[ebp + 0x18]
		push[ebp + 0x14]
		push[ebp + 0x10]
		push[ebp + 0x0C]
		push[ebp + 0x08]
		push ecx //unknown parameter
		push [ebp] // ebp is needed only to know which creatureID is being drawn
		call myDrawRect
		leave
		ret 0x24
	}
}

__declspec(naked) void INmyDrawBlackRect() //(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8)
{
	__asm {
		mov ecx, dword ptr[ebp - 0x5D4C]
		push ebp
		mov ebp, esp
		push[edx + 0x14]
		push[ebp + 0x24]
		push[ebp + 0x20]
		push[ebp + 0x1C]
		push[ebp + 0x18]
		push[ebp + 0x14]
		push[ebp + 0x10]
		push[ebp + 0x0C]
		push[ebp + 0x08]
		push ecx //unknown parameter
		push [ebp] // ebp is needed only to know which creatureID is being drawn
		call myDrawRect
		leave
		ret 0x24
	}
}

//(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<ecx>, int v9)
__declspec(noinline) int OUTmyPrintText(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9)
{
	int retvar;
	typedef void (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9);

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaPrintText);

	__asm {
		push v9 //[ebp + 0x20]
		push v8 //[ebp + 0x1C]
		push v7 //[ebp + 0x18]
		push v6 //[ebp + 0x14]
		push v5 //[ebp + 0x10]
		push v4 //[ebp + 0x0C]
		push v3 //[ebp + 0x08]
		mov edx, v2
		mov ecx, v1
		call fun
		add esp, 0x20
		mov retvar, eax
	}
	return retvar;
}

int myPrintText(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign)
{
	//myPrintText runs continuously, so this is a good place to check if the TA Message pipe has anything for us
	//EXCEPT when display creature names is off
	if (taMessageStart != taMessageEnd)
	{
		if (debugFile && COMPLEX)
			WriteOutDebug("privChanBufferPtr=%x\r\n", privChanBufferPtr);
		sendTAMessage(taMessage[taMessageEnd]);
		taMessageEnd++;
		if (taMessageEnd == TA_MESSAGE_QLEN)
			taMessageEnd = 0;
	}
	CMemReader& reader = CMemReader::getMemReader();
	CTibiaCharacter self;
	reader.readSelfCharacter(&self);
	int creatureID = *(int*)(lpText - 4); //Character ID is currently stored in data before text string
	if (self.tibiaId == creatureID)
	{
		if (showManaBar){ // print name a bit higher for extra space to input the second bar
			nY = nY - 5;
		}
		// Runt printText for HUD only once at the time the character name is printed
		vector<HUD>::size_type i;
		for (i = 0; i != vecHUD.size(); i++)
		{
			if (vecHUD[i].pos.x > 2 && vecHUD[i].pos.y && !(vecHUD[i].message.empty()) && vecHUD[i].message[0] != '\0'){
				OUTmyPrintText(1, vecHUD[i].pos.x, vecHUD[i].pos.y, nFont, vecHUD[i].redColor, vecHUD[i].greenColor, vecHUD[i].blueColor, const_cast<char*>(vecHUD[i].message.c_str()), 0);
			}

		}
	}

	int ret = OUTmyPrintText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
	return ret;
}

__declspec(naked) void INmyPrintText() //(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<ecx>, int v9)
{
	__asm {
		push ebp
		mov ebp, esp
		push [ebp + 0x20]
		push [ebp + 0x1C]
		push [ebp + 0x18]
		push [ebp + 0x14]
		push [ebp + 0x10]
		push [ebp + 0x0C]
		push [ebp + 0x08]
		push edx
		push ecx
		call myPrintText
		leave
		ret
	}
}

//(int align<ecx>, char *str<edx>, int visible, int x, int y, int fontNumber, int colR, int colG, int colB, int showFormatting, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, int v16<ebx>, int v17<edi>)
__declspec(noinline) int OUTmyPlayerNameText(int alignvar, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char *strvar, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17)
{
	int retvar;
	typedef int (*Proto_fun)(int align, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char *str, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17);

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaPlayerNameText);

	__asm {
		mov edi, v17 //[ebp + 0x48]
		mov ebx, v16 //[ebp + 0x44]
		push cropHeight //[ebp + 0x40]
		push cropWidth //[ebp + 0x3C]
		push cropTop //[ebp + 0x38]
		push cropLeft //[ebp + 0x34]
		push charCut //[ebp + 0x30]
		push strvar //[ebp + 0x2C]
		push colB //[ebp + 0x28]
		push colG //[ebp + 0x24]
		push colR //[ebp + 0x20]
		push showFormatting //[ebp + 0x1C]
		push y //[ebp + 0x18]
		push x //[ebp + 0x14]
		push visible //[ebp + 0x10]
		mov edx, fontNumber //[ebp + 0x0C]
		mov ecx, alignvar //[ebp + 0x08]
		call fun
		add esp, 0x34
		mov retvar, eax
	}
	return retvar;
}

int myPlayerNameText(int align, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char* str, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17)
{
	int ret;
	int titleOffset = 0;
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
	if (str != NULL && fontNumber == 2 && strlen(str) < 1000)
	{
		char info1[1024];
		char info2[1024];
		info1[0] = info2[0] = 0;
		int i, len;
		char convString[1024];

		sprintf(convString, "%s", str);
		for (i = 0, len = strlen(str); i < len; i++)
		{
			if (convString[i] == '[')
				convString[i] = '\0';
		}

		for (i = 0; i < MAX_CREATUREINFO; i++)
		{
			if (!strcmp(str, creatureInfoPlayerName[i]))
			{
				strncpy(info1, creatureInfoPlayerInfo1[i], 128);
				strncpy(info2, creatureInfoPlayerInfo2[i], 128);
				break;
			}
		}

		if (strlen(info2))
		{
			OUTmyPlayerNameText(align, fontNumber, visible, x, y - titleOffset, 10, colR, colG, colB, info2, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
			titleOffset += 14;
		}
		if (strlen(info1))
		{
			OUTmyPlayerNameText(align, fontNumber, visible, x, y - titleOffset, showFormatting, colR, colG, colB, info1, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
			titleOffset += 14;
		}
		ret = OUTmyPlayerNameText(align, fontNumber, visible, x, y - titleOffset, showFormatting, colR, colG, colB, convString, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
	}
	else
	{
		ret = OUTmyPlayerNameText(align, fontNumber, visible, x, y - titleOffset, showFormatting, colR, colG, colB, str, charCut, cropLeft, cropTop, cropWidth, cropHeight, v16, v17);
	}
	return ret;
}

__declspec(naked) void INmyPlayerNameText() //()
{
	__asm {
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
	}
}

void OUTmyInterceptInfoMiddleScreen(int typevar, char* s)
{
	typedef void (*Proto_fun)(int typevar, char *s);

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaInfoMiddleScreen);

	__asm {
		mov edx, s //[ebp + 0x0C]
		mov ecx, typevar //[ebp + 0x08]
		call fun
	}
}

void myInterceptInfoMiddleScreen(int type, char *s)
{
	if (debugFile && COMPLEX)
		WriteOutDebug("got middle screen %d/%s\r\n", type, s);

	char buf[1111];
	if (s)
		sprintf(buf, "XXY v1Type=%x, v2Text=%s", type, s);
	else
		sprintf(buf, "XXY v1Type=%x, v2Text=", type);

	//sendTAMessage(buf);
	if (type == 0x13)
	{
		if (debugFile && COMPLEX)
			WriteOutDebug("got see '%s'\r\n", s);
		unsigned long bytesWritten = 0;

		CIpcMessage mess;
		mess.messageType = 1002;
		int len = strlen(s);
		if (len < 900)
		{
			memcpy(mess.payload, &len, sizeof(int));
			memcpy(mess.payload + 4, s, len);
			ipcPipeBack.send(mess);
		}
	}


	if (type != 0x13 || time(NULL) > ignoreLookEnd)
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
		OUTmyInterceptInfoMiddleScreen(type, s);
	else if (debugFile && COMPLEX)
		WriteOutDebug("ignoring look");
}

// void(int type<ecx>, char* text<edx>)
__declspec(naked) void INmyInterceptInfoMiddleScreen() //()
{
	__asm {
		push ebp
		mov ebp, esp
		push edx
		push ecx
		call myInterceptInfoMiddleScreen
		leave
		ret
	}
}

int myIsCreatureVisible(int *creaturePtr) //Should Draw Creature(not used)
{
	if (revealCNameActive)
	{
		int ret = 0;

		static CMemReader *reader = NULL;

		if (!reader)
			reader = &CMemReader::getMemReader();
		static int firstAddr = reader->m_memAddressFirstCreature;
		static int crSize = reader->m_memLengthCreature;


		if (creaturePtr)
		{
			int *coordPtr;
			coordPtr = creaturePtr + 36 / 4;
			int coordX = *coordPtr;
			coordPtr = creaturePtr + 40 / 4;
			int coordY       = *coordPtr;
			int loggedCharNr = reader->getLoggedCharNr();
			coordPtr = (int *)(firstAddr + loggedCharNr * crSize + 36);
			int selfX = *coordPtr;
			coordPtr = (int *)(firstAddr + loggedCharNr * crSize + 40);
			int selfY = *coordPtr;
			if (abs(coordX - selfX) <= 8 && abs(coordY - selfY) <= 6)
				ret = 1;
		}
		return ret;
	}
	else
	{
		typedef int (*Proto_fun)(int *creaturePtr);

		Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaIsCreatureVisible);


		return fun(creaturePtr);
	}
}

__declspec(noinline) int OUTmyInterceptEncrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	int retvar;
	typedef void (*Proto_fun)(int v1, int v2);

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaEncrypt);

	__asm {
		mov edx, v2 // move v2 to edx
		mov ecx, v1 // move v1 to ecx
		call fun // jump to the real Tibia function
		mov retvar, eax //store value returned from real function in ret
	}
	return retvar;
}

int myInterceptEncrypt(int v1, int v2)
{
	encryptKeyPtr = v2;
	if (debugFile && COMPLEX)
		WriteOutDebug("QQQQQQQQQQ: %x,%x,%x\r\n", encryptKeyPtr, v1, v2);

	if (v1 != encryptPrevPtr + 8)
	{
		// means: next packet is being encrypted
		encryptPrevPtr = v1;
		int lowB = ((char *)v1)[0];
		int hiB  = ((char *)v1)[1];
		if (lowB < 0)
			lowB += 256;
		if (hiB < 0)
			hiB += 256;
		encryptLen = lowB + hiB * 256 + 2;
		encryptPos = 0;
	}
	else
	{
		encryptPos     += 8;
		encryptPrevPtr += 8;
	}

	memcpy(encryptBeforeBuf + encryptPos, (void *)v1, 8);
	int ret = OUTmyInterceptEncrypt(v1, v2);
	memcpy(encryptAfterBuf + encryptPos, (void *)v1, 8);
	return ret;
}

__declspec(naked) void INmyInterceptEncrypt() //<eax>(int v1<ecx>, int v2<edx>)
{
	__asm {
		push ebp
		mov ebp, esp
		push edx // push v2 to stack
		push ecx // push v1 to stack
		call myInterceptEncrypt
		leave
		    ret
	}
}

__declspec(noinline) int OUTmyInterceptDecrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	int retvar; //create [ebp - 4] variable
	typedef void (*Proto_fun)();

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaDecrypt);

	__asm {
		mov edx, v2 // move v2 to edx
		mov ecx, v1 // move v1 to ecx
		call fun // jump to the real Tibia function
		mov retvar, eax //store value returned from real function in ret
	}
	return retvar;
}

int myInterceptDecrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	if (taMessageStart != taMessageEnd)
	{
		if (debugFile && COMPLEX)
			WriteOutDebug("privChanBufferPtr=%x\r\n", privChanBufferPtr);
		sendTAMessage(taMessage[taMessageEnd]);
		taMessageEnd++;
		if (taMessageEnd == TA_MESSAGE_QLEN)
			taMessageEnd = 0;
	}

	encryptKeyPtr = v2;
	return OUTmyInterceptDecrypt(v1, v2);
}

__declspec(naked) void INmyInterceptDecrypt() //<eax>(int v1<ecx>, int v2<edx>)
{
	__asm {
		push ebp //store extended base pointer for prev func
		mov ebp, esp //use current stack pointer as base pointer
		push edx // push v2 to stack
		push ecx // push v1 to stack
		call myInterceptDecrypt //jump to actuar function
		leave //move EBP to ESP and pop EBP, the extended base pointer on the stack gets restored
		    ret //jump back to location on stack
	}
}

/*__declspec(noinline) int OUTmyInterceptAllRegs(int v1, int v2, int v3, int v4, int v5, int v6, int va, int vb, int vc, int vd, int vdi, int vsi)
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

//This captured function is what Tibia uses to decrypt the packet
// and determine which action types it contains in order to parse it properly
// The function returns the action type and Tibia then parses the stream.
// If the stream has not yet been used up, it will continually run this function to
// determine the type of and parse the next action.
// We can interfere with this process, but this requires knowing packet structures
// and error handling for incomplete packets which takes much more work than payoff.
int prevRecvStreamPos = 0;
int prevRecvStreamLen = 0;
int prevNextRet       = 0;
char prevRecvStream[32768];
int myShouldParseRecv()
{
	CMemReader& reader = CMemReader::getMemReader();
	
	typedef int (*Proto_fun)();

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaShouldParseRecv);
	//This function looks ahead, it returns the next packet type. It will return -1 if no next packet.
	int ret = fun();

	//look for this address near above location, it will be off by 8

	packStream* recvStream = (packStream*)baseAdjust(arrayPtr_recvStream);
	//int packLen = ((unsigned char)recvStream->s[6]) + ((unsigned char)recvStream->s[7])*256;

	if (prevNextRet != -1)
	{
		int actionStart = prevRecvStreamPos - 1; //Tibia eats 1 byte to return "ret" every call it is not -1
		int actionEnd; // index of last byte
		if (recvStream->pos <= prevRecvStreamPos || recvStream->pos > prevRecvStreamLen)  // when stream restarts, use previous length instead of current pos
			actionEnd = prevRecvStreamLen - 1;
		else if (ret == -1)
			actionEnd = recvStream->pos - 1; // did not eat an extra byte
		else
			actionEnd = recvStream->pos - 2; // ate an extra byte
		int actionLen = actionEnd - actionStart + 1;
		//Parse packet and perform any needed actions
		Protocol::parsePacketIn(NetworkMessage((char*)(prevRecvStream + actionStart), actionLen));
		/*
		   int packtype = ((char*)prevRecvStream+actionStart)[0]&0xff;
		   int wanted = 0xb4;
		   if(packtype == wanted){
		        Protocol::parsePacketIn(NetworkMessage((char*)(prevRecvStream+actionStart),actionLen),ipcPipeBack);
		        bufToHexString(((char*)prevRecvStream+actionStart),actionLen);
		        sendTAMessage(bufToHexStringRet);
		   }*/
		{for (int i = 0; i < recvRegexCount; i++)
		 {
			 if (recvRegex[i].inUse == 1)
			 {
				 int match = regnexec(&(recvRegex[i].preg), ((char*)prevRecvStream + actionStart), actionLen, 0, NULL, 0);
				 if (match == 0)
				 {
					 if (0)
					 {
						 bufToHexString(((char*)prevRecvStream + actionStart), actionLen);
						 sendTAMessage(bufToHexStringRet);
					 }
					 parseRecvActionData(recvRegex[i].handle, (char*)prevRecvStream + actionStart, actionLen);
				 }
			 }
		 }
		}
	}
	if (ret != -1 && (recvStream->pos <= prevRecvStreamPos || recvStream->pos > prevRecvStreamLen))
	{
		prevRecvStreamLen = recvStream->length;
		memcpy(prevRecvStream, recvStream->s, prevRecvStreamLen);
	}
	prevRecvStreamPos = recvStream->pos;
	prevNextRet       = ret;

	//char buf[1111];
	//sprintf(bufToHexStringRet,"%x:%d, %d  %x%x",ret, recvStream->pos, recvStream->length,((char)recvStream->s[recvStream->pos-2]),((char)recvStream->s[recvStream->pos-2-1]));
	//AfxMessageBox(buf);
	//bufToHexString(recvStream->s,30);
	//sendTAMessage(bufToHexStringRet);
	if (0 && reader.isLoggedIn()) // see what happens when packets are ignored
	{
		if (ret == 107)
			ret = -1;
		if (ret == -1)
			recvStream->pos += 10;
	}
	return ret;
}

//<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
__declspec(noinline) int OUTmyInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9)
{
	int retval;
	typedef int (*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);

	Proto_fun fun = (Proto_fun)baseAdjust(funAddr_tibiaInfoMessageBox);

	__asm {
		push v9 //[ebp + 0x28]
		push v8 //[ebp + 0x24]
		push v7 //[ebp + 0x20]
		push v6 //[ebp + 0x1C]
		push v5 //[ebp + 0x18]
		push v4 //[ebp + 0x14]
		push v3 //[ebp + 0x10]
		mov edx, v2 //[ebp + 0x0C]
		mov ecx, v1 //[ebp + 0x08]
		call fun
		add esp, 0x1C
		mov retval, eax
	}
	return retval;
}

//<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
int myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9)
{
	char buf[1111];
	sprintf(buf, "XXX v1ConsolePointer=%x, v2Time=%x, v3Text=%x, v4Type=%x, v5Name=%x, v6=%x, v7Type2=%x, v8Lvl=%d, v9=%x\r\n", v1, v2, v3, v4, v5, v6, v7, v8, v9);
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

	char *nick = (char *)v5;
	char *s    = (char *)v3;

	int type     = v4;
	int chantype = v9;
	if (debugFile)
		WriteOutDebug("XXX s1=%s s2=%s\r\n", s, nick);
	//char* happy2="dddddddddddddddddddd";
	//memcpy(*(char**)(*(char**)v1),happy2,strlen(happy2));
	// note: at least 0x14 bytes are passed on stack; at most 0x2c bytes are passed

	if (type == 1)
		privChanBufferPtr = v1;


	int nickLen = nick ? strlen(nick) : 0;
	int msgLen  = strlen(s);
	if (nickLen + msgLen < 900)
	{
		CIpcMessage mess;
		memcpy(mess.payload, &type, sizeof(int));
		memcpy(mess.payload + 4, &chantype, sizeof(int));
		memcpy(mess.payload + 8, &nickLen, sizeof(int));
		memcpy(mess.payload + 12, &msgLen, sizeof(int));
		if (nickLen)
			memcpy(mess.payload + 16, nick, nickLen);
		memcpy(mess.payload + 16 + nickLen, s, msgLen);
		mess.messageType = 1001;
		ipcPipeBack.send(mess);
		mess.messageType = 1003;
		ipcPipeBack.send(mess);
		mess.messageType = 1004;
		ipcPipeBack.send(mess);
		//mess.messageType=1005;
		//ipcPipeBack.send(mess);
		mess.messageType = 1006;
		ipcPipeBack.send(mess);
	}

	if (type != 0x13 || time(NULL) > ignoreLookEnd)
	{
		//int happy=*(int*)v1;
		//happy+=0x20;
		//*(int***)v1+=0x20;

		return OUTmyInterceptInfoMessageBox(v1, v2, v3, v4, v5, v6, v7, v8, v9);
	}
	return 2;
}

__declspec(naked) void INmyInterceptInfoMessageBox() //<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
{
	__asm {
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
	}
}

void trapFun(HANDLE dwHandle, int addr, unsigned int targetFun)
{
	int targetAddr = targetFun - addr - 4;
	WriteProcessMemory(dwHandle, (void *)addr, &targetAddr, sizeof(long int), NULL);
}

void hookDrawRect(HANDLE dwHandle, int addr, unsigned int targetFun)
{
	// CALL, 32bit addr, 4x NOP
	BYTE bytes[9] = { 0xE8, 0xFF, 0xFF, 0xFF, 0xFF, 0x90, 0x90, 0x90, 0x90 };
	DWORD targetAddr = targetFun - addr - 5;
	*(DWORD*)(bytes + 1) = targetAddr;
	WriteProcessMemory(dwHandle, (void *)addr, bytes, 9, NULL);
}

void InitialisePlayerInfoHack()
{
	DWORD procId    = GetCurrentProcessId();
	HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);

	hookDrawRect(dwHandle, baseAdjust(callAddr_DrawBlackRect - 6), (unsigned int)INmyDrawBlackRect); // first layer black bar
	hookDrawRect(dwHandle, baseAdjust(callAddr_DrawRect - 6), (unsigned int)INmyDrawRect); // second layer colored bar

	trapFun(dwHandle, baseAdjust(callAddr_PrintText03 + 1), (unsigned int)INmyPrintText);
	trapFun(dwHandle, baseAdjust(callAddr_PrintText04 + 1), (unsigned int)INmyPrintText);

	// lookup: find string In(FontNumber,1 [6th match is in the middle of the function]

	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText01 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText02 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText03 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText04 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText05 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText06 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText07 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText08 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText09 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(callAddr_PlayerNameText10 + 1), (unsigned int)INmyPlayerNameText);

	// lookup: TALK_INFO_MESSAGE; this is inside of the function

	trapFun(dwHandle, baseAdjust(callAddr_InfoMiddleScreen01 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMiddleScreen02 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMiddleScreen03 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMiddleScreen04 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);

	// lookup: TargetBuffer!=NULL; first call is in the middle of the infomessage function
	//         in fact you need to increcept all calls

	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox01 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox02 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox03 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox04 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox05 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox06 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox07 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox08 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox09 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox10 + 1), (unsigned int)INmyInterceptInfoMessageBox);//BADBAD
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox11 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox12 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(callAddr_InfoMessageBox13 + 1), (unsigned int)INmyInterceptInfoMessageBox);

	// lookup: manually match around previous address by assembly similarity
	trapFun(dwHandle, baseAdjust(callAddr_Encrypt01 + 1), (unsigned int)INmyInterceptEncrypt);

	// lookup: function below encrypt is the decrypt function
	trapFun(dwHandle, baseAdjust(callAddr_Decrypt01 + 1), (unsigned int)INmyInterceptDecrypt);


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
	trapFun(dwHandle, baseAdjust(callAddr_ShouldParseRecv01 + 1), (unsigned int)myShouldParseRecv);

	//This allows TA ta handle any errors before they are sent to Tibia
	//trapFun(dwHandle,baseAdjust(0x460EB3+1),(unsigned int)myShouldParseRecv); // 9.43 // need to find stuff for this

	CloseHandle(dwHandle);
}

void InitialiseProxyClasses()
{
	CMemUtil::getMemUtil().setGlobalProcessId(GetCurrentProcessId());
}

WNDPROC wndProcOriginal = NULL;

LRESULT APIENTRY FilterProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox("I did it!");
	if (uMsg == WM_COMMAND)
		if (LOWORD(wParam) == 4001)
			AfxMessageBox("I did it!");

	return CallWindowProc(wndProcOriginal, hwnd, uMsg, wParam, lParam);
}

void GetDebugPrivs()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tp;

	if (::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		if (!::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
			::CloseHandle(hToken);

		tp.PrivilegeCount           = 1;
		tp.Privileges[0].Luid       = sedebugnameValue;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
			::CloseHandle(hToken);

		::CloseHandle(hToken);
	}
}

BOOL CALLBACK EnumWindowsProc(

        HWND hwnd,
        LPARAM lParam
        )
{
	DWORD procId;
	GetWindowThreadProcessId(hwnd, &procId);
	if (procId == ::GetCurrentProcessId())
	{
		CWnd *wnd = new CWnd();
		wnd->Attach(hwnd);


		CMenu *subMenu1 = new CMenu();
		subMenu1->CreatePopupMenu();
		subMenu1->AppendMenu(MF_STRING, 4001, "sub 1");
		subMenu1->AppendMenu(MF_STRING, 4002, "sub 2");
		subMenu1->AppendMenu(MF_STRING | MF_ENABLED, 4003, "Sub 3");

		CMenu *subMenu2 = new CMenu();
		subMenu2->CreatePopupMenu();
		subMenu2->AppendMenu(MF_STRING, 4010, "sub 10");
		subMenu2->AppendMenu(MF_STRING, 4011, "sub 11");
		subMenu2->EnableMenuItem(4011, 1);


		CMyMenu *menu = new CMyMenu();
		menu->CreateMenu();
		menu->AppendMenu(MF_STRING, 4013, "test menu 1");
		menu->AppendMenu(MF_STRING, 4015, "test menu 2");
		menu->AppendMenu(MF_POPUP | MF_DISABLED, (int)subMenu1->GetSafeHmenu(), "test menu 3");
		menu->AppendMenu(MF_POPUP, (int)subMenu2->GetSafeHmenu(), "test menu 4");
		//menu->LoadOwnerDrawMenu(menu);


		wnd->SetMenu(menu);
		wndProcOriginal = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)(WNDPROC)FilterProc);
		if (wndProcOriginal)
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
	EnumWindows(&EnumWindowsProc, NULL);
}

BOOL APIENTRY DllMain(HINSTANCE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
	static HINSTANCE hinstDLL = hModule;


	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitialiseProxyClasses();
		InitialiseDebugFile();
		InitialiseTibiaState();
		InitialiseHooks();
		InitialiseCommunication();
		InitialisePlayerInfoHack();
		InitialiseCreatureInfo();
		//InitialiseTibiaMenu();
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}


	return TRUE;
}

void ParseIPCMessage(CIpcMessage mess)
{
	switch (mess.messageType)
	{
	case 1:
		if (tibiaSocket != NULL)
		{
			char buf[3];
			buf[0] = 0x1;
			buf[1] = 0x0;
			buf[2] = 0x67;
			Mine_send(tibiaSocket, buf, 3, 0);

			break;
		};
	case 2:
		if (tibiaSocket != NULL)
			sendBufferViaSocket((unsigned char*)mess.payload);

		;
		break;
	case 3:
		if (taMessageStart != taMessageEnd - 1 && !(taMessageStart == TA_MESSAGE_QLEN - 1 && taMessageEnd == 0))
		{
			strcpy(taMessage[taMessageStart], mess.payload);
			taMessageStart++;
			if (taMessageStart == TA_MESSAGE_QLEN)
				taMessageStart = 0;
		}
		break;
	case 4:
		if (debugFile && COMPLEX)
			WriteOutDebug("[debug] will try to connect back IPC pipe\r\n");
		if (!ipcPipeBack.hasInvalidHandle())
			// close back pipe
			ipcPipeBack.closePipe();
		memcpy(&partnerProcessId, mess.payload, sizeof(int));
		ipcPipeBack.InitialiseIPCback(partnerProcessId, debugFile, COMPLEX);
		break;
	case 100:
		memcpy(&tibiaState.attackedCreature, mess.payload, sizeof(int));
		break;
	case 101:
		outHmmAvail = 1;
		memcpy(&tibiaState.outbufHmm, mess.payload, payloadLen(mess.payload) + 2);
		break;
	case 102:
		outGfbAvail = 1;
		memcpy(&tibiaState.outbufGfb, mess.payload, payloadLen(mess.payload) + 2);
		break;
	case 103:
		outSdAvail = 1;
		memcpy(&tibiaState.outbufSd, mess.payload, payloadLen(mess.payload) + 2);
		break;
	case 104:
		outExploAvail = 1;
		memcpy(&tibiaState.outbufExplo, mess.payload, payloadLen(mess.payload) + 2);
		break;
	case 105:
		outSelfUHAvail = 1;
		memcpy(&tibiaState.outbufSelfUH, mess.payload, payloadLen(mess.payload) + 2);
		break;
	case 106:
		outFluidManaAvail = 1;
		memcpy(&tibiaState.outbufFluidMana, mess.payload, payloadLen(mess.payload) + 2);
		break;
	case 107:
		outFluidLifeAvail = 1;
		memcpy(&tibiaState.outbufFluidLife, mess.payload, payloadLen(mess.payload) + 2);
		break;
	case 201:
		revealFish = 1;
		break;
	case 202:
		revealFish = 0;
		break;
	case 301:
	{
		int found = 0;
		int i;
		for (i = 0; i < MAX_CREATUREINFO; i++)
		{
			if (!strcmp(creatureInfoPlayerName[i], mess.payload))
			{
				found = 1;
				memcpy(creatureInfoPlayerInfo1[i], mess.payload + 32, 500);
				memcpy(creatureInfoPlayerInfo2[i], mess.payload + 32 + 500, 500);
				break;
			}
		}
		if (!found)
		{
			memcpy(creatureInfoPlayerName[creatureInfoNext], mess.payload, 32);
			memcpy(creatureInfoPlayerInfo1[creatureInfoNext], mess.payload + 32, 500);
			memcpy(creatureInfoPlayerInfo2[creatureInfoNext], mess.payload + 32 + 500, 500);
			creatureInfoNext++;
			// roll creatureInfoText if we are out of limit
			if (creatureInfoNext == MAX_CREATUREINFO)
				creatureInfoNext = 0;
		}
		break;
	}
	case 302:
	{
		int tmpignoreLookEnd = 0;
		memcpy(&tmpignoreLookEnd, mess.payload, 4);
		ignoreLookEnd = tmpignoreLookEnd;
		break;
	}
	case 303:
	{
		memcpy(&autoAimActive, mess.payload, 4);
		memcpy(&autoAimOnlyCreatures, mess.payload + 4, 4);
		memcpy(&autoAimAimPlayersFromBattle, mess.payload + 8, 4);
		break;
	}
	case 304:
	{
		InitialiseCreatureInfo();
		break;
	}
	case 305:
	{
		revealCNameActive = 1;
		break;
	};
	case 306:
	{
		revealCNameActive = 0;
		break;
	}
	case 307:
	{
		//CString buf;
		//buf.Format("(%d, %d) rgb(%d, %d, %d) %d '%s'", mess.payload[0], mess.payload[4], mess.payload[8], mess.payload[12], mess.payload[16], mess.payload[20], mess.payload+24);
		//AfxMessageBox(buf);
		int x, y, red, green, blue, messLen;
		
		memcpy(&x, mess.payload, sizeof(int));
		memcpy(&y, mess.payload + 4, sizeof(int));
		memcpy(&red, mess.payload + 8, sizeof(int));
		memcpy(&green, mess.payload + 12, sizeof(int));
		memcpy(&blue, mess.payload + 16, sizeof(int));
		memcpy(&messLen, mess.payload + 20, sizeof(int));

		string message(mess.payload + 24, min(MAX_PRINTEXT_LEN,messLen));

		bool actionComplete = false;
		vector<HUD>::size_type i;
		for (i = 0; i != vecHUD.size(); i++)
		{
			//buf.Format("(%d, %d), %d", vecHUD[i].pos.x, vecHUD[i].pos.y, vecHUD.size());
			//AfxMessageBox(buf);
			if (vecHUD[i].pos.x == x && vecHUD[i].pos.y == y)
			{
				if (messLen != 0)          // Update message on screen at point (mess.data[0], mess.data[1]):
				{
					vecHUD[i].redColor = red;
					vecHUD[i].greenColor = green;
					vecHUD[i].blueColor = blue;
					vecHUD[i].message = message;
					//AfxMessageBox("update");
					actionComplete = true;
					break;
				}
				else          // "Delete" message from screen
				{
					vecHUD[i].pos.x = 0;
					vecHUD[i].pos.y = 0;
					vecHUD[i].redColor = 0;
					vecHUD[i].greenColor = 0;
					vecHUD[i].blueColor = 0;
					vecHUD[i].message.clear();
					vecHUD.erase(vecHUD.begin() + i);
					//AfxMessageBox("delete");
					actionComplete = true;
					break;
				}
			}
		}
		if (!actionComplete)           // Add a message to the screen if there is room to store it and no delete or update action took place
		{
			HUD tmpHUD;			
			tmpHUD.pos.x = x;
			tmpHUD.pos.y = y;
			tmpHUD.redColor = red;
			tmpHUD.greenColor = green;
			tmpHUD.blueColor = blue;
			tmpHUD.message = message;
			vecHUD.push_back(tmpHUD);
			//AfxMessageBox("add");
		}
		break;
	}
	case 308: // adds given regex to list
	{
		switch (*(int*)mess.payload)
		{
		case 1:
		{
			
			int handle, regLen;
			memcpy(&handle, mess.payload + 4, sizeof(int));
			// Since we are using the array at the same time we are creating it, we do not move around items in use to fill empty spaces
			// this is run only once when the python script is loaded so this inefficioncy is acceptable.
			// Handles are the same when TA restarts so disable duplicate handles
			int i;
			for (i = 0; i < RECV_REGEX_MAX && recvRegex[i].inUse != 0; i++)            //find first empty space
			{
				if (recvRegex[i].handle == handle)                  // disable duplicate handles as TA has restarted
					recvRegex[i].inUse = 0;
			}
			if (i < RECV_REGEX_MAX)
			{
				memcpy(&regLen, mess.payload + 8, sizeof(int));
				char* regExp = (char*)malloc(regLen);
				memcpy(regExp, mess.payload + 12, regLen);

				recvRegex[i].handle = handle;
				if (regncomp(&(recvRegex[i].preg), regExp, regLen, REG_NOSUB | REG_EXTENDED))
				{
					//Failed to compile expression, send one message back to user
					char base[]    = "Error: Failed to compile regular expression ";
					char* errorMsg = (char*)malloc(strlen(base) + regLen + 1);
					strcpy(errorMsg, base);
					strncpy(errorMsg + strlen(base), regExp, regLen);
					errorMsg[strlen(base) + regLen] = 0;
					sendTAMessage(errorMsg);
					break;
				}
				free(regExp);

				recvRegex[i].inUse = 1;                 //activates regex once everything is in order
				if (i >= recvRegexCount)
					recvRegexCount++;
			}
			break;
		}
		case 2:         //sets use of given regex to 0
		{
			// it is a (almost absolute) guarantee that the time between the regex being set to 0 and
			// being set to 1 again will be long enough for the reader to have finished with the entry
			int handle;
			memcpy(&handle, mess.payload + 4, sizeof(int));
			for (int i = 0; i < recvRegexCount && recvRegex[i].inUse != 0; i++)
			{
				if (recvRegex[i].handle == handle)
					recvRegex[i].inUse = 0;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case 309:
		showManaBar = 1;
		break;
	case 310:
		showManaBar = 0;
		break;
	default:
		break;
	};
};

int ReadFromPipe()
{
	BOOL fSuccess = false;
	do
	{
		// Read from the pipe.
		CIpcMessage inBuf;
		DWORD lpBytesRead;
		inBuf.messageType = 0;

		fSuccess = ReadFile(
		        hPipe,    // pipe handle
		        &inBuf,    // buffer to receive reply
		        sizeof(CIpcMessage),      // size of buffer
		        &lpBytesRead,  // number of bytes read
		        NULL);    // not overlapped

		int err = GetLastError();

		if (!fSuccess && err == ERROR_BROKEN_PIPE)
			return 0;
		if (!fSuccess && err == ERROR_PIPE_NOT_CONNECTED)
			return 0;

		if (!fSuccess && err != ERROR_MORE_DATA)
		{
			char buf[128];
			sprintf(buf, "success=%d read=%d err=%d type=%d", fSuccess, lpBytesRead, err, inBuf.messageType);
			MessageBox(NULL, buf, "DEBUG", 0);
		}

		ParseIPCMessage(inBuf);

		// err = 234, 231, 231, 231, 2, 2, 2
	}
	while (!fSuccess);    // repeat loop if ERROR_MORE_DATA
	return 1;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam)
{
	InitialiseIPC();
	for (;; )
	{
		if (!ReadFromPipe())
			// communcation closed - reestablish communication pipe
			InitialiseIPC();
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
