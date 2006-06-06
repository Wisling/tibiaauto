; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=2
Class1=CMod_showmapApp
LastClass=CCharInfoDialog
NewFileInclude2=#include "mod_showmap.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Class2=CCharInfoDialog
LastTemplate=CDialog
Resource1=IDD_TOOL_MAPSHOW
Resource2=IDR_SHOWMAP_MENU

[CLS:CMod_showmapApp]
Type=0
HeaderFile=mod_showmap.h
ImplementationFile=mod_showmap.cpp
Filter=N

[CLS:CCharInfoDialog]
Type=0
HeaderFile=CharInfoDialog.h
ImplementationFile=CharInfoDialog.cpp
BaseClass=CDialog
Filter=D
LastObject=CCharInfoDialog
VirtualFilter=dWC

[DLG:IDD_TOOL_MAPSHOW]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_TOOL_MAPSHOW_RESEARCH,button,1342242819
Control3=IDC_TOOL_MAPSHOW_CLEAR,button,1342242816
Control4=IDC_TILE_INFO,static,1350696960
Control5=IDC_TOOL_MAPSHOW_EXTENDED_RESEARCH,button,1342242819

[MNU:IDR_SHOWMAP_MENU]
Type=1
Class=?
Command1=ID_SHOWMAPOPTIONS_CLEAR
Command2=ID_SHOWMAPOPTIONS_AVAILABLE
Command3=ID_SHOWMAPOPTIONS_ROPE
Command4=ID_SHOWMAPOPTIONS_LADDER
Command5=ID_SHOWMAPOPTIONS_STAIRS
Command6=ID_SHOWMAPOPTIONS_MAGICROPE
Command7=ID_SHOWMAPOPTIONS_OPENHOLE
Command8=ID_SHOWMAPOPTIONS_CLOSEDHOLE
Command9=ID_SHOWMAPOPTIONS_CRATE
Command10=ID_SHOWMAPOPTIONS_DEPOT
CommandCount=10

