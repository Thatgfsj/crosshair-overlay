#include <windows.h>

#define WINDOW_CLASS  L"CrosshairOverlay"
#define WM_TRAYICON   (WM_USER + 1)
#define ID_TRAY_EXIT  1001
#define ID_SIZE_1     2006
#define ID_SIZE_2     2001
#define ID_SIZE_4     2002
#define ID_SIZE_6     2003
#define ID_SIZE_8     2004
#define ID_SIZE_10    2005
#define TIMER_TOPMOST 1
#define MUTEX_NAME    L"Global\\CrosshairOverlaySingleInstance"

NOTIFYICONDATA nid = {};
HMENU hTrayMenu = NULL;
HMENU hSizeMenu = NULL;
int g_radius = 2;
HWND g_hwnd = NULL;

void UpdateTrayTip() {
    wsprintf(nid.szTip, L"准心覆盖 [%dpx] - 右键设置", g_radius);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void RedrawCrosshair() {
    if (g_hwnd) {
        RedrawWindow(g_hwnd, NULL, NULL,
            RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        UpdateTrayTip();
    }
}

void ForceTopMost() {
    if (!g_hwnd) return;
    int sw = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int sh = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h  = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    SetWindowPos(g_hwnd, HWND_TOPMOST, sw, sh, w, h,
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND:
        return 1;  // 防止默认擦除导致闪烁，WM_PAINT 里自己清

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        // 用透明色（黑色）清底
        HBRUSH bg = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rc, bg);
        DeleteObject(bg);
        // 画白色准心
        int cx = rc.right / 2;
        int cy = rc.bottom / 2;
        HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);
        Ellipse(hdc, cx - g_radius, cy - g_radius, cx + g_radius + 1, cy + g_radius + 1);
        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_TIMER:
        if (wParam == TIMER_TOPMOST) {
            ForceTopMost();
        }
        return 0;
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            CheckMenuItem(hSizeMenu, ID_SIZE_1,  g_radius == 1  ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hSizeMenu, ID_SIZE_2,  g_radius == 2  ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hSizeMenu, ID_SIZE_4,  g_radius == 4  ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hSizeMenu, ID_SIZE_6,  g_radius == 6  ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hSizeMenu, ID_SIZE_8,  g_radius == 8  ? MF_CHECKED : MF_UNCHECKED);
            CheckMenuItem(hSizeMenu, ID_SIZE_10, g_radius == 10 ? MF_CHECKED : MF_UNCHECKED);
            TrackPopupMenu(hTrayMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN,
                pt.x, pt.y, 0, hwnd, NULL);
        }
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SIZE_1:  g_radius = 1;  RedrawCrosshair(); break;
        case ID_SIZE_2:  g_radius = 2;  RedrawCrosshair(); break;
        case ID_SIZE_4:  g_radius = 4;  RedrawCrosshair(); break;
        case ID_SIZE_6:  g_radius = 6;  RedrawCrosshair(); break;
        case ID_SIZE_8:  g_radius = 8;  RedrawCrosshair(); break;
        case ID_SIZE_10: g_radius = 10; RedrawCrosshair(); break;
        case ID_TRAY_EXIT:
            DestroyWindow(hwnd);
            break;
        }
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, TIMER_TOPMOST);
        Shell_NotifyIcon(NIM_DELETE, &nid);
        DestroyMenu(hTrayMenu);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    // 单实例：防止重复启动
    HANDLE hMutex = CreateMutex(NULL, TRUE, MUTEX_NAME);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(NULL, L"准心已在运行中，请查看右下角托盘图标。",
            L"准心覆盖", MB_ICONINFORMATION | MB_TOPMOST);
        return 0;
    }

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    RegisterClassEx(&wc);

    // 虚拟屏幕尺寸，支持多显示器
    int sx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int sy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int sw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int sh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    g_hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        WINDOW_CLASS, L"Crosshair",
        WS_POPUP,
        sx, sy, sw, sh,
        NULL, NULL, hInstance, NULL
    );

    SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // 每秒强制置顶
    SetTimer(g_hwnd, TIMER_TOPMOST, 1000, NULL);

    // 系统托盘
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = g_hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    UpdateTrayTip();
    Shell_NotifyIcon(NIM_ADD, &nid);

    // 大小子菜单
    hSizeMenu = CreatePopupMenu();
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_1,  L"极小 (1px)");
    AppendMenu(hSizeMenu, MF_STRING | MF_CHECKED,  ID_SIZE_2,  L"小 (2px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_4,  L"中 (4px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_6,  L"大 (6px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_8,  L"特大 (8px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_10, L"巨无霸 (10px)");

    // 主菜单
    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_POPUP, (UINT_PTR)hSizeMenu, L"准心大小");
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"退出准心");

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    ForceTopMost();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CloseHandle(hMutex);
    return 0;
}
