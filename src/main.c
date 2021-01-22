#include "../include/util.h"
#include <string.h>

char *expr = NULL;
int ix = 0;

HWND loadOutput, loadNotation, enterExpr, textBox; 

LRESULT WINAPI wnd_proc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
    case WM_COMMAND:
        switch(HIWORD(wp)) {
        case BN_CLICKED:
            if((HWND)lp == loadOutput) {
                HDC dc = GetDC(wnd);
                char *fbuff = read_file(expr);

                if(!fbuff) {
                    MessageBoxA(NULL, "File not found.", "Error", MB_ICONERROR);
                    break;
                }

                SET_UI_STATE(SW_HIDE);
                plot_file(dc, fbuff);

                ReleaseDC(wnd, dc);
                free(fbuff);
            }
            else if((HWND)lp == loadNotation) {
                HDC dc = GetDC(wnd);

                char *fbuff = read_file(expr);
                if(!fbuff) {
                    MessageBoxA(NULL, "File not found.", "Error", MB_ICONERROR);
                    break;
                }

                SET_UI_STATE(SW_HIDE);
                INIT_GRID(dc);

                plot_expr_file(dc, fbuff);

                ReleaseDC(wnd, dc);
                free(fbuff);
            }
            else {
                HDC parentDC = GetDC(wnd);

                SET_UI_STATE(SW_HIDE);
                INIT_GRID(parentDC);

                plot_function(parentDC, expr, COL_PLOT);

                ReleaseDC(wnd, parentDC);
            }            
            
            break;
        }
        break;
    case WM_CHAR:
        if(wp == '\b' && ix > 0) {
            expr[--ix] = ' '; 
        }

        if(ix < 56) {
            HDC boxDC = GetDC(textBox);

            switch(wp) {
            case '\b':
            case 0x1B:
                break;
            default: expr[ix++] = wp;
            }

            TextOutA(boxDC, 10, 15, expr, strlen(expr) + 1);
            ReleaseDC(textBox, boxDC);
        }
        break;
    case WM_KEYDOWN:
        if(wp == VK_ESCAPE) {
            HDC dc = GetDC(wnd);

            clear_screen(dc, WIDTH, HEIGHT, COL_BLACK);
            RedrawWindow(wnd, NULL, NULL, RDW_INVALIDATE);

            ix = 0;
            memset(expr, 0, MAX_LINE);

            SET_UI_STATE(SW_SHOW);
            ReleaseDC(wnd, dc);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default: return DefWindowProcA(wnd, msg, wp, lp);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    FreeConsole();
    WNDCLASSA wndClass = {0};

    char buff[MAX_LINE] = {0};
    expr = buff;

    wndClass.hInstance = hInstance;
    wndClass.lpfnWndProc = &wnd_proc;
    wndClass.lpszClassName = "Calc";
    wndClass.hIcon = LoadImageA(NULL, "..\\img\\calc_icon.ico", IMAGE_ICON, 129, 143, LR_LOADFROMFILE);

    HBITMAP bground = (HBITMAP)LoadImageA(NULL, "..\\img\\menu.bmp", IMAGE_BITMAP, 
        WIDTH, HEIGHT, LR_LOADFROMFILE);

    wndClass.hbrBackground = CreatePatternBrush(bground);

    RegisterClassA(&wndClass);
    HWND wind = CreateWindowA(
        "Calc", 
        "Graphical Calculator", 
        WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX), 
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        WIDTH, 
        HEIGHT, 
        NULL, 
        NULL, 
        hInstance, 
        NULL
    );

    loadOutput = UI_Button(wind, "Load Graphik output", 70, 430, UI_WIDTH, UI_HEIGHT);
    loadNotation = UI_Button(wind, "Load expression file", 70, 370, UI_WIDTH, UI_HEIGHT);
    enterExpr = UI_Button(wind, "Enter expression", 70, 490, UI_WIDTH, UI_HEIGHT);
    textBox = CreateWindowA(
        "ListBox",
        "smth",
        WS_VISIBLE | WS_CHILD | WS_DISABLED,
        70,
        300,
        UI_WIDTH,
        UI_HEIGHT,
        wind,
        NULL,
        (HINSTANCE)GetWindowLongPtrA(wind, GWLP_HINSTANCE),
        NULL
    );

    ShowWindow(wind, nShowCmd);

    MSG msg = {0};
    while(GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    DeleteObject(wndClass.hbrBackground);
    DeleteObject(wndClass.hIcon);
    return 0;
}
