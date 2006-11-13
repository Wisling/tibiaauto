; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ClassCount=3
Class1=CMod_autogoApp
LastClass=CConfigDialog
NewFileInclude2=#include "mod_autogo.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
Class2=CConfigDialog
LastTemplate=CDialog
Resource1=IDD_CONFIG
Class3=CWhiteList
Resource2=IDD_WHITELIST

[CLS:CMod_autogoApp]
Type=0
HeaderFile=mod_autogo.h
ImplementationFile=mod_autogo.cpp
Filter=N
LastObject=CMod_autogoApp

[DLG:IDD_CONFIG]
Type=1
Class=CConfigDialog
ControlCount=82
Control1=IDC_AUTOGO_TRIGGERBATTLELIST,button,1342246147
Control2=IDC_AUTOGO_BATTLEPLAYER,button,1342245891
Control3=IDC_AUTOGO_BATTLEMONSTER,button,1342245891
Control4=IDC_AUTOGO_BATTLEGM,button,1342245891
Control5=IDC_AUTOGO_BATTELIST,button,1342245891
Control6=IDC_AUTOGO_BATTLEWHITELIST,button,1342242816
Control7=IDC_AUTOGO_ACTIONBATTLELIST,combobox,1344340227
Control8=IDC_AUTOGO_SOUNDBATTLELIST,button,1342242819
Control9=IDC_AUTOGO_TRIGGERSIGN,button,1342245891
Control10=IDC_AUTOGO_SIGNBATTLE,button,1342185283
Control11=IDC_AUTOGO_SIGNPOISON,button,1342185283
Control12=IDC_AUTOGO_SIGNFIRE,button,1342185283
Control13=IDC_AUTOGO_SIGNENERGY,button,1342185283
Control14=IDC_AUTOGO_ACTIONSIGN,combobox,1344340227
Control15=IDC_AUTOGO_SOUNDSIGN,button,1342242819
Control16=IDC_AUTOGO_TRIGGERMESSAGE,button,1342245891
Control17=IDC_AUTOGO_MESSAGEPRIVATE,button,1342245891
Control18=IDC_AUTOGO_MESSAGEPUBLIC,button,1342245891
Control19=IDC_AUTOGO_ACTIONMESSAGE,combobox,1344340227
Control20=IDC_AUTOGO_SOUNDMESSAGE,button,1342242819
Control21=IDC_AUTOGO_TRIGGERMOVE,button,1342245891
Control22=IDC_AUTOGO_ACTIONMOVE,combobox,1344340227
Control23=IDC_AUTOGO_SOUNDMOVE,button,1342242819
Control24=IDC_AUTOGO_TRIGGERHPLOSS,button,1342245891
Control25=IDC_AUTOGO_ACTIONHPLOSS,combobox,1344340227
Control26=IDC_AUTOGO_SOUNDHPLOSS,button,1342242819
Control27=IDC_AUTOGO_TRIGGERHPBELOW,button,1342245891
Control28=IDC_AUTOGO_HPBELOW,edit,1350639744
Control29=IDC_AUTOGO_ACTIONHPBELOW,combobox,1344340227
Control30=IDC_AUTOGO_SOUNDHPBELOW,button,1342242819
Control31=IDC_AUTOGO_TRIGGERSOULPOINTS,button,1342245891
Control32=IDC_AUTOGO_SOULPOINT,edit,1350639744
Control33=IDC_AUTOGO_ACTIONSOULPOINT,combobox,1344340227
Control34=IDC_AUTOGO_SOUNDSOULPOINT,button,1342242819
Control35=IDC_AUTOGO_TRIGGERBLANK,button,1342245891
Control36=IDC_AUTOGO_BLANK,edit,1350639744
Control37=IDC_AUTOGO_ACTIONBLANK,combobox,1344340227
Control38=IDC_STATIC,static,1342177296
Control39=IDC_AUTOGO_SOUNDBLANK,button,1342242819
Control40=IDC_AUTOGO_TRIGGERCAPACITY,button,1342245891
Control41=IDC_AUTOGO_CAPACITY,edit,1350639744
Control42=IDC_AUTOGO_ACTIONCAPACITY,combobox,1344340227
Control43=IDC_AUTOGO_SOUNDCAPACITY,button,1342242819
Control44=IDC_AUTOGO_TRIGGEROUTOF,button,1342245891
Control45=IDC_AUTOGO_OUTOFFOOD,button,1342245891
Control46=IDC_AUTOGO_OUTOFWORMS,button,1342245891
Control47=IDC_AUTOGO_OUTOFSPACE,button,1342245891
Control48=IDC_AUTOGO_ACTIONOUTOF,combobox,1344340227
Control49=IDC_AUTOGO_SOUNDOUTOF,button,1342242819
Control50=IDC_AUTOGO_RUNAWAY_X,edit,1350639744
Control51=IDC_AUTOGO_RUNAWAY_Y,edit,1350639744
Control52=IDC_AUTOGO_RUNAWAY_Z,edit,1350639744
Control53=IDC_AUTOGO_TORUNAWAY,button,1342245888
Control54=IDC_AUTOGO_CUR_X,edit,1484849280
Control55=IDC_AUTOGO_CUR_Y,edit,1484849280
Control56=IDC_AUTOGO_CUR_Z,edit,1484849280
Control57=IDC_AUTOGO_ACT_X,edit,1350639744
Control58=IDC_AUTOGO_ACT_Y,edit,1350639744
Control59=IDC_AUTOGO_ACT_Z,edit,1350639744
Control60=IDC_AUTOGO_TOSTART,button,1342245888
Control61=IDC_ENABLE,button,1342246915
Control62=IDOK,button,1342242817
Control63=IDC_STATIC,button,1342177287
Control64=IDC_STATIC,button,1342177287
Control65=IDC_STATIC,static,1342308864
Control66=IDC_STATIC,static,1342308864
Control67=IDC_AUTOGO_STATUS,static,1342308352
Control68=IDC_STATIC,static,1342177296
Control69=IDC_STATIC,static,1342177297
Control70=IDC_STATIC,static,1342177297
Control71=IDC_STATIC,static,1342177297
Control72=IDC_STATIC,static,1342177296
Control73=IDC_STATIC,static,1342177296
Control74=IDC_STATIC,static,1342177296
Control75=IDC_STATIC,static,1342177296
Control76=IDC_STATIC,static,1342177296
Control77=IDC_STATIC,static,1342177296
Control78=IDC_STATIC,static,1342308864
Control79=IDC_STATIC,static,1342177296
Control80=IDC_STATIC,static,1342177296
Control81=IDC_AUTOGO_ACT_DIRECTION,combobox,1344340227
Control82=IDC_AUTOGO_BATTLEPARANOIAM,button,1342245891

[CLS:CConfigDialog]
Type=0
HeaderFile=ConfigDialog.h
ImplementationFile=ConfigDialog.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CConfigDialog

[DLG:IDD_WHITELIST]
Type=1
Class=CWhiteList
ControlCount=5
Control1=IDC_WHITELIST_NAME,edit,1350635648
Control2=IDC_WHITELIST_ADD,button,1342242816
Control3=IDC_WHITELIST_LIST,listbox,1352728835
Control4=IDC_WHITELIST_DELETE,button,1342242816
Control5=IDOK,button,1342242817

[CLS:CWhiteList]
Type=0
HeaderFile=WhiteList.h
ImplementationFile=WhiteList.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CWhiteList

