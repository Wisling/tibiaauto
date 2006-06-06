; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=2
Class1=CMod_responderApp
LastClass=CCharInfoDialog
NewFileInclude2=#include "mod_responder.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Class2=CCharInfoDialog
LastTemplate=CDialog
Resource1=IDR_SHOWMAP_MENU
Resource2=IDD_TOOL_AUTORESPOND

[CLS:CMod_responderApp]
Type=0
HeaderFile=mod_responder.h
ImplementationFile=mod_responder.cpp
Filter=N

[CLS:CCharInfoDialog]
Type=0
HeaderFile=CharInfoDialog.h
ImplementationFile=CharInfoDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CCharInfoDialog
VirtualFilter=dWC

[MNU:IDR_SHOWMAP_MENU]
Type=1
Class=?
Command1=ID_SHOWMAPOPTIONS_CLEAR
Command2=ID_SHOWMAPOPTIONS_AVAILABLE
Command3=ID_SHOWMAPOPTIONS_UP
Command4=ID_SHOWMAPOPTIONS_DOWN
CommandCount=4

[DLG:IDD_TOOL_AUTORESPOND]
Type=1
Class=?
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDC_AUTORESPOND_MSGHISTORY,SysListView32,1350631429
Control3=IDC_AUTORESPOND_ACTIONLOG,SysListView32,1350631429
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_AUTORESPOND_ENABLE,button,1342246915
Control7=IDC_AUTORESPOND_SCRIPT,edit,1353779396
Control8=IDC_AUTORESPOND_CLEAR,button,1342242816
Control9=IDC_AUTORESPOND_THREADSTATUS,SysListView32,1350631429
Control10=IDC_STATIC,static,1342308352
Control11=IDC_AUTORESPOND_ENABLEDEBUG,button,1342242819
Control12=IDC_AUTORESPOND_LOCALECHO,button,1342242819

