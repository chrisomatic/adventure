@echo off
cls

set pyth=python.exe 
set scrpt=map_editor.pyw

where /q %pyth%
IF ERRORLEVEL 1 (
	set pp=\\S-filesrv-p-1\ERM\DEPO\SOFTWARE\SUPPORT\Anaconda\%pyth%
	set py=%cd%\%scrpt%
) ELSE (
    	set pp=%pyth%
	set py=%scrpt%
)


IF %pyth% == python.exe (
%pp% %py%
pause
) ELSE (
start %pp% %py%
)

