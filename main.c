#include <stdio.h>
#include "matching.h"

int main()
{
    struct matching *m = mh_init();
    while (apply(m)) {
    };
    puts("partcipant\tdepartment");
    for (int pa = 0; pa < m->pacpt_size; pa++)
        printf("%d -- %d\n", pa, m->pacpt[pa].prefer[m->pacpt[pa].progs]);
    mh_destory(m);
    m = NULL;
    return 0;
}
