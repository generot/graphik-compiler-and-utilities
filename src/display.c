#include "../include/util.h"
#include <time.h>

void draw_grid(HDC dc, int width, int height) {
    SetTextColor(dc, COL_BLACK);
    TextOutA(dc, width/2 - 60, 20, "Y axis", 7);
    TextOutA(dc, width - 70, height/2 + 20, "X axis", 7);

    for(int i = 0; i < height; i++)
        SetPixel(dc, width / 2, i, COL_WHITE);

    for(int i = 0; i < width; i++)
        SetPixel(dc, i, height / 2, COL_WHITE);
}

void clear_screen(HDC dc, int w, int h, COLORREF color) {
    RECT rc = {0, 0, w, h};
    HBRUSH br = CreateSolidBrush(color);

    FillRect(dc, &rc, br);
    DeleteObject(br);
}

void plot_file(HDC dc, char *filebuff) {
    clear_screen(dc, WIDTH, HEIGHT, COL_BLACK);
    draw_grid(dc, WIDTH, HEIGHT);

    char *line = strtok(filebuff, "\n");

    while(line) {
        int seen_space = 0, crd1 = 0, crd2 = 0;

        while(*line) {
            if(*line == ' ')
                seen_space = 1;

            if(IS_NUM(*line)) {
                if(!seen_space)
                    crd1 = crd1 * 10 + (*line) - '0';
                else
                   crd2 = crd2 * 10 + (*line) - '0';
            }

            line++;
        }

        SetPixel(dc, crd1 + WIDTH / 2, -crd2 + HEIGHT / 2, COL_PLOT);
        line = strtok(NULL, "\n");
    }
}

HWND UI_Button(HWND hwnd, LPSTR text, int x, int y, int width, int height) {
    return CreateWindowA(
        "BUTTON", 
        text, 
        WS_VISIBLE | WS_CHILD,
        x,
        y,
        width,
        height,
        hwnd,
        NULL,
        (HINSTANCE)GetWindowLongPtrA(hwnd, GWLP_HINSTANCE),
        NULL
    );
}

int plot_expr_file(HDC dc, char *filebuff) {
    char buff[MAX_TOKENS] = {0};
    int ix = 0;

    srand(time(0));

    while(*filebuff) {
        if(*filebuff == '\n') {
            COLORREF new_col = RGB(
                2 * rand() % 200 + 55, 
                2 * rand() % 200 + 55, 
                2 * rand() % 200 + 55
            );

            if(plot_function(dc, buff, new_col) == ERR)
                return ERR;

            memset(buff, 0, MAX_TOKENS);
            ix = 0;

            filebuff++;
        }

        buff[ix++] = *filebuff++;
    }
    return 0;
}

int plot_function(HDC dc, char *input_buffer, COLORREF col) {
    char *expr_tkn = tokenize(input_buffer);
    if(!expr_tkn) return ERR;

    double prevY = 0;

    for(double i = -WIDTH / 2; i < WIDTH / 2; i++) {
        init_parser(expr_tkn, i);
        
        double res = parse_term();

        Vector2 scrCoords = to_screen((Vector2){(int)i, (int)res}, WIDTH, HEIGHT);

        BOOL bool_res = prevY > scrCoords.y;

        size_t begin = bool_res ? scrCoords.y : prevY;
        size_t end = bool_res ? prevY : scrCoords.y;

        if(abs(begin - end) > 2 && abs(begin - end) < 100)
            for(size_t i = begin; i < end; i++)
               SetPixel(dc, scrCoords.x, i, col);

        prevY = scrCoords.y;

        SetPixelV(dc, scrCoords.x, scrCoords.y, col);
    }

    free(expr_tkn);
    return 0;
}

Vector2 to_screen(Vector2 coords, int w, int h) {
    return (Vector2){coords.x + w/2, -coords.y + h/2};
}
