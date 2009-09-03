// CharDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "CharDialog.h"
#include "MemUtil.h"
#include "MemReaderProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
	DDX_Control(pDX, IDC_CHAR_LIST, m_charList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharDialog, CDialog)
	//{{AFX_MSG_MAP(CCharDialog)
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
	CMemReaderProxy reader;
	char buf[256];
	//int procFound[65536];
	int *procFound = new int[65536];
	memset(procFound,0x00,sizeof(int)*65536);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize=sizeof(PROCESSENTRY32);
	HANDLE procSnapshortHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	
		
	
	Process32First(procSnapshortHandle,&procEntry);
	
	do
	{
		int pos,len=strlen(procEntry.szExeFile);
		for (pos=0;pos<len;pos++)
			procEntry.szExeFile[pos]=(char)tolower(procEntry.szExeFile[pos]);		
		if (!strcmp(procEntry.szExeFile,"tibia.exe"))
		{
			char *loggedCharName=reader.GetLoggedChar(procEntry.th32ProcessID);
			sprintf(buf,"[%5d] %s",procEntry.th32ProcessID,loggedCharName);
			free(loggedCharName);
			if (m_charList.FindString(0,buf)==-1)
			{		
				m_charList.AddString(buf);				
				m_charList.SetItemData(m_charList.FindString(0,buf),procEntry.th32ProcessID);
				m_charList.SetCurSel(0);
			}

			procFound[procEntry.th32ProcessID]=1;
		}		
	} while (Process32Next(procSnapshortHandle,&procEntry));

	
	int i;
	
	for (i=m_charList.GetCount()-1;i>=0;i--)
	{				
		if (!procFound[m_charList.GetItemData(i)])
		{			
			m_charList.DeleteString(i);
		}
	};	
	
	CloseHandle(procSnapshortHandle);	
	delete []procFound;
}

void CCharDialog::OnOK() 
{
	int sel=m_charList.GetCurSel();
	
	if (sel==-1)
	{
		AfxMessageBox("You must select a character!");
	} else {
		int itemData = m_charList.GetItemData(sel);	
		long m_processId=itemData;			
	
		EndDialog(m_processId);
	}
}

BOOL CCharDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnCharRefresh();
	
	SetTimer(1001,250,NULL);

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
			SetTimer(1001,250,NULL);
		}
		break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CCharDialog::OnCancel() 
{
	EndDialog(-1);		
}
