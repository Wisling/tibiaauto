// CharDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "CharDialog.h"
#include "MemUtil.h"
#include <MemReader.h>
#include <TibiaItem.h>
#include "windows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // ifdef _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCharDialog dialog


CCharDialog::CCharDialog(CWnd* pParent /*=NULL*/)
	: MyDialog(CCharDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCharDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CCharDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharDialog)
	DDX_Control(pDX, IDCANCEL, m_Exit);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_CHAR_LIST, m_charList);
	DDX_Control(pDX, IDC_TOOL_INJECTMC, m_InjectMC);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCharDialog, CDialog)
//{{AFX_MSG_MAP(CCharDialog)
ON_BN_CLICKED(IDCANCEL, OnCancel)
ON_BN_CLICKED(IDC_TOOL_INJECTMC, OnToolInjectmc)
ON_WM_TIMER()
ON_WM_ERASEBKGND()
ON_WM_DRAWITEM()
ON_WM_CTLCOLOR()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharDialog message handlers

void CCharDialog::OnCharRefresh()
{
	CMemReader& reader = CMemReader::getMemReader();
	char buf[256];
	//int procFound[65536];
	int *procFound = new int[65536];
	memset(procFound, 0x00, sizeof(int) * 65536);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE procSnapshortHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	Process32First(procSnapshortHandle, &procEntry);
	do
	{
		int pos, len = strlen(procEntry.szExeFile);
		for (pos = 0; pos < len; pos++)
			procEntry.szExeFile[pos] = (char)tolower(procEntry.szExeFile[pos]);
		if (!strcmp(procEntry.szExeFile, "tibia.exe"))
		{
			char loggedCharName[65];
			reader.GetLoggedChar(procEntry.th32ProcessID, loggedCharName, sizeof(loggedCharName));

			sprintf(buf, "[%5d] %s", procEntry.th32ProcessID, loggedCharName);
			if (m_charList.FindStringExact(-1, buf) == -1)
			{
				m_charList.AddString(buf);
				m_charList.SetItemData(m_charList.FindStringExact(-1, buf), procEntry.th32ProcessID);
				m_charList.SetCurSel(0);
			}

			procFound[procEntry.th32ProcessID] = 1;
		}
	}
	while (Process32Next(procSnapshortHandle, &procEntry));


	int i;

	for (i = m_charList.GetCount() - 1; i >= 0; i--)
	{
		if (!procFound[m_charList.GetItemData(i)])
			m_charList.DeleteString(i);
	};

	CloseHandle(procSnapshortHandle);
	delete []procFound;
}

void CCharDialog::OnOK()
{
	int sel = m_charList.GetCurSel();

	if (sel == -1)
	{
		AfxMessageBox("You must select a character!");
	}
	else
	{
		int itemData     = m_charList.GetItemData(sel);
		long m_processId = itemData;

		EndDialog(m_processId);
	}
}

BOOL CCharDialog::OnInitDialog()
{
	// wis: add large set of checks for all operations needed to be performed by TA
	CDialog::OnInitDialog();
	skin.SetButtonSkin(m_OK);
	skin.SetButtonSkin(m_Exit);
	skin.SetButtonSkin(m_InjectMC);
	OnCharRefresh();

	SetTimer(1001, 250, NULL);

	return TRUE;
}

void CCharDialog::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case 1001:
	{
		KillTimer(1001);
		OnCharRefresh();
		SetTimer(1001, 250, NULL);
		break;
	}
	}
	CDialog::OnTimer(nIDEvent);
}

void CCharDialog::OnCancel()
{
	
	CTibiaItem::cleanup();
	EndDialog(-1);
}

int copyFile(CString inpath, CString outpath)
{
	//inpath and outpath cannot be the same
	// returns 0 if it did not succeed, non-zero if it succeeded
	int MAXREAD = 10000;
	if (inpath.CompareNoCase(outpath) != 0)
	{
		char* filedata;
		FILE* fin = fopen(inpath, "rb");
		if (!fin)
		{
			AfxMessageBox("Cannot read input file!");
			return 0;
		}
		fseek(fin, 0, SEEK_END);
		int size = ftell(fin);
		fseek(fin, 0, SEEK_SET);
		filedata = (char*)malloc(size);
		int loc = 0;
		while (1)
		{
			int readlength = fread((char*)(filedata + loc), 1, MAXREAD, fin);
			loc += readlength;
			if (!readlength)
				break;
		}
		fclose(fin);
		if (!loc)
		{
			AfxMessageBox("Cannot read input file!");
			delete filedata;
			return 0;
		}

		FILE* fout = fopen(outpath, "wb");
		if (!fout)
		{
			AfxMessageBox("Cannot write new file!");
			delete filedata;
			return 0;
		}
		fwrite(filedata, 1, size, fout);

		fclose(fout);
		delete filedata;
	}
	else
	{
		AfxMessageBox("Input file and output file cannot be the same file.");
		return 0;
	}
	return 1;
}

