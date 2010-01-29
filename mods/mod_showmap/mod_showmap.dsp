# Microsoft Developer Studio Project File - Name="mod_showmap" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mod_showmap - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mod_showmap.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mod_showmap.mak" CFG="mod_showmap - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mod_showmap - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mod_showmap - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mod_showmap - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\sdk" /I "C:\Documents and Settings\Owner\My Documents\Xerces\xerces-c-src_2_8_0\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 xerces.lib /nologo /subsystem:windows /dll /machine:I386 /out:"../mod_showmap.dll" /libpath:"C:\Documents and Settings\Owner\My Documents\TibiaAuto\tibiaauto-kernel" /libpath:"e:\projects\tibiaauto-pub"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "mod_showmap - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\sdk" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /i "C:\Documents and Settings\Owner\Desktop\xercesc\xerces-c-src_2_8_0\src" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"E:\Tibia\Tibia\Tibia Auto\mods/mod_showmap.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mod_showmap - Win32 Release"
# Name "mod_showmap - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\sdk\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigData.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\exports.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\GroupBoxEx.cpp
# End Source File
# Begin Source File

SOURCE=.\MapButton.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_showmap.cpp
# End Source File
# Begin Source File

SOURCE=.\mod_showmap.def
# End Source File
# Begin Source File

SOURCE=.\mod_showmap.rc
# End Source File
# Begin Source File

SOURCE=..\..\sdk\MyDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\proxy.cpp
# End Source File
# Begin Source File

SOURCE=..\..\sdk\Skin.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TibiaItem_impl.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolMapShow.cpp
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

SOURCE=.\res\map_bloced.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_block.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_blocked.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_closedhole.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_crate.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_depot.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_empty.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_ladder.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_magicrope.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_openhole.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_rope.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_samefloor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_self.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_stairs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_tele.bmp
# End Source File
# Begin Source File

SOURCE=.\res\map_unktele.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mod_showmap.rc2
# End Source File
# Begin Source File

SOURCE=.\thumbnailCARAJ88G.bmp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConfigData.h
# End Source File
# Begin Source File

SOURCE=.\ConfigWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\GroupBoxEx.h
# End Source File
# Begin Source File

SOURCE=.\MapButton.h
# End Source File
# Begin Source File

SOURCE=.\mod_showmap.h
# End Source File
# Begin Source File

SOURCE=..\..\sdk\MyDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ToolMapShow.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
