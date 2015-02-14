
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


// mod_autogo.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mod_autogo.h"

#include "ConfigDialog.h"
#include "ConfigData.h"
#include "TibiaContainer.h"

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

#define JPEGLIB_USE_STDIO
#include <jpeglib.h>
#include <zlib.h>
#include <png.h>

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
HANDLE soundThreadHandle;
char soundPath[2048];
HWND tibiaHWND = NULL;
time_t timeLastSS = 0;
time_t isTakingScreenshot = 0;

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
			if (lpModule.szModule == module){
				FARPROC isStarted;
				isStarted = GetProcAddress(lpModule.hModule,"isStarted");
				if (isStarted && isStarted()) {
					GetProcAddress(lpModule.hModule,"stop")();
					retval = true;
				}
				break;
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
			if (lpModule.szModule == module){
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
		if (!_strcmpi(whiteList[i],name)){
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
		int chanType;
		int nickLen;
		int msgLen;
		char nickBuf[16384];
		char msgBuf[16384];
		
		memset(nickBuf,0,16384);
		memset(msgBuf,0,16384);
		memcpy(&infoType,mess.payload,sizeof(int));
		memcpy(&chanType,mess.payload+4,sizeof(int));
		memcpy(&nickLen,mess.payload+8,sizeof(int));
		memcpy(&msgLen,mess.payload+12,sizeof(int));
		memcpy(nickBuf,mess.payload+16,nickLen);
		memcpy(msgBuf,mess.payload+16+nickLen,msgLen);
		
		CTibiaCharacter *self = reader.readSelfCharacter();

		if (_strcmpi(nickBuf,self->name)!=0 && _strcmpi(nickBuf,"Tibia Auto") != 0) {
			delete self;
			struct tibiaMessage *newMsg = new tibiaMessage();
			newMsg->type=infoType;
			newMsg->chanType=chanType;
			strcpy(newMsg->nick,nickBuf);
			strcpy(newMsg->msg,msgBuf);
			masterDebug("triggerMessage Exit");
			return newMsg;
		}
		delete self;
	}
	masterDebug("triggerMessage Exit");
	return NULL;
}

CString* alarmStatus(CString alarm) {
	static int index = 0;
	if (alarm.GetLength() < 75) {
		index = 0;
		CString* ret=new CString(alarm);
		return ret;
	}
	if (index >= alarm.GetLength())
		index = 0;
	CString buf = alarm + alarm.Left(200 - 10 - (alarm.GetLength() ? alarm.GetLength() : -1));
	CString* ret=new CString(buf.Mid(index++) + buf.Left(200 - 10 - alarm.GetLength()));
	return ret;
}

void WriteJPGFile(HBITMAP bitmap, CString filename, HDC hDC){
	static int extra = 0;
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD cClrBits;
	PBITMAPINFOHEADER pbih; // bitmap info-header
	LPBYTE lpBits; // memory pointer
	BYTE *hp; // byte pointer
	
	// create the bitmapinfo header information
	
	if (!GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp)) {
		AfxMessageBox("Could not retrieve bitmap info");
		return;
	}
	
	// Specify the color format to a count of bits.
	cClrBits = 24;
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
	pbmi->bmiHeader.biBitCount = cClrBits;
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
	
	hp = lpBits;

	FILE* outfile = fopen(filename, "wb");
	if (!outfile){
		char errormsg[4096];
		sprintf(errormsg,"Could not open file for writing: %s",filename);
		AfxMessageBox(errormsg);
		return;
	}
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_CreateCompress(&cinfo,JPEG_LIB_VERSION,sizeof(jpeg_compress_struct));
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width      = pbih->biWidth;
	cinfo.image_height     = pbih->biHeight;
	cinfo.input_components = 3;
	cinfo.in_color_space   = (J_COLOR_SPACE)2;
	jpeg_set_defaults(&cinfo);
	/*set the quality [0..100]  */
	jpeg_set_quality (&cinfo, 60, true);
	jpeg_start_compress(&cinfo, true);


	JSAMPROW row_pointer;          /* pointer to a single row */
	 
	while ((LONG)cinfo.next_scanline < pbih->biHeight) {
		unsigned int linesize = pbih->biSizeImage/pbih->biHeight;
		unsigned int linestart = pbih->biSizeImage-(cinfo.next_scanline+1)*linesize;
		row_pointer = (JSAMPROW) &(hp[linestart]);
		for (unsigned int i = 0; i < linesize; i += 3){//output from GetDIBits is BGR, switch to RGB
			hp[linestart + i] ^= hp[linestart + i + 2];
			hp[linestart + i + 2] ^= hp[linestart + i];
			hp[linestart + i] ^= hp[linestart + i + 2];
		}
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	fclose( outfile );
	// Free memory.
	GlobalFree((HGLOBAL)lpBits);
}

void WritePNGFile(HBITMAP bitmap, CString filename, HDC hDC){
	static int extra = 0;
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD cClrBits;
	PBITMAPINFOHEADER pbih; // bitmap info-header
	LPBYTE lpBits; // memory pointer
	BYTE *hp; // byte pointer
	
	// create the bitmapinfo header information
	
	if (!GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp)) {
		AfxMessageBox("Could not retrieve bitmap info");
		return;
	}
	
	// Specify the color format to a count of bits.
	cClrBits = 24;
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
	pbmi->bmiHeader.biBitCount = cClrBits;
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
	
	hp = lpBits;

	FILE* outfile = fopen(filename, "wb");
	if (!outfile){
		char errormsg[4096];
		sprintf(errormsg,"Could not open file for writing: %s",filename);
		AfxMessageBox(errormsg);
		return;
	}
	//wiz:
	png_structp png_ptr;
	png_infop info_ptr;

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr){
        AfxMessageBox("[write_png_file] png_create_write_struct failed");
		return;
	}


    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr){
        AfxMessageBox("[write_png_file] png_create_info_struct failed");
		return;
	}

    if (setjmp(png_jmpbuf(png_ptr))){
         AfxMessageBox("[write_png_file] Error during init_io");
		 return;
	}

    png_init_io(png_ptr, outfile);


    /* write header */
    if (setjmp(png_jmpbuf(png_ptr))){
        AfxMessageBox("[write_png_file] Error during writing header");
		return;
	}

    png_set_IHDR(png_ptr, info_ptr, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight,
                 (png_byte)8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr))){
        AfxMessageBox("[write_png_file] Error during writing bytes");
		return;
	}

	png_bytep* row_pointers = (png_bytep*)malloc(pbmi->bmiHeader.biHeight*sizeof(png_bytep));
    for (int k = 0; k < pbmi->bmiHeader.biHeight; k++){
		unsigned int linesize = pbih->biSizeImage/pbih->biHeight;
		unsigned int linestart = pbih->biSizeImage-(k+1)*linesize;
		for(unsigned int i=0;i<linesize;i+=3){//output from GetDIBits is BGR, switch to RBG
			hp[linestart+i] ^= hp[linestart+i+2];
			hp[linestart+i+2] ^= hp[linestart+i];
			hp[linestart+i] ^= hp[linestart+i+2];
		}
        row_pointers[k] = (png_bytep) &(hp[linestart]);
	}
    png_write_image(png_ptr, row_pointers);
	free(row_pointers);

    /* end write */
    if (setjmp(png_jmpbuf(png_ptr))){
        AfxMessageBox("[write_png_file] Error during end of write");
		return;
	}
    png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */

    fclose(outfile);
}

