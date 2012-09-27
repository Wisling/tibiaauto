// LoadedModules.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "LoadedModules.h"
#include "ModuleProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CLoadedModules dialog


CLoadedModules::CLoadedModules(CWnd* pParent /*=NULL*/)
	: MyDialog(CLoadedModules::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadedModules)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	moduleActionsMenu=NULL;

}

CLoadedModules::~CLoadedModules(){
	if (moduleActionsMenu) delete moduleActionsMenu;
}
void CLoadedModules::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadedModules)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_LOADEDMODULES_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadedModules, CDialog)
	//{{AFX_MSG_MAP(CLoadedModules)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_LOADEDMODULES_LIST, OnRclickLoadedmodulesList)
	ON_WM_ERASEBKGND()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadedModules message handlers

void CLoadedModules::OnClose() 
{
	ShowWindow(SW_HIDE);
}

void CLoadedModules::OnOK() 
{
	ShowWindow(SW_HIDE);
}

void CLoadedModules::DoSetButtonSkin(){
	skin.SetButtonSkin(m_ok);
}

BOOL CLoadedModules::OnInitDialog() 
{
	CDialog::OnInitDialog();
	DoSetButtonSkin();

	m_list.InsertColumn(0,"name",LVCFMT_LEFT,70);
	m_list.InsertColumn(1,"loaded",LVCFMT_LEFT,50);
	m_list.InsertColumn(2,"description",LVCFMT_LEFT,100);
	m_list.InsertColumn(3,"version",LVCFMT_LEFT,50);
	m_list.InsertColumn(4,"started",LVCFMT_LEFT,50);
	

	m_list.SetExtendedStyle(m_list.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	
	refreshModulesInformation();

	SetTimer(1001,500,NULL);

	moduleActionsMenu = new CMenu();	
	moduleActionsMenu->LoadMenu(IDR_LOADED_MODULES_MENU);	
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoadedModules::refreshModulesInformation()
 {	
 	int i;
 	//m_list.DeleteAllItems();	
 	for (i=0;i<CModuleProxy::allModulesCount;i++)
 	{
 		char buf[128],buf2[128];
 		if (i>=m_list.GetItemCount())
 			m_list.InsertItem(i,"");
 
 		sprintf(buf,"%s",CModuleProxy::allModules[i]->getModuleName());
 		memset(buf2,0,128);
 		m_list.GetItemText(i,0,buf2,127);
 		if (strcmp(buf,buf2)) m_list.SetItemText(i,0,buf);
 
 		sprintf(buf,"%s",CModuleProxy::allModules[i]->isLoaded()?"yes":"no");
 		memset(buf2,0,128);
 		m_list.GetItemText(i,1,buf2,127);
 		if (strcmp(buf,buf2)) m_list.SetItemText(i,1,buf);
 
 		sprintf(buf,"%s",CModuleProxy::allModules[i]->getName());
 		memset(buf2,0,128);
 		m_list.GetItemText(i,2,buf2,127);
 		if (strcmp(buf,buf2)) m_list.SetItemText(i,2,buf);
 
 		sprintf(buf,"%s",CModuleProxy::allModules[i]->getVersion());
 		memset(buf2,0,128);
 		m_list.GetItemText(i,3,buf2,127);
 		if (strcmp(buf,buf2)) m_list.SetItemText(i,3,buf);
 
 		sprintf(buf,"%s",CModuleProxy::allModules[i]->isStarted()?"yes":"no");
 		memset(buf2,0,128);
 		m_list.GetItemText(i,4,buf2,127);
 		if (strcmp(buf,buf2)) m_list.SetItemText(i,4,buf);
 		
 	}	
 }

void CLoadedModules::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1001)
	{
		KillTimer(1001);

		
		refreshModulesInformation();
		

		SetTimer(1001,500,NULL);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CLoadedModules::OnRclickLoadedmodulesList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/* Get the mouse cursor position */
	DWORD dwPos = GetMessagePos();

	/* Convert the co-ords into a CPoint structure */
	CPoint pt( dwPos&0xffff , (dwPos>>16)&0xffff );

	moduleActionsMenu->GetSubMenu(0)->TrackPopupMenu(0,pt.x,pt.y,this);
	
	*pResult = 0;
}

BOOL CLoadedModules::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==ID_STOPMODULE_STOPMODULE||wParam==ID_STOPMODULE_STARTMODULE||wParam==ID_MODULEACTIONS_SHOWCONFIGDIALOG)
	{			
		int modStartFail=0;
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		while (pos)
		{			
			int modNr=m_list.GetNextSelectedItem(pos);
			IModuleInterface *module=CModuleProxy::allModules[modNr];
			switch (wParam)
			{
			case ID_STOPMODULE_STOPMODULE:
				module->stop();
				module->enableControls();
				break;
			case ID_STOPMODULE_STARTMODULE:
				module->controlsToConfig();
				if (module->validateConfig(0))
				{					
					module->disableControls();
					module->start();
				} else {
					modStartFail=1;
				}				
				break;
			case ID_MODULEACTIONS_SHOWCONFIGDIALOG:
				module->showConfigDialog();
				break;
			}
			
		}
		if (modStartFail)
		{
			AfxMessageBox("At least one module failed to start!");
		}
	}
	if (wParam==ID_MODULEACTIONS_LOADCUSTOMMODULE)
	{
		char szFilters[]=
			"Module dll (*.dll)|*.dll|All Files (*.*)|*.*||";
		
		CFileDialog fd(true,"dll","*.dll",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
		if (fd.DoModal()==IDOK)
		{
			CString pathName = fd.GetPathName();    
			new CModuleProxy(pathName.GetBuffer(128),1);
		}
		
		
	}
	
	return CDialog::OnCommand(wParam, lParam);
}


void CLoadedModules::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DoSetButtonSkin();
	
	// TODO: Add your message handler code here
	CDialog::OnPaint();
	// Do not call CDialog::OnPaint() for painting messages
}
