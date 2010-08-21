; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CToolContainerContent
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "mod_runemaker.h"
LastPage=0

ClassCount=3
Class1=CConfigDialog
Class2=CMod_runemakerApp
Class3=CToolContainerContent

ResourceCount=2
Resource1=IDD_CONFIG (Polish)
Resource2=IDD_TOOL_CONTAINERCONTENT (Polish)

[CLS:CConfigDialog]
Type=0
BaseClass=CDialog
HeaderFile=ConfigDialog.h
ImplementationFile=ConfigDialog.cpp

[CLS:CMod_runemakerApp]
Type=0
BaseClass=CWinApp
HeaderFile=mod_runemaker.h
ImplementationFile=mod_runemaker.cpp

[CLS:CToolContainerContent]
Type=0
BaseClass=CDialog
HeaderFile=ToolContainerContent.h
ImplementationFile=ToolContainerContent.cpp
Filter=D
VirtualFilter=dWC

[DLG:IDD_CONFIG]
Type=1
Class=CConfigDialog

[DLG:IDD_TOOL_CONTAINERCONTENT]
Type=1
Class=CToolContainerContent

[DLG:IDD_CONFIG (Polish)]
Type=1
Class=?
ControlCount=30
Control1=IDC_FRAME_BACKPACK_VIEWER,button,1342177287
Control2=IDC_FRAME_RUNE_OPTIONS,button,1342177287
Control3=IDC_FRAME_GENERAL_OPTIONS,button,1342177287
Control4=IDC_FRAME_STATISTICS,button,1342177287
Control5=IDOK,button,1342242817
Control6=IDC_ENABLE,button,1342246915
Control7=IDC_TOOL_RUNEMAKER_CONT,SysListView32,1350664197
Control8=IDC_TOOL_RUNEMAKER_RUNESPELL,edit,1350631552
Control9=IDC_TOOL_RUNEMAKER_SOUL,edit,1350631552
Control10=IDC_STATIC,static,1342308864
Control11=IDC_STATIC,static,1342308864
Control12=IDC_TOOL_RUNEMAKER_INFO_BLANKS,static,1342308352
Control13=IDC_TOOL_RUNEMAKER_INFO_FOOD,static,1342308352
Control14=IDC_TOOL_RUNEMAKER_INFO_RUNETIME,static,1342308352
Control15=IDC_STATIC,static,1342308864
Control16=IDC_TOOL_RUNEMAKER_MANALIMIT,edit,1350631552
Control17=IDC_STATIC,static,1342308864
Control18=IDC_TOOL_RUNEMAKER_MANA,edit,1350631552
Control19=IDC_TOOL_PREMIUM,button,1342242819
Control20=IDC_TOOL_MAXMANAUSE,button,1342242819
Control21=IDC_TOOL_RUNEMAKER_INFO_SOULPOINTSTIME,static,1342308352
Control22=IDC_ADDSPELLTOLIST,button,1342242816
Control23=IDC_LOADSPELLFROMLIST,button,1342242816
Control24=IDC_DELETEFROMLIST,button,1342242816
Control25=IDC_TOOL_RUNEMAKER_SPELLS,SysListView32,1350664197
Control26=IDC_MAKENOW,button,1342246915
Control27=IDC_TOOL_RANDOMCAST,button,1342242819
Control28=IDC_TOOL_USESPEAR,button,1342242819
Control29=IDC_TOOL_USEARROW,button,1342177289
Control30=IDC_USE_BACKPACK,button,1342177289

[DLG:IDD_TOOL_CONTAINERCONTENT (Polish)]
Type=1
Class=?
ControlCount=3
Control1=IDC_FRAME_BACKPACK_CONTENTS,button,1342177287
Control2=IDOK,button,1342242817
Control3=IDC_CONTAINERCONTENT_LIST,SysListView32,1350664193