void WriteBMPFile(HBITMAP bitmap, CString filename, HDC hDC) {
	static int extra = 0;
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
	if (hf == INVALID_HANDLE_VALUE) {
		CString postfix;
		postfix.Format("%d.bmp", extra++);
		filename.Replace(".bmp", postfix);
		hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD) 0,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
			(HANDLE) NULL);
		if (hf == INVALID_HANDLE_VALUE) {
			AfxMessageBox("Could not create file for writing");
			return;
		}
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

enum PicFileType{
	BMPFILE,PNGFILE,JPGFILE
};
struct TS_param{
	PicFileType fileType;
};

DWORD WINAPI takeScreenshot(LPVOID lpParam) {
	TS_param* params = (TS_param*) lpParam;

	CMemReaderProxy reader;
	if (!tibiaHWND) InitTibiaHandle();
	RECT rect;
	bool minimized = IsIconic(tibiaHWND)!=0;
	bool trayed = IsWindowVisible(tibiaHWND)==0;

	char path[1024];
	CModuleUtil::getInstallPath(path);
	time_t lTime;
	time(&lTime);
	char timeBuf[64];
		strftime(timeBuf, 64, " %a %d %b-%H%M(%S)", localtime(&lTime));
	CString filePath;
	if(params->fileType==PNGFILE){
		filePath.Format("%s\\screenshots\\Screenshot%s.png", path, timeBuf);
	}else if(params->fileType==JPGFILE){
		filePath.Format("%s\\screenshots\\Screenshot%s.jpg", path, timeBuf);
	}else if(params->fileType==BMPFILE){
		filePath.Format("%s\\screenshots\\Screenshot%s.bmp", path, timeBuf);
	}else{
		AfxMessageBox("takeScreenshot: Unknown file type.");
	}
	int tr=50;
	while (tr>=0) {//attempt to take screenshot for 5 seconds
		tr--;
		Sleep (100);
		if (!reader.isLoggedIn())
			continue;
		if(!IsWindowVisible(tibiaHWND)) {
			ShowWindow(tibiaHWND, SW_SHOW);
			continue;
		}
		if(IsIconic(tibiaHWND)) {
			ShowWindow(tibiaHWND, SW_RESTORE);
			continue;
		}
		if(tibiaHWND != GetForegroundWindow()) {
			SetForegroundWindow(tibiaHWND);
			continue;
		}
		if (trayed){
			Sleep(1000);
		} else if (minimized){
			Sleep (500);
		} else {
			Sleep(100);
		}
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
		if(params->fileType==PNGFILE){
			WritePNGFile(hBitmap, filePath, hCDC);
		}else if(params->fileType==JPGFILE){
			WriteJPGFile(hBitmap, filePath, hCDC);
		}else if(params->fileType==BMPFILE){
			WriteBMPFile(hBitmap, filePath, hCDC);
		}else{
			AfxMessageBox("takeScreenshot: Unknown file type.");
		}
		
		ReleaseDC(GetDesktopWindow(), hDDC);
		DeleteDC(hCDC);
		timeLastSS = time(NULL);
		break;
	}
	if (minimized)
		ShowWindow(tibiaHWND, SW_MINIMIZE);
	if (trayed)
		ShowWindow(tibiaHWND, SW_HIDE);
	isTakingScreenshot = 0;
	delete(params); // delete here as owner process has no indication of when this should be removed
	return NULL;
}

