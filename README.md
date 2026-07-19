# Crosshair Overlay

轻量级屏幕准心覆盖工具，适合 FPS 游戏使用。

## 功能

- 屏幕正中央显示白色准心点
- 全屏置顶，鼠标穿透，不影响游戏操作
- 系统托盘图标，右键菜单可选准心大小（1px ~ 10px）
- 单文件 exe，带图标

## 使用

双击 `crosshair.exe` 即可运行。

右键右下角托盘图标：
- **准心大小** — 切换 6 种尺寸
- **退出准心** — 关闭程序

## 编译

需要 [MSYS2/MinGW](https://www.msys2.org/) 或 LLVM-MinGW 工具链。

```bash
windres resource.rc -o resource.o
g++ -O2 -mwindows -DUNICODE -D_UNICODE -o crosshair.exe main.cpp resource.o -lgdi32 -lshell32
```

生成图标需要 Python（运行一次 `python gen_icon.py`）。

## 截图

准心显示为屏幕正中的一个小白点，游戏里看起来像这样：

```
        ┃
    ━━━━●━━━━
        ┃
```

## License

MIT
