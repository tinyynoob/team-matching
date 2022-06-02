#include <locale.h>
#include <stdio.h>
#include "matching.h"
#include "parse.h"

int main()
{
    struct whash *h_dpmt = whash_init(count_line("department.csv"));
    struct whash *h_pacpt = whash_init(count_line("partcipant.csv"));
    in_pacpt(h_pacpt, h_dpmt);
    return 0;
}
