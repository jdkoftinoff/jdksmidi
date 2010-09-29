# Microsoft Developer Studio Project File - Name="jdksmidi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jdksmidi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "jdksmidi.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "jdksmidi.mak" CFG="jdksmidi - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "jdksmidi - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jdksmidi - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jdksmidi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "jdksmidi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF

# Begin Target

# Name "jdksmidi - Win32 Release"
# Name "jdksmidi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\jdksmidi_advancedsequencer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_driver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_driverdump.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_driverwin32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_edittrack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_fileread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_filereadmultitrack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_fileshow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_filewrite.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_filewritemultitrack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_keysig.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_manager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_midi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_multitrack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_process.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_queue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_sequencer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_showcontrol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_showcontrolhandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_smpte.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_sysex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_tempo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_tick.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\jdksmidi_track.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\jdksmidi\advancedsequencer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\driver.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\driverdump.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\driverwin32.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\edittrack.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\file.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\fileread.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\filereadmultitrack.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\fileshow.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\filewrite.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\filewritemultitrack.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\keysig.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\manager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\matrix.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\midi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\msg.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\multitrack.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\parser.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\process.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\queue.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\sequencer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\showcontrol.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\showcontrolhandler.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\smpte.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\song.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\sysex.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\tempo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\tick.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\track.h
# End Source File
# Begin Source File

SOURCE=..\..\include\jdksmidi\world.h
# End Source File
# End Group
# End Target
# End Project
