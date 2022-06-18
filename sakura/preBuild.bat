@echo off

SETLOCAL

set EXEDIR=%1
set SRCDIR=..\sakura_core

@echo =======================
@echo preBuild
@echo =======================

copy /n "%SRCDIR%\hsp\res\UDEVGothic-Regular.ttf" "%EXEDIR%\UDEVGothic-Regular.ttf"