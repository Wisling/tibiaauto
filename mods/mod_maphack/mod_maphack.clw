; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMod_maphackApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "mod_maphack.h"
LastPage=0

ClassCount=2
<<<<<<< mod_maphack.clw
Class1=CConfigDialog
LastClass=CMod_maphackApp
NewFileInclude2=#include "mod_maphack.h"
ResourceCount=1
NewFileInclude1=#include "stdafx.h"
Class2=CMod_maphackApp
LastTemplate=CDialog
Resource2=IDD_CONFIG (Polish)
=======
Class1=CConfigDialog
Class2=CMod_maphackApp

ResourceCount=1
Resource1=IDD_CONFIG (Polish)
Resource1=IDD_CONFIG (Polish)

[CLS:CConfigDialog]
Type=0
BaseClass=CDialog
HeaderFile=ConfigDialog.h
ImplementationFile=ConfigDialog.cpp
LastObject=IDC_ENABLE
Filter=D
VirtualFilter=dWC
>>>>>>> 1.4

[CLS:CMod_maphackApp]
Type=0
BaseClass=CWinApp
HeaderFile=mod_maphack.h
ImplementationFile=mod_maphack.cpp
LastObject=CMod_maphackApp

[DLG:IDD_CONFIG]
Type=1
Class=CConfigDialog

[DLG:IDD_CONFIG (Polish)]
Type=1
Class=CConfigDialog
ControlCount=8
Control1=IDC_FRAME_MAP_OPTIONS,button,1342177287
Control2=IDOK,button,1342242817
Control3=IDC_ENABLE,button,1342246915
Control4=IDC_REVEAL_NOFISH,button,1342242819
Control5=IDC_REVEALCNAME,button,1342242819
Control6=IDC_MINIMAPRESEARCH,button,1342242819
Control7=IDC_REVEALINVISIBLE,button,1073807363
Control8=IDC_AUTOMOUNT,button,1342242819

