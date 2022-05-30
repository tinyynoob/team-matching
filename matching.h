
typedef struct partcipant pacpt_t;
struct partcipant {
    int *prefer;    // [order] = department
    int progs;
};

typedef struct member_list memlis_t;
struct member_list {
    int member;
    struct member_list *next;
};

typedef struct department dpmt_t;
struct department {
    int *rank;  // [partcipant] = rank
    int slot;   // slot number (memlis max capacity)
    int memnum; // current member number (memlis size)
    memlis_t *head;
};

struct matching {
    int pacpt_size;
    pacpt_t *pacpt;
    int dpmt_size;
    dpmt_t *dpmt;
};


struct matching *mh_init();
bool apply(struct matching *m, int pacpt);
bool dpmt_add_member(dpmt_t *dpmt, int pacpt);
bool dpmt_remove_member(dpmt_t *dpmt);