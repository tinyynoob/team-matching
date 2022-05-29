#include <stdio.h>
#include <stdlib.h>
#include "matching.h"

// default: init with department.csv and partcipant.csv
struct matching *mh_init()
{
    struct matching *m = (struct matching *) malloc(sizeof(struct matching));
    FILE *df = fopen("department.csv", "r");
    FILE *pf = fopen("partcipant.csv", "r");
    fscanf(df, "%d ", &m->dpmt_size);
    fscanf(pf, "%d ", &m->pacpt_size);
    m->dpmt = (dpmt_t *) malloc(sizeof(dpmt_t) * m->dpmt_size);
    for (int de = 0; de < m->dpmt_size; de++) {
        fscanf(pf, "%d ", &m->dpmt[de].slot);
        m->dpmt[de].rank = (int *) malloc(sizeof(int) * m->pacpt_size);
        for (int i = 0; i < m->pacpt_size; i++) {
            int partcipant;
            fscanf(pf, "%d ", &partcipant);
            m->dpmt[de].rank[partcipant] = i;
        }
        m->dpmt[de].head = NULL;
        m->dpmt[de].memnum = 0;
    }
    fclose(df);
    m->pacpt = (pacpt_t *) malloc(sizeof(pacpt_t) * m->pacpt_size);
    for (int pa = 0; pa < m->pacpt_size; pa++) {
        m->pacpt[pa].prefer = (int *) malloc(sizeof(int) * m->dpmt_size);
        for (int i = 0; i < m->dpmt_size; i++)
            fscanf(pf, "%d ", &m->pacpt[pa].prefer[i]);
        m->pacpt[pa].progs = -1;
    }
    fclose(pf);
    return m;
}
