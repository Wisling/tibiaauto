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

{"14AC68C802E75FA8D3BCA2CCB0DB66CF","mods\\alice.dll"},
{"F34CE3B83C8C7E93FBF29CC6F9BBEA45","mods\\mod_aim.dll"},
{"B8F78CD655CFD1A500C3047A27955D94","mods\\mod_antylogout.dll"},
{"E9DBA7E6D86BD6D71F974958F8F1E4FD","mods\\mod_autogo.dll"},
{"C97AE7608700567B92D4A0BEFB87CB7C","mods\\mod_cavebot.dll"},
{"2C0B3B95EADCE644107B25A53ABEF321","mods\\mod_creatureinfo.dll"},
{"18C0002E7176CA35B8F70972CCE3A826","mods\\mod_eater.dll"},
{"D0F1AF511F40CE3D41838D340C119042","mods\\mod_fisher.dll"},
{"7EF19FCA4B02A16EE9392D40A3A70EFE","mods\\mod_fluid.dll"},
{"0D02F7DC496F12B8B759FA6DEBCB5B52","mods\\mod_fps.dll"},
{"199F480AE08981C96752175618F0C34F","mods\\mod_grouping.dll"},
{"D20F5C4E1E50175556342C7EB6F46479","mods\\mod_itemconfig.dll"},
{"1F003ED91C1834E756EC4EE61137E89A","mods\\mod_light.dll"},
{"D5461101295D45CC75F6AF4FF3EB642E","mods\\mod_looter.dll"},
{"10E7A97C2A3C532F5D7E49296455B422","mods\\mod_maphack.dll"},
{"0B3F6839D7F79A301C446F7EB55059BA","mods\\mod_memdebug.dll"},
{"93BF6C63758BE166473CEDCE647E7DAB","mods\\mod_monstershow.dll"},
{"56EF4BA3272025EB58A3BFAC31BB6C17","mods\\mod_playerinfo.dll"},
{"33CC4942327B33F2E4EF51899538754A","mods\\mod_responder.dll"},
{"3E9F0BF2CAFCDD12B43E72FD4029A7A9","mods\\mod_restack.dll"},
{"0F12C7F2572AF7275D7184568A750D26","mods\\mod_runemaker.dll"},
{"35B0AC344F51B3059CB7A528CF1F9CD3","mods\\mod_showmap.dll"},
{"1389CF1511EBD1A905A1265CA781C29B","mods\\mod_spellcaster.dll"},
{"5E53D3B4BEA87C561DF3BC3298B47860","mods\\mod_team.dll"},
{"386CE62DE2B4632B05F812AA6C1341AD","mods\\mod_trademon.dll"},
{"86569D8FF96531EA427E3E0E0F9AE065","mods\\mod_uh.dll"},
{"C9DF04E7C5B8666FC44587049AD90303","mods\\tre.dll"},
{"E515193D951F5314A9771CC225130B64","tibiaautoinject2.dll"}
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


