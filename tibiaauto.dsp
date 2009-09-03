# Microsoft Developer Studio Project File - Name="tibiaauto" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=tibiaauto - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tibiaauto.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tibiaauto.mak" CFG="tibiaauto - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tibiaauto - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "tibiaauto - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "tibiaauto"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tibiaauto - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I "E:/Python24/include" /I "../tibiaauto-pub/sdk" /I "E:\projects\tibiaauto-pub\external\db-4.5.20\build_windows" /I "C:\Documents and Settings\Owner\My Documents\Xerces\xerces-c-src_2_8_0\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 zlib.lib ws2_32.lib zdll.lib xerces.lib detours.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"library" /nodefaultlib:"libcd.lib" /out:"../tibiaauto-pub/tibiaauto.exe" /libpath:"e:\python24\libs" /libpath:"C:\Documents and Settings\Owner\My Documents\TibiaAuto\tibiaauto-kernel" /libpath:"e:\projects\tibiaauto-kernel"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tibiaauto - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "E:\Python24\include" /I "../tibiaauto-pub/sdk" /I "E:\projects\tibiaauto-pub\external\db-4.5.20\build_windows" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 zlib.lib ws2_32.lib xerces.lib zdll.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /out:"C:\Program Files\Tibia Auto/tibiaauto.exe" /pdbtype:sept /libpath:"e:\python24\libs"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "tibiaauto - Win32 Release"
# Name "tibiaauto - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CharDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorChooser.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigCreatorUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigCreatorUtilMultiParams.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigDialogStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\DonationDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EnterCode.cpp
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\GroupBoxEx.cpp"
# End Source File
# Begin Source File

SOURCE=.\LoadedModules.cpp
# End Source File
# Begin Source File

SOURCE=.\md5c.cpp
# End Source File
# Begin Source File

SOURCE=.\md5class.cpp
# End Source File
# Begin Source File

SOURCE=.\MemUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleProxy.cpp
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\MyDialog.cpp"
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\PythonEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\PythonScript.cpp
# End Source File
# Begin Source File

SOURCE=.\PythonScriptsDialog.cpp
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\RepeatButton.cpp"
# End Source File
# Begin Source File

SOURCE=.\ScriptConfigDialg.cpp
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\Skin.cpp"
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\SliderCtrlEx.cpp"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\tibiaauto.cpp
# End Source File
# Begin Source File

SOURCE=.\tibiaauto.rc
# End Source File
# Begin Source File

SOURCE=.\tibiaautoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CharDialog.h
# End Source File
# Begin Source File

SOURCE=.\ColorChooser.h
# End Source File
# Begin Source File

SOURCE=.\ConfigCreatorUtil.h
# End Source File
# Begin Source File

SOURCE=.\ConfigCreatorUtilMultiParams.h
# End Source File
# Begin Source File

SOURCE=.\ConfigDialogStatus.h
# End Source File
# Begin Source File

SOURCE=.\DonationDialog.h
# End Source File
# Begin Source File

SOURCE=.\EnterCode.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\GroupBoxEx.h"
# End Source File
# Begin Source File

SOURCE=.\LoadedModules.h
# End Source File
# Begin Source File

SOURCE=.\MD5.h
# End Source File
# Begin Source File

SOURCE=.\md5class.h
# End Source File
# Begin Source File

SOURCE=.\MemUtil.h
# End Source File
# Begin Source File

SOURCE=.\ModuleProxy.h
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\MyDialog.h"
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\PythonEngine.h
# End Source File
# Begin Source File

SOURCE=.\PythonScript.h
# End Source File
# Begin Source File

SOURCE=.\PythonScriptsDialog.h
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\RepeatButton.h"
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScriptConfigDialg.h
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\Skin.h"
# End Source File
# Begin Source File

SOURCE="..\tibiaauto-pub\sdk\SliderCtrlEx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tibiaauto.h
# End Source File
# Begin Source File

SOURCE=.\tibiaautoDlg.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\zconf.h
# End Source File
# Begin Source File

SOURCE=.\zlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon4.ico
# End Source File
# Begin Source File

SOURCE=.\res\map_self.bmp
# End Source File
# Begin Source File

SOURCE=".\res\TA LowFi version.ico"
# End Source File
# Begin Source File

SOURCE=.\res\tibiaauto.ico
# End Source File
# Begin Source File

SOURCE=.\res\tibiaauto.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section tibiaauto : {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}
# 	2:5:Class:CWebBrowser2
# 	2:10:HeaderFile:webbrowser2.h
# 	2:8:ImplFile:webbrowser2.cpp
# End Section
# Section tibiaauto : {8856F961-340A-11D0-A96B-00C04FD705A2}
# 	2:21:DefaultSinkHeaderFile:webbrowser2.h
# 	2:16:DefaultSinkClass:CWebBrowser2
# End Section
