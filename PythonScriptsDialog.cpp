// PythonScriptsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "PythonScriptsDialog.h"
#include "PythonScript.h"
#include "PythonEngine.h"
#include "ScriptConfigDialg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int pythonInterpreterRunning=0;

/////////////////////////////////////////////////////////////////////////////
// CPythonScriptsDialog dialog


CPythonScriptsDialog::CPythonScriptsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CPythonScriptsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPythonScriptsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT	
}


void CPythonScriptsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPythonScriptsDialog)
	DDX_Control(pDX, IDC_FUNLIST, m_funlist);
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPythonScriptsDialog, CDialog)
	//{{AFX_MSG_MAP(CPythonScriptsDialog)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_NOTIFY(NM_RCLICK, IDC_FUNLIST, OnRclickFunlist)
	ON_BN_CLICKED(IDC_RUN_INTERPRETER, OnRunInterpreter)
	ON_BN_CLICKED(IDC_LOAD_SCRIPT, OnLoadScript)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPythonScriptsDialog message handlers

void CPythonScriptsDialog::OnOK() 
{
	ShowWindow(SW_HIDE);
}

void CPythonScriptsDialog::OnClose() 
{	
	ShowWindow(SW_HIDE);
}

void CPythonScriptsDialog::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==1001)
	{
		KillTimer(1001);

		CPythonEngine pythonEngine;
		pythonEngine.enterCriticalSection();
		refreshLoadedScripts();
		pythonEngine.leaveCriticalSection();

		SetTimer(1001,500,NULL);
	}
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CPythonScriptsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_list.InsertColumn(0,"Name",LVCFMT_LEFT,70);
	m_list.InsertColumn(1,"Version",LVCFMT_LEFT,50);
	m_list.InsertColumn(2,"Functions",LVCFMT_LEFT,70);
	m_list.InsertColumn(3,"Enabled",LVCFMT_LEFT,70);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	m_funlist.InsertColumn(0,"Type",LVCFMT_LEFT,40);
	m_funlist.InsertColumn(1,"Name",LVCFMT_LEFT,70);
	m_funlist.InsertColumn(2,"Period [ms]",LVCFMT_LEFT,60);
	m_funlist.InsertColumn(3,"Next [ms]",LVCFMT_LEFT,60);
	m_funlist.InsertColumn(4,"Last [ms]",LVCFMT_LEFT,60);
	m_funlist.SetExtendedStyle(m_list.GetExtendedStyle()|LVS_EX_FULLROWSELECT);

	refreshLoadedScripts();

	funListMenu = new CMenu();	
	funListMenu ->LoadMenu(IDR_PYTHON_SCRIPTS_FUNLIST_MENU);	

	modListMenu = new CMenu();	
	modListMenu ->LoadMenu(IDR_PYTHON_SCRIPTS_MENU);	

	SetTimer(1001,500,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPythonScriptsDialog::refreshLoadedScripts()
{		
	int i;
	int visibleScripts[1000];
	int foundScripts[1000];
	for (i=0;i<1000;i++) 
	{
		visibleScripts[i]=-1;
		foundScripts[i]=0;
	}

	for (i=0;i<m_list.GetItemCount();i++)
	{
		int data=m_list.GetItemData(i);
		visibleScripts[data]=i;
	}
	for (i=0;;i++)
	{		
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(i);
		if (!pythonScript) break;		

		if (visibleScripts[i]==-1)
		{
			int newpos=m_list.GetItemCount();
			m_list.InsertItem(newpos,"");
			m_list.SetItemData(newpos,i);
			visibleScripts[i]=newpos;
		}

		foundScripts[i]=1;

		int actpos=visibleScripts[i];
		
		char buf[128],buf2[128];		
		

		sprintf(buf,"%s",pythonScript->getName());
		memset(buf2,0,128);
		m_list.GetItemText(actpos,0,buf2,127);
		if (strcmp(buf,buf2)) m_list.SetItemText(actpos,0,buf);

		sprintf(buf,"%s",pythonScript->getVersion());
		memset(buf2,0,128);
		m_list.GetItemText(actpos,1,buf2,127);
		if (strcmp(buf,buf2)) m_list.SetItemText(actpos,1,buf);

		int c=0;
		while (pythonScript->getFunDef(c)) c++;
		sprintf(buf,"%d",c);
		memset(buf2,0,128);
		m_list.GetItemText(actpos,2,buf2,127);
		if (strcmp(buf,buf2)) m_list.SetItemText(actpos,2,buf);

		m_list.SetItemText(actpos,3,pythonScript->isEnabled()?"yes":"no");
	}

	for (i=m_list.GetItemCount()-1;i>=0;i--)
	{
		int scriptNr = m_list.GetItemData(i);
		if (!foundScripts[scriptNr])
		{
			m_list.DeleteItem(scriptNr);
		}
	}
	// and refresh fun list
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos)
	{			
		int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));
				
		refreshModuleFunctions(scriptNr);
	} else {
		m_funlist.DeleteAllItems();
	}	
}

