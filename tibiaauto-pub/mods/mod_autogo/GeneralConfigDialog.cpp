// GeneralConfigDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mod_autogo.h"
#include "GeneralConfigDialog.h"
#include <MemReader.h>
#include "WhiteList.h"
#include <ModuleUtil.h>
#include <MemReader.h>
#include <MemUtil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

static HWND tibiaHwnd = NULL;

void WriteBMPFileLoc(HBITMAP bitmap, CString filename, HDC hDC)
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

	if (!GetObject(bitmap, sizeof(BITMAP), (LPSTR)&bmp))
	{
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
	else
		cClrBits = 32;
	// Allocate memory for the BITMAPINFO structure.
	if (cClrBits != 24)
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
		                                sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
	else
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.

	pbmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth    = bmp.bmWidth;
	pbmi->bmiHeader.biHeight   = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes   = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color
	// indices and store the result in biSizeImage.
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8 * pbmi->bmiHeader.biHeight * cClrBits;
	// Set biClrImportant to 0, indicating that all of the
	// device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;

	// now open file and save the data
	pbih   = (PBITMAPINFOHEADER) pbmi;
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
	{
		AfxMessageBox("writeBMP::Could not allocate memory");
		return;
	}

	// Retrieve the color table (RGBQUAD array) and the bits
	if (!GetDIBits(hDC, HBITMAP(bitmap), 0, (WORD) pbih->biHeight, lpBits, pbmi,
	               DIB_RGB_COLORS))
	{
		AfxMessageBox("writeBMP::GetDIB error");
		return;
	}

	// Create the .BMP file.
	hf = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, (DWORD) 0,
	                NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
	                (HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE)
	{
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
	               (LPDWORD) &dwTmp, NULL))
	{
		AfxMessageBox("Could not write in to file");
		return;
	}

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
	               + pbih->biClrUsed * sizeof (RGBQUAD),
	               (LPDWORD) &dwTmp, (NULL)))
	{
		AfxMessageBox("Could not write in to file");
		return;
	}

	// Copy the array of color indices into the .BMP file.
	dwTotal = cb = pbih->biSizeImage;
	hp      = lpBits;
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp, NULL))
	{
		AfxMessageBox("Could not write in to file");
		return;
	}

	// Close the .BMP file.
	if (!CloseHandle(hf))
	{
		AfxMessageBox("Could not close file");
		return;
	}

	// Free memory.
	GlobalFree((HGLOBAL)lpBits);
}

