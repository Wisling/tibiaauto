# Microsoft Developer Studio Project File - Name="tibiaauto_util" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tibiaauto_util - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tibiaauto_util.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tibiaauto_util.mak" CFG="tibiaauto_util - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tibiaauto_util - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "tibiaauto_util - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tibiaauto_util - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../tibiaauto-pub/sdk" /I "E:\projects\tibiaauto-pub\external\db-4.5.20\build_windows" /I "C:\Documents and Settings\Owner\My Documents\Xerces\xerces-c-src_2_8_0\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 libdb45.lib xerces.lib /nologo /subsystem:windows /dll /machine:I386 /def:".\tibiaauto_util.def" /out:"../../tibiaauto-pub/mods/tibiaauto_util.dll" /libpath:"E:\Tibia\Sourse Code\tibiaauto-kernel"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "tibiaauto_util - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../tibiaauto-pub/sdk" /I "E:\projects\tibiaauto-pub\external\db-4.5.20\build_windows" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 xerces.lib libdb45.lib xerces.lib libdb45.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"E:\Tibia\Tibia\Tibia Auto\mods/tibiaauto_util.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tibiaauto_util - Win32 Release"
# Name "tibiaauto_util - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\tibiaauto-pub\sdk\BtnST.cpp"
# End Source File
# Begin Source File

SOURCE=.\CreaturesReader.cpp
# End Source File
# Begin Source File

SOURCE=.\IPCBackPipe.cpp
# End Source File
# Begin Source File

SOURCE=..\md5c.cpp
# End Source File
# Begin Source File

SOURCE=..\md5class.cpp
# End Source File
# Begin Source File

SOURCE=.\MemConstData.cpp
# End Source File
# Begin Source File

SOURCE=.\MemReader.cpp
# End Source File
# Begin Source File

SOURCE=.\MemUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\PackSender.cpp
# End Source File
# Begin Source File

SOURCE=.\proxy.cpp
# End Source File
# Begin Source File

SOURCE=.\Queue.cpp
# End Source File
# Begin Source File

SOURCE="..\..\tibiaauto-pub\sdk\Skin.cpp"
# End Source File
# Begin Source File

SOURCE=.\SkinLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TAMiniMap.cpp
# End Source File
# Begin Source File

SOURCE=.\tibiaauto_util.cpp
# End Source File
# Begin Source File

SOURCE=.\tibiaauto_util.def

!IF  "$(CFG)" == "tibiaauto_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "tibiaauto_util - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tibiaauto_util.rc
# End Source File
# Begin Source File

SOURCE=.\TibiaCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\TibiaContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\TibiaItem.cpp
# End Source File
# Begin Source File

SOURCE=.\TibiaMap.cpp
# End Source File
# Begin Source File

SOURCE=.\TibiaVIPEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\TileReader.cpp
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\VariableStore.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CreaturesReader.h
# End Source File
# Begin Source File

SOURCE=.\IPCBackPipe.h
# End Source File
# Begin Source File

SOURCE=.\MemReader.h
# End Source File
# Begin Source File

SOURCE=.\MemUtil.h
# End Source File
# Begin Source File

SOURCE=.\PackSender.h
# End Source File
# Begin Source File

SOURCE=.\Queue.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE="..\..\tibiaauto-pub\sdk\Skin.h"
# End Source File
# Begin Source File

SOURCE=.\SkinLoader.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TAMiniMap.h
# End Source File
# Begin Source File

SOURCE=.\tibiaauto_util.h
# End Source File
# Begin Source File

SOURCE=.\TibiaContainer.h
# End Source File
# Begin Source File

SOURCE=.\TibiaItem.h
# End Source File
# Begin Source File

SOURCE=.\TibiaMap.h
# End Source File
# Begin Source File

SOURCE=.\TibiaMapHash.h
# End Source File
# Begin Source File

SOURCE=.\TibiaVIPEntry.h
# End Source File
# Begin Source File

SOURCE=.\TileReader.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# Begin Source File

SOURCE=.\VariableStore.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\tibiaauto_util.rc2
# End Source File
# Begin Source File

SOURCE=.\Tibiaauto_util.rgs
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
