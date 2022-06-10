#ifndef __MATCHING_H
#define __MATCHING_H

#include <stdbool.h>

typedef struct participant pacpt_t;
struct participant {
    int *prefer;  // [order] = department
    int progs;
};

typedef struct member_list memlis_t;
struct member_list {
    int member;  // member id
    struct member_list *next;
};

typedef struct department dpmt_t;
struct department {
    int *rank;   // [participant] = rank
    int slot;    // slot number (memlis max capacity)
    int memnum;  // current member number (memlis size)
    memlis_t *head;
};

typedef struct unemployed_list umplis_t;
struct unemployed_list {
    int participant;
    struct unemployed_list *next;
};

struct matching {
    int pacpt_size;
    pacpt_t *pacpt;
    int dpmt_size;
    dpmt_t *dpmt;
    umplis_t *umpl;
};


struct matching *mh_init();
void mh_destory(struct matching *m);
int mh_getTeam(struct matching *m, int pa);
int mh_apply(struct matching *m);
int dpmt_add_member(dpmt_t *dpmt, int pacpt);
int dpmt_remove_member(dpmt_t *dpmt);
#endif