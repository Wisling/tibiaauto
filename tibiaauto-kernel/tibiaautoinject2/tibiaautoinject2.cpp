// tibiaautoinject2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "tibiaautoinject2.h"
#include "stdio.h"
#include "time.h"
#include "MyMenu.h"
#include "resource.h"
#include <ModuleUtil.h>
#include <MemReader.h>
#include <MemUtil.h>
#include "IPCPipeBack.h"
#include "SystemHooks.h"
#include "TibiaHooks.h"
#include "protocol.h"

CIPCPipeBack ipcPipeBack;


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

actionRegexData recvRegex[RECV_REGEX_MAX];
int recvRegexCount = 0;

int partnerProcessId = 0;
int ignoreLookEnd    = 0;

int autoAimActive               = 0;
int autoAimOnlyCreatures        = 0;
int autoAimAimPlayersFromBattle = 0;

int revealCNameActive = 0;

HANDLE hPipe = INVALID_HANDLE_VALUE;

HHOOK hook;

SOCKET tibiaSocket = NULL;
FILE *debugFile    = NULL;

int COMPLEX  = 0;
int SENTONLY = 0;

time_t debugFileStart;

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

void InitialiseCommunication();
void InitialiseDebugFile();
void adler(unsigned char *data, size_t len, unsigned char* outBuf);

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

void sendTAMessage(char* message)
{
	if (privChanBufferPtr)
	{
		WriteOutDebug("sendTAMessage %X %s", privChanBufferPtr, message);
		OUTmyInterceptInfoMessageBox(privChanBufferPtr, 0, (int)message, 4, (int)"Tibia Auto", 0, 0, 0, 0);
	}
}

int aaa = 0;

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

void parseRecvActionData(int handle, char* data, int len)
{
#ifdef _DEBUG
	if (len == 0)
		AfxMessageBox("parseRecvActionData: length is 0.");

#endif // ifdef _DEBUG
	CIpcMessage mess;
	mess.messageType = 1010;

	parcelRecvActionData* p = ((parcelRecvActionData*)mess.payload);
	int maxlen = sizeof(p->actionData);
	int splits = len / maxlen;
	int messageCounts = splits + 1;

	p->handle = handle;
	while (splits >= 0)
	{
		p->totalCount = messageCounts;
		p->countLeft = splits;
		p->len = min(len, maxlen);
		memcpy(p->actionData, data, p->len);
		char buf[1111];
		sprintf(buf, "%d, %s", p->len, p->actionData);
		//AfxMessageBox(buf);
		ipcPipeBack.send(mess);
		splits--;
		data += p->len;
		len -= p->len;
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

}

int parseMessageForTibiaAction(char *buf, int len)
{
	//char buf2[1111]="hi ";
	//for (int i=0;i<min(len,1110);i++){ sprintf(buf2,"%s %x",buf2,(char)buf[i]); }
	//AfxMessageBox(buf2);
	static int gatherStats = CModuleUtil::getTASetting("GatherBotStats");
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
		int nameLen = nameV1 + 256 * nameV2;

		unsigned char sayV1 = buf[6 + nameLen];
		unsigned char sayV2 = buf[7 + nameLen];
		int sayLen = sayV1 + 256 * sayV2;

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
		InitialiseCommunication();
		InitialiseSystemHooks();
		InitialiseTibiaHooks();
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