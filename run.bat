@echo off
echo Running T-GUI Framework...
cd build\bin
echo Current directory: %CD%
echo.
echo Checking for required files:
if exist t_gui_cpp.exe (echo [OK] t_gui_cpp.exe found) else (echo [ERROR] t_gui_cpp.exe not found)
if exist Qt5Core.dll (echo [OK] Qt5Core.dll found) else (echo [ERROR] Qt5Core.dll not found)
if exist Qt5Gui.dll (echo [OK] Qt5Gui.dll found) else (echo [ERROR] Qt5Gui.dll not found)
if exist Qt5Widgets.dll (echo [OK] Qt5Widgets.dll found) else (echo [ERROR] Qt5Widgets.dll not found)
if exist ..\platforms\qwindows.dll (echo [OK] qwindows.dll found) else (echo [ERROR] qwindows.dll not found)
echo.
echo Starting application...
t_gui_cpp.exe
echo.
echo Application exited with code: %ERRORLEVEL%
pause
