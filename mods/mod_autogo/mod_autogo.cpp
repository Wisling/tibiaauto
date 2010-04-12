
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
HWND tibiaHWND = NULL;

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

void InitTibiaHandle(){
	CMemReaderProxy reader;
	tibiaHWND = FindWindowEx(NULL, NULL, "TibiaClient", NULL);
	while (tibiaHWND) {
		DWORD pid;
		DWORD dwThreadId = ::GetWindowThreadProcessId(tibiaHWND, &pid);

		if (pid == reader.getProcessId())
			break;
		tibiaHWND = FindWindowEx(NULL, tibiaHWND, "TibiaClient", NULL);
	}
}

bool flashToggle(){
	static bool flashState=true;
	static int lastFlash=GetTickCount();
	if (time(NULL)-lastFlash>=1000){
		flashState=!flashState;
		lastFlash=GetTickCount();
	}
	return flashState;
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

bool actionSuspend(CString module) {
	bool retval = false;
	masterDebug("actionSuspend");
	masterDebug("actionSuspend","Time to stop module");
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	if (hSnap) {
		MODULEENTRY32 lpModule;
		lpModule.dwSize = sizeof(MODULEENTRY32);

		Module32First(hSnap,&lpModule);
		do {
			if (lpModule.szModule == module || module.Find("<all modules>") > 0){
				FARPROC isStarted;
				isStarted = GetProcAddress(lpModule.hModule,"isStarted");
				if (isStarted && isStarted()) {
					GetProcAddress(lpModule.hModule,"stop")();
					retval = true;
				}
			}
		} while (Module32Next(hSnap,&lpModule));
		CloseHandle(hSnap);
	}
	return retval;
}

bool actionStart(CString module) {
	bool retval = false;
	masterDebug("actionStart");
	masterDebug("actionStart","Time to start module", module);
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
	if (hSnap) {
		MODULEENTRY32 lpModule;
		lpModule.dwSize = sizeof(MODULEENTRY32);

		Module32First(hSnap,&lpModule);
		do {
			if (lpModule.szModule == module || module.Find("<all modules>") > 0){
				FARPROC isStarted;
				isStarted = GetProcAddress(lpModule.hModule,"isStarted");
				if (isStarted && !isStarted()) {
					GetProcAddress(lpModule.hModule,"start")();
					retval = true;
				}
			}
		} while (Module32Next(hSnap,&lpModule));
		CloseHandle(hSnap);
	}
	return retval;
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

CString alarmStatus(CString alarm) {
	static int index = 0;
	if (alarm.GetLength() < 200) {
		index = 0;
		return alarm;
	}
	if (index >= alarm.GetLength())
		index = 0;
	CString buf = alarm + alarm.Left(200 - 10 - (alarm.GetLength() ? alarm.GetLength() : -1));
	return buf.Mid(index++) + buf.Left(200 - 10 - alarm.GetLength());
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
	if (!tibiaHWND) InitTibiaHandle();
	RECT rect;
	bool captured = false;
	bool minimized = IsIconic(tibiaHWND);

	char path[1024];
	CModuleUtil::getInstallPath(path);
	time_t lTime;
	time(&lTime);
	char timeBuf[64];
		strftime(timeBuf, 64, " %a %d %b-%H%M(%S)", localtime(&lTime));
	CString filePath;
	filePath.Format("%s\\screenshots\\Screenshot%s.bmp", path, timeBuf);
	
	while (!captured) {
		Sleep (100);
		if (reader.getConnectionState() != 8)
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
		ClientToScreen(tibiaHWND, (LPPOINT)&rect.left);
		ClientToScreen(tibiaHWND, (LPPOINT)&rect.right);

		int nWidth = rect.right - rect.left;
		int mHeight = rect.bottom - rect.top;
		HDC hDDC = GetDC(GetDesktopWindow());
		HDC hCDC = CreateCompatibleDC(hDDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(hDDC,nWidth,mHeight);
		SelectObject(hCDC,hBitmap); 
		BitBlt(hCDC,0,0,nWidth,mHeight,hDDC,rect.left,rect.top,SRCCOPY);
		WriteBMPFile(hBitmap, filePath, hCDC);
		captured = true;
		ReleaseDC(GetDesktopWindow(), hDDC);
		DeleteDC(hCDC);
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

	int lastX = self->x;
	int lastY = self->y;
	int lastZ = self->z;
	int lastMoved = 0;
	int lastHp = self->hp;
	int lastMana = self->mana;
	int recoveryAlarmHp = 0;
	int recoveryAlarmMana = 0;
	int goPriority = 0;
	bool cavebotForced = false;
	int fullSleepCount = 0;
	int halfSleepCount = 0;
	CString statusBuf = "";
	
	delete self;

	PlaySound(0, 0, 0);
		
	while (!toolThreadShouldStop) {			
		Sleep(100);
		//if we're not looking for messages consume them anyway to avoid buffer overflow/expansion
		if (!config->triggerMessage)
			triggerMessage();
		alarmItr = config->alarmList.begin();
		CTibiaCharacter *self = reader.readSelfCharacter();
		if (recoveryAlarmMana && self->mana == self->maxMana) recoveryAlarmMana = 0;
		if (recoveryAlarmHp && self->hp == self->maxMana) recoveryAlarmHp = 0;
		
		//insert my alarm check and action code
		while (alarmItr != config->alarmList.end()) {
			if (alarmItr->checkAlarm(config->whiteList, config->options)) {
				if (statusBuf.Find(alarmItr->getDescriptor()) == -1)
					statusBuf += "  ******  " + alarmItr->getDescriptor();
				// Flash Window ***************
				if (config->options&OPTIONS_FLASHONALARM && !alarmItr->flashed) {
					if (!alarmItr->windowActed){
						if (!tibiaHWND) InitTibiaHandle();
						FlashWindow(tibiaHWND, true);
					}
					alarmItr->flashed = true;
				}// ***************************
				
				// Log Event ******************
				if (alarmItr->doLogEvents() && !alarmItr->eventLogged) {
					time_t rawtime;
					time(&rawtime );
					char filename[64];
						strftime(filename, 63, "%d%b%y.txt", localtime(&rawtime));
					char timestamp[64];
						strftime(timestamp, 63, "%c", localtime(&rawtime));
					CString pathBuf;
					pathBuf.Format("%s\\logs\\%s", path, filename);
					FILE *f = fopen(pathBuf, "a+");
					if (f) {
						fprintf(f, "%s  %s\n\t%s", "***  Active  -->", alarmItr->getDescriptor(), timestamp);
						fclose(f);
					}
					alarmItr->eventLogged = true;
				}// ****************************
				
				// Play sound ******************
				if (alarmItr->doAlarm().GetLength()) {
					CString pathBuf;
					pathBuf.Format("%s\\mods\\sound\\%s", path, alarmItr->doAlarm());
					PlaySound(pathBuf, NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
				}// ****************************
				
				// Attack **********************
				if (alarmItr->doAttack()) 
					cavebotForced = actionStart("mod_cavebot.dll");
				else {
					reader.setGlobalVariable("cavebot_fullsleep","true");
					if (!alarmItr->fullSleep) {
						alarmItr->fullSleep = true;
						fullSleepCount++;
					}
				}
				// *****************************
				
				// Window Action*************
				if (alarmItr->doWindowAction()!=-1){
					switch (alarmItr->doWindowAction()){
					case 0://maximize
						if (!alarmItr->windowActed) {
							if (!tibiaHWND) InitTibiaHandle();
							ShowWindow(tibiaHWND, SW_MAXIMIZE);
							alarmItr->windowActed = true;
						}
						break;
					case 1://restore
						if (!alarmItr->windowActed) {
							if (!tibiaHWND) InitTibiaHandle();
							if(IsIconic(tibiaHWND)) {
								ShowWindow(tibiaHWND, SW_RESTORE);
							} else if(tibiaHWND != GetForegroundWindow()) {
								SetForegroundWindow(tibiaHWND);
							}
							alarmItr->windowActed = true;
						}
						break;
					case 2://flash once
						if (!alarmItr->windowActed && !alarmItr->flashed) {
							if (!tibiaHWND) InitTibiaHandle();
							FlashWindow(tibiaHWND, true);
							alarmItr->windowActed = true;
						}
						break;
					case 3://flash continuously
						if (!alarmItr->windowActed) {
							if (!tibiaHWND) InitTibiaHandle();
							static int lastFlash=GetTickCount();
							if (GetTickCount()-lastFlash>=1000){
								FlashWindow(tibiaHWND, false);
								lastFlash=GetTickCount();
							}
						}
						break;
					}
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
				if (alarmItr->doCastSpell().GetLength()) {						
					if (self->mana >= alarmItr->getManaCost() && time(NULL) - alarmItr->spellCast >= alarmItr->getSpellDelay()) {
						sender.sayWhisper(alarmItr->doCastSpell());
						alarmItr->spellCast = time(NULL);
					}
					delete self;
				}// *****************************
				
				// Take Screenshot **************
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
				}// ****************************
				
				// Goto Start ******************
				if (alarmItr->doGoToStart()  && goPriority <= 1)
					goPriority = 1;
				// *****************************

				// Goto Runaway ****************
				if (alarmItr->doGoToRunaway() && goPriority <= 2)
					goPriority = 2;
				// *****************************

				// Goto Depot ******************
				if (alarmItr->doGoToDepot()  && goPriority <= 3)
					goPriority = 3;
				// ****************************

				// Logout **********************
				if (alarmItr->doLogout()) {
					if (!(reader.getSelfEventFlags() & (int)pow(2, LOGOUTBLOCK)) && !(reader.getSelfEventFlags() & (int)pow(2, PZBLOCK)) && reader.getConnectionState() == 8 ) {
						sender.logout();
					}
				}// ****************************

				// Kill Client *****************
				if (alarmItr->doKillClient()) {
					actionTerminate();
				}
				// *****************************

				// Shutdown Computer ***********
				if (alarmItr->doShutdownComputer()) {
					actionShutdownSystem();
				}// ****************************

			}
			else if (alarmItr->doTakeScreenshot() == 1 && alarmItr->screenshotsTaken < 3 && time(NULL) - alarmItr->timeLastSS >= 1) {
				// the alarm may have stopped BEFORE we took our 3 screenshots, let's contuinue
				DWORD threadId;
				::CreateThread(NULL, 0, takeScreenshot,NULL, 0, &threadId);				
				alarmItr->screenshotsTaken++;
				alarmItr->timeLastSS = time(NULL);
			}
			else { // Clean up Alarm flags here when alarm condition are no longer true , reset modules to previous state, return to start??
				if (statusBuf.Find(alarmItr->getDescriptor()) > 0)
					statusBuf.Replace("  ******  " + alarmItr->getDescriptor(), "");
				memcpy(config->status, "", 12);
				if (alarmItr->fullSleep) {
					alarmItr->halfSleep = false;
					fullSleepCount--;
				}
				if (alarmItr->halfSleep) {
					alarmItr->halfSleep = false;
					halfSleepCount--;
				}
				if (cavebotForced)
					actionSuspend("mod_cavebot.dll");
				alarmItr->cavebotForced = false;
				alarmItr->flashed = false;
				alarmItr->windowActed = false;
				PlaySound(NULL, NULL, SND_NOSTOP);
				alarmItr->spellCast = 0;
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
					time(&rawtime );
					char filename[64];
						strftime(filename, 64, "%d%b%y.txt", localtime(&rawtime));
					char timestamp[64];
						strftime(timestamp, 63, "%c", localtime(&rawtime));
					CString pathBuf;
					pathBuf.Format("%s\\logs\\%s", path, filename);
					FILE *f = fopen(pathBuf, "a+");
					if (f) {
						fprintf(f, "%s  %s\n\t%s", "***  Inactive  -->", alarmItr->getDescriptor(), timestamp);
						fclose(f);
					}
					alarmItr->eventLogged = false;
				}
			}
			alarmItr++;
		}
		if (statusBuf.GetLength())
			memcpy(config->status, alarmStatus(statusBuf), 200);
		else
			memcpy(config->status, "", 200);
		// Do not let seperate alarms fight for control!!
		switch (goPriority) {
			if (!reader.getGlobalVariable("cavebot_fullsleep")) {
				reader.setGlobalVariable("cavebot_halfsleep","true");
				halfSleepCount++;
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
		if (reader.getGlobalVariable("cavebot_fullsleep") && fullSleepCount <= 0) {
			reader.setGlobalVariable("cavebot_fullsleep", "false");
			fullSleepCount = 0;
		}
		if (reader.getGlobalVariable("cavebot_halfsleep") && halfSleepCount <= 0) {
			reader.setGlobalVariable("cavebot_halfsleep", "false");
			halfSleepCount = 0;
		}
		goPriority = 0;
	}

	// Clean-up alarm flags before disabling, reset modules to previous state.
	alarmItr = config->alarmList.begin();
	while (alarmItr != config->alarmList.end()) {
		if (statusBuf.Find(alarmItr->getDescriptor()) > 0)
			statusBuf.Replace("  ******  " + alarmItr->getDescriptor(), "");
		if (cavebotForced) {
			cavebotForced = false;
			actionSuspend("mod_cavebot.dll");
		}
		alarmItr->flashed = false;
		alarmItr->windowActed = false;
		alarmItr->spellCast = 0;
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
		if (alarmItr->eventLogged) {
			time_t rawtime;
			time(&rawtime );
			char filename[64];
				strftime(filename, 64, "%d%b%y.txt", localtime(&rawtime));
			char timestamp[64];
				strftime(timestamp, 63, "%c", localtime(&rawtime));
			CString pathBuf;
			pathBuf.Format("%s\\logs\\%s", path, filename);
			FILE *f = fopen(pathBuf, "a+");
			if (f) {
				fprintf(f, "%s\n\t%s", "***  Module Stopped  ***", timestamp);
				fclose(f);
			}
			alarmItr->eventLogged = false;
		}
		alarmItr++;
	}
	// clear current status
	config->status[0] = '\0';
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

CMod_autogoApp::CMod_autogoApp() {
	m_configDialog = NULL;
	m_started=0;
	m_configData = new CConfigData();
}

CMod_autogoApp::~CMod_autogoApp() {
	if (m_configDialog)
		delete m_configDialog;
	delete m_configData;	
}

char * CMod_autogoApp::getName() {
	return "Auto go/logout";
}

int CMod_autogoApp::isStarted() {
	if (!m_started) {
		// if not started then regularly consume 1003 messages from the queue
		CIPCBackPipeProxy backPipe;
		struct ipcMessage mess;	

		backPipe.readFromPipe(&mess,1003);
	}
	return m_started;
}


void CMod_autogoApp::start() {
	superStart();
	if (m_configDialog) {
		m_configDialog->disableControls();
		m_configDialog->activateEnableButton(true);
	}

	DWORD threadId;
		
	toolThreadShouldStop=0;
	toolThreadHandle = ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_autogoApp::stop() {
	toolThreadShouldStop = 1;
	while (toolThreadShouldStop)
		Sleep(50);
	m_started = 0;
	
	if (m_configDialog) {
		m_configDialog->enableControls();
		m_configDialog->activateEnableButton(false);
	}
} 

void CMod_autogoApp::showConfigDialog() {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());	

	if (!m_configDialog) {
		m_configDialog = new CConfigDialog(this);
		m_configDialog->Create(IDD_CONFIG);
		configToControls();
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_autogoApp::configToControls() {
	if (m_configDialog)		
		m_configDialog->configToControls(m_configData);
}


void CMod_autogoApp::controlsToConfig() {
	if (m_configDialog) {
		delete m_configData;
		m_configData = m_configDialog->controlsToConfig();
	}
}


void CMod_autogoApp::disableControls() {
	if (m_configDialog) 
		m_configDialog->disableControls();
}

void CMod_autogoApp::enableControls() {
	if (m_configDialog) 
		m_configDialog->enableControls();
}

char *CMod_autogoApp::getVersion() {	
	return "4.11";
}


int CMod_autogoApp::validateConfig(int showAlerts) {	
	return 1;
}

void CMod_autogoApp::resetConfig() {
	m_configData = new CConfigData();
}

void CMod_autogoApp::loadConfigParam(char *paramName,char *paramValue) {
#pragma warning(disable: 4800)
	if (!strcmp(paramName,"act/x"))						m_configData->actX					= atoi(paramValue);
	if (!strcmp(paramName,"act/y"))						m_configData->actY					= atoi(paramValue);
	if (!strcmp(paramName,"act/z"))						m_configData->actZ					= atoi(paramValue);
	if (!strcmp(paramName,"act/direction"))				m_configData->actDirection			= atoi(paramValue);
	if (!strcmp(paramName,"runaway/x"))					m_configData->runawayX				= atoi(paramValue);
	if (!strcmp(paramName,"runaway/y"))					m_configData->runawayY				= atoi(paramValue);
	if (!strcmp(paramName,"runaway/z"))					m_configData->runawayZ				= atoi(paramValue);
	if (!strcmp(paramName,"triggerMessage"))			m_configData->triggerMessage		= atoi(paramValue);
	if (!strcmp(paramName,"whiteList/List")) {
		if (currentPos>99)
			return;
		lstrcpyn(m_configData->whiteList[currentPos++],paramValue,32);
	}
	if (!strcmp(paramName,"whiteList/mkBlack"))			m_configData->mkBlack	= atoi(paramValue);
	if (!strcmp(paramName,"alarmList")) {
		if (m_configData->alarmList.begin()==currentAlarmPos){
			m_configData->alarmList.clear();
		}
		CString cstr;
		char* sep=paramValue;

		char* startModules=sep;
		if ((sep=strstr(sep,"|"))==NULL) return;
		sep[0]='\0'; sep=sep+1;

		char* stopModules=sep;
		if ((sep=strstr(sep,"|"))==NULL) return;
		sep[0]='\0'; sep=sep+1;

		char* selections=sep;
		if ((sep=strstr(sep,"|"))==NULL) return;
		sep[0]='\0'; sep=sep+1;

		char* strTrigger=sep;
		if ((sep=strstr(sep,"|"))==NULL) return;
		sep[0]='\0'; sep=sep+1;

		char* castSpell=sep;
		if ((sep=strstr(sep,"|"))==NULL) return;
		sep[0]='\0'; sep=sep+1;

		char* alarmName=sep;
		if ((sep=strstr(sep,"|"))==NULL) return;
		sep[0]='\0'; sep=sep+1;

		char* params=sep;

		list<CString> startList=list<CString>();
		for (sep=strstr(startModules,",");sep!=NULL;sep=strstr(sep+1,",")){
			sep[0]=0;
			cstr=startModules;
			startList.push_back(cstr);
			startModules=sep+1;
		}

		list<CString> stopList=list<CString>();
		for (sep=strstr(stopModules,",");sep!=NULL;sep=strstr(sep+1,",")){
			sep[0]=0;
			cstr=stopModules;
			stopList.push_back(cstr);
			stopModules=sep+1;
		}
		
		int alarmType=0,attribute=0,condition=0,intTrigger=0;
		if (sscanf(selections,"%d %d %d %d",&alarmType,&attribute,&condition,&intTrigger)!=4) return;

		CString cStrTrigger=CString(strTrigger);

		CString cCastSpell=CString(castSpell);

		CString cAlarmName=CString(alarmName);

		int screenshot=0,logEvents=0,windowAction=0,shutdown=0,killTibia=0,logout=0,attack=0,depot=0,start=0,runaway=0,manaCost=0,spellDelay=0;
		if (sscanf(params,"%d %d %d %d %d %d %d %d %d %d %d %d",&screenshot,&logEvents,&windowAction,&shutdown,&killTibia,&logout,&attack,&depot,&start,&runaway,&manaCost,&spellDelay)!=12) return;

		Alarm temp(alarmType,attribute,condition,intTrigger,cStrTrigger,runaway,start,depot,cCastSpell,manaCost,spellDelay,screenshot,attack,logout,killTibia,shutdown,windowAction,cAlarmName,logEvents,startList,stopList);
		m_configData->alarmList.push_back(temp);
	}
#pragma warning(default: 4800)
}

char *CMod_autogoApp::saveConfigParam(char *paramName) {
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
	if (!strcmp(paramName,"alarmList")) {
		if (currentAlarmPos == m_configData->alarmList.end())
			return buf;
		else {
			//special "|" character used for seaparation
			Alarm alm=*currentAlarmPos;
			currentAlarmPos++;
			list<CString> startList=alm.doStartModules();
			list<CString> stopList=alm.doStopModules();
			//AddStart Modules
			list<CString>::iterator lstIter;
			lstIter=startList.begin();
			while (lstIter!=startList.end()){
				strcpy(buf+strlen(buf),*lstIter);
				sprintf(buf+strlen(buf),",");
				lstIter++;
			}
			//AddStop Modules
			lstIter=stopList.begin();
			sprintf(buf+strlen(buf),"%s","|");
			while (lstIter!=stopList.end()){
				strcpy(buf+strlen(buf),*lstIter);
				sprintf(buf+strlen(buf),",");
				lstIter++;
			}
			sprintf(buf+strlen(buf),"%s","|");
			sprintf(buf+strlen(buf),"%d %d %d %d|%s|%s|%s|%d %d %d %d %d %d %d %d %d %d %d %d",
				alm.getAlarmType(),
				alm.getAttribute(),
				alm.getCondition(),
				alm.getIntTrigger(),
				alm.getStrTrigger(),
				alm.doCastSpell(),
				alm.doAlarm(),
				alm.doTakeScreenshot(),
				alm.doLogEvents(),
				alm.doWindowAction(),
				alm.doShutdownComputer(),
				alm.doKillClient(),
				alm.doLogout(),
				alm.doAttack(),
				alm.doGoToDepot(),
				alm.doGoToStart(),
				alm.doGoToRunaway(),
				alm.getManaCost(),
				alm.getSpellDelay()
			);
		}
	}
	return buf;
}

char *CMod_autogoApp::getConfigParamName(int nr) {
	switch (nr) {
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
	case 10: return "alarmList";

	default:
		return NULL;
	}
}

int CMod_autogoApp::isMultiParam(char *paramName) {
	if (!strcmp(paramName,"whiteList/List")) return 1;
	if (!strcmp(paramName,"alarmList")) return 1;
	return 0;
}

void CMod_autogoApp::resetMultiParamAccess(char *paramName) {
	if (!strcmp(paramName,"whiteList/List")) currentPos=0;
	if (!strcmp(paramName,"alarmList")){
		currentAlarmPos=m_configData->alarmList.begin();
	}
}

void CMod_autogoApp::getNewSkin(CSkin newSkin) {
	skin = newSkin;
	skin.SetButtonSkin(	m_configDialog->m_enable);
	skin.SetButtonSkin(	m_configDialog->m_OK);

	AFX_MANAGE_STATE(AfxGetStaticModuleState());			
	if (m_configDialog)
		m_configDialog->Invalidate();
}
