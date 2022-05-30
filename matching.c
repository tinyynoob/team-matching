#include "matching.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
    m->umpl = NULL;
    m->pacpt = (pacpt_t *) malloc(sizeof(pacpt_t) * m->pacpt_size);
    for (int pa = 0; pa < m->pacpt_size; pa++) {
        m->pacpt[pa].prefer = (int *) malloc(sizeof(int) * m->dpmt_size);
        for (int i = 0; i < m->dpmt_size; i++)
            fscanf(pf, "%d ", &m->pacpt[pa].prefer[i]);
        m->pacpt[pa].progs = -1;
        umplis_t *new = (umplis_t *) malloc(sizeof(umplis_t));
        new->partcipant = pa;
        new->next = m->umpl;
        m->umpl = new;
    }
    fclose(pf);
    return m;
}

void mh_destory(struct matching *m)
{
    for (int i = 0; i < m->pacpt_size; i++)
        free(m->pacpt[i].prefer);
    free(m->pacpt);
    for (int i = 0; i < m->dpmt_size; i++) {
        while (dpmt_remove_member(&m->dpmt[i])) {
        }
        free(m->dpmt[i].rank);
    }
    free(m->dpmt);
    while (m->umpl) {
        umplis_t *tmp = m->umpl;
        m->umpl = m->umpl->next;
        free(tmp);
    }
    free(m);
}

/* Return true if accepted.
 * Return false if rejected.
 */
bool apply(struct matching *m)
{

}

/* Insert pacpt to dpmt->head and keep the list in descending order.
 * Return value:
 * -1: The list is full and pacpt cannot replace anyone.
 *  0: The list is full but pacpt has chance to replace some member.
 * +1: Successed
 */
int dpmt_add_member(dpmt_t *dpmt, int pacpt)
{
    if (dpmt->memnum == dpmt->slot) {
        if (dpmt->memnum && dpmt->rank[pacpt] < dpmt->rank[dpmt->head->member])
            return 0;
        else
            return -1;
    }
    memlis_t **it = &dpmt->head;
    while (*it && dpmt->rank[pacpt] <= dpmt->rank[(*it)->member]) {
        assert(dpmt->rank[pacpt] != dpmt->rank[(*it)->member]);
        it = &(*it)->next;
    }
    memlis_t *new = (memlis_t *) malloc(sizeof(memlis_t));
    new->member = pacpt;
    new->next = *it;
    *it = new;
    return 1;
}

/* Delete the front node in dpmt->list.
 * Return -1 if the list is originally empty.
 * Otherwise, return the member being removed.
 */
int dpmt_remove_member(dpmt_t *dpmt)
{
    if (!dpmt->head)
        return -1;
    memlis_t *tmp = dpmt->head;
    int ret = tmp->member;
    dpmt->head = dpmt->head->next;
    free(tmp);
    return ret;
}
