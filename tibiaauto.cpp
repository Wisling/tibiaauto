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

		{"CBF2FF8A5F6DF18107A2771C525AB7AE","mods\\alice.dll"},
{"23DF45A4B0281B6076D224F4BA8E6FD2","mods\\mod_aim.dll"},
{"B0C4E856C6D8E3592AE59B4ACF32E9AA","mods\\mod_antylogout.dll"},
{"F7DA29E28DF0A023742E778424058330","mods\\mod_autogo.dll"},
{"FF5F0F68363DC0CD26C69FCF5DD89FDD","mods\\mod_cavebot.dll"},
{"6E685112C9031957F46C26EDFD17554D","mods\\mod_creatureinfo.dll"},
{"E34276F68D5413A1E79D6ACCD13C638C","mods\\mod_eater.dll"},
{"3FC83127414C94267D8E2E8EEBAD3082","mods\\mod_fisher.dll"},
{"2A4855E623BDE81BFA5DC1C6C8E10171","mods\\mod_fluid.dll"},
{"DBAAE32232AF5ACC8C21CE13A1CBEE76","mods\\mod_fps.dll"},
{"949AC7D4687D5FC5A3A24B68F832D145","mods\\mod_grouping.dll"},
{"292D07C65A68E6FF7C819C56E47766FA","mods\\mod_itemconfig.dll"},
{"171D0237FCB6F63C92999C6C81C35BAC","mods\\mod_light.dll"},
{"BBA66B4F682A2CD8E18F199F9607BF8E","mods\\mod_looter.dll"},
{"179C00E37B5233BADD7DDCACFBD6EBC6","mods\\mod_maphack.dll"},
{"08081F079BB97C7D872DCDEC98C8E334","mods\\mod_memdebug.dll"},
{"471A6A037C0E4DAF62C9365F7AAC698D","mods\\mod_monstershow.dll"},
{"34D9ADACA49DB6C3C7C1AA00A8E647F5","mods\\mod_playerinfo.dll"},
{"18B3C2BE9C2580308D3D4448D24889A4","mods\\mod_responder.dll"},
{"10D0D083611D713B1AEAB96958667B6E","mods\\mod_restack.dll"},
{"8E13BF01B3E2CECCF3D54D09E1E37CA3","mods\\mod_runemaker.dll"},
{"C0B340B852376507E0D4150AD1C6B2C9","mods\\mod_showmap.dll"},
{"BF6E47BF6DF23EC141346A7264C82DBA","mods\\mod_spellcaster.dll"},
{"71AA18EEE9699DEA1A259B3EB7593FA0","mods\\mod_team.dll"},
{"456DBF3BA8E178C41980F483A56FCB9F","mods\\mod_trademon.dll"},
{"5794A25E8A36B77AE0E45A53A0D38659","mods\\mod_uh.dll"},
{"74B1B714F9B7913A0EC7E4975F161DF9","mods\\tre.dll"}
	

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

	cont=1;
	if (cont)
	{		
		CTibiaautoDlg dlg;
		m_pMainWnd = &dlg;
		dlg.passSecurityInfo(zeta);
		dlg.DoModal();
	}
	

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


