
typedef struct partcipant pacpt_t;
struct partcipant {
    int *prefer;
    int progs;
};

typedef struct member_list memlis_t;
struct member_list {
    struct person;
    struct member_list *next;
};

typedef struct department dpmt_t;
struct department {
    int *rank;  //
    int slot;   // slot number
    int memnum; // current member number (memlis size)
    struct memlis *head;
};

struct matching {
    pacpt_t *pacpt;
    dpmt_t *dpmt;
};


