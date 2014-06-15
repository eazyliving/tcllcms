# Microsoft Developer Studio Project File - Name="tcllcms" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TCLLCMS - WIN32 RELEASE
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "tcllcms.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "tcllcms.mak" CFG="TCLLCMS - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "tcllcms - Win32 Release" (basierend auf  "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TCLLCMS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\tcl\tcl8.4.7\generic" /I ":..\..\tcl\tcl8.4.7\win" /I "..\..\tcl\tk8.4.7\win" /I "..\..\tcl\tk8.4.7\generic" /I "..\..\tcl\tk8.4.7\xlib" /I "..\..\lcms-1.14\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TCLLCMS_EXPORTS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 lcms115.lib tcl84.lib tk84.lib kernel32.lib shell32.lib user32.lib /nologo /dll /incremental:yes /machine:I386 /out:"c:\programme\tcl\lib\tcllcms\tcllcms.dll" /libpath:"..\..\tcl\tcl8.4.7\win\Release" /libpath:"..\..\lcms-1.15\Lib\MS" /libpath:"..\..\tcl\tk8.4.7\win\Release"
# SUBTRACT LINK32 /nodefaultlib
# Begin Target

# Name "tcllcms - Win32 Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\generic\tcllcms.c
# End Source File
# Begin Source File

SOURCE=..\generic\tcllcms_conversion.c
# End Source File
# Begin Source File

SOURCE=..\generic\tcllcms_gamma.c
# End Source File
# Begin Source File

SOURCE=..\generic\tcllcms_info.c
# End Source File
# Begin Source File

SOURCE=..\generic\tcllcms_misc.c
# End Source File
# Begin Source File

SOURCE=..\generic\tcllcms_profile.c
# End Source File
# Begin Source File

SOURCE=..\generic\tcllcms_trans.c
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\generic\tcllcms.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\tcllcms.rc
# End Source File
# End Group
# End Target
# End Project
