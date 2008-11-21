; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=2
Class1=CMod_itemconfigApp
LastClass=CCharInfoDialog
NewFileInclude2=#include "mod_itemconfig.h"
ResourceCount=10
NewFileInclude1=#include "stdafx.h"
Class2=CCharInfoDialog
LastTemplate=CDialog
Resource1=IDD_TOOL_ITEMCONFIG
Resource2=IDR_SHOWMAP_MENU (Polish)
Resource3=IDD_ITEM_EDIT (H)
Resource4=IDD_FOOD_ADD
Resource5=IDD_ITEM_ADD
Resource6=IDD_LOOT_ADD
Resource7=IDD_ITEM_EDIT
Resource8=IDD_FOOD_EDIT
Resource9=IDD_TOOL_ITEMCONFIG (Polish)
Resource10=IDD_LOOT_EDIT

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

[DLG:IDD_TOOL_ITEMCONFIG (Polish)]
Type=1
Class=?
ControlCount=19
Control1=IDC_TOOL_ITEMCONFIG_FOODLIST,listbox,1352728835
Control2=IDC_TOOL_ITEMCONFIG_LOOTED_LIST,listbox,1352728835
Control3=IDC_TOOL_ITEMCONFIG_CORPSES_LIST,listbox,1084293379
Control4=IDC_TOOL_ITEMCONFIG_ITEMS_LIST,listbox,1352728835
Control5=IDC_TOOL_ITEMCONFIG_REFRESH,button,1342242816
Control6=IDOK,button,1342242817
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,button,1073741831
Control11=IDC_ADD_ITEM,button,1342242816
Control12=IDC_EDIT_ITEM,button,1342242816
Control13=IDC_DELETE_ITEM,button,1342242816
Control14=IDC_ADD_FOOD,button,1342242816
Control15=IDC_EDIT_FOOD,button,1342242816
Control16=IDC_DELETE_FOOD,button,1342242816
Control17=IDC_ADD_LOOT,button,1342242816
Control18=IDC_EDIT_LOOT,button,1342242816
Control19=IDC_DELETE_LOOT,button,1342242816

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
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_ITEM_ADD_NAME,edit,1350631552
Control6=IDC_ITEM_ADD_ID,edit,1350631552

[DLG:IDD_ITEM_EDIT]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_ITEM_EDIT_NAME,edit,1350631552
Control6=IDC_ITEM_EDIT_ID,edit,1350631552

[DLG:IDD_FOOD_ADD]
Type=1
Class=?
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_FOOD_ADD_NAME,edit,1350631552
Control6=IDC_FOOD_ADD_ID,edit,1350631552
Control7=IDC_STATIC,static,1342308864
Control8=IDC_FOOD_ADD_TIME,edit,1350631552

[DLG:IDD_FOOD_EDIT]
Type=1
Class=?
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_FOOD_EDIT_NAME,edit,1350631552
Control6=IDC_FOOD_EDIT_ID,edit,1350631552
Control7=IDC_STATIC,static,1342308864
Control8=IDC_FOOD_EDIT_TIME,edit,1350631552

[DLG:IDD_LOOT_ADD]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_LOOT_ADD_NAME,edit,1350631552
Control6=IDC_LOOT_ADD_ID,edit,1350631552

[DLG:IDD_LOOT_EDIT]
Type=1
Class=?
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308864
Control4=IDC_STATIC,static,1342308864
Control5=IDC_LOOT_EDIT_NAME,edit,1350631552
Control6=IDC_LOOT_EDIT_ID,edit,1350631552

