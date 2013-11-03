SET PROGNAME=USB AVR-Lab Oszi
mkdir ..\..\output
mkdir ..\..\output\i386-win32
lazbuild ..\..\source\avrusblaboszi.lpi
copy ..\..\output\i386-win32\avrusblaboszi.exe 
strip --strip-debug avrusblaboszi.exe
strip --strip-all avrusblaboszi.exe

FOR /F %%L IN ('fpc -iTO') DO SET TARGETOS=%%L
FOR /F %%L IN ('fpc -iTP') DO SET TARGETCPU=%%L
FOR /F "delims='" %%F IN (..\..\source\version.inc) DO set VERSION=%%F
FOR /F "delims='" %%F IN (..\..\source\revision.inc) DO set VERSION=%VERSION%.%%F
SET FULLTARGET=%TARGETCPU%-%TARGETOS%-%VERSION%
iscc avrisptool.iss
del avrusblaboszi.exe