CString capturePosition(CString name)
{
	CMemReader& reader = CMemReader::getMemReader();
	if (!tibiaHwnd)
	{
		tibiaHwnd = FindWindowEx(NULL, NULL, "TibiaClient", NULL);
		while (tibiaHwnd)
		{
			DWORD pid;
			DWORD dwThreadId = ::GetWindowThreadProcessId(tibiaHwnd, &pid);

			if (pid == CMemUtil::getGlobalProcessId())
				break;
			tibiaHwnd = FindWindowEx(NULL, tibiaHwnd, "TibiaClient", NULL);
		}
	}
	RECT rect;
	bool captured  = false;
	bool minimized = IsIconic(tibiaHwnd) != 0;
	bool trayed    = IsWindowVisible(tibiaHwnd) == 0;

	char path[1024];
	CModuleUtil::getInstallPath(path);
	time_t lTime;
	time(&lTime);
	char timeBuf[64];
	strftime(timeBuf, 64, " %a %d %b-%H%M(%S)", localtime(&lTime));
	CString filePath;
	filePath.Format("%s\\screenshots\\%s%s.bmp", path, name, timeBuf);

	while (!captured)
	{
		Sleep(100);
		if (!IsWindowVisible(tibiaHwnd))
		{
			ShowWindow(tibiaHwnd, SW_SHOW);
			continue;
		}
		if (IsIconic(tibiaHwnd))
		{
			ShowWindow(tibiaHwnd, SW_RESTORE);
			continue;
		}
		if (tibiaHwnd != GetForegroundWindow())
		{
			SetForegroundWindow(tibiaHwnd);
			continue;
		}
		Sleep(500);
		GetClientRect(tibiaHwnd, &rect);
		ClientToScreen(tibiaHwnd, (LPPOINT)&rect.left);
		ClientToScreen(tibiaHwnd, (LPPOINT)&rect.right);

		rect.right -= 200;
		int width = rect.right - rect.left;
		int posx  = (width / 2) - 102 + rect.left;
		rect.bottom -= 200;
		int height      = rect.bottom - rect.top;
		int posy        = (height / 2) - 85 + rect.top;
		HDC hDDC        = GetDC(GetDesktopWindow());
		HDC hCDC        = CreateCompatibleDC(hDDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(hDDC, 205, 175);
		SelectObject(hCDC, hBitmap);
		BitBlt(hCDC, 0, 0, 205, 175, hDDC, posx, posy, SRCCOPY);
		WriteBMPFileLoc(hBitmap, filePath, hCDC);
		captured = true;
		ReleaseDC(GetDesktopWindow(), hDDC);
		DeleteDC(hCDC);
	}
	if (minimized)
		ShowWindow(tibiaHwnd, SW_MINIMIZE);
	if (trayed)
		ShowWindow(tibiaHwnd, SW_HIDE);
	//CWnd::SetForegroundWindow();
	return filePath;
}

/////////////////////////////////////////////////////////////////////////////
// GeneralConfigDialog dialog


GeneralConfigDialog::GeneralConfigDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(GeneralConfigDialog::IDD, pParent)
{
	memset(memWhiteList, 0, 3200);
	memMkBlack = 0;
	//{{AFX_DATA_INIT(GeneralConfigDialog)
	lastX = 0;
	lastY = 0;
	lastZ = 0;
	//}}AFX_DATA_INIT
}

void GeneralConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GeneralConfigDialog)
	DDX_Control(pDX, IDC_RUNAWAY_PICTURE, m_runawayPicture);
	DDX_Control(pDX, IDC_START_PICTURE, m_startPicture);
	DDX_Control(pDX, IDC_MAINTAIN_POS, m_maintainPos);
	DDX_Control(pDX, IDC_AUTOGO_CUR_Y, m_curY);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_PROXIMITY_OPTIONS_FRAME, m_proximityFrame);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_POSITION_OPTIONS_FRAME, m_postionFrame);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_MESSAGE_OPTIONS_FRAME, m_messageFrame);
	DDX_Control(pDX, IDC_AUTOGO_TOSTART, m_SetStart);
	DDX_Control(pDX, IDC_AUTOGO_TORUNAWAY, m_SetRunaway);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_Z, m_runawayZ);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_Y, m_runawayY);
	DDX_Control(pDX, IDC_AUTOGO_RUNAWAY_X, m_runawayX);
	DDX_Control(pDX, IDC_AUTOGO_CUR_Z, m_curZ);
	DDX_Control(pDX, IDC_AUTOGO_CUR_X, m_curX);
	DDX_Control(pDX, IDC_AUTOGO_ACT_Z, m_actZ);
	DDX_Control(pDX, IDC_AUTOGO_ACT_Y, m_actY);
	DDX_Control(pDX, IDC_AUTOGO_ACT_X, m_actX);
	DDX_Control(pDX, IDC_AUTOGO_ACT_DIRECTION, m_actDirection);
	DDX_Control(pDX, IDC_AUTOGO_MESSAGE_IGNORE_SPELLS, m_ignoreSpells);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEPARANOIA, m_battleParanoia);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEANXIETY, m_battleAnxiety);
	DDX_Control(pDX, IDC_GENERAL_CONFIG_FRAME, m_GeneralConfigFrame);
	DDX_Control(pDX, IDC_AUTOGO_BATTLEWHITELIST, m_battleWhiteList);
	DDX_Text(pDX, IDC_AUTOGO_CUR_X, lastX);
	DDX_Text(pDX, IDC_AUTOGO_CUR_Y, lastY);
	DDX_Text(pDX, IDC_AUTOGO_CUR_Z, lastZ);
	DDX_Control(pDX, IDC_FLASHONALARM, m_flashOnAlarm);
	DDX_Control(pDX, IDC_AUTOGO_MODPRIORITY, m_modPriority);
	DDX_Control(pDX, IDC_AUTOGO_SCREENSHOT_TYPE, m_screenshotType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(GeneralConfigDialog, CDialog)
//{{AFX_MSG_MAP(GeneralConfigDialog)
ON_BN_CLICKED(IDC_AUTOGO_WHITELIST, OnAutogoWhitelist)
ON_BN_CLICKED(IDC_AUTOGO_BATTLEPARANOIA, OnAutogoBattleparanoia)
ON_BN_CLICKED(IDC_AUTOGO_BATTLEANXIETY, OnAutogoBattleanxiety)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_AUTOGO_TORUNAWAY, OnAutogoTorunaway)
ON_WM_ERASEBKGND()
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_AUTOGO_TOSTART, OnAutogoTostart)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GeneralConfigDialog message handlers

