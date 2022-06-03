#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "matching.h"
#include "parse.h"

int main()
{
    struct whash *h_dpmt = whash_init(count_line("department.csv"));
    scan_wentry(h_dpmt, "department.csv");
    struct whash *h_pacpt = whash_init(count_line("participant.csv"));
    scan_wentry(h_pacpt, "participant.csv");
    if (!read_pacpt(h_pacpt, h_dpmt) || !read_dpmt(h_dpmt, h_pacpt))
        exit(1);
    struct matching *m = mh_init();
    while (apply(m)) {
    };
    result(m, h_dpmt, h_pacpt);
    mh_destory(m);
    m = NULL;
    whash_destory(h_pacpt);
    h_pacpt = NULL;
    whash_destory(h_dpmt);
    h_dpmt = NULL;
    remove("department.tmp");
    remove("participant.tmp");
    return 0;
}
