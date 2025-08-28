#include "ui.h"


int init_ui() {
    if (getcwd(left_panel.current_path, PATH_MAX) == NULL) {
        strcpy(left_panel.current_path, "/");
    }
    left_panel.selected_index = 0;
    left_panel.scroll_offset = 0;
    left_panel.file_list = NULL;
    left_panel.file_count = 0;
    left_panel.win = NULL;

    strncpy(right_panel.current_path, left_panel.current_path, PATH_MAX);
    right_panel.selected_index = 0;
    right_panel.scroll_offset = 0;
    right_panel.file_list = NULL;
    right_panel.file_count = 0;
    right_panel.win = NULL;

    if (update_panel_content(0) != 0) return 1;
    if (update_panel_content(1) != 0) return 1;

    active_panel = 0;
    return 0;
}

void create_panels() {
    int height = LINES - 3;
    int width = COLS / 2;

    left_panel.win = newwin(height, width, 1, 0);
    right_panel.win = newwin(height, width, 1, COLS / 2);

    wbkgd(left_panel.win, COLOR_PAIR(1));
    wbkgd(right_panel.win, COLOR_PAIR(1));
    
    left_panel.col1_win = left_panel.col2_win = left_panel.col3_win = left_panel.bottom_win = NULL;
    right_panel.col1_win = right_panel.col2_win = right_panel.col3_win = right_panel.bottom_win = NULL;
    
    create_subwindows(0);
    create_subwindows(1);
}

void create_subwindows(int panel_id) {
    Panel *p = (panel_id == 0) ? &left_panel : &right_panel;
    
    int height, width;
    getmaxyx(p->win, height, width);

    if (p->col1_win) delwin(p->col1_win);
    if (p->col2_win) delwin(p->col2_win);
    if (p->col3_win) delwin(p->col3_win);
    if (p->bottom_win) delwin(p->bottom_win);
    
    int col2_width = 7;
    int col3_width = 12;
    int col1_width = width - col2_width - col3_width - 4;

    p->col1_win = derwin(p->win, height - 5, col1_width, 2, 1);
    p->col2_win = derwin(p->win, height - 5, col2_width, 2, col1_width + 2);
    p->col3_win = derwin(p->win, height - 5, col3_width, 2, col1_width + col2_width + 3);
    p->bottom_win = derwin(p->win, 1, width - 2, height - 2, 1);

    wbkgd(p->col1_win, COLOR_PAIR(1));
    wbkgd(p->col2_win, COLOR_PAIR(1));
    wbkgd(p->col3_win, COLOR_PAIR(1));
    wbkgd(p->bottom_win, COLOR_PAIR(1));
}

void shorten_path(char *path) {
    char *home = getenv("HOME");
    if (home != NULL && strncmp(path, home, strlen(home)) == 0) {
        memmove(path + 1, path + strlen(home), strlen(path) - strlen(home) + 1);
        path[0] = '~';
    }
}

