#include <windows.h>

#define WINDOW_CLASS  L"CrosshairOverlay"
#define WM_TRAYICON   (WM_USER + 1)
#define ID_TRAY_EXIT  1001
#define ID_SIZE_2     2001
#define ID_SIZE_4     2002
#define ID_SIZE_6     2003
#define ID_SIZE_8     2004
#define ID_SIZE_1     2006
#define ID_SIZE_10    2005

NOTIFYICONDATA nid = {};
HMENU hTrayMenu = NULL;
HMENU hSizeMenu = NULL;
int g_radius = 2;  // 默认准心半径
HWND g_hwnd = NULL;

void RedrawCrosshair() {
    if (g_hwnd) {
        InvalidateRect(g_hwnd, NULL, TRUE);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);
        int cx = rc.right / 2;
        int cy = rc.bottom / 2;
        HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        SelectObject(hdc, brush);
        SelectObject(hdc, pen);
        Ellipse(hdc, cx - g_radius, cy - g_radius, cx + g_radius + 1, cy + g_radius + 1);
        DeleteObject(brush);
        DeleteObject(pen);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            // 更新菜单勾选状态
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
            Shell_NotifyIcon(NIM_DELETE, &nid);
            DestroyWindow(hwnd);
            break;
        }
        return 0;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    RegisterClassEx(&wc);

    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);

    g_hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        WINDOW_CLASS, L"Crosshair",
        WS_POPUP,
        0, 0, sw, sh,
        NULL, NULL, hInstance, NULL
    );

    SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    // 系统托盘图标
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = g_hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    lstrcpy(nid.szTip, L"准心覆盖 - 右键设置");
    Shell_NotifyIcon(NIM_ADD, &nid);

    // 大小子菜单
    hSizeMenu = CreatePopupMenu();
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_1,  L"极小 (1px)");
    AppendMenu(hSizeMenu, MF_STRING | MF_CHECKED,  ID_SIZE_2,  L"小 (2px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_4,  L"中 (4px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_6,  L"大 (6px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_8,  L"特大 (8px)");
    AppendMenu(hSizeMenu, MF_STRING,               ID_SIZE_10, L"巨无霸 (10px)");

    // 主右键菜单
    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_POPUP, (UINT_PTR)hSizeMenu, L"准心大小");
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"退出准心");

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, sw, sh, SWP_SHOWWINDOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
