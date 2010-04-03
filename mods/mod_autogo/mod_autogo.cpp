
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


// mod_autogo.cpp : Defines the initialization routines
 for the DLL.
//

#include "stdafx.h"
#include "mod_autogo.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"
#include "MemConstData.h"

#include "MemReaderProxy.h"
#include "PackSenderProxy.h"
#include "TibiaItemProxy.h"
#include "TibiaMapProxy.h"
#include "ModuleUtil.h"
#include <MMSystem.h>
#include <Tlhelp32.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <math.h>

using namespace std;


#include "IPCBackPipeProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define MASTER_DEBUG
#define EWX_FORCEIFHUNG      0x00000010


/////////////////////////////////////////////////////////////////////////////
// CMod_autogoApp

BEGIN_MESSAGE_MAP(CMod_autogoApp, CWinApp)
	//{{AFX_MSG_MAP(CMod_autogoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////

int toolThreadShouldStop=0;
HANDLE toolThreadHandle;
char suspendedModules[20][64];
int suspendedCount = 0;
char* lastFilename="";
HWND tibiaHWND = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
	CMemReaderProxy reader;
	
	DWORD dwThreadId, dwProcessId;
	if (!hWnd)
		return TRUE;		// Not a window
	dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
	if (dwProcessId == reader.getProcessId() && GetParent(hWnd) == NULL)
		tibiaHWND = hWnd;
	return TRUE;
}

void masterDebug(const char* buf1,const char* buf2="",const char* buf3="",const char* buf4="",const char* buf5="",const char* buf6="",const char* buf7=""){

#ifdef MASTER_DEBUG
		char dateStr [15];
		char timeStr [15];
		_strdate( dateStr);
		_strtime( timeStr );
		char installPath[1024];
		CModuleUtil::getInstallPath(installPath);
		char pathBuf[2048];
		sprintf(pathBuf,"%s\\tibiaauto-debug-autogo.txt",installPath);
		FILE *f=fopen(pathBuf,"a+");
		if (f)
		{
			fprintf(f,"%s\t%s\tAutogo\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",dateStr,timeStr,buf1,buf2,buf3,buf4,buf5,buf6,buf7);
			fclose(f);
		}
#endif
}

std::string intstr(int value){
	stringstream ss;// from ModuleUtil.h import
	ss << value;
	return ss.str();
}

void actionTerminate(){
	masterDebug("ActionTerminate");
	CMemReaderProxy reader;
	HANDLE hTibiaProc;

	hTibiaProc = OpenProcess(PROCESS_TERMINATE,true,reader.getProcessId());

	TerminateProcess(hTibiaProc,0);

	CloseHandle(hTibiaProc);
}

int actionShutdownSystem(){
	masterDebug("actionShutdownSystem");
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
	//T4: Privilages are needed for XP & 2000
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken)){
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
		
        tkp.PrivilegeCount = 1; 
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

        if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0)){
			ExitWindowsEx(EWX_SHUTDOWN|EWX_POWEROFF|EWX_FORCEIFHUNG,0);
			return 1;
		}
	}
	return 0;
}

void actionSuspend(CString module) {
	masterDebug("actionSuspend");
	masterDebug("actionSuspend","Time to stop module");
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	if (hSnap) {
		MODULEENTRY32 lpModule;
		lpModule.dwSize = sizeof(MODULEENTRY32);

		Module32First(hSnap,&lpModule);
		do {
			if (lpModule.szModule == module){
				FARPROC isStarted;
				isStarted = GetProcAddress(lpModule.hModule,"isStarted");
				if (isStarted && isStarted()) {
					GetProcAddress(lpModule.hModule,"stop")();
				}
			}
		} while (Module32Next(hSnap,&lpModule));
		CloseHandle(hSnap);
	}
}

