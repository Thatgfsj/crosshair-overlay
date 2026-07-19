@echo off
REM === 编译准心覆盖程序 ===
REM 需要：LLVM-MinGW 或 MSYS2 (g++ + windres)

REM 生成图标（需要 Python）
py gen_icon.py 2>nul || echo [跳过] 需要 Python 生成图标，使用已有 ico

REM 编译资源
windres resource.rc -o resource.o
if errorlevel 1 (
    echo [ERROR] 资源编译失败
    pause
    exit /b 1
)

REM 编译
g++ -O2 -mwindows -DUNICODE -D_UNICODE -o crosshair.exe main.cpp resource.o -lgdi32 -lshell32
if errorlevel 1 (
    echo [ERROR] 编译失败
    pause
    exit /b 1
)

echo.
echo ================================
echo   编译成功: crosshair.exe
echo ================================

del *.o 2>nul
pause
