@echo off
REM === 编译准心覆盖程序 ===
REM 需要：Visual Studio 或 Build Tools (cl.exe + rc.exe)

REM 生成图标
python gen_icon.py
if errorlevel 1 (
    echo [ERROR] 需要 Python 来生成图标文件
    pause
    exit /b 1
)

REM 编译资源
rc resource.rc
if errorlevel 1 (
    echo [ERROR] 资源编译失败，请确保 rc.exe 在 PATH 中
    echo 提示：打开 "x64 Native Tools Command Prompt for VS" 再运行此脚本
    pause
    exit /b 1
)

REM 编译 C++ 源码
cl /O2 /EHsc /W4 main.cpp resource.res /Fe:crosshair.exe /link user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
if errorlevel 1 (
    echo [ERROR] 编译失败
    pause
    exit /b 1
)

echo.
echo ================================
echo   编译成功！生成 crosshair.exe
echo ================================
echo   按 ESC 或关闭窗口退出程序
echo ================================

del *.obj *.res 2>nul
pause
