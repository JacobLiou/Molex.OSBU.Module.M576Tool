# Microsoft Developer Studio Generated NMAKE File, Based on list.dsp
!IF "$(CFG)" == ""
CFG=list - Win32 Debug
!MESSAGE No configuration specified. Defaulting to list - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "list - Win32 Release" && "$(CFG)" != "list - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "list.mak" CFG="list - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "list - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "list - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "list - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\list.exe" "$(OUTDIR)\list.bsc"


CLEAN :
	-@erase "$(INTDIR)\ColorListBox.obj"
	-@erase "$(INTDIR)\ColorListBox.sbr"
	-@erase "$(INTDIR)\CommandSFPVOA.obj"
	-@erase "$(INTDIR)\CommandSFPVOA.sbr"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\list.pch"
	-@erase "$(INTDIR)\list.res"
	-@erase "$(INTDIR)\list.sbr"
	-@erase "$(INTDIR)\listDlg.obj"
	-@erase "$(INTDIR)\listDlg.sbr"
	-@erase "$(INTDIR)\OpComm.obj"
	-@erase "$(INTDIR)\OpComm.sbr"
	-@erase "$(INTDIR)\OpCRC32.obj"
	-@erase "$(INTDIR)\OpCRC32.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\list.bsc"
	-@erase "$(OUTDIR)\list.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\list.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\list.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\list.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ColorListBox.sbr" \
	"$(INTDIR)\CommandSFPVOA.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\listDlg.sbr" \
	"$(INTDIR)\OpComm.sbr" \
	"$(INTDIR)\OpCRC32.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\list.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\list.pdb" /machine:I386 /out:"$(OUTDIR)\list.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ColorListBox.obj" \
	"$(INTDIR)\CommandSFPVOA.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\listDlg.obj" \
	"$(INTDIR)\OpComm.obj" \
	"$(INTDIR)\OpCRC32.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\list.res"

"$(OUTDIR)\list.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "list - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\list.exe" "$(OUTDIR)\list.bsc"


CLEAN :
	-@erase "$(INTDIR)\ColorListBox.obj"
	-@erase "$(INTDIR)\ColorListBox.sbr"
	-@erase "$(INTDIR)\CommandSFPVOA.obj"
	-@erase "$(INTDIR)\CommandSFPVOA.sbr"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\list.pch"
	-@erase "$(INTDIR)\list.res"
	-@erase "$(INTDIR)\list.sbr"
	-@erase "$(INTDIR)\listDlg.obj"
	-@erase "$(INTDIR)\listDlg.sbr"
	-@erase "$(INTDIR)\OpComm.obj"
	-@erase "$(INTDIR)\OpComm.sbr"
	-@erase "$(INTDIR)\OpCRC32.obj"
	-@erase "$(INTDIR)\OpCRC32.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\list.bsc"
	-@erase "$(OUTDIR)\list.exe"
	-@erase "$(OUTDIR)\list.ilk"
	-@erase "$(OUTDIR)\list.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\list.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\list.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\list.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ColorListBox.sbr" \
	"$(INTDIR)\CommandSFPVOA.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\listDlg.sbr" \
	"$(INTDIR)\OpComm.sbr" \
	"$(INTDIR)\OpCRC32.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\list.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\list.pdb" /debug /machine:I386 /out:"$(OUTDIR)\list.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ColorListBox.obj" \
	"$(INTDIR)\CommandSFPVOA.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\listDlg.obj" \
	"$(INTDIR)\OpComm.obj" \
	"$(INTDIR)\OpCRC32.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\list.res"

"$(OUTDIR)\list.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("list.dep")
!INCLUDE "list.dep"
!ELSE 
!MESSAGE Warning: cannot find "list.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "list - Win32 Release" || "$(CFG)" == "list - Win32 Debug"
SOURCE=.\ColorListBox.cpp

"$(INTDIR)\ColorListBox.obj"	"$(INTDIR)\ColorListBox.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\list.pch"


SOURCE=.\CommandSFPVOA.cpp

"$(INTDIR)\CommandSFPVOA.obj"	"$(INTDIR)\CommandSFPVOA.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\list.pch"


SOURCE=.\list.cpp

"$(INTDIR)\list.obj"	"$(INTDIR)\list.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\list.pch"


SOURCE=.\list.rc

"$(INTDIR)\list.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\listDlg.cpp

"$(INTDIR)\listDlg.obj"	"$(INTDIR)\listDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\list.pch"


SOURCE=.\OpComm.cpp

"$(INTDIR)\OpComm.obj"	"$(INTDIR)\OpComm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\list.pch"


SOURCE=.\OpCRC32.cpp

"$(INTDIR)\OpCRC32.obj"	"$(INTDIR)\OpCRC32.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\list.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "list - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\list.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\list.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "list - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\list.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\list.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

