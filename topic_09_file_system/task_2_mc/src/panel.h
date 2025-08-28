#ifndef PANEL_H
#define PANEL_H

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ncurses.h>
#include <unistd.h>

#define PATH_MAX 4096

typedef struct {
    char current_path[PATH_MAX];
    struct dirent **file_list;
    int file_count;
    int selected_index;
    int scroll_offset;
    WINDOW *win;
    WINDOW *col1_win;
    WINDOW *col2_win;
    WINDOW *col3_win;
    WINDOW *bottom_win;
} Panel;

extern Panel left_panel;
extern Panel right_panel;
extern int active_panel;

int update_panel_content(int panel_id);
void move_selection(int panel_id, int direction);
void enter_directory(int panel_id);
void go_parent_directory(int panel_id);
int mc_sort(const struct dirent **a, const struct dirent **b);
int dir_filter(const struct dirent *entry);

#endif