#include "stdafx.h"
#include "TibiaHooks.h"

#include "tibiaautoinject2.h"
#include "IPCPipeBack.h"
#include "SystemHooks.h"
#include "protocol.h"

#include <ModuleUtil.h>
#include <MemReader.h>
#include <MemUtil.h>

#include <Psapi.h>

int myInterceptEncrypt(int v1, int v2);
int myInterceptDecrypt(int v1, int v2);
int myInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);
int myShouldParseRecv();

int privChanBufferPtr = 0;
int prevRecvStreamPos = 0;
int prevRecvStreamLen = 0;
int prevNextRet = 0;
char prevRecvStream[32768];

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


int GetProcessBaseAddr()
{
	static HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());
	if (dwHandle == NULL)
	{
		CloseHandle(dwHandle);
		AfxMessageBox("Unable to read Tibia memory!");
	}


	int ret = 0;
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
				ret = (int)moduleInfo.lpBaseOfDll;
				break;
			}
			if (i == 0) // catches first module in case Tibia.exe does not exist
			{
				MODULEINFO moduleInfo;
				GetModuleInformation(dwHandle, modules[i], &moduleInfo, sizeof(moduleInfo));
				isNotFromNormalScan = 1;
				ret = (int)moduleInfo.lpBaseOfDll;
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

void InitialiseTibiaHooks()
{
	DWORD procId = GetCurrentProcessId();
	HANDLE dwHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
	CMemReader& memReader = CMemReader::getMemReader();

//	hookDrawRect(dwHandle, baseAdjust(memReader.m_memAddressCallDrawBlackRect - 6), (unsigned int)INmyDrawBlackRect); // first layer black bar
//	hookDrawRect(dwHandle, baseAdjust(memReader.m_memAddressCallDrawRect - 6), (unsigned int)INmyDrawRect); // second layer colored bar

	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPrintText03 + 1), (unsigned int)INmyPrintText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPrintText04 + 1), (unsigned int)INmyPrintText);

	// lookup: find string In(FontNumber,1 [6th match is in the middle of the function]

	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText01 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText02 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText03 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText04 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText05 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText06 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText07 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText08 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText09 + 1), (unsigned int)INmyPlayerNameText);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallPlayerNameText10 + 1), (unsigned int)INmyPlayerNameText);

	// lookup: TALK_INFO_MESSAGE; this is inside of the function

	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMiddleScreen01 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMiddleScreen02 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMiddleScreen03 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMiddleScreen04 + 1), (unsigned int)INmyInterceptInfoMiddleScreen);

	// lookup: TargetBuffer!=NULL; first call is in the middle of the infomessage function
	//         in fact you need to increcept all calls

	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox01 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox02 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox03 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox04 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox05 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox06 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox07 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox08 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox09 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox10 + 1), (unsigned int)INmyInterceptInfoMessageBox);//BADBAD
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox11 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox12 + 1), (unsigned int)INmyInterceptInfoMessageBox);
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallInfoMessageBox13 + 1), (unsigned int)INmyInterceptInfoMessageBox);

	// lookup: manually match around previous address by assembly similarity
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallEncrypt01 + 1), (unsigned int)INmyInterceptEncrypt);

	// lookup: function below encrypt is the decrypt function
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallDecrypt01 + 1), (unsigned int)INmyInterceptDecrypt);


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
	trapFun(dwHandle, baseAdjust(memReader.m_memAddressCallShouldParseRecv01 + 1), (unsigned int)myShouldParseRecv);

	//This allows TA ta handle any errors before they are sent to Tibia
	//trapFun(dwHandle,baseAdjust(0x460EB3+1),(unsigned int)myShouldParseRecv); // 9.43 // need to find stuff for this


	CloseHandle(dwHandle);
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
		push[ebp] // ebp is needed only to know which creatureID is being drawn
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
		push[ebp] // ebp is needed only to know which creatureID is being drawn
		call myDrawRect
		leave
		ret 0x24
	}
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
		OUTmyDrawRect(ecx, nSurface, nX, nY - 5, nWeight, nHeight, nRed, nGreen, nBlue, drawRectPtr); //draw hp bar 5 pixels higher

		if (!(!nRed && !nGreen && !nBlue)) //if it is not black bar being drawn
		{
			float myRed = 0x20;
			float myBlue = 0xC0;
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

			if ((myBlue - luminosity) > 0) //adjust blue color to light
				myBlue = myBlue - (float)luminosity / (float)1.5;
			else
				myBlue = 0;

			myRed = myRed - (float)luminosity / (float)4; //adjust red color to light
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

__declspec(naked) void INmyPrintText() //(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<ecx>, int v9)
{
	__asm {
		push ebp
		mov ebp, esp
		push[ebp + 0x20]
		push[ebp + 0x1C]
		push[ebp + 0x18]
		push[ebp + 0x14]
		push[ebp + 0x10]
		push[ebp + 0x0C]
		push[ebp + 0x08]
		push edx
		push ecx
		call myPrintText
		leave
		ret
	}
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
		if (showManaBar) { // print name a bit higher for extra space to input the second bar
			nY = nY - 5;
		}
		// Runt printText for HUD only once at the time the character name is printed
		vector<HUD>::size_type i;
		for (i = 0; i != vecHUD.size(); i++)
		{
			if (vecHUD[i].pos.x > 2 && vecHUD[i].pos.y && !(vecHUD[i].message.empty()) && vecHUD[i].message[0] != '\0') {
				OUTmyPrintText(1, vecHUD[i].pos.x, vecHUD[i].pos.y, nFont, vecHUD[i].redColor, vecHUD[i].greenColor, vecHUD[i].blueColor, const_cast<char*>(vecHUD[i].message.c_str()), 0);
			}

		}
	}

	int ret = OUTmyPrintText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
	return ret;
}

