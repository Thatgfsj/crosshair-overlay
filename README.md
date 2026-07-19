# Crosshair Overlay

轻量级屏幕准心覆盖工具，适合 FPS 游戏使用。

## 功能

- 屏幕正中央显示白色准心点
- 全屏置顶 + 鼠标穿透，不影响游戏操作
- 系统托盘图标，右键菜单可选 6 种准心大小（1px ~ 10px）
- 托盘提示实时显示当前大小
- 多显示器支持
- 单实例保护，不会重复启动
- 单文件 exe，约 80KB

## 使用

双击 `crosshair.exe` 即可运行。

右键右下角托盘图标：
- **准心大小** — 切换 6 种尺寸
- **退出准心** — 关闭程序

> ⚠️ 游戏需设为**无边框窗口全屏**模式，独占全屏下覆盖窗口不可见。

## 编译

需要 [LLVM-MinGW](https://github.com/mstorsjo/llvm-mingw) 或 MSYS2 工具链。

```bash
# 生成图标（需要 Python，仅一次）
python gen_icon.py

# 编译
windres resource.rc -o resource.o
g++ -O2 -mwindows -DUNICODE -D_UNICODE -o crosshair.exe main.cpp resource.o -lgdi32 -lshell32
```

或直接运行 `build.bat`。

## License

MIT