void actionStart(CString module) {
	masterDebug("actionStart");
	masterDebug("actionStart","Time to start module", module);
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	if (hSnap) {
		MODULEENTRY32 lpModule;
		lpModule.dwSize = sizeof(MODULEENTRY32);

		Module32First(hSnap,&lpModule);
		do {
			if (lpModule.szModule == module){
				FARPROC isStarted;
				isStarted = GetProcAddress(lpModule.hModule,"isStarted");
				if (isStarted && !isStarted()) {
					GetProcAddress(lpModule.hModule,"start")();
				}
			}
		} while (Module32Next(hSnap,&lpModule));
		CloseHandle(hSnap);
	}
}

int OnList(char whiteList[100][32],char name[]){
	masterDebug("OnList");
	int i=0;
	while (IsCharAlphaNumeric(whiteList[i][0])){
		if (!strcmpi(whiteList[i],name)){
			return 1;
		}
		i++;
	}

	return 0;
}

struct tibiaMessage * triggerMessage(){
	masterDebug("triggerMessage");
	CMemReaderProxy reader;
	CIPCBackPipeProxy backPipe;
	struct ipcMessage mess;

	if (backPipe.readFromPipe(&mess,1003)){
		int infoType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];
		
		memset(nickBuf,0,16384);
		memset(msgBuf,0,16384);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&nickLen,mess.payload+4,sizeof(int));
		memcpy(&msgLen,mess.payload+8,sizeof(int));
		memcpy(nickBuf,mess.payload+12,nickLen);
		memcpy(msgBuf,mess.payload+12+nickLen,msgLen);
		
		CTibiaCharacter *self = reader.readSelfCharacter();

		if (strcmpi(nickBuf,self->name)!=0 && strcmpi(nickBuf,"Tibia Auto") != 0) {
			delete self;
			struct tibiaMessage *newMsg = new tibiaMessage();
			newMsg->type=infoType;
			strcpy(newMsg->msg,msgBuf);
			masterDebug("triggerMessage Exit");
			return newMsg;
		}
		delete self;
	}
	masterDebug("triggerMessage Exit");
	return NULL;
}

char *alarmStatus(int alarmId){	
	masterDebug("alarmStatus");
	if (alarmId&TRIGGER_BATTLELIST_LIST) return "BattleList list alarm";	
	if (alarmId&TRIGGER_BATTLELIST_GM) return "BattleList GM alarm";	
	if (alarmId&TRIGGER_BATTLELIST_PLAYER) return "BattleList player alarm";	
	if (alarmId&TRIGGER_BATTLELIST_MONSTER) return "BattleList monster alarm";	
	if (alarmId&TRIGGER_SIGN)		return "A sign appeared";
	if (alarmId&TRIGGER_SKULL)		return "A skulled player appeared";
	if (alarmId&TRIGGER_VIP)		return "A vip with crossbones is logged in";
	if (alarmId&TRIGGER_MESSAGE)	return "New message";
	if (alarmId&TRIGGER_MOVE)		return "You're moving";
	if (alarmId&TRIGGER_HPLOSS)		return "You have lost HP";
	if (alarmId&TRIGGER_HPBELOW)	return "Your HP is below a certain value";
	if (alarmId&TRIGGER_HPABOVE)	return "Your HP is above a certain value";
	if (alarmId&TRIGGER_MANABELOW)	return "Your mana is below a certain value";
	if (alarmId&TRIGGER_MANAABOVE)	return "Your mana is above a certain value";
	if (alarmId&TRIGGER_SOULPOINT_BELOW)	return "Too few soul points";
	if (alarmId&TRIGGER_SOULPOINT_ABOVE)	return "Too many soul points";
	if (alarmId&TRIGGER_STAMINA_BELOW)	return "Stamina is below a certain value";
	if (alarmId&TRIGGER_STAMINA_ABOVE)	return "Stamina is above a certain value";
	if (alarmId&TRIGGER_BLANK)		return "Too few blank runes";
	if (alarmId&TRIGGER_CAPACITY)	return "Capacity is too small";
	if (alarmId&TRIGGER_OUTOF_SPACE)		return "You've run out of space";
	if (alarmId&TRIGGER_OUTOF_FOOD)		return "You've run out of food";
	if (alarmId&TRIGGER_OUTOF_CUSTOM)		return "You've run out of an item";
	if (alarmId&TRIGGER_RUNAWAY_REACHED)		return "You reached the run away point";
	return "";
}