void CPythonScriptsDialog::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos)
	{			
		int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));
				
		refreshModuleFunctions(scriptNr);
	} else {
		m_funlist.DeleteAllItems();
	}
	
	*pResult = 0;
}

void CPythonScriptsDialog::refreshModuleFunctions(int scriptNr)
{				
	static int lastScriptNr=-1;
	CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
	
	if (scriptNr!=lastScriptNr||pythonScript==NULL)
	{
		m_funlist.DeleteAllItems();
	}
	// in case when a plugin has been unloaded
	if (!pythonScript) return;

	lastScriptNr=scriptNr;
	
	int funNr;
	for (funNr=0;;funNr++)
	{		
		char buf[128];

		struct funType *fun = pythonScript->getFunDef(funNr);
		if (!fun) break;

		if (funNr>=m_funlist.GetItemCount())
		{
			m_funlist.InsertItem(funNr,"");
		}

		switch (fun->type)
		{
		case FUNTYPE_PERIODICAL: sprintf(buf,"period");break;
		case FUNTYPE_MSG: sprintf(buf,"msg");break;
		default: sprintf(buf,"unknown");break;
		}		
		m_funlist.SetItemText(funNr,0,buf);

		sprintf(buf,"%s",fun->name);
		m_funlist.SetItemText(funNr,1,buf);

		sprintf(buf,"%d",fun->interval);
		m_funlist.SetItemText(funNr,2,buf);

		if (fun->tmNextExec&&pythonScript->isEnabled())
		{
			sprintf(buf,"%d",(fun->tmNextExec-GetTickCount()));
			m_funlist.SetItemText(funNr,3,buf);
		} else {
			m_funlist.SetItemText(funNr,3,"-");
		}

		if (fun->tmLastExec)
		{
			sprintf(buf,"%d",(GetTickCount()-fun->tmLastExec));
			m_funlist.SetItemText(funNr,4,buf);
		} else {
			m_funlist.SetItemText(funNr,4,"-");
		}

	}
	
}

void CPythonScriptsDialog::OnRclickFunlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_funlist.GetFirstSelectedItemPosition();
	if (pos)
	{
		/* Get the mouse cursor position */
		DWORD dwPos = GetMessagePos();

		/* Convert the co-ords into a CPoint structure */
		CPoint pt( dwPos&0xffff , (dwPos>>16)&0xffff );
	
		funListMenu->GetSubMenu(0)->TrackPopupMenu(0,pt.x,pt.y,this);
	
		*pResult = 0;
	}
}

