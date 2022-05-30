#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
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

/* @pacpt applies to his next department.
 * Return true if accepted.
 * Return false if rejected.
 */
bool apply(struct matching *m, int pacpt)
{

}

/* Insert pacpt to dpmt->head and keep the list in descending order.
 * Return false if the list is full and cannot be inserted.
 */
bool dpmt_add_member(dpmt_t *dpmt, int pacpt)
{
    if (dpmt->memnum == dpmt->slot)
        return false;
    memlis_t **it = &dpmt->head;
    while (*it && dpmt->rank[pacpt] <= dpmt->rank[(*it)->member]) {
        assert(dpmt->rank[pacpt] != dpmt->rank[(*it)->member]);
        it = &(*it)->next;
    }
    memlis_t *new = (memlis_t *) malloc(sizeof(memlis_t));
    new->member = pacpt;
    new->next = *it;
    *it = new;
    return true;
}

/* Delete the front node in dpmt->list.
 * Return false if the list is originally empty.
 */
bool dpmt_remove_member(dpmt_t *dpmt)
{
    if (!dpmt->head)
        return false;
    memlis_t *tmp = dpmt->head;
    dpmt->head = dpmt->head->next;
    free(tmp);
    return true;
}
