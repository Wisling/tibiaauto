; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
<<<<<<< mod_itemconfig.clw
ClassCount=8
Class1=CItemAdd
LastClass=CItemAdd
=======
LastClass=CItemAdd
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
>>>>>>> 1.7
NewFileInclude2=#include "mod_itemconfig.h"
<<<<<<< mod_itemconfig.clw
ResourceCount=8
NewFileInclude1=#include "stdafx.h"
Class2=CItemEdit
LastTemplate=CDialog
Resource1=IDD_FOOD_EDIT
Resource2=IDD_ITEM_EDIT
Resource3=IDD_FOOD_ADD
Resource4=IDD_FULL_ITEM_LIST_ADD
Resource5=IDD_LOOT_EDIT
Resource6=IDD_LOOT_ADD
Resource9=IDD_LOOT_EDIT
Resource10=IDD_TOOL_ITEMCONFIG (Polish)
=======
LastPage=0

ClassCount=8
Class1=CItemAdd
Class2=CItemEdit
Class3=CFoodAdd
Class4=CFoodEdit
Class5=CLootAdd
Class6=CLootEdit
Class7=CMod_itemconfigApp
Class8=CToolItemConfig

<<<<<<< mod_itemconfig.clw
ResourceCount=8
Resource1=IDD_FOOD_EDIT
Resource2=IDD_ITEM_EDIT
Resource3=IDD_FOOD_ADD
Resource4=IDD_FULL_ITEM_LIST_ADD
Resource5=IDD_LOOT_EDIT
Resource6=IDD_LOOT_ADD
Resource7=IDD_TOOL_ITEMCONFIG (Polish)
Resource7=IDD_TOOL_ITEMCONFIG (Polish)
=======
ResourceCount=8
Resource1=IDD_FOOD_EDIT
Resource2=IDD_ITEM_EDIT
Resource3=IDD_FOOD_ADD
Resource4=IDD_FULL_ITEM_LIST_ADD
Resource5=IDD_LOOT_EDIT
Resource6=IDD_LOOT_ADD
Resource7=IDD_TOOL_ITEMCONFIG (Polish)
Resource8=IDD_ITEM_ADD
Resource8=IDD_ITEM_ADD
>>>>>>> 1.8

[CLS:CItemAdd]
Type=0
BaseClass=CDialog
HeaderFile=ItemConfig.h
ImplementationFile=ItemConfig.cpp
LastObject=CItemAdd

[CLS:CItemEdit]
Type=0
BaseClass=CDialog
HeaderFile=ItemConfig.h
ImplementationFile=ItemConfig.cpp
Filter=D
VirtualFilter=dWC

[CLS:CFoodAdd]
Type=0
BaseClass=CDialog
HeaderFile=ItemConfig.h
ImplementationFile=ItemConfig.cpp
Filter=D
VirtualFilter=dWC
LastObject=CFoodAdd

[CLS:CFoodEdit]
Type=0
BaseClass=CDialog
HeaderFile=ItemConfig.h
ImplementationFile=ItemConfig.cpp
Filter=D
VirtualFilter=dWC

[CLS:CLootAdd]
Type=0
BaseClass=CDialog
HeaderFile=ItemConfig.h
ImplementationFile=ItemConfig.cpp
LastObject=IDCANCEL

[CLS:CLootEdit]
Type=0
BaseClass=CDialog
HeaderFile=ItemConfig.h
ImplementationFile=ItemConfig.cpp
LastObject=CLootEdit
Filter=D
VirtualFilter=dWC
>>>>>>> 1.7

[CLS:CMod_itemconfigApp]
Type=0
BaseClass=CWinApp
HeaderFile=mod_itemconfig.h
ImplementationFile=mod_itemconfig.cpp
<<<<<<< mod_itemconfig.clw
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=IDC_TREE1
=======

[CLS:CToolItemConfig]
Type=0
BaseClass=CDialog
HeaderFile=ToolItemConfig.h
ImplementationFile=ToolItemConfig.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDC_TREE1
>>>>>>> 1.7

[DLG:IDD_TOOL_ITEMCONFIG]
Type=1
<<<<<<< mod_itemconfig.clw
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