void WriteBMPFile(HBITMAP bitmap, CString filename, HDC hDC) {
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD cClrBits; 
	HANDLE hf; // file handle 
	BITMAPFILEHEADER hdr; // bitmap file-header 
	PBITMAPINFOHEADER pbih; // bitmap info-header 
	LPBYTE lpBits; // memory pointer 
	DWORD dwTotal; // total count of bytes 
	DWORD cb; // incremental count of bytes 
	BYTE *hp; // byte pointer 
	DWORD dwTmp; 
	
	// create the bitmapinfo header information
	
	if (!GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp)) {
		AfxMessageBox("Could not retrieve bitmap info");
		return;
	}
	
	// Convert the color format to a count of bits. 
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 
	// Allocate memory for the BITMAPINFO structure.
	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< cClrBits)); 
	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 
	
	// Initialize the fields in the BITMAPINFO structure. 
	
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 
	
	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB; 
	
	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 * pbmi->bmiHeader.biHeight * cClrBits; 
	// Set biClrImportant to 0, indicating that all of the 
	// device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 
	
	// now open file and save the data
	pbih = (PBITMAPINFOHEADER) pbmi; 
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
	
	if (!lpBits) {
		AfxMessageBox("writeBMP::Could not allocate memory");
		return;
	}
	
	// Retrieve the color table (RGBQUAD array) and the bits 
	if (!GetDIBits(hDC, HBITMAP(bitmap), 0, (WORD) pbih->biHeight, lpBits, pbmi, 
		DIB_RGB_COLORS)) {
		AfxMessageBox("writeBMP::GetDIB error");
		return;
	}
	
	// Create the .BMP file. 
	hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 
	if (hf == INVALID_HANDLE_VALUE){
		AfxMessageBox("Could not create file for writing");
		return;
	}
	hdr.bfType = 0x4d42; // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof(RGBQUAD) + pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 
	
	// Compute the offset to the array of color indices. 
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + pbih->biClrUsed 
		* sizeof (RGBQUAD); 
	
	// Copy the BITMAPFILEHEADER into the .BMP file. 
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
		(LPDWORD) &dwTmp, NULL)) {
		AfxMessageBox("Could not write in to file");
		return;
	}
	
	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
		+ pbih->biClrUsed * sizeof (RGBQUAD), 
		(LPDWORD) &dwTmp, ( NULL))){
		AfxMessageBox("Could not write in to file");
		return;
	}	
	
	// Copy the array of color indices into the .BMP file. 
	dwTotal = cb = pbih->biSizeImage; 
	hp = lpBits; 
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) {
		AfxMessageBox("Could not write in to file");
		return;
	}
	
	// Close the .BMP file. 
	if (!CloseHandle(hf)){
		AfxMessageBox("Could not close file");
		return;
	}

	// Free memory. 
	GlobalFree((HGLOBAL)lpBits);
}

