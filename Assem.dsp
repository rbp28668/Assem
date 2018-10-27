# Microsoft Developer Studio Project File - Name="Assem" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Assem - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Assem.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Assem.mak" CFG="Assem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Assem - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Assem - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Assem - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Assem - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Assem - Win32 Release"
# Name "Assem - Win32 Debug"
# Begin Source File

SOURCE=.\AssemblyGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\AssemblyGenerator.hpp
# End Source File
# Begin Source File

SOURCE=.\AssemblyParser.cpp
# End Source File
# Begin Source File

SOURCE=.\AssemblyParser.hpp
# End Source File
# Begin Source File

SOURCE=.\AssemblyUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\AssemblyUnit.hpp
# End Source File
# Begin Source File

SOURCE=.\ByteStream.h
# End Source File
# Begin Source File

SOURCE=.\Error.h
# End Source File
# Begin Source File

SOURCE=.\Exception.cpp
# End Source File
# Begin Source File

SOURCE=.\Exception.hpp
# End Source File
# Begin Source File

SOURCE=.\expression.cpp
# End Source File
# Begin Source File

SOURCE=.\expression.hpp
# End Source File
# Begin Source File

SOURCE=.\ExpressionEvaluator.cpp
# End Source File
# Begin Source File

SOURCE=.\ExpressionEvaluator.hpp
# End Source File
# Begin Source File

SOURCE=.\FreeFormatParser.cpp
# End Source File
# Begin Source File

SOURCE=.\FreeFormatParser.hpp
# End Source File
# Begin Source File

SOURCE=.\InputLine.cpp
# End Source File
# Begin Source File

SOURCE=.\InputLIne.hpp
# End Source File
# Begin Source File

SOURCE=.\INTELHEX.cpp
# End Source File
# Begin Source File

SOURCE=.\IntelHex.h
# End Source File
# Begin Source File

SOURCE=.\KeywordParser.cpp
# End Source File
# Begin Source File

SOURCE=.\KeywordParser.hpp
# End Source File
# Begin Source File

SOURCE=.\LabelParser.cpp
# End Source File
# Begin Source File

SOURCE=.\LabelParser.hpp
# End Source File
# Begin Source File

SOURCE=.\Listing.cpp
# End Source File
# Begin Source File

SOURCE=.\Listing.hpp
# End Source File
# Begin Source File

SOURCE=.\MAIN.cpp
# End Source File
# Begin Source File

SOURCE=.\Module.cpp
# End Source File
# Begin Source File

SOURCE=.\Module.h
# End Source File
# Begin Source File

SOURCE=.\ModuleList.cpp
# End Source File
# Begin Source File

SOURCE=.\ModuleList.hpp
# End Source File
# Begin Source File

SOURCE=.\Opcode.cpp
# End Source File
# Begin Source File

SOURCE=.\Opcode.hpp
# End Source File
# Begin Source File

SOURCE=.\Parameter.cpp
# End Source File
# Begin Source File

SOURCE=.\Parameter.hpp
# End Source File
# Begin Source File

SOURCE=.\ParameterList.cpp
# End Source File
# Begin Source File

SOURCE=.\ParameterList.hpp
# End Source File
# Begin Source File

SOURCE=.\PDLParser.cpp
# End Source File
# Begin Source File

SOURCE=.\PDLParser.hpp
# End Source File
# Begin Source File

SOURCE=.\Plant.cpp
# End Source File
# Begin Source File

SOURCE=.\Plant.hpp
# End Source File
# Begin Source File

SOURCE=.\PlantExpression.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessorDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\ProcessorDefinition.hpp
# End Source File
# Begin Source File

SOURCE=.\Production.cpp
# End Source File
# Begin Source File

SOURCE=.\Production.hpp
# End Source File
# Begin Source File

SOURCE=.\ProductionList.cpp
# End Source File
# Begin Source File

SOURCE=.\ProductionList.hpp
# End Source File
# Begin Source File

SOURCE=.\Segment.cpp
# End Source File
# Begin Source File

SOURCE=.\Segment.hpp
# End Source File
# Begin Source File

SOURCE=.\SegmentList.cpp
# End Source File
# Begin Source File

SOURCE=.\SegmentList.h
# End Source File
# Begin Source File

SOURCE=.\Set.cpp
# End Source File
# Begin Source File

SOURCE=.\Set.hpp
# End Source File
# Begin Source File

SOURCE=.\SymbolTable.cpp
# End Source File
# Begin Source File

SOURCE=.\SymbolTable.hpp
# End Source File
# End Target
# End Project