[DLG:IDD_TOOL_ITEMCONFIG (Polish)]
Type=1
Class=CToolItemConfig
ControlCount=16
Control1=IDC_FRAME_FOOD,button,1342177287
Control2=IDC_FRAME_ITEMS,button,1342177287
Control3=IDC_TOOL_ITEMCONFIG_FOODLIST,listbox,1352728835
Control4=IDC_TOOL_ITEMCONFIG_REFRESH,button,1342242816
Control5=IDOK,button,1342242817
Control6=IDC_EDIT_ITEM,button,1342242816
Control7=IDC_DELETE_ITEM,button,1342242816
Control8=IDC_ADD_FOOD,button,1342242816
Control9=IDC_EDIT_FOOD,button,1342242816
Control10=IDC_DELETE_FOOD,button,1342242816
Control11=IDC_ADD_ITEM,button,1342242816
Control12=IDCANCEL,button,1342242817
Control13=IDC_ITEM_SORT,button,1342242816
Control14=IDC_HELP_INFO,button,1342242816
Control15=IDC_SELECTEDTOBRANCH,button,1342251008
Control16=IDC_ITEMCONFIG_ITEMSTREE,SysTreeView32,1342242816

[MNU:IDR_SHOWMAP_MENU (Polish)]
Type=1
Class=?
Command1=ID_SHOWMAPOPTIONS_CLEAR
Command2=ID_SHOWMAPOPTIONS_AVAILABLE
Command3=ID_SHOWMAPOPTIONS_UP
Command4=ID_SHOWMAPOPTIONS_DOWN
CommandCount=4

[DLG:IDD_ITEM_EDIT (H)]
Type=1
Class=?
ControlCount=6
Control1=IDC_ADD_ITEM,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_ITEM_ADD_NAME,edit,1350631552
Control6=IDC_ITEM_ADD_ID,edit,1350631552

[DLG:IDD_ITEM_ADD]
Type=1
Class=CItemAdd
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1350697473
Control4=IDC_STATIC,static,1350697473
Control5=IDC_ITEM_ADD_NAME,edit,1350631552
Control6=IDC_ITEM_ADD_ID,edit,1350631552
Control7=IDC_ITEM_ADD_BRANCH,button,1342242819
Control8=IDC_STATIC,static,1350697473
Control9=IDC_TYPE,combobox,1344340498

[DLG:IDD_ITEM_EDIT]
Type=1
Class=CItemEdit
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1350697473
Control4=IDC_STATIC,static,1350697473
Control5=IDC_ITEM_EDIT_NAME,edit,1350631552
Control6=IDC_ITEM_EDIT_ID,edit,1350631552
Control7=IDC_STATIC,static,1350697473
Control8=IDC_TYPE,combobox,1344340498

[DLG:IDD_FOOD_ADD]
Type=1
Class=CFoodAdd
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1350697472
Control4=IDC_STATIC,static,1350697472
Control5=IDC_FOOD_ADD_NAME,edit,1350631552
Control6=IDC_FOOD_ADD_ID,edit,1350631552
Control7=IDC_STATIC,static,1350697472
Control8=IDC_FOOD_ADD_TIME,edit,1350631552

[DLG:IDD_FOOD_EDIT]
Type=1
Class=CFoodEdit
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1350697472
Control4=IDC_STATIC,static,1350697472
Control5=IDC_FOOD_EDIT_NAME,edit,1350631552
Control6=IDC_FOOD_EDIT_ID,edit,1350631552
Control7=IDC_STATIC,static,1350697472
Control8=IDC_FOOD_EDIT_TIME,edit,1350631552

[DLG:IDD_LOOT_ADD]
Type=1
Class=CLootAdd
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1350697472
Control4=IDC_STATIC,static,1350697472
Control5=IDC_LOOT_ADD_NAME,edit,1350631552
Control6=IDC_LOOT_ADD_ID,edit,1350631552

[DLG:IDD_LOOT_EDIT]
Type=1
Class=CLootEdit
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1350697472
Control4=IDC_STATIC,static,1350697472
Control5=IDC_LOOT_EDIT_NAME,edit,1350631552
Control6=IDC_LOOT_EDIT_ID,edit,1350631552

[DLG:IDD_FULL_ITEM_LIST_ADD]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_FRAME_TIBIA_ITEMS,button,1342177287
Control4=IDC_TIBIA_ITEM_TREE,SysTreeView32,1350631680

