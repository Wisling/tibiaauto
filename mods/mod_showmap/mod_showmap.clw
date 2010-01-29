; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMapConfig
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "mod_showmap.h"
LastPage=0

ClassCount=4
Class1=CMapConfig
Class2=CMapButton
Class3=CMod_showmapApp
Class4=CToolMapShow

ResourceCount=3
Resource1=IDD_TOOL_MAPSHOW (Polish)
Resource2=IDD_TOOL_MAPSHOW_CONFIG
Resource3=IDR_SHOWMAP_MENU (Polish)

[CLS:CMapConfig]
Type=0
BaseClass=CDialog
HeaderFile=ConfigWindow.h
ImplementationFile=ConfigWindow.cpp
Filter=D
VirtualFilter=dWC

[CLS:CMapButton]
Type=0
BaseClass=CButton
HeaderFile=MapButton.h
ImplementationFile=MapButton.cpp

[CLS:CMod_showmapApp]
Type=0
BaseClass=CWinApp
HeaderFile=mod_showmap.h
ImplementationFile=mod_showmap.cpp

[CLS:CToolMapShow]
Type=0
BaseClass=CDialog
HeaderFile=ToolMapShow.h
ImplementationFile=ToolMapShow.cpp
Filter=D
VirtualFilter=dWC

[DLG:IDD_TOOL_MAPSHOW_CONFIG]
Type=1
Class=CMapConfig
ControlCount=25
Control1=IDC_FRAME_TILE_TYPE,button,1342177287
Control2=IDC_FRAME_MOVEMENT,button,1342177287
Control3=IDC_FRAME_USE,button,1342177287
Control4=IDC_FRAME_CHARACTERISTICS,button,1342177287
Control5=IDC_COMMIT,button,1342242817
Control6=IDC_GROUND,button,1342242819
Control7=IDC_CONTAINER,button,1342242819
Control8=IDC_DEPOT,button,1342242819
Control9=IDC_SHOVEL,button,1342243075
Control10=IDC_ROPE,button,1342243075
Control11=IDC_USE_TILE,button,1342243075
Control12=IDC_MOVABLE,button,1342243075
Control13=IDC_GO_UP,button,1342243075
Control14=IDC_GO_DOWN,button,1342243075
Control15=IDC_BLOCKING,button,1342251267
Control16=IDC_WALKABLE,button,1342251267
Control17=IDC_ALWAYS_ON_TOP,button,1342251267
Control18=IDC_STACKABLE,button,1342243075
Control19=IDC_TILE_ID,edit,1350641792
Control20=IDC_STATIC,static,1342309120
Control21=IDC_SPEED,edit,1350639744
Control22=IDC_STATIC,static,1342308864
Control23=IDC_PREVIOUS_CONFIG_TILE,button,1342242816
Control24=IDC_NEXT_CONFIG_TILE,button,1342242816
Control25=IDC_TELEPORTER,button,1342243075

[DLG:IDD_TOOL_MAPSHOW]
Type=1
Class=CToolMapShow

[DLG:IDD_TOOL_MAPSHOW (Polish)]
Type=1
Class=?
ControlCount=6
Control1=IDC_FRAME_MAPPING_OPTIONS,button,1342177287
Control2=IDOK,button,1342242817
Control3=IDC_TOOL_MAPSHOW_RESEARCH,button,1342242819
Control4=IDC_TOOL_MAPSHOW_CLEAR,button,1342242816
Control5=IDC_TILE_INFO,static,1350696960
Control6=IDC_TOOL_MAPSHOW_EXTENDED_RESEARCH,button,1342242819

[MNU:IDR_SHOWMAP_MENU (Polish)]
Type=1
Class=?
Command1=ID_SHOWMAPOPTIONS_CLEAR
Command2=ID_SHOWMAPOPTIONS_AVAILABLE
Command3=ID_SHOWMAPOPTIONS_DEPOT
Command4=ID_SHOWMAPOPTIONS_TELE
Command5=ID_SHOWMAPOPTIONS_BLOCK
Command6=ID_SHOWMAPOPTIONS_ROPE
Command7=ID_SHOWMAPOPTIONS_LADDER
Command8=ID_SHOWMAPOPTIONS_STAIRS
Command9=ID_SHOWMAPOPTIONS_MAGICROPE
Command10=ID_SHOWMAPOPTIONS_OPENHOLE
Command11=ID_SHOWMAPOPTIONS_CLOSEDHOLE
Command12=ID_SHOWMAPOPTIONS_CRATE
Command13=IDD_TOOL_MAPSHOW_CONFIG
CommandCount=13

