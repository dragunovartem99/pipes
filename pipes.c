#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

static const char *LOGO[] = {
    "┏━┓╻┏━┓┏━╸┏━┓",
    "┣━┛┃┣━┛┣╸ ┗━┓",
    "╹  ╹╹  ┗━╸┗━┛"
};

static const int DX[] = {0, 1, 0, -1}, DY[] = {-1, 0, 1, 0};

static const char *PIPE[4][4] = {
    {"║", "╔", 0, "╗"}, {"╝", "═", "╗", 0},
    {0, "╚", "║", "╝"}, {"╚", 0, "╔", "═"}
};

int main(void)
{
    setlocale(LC_ALL, "");
    initscr(); cbreak(); noecho(); curs_set(0); nodelay(stdscr, TRUE);
    start_color(); use_default_colors();
    for (int i = 1; i <= 6; i++) init_pair(i, i, -1);
    init_pair(7, COLOR_WHITE, -1);

    int h, w;
    getmaxyx(stdscr, h, w);
    srand(time(NULL));

    int lw = 13, lh = 3, ly = h / 2 - 1, lx = w / 2 - lw / 2;

    attron(COLOR_PAIR(7) | A_BOLD);
    for (int i = 0; i < lh; i++) mvprintw(ly + i, lx, "%s", LOGO[i]);
    attroff(COLOR_PAIR(7) | A_BOLD);

    int x = 0, y = 0, dir = 1, col = 1, life = 0;

    while (getch() == ERR) {
        if (!life) {
            x = rand() % w; y = rand() % h;
            dir = rand() % 4; col = rand() % 6 + 1;
            life = 20 + rand() % 30;
        }
        int d = rand() % 3 ? dir : (dir + (rand() % 2 ? 1 : 3)) % 4;
        int nx = x + DX[d], ny = y + DY[d];
        if (nx < 0 || nx >= w || ny < 0 || ny >= h ||
            (ny >= ly - 1 && ny <= ly + lh && nx >= lx - 2 && nx < lx + lw + 2)) {
            life = 0; continue;
        }
        attron(COLOR_PAIR(col) | A_BOLD);
        mvprintw(ny, nx, "%s", PIPE[dir][d]);
        attroff(COLOR_PAIR(col) | A_BOLD);
        refresh();
        x = nx; y = ny; dir = d;
        if (!--life) continue;
        napms(30);
    }
    endwin();
}
