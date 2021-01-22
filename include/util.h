#ifndef __UTIL_H__
#define __UTIL_H__

#include <Windows.h>

#include <stdio.h>
#include <math.h>

///////////////DEFINITIONS//////////////////
#define MAX_TOKENS 300
#define GRID_MAX 100
#define ERR -1

#define INIT_GRID(dc) {                             \
    clear_screen(dc, WIDTH, HEIGHT, COL_BLACK);     \
    draw_grid(dc, WIDTH, HEIGHT);                   \
}

#define SET_UI_STATE(s) {           \
    ShowWindow(loadOutput, (s));    \
    ShowWindow(loadNotation, (s));  \
    ShowWindow(enterExpr, (s));     \
    ShowWindow(textBox, (s));       \
}

#define IS_NUM(x) ((x) >= '0' && (x) <= '9')
#define IS_OP(x) ((x) == '+' || (x) == '-' || (x) == '*' || (x) == '/' || (x) == '^')

#define COL_WHITE RGB(255, 255, 255)
#define COL_BLACK RGB(0, 0, 0)
#define COL_PLOT RGB(0, 255, 170)

#define MAX_LINE 100

#define UI_WIDTH 450
#define UI_HEIGHT 50

#define WIDTH 600
#define HEIGHT 600
///////////////DEFINITIONS//////////////////

typedef struct {
    int x, y;
} Vector2;

double parse_term();
double parse_factor();
double parse_number();
double parse_exp();

char *tokenize(char*);
char *read_file(char *filename);

void init_parser(char*, double);

void draw_grid(HDC, int, int);
void clear_screen(HDC, int, int, COLORREF);

void plot_file(HDC, char*);
int plot_function(HDC, char*, COLORREF);
int plot_expr_file(HDC, char*);

HWND UI_Button(HWND, LPSTR, int, int, int, int);

Vector2 to_screen(Vector2, int, int);

#endif