BOOL CPythonScriptsDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{	
	if (wParam==ID_PYTHONSCRIPTSFUNLISTMENU_CALL)
	{					
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		if (pos)
		{			
			int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));		
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
			
			pos = m_funlist.GetFirstSelectedItemPosition();
			if (pos)
			{						
				PyGILState_STATE gstate;
				gstate = PyGILState_Ensure();
				int funNr=m_funlist.GetItemData(m_funlist.GetNextSelectedItem(pos));
				struct funType *fun = pythonScript->getFunDef(funNr);								
				PyObject *params = pythonScript->getParamsDic();								
				PyObject *result = PyObject_CallMethod(pythonScript->getPluginObject(), fun->name,"(O)",params);
				Py_XDECREF(result);
				Py_XDECREF(params);
				fun->call();				
				PyGILState_Release(gstate);
			}
		}
	}
	if (wParam==ID_MENU_UNLOAD)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		if (pos)
		{			
			int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));
			CPythonEngine pythonEngine;
			pythonEngine.unloadScript(scriptNr);			
		}
	}
	if (wParam==ID_MENU_RELOAD)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		if (pos)
		{			
			int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));
			char fileName[1024];
			strcpy(fileName,CPythonScript::getScriptByNr(scriptNr)->getFileName());
			CPythonEngine pythonEngine;
			pythonEngine.unloadScript(scriptNr);
			pythonEngine.loadScript(fileName);
		}
	}
	if (wParam==ID_MENU_CONFIGOPTIONS)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		if (pos)
		{			
			int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));
			CScriptConfigDialg *scriptConfigDialog = new CScriptConfigDialg(scriptNr);
			scriptConfigDialog->DoModal();

		}
	}
	if (wParam==ID_MENU_ENABLE)
	{
		POSITION pos = m_list.GetFirstSelectedItemPosition();
		if (pos)
		{			
			int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));
			CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);
			pythonScript->setEnabled(!pythonScript->isEnabled());			
			if (pythonScript->isEnabled()) 
			{
				modListMenu->GetSubMenu(0)->CheckMenuItem(ID_MENU_ENABLE,MF_BYCOMMAND|MF_CHECKED);		
			} else {
				modListMenu->GetSubMenu(0)->CheckMenuItem(ID_MENU_ENABLE,MF_BYCOMMAND|MF_UNCHECKED);		
			}			
		}
	}
	
	
	return CDialog::OnCommand(wParam, lParam);
}

void CPythonScriptsDialog::OnRunInterpreter() 
{		
	if (!pythonInterpreterRunning)
	{		
		pythonInterpreterRunning=1;
		PyGILState_STATE gstate;
		gstate = PyGILState_Ensure();

		PyRun_SimpleString("import idlelib.PyShell");
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.argv=['tibiaauto','-n']");
		PyRun_SimpleString("import idlelib.PyShell");
		PyRun_SimpleString("idlelib.PyShell.main()");			

		PyGILState_Release(gstate);		
		pythonInterpreterRunning=0;
	} 
}

void CPythonScriptsDialog::OnLoadScript() 
{
	
	char szFilters[]=
      "Python script (*.py)|*.py|All Files (*.*)|*.*||";

	CFileDialog fd(true,"","",OFN_FILEMUSTEXIST, szFilters, this);
	if (fd.DoModal()==IDOK)
	{
		CString pathName = fd.GetPathName();         
				
		CPythonEngine pythonEngine;	
		pythonEngine.loadScript(pathName.GetBuffer(200));
	}	
}

void CPythonScriptsDialog::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	if (pos)
	{
		int scriptNr=m_list.GetItemData(m_list.GetNextSelectedItem(pos));
		CPythonScript *pythonScript = CPythonScript::getScriptByNr(scriptNr);

		/* Get the mouse cursor position */
		DWORD dwPos = GetMessagePos();

		/* Convert the co-ords into a CPoint structure */
		CPoint pt( dwPos&0xffff , (dwPos>>16)&0xffff );

		if (pythonScript->isEnabled())
		{
			modListMenu->GetSubMenu(0)->CheckMenuItem(ID_MENU_ENABLE,MF_BYCOMMAND|MF_CHECKED);		
		} else {
			modListMenu->GetSubMenu(0)->CheckMenuItem(ID_MENU_ENABLE,MF_BYCOMMAND|MF_UNCHECKED);		
		}
		modListMenu->GetSubMenu(0)->TrackPopupMenu(0,pt.x,pt.y,this);
		
		*pResult = 0;
	}
}