void draw_panel(int panel_id) {
    Panel *p = (panel_id == 0) ? &left_panel : &right_panel;
    int is_active = (panel_id == active_panel);

    int height, width;
    getmaxyx(p->win, height, width);

    wbkgd(p->win, COLOR_PAIR(1));
    werase(p->win);
    box(p->win, 0, 0);

    char title[width];
    char display_path[PATH_MAX];
    strcpy(display_path, p->current_path);
    shorten_path(display_path);

    if ((int)strlen(display_path) > width - 6) {
        snprintf(title, width - 4, " ...%s ", display_path + strlen(display_path) - (width - 10));
    } else {
        snprintf(title, width - 4, " %s ", display_path);
    }
    
    if (is_active) wattron(p->win, COLOR_PAIR(3));
    mvwprintw(p->win, 0, (width - strlen(title)) / 2, "%s", title);
    if (is_active) wattroff(p->win, COLOR_PAIR(3));

    int col2_width = 7;
    int col3_width = 12;
    int col1_width = width - col2_width - col3_width - 4;
    
    for (int y = 1; y < height - 3; y++) {
        mvwaddch(p->win, y, col1_width + 1, ACS_VLINE);
        mvwaddch(p->win, y, col1_width + col2_width + 2, ACS_VLINE);
    }
    
    mvwhline(p->win, height - 3, 1, ACS_HLINE, width - 2);

    wattron(p->win, COLOR_PAIR(4));
    mvwprintw(p->win, 1, 1, ".n");
    mvwprintw(p->win, 1, col1_width/2, "Name");
    mvwprintw(p->win, 1, col1_width + 3, "Size");
    mvwprintw(p->win, 1, col1_width + col2_width + 3, "Modify time");
    wattroff(p->win, COLOR_PAIR(4));

    werase(p->col1_win);
    werase(p->col2_win);
    werase(p->col3_win);

    int visible_lines = height - 5;
    int start_index = p->scroll_offset;
    int end_index = start_index + visible_lines;
    if (end_index > p->file_count) end_index = p->file_count;

    for (int i = start_index; i < end_index; i++) {
        int line_pos = i - start_index;

        struct stat file_stat;
        char full_path[PATH_MAX];
        int path_len = snprintf(full_path, PATH_MAX, "%s/%s", p->current_path, p->file_list[i]->d_name);
        
        if (path_len >= PATH_MAX) continue;
        if (stat(full_path, &file_stat) == -1) continue;

        if (is_active && i == p->selected_index) {
            wattron(p->col1_win, COLOR_PAIR(2));
            wattron(p->col2_win, COLOR_PAIR(2));
            wattron(p->col3_win, COLOR_PAIR(2));
            
            for (int x = 0; x < getmaxx(p->col1_win); x++) mvwaddch(p->col1_win, line_pos, x, ' ');
            for (int x = 0; x < getmaxx(p->col2_win); x++) mvwaddch(p->col2_win, line_pos, x, ' ');
            for (int x = 0; x < getmaxx(p->col3_win); x++) mvwaddch(p->col3_win, line_pos, x, ' ');
        }

        if (p->file_list[i]->d_type == DT_DIR) {
            mvwprintw(p->col1_win, line_pos, 0, "/");
        } else if (file_stat.st_mode & S_IXUSR) {
            mvwprintw(p->col1_win, line_pos, 0, "*");
        }
        
        char name_display[getmaxx(p->col1_win)];
        snprintf(name_display, getmaxx(p->col1_win) - 1, "%.*s", 
                getmaxx(p->col1_win) - 1, p->file_list[i]->d_name);
        mvwprintw(p->col1_win, line_pos, 1, "%s", name_display);

        if (strcmp(p->file_list[i]->d_name, "..") == 0) {
            mvwprintw(p->col2_win, line_pos, 0, "UP--DIR");
        } else {
            mvwprintw(p->col2_win, line_pos, 1, "%6ld", (long)file_stat.st_size);
        }

        char date_str[20];
        time_t mtime = file_stat.st_mtime;
        struct tm *tm_info = localtime(&mtime);
        strftime(date_str, sizeof(date_str), "%b %e %H:%M", tm_info);
        mvwprintw(p->col3_win, line_pos, 0, "%s", date_str);

        if (is_active && i == p->selected_index) {
            wattroff(p->col1_win, COLOR_PAIR(2));
            wattroff(p->col2_win, COLOR_PAIR(2));
            wattroff(p->col3_win, COLOR_PAIR(2));
        }
    }

    werase(p->bottom_win);
    if (p->file_count > 0 && p->selected_index < p->file_count) {
        if (strcmp(p->file_list[p->selected_index]->d_name, "..") == 0) {
            mvwprintw(p->bottom_win, 0, 0, "UP--DIR");
        } else {
            char info_name[getmaxx(p->bottom_win) - 20];
            snprintf(info_name, sizeof(info_name), "%.*s", 
                    getmaxx(p->bottom_win) - 21, p->file_list[p->selected_index]->d_name);
            mvwprintw(p->bottom_win, 0, 1, "%s", info_name);
        }
    }

    wrefresh(p->bottom_win);
    wrefresh(p->col1_win);
    wrefresh(p->col2_win);
    wrefresh(p->col3_win);
    wrefresh(p->win);
}