bool shouldStopWalking(list<Alarm> test) {
	bool retVal = false;
	list<Alarm>::iterator alarmItr = test.begin();
	while (alarmItr != test.end() && !retVal) {
		retVal = alarmItr->alarmState && alarmItr->getStopWalking();
		alarmItr++;
	}
	return retVal;
}
int getGoPriority(list<Alarm> test, bool isGoingToRunaway, int maintainPos){
	int retVal = 0;
	list<Alarm>::iterator alarmItr = test.begin();
	while (alarmItr != test.end() && retVal!=3) {
		if(alarmItr->alarmState==0){
			alarmItr++;
			continue;
		}
		// Stay In Place ******************
		if (maintainPos  && retVal <= 1) {
			retVal = 1;
		}

		//Start and Runaway can both be selected at the same time, but not Depot
		// Goto Start ******************
		if (alarmItr->getGoToStart()  && retVal <= 2) {
			retVal = 2;
		}

		// Goto Runaway **************** //stick with going to Start if not isGoingToRunaway
		if (alarmItr->getGoToRunaway() && retVal <= 3 && (!alarmItr->getGoToStart() || isGoingToRunaway)) {
			retVal = 3;
		}

		// Goto Depot ******************
		if (alarmItr->getGoToDepot()  && retVal <= 4) {
			retVal = 4;
		}
		alarmItr++;
	}
	return retVal;

}
int shouldKeepWalking() {
	//considers whether we are attacking and done looting
	static time_t lastAttackTime=0;
	CMemReaderProxy reader;
	if (!reader.getAttackedCreature()){
		const char *var=reader.getGlobalVariable("autolooterTm");
		if (strcmp(var,"")==0){
			if (time(NULL)-lastAttackTime>3)
				return 1;
			else
				return 0;
		}
	}
	lastAttackTime=time(NULL);
	return 0;
}

