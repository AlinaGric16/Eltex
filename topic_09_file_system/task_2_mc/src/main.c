#include "ui.h"
#include "panel.h"

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    start_color();
    
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_YELLOW, COLOR_BLUE);

    if (init_ui() != 0) {
        endwin();
        return 1;
    }

    create_panels();
    refresh();

    int ch;
    while (1) {
        draw_panel(0);
        draw_panel(1);
        
        attron(COLOR_PAIR(2));
        mvprintw(LINES - 1, (COLS - 40)/2, " q - Exit | Tab - Switch | Enter - Open ");
        attroff(COLOR_PAIR(2));
        refresh();
        
        ch = getch();
        if (ch == 'q') break;
        
        switch (ch) {
            case KEY_UP:    move_selection(active_panel, -1); break;
            case KEY_DOWN:  move_selection(active_panel, 1); break;
            case '\t':      active_panel = !active_panel; break;
            case '\n':      enter_directory(active_panel); break;
            case KEY_ENTER: enter_directory(active_panel); break;
        }
    }

    delwin(left_panel.win);
    delwin(right_panel.win);
    for (int i = 0; i < left_panel.file_count; i++) free(left_panel.file_list[i]);
    free(left_panel.file_list);
    for (int i = 0; i < right_panel.file_count; i++) free(right_panel.file_list[i]);
    free(right_panel.file_list);
    
    endwin();
    return 0;
}