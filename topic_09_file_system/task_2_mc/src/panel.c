#include "panel.h"

Panel left_panel;
Panel right_panel;
int active_panel;

int update_panel_content(int panel_id) {
    Panel *p = (panel_id == 0) ? &left_panel : &right_panel;
    
    if (p->file_list != NULL) {
        for (int i = 0; i < p->file_count; i++) {
            free(p->file_list[i]);
        }
        free(p->file_list);
        p->file_list = NULL;
    }
    
    p->file_count = scandir(p->current_path, &p->file_list, dir_filter, mc_sort);
    
    if (p->file_count < 0) {
        p->file_count = 0;
        return -1;
    }
    
    return 0;
}

void move_selection(int panel_id, int direction) {
    Panel *p = (panel_id == 0) ? &left_panel : &right_panel;
    
    if (p->file_count == 0) return;
    
    int new_index = p->selected_index + direction;
    
    if (new_index >= 0 && new_index < p->file_count) {
        p->selected_index = new_index;
        
        int visible_lines = getmaxy(p->win) - 5;
        if (p->selected_index < p->scroll_offset) {
            p->scroll_offset = p->selected_index;
        } else if (p->selected_index >= p->scroll_offset + visible_lines) {
            p->scroll_offset = p->selected_index - visible_lines + 1;
        }
    }
}

void go_parent_directory(int panel_id) {
    Panel *p = (panel_id == 0) ? &left_panel : &right_panel;
    
    char *last_slash = strrchr(p->current_path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
        if (strlen(p->current_path) == 0) {
            strcpy(p->current_path, "/");
        }
        update_panel_content(panel_id);
        p->selected_index = 0;
        p->scroll_offset = 0;
    }
}

void enter_directory(int panel_id) {
    Panel *p = (panel_id == 0) ? &left_panel : &right_panel;
    
    if (p->file_count == 0 || p->selected_index >= p->file_count) return;
    
    char *selected_name = p->file_list[p->selected_index]->d_name;
    
    if (strcmp(selected_name, "..") == 0) {
        go_parent_directory(panel_id);
        return;
    } else if (p->file_list[p->selected_index]->d_type == DT_DIR) {
        char new_path[PATH_MAX];
        
        strncpy(new_path, p->current_path, PATH_MAX);
        new_path[PATH_MAX - 1] = '\0';
        
        size_t current_len = strlen(new_path);
        if (current_len > 0 && new_path[current_len - 1] != '/') {
            if (current_len < PATH_MAX - 1) {
                new_path[current_len] = '/';
                new_path[current_len + 1] = '\0';
                current_len++;
            } else {
                return;
            }
        }
        
        size_t name_len = strlen(selected_name);
        if (current_len + name_len < PATH_MAX - 1) {
            strcat(new_path, selected_name);
        } else {
            return;
        }
        
        DIR *dir = opendir(new_path);
        if (dir != NULL) {
            closedir(dir);
            strncpy(p->current_path, new_path, PATH_MAX);
            p->current_path[PATH_MAX - 1] = '\0';
            
            update_panel_content(panel_id);
            p->selected_index = 0;
            p->scroll_offset = 0;
        }
    }
}

int mc_sort(const struct dirent **a, const struct dirent **b) {
    int type_diff = ((*a)->d_type == DT_DIR ? 0 : 1) - ((*b)->d_type == DT_DIR ? 0 : 1);
    if (type_diff != 0) return type_diff;
    return strcasecmp((*a)->d_name, (*b)->d_name);
}

int dir_filter(const struct dirent *entry) {
    return strcmp(entry->d_name, ".") != 0;
}