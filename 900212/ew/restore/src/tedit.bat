@ECHO OFF

SET TMPPATH=%PATH%
SET PATH=%PATH%;c:\c196\bin

d:
cd d:\sw_dev\visualstate\projectm16motor\ew\src

if "%1"=="build" goto build
if "%1"=="rebuild" goto rebuild

ftee mk32 %1 -a -r -f tedit.mak > tedit.err
goto end

:build
ftee mk32 -r -f tedit.mak > tedit.err
goto end

:rebuild
ftee mk32 -a -r -f tedit.mak > tedit.err

:end
SET PATH=%TMPPATH%
