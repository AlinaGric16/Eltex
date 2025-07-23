#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <string.h>

#define MAX 100
#define INPUT_LEN 10

struct abonent {
    char name[INPUT_LEN];
    char second_name[INPUT_LEN];
    char tel[INPUT_LEN];
};

int add_subscriber(struct abonent *ptr);
int delete_subscriber(struct abonent *ptr);
void name_search(struct abonent *ptr);
void output_of_all_records(struct abonent *ptr);

void overflow(char *ptr);

#endif