// Required to be run at least every 3 seconds to be useful since it updates lastAttackTm
int doneAttackingAndLooting(){
	CMemReaderProxy reader;
	static int lastAttackTm=0;
	int ret = GetTickCount()-lastAttackTm>3*1000 && !reader.getAttackedCreature();
	if (reader.getAttackedCreature()){
		lastAttackTm=GetTickCount();
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// Tool thread function

DWORD WINAPI toolThreadProc( LPVOID lpParam ) {
	masterDebug("toolThreadProc");
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	CConfigData *config = (CConfigData *)lpParam;
	list<Alarm>::iterator alarmItr;
	CTibiaCharacter *self = reader.readSelfCharacter();
	char path[1024];
	CModuleUtil::getInstallPath(path);

	int maintainX = 0;
	int maintainY = 0;
	int maintainZ = 0;
	int sentMessagePathnotfound = 0;
	//int lastMoved = 0;
	//int lastHp = self->hp;
	//int lastMana = self->mana;
	int recoveryAlarmHp = 0;
	int recoveryAlarmMana = 0;
	int goPriority = 0;
	bool isGoingToRunaway=true;
	bool isDestinationReached=false;
	int stopWalk = 0;
	CString statusBuf = "";
	soundPath[0]=0;
	int modRuns=0;
	timeLastSS = 0;
	
	delete self;

	PlaySound(0, 0, 0);
	alarmItr = config->alarmList.begin();
	while (alarmItr != config->alarmList.end()) {
		alarmItr->initializeCharacter();
		alarmItr++;
	}
	while (!toolThreadShouldStop) {
		Sleep(200);
		//if we're not looking for messages consume them anyway to avoid buffer overflow/expansion
		alarmItr = config->alarmList.begin();
		CTibiaCharacter *self = reader.readSelfCharacter();
		if (recoveryAlarmMana && self->mana == self->maxMana) recoveryAlarmMana = 0;
		if (recoveryAlarmHp && self->hp == self->maxMana) recoveryAlarmHp = 0;

		modRuns++;

		statusBuf="";
		//insert my alarm check and action code
		struct tibiaMessage *msg = triggerMessage();
		while (alarmItr != config->alarmList.end()) {
			if (modRuns%alarmItr->runCycle != 0){ // Never skips alarm if runCycle==1
				alarmItr++;
				continue;
			}
			bool isLoggedOut = !reader.isLoggedIn();

			//Note that alarmItr->alarmState is set to shouldAlarm when finished
			int shouldAlarm=alarmItr->checkAlarm(config->whiteList, config->options, msg); //Check if criteria is satisfied
			shouldAlarm = shouldAlarm || alarmItr->alarmState && alarmItr->getPermanent(); // keep alarm on if permanent
			shouldAlarm = shouldAlarm || alarmItr->alarmState && (alarmItr->getPersistent() && alarmItr->keepPersistent(isDestinationReached,isLoggedOut)); // keep alarm on if action not yet completed

			if (shouldAlarm && shouldAlarm != alarmItr->alarmState) {//state changed to ON

				// Highlight Window ***************
				if ((config->options&OPTIONS_FLASHONALARM)) {
					if (!tibiaHWND) InitTibiaHandle();
					FlashWindow(tibiaHWND, true);
				}// ****************************

				// Log Event ******************
				if (alarmItr->getLogEvents()) {
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
				}// ****************************

				// Window Action*************
				if (alarmItr->getWindowAction() > -1) {
					switch (alarmItr->getWindowAction()) {
					case 0://maximize
						if (!tibiaHWND) InitTibiaHandle();
						ShowWindow(tibiaHWND, SW_MAXIMIZE);
						if(tibiaHWND != GetForegroundWindow())
							SetForegroundWindow(tibiaHWND);
						break;
					case 1://restore
						if (!tibiaHWND) InitTibiaHandle();
						if(!IsWindowVisible(tibiaHWND))
							ShowWindow(tibiaHWND, SW_SHOW);
						if(IsIconic(tibiaHWND))
							ShowWindow(tibiaHWND, SW_RESTORE);
						else if(tibiaHWND != GetForegroundWindow())
							SetForegroundWindow(tibiaHWND);
						break;
					case 2://flash once
						if (!(config->options&OPTIONS_FLASHONALARM)) {
							if (!tibiaHWND) InitTibiaHandle();
							FlashWindow(tibiaHWND, true);
						}
						break;
					}
				}// ****************************

				// Suspend Modules  ************
				if (alarmItr->getStopModules().size()) {
					list<CString> temp = alarmItr->getStopModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionSuspend(*modulesItr);
						modulesItr++;
					}
				}// ****************************

				// Start Modules ***************
				if (alarmItr->getStartModules().size()) {
					list<CString> temp = alarmItr->getStartModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionStart(*modulesItr);
						modulesItr++;
					}
					
				}// ****************************

				// Kill Client *****************
				if (alarmItr->getKillClient()) {
					actionTerminate();
				}
				// *****************************

				// Shutdown Computer ***********
				if (alarmItr->getShutdownComputer()) {
					actionShutdownSystem();
				}// ****************************
			}

			if (shouldAlarm) {
				if (statusBuf.Find(alarmItr->getDescriptor()) == -1)
					statusBuf += "  ******  " + alarmItr->getDescriptor();
								
				// Play sound ******************
				if (alarmItr->getAlarm().GetLength()) {
					CString pathBuf;
					pathBuf.Format("%s\\mods\\sound\\%s", path, alarmItr->getAlarm());
					PlaySound(pathBuf, NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
				}// ****************************
								
				// Window Action*************
				if (alarmItr->getWindowAction() > -1) {
					switch (alarmItr->getWindowAction()) {
					case 3://flash continuously
						if (!tibiaHWND) InitTibiaHandle();
						static int lastFlash=GetTickCount();
						if (GetTickCount()-lastFlash>=1000){
							FlashWindow(tibiaHWND, true);
							lastFlash=GetTickCount();
						}
						break;
					}
				}// ****************************
							
				// Cast spell ******************
				if (alarmItr->getCastSpell().GetLength()) {
					if (self->mana >= alarmItr->getManaCost() && time(NULL) - alarmItr->spellCast >= alarmItr->getSpellDelay()) {
						sender.say(alarmItr->getCastSpell());
						alarmItr->spellCast = time(NULL);
					}
				}// *****************************
				
				// Take Screenshot **************
				if (alarmItr->getTakeScreenshot() > -1 && (time(NULL)-isTakingScreenshot)>10) { // give takeScreenshot thread 10 seconds to finish
					DWORD threadId;
					TS_param* params = new TS_param;
					params->fileType=(PicFileType)(config->screenshotType);
					switch (alarmItr->getTakeScreenshot()) {
					case 0:
						if (!alarmItr->screenshotsTaken) {
							isTakingScreenshot = time(NULL);
							::CreateThread(NULL, 0, takeScreenshot,params, 0, &threadId);
							alarmItr->screenshotsTaken++;
						}
						break;
					case 1:
						if (alarmItr->screenshotsTaken < 3 && time(NULL) - timeLastSS >= 1) {
							isTakingScreenshot = time(NULL);
							::CreateThread(NULL, 0, takeScreenshot,params, 0, &threadId);
							alarmItr->screenshotsTaken++;
						}
						break;
					case 2:
						if (time(NULL) - timeLastSS >= 5) {
							isTakingScreenshot = time(NULL);
							::CreateThread(NULL, 0, takeScreenshot,params, 0, &threadId);
							alarmItr->screenshotsTaken++;
						}
						break;
					case 3:
						if (time(NULL) - timeLastSS >= 10) {
							isTakingScreenshot = time(NULL);
							::CreateThread(NULL, 0, takeScreenshot,params, 0, &threadId);
							alarmItr->screenshotsTaken++;
						}
						break;
					}
				}// ****************************
				
				// Logout **********************
				if (alarmItr->getLogout()) {
					if (!(reader.getSelfEventFlags() & (1L << LOGOUTBLOCK)) && !(reader.getSelfEventFlags() & (1L << PZBLOCK)) && reader.isLoggedIn()) {
						sender.logout();
					}
				}// ****************************
			}

			if (!shouldAlarm){ //Alarm is OFF

				// Take Screenshot **************  //Finishes taking screenshots then resets to 0
				if (alarmItr->getTakeScreenshot() == 1 && alarmItr->screenshotsTaken > 0 && time(NULL) - timeLastSS >= 1 && time(NULL)-isTakingScreenshot>10){ // give takeScreenshot thread 10 seconds to finish
					DWORD threadId;
					TS_param* params = new TS_param;
					params->fileType=(PicFileType)(config->screenshotType);
					if (alarmItr->screenshotsTaken < 3) {
						isTakingScreenshot = time(NULL);
						::CreateThread(NULL, 0, takeScreenshot,params, 0, &threadId);
						alarmItr->screenshotsTaken++;
					}
					if (alarmItr->screenshotsTaken >= 3) alarmItr->screenshotsTaken = 0;
				}// ****************************
			}

			if (!shouldAlarm && shouldAlarm != alarmItr->alarmState) {//state changed to OFF

				// Take Screenshot **************  //Let option 1 finish taking screenshots, it resets to 0 after
				if (alarmItr->getTakeScreenshot() != 1){
					alarmItr->screenshotsTaken = 0;
				}
				// *****************************

				//Start and Suspend modules MUST be done in reverse order to before to prevent thread dead-lock
				// Start Modules ***************
				if (alarmItr->getStartModules().size()) {
					list<CString> temp = alarmItr->getStartModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionSuspend(*modulesItr);
						modulesItr++;
					}
				}// ****************************

				// Suspend Modules  ************
				if (alarmItr->getStopModules().size()) {
					list<CString> temp = alarmItr->getStopModules();
					list<CString>::iterator modulesItr = temp.begin();
					while(modulesItr != temp.end()) {
						actionStart(*modulesItr);
						modulesItr++;
					}
				}// ****************************

				// Log Event ***************
				if (alarmItr->getLogEvents()) {
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
				}// ****************************

				// Clean up
				if (statusBuf.Find(alarmItr->getDescriptor()) > 0)
					statusBuf.Replace("  ******  " + alarmItr->getDescriptor(), "");
				config->status[0]='\0';
				//alarmItr->spellCast = 0; // Probably a good idea to keep cast time
			}
			alarmItr->alarmState=shouldAlarm;
			alarmItr++;
		}

		//Calculate walk variables from entire list
		stopWalk = shouldStopWalking(config->alarmList);
		goPriority = getGoPriority(config->alarmList,isGoingToRunaway,config->maintainPos);

		if (statusBuf.GetLength()){
			CString* statusMsg=alarmStatus(statusBuf);
			int len=min(statusBuf.GetLength(),2000);
			memcpy(config->status,*statusMsg, len);
			config->status[len]=0;
			delete statusMsg;
		} else
			config->status[0]='\0';

		// Do not let seperate modules fight for control!!
		bool control= strcmp(reader.getGlobalVariable("walking_control"),"autogo")==0;
		int modpriority=atoi(reader.getGlobalVariable("walking_priority"));
		// if wants control
		if (goPriority || stopWalk){
			//if should have control, take it
			if (!control && atoi(config->modPriorityStr) > modpriority){
				reader.setGlobalVariable("walking_control","autogo");
				reader.setGlobalVariable("walking_priority",config->modPriorityStr);
			}
		} else { // if doesn't want control
			//if has control, give it up
			if (control){
				reader.setGlobalVariable("walking_control","");
				reader.setGlobalVariable("walking_priority","0");
			}
		}
		int keepMaintainPos = 0;
		doneAttackingAndLooting(); //guarantees it will be run even when it is not needed
		if (doneAttackingAndLooting() && strcmp(reader.getGlobalVariable("walking_control"),"autogo")==0){
			// if stopWalk and not maintainPos then do nothing
			if (!stopWalk || goPriority==1) {
				switch (goPriority) {
				case 1: { // Stay in one place
					keepMaintainPos=1;
					if (maintainX == 0){ //Sets position here. Unsets position if not reached here.
						maintainX = self->x;
						maintainY = self->y;
						maintainZ = self->z;
					}
					int pathSize = 0;
					int path[15];
					
					if (abs(self->x-maintainX)>1 || abs(self->y-maintainY)>1 || self->z!=maintainZ) {
						if (shouldKeepWalking()) {
							// proceed with path searching
							delete self;
							self=reader.readSelfCharacter();
							CModuleUtil::findPathOnMap(self->x, self->y, self->z, maintainX, maintainY, maintainZ, 0, path, 1);
							for (; pathSize < 15 && path[pathSize]; pathSize++);
							if (pathSize){
								CModuleUtil::executeWalk(self->x, self->y, self->z, path);
							}else if(self->z==maintainZ && abs(self->x-maintainX)<50 && abs(self->y-maintainY)<50) {
								//Since we should already be close to the position, try to mapclick when map not researched
								reader.writeGotoCoords(maintainX, maintainY, maintainZ);
							}else if(!sentMessagePathnotfound){
								sentMessagePathnotfound=1;
								sender.sendTAMessage("Auto go/log: No path found to maintained position");
							}
						}
					}
					   }
					break;
				case 2: {// Start position (By definition, the least safe place to be)
					int pathSize = 0;
					int path[15];
					const char* var=reader.getGlobalVariable("autolooterTm");


					if (abs(self->x-config->actX)>1 || abs(self->y-config->actY)>1 || self->z!=config->actZ) {
						if (shouldKeepWalking()){
							// proceed with path searching
							delete self;
							self=reader.readSelfCharacter();
							CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->actX, config->actY, config->actZ, 0, path, 1);
							for (; pathSize < 15 && path[pathSize]; pathSize++);
							if (pathSize){
								CModuleUtil::executeWalk(self->x, self->y, self->z, path);
							}else if(!sentMessagePathnotfound){
								sentMessagePathnotfound=1;
								sender.sendTAMessage("Auto go/log: No path found to start position");
							}
						}
					} else {
						isDestinationReached=true;
						isGoingToRunaway=true; //Switch to going to runaway if enabled
						if (config->actDirection) {
							if (config->actDirection == DIR_LEFT)
								sender.turnLeft();
							else if(config->actDirection == DIR_RIGHT)
								sender.turnRight();
							else if(config->actDirection == DIR_UP)
								sender.turnUp();
							else if(config->actDirection == DIR_DOWN)
								sender.turnDown();
						}
					}
					   }
					break;
				case 3: {// Runaway Position (By definition, the relatively safe spot chosen by the user)
					int pathSize = 0;
					CTibiaMapProxy tibiaMap;
					
					int path[15];
					
					if (abs(self->x-config->runawayX)>1 || abs(self->y-config->runawayY)>1 || self->z!=config->runawayZ) {
						// proceed with path searching
						if (shouldKeepWalking()){
							delete self;
							self=reader.readSelfCharacter();
							CModuleUtil::findPathOnMap(self->x, self->y, self->z, config->runawayX, config->runawayY, config->runawayZ, 0, path, 1);
							for (; pathSize < 15 && path[pathSize]; pathSize++);
							if (pathSize){
								CModuleUtil::executeWalk(self->x,self->y,self->z,path);
							}else if(!sentMessagePathnotfound){
								sentMessagePathnotfound=1;
								sender.sendTAMessage("Auto go/log: No path found to Runaway position");
							}
						}
					} else {
						isDestinationReached=true;
						isGoingToRunaway=false; //switch going back to start if enabled
					}
						}
					break;
				case 4: {// Depot (Reasoned as, the safest position [because you are protected from attack])
					int pathSize = 0;
					int path[15];
					struct point p = CModuleUtil::findPathOnMap(self->x,self->y,self->z,0,0,0,301,path);
					for (; pathSize < 15 && path[pathSize]; pathSize++);
					if (shouldKeepWalking()){
						delete self;
						self=reader.readSelfCharacter();
						if (pathSize){
							CModuleUtil::executeWalk(self->x, self->y, self->z, path);
						}
					}
					if (pathSize==0){
						if(p.x || p.y || p.z) {
							isDestinationReached=true;
						}else if(!sentMessagePathnotfound){
							sentMessagePathnotfound=1;
							sender.sendTAMessage("Auto go/log: No path found to depot position");
						}
					}
						}
					break;
				}
			}
		}
		if (!keepMaintainPos){
			maintainX = 0;
			maintainY = 0;
			maintainZ = 0;
		}
		delete self;
		if (msg) delete msg;
	}

	// Clean-up alarm flags before disabling, reset modules to previous state.
	alarmItr = config->alarmList.begin();
	while (alarmItr != config->alarmList.end()) {
		alarmItr->screenshotsTaken = 0;
		alarmItr->spellCast = 0;

		if (alarmItr->alarmState==0){
			alarmItr++;
			continue;
		}
		// Suspend Modules  ************
		if (alarmItr->getStopModules().size()) {
			list<CString> temp = alarmItr->getStopModules();
			list<CString>::iterator modulesItr = temp.begin();
			while(modulesItr != temp.end()) {
				actionStart(*modulesItr);
				modulesItr++;
			}
		}// ****************************

		// Start Modules ***************
		if (alarmItr->getStartModules().size()) {
			list<CString> temp = alarmItr->getStartModules();
			list<CString>::iterator modulesItr = temp.begin();
			while(modulesItr != temp.end()) {
				actionSuspend(*modulesItr);
				modulesItr++;
			}
		}// ****************************

		// Log Event ***************
		if (alarmItr->getLogEvents()) {
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
				fprintf(f, "%s  %s\n\t%s", "***  Module Stopped  -->", alarmItr->getDescriptor(), timestamp);
				fclose(f);
			}
		}// ****************************

		// Clean up
		if (statusBuf.Find(alarmItr->getDescriptor()) > 0)
			statusBuf.Replace("  ******  " + alarmItr->getDescriptor(), "");
		config->status[0]='\0';
				
		alarmItr++;
	}
	// clear current status
	config->status[0]=0;
	// stop the alarm;
	PlaySound(NULL, NULL, NULL);

	//release control
	if (strcmp(reader.getGlobalVariable("walking_control"),"autogo")==0){
		reader.setGlobalVariable("walking_control","");
		reader.setGlobalVariable("walking_priority","0");
	}

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
	if (m_configDialog){
		m_configDialog->DestroyWindow();
		delete m_configDialog;
	}
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
		if (m_started) disableControls();
		else enableControls();
		m_configDialog->m_enable.SetCheck(m_started);
	}
	m_configDialog->ShowWindow(SW_SHOW);
}