DWORD WINAPI takeScreenshot(LPVOID lpParam) {		
	CMemReaderProxy reader;
	if (!tibiaHWND)
		EnumWindows(EnumWindowsProc, NULL);
	RECT rect;
	bool captured = false;
	bool minimized = IsIconic(tibiaHWND);

	char path[1024];
	CModuleUtil::getInstallPath(path);
	time_t lTime;
	time(&lTime);
	char timeBuf[64];
	strftime(timeBuf, 64, " %a %d %b-%H%M(%S)", gmtime(&lTime));
	CString filePath;
	filePath.Format("%s\\screenshots\\Screenshot%s.bmp", path, timeBuf);
	
	while (!captured) {
		Sleep (100);
		if (reader.getConnectionState()!=8)
			continue;
		if(IsIconic(tibiaHWND)) {
			ShowWindow(tibiaHWND, SW_RESTORE);
			continue;
		}
		if(tibiaHWND != GetForegroundWindow()) {
			SetForegroundWindow(tibiaHWND);
			continue;
		}
		Sleep (500);
		GetClientRect(tibiaHWND, &rect);

		int nWidth = rect.right - rect.left;
		int mHeight = rect.bottom - rect.top;
		HDC hDDC = GetDC(tibiaHWND);
		HDC hCDC = CreateCompatibleDC(hDDC);
		HBITMAP hBitmap =CreateCompatibleBitmap(hDDC,nWidth,mHeight);
		SelectObject(hCDC,hBitmap); 
		BitBlt(hCDC,0,0,nWidth,mHeight,hDDC,0,0,SRCCOPY); 
		WriteBMPFile(hBitmap, filePath, hCDC);
		captured = true;
	}
	if (minimized)
		ShowWindow(tibiaHWND, SW_MINIMIZE);
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {		
	masterDebug("toolThreadProc");
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CMemConstData memConstData = reader.getMemConstData();
	CConfigData *config = (CConfigData *)lpParam;
	list<Alarm>::iterator alarmItr;
	CTibiaCharacter *self = reader.readSelfCharacter();
	char path[1024];
	CModuleUtil::getInstallPath(path);
	strcat(path, "\\mods\\sound\\");

	int lastX = self->x;
	int lastY = self->y;
	int lastZ = self->z;
	int lastMoved = 0;
	int lastHp = self->hp;
	int lastMana = self->mana;
	int recoveryAlarmHp=0;
	int recoveryAlarmMana=0;
	int goPriority = 0;
	
	delete self;	

	PlaySound(0, 0, 0);
		
	while (!toolThreadShouldStop) {			
		Sleep(100);
		//if we're not looking for messages consume them anyway to avoid buffer overflow/expansion
		if (!config->triggerMessage)
			triggerMessage();
		alarmItr = config->alarmList.begin();
		CTibiaCharacter *self = reader.readSelfCharacter();
		if (recoveryAlarmMana&&self->mana==self->maxMana) recoveryAlarmMana=0;
		if (recoveryAlarmHp&&self->hp==self->maxMana) recoveryAlarmHp=0;

		//insert my alarm check and action code
		while (alarmItr != config->alarmList.end()) {
			if (alarmItr->checkAlarm(config->whiteList, config->options)) {
				// Flash Window ***************
				if (!alarmItr->flashed) {
					if (!tibiaHWND)
						EnumWindows(EnumWindowsProc, NULL);
					FlashWindow(tibiaHWND, true);
					alarmItr->flashed = true;
				}// ***************************

				// Log Event ******************
				if (alarmItr->doLogEvents() && !alarmItr->eventLogged) {
					time_t rawtime;
					struct tm * timeinfo;					
					timeinfo = localtime(&rawtime );
					CString dateStr = asctime(timeinfo);
					char filename[64];
					strftime(filename, 64, "%d%b%y.txt", gmtime(&rawtime));
					CString pathBuf;
					pathBuf.Format("%s\\logs\\%s", path, filename);
					FILE *f = fopen(pathBuf, "a+");
					if (f) {
						fprintf(f, "%s  %s\n\t%s", "***  Active  -->", alarmItr->getDescriptor(), dateStr);
						fclose(f);
					}
					alarmItr->eventLogged = true;
				}// ****************************

				// Play sound ******************
				if (alarmItr->doAlarm().GetLength()) 
					PlaySound(path + alarmItr->doAlarm(), NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
				// *****************************

				// Attack ******************
				if (alarmItr->doAttack()) { 
					HANDLE hSnap;
					hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
					if (hSnap) {
						MODULEENTRY32 lpModule;
						lpModule.dwSize = sizeof(MODULEENTRY32);

						Module32First(hSnap,&lpModule);
						do {
							if (lpModule.szModule == "mod_cavebot.dll") {
								FARPROC isStarted;
								isStarted = GetProcAddress(lpModule.hModule, "isStarted");
								if (isStarted && !isStarted()) {
									GetProcAddress(lpModule.hModule,"start")();
									alarmItr->cavebotForced = true;
								}
							}
						} while (Module32Next(hSnap,&lpModule));
						CloseHandle(hSnap);
					}
				}
				else {
					reader.setGlobalVariable("cavebot_fullsleep","true");
					fullSleep++;
				}
				// *****************************
				
				// Maximize Window *************
				if (alarmItr->doMaximizeClient() && !alarmItr->maximized) {
					if (!tibiaHWND)
						EnumWindows(EnumWindowsProc, NULL);
					ShowWindow(tibiaHWND, SW_MAXIMIZE);
					alarmItr->maximized = true;
				}// ****************************
				
				// Suspend Modules  ************
				if (alarmItr->doStopModules().size() && !alarmItr->modulesSuspended) {
					list<CString> temp = alarmItr->doStopModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionSuspend(*modulesItr);
						modulesItr++;
						alarmItr->modulesSuspended = true;
					}
				}// ****************************
			
				// Start Modules ***************
				if (alarmItr->doStartModules().size() && !alarmItr->modulesStarted) {
					list<CString> temp = alarmItr->doStartModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionStart(*modulesItr);
						modulesItr++;
						alarmItr->modulesStarted = true;
					}

				}// ****************************

				// Cast spell ******************
				if (alarmItr->doCastSpell().GetLength() && !alarmItr->spellCast) {
					if (self->mana >= alarmItr->getManaCost()) {
						sender.sayWhisper(alarmItr->doCastSpell());
						alarmItr->spellCast = true;
					}
					delete self;
				}
				// *****************************

				// Take Screenshot ******************
				if (alarmItr->doTakeScreenshot() > -1) {
					DWORD threadId;
					switch (alarmItr->doTakeScreenshot()) {
					case 0:
						if (!alarmItr->screenshotsTaken) {
							::CreateThread(NULL, 0, takeScreenshot,NULL, 0, &threadId);				
							alarmItr->screenshotsTaken++;
							alarmItr->timeLastSS = time(NULL);
						}
						break;
					case 1:
						if (alarmItr->screenshotsTaken < 3 && time(NULL) - alarmItr->timeLastSS >= 1) {
							::CreateThread(NULL, 0, takeScreenshot,NULL, 0, &threadId);				
							alarmItr->screenshotsTaken++;
							alarmItr->timeLastSS = time(NULL);
						}
						break;
					case 2:
						if (time(NULL) - alarmItr->timeLastSS >= 5) {
							::CreateThread(NULL, 0, takeScreenshot,NULL, 0, &threadId);				
							alarmItr->screenshotsTaken++;
							alarmItr->timeLastSS = time(NULL);
						}
						break;
					case 3:
						if (time(NULL) - alarmItr->timeLastSS >= 10) {
							::CreateThread(NULL, 0, takeScreenshot,NULL, 0, &threadId);				
							alarmItr->screenshotsTaken++;
							alarmItr->timeLastSS = time(NULL);
						}
						break;
					}				
				}
				// *****************************

				// Goto Start ******************
				if (alarmItr->doGoToStart()  && goPriority <= 1) {
					goPriority = 1;
				}// ****************************

				// Goto Runaway ****************
				if (alarmItr->doGoToRunaway() && goPriority <= 2) {
					goPriority = 2;
				}
				// *****************************

				// Goto Depot ******************
				if (alarmItr->doGoToDepot()  && goPriority <= 3) {
					goPriority = 3;
				}// ****************************

				// Logout **********************
				if (alarmItr->doLogout()) {
					if (!(reader.getSelfEventFlags() & (int)pow(2, LOGOUTBLOCK)) && !(reader.getSelfEventFlags() & (int)pow(2, PZBLOCK)) && reader.getConnectionState() != 8 ) {
						sender.logout();
					}
				}
				// *****************************

				// Kill Client *****************
				if (alarmItr->doKillClient()) {
					actionTerminate();
				}
				// *****************************

				// Shutdown Computer ***********
				if (alarmItr->doShutdownComputer()) {
					actionShutdownSystem();
				}
				// *****************************

			}
			else if (alarmItr->doTakeScreenshot() == 1 && alarmItr->screenshotsTaken < 3 && time(NULL) - alarmItr->timeLastSS >= 1) {
				// the alarm may have stopped BEFORE we took our 3 screenshots, let's contuinue
				DWORD threadId;
				::CreateThread(NULL, 0, takeScreenshot,NULL, 0, &threadId);				
				alarmItr->screenshotsTaken++;
				alarmItr->timeLastSS = time(NULL);
			}
			else { // Clean up Alarm flags here when alarm condition are no longer true , reset modules to previous state, return to start??
				if (alarmItr->cavebotForced)
					actionSuspend("mod_cavebot.dll");
				alarmItr->cavebotForced = false;
				alarmItr->flashed = false;
				alarmItr->maximized = false;
				PlaySound(NULL, NULL, SND_NOSTOP);
				alarmItr->spellCast = false;
				alarmItr->timeLastSS = time(NULL);
				alarmItr->screenshotsTaken = 0;
				if (alarmItr->modulesStarted && alarmItr->modulesStarted) { // Stop when alarm is over
					list<CString> temp = alarmItr->doStartModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionSuspend(*modulesItr);
						modulesItr++;
					}
					alarmItr->modulesStarted = false;
				}
				if (alarmItr->modulesSuspended && alarmItr->modulesSuspended) { // Start when alarm is over
					list<CString> temp = alarmItr->doStopModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionStart(*modulesItr);
						modulesItr++;
					}
					alarmItr->modulesSuspended = false;
				}
				if (alarmItr->eventLogged) {
					time_t rawtime;
					struct tm * timeinfo;					
					timeinfo = localtime(&rawtime );
					CString dateStr = asctime(timeinfo);
					char filename[64];
					strftime(filename, 64, "%d%b%y.txt", gmtime(&rawtime));
					CString pathBuf;
					pathBuf.Format("%s\\logs\\%s", path, filename);
					FILE *f = fopen(pathBuf, "a+");
					if (f) {
						fprintf(f, "%s  %s\n\t%s", "***  Inactive  -->", alarmItr->getDescriptor(), dateStr);
						fclose(f);
					}
					alarmItr->eventLogged = false;
				}
			}
			alarmItr++;
		}
		// Do not let seperate alarms fight for control!!
		switch (goPriority) {
			if (!reader.getGlobalVariable("cavebot_fullsleep")) {
				reader.setGlobalVariable("cavebot_halfsleep","true");
				halfSleep++;
			}
		case 1: {// Start position (By definition, the least safe place to be)
			int pathSize = 0;
			self = reader.readSelfCharacter();
			CMemConstData memConstData = reader.getMemConstData();
			int path[15];
			delete self;
			
			self = reader.readSelfCharacter();
			if (self->x != config->actX || self->y != config->actY || self->z != config->actZ) {						
				// proceed with path searching									
				CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->actX, config->actY, config->actZ, 0, path, 1);
				for (; pathSize < 15 && path[pathSize]; pathSize++);										
				if (pathSize)
					CModuleUtil::executeWalk(self->x, self->y, self->z, path);											
			}
			else if (config->actDirection) {
				if (config->actDirection == DIR_LEFT)
					sender.turnLeft();
				else if(config->actDirection == DIR_RIGHT)
					sender.turnRight();
				else if(config->actDirection == DIR_UP)
					sender.turnUp();
				else if(config->actDirection == DIR_DOWN)
					sender.turnDown();
			}
			delete self;
			   }
			break;
		case 2: {// Runaway Position (By definition, the relatively safe spot chosen by the user)
			int pathSize = 0;
			self = reader.readSelfCharacter();
			CMemConstData memConstData = reader.getMemConstData();
			CTibiaMapProxy tibiaMap;
			
			int path[15];
			delete self;
			self = reader.readSelfCharacter();
			
			if (self->x != config->runawayX || self->y != config->runawayY || self->z != config->runawayZ) {						
				// proceed with path searching									
				CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->runawayX, config->runawayY, config->runawayZ, 0, path, 1);
				for (; pathSize < 15 && path[pathSize]; pathSize++);										
				if (pathSize)
					CModuleUtil::executeWalk(self->x,self->y,self->z,path);
			}
			delete self;
				}
			break;
		case 3: {// Depot (Reasoned as, the safest position [because you are protected from attack])  
			int pathSize = 0;
			self = reader.readSelfCharacter();
			CMemConstData memConstData = reader.getMemConstData();
			int path[15];
			CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
			for (; pathSize < 15 && path[pathSize]; pathSize++);										
			if (pathSize)
				CModuleUtil::executeWalk(self->x, self->y, self->z, path);											
			delete self;
				}
			break;
		default: {
			if (config->maintainStart) {
				int pathSize = 0;
				self = reader.readSelfCharacter();
				CMemConstData memConstData = reader.getMemConstData();
				int path[15];
				delete self;
				
				self = reader.readSelfCharacter();
				if (self->x != config->actX || self->y != config->actY || self->z != config->actZ) {						
					// proceed with path searching									
					CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->actX, config->actY, config->actZ, 0, path, 1);
					for (; pathSize < 15 && path[pathSize]; pathSize++);										
					if (pathSize)
						CModuleUtil::executeWalk(self->x, self->y, self->z, path);											
				}
				else if (config->actDirection) {
					if (config->actDirection == DIR_LEFT)
						sender.turnLeft();
					else if(config->actDirection == DIR_RIGHT)
						sender.turnRight();
					else if(config->actDirection == DIR_UP)
						sender.turnUp();
					else if(config->actDirection == DIR_DOWN)
						sender.turnDown();
				}
				delete self;
			}
			   }
			break;
		}
		goPriority = 0;
	}

	// Clean-up alarm flags before disabling, reset modules to previous state.
	alarmItr = config->alarmList.begin();
	while (alarmItr != config->alarmList.end()) {
		alarmItr->cavebotForced = false;
		alarmItr->flashed = false;
		alarmItr->maximized = false;
		alarmItr->spellCast = false;
		alarmItr->timeLastSS = time(NULL);
		alarmItr->screenshotsTaken = 0;
		if (alarmItr->modulesStarted) { // Stop when alarm is over
			list<CString> temp = alarmItr->doStartModules();
			list<CString>::iterator modulesItr = temp.begin();
			while(modulesItr != temp.end()) {
				actionSuspend(*modulesItr);
				modulesItr++;
			}
			alarmItr->modulesStarted = false;
		}
				if (alarmItr->modulesSuspended && alarmItr->modulesSuspended) { // Start when alarm is over
			list<CString> temp = alarmItr->doStopModules();
			list<CString>::iterator modulesItr = temp.begin();
			while(modulesItr != temp.end()) {
				actionStart(*modulesItr);
				modulesItr++;
			}
			alarmItr->modulesSuspended = false;
		}
		alarmItr++;
	}
	// clear current status
	config->status.Empty();
	// stop the alarm;
	PlaySound(NULL, NULL, NULL);

	// clear cavebot half/full sleep vars
	reader.setGlobalVariable("cavebot_halfsleep","false");
	reader.setGlobalVariable("cavebot_fullsleep","false");

	toolThreadShouldStop=0;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMod_autogoApp construction

