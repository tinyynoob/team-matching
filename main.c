#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "matching.h"
#include "parse.h"

int main()
{
    struct whash *h_dpmt = whash_init(count_line("department.csv"));
    scan_wentry(h_dpmt, "department.csv");
    struct whash *h_pacpt = whash_init(count_line("partcipant.csv"));
    scan_wentry(h_pacpt, "partcipant.csv");
    if (!read_pacpt(h_pacpt, h_dpmt) || !read_dpmt(h_dpmt, h_pacpt))
        exit(1);
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
