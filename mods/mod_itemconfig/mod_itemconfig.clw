; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=2
Class1=CMod_itemconfigApp
LastClass=CCharInfoDialog
NewFileInclude2=#include "mod_itemconfig.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Class2=CCharInfoDialog
LastTemplate=CDialog
Resource1=IDD_TOOL_ITEMCONFIG
Resource2=IDR_SHOWMAP_MENU

[CLS:CMod_itemconfigApp]
Type=0
HeaderFile=mod_itemconfig.h
ImplementationFile=mod_itemconfig.cpp
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

[DLG:IDD_TOOL_ITEMCONFIG]
Type=1
Class=?
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDC_TOOL_ITEMCONFIG_FOODLIST,listbox,1352728835
Control3=IDC_STATIC,button,1342177287
Control4=IDC_TOOL_ITEMCONFIG_LOOTED_LIST,listbox,1352728835
Control5=IDC_STATIC,button,1342177287
Control6=IDC_TOOL_ITEMCONFIG_ITEMS_LIST,listbox,1352728835
Control7=IDC_STATIC,button,1342177287
Control8=IDC_TOOL_ITEMCONFIG_CORPSES_LIST,listbox,1084293379
Control9=IDC_STATIC,button,1073741831
Control10=IDC_TOOL_ITEMCONFIG_REFRESH,button,1342242816