void CMod_autogoApp::configToControls() {
	if (m_configDialog)	{
		m_configDialog->configToControls(m_configData);
	}
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
	if(m_configData){
		delete m_configData;
		m_configData = NULL;
	}
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
	if (!strcmp(paramName,"screenshotType"))			m_configData->screenshotType		= atoi(paramValue);
	if (!strcmp(paramName,"whiteList/mkBlack"))			m_configData->options |= OPTIONS_MAKE_BLACKLIST*atoi(paramValue);
	if (!strcmp(paramName,"options"))			m_configData->options = atoi(paramValue);
	if (!strcmp(paramName,"modPriority")) strncpy(m_configData->modPriorityStr,paramValue,2);
	if (!strcmp(paramName,"whiteList/List")) {
		if (currentPos>99)
			return;
		lstrcpyn(m_configData->whiteList[currentPos++],paramValue,32);
	}
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

		//added a new saved parameter, not present in older saved configs
		char* descript;
		if ((sep=strstr(sep,"|"))==NULL){
			descript = "resave script to save the correct description";
		}else{
			sep[0]='\0'; sep=sep+1;
			descript=sep;
		}

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
		
		int alarmType=0,attribute=0,condition=0,trigType=0;
		if (sscanf(selections,"%d %d %d %d",&alarmType,&attribute,&condition,&trigType)!=4) return;

		CString cStrTrigger=CString(strTrigger);

		CString cCastSpell=CString(castSpell);

		CString cAlarmName=CString(alarmName);

		int screenshot=0,logEvents=0,windowAction=0,shutdown=0,killTibia=0,logout=0,stopWalk=0,depot=0,start=0,runaway=0,manaCost=0,spellDelay=0,persistent=0,permanent=0;
		if (sscanf(params,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d",&screenshot,&logEvents,&windowAction,&shutdown,&killTibia,&logout,&stopWalk,&depot,&start,&runaway,&manaCost,&spellDelay,&persistent,&permanent)!=14){
			if (sscanf(params,"%d %d %d %d %d %d %d %d %d %d %d %d",&screenshot,&logEvents,&windowAction,&shutdown,&killTibia,&logout,&stopWalk,&depot,&start,&runaway,&manaCost,&spellDelay)!=12) return;
		}

		Alarm temp(alarmType,attribute,condition,trigType,cStrTrigger,runaway,start,depot,cCastSpell,manaCost,spellDelay,screenshot,stopWalk,logout,killTibia,shutdown,windowAction,cAlarmName,logEvents,startList,stopList,persistent,permanent,descript);
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
	if (!strcmp(paramName,"screenshotType"))			sprintf(buf,"%d",m_configData->screenshotType);
	if (!strcmp(paramName,"options"))			sprintf(buf,"%d",m_configData->options);
	if (!strcmp(paramName,"modPriority")) strncpy(buf,m_configData->modPriorityStr,2);
	if (!strcmp(paramName,"whiteList/List")){
		if (currentPos<100){
			if (IsCharAlphaNumeric(m_configData->whiteList[currentPos][0])){
				lstrcpyn(buf,m_configData->whiteList[currentPos++],32);
			}
		}
	}
	if (!strcmp(paramName,"alarmList")) {
		if (currentAlarmPos == m_configData->alarmList.end())
			return buf;
		else {
			//special "|" character used for seaparation
			Alarm alm=*currentAlarmPos;
			currentAlarmPos++;
			list<CString> startList=alm.getStartModules();
			list<CString> stopList=alm.getStopModules();
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
			sprintf(buf+strlen(buf),"%d %d %d %d|%s|%s|%s|%d %d %d %d %d %d %d %d %d %d %d %d %d %d|%s",
				alm.getAlarmType(),
				alm.getAttribute(),
				alm.getCondition(),
				alm.getTrigger().getType(),
				alm.getTrigger().getTriggerText(),
				alm.getCastSpell(),
				alm.getAlarm(),
				alm.getTakeScreenshot(),
				alm.getLogEvents(),
				alm.getWindowAction(),
				alm.getShutdownComputer(),
				alm.getKillClient(),
				alm.getLogout(),
				alm.getStopWalking(),
				alm.getGoToDepot(),
				alm.getGoToStart(),
				alm.getGoToRunaway(),
				alm.getManaCost(),
				alm.getSpellDelay(),
				alm.getPersistent(),
				alm.getPermanent(),
				alm.getDescriptor()
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
	case 11: return "options";
	case 12: return "modPriority";
	case 13: return "screenshotType";

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

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_configDialog){
		m_configDialog->DoSetButtonSkin();
		m_configDialog->Invalidate();
	}
}