void GeneralConfigDialog::OnAutogoWhitelist()
{
	CWhiteList *dialog = new CWhiteList(memWhiteList, &memMkBlack);
	dialog->DoModal();
	delete dialog;
}

void GeneralConfigDialog::disableControls()
{
	m_battleWhiteList.EnableWindow(false);
	m_curX.EnableWindow(false);
	m_curY.EnableWindow(false);
	m_curZ.EnableWindow(false);
	m_runawayZ.EnableWindow(false);
	m_runawayY.EnableWindow(false);
	m_runawayX.EnableWindow(false);
	m_actZ.EnableWindow(false);
	m_actY.EnableWindow(false);
	m_actX.EnableWindow(false);
	m_actDirection.EnableWindow(false);
	m_battleAnxiety.EnableWindow(false);
	m_battleParanoia.EnableWindow(false);
	m_ignoreSpells.EnableWindow(false);
	m_SetRunaway.EnableWindow(false);
	m_SetStart.EnableWindow(false);
	m_maintainPos.EnableWindow(false);
	m_flashOnAlarm.EnableWindow(false);
	m_modPriority.EnableWindow(false);
	m_screenshotType.EnableWindow(false);
}

void GeneralConfigDialog::DoSetButtonSkin()
{
	skin.SetButtonSkin(m_battleWhiteList);
	skin.SetButtonSkin(m_SetStart);
	skin.SetButtonSkin(m_SetRunaway);
}