//(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8<ecx>, int v9)
__declspec(noinline) int OUTmyPrintText(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9)
{
	int retvar;
	typedef void(*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, char* v8, int v9);

	Proto_fun fun = (Proto_fun)baseAdjust(CMemReader::getMemReader().m_memAddressFunTibiaPrintText);

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


__declspec(naked) void INmyPlayerNameText() //()
{
	__asm {
		push ebp
		mov ebp, esp
		push edi
		push ebx
		push[ebp + 0x38]
		push[ebp + 0x34]
		push[ebp + 0x30]
		push[ebp + 0x2C]
		push[ebp + 0x28]
		push[ebp + 0x24]
		push[ebp + 0x20]
		push[ebp + 0x1C]
		push[ebp + 0x18]
		push[ebp + 0x14]
		push[ebp + 0x10]
		push[ebp + 0x0C]
		push[ebp + 0x08]
		push edx
		push ecx
		call myPlayerNameText
		leave
		ret
	}
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

//(int align<ecx>, char *str<edx>, int visible, int x, int y, int fontNumber, int colR, int colG, int colB, int showFormatting, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, int v16<ebx>, int v17<edi>)
__declspec(noinline) int OUTmyPlayerNameText(int alignvar, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char *strvar, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17)
{
	int retvar;
	typedef int(*Proto_fun)(int align, int fontNumber, int visible, int x, int y, int showFormatting, int colR, int colG, int colB, char *str, int charCut, int cropLeft, int cropTop, int cropWidth, int cropHeight, char* v16, int v17);

	Proto_fun fun = (Proto_fun)baseAdjust(CMemReader::getMemReader().m_memAddressFunTibiaPlayerNameText);

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

// void(int type<ecx>, char* text<edx>)
__declspec(naked) void INmyInterceptInfoMiddleScreen()
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

__declspec(noinline) void OUTmyInterceptInfoMiddleScreen(int typevar, char* s)
{
	typedef void(*Proto_fun)(int typevar, char *s);

	Proto_fun fun = (Proto_fun)baseAdjust(CMemReader::getMemReader().m_memAddressFunTibiaInfoMiddleScreen);

	__asm {
		mov edx, s //[ebp + 0x0C]
		mov ecx, typevar //[ebp + 0x08]
		call fun
	}
}

__declspec(naked) void INmyInterceptInfoMessageBox() //<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
{
	__asm {
		push ebp
		mov ebp, esp
		push[ebp + 0x20]
		push[ebp + 0x1C]//lvl
		push[ebp + 0x18]//type2
		push[ebp + 0x14]//unk0x76d43ef5,5f6ec11c,754f5e89
		push[ebp + 0x10]//name
		push[ebp + 0x0C]//type
		push[ebp + 8]//text
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
	char *s = (char *)v3;

	int type = v4;
	int chantype = v9;
	if (debugFile)
		WriteOutDebug("XXX s1=%s s2=%s\r\n", s, nick);
	//char* happy2="dddddddddddddddddddd";
	//memcpy(*(char**)(*(char**)v1),happy2,strlen(happy2));
	// note: at least 0x14 bytes are passed on stack; at most 0x2c bytes are passed

	if (type == 1)
		privChanBufferPtr = v1;


	int nickLen = nick ? strlen(nick) : 0;
	int msgLen = strlen(s);
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

//<eax>(int v1, int v2, int v3<ecx>, int v4, int v5, int v6, int v7, int v8<edx>, int v9)
__declspec(noinline) int OUTmyInterceptInfoMessageBox(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9)
{
	int retval;
	typedef int(*Proto_fun)(int v1, int v2, int v3, int v4, int v5, int v6, int v7, int v8, int v9);

	Proto_fun fun = (Proto_fun)baseAdjust(CMemReader::getMemReader().m_memAddressFunTibiaInfoMessageBox);

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
		int hiB = ((char *)v1)[1];
		if (lowB < 0)
			lowB += 256;
		if (hiB < 0)
			hiB += 256;
		encryptLen = lowB + hiB * 256 + 2;
		encryptPos = 0;
	}
	else
	{
		encryptPos += 8;
		encryptPrevPtr += 8;
	}

	memcpy(encryptBeforeBuf + encryptPos, (void *)v1, 8);
	int ret = OUTmyInterceptEncrypt(v1, v2);
	memcpy(encryptAfterBuf + encryptPos, (void *)v1, 8);
	return ret;
}

__declspec(noinline) int OUTmyInterceptEncrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	int retvar;
	typedef void(*Proto_fun)(int v1, int v2);

	Proto_fun fun = (Proto_fun)baseAdjust(CMemReader::getMemReader().m_memAddressFunTibiaEncrypt);

	__asm {
		mov edx, v2 // move v2 to edx
		mov ecx, v1 // move v1 to ecx
		call fun // jump to the real Tibia function
		mov retvar, eax //store value returned from real function in ret
	}
	return retvar;
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

__declspec(noinline) int OUTmyInterceptDecrypt(int v1, int v2) //<eax>(int v1<ecx>, int v2<edx>)
{
	int retvar; //create [ebp - 4] variable
	typedef void(*Proto_fun)();

	Proto_fun fun = (Proto_fun)baseAdjust(CMemReader::getMemReader().m_memAddressFunTibiaDecrypt);

	__asm {
		mov edx, v2 // move v2 to edx
		mov ecx, v1 // move v1 to ecx
		call fun // jump to the real Tibia function
		mov retvar, eax //store value returned from real function in ret
	}
	return retvar;
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

//wiz
//This captured function is what Tibia uses to decrypt the packet
// and determine which action types it contains in order to parse it properly
// The function returns the action type and Tibia then parses the stream.
// If the stream has not yet been used up, it will continually run this function to
// determine the type of and parse the next action.
// We can interfere with this process, but this requires knowing packet structures
// and error handling for incomplete packets which takes much more work than payoff.
int myShouldParseRecv()
{
	CMemReader& reader = CMemReader::getMemReader();

	typedef int(*Proto_fun)();

	Proto_fun fun = (Proto_fun)baseAdjust(reader.m_memAddressFunTibiaShouldParseRecv);
	//This function looks ahead, it returns the next packet type. It will return -1 if no next packet.
	int ret = fun();

	//look for this address near above location, it will be off by 8

	packStream* recvStream = (packStream*)baseAdjust(reader.m_memAddressArrayPtrRecvStream);
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
	prevNextRet = ret;

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
