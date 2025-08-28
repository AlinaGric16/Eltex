#ifndef UI_H
#define UI_H

#include "panel.h"
#include <sys/stat.h>
#include <time.h>

void shorten_path(char *path);
void create_subwindows(int panel_id);
void create_panels();
void draw_panel(int panel_id);
int init_ui();

#endif