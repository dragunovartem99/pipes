// gcc pipes.c -o pipes -lncursesw && ./pipes

#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>
#include <signal.h>

static void quit(int sig) { endwin(); exit(0); }

static const char *LOGO[] = { "┏━┓╻┏━┓┏━╸┏━┓",
                              "┣━┛┃┣━┛┣╸ ┗━┓",
                              "╹  ╹╹  ┗━╸┗━┛" };

static const int DX[] = { 0, 1, 0, -1 };
static const int DY[] = { -1, 0, 1, 0 };

static const char *PIPE[4][4] = { { "║", "╔", 0, "╗" },
                                  { "╝", "═", "╗", 0 },
                                  { 0, "╚", "║", "╝" },
                                  { "╚", 0, "╔", "═" } };

int main(void)
{
    setlocale(LC_ALL, "");
    signal(SIGINT, quit);
    initscr(); cbreak(); noecho(); curs_set(0);
    start_color(); use_default_colors();
    for (int i = 1; i <= 6; i++) init_pair(i, i, -1);
    init_pair(7, COLOR_WHITE, -1);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    srand(time(NULL));

    int logo_w = 13, logo_h = 3;
    int logo_y = rows / 2 - 1, logo_x = cols / 2 - logo_w / 2;
    int top  = logo_y - 1, bottom = logo_y + logo_h;
    int left = logo_x - 2, right  = logo_x + logo_w + 2;

    attrset(COLOR_PAIR(7) | A_BOLD);
    for (int i = 0; i < logo_h; i++)
        mvaddstr(logo_y + i, logo_x, LOGO[i]);
    attrset(A_NORMAL);

    int x = 0, y = 0, dir = 1, steps = 0;

    while (1) {
        if (!steps) {
            x = rand() % cols; y = rand() % rows;
            dir = rand() % 4; steps = 20 + rand() % 30;
            attrset(COLOR_PAIR(rand() % 6 + 1) | A_BOLD);
        }

        int next_dir = rand() % 3 ? dir : (dir + (rand() % 2 ? 1 : 3)) % 4;
        int nx = x + DX[next_dir], ny = y + DY[next_dir];
        int blocked = (nx < 0 || nx >= cols || ny < 0 || ny >= rows) ||
                      (ny >= top && ny <= bottom && nx >= left && nx < right);

        if (blocked) { steps = 0; continue; }

        mvaddstr(ny, nx, PIPE[dir][next_dir]);
        refresh();
        x = nx; y = ny; dir = next_dir;

        if (--steps) napms(30);
    }
    endwin();
}