BOOL GeneralConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	m_actDirection.InsertString(0, "<None>");
	m_actDirection.InsertString(DIR_LEFT, "Left");
	m_actDirection.InsertString(DIR_RIGHT, "Right");
	m_actDirection.InsertString(DIR_UP, "Up");
	m_actDirection.InsertString(DIR_DOWN, "Down");

	SetTimer(1001, 250, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void GeneralConfigDialog::enableControls()
{
	m_battleWhiteList.EnableWindow(true);
	m_curX.EnableWindow(true);
	m_curY.EnableWindow(true);
	m_curZ.EnableWindow(true);
	m_runawayZ.EnableWindow(true);
	m_runawayY.EnableWindow(true);
	m_runawayX.EnableWindow(true);
	m_actZ.EnableWindow(true);
	m_actY.EnableWindow(true);
	m_actX.EnableWindow(true);
	m_actDirection.EnableWindow(true);
	m_battleAnxiety.EnableWindow(true);
	m_battleParanoia.EnableWindow(true);
	m_ignoreSpells.EnableWindow(true);
	m_SetRunaway.EnableWindow(true);
	m_SetStart.EnableWindow(true);
	m_maintainPos.EnableWindow(true);
	m_flashOnAlarm.EnableWindow(true);
	m_modPriority.EnableWindow(true);
	m_screenshotType.EnableWindow(true);
}

void GeneralConfigDialog::configToControls(CConfigData *configData)
{
	CString buf;
	memcpy(memWhiteList, configData->whiteList, 3200);
	buf.Format("%d", configData->actX);
	m_actX.SetWindowText(buf);
	buf.Format("%d", configData->actY);
	m_actY.SetWindowText(buf);
	buf.Format("%d", configData->actZ);
	m_actZ.SetWindowText(buf);
	m_actDirection.SetCurSel(configData->actDirection);
	buf.Format("%d", configData->runawayX);
	m_runawayX.SetWindowText(buf);
	buf.Format("%d", configData->runawayY);
	m_runawayY.SetWindowText(buf);
	buf.Format("%d", configData->runawayZ);
	m_runawayZ.SetWindowText(buf);
	m_ignoreSpells.SetCheck(configData->options & OPTIONS_IGNORE_SPELLS);
	m_battleParanoia.SetCheck(configData->options & OPTIONS_BATTLE_PARANOIA);
	m_battleAnxiety.SetCheck(configData->options & OPTIONS_BATTLE_ANXIETY);
	memMkBlack = configData->options & OPTIONS_MAKE_BLACKLIST;
	m_flashOnAlarm.SetCheck(configData->options & OPTIONS_FLASHONALARM);
	m_maintainPos.SetCheck(configData->maintainPos);
	m_modPriority.SetCurSel(atoi(configData->modPriorityStr) - 1);
	m_screenshotType.SetCurSel(configData->screenshotType);
}

void GeneralConfigDialog::controlsToConfig(CConfigData *newConfigData)
{
	CString buf;
	memcpy(newConfigData->whiteList, memWhiteList, 3200);
	m_actX.GetWindowText(buf);
	newConfigData->actX = atoi(buf);
	m_actY.GetWindowText(buf);
	newConfigData->actY = atoi(buf);
	m_actZ.GetWindowText(buf);
	newConfigData->actZ         = atoi(buf);
	newConfigData->actDirection = m_actDirection.GetCurSel();
	m_runawayX.GetWindowText(buf);
	newConfigData->runawayX = atoi(buf);
	m_runawayY.GetWindowText(buf);
	newConfigData->runawayY = atoi(buf);
	m_runawayZ.GetWindowText(buf);
	newConfigData->runawayZ = atoi(buf);

	newConfigData->options = 0;
	if (m_ignoreSpells.GetCheck())
		newConfigData->options |= OPTIONS_IGNORE_SPELLS;
	if (m_battleParanoia.GetCheck())
		newConfigData->options |= OPTIONS_BATTLE_PARANOIA;
	if (m_battleAnxiety.GetCheck())
		newConfigData->options |= OPTIONS_BATTLE_ANXIETY;
	if (memMkBlack)
		newConfigData->options |= OPTIONS_MAKE_BLACKLIST;
	if (m_flashOnAlarm.GetCheck())
		newConfigData->options |= OPTIONS_FLASHONALARM;
	newConfigData->maintainPos = m_maintainPos.GetCheck() != 0;
	sprintf(newConfigData->modPriorityStr, "%d", m_modPriority.GetCurSel() + 1);
	newConfigData->screenshotType = m_screenshotType.GetCurSel();
}

void GeneralConfigDialog::OnAutogoBattleparanoia()
{
	m_battleAnxiety.EnableWindow(!m_battleParanoia.GetCheck());
}

void GeneralConfigDialog::OnAutogoBattleanxiety()
{
	m_battleParanoia.EnableWindow(!m_battleAnxiety.GetCheck());
}

void GeneralConfigDialog::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1001)
	{
		CMemReader& reader = CMemReader::getMemReader();
		CTibiaCharacter self;
		reader.readSelfCharacter(&self);
		CString buf;
		if (lastX != self.x || lastY != self.y || lastZ != self.z)
		{
			buf.Format("%d", self.x);
			m_curX.SetWindowText(buf);
			buf.Format("%d", self.y);
			m_curY.SetWindowText(buf);
			buf.Format("%d", self.z);
			m_curZ.SetWindowText(buf);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void GeneralConfigDialog::OnAutogoTorunaway()
{
	CString buf;
	m_curX.GetWindowText(buf);
	m_runawayX.SetWindowText(buf);
	m_curY.GetWindowText(buf);
	m_runawayY.SetWindowText(buf);
	m_curZ.GetWindowText(buf);
	m_runawayZ.SetWindowText(buf);
	if (RunawayBMP.GetLength())
		remove(RunawayBMP);
	RunawayBMP = capturePosition("RunawayPosition");
	m_runawayPicture.SetBitmap((HBITMAP)::LoadImage(NULL, RunawayBMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	m_runawayPicture.Invalidate();
}

void GeneralConfigDialog::OnAutogoTostart()
{
	CString buf;
	m_curX.GetWindowText(buf);
	m_actX.SetWindowText(buf);
	m_curY.GetWindowText(buf);
	m_actY.SetWindowText(buf);
	m_curZ.GetWindowText(buf);
	m_actZ.SetWindowText(buf);
	if (StartBMP.GetLength())
		remove(StartBMP);
	StartBMP = capturePosition("StartPosition");
	m_startPicture.SetBitmap((HBITMAP)::LoadImage(NULL, StartBMP, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
	m_startPicture.Invalidate();
}
