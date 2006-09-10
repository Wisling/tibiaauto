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
{"7F224FE0E1DA5998FDEF00E1554057AA","mods\\alice.dll"},
{"19F9900EA681AAEA31DFFB04A2487ECB","mods\\mod_aim.dll"},
{"769F60E06FDBE25B2C0A57A497B7EE10","mods\\mod_antylogout.dll"},
{"25C0E80C8FB7116C055D2EB9A58D5EAE","mods\\mod_autogo.dll"},
{"A122AFE4BD87F9F7372DFE23E24ADAE6","mods\\mod_cavebot.dll"},
{"74A5701454C2A968D315CE7DE4EB84AF","mods\\mod_creatureinfo.dll"},
{"E371BF94C6E7DFF4568902B06E87927B","mods\\mod_eater.dll"},
{"F9B224872CDB19AACC9748299FA61F86","mods\\mod_fisher.dll"},
{"3538E8E73383465B4BDCF53CEB7E778B","mods\\mod_fluid.dll"},
{"8DF6C69CBEA5731917534F37EF86AF78","mods\\mod_fps.dll"},
{"AC3E43E6E533CB3E6CE9CF883D4AA952","mods\\mod_grouping.dll"},
{"AD7A61380C43C5656863BB8E38A17E34","mods\\mod_itemconfig.dll"},
{"4DA5EF6AD6032C84B4ADDA34C699539C","mods\\mod_light.dll"},
{"01EED1F0020035F226CB763FC84D55DA","mods\\mod_looter.dll"},
{"B49C30ABC100247B6987E11EEA5E6C36","mods\\mod_maphack.dll"},
{"5C6C94EF8762D78F7E634BB080D087ED","mods\\mod_memdebug.dll"},
{"D7D4769395225B193A3664397A2B71FE","mods\\mod_monstershow.dll"},
{"5006897163E0B7393CBF61356449EFBD","mods\\mod_playerinfo.dll"},
{"F053FEC209BD57B21FA2390F3A0F02A9","mods\\mod_responder.dll"},
{"647B64DC96D063AF81EC164DA155C573","mods\\mod_restack.dll"},
{"81F3132B60FD14F41D00862DD333D9DE","mods\\mod_runemaker.dll"},
{"377CD1DF1E9A1CEC55FE532F753C01F3","mods\\mod_showmap.dll"},
{"C3CA99F325C8D80F6D24E7032AE6CBDA","mods\\mod_spellcaster.dll"},
{"46F0FE5CCF5166F99C09350E2068BB4C","mods\\mod_team.dll"},
{"A92391B367397D0608528169549CB24E","mods\\mod_trademon.dll"},
{"24376EDB151539A51913D9655FEFF6EE","mods\\mod_uh.dll"},
{"0472648507F5AAFAB166492B33226363","mods\\tre.dll"},
{"32F19BA3503F0AA84813B917F1A84E07","tibiaautoinject2.dll"}
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
	cont=1;
	if (cont)
	{		
		CTibiaautoDlg dlg;
		m_pMainWnd = &dlg;
		// TODO: security part
		//dlg.passSecurityInfo(zeta);
		dlg.DoModal();
	}
	

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


