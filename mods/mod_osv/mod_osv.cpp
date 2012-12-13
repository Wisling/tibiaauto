#include "stdafx.h"
#include "Includes.h"

void WriteBMPFile(HBITMAP bitmap, LPTSTR filename, HDC hDC);

BEGIN_MESSAGE_MAP(CMod_OSVApp, CWinApp)
END_MESSAGE_MAP()

int toolThreadShouldStop=0;

HANDLE toolThreadHandle;


DWORD WINAPI toolThreadProc( LPVOID lpParam ) {			
	CMemReaderProxy reader;
	CPackSenderProxy sender;
	HWND tibiaHWND=NULL;
	int Orijanwidth=0, Orijanheight=0, OrijanLeft=0, OrijanTop=0, Oribarpos=0 ;
	HANDLE procHandle;
	int rectptr = 0x00126204;
	int baradr  = 0x06F09838;
	int distLeft = 0x14;
	int distTop = 0x18;
	int distHeight = 0x20;
	int distWidth = 0x1c;
	int rectadr;
	DWORD procID=0;
	RECT rect;
	
	int janwidth=0, janheight=0;

	procHandle = OpenProcess(PROCESS_ALL_ACCESS, false, reader.getProcessId());

	rectadr = reader.getMemIntValue(rectptr);

	//Oribarpos = reader.getMemIntValue(baradr);
	OrijanLeft = reader.getMemIntValue(rectadr+distLeft);
	OrijanTop = reader.getMemIntValue(rectadr+distTop);
	Orijanwidth = reader.getMemIntValue(rectadr+distWidth);
	Orijanheight = reader.getMemIntValue(rectadr+distHeight);
	bool captured = false;

	bool wait=false;
	while (!toolThreadShouldStop) {	
		
		Sleep(200);
		
		if (reader.getConnectionState()!=10)
			continue;
		
		if(tibiaHWND == NULL) {
			GetWindowThreadProcessId(GetForegroundWindow(), &procID);
			if((int)procID == reader.getProcessId())
				tibiaHWND = GetForegroundWindow(); 	
		}
		else {
			if(tibiaHWND != GetForegroundWindow())
				wait = true;
			else
				wait = false;
			if(wait)
				continue;
			
			GetWindowRect(tibiaHWND,&rect);

			if (!captured) {
				int nWidth = rect.right - rect.left;
				int mHeight = rect.bottom - rect.top;
				HDC hDDC = GetDC( tibiaHWND );
				HDC hCDC = CreateCompatibleDC(hDDC);
				HBITMAP hBitmap =CreateCompatibleBitmap(hDDC,nWidth,mHeight);
				SelectObject(hCDC,hBitmap); 
				BitBlt(hCDC,0,0,nWidth,mHeight,hDDC,0,0,SRCCOPY); 
				WriteBMPFile(hBitmap, "c:\\temp\\screencapture.bmp", hCDC);
				captured = true;
			}
			else {
				janwidth = rect.right - rect.left;
				janheight = rect.bottom - rect.top;
				
				//reader.setMemIntValue(baradr,0);
				reader.setMemIntValue((rectadr+distLeft),0);
				reader.setMemIntValue((rectadr+distTop),0);
				reader.setMemIntValue((rectadr+distHeight),janheight);
				reader.setMemIntValue((rectadr+distWidth),janwidth);
			}			
			Sleep(500);
		}		
	}	

	//back to original values
	reader.setMemIntValue(baradr,Oribarpos);
	reader.setMemIntValue((rectadr+distLeft),OrijanLeft);
	reader.setMemIntValue((rectadr+distTop),OrijanTop);
	reader.setMemIntValue((rectadr+distHeight),Orijanheight);
	reader.setMemIntValue((rectadr+distWidth),Orijanwidth);

	toolThreadShouldStop=0;

	return 0;
}

CMod_OSVApp::CMod_OSVApp()
{	
	m_started=0;	
}

CMod_OSVApp::~CMod_OSVApp()
{
	delete m_configData;	
}

char * CMod_OSVApp::getName()
{
	return "Screen-Only View";
}

int CMod_OSVApp::isStarted()
{
	return m_started;
}

void CMod_OSVApp::start()
{	
	superStart();
	DWORD threadId;
	toolThreadShouldStop=0;
	toolThreadHandle =  ::CreateThread(NULL,0,toolThreadProc,m_configData,0,&threadId);				
	m_started=1;
}

void CMod_OSVApp::stop()
{
	toolThreadShouldStop=1;

	while (toolThreadShouldStop) {
		Sleep(50);
	};

	m_started=0;		
} 

char *CMod_OSVApp::getVersion()
{
	return "1.0";
}

void WriteBMPFile(HBITMAP bitmap, LPTSTR filename, HDC hDC)
{
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

if (!GetObject( bitmap, sizeof(BITMAP), (LPSTR)&bmp)){
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
if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)){
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