CMod_autogoApp::CMod_autogoApp()
{
	m_configDialog =NULL;
	m_started=0;
	m_configData = new CConfigData();
}

CMod_autogoApp::~CMod_autogoApp()
{
	if (m_configDialog)
	{
		delete m_configDialog;
	}
	delete m_configData;	
}

char * CMod_autogoApp::getName()
{
	return "Auto go/logout";
}


int CMod_autogoApp::isStarted()
{
	if (!m_started)
	{
		// if not started then regularly consume 1003 messages from the queue
		CIPCBackPipeProxy backPipe;
		struct ipcMessage mess;	

		backPipe.readFromPipe(&mess,1003);
	}
	return m_started;
}


void CMod_autogoApp::start()
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

void CMod_autogoApp::stop()
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

void CMod_autogoApp::showConfigDialog()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	

	if (!m_configDialog)
	{
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_autogoApp::configToControls()
{
	if (m_configDialog)
	{		
		
		m_configDialog->configToControls(m_configData);
	}
}


void CMod_autogoApp::controlsToConfig()
{
	if (m_configDialog)
	{
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_autogoApp::disableControls()
{
	if (m_configDialog)
	{
		m_configDialog->disableControls();
	}
}

void CMod_autogoApp::enableControls()
{
	if (m_configDialog)
	{
		m_configDialog->enableControls();
	}
}


char *CMod_autogoApp::getVersion()
{	
	return "3.1";
}


int CMod_autogoApp::validateConfig(int showAlerts)
{	
	return 1;
}

void CMod_autogoApp::resetConfig()
{
	m_configData = new CConfigData();
}

void CMod_autogoApp::loadConfigParam(char *paramName,char *paramValue)
{
	if (!strcmp(paramName,"act/x"))						m_configData->actX					= atoi(paramValue);
	if (!strcmp(paramName,"act/y"))						m_configData->actY					= atoi(paramValue);
	if (!strcmp(paramName,"act/z"))						m_configData->actZ					= atoi(paramValue);
	if (!strcmp(paramName,"act/direction"))				m_configData->actDirection			= atoi(paramValue);
	if (!strcmp(paramName,"runaway/x"))					m_configData->runawayX				= atoi(paramValue);
	if (!strcmp(paramName,"runaway/y"))					m_configData->runawayY				= atoi(paramValue);
	if (!strcmp(paramName,"runaway/z"))					m_configData->runawayZ				= atoi(paramValue);
	if (!strcmp(paramName,"triggerMessage"))			m_configData->triggerMessage		= atoi(paramValue);
	if (!strcmp(paramName,"whiteList/List")){
		if (currentPos>99)
			return;
		lstrcpyn(m_configData->whiteList[currentPos++],paramValue,32);
	}
	if (!strcmp(paramName,"whiteList/mkBlack"))			m_configData->mkBlack	= atoi(paramValue);

}

char *CMod_autogoApp::saveConfigParam(char *paramName)
{
	static char buf[1024];
	buf[0]=0;
	
	if (!strcmp(paramName,"act/x"))						sprintf(buf,"%d",m_configData->actX);
	if (!strcmp(paramName,"act/y"))						sprintf(buf,"%d",m_configData->actY);
	if (!strcmp(paramName,"act/z"))						sprintf(buf,"%d",m_configData->actZ);
	if (!strcmp(paramName,"act/direction"))				sprintf(buf,"%d",m_configData->actDirection);
	if (!strcmp(paramName,"runaway/x"))					sprintf(buf,"%d",m_configData->runawayX);
	if (!strcmp(paramName,"runaway/y"))					sprintf(buf,"%d",m_configData->runawayY);
	if (!strcmp(paramName,"runaway/z"))					sprintf(buf,"%d",m_configData->runawayZ);
	if (!strcmp(paramName,"triggerMessage"))			sprintf(buf,"%d",m_configData->triggerMessage);
	if (!strcmp(paramName,"whiteList/List")){		
		if (currentPos<100){
			if (IsCharAlphaNumeric(m_configData->whiteList[currentPos][0])){				
				lstrcpyn(buf,m_configData->whiteList[currentPos++],32);
			}
		}		
	}
	if (!strcmp(paramName,"whiteList/mkBlack"))						sprintf(buf,"%d",m_configData->mkBlack);
	return buf;
}

char *CMod_autogoApp::getConfigParamName(int nr)
{

	switch (nr)
	{
	case 0: return "act/x";
	case 1: return "act/y";
	case 2: return "act/z";
	case 3: return "act/direction";
	case 4: return "runaway/x";
	case 5: return "runaway/y";
	case 6: return "runaway/z";
	case 7: return "triggerMessage";
	case 8: return "whiteList/List";
	case 9: return "whiteList/mkBlack";

	default:
		return NULL;
	}
}
int CMod_autogoApp::isMultiParam(char *paramName)
{
	if (!strcmp(paramName,"whiteList/List")) return 1;
	return 0;
}

void CMod_autogoApp::resetMultiParamAccess(char *paramName)
{
	if (!strcmp(paramName,"whiteList/List")) currentPos=0;
}

void CMod_autogoApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin(	m_configDialog->m_enable);
	skin.SetButtonSkin(	m_configDialog->m_OK);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog)
		m_configDialog->Invalidate();
}
