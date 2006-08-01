// tibiaauto.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "tibiaauto.h"
#include "tibiaautoDlg.h"
#include "afxsock.h"
#include "md5class.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int tprogrammingEdition=0;

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoApp

BEGIN_MESSAGE_MAP(CTibiaautoApp, CWinApp)
	//{{AFX_MSG_MAP(CTibiaautoApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoApp construction

volatile char *checksum="d5d680328fe4da96d6ba7a29e65b04a6";

CTibiaautoApp::CTibiaautoApp()

{	
	/*
	char *fileBuf=(char *)malloc(1000000);	
	int ep=0x1000;
	int start=0x400000+ep;	
	int len=0x406EF6-start;
	unsigned long int lenRead=0;
	ReadProcessMemory((HANDLE)-1,(void *)start,fileBuf,len,&lenRead);
	CMD5 md5;	
	if (strcmpi(md5.getMD5Digest(fileBuf,len),(char *)checksum))
	{
		FILE *f1=fopen("checksum.txt","r");
		if (f1)
		{
			fclose(f1);
			FILE *f=fopen("checksum.txt","w+");
			fprintf(f,"%s\n",md5.getMD5Digest(fileBuf,len));
			fclose(f);
		}		
		//ExitProcess(0);
	} 
	free(fileBuf);	
	*/
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CTibiaautoApp object

CTibiaautoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTibiaautoApp initialization

BOOL CTibiaautoApp::InitInstance()
{
	struct dllEntry {
		char *md5result;
		char *dllName;
	} dllEntryTab[] = {

{"1445BBF043ECD0A8FD7D5619B05BAD33","mods\\alice.dll"},
{"7F181275CA4F521565B6B693958ABDAC","mods\\mod_aim.dll"},
{"191205FC904BCBD92BDCAE3B7F7DFE6B","mods\\mod_antylogout.dll"},
{"FAE988309FFCC9B4C00CFC18798C1421","mods\\mod_autogo.dll"},
{"C520ED5D0AC959BCD5515D4FC8161D4A","mods\\mod_cavebot.dll"},
{"7C4A784416D434BB106B87F51B597140","mods\\mod_creatureinfo.dll"},
{"42C13A2D5842169E63151F2894C6D318","mods\\mod_eater.dll"},
{"2F78E6B4BE9E6A4410CE331526931883","mods\\mod_fisher.dll"},
{"7CFC437F97BD26F59F87C02EE7EC5961","mods\\mod_fluid.dll"},
{"A9CF95A26033ABC0857776BC1E965C10","mods\\mod_fps.dll"},
{"FDD7D1D578CF328C76EDA5ED8F5A5F40","mods\\mod_grouping.dll"},
{"8615A55ACB8D19B9D9A93627170A1167","mods\\mod_itemconfig.dll"},
{"7F378FEE7A59620DE3F58F13CC5D009A","mods\\mod_light.dll"},
{"3D5FB3AE829BE6447521F4504573C6A3","mods\\mod_looter.dll"},
{"22CDE8215BA45B02E0FE4CD58B2B169A","mods\\mod_maphack.dll"},
{"713E28E0A952C885A1923BAFFC332E84","mods\\mod_memdebug.dll"},
{"04495CFB6EA53E09B135158AE5239D6C","mods\\mod_monstershow.dll"},
{"A627AE97CDCCF8DC55C38AD630D71832","mods\\mod_playerinfo.dll"},
{"3947F2ADF35A9D7B49BDE4A566A65260","mods\\mod_responder.dll"},
{"F453E0B6F94C56ABA7A24CEA1359F475","mods\\mod_restack.dll"},
{"74C6400FE63A70D766E8B8922B1FFE55","mods\\mod_runemaker.dll"},
{"58D0690F81A111659A88DAE72BD09607","mods\\mod_showmap.dll"},
{"DB82B56EAFA2D615AA434B0105213CB6","mods\\mod_spellcaster.dll"},
{"4288BAF72363322D8C4DCBAA1A4644C4","mods\\mod_team.dll"},
{"67648A2CC373AD6A0C6B62276D5CD3B7","mods\\mod_trademon.dll"},
{"DF02E49139A1B6CDD06687A2A240C751","mods\\mod_uh.dll"},
{"9FDE3AD3BE4C1F14AF47AF61B0C61A1B","mods\\tre.dll"},
{"56D1AC23005AB9F37FD8A7B1E6422CAE","tibiaautoinject2.dll"},
};

	AfxEnableControlContainer();

	if(!AfxSocketInit())
      AfxMessageBox("Failed to Initialize Sockets",MB_OK| MB_ICONSTOP);


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// perform MD5 check on all dlls which are included
	
	CMD5 md5;
	int i;
	int cont=1;
	int zeta=100;
	for (i=0;i<sizeof(dllEntryTab)/sizeof(struct dllEntry);i++)
	{
		char buf[128];
		char *fileBuffer = (char *)malloc(1000000);

		sprintf(buf,"%s",dllEntryTab[i].dllName);
		FILE *f=fopen(buf,"rb");
		int len=0;
		if (f)
		{
			len=fread(fileBuffer,1,1000000,f);
			fclose(f);
		} else {
			cont=0;
		}
		
		if (len>0)
		{			
			if (strcmpi(md5.getMD5Digest(fileBuffer,len),dllEntryTab[i].md5result))
			{
				// Some md5 checksum is incorrect. TA will not continue.
				cont=0;
				zeta-=100;
				break;
			}
		}

		free(fileBuffer);		
	}


	// end of perform MD5 check

	// TODO: security part	
	//cont=1;
	if (cont)
	{		
		CTibiaautoDlg dlg;
		m_pMainWnd = &dlg;
		// TODO: security part
		dlg.passSecurityInfo(zeta);
		dlg.DoModal();
	}
	

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