void getVersion(CString& pathName, CString& versionNum)
{
	versionNum.SetString("Unknown Version");
	DWORD  verHandle = NULL;
	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(pathName, &verHandle);

	if (verSize != NULL)
	{
		LPSTR verData = new char[verSize];

		if (GetFileVersionInfo(pathName, verHandle, verSize, verData))
		{
			if (VerQueryValue(verData, "\\", (VOID FAR* FAR*)&lpBuffer, &size))
			{
				if (size)
				{
					VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
					if (verInfo->dwSignature == 0xfeef04bd)
					{

						// Doesn't matter if you are on 32 bit or 64 bit,
						// DWORD is always 32 bits, so first two revision numbers
						// come from dwFileVersionMS, last two come from dwFileVersionLS
						versionNum.Format("%d.%d%d",
							(verInfo->dwFileVersionMS >> 16) & 0xffff,
							(verInfo->dwFileVersionMS >> 0) & 0xffff,
							(verInfo->dwFileVersionLS >> 16) & 0xffff
							);
					}
				}
			}
		}
		delete[] verData;
	}
}

void CCharDialog::OnToolInjectmc()
{
	
	char szFilters[] =
		"Tibia client (Tibia.exe)|Tibia.exe|Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||";

	// Try and get Tibia Program Files path if available
	char tibiaPath[1024];
	unsigned long tibiaPathLen = 1023;
	tibiaPath[0] = '\0';
	HKEY hkey = NULL;
	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Tibia Auto\\", 0, KEY_READ, &hkey))
	{
		RegQueryValueEx(hkey, TEXT("TibiaClientHome"), NULL, NULL, (unsigned char *)tibiaPath, &tibiaPathLen);
		RegCloseKey(hkey);
	}

	//Ask for location of file to patch
	CFileDialog fd(true, "tibia", "tibia.exe", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters, this);
	fd.m_ofn.lpstrInitialDir = tibiaPath; // blank path defaults to current directory
	if (fd.DoModal() == IDOK)
	{
		CString pathName = fd.GetPathName();
		CString versionNum;
		getVersion(pathName, versionNum);
		int msgboxID = IDOK;

		// Load exe file into memory
		FILE* fout = NULL;
		while (fout == NULL && msgboxID == IDOK)
		{
			fout = fopen(pathName, "r+b");
			if(!fout)
				msgboxID = MessageBox("The file is currently being used or cannot be edited.\nPlease check your permissions and close any applications using the file and click OK.", "Cannot edit file", MB_OKCANCEL);
		}
		if (fout)
		{
			// BYTE MCText[3] = { 0xD7, 0xD3, 0xB7 };
			fseek(fout, 0, SEEK_END);
			size_t filesize = ftell(fout);
			fseek(fout, 0, SEEK_SET);
			if (filesize < 50000000){
				BYTE* tibiacontents = (BYTE*)malloc(filesize);
				fread(tibiacontents, filesize, 1, fout);

				// Search for MCText within file
				BYTE* tibiacontents_ind = tibiacontents;
				bool isFound = FALSE;
				while (!isFound && size_t(tibiacontents_ind - tibiacontents) < filesize-3)
				{
					if (tibiacontents_ind[0] == (BYTE)0xD7)
						if (tibiacontents_ind[1] == (BYTE)0x3D)
							if (tibiacontents_ind[2] == (BYTE)0xB7 || tibiacontents_ind[2] == (BYTE)0xFF)
							{
								isFound = TRUE;
								tibiacontents_ind+=2;
								break;
							}
					tibiacontents_ind++;
				}
				if (isFound){
					if (tibiacontents_ind[0] == (BYTE)0xFF)
					{
						AfxMessageBox("This file is already patched for using multiple clients.");
					}
					else // Patch file
					{
						CString outpath = pathName.Mid(0, pathName.GetLength() - 4);
						outpath.Append(".original.exe");
						if (copyFile(pathName, outpath))
						{
							unsigned char val = 0xff;
							fseek(fout, tibiacontents_ind - tibiacontents, SEEK_SET);
							fwrite(&val, 1, 1, fout);
							char buf[256];
							sprintf(buf, "This file has been successfully patched for multi-clienting.\nA backup has been saved in the same directory.");
							AfxMessageBox(buf);
						}
						else
						{
							char buf[2048];
							sprintf(buf, "The destination backup file cannot be accessed\n%s\nPlease close applications using the file or check that you have permissions to make edits to this folder.",outpath);
							AfxMessageBox(buf);
						}
					}
				}
				else
				{
					char msg[256];
					sprintf(msg, "The file does not seem to have a multi-clienting address.\nPlease ensure the file is a valid Tibia client.", versionNum);
					AfxMessageBox(msg);
				}
				
				delete[] tibiacontents;

				fclose(fout);
				//AfxMessageBox("Unable to modify client to MC mode!");
			}
		}
	}
}