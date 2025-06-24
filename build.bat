@echo off
echo Building T-GUI Framework...

REM Set CMake, MinGW and Qt paths
set CMAKE_PATH=C:/CMake/bin/cmake.exe
set MINGW_PATH=C:\mingw_distro161\MinGW\bin
set QT_PATH=E:\Qt\Qt5.14.2\5.14.2\mingw73_64

REM Add MinGW and Qt to PATH
set PATH=%MINGW_PATH%;%QT_PATH%\bin;%PATH%

REM Check if CMake exists
if not exist "%CMAKE_PATH%" (
    echo Error: CMake not found at %CMAKE_PATH%
    echo Please install CMake or update the path in this script
    pause
    exit /b 1
)

REM Check if MinGW exists
if not exist "%MINGW_PATH%\gcc.exe" (
    echo Error: MinGW not found at %MINGW_PATH%
    echo Please install MinGW or update the path in this script
    pause
    exit /b 1
)

REM Check if Qt exists
if not exist "%QT_PATH%\lib\cmake\Qt5" (
    echo Error: Qt5 not found at %QT_PATH%
    echo Please install Qt5 or update the path in this script
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure project
echo Configuring project...
"%CMAKE_PATH%" .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_PATH%"
if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

REM Build project
echo Building project...
"%CMAKE_PATH%" --build .
if %ERRORLEVEL% neq 0 (
    echo Error: Build failed
    pause
    exit /b 1
)

REM Copy Qt DLLs
echo Copying Qt DLLs...
copy "%QT_PATH%\bin\Qt5Core.dll" bin\ >nul 2>&1
copy "%QT_PATH%\bin\Qt5Gui.dll" bin\ >nul 2>&1
copy "%QT_PATH%\bin\Qt5Widgets.dll" bin\ >nul 2>&1
copy "%QT_PATH%\bin\Qt5OpenGL.dll" bin\ >nul 2>&1

REM Copy MinGW DLLs
copy "%MINGW_PATH%\libgcc_s_seh-1.dll" bin\ >nul 2>&1
copy "%MINGW_PATH%\libstdc++-6.dll" bin\ >nul 2>&1
copy "%MINGW_PATH%\libwinpthread-1.dll" bin\ >nul 2>&1

echo Build completed successfully!
echo Executable location: build\bin\t_gui_cpp.exe
echo.
echo Running the application...
bin\t_gui_cpp.exe
pause
