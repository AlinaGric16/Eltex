#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_LEN 10

struct abonent {
    char name[INPUT_LEN];
    char second_name[INPUT_LEN];
    char tel[INPUT_LEN];
};

struct node {
    struct abonent data;
    struct node *prev;
    struct node *next;
};

void add_subscriber(struct node **ptr);
int delete_subscriber(struct node **ptr);
void name_search(struct node *ptr);
void output_of_all_records(struct node *ptr);
void completion(struct node *ptr);

void overflow(char *ptr);

#endif