#include "matching.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1

// default: init with department.tmp and participant.tmp
struct matching *mh_init()
{
    struct matching *m = (struct matching *) malloc(sizeof(struct matching));
    FILE *df = fopen("department.tmp", "r");
    FILE *pf = fopen("participant.tmp", "r");
    fscanf(df, "%d ", &m->dpmt_size);
    fscanf(pf, "%d ", &m->pacpt_size);
    m->dpmt = (dpmt_t *) malloc(sizeof(dpmt_t) * m->dpmt_size);
    for (int de = 0; de < m->dpmt_size; de++) {
        fscanf(df, "%d ", &m->dpmt[de].slot);
        m->dpmt[de].rank = (int *) malloc(sizeof(int) * m->pacpt_size);
        for (int i = 0; i < m->pacpt_size; i++) {
            int participant;
            fscanf(df, "%d ", &participant);
            m->dpmt[de].rank[participant] = i;
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
        new->participant = pa;
        new->next = m->umpl;
        m->umpl = new;
    }
    fclose(pf);
    return m;
}

/* Return the department that @pa is currently in.
 */
int mh_getTeam(struct matching *m, int pa)
{
    return m->pacpt[pa].prefer[m->pacpt[pa].progs];
}

void mh_destory(struct matching *m)
{
    for (int i = 0; i < m->pacpt_size; i++)
        free(m->pacpt[i].prefer);
    free(m->pacpt);
    for (int i = 0; i < m->dpmt_size; i++) {
        free(m->dpmt[i].rank);
        while (dpmt_remove_member(&m->dpmt[i]) != -1) {
        }
    }
    free(m->dpmt);
    while (m->umpl) {
        umplis_t *tmp = m->umpl;
        m->umpl = m->umpl->next;
        free(tmp);
    }
    free(m);
}

/* Let the first guy in ump list apply to his next target.
 * Return value:
 * -1: Rejected.
 *  0: ump list is empty and thus nobody applies.
 * +1: Accepted.
 * Some guy would be added to ump list after this function.
 */
int mh_apply(struct matching *m)
{
    if (!m->umpl)
        return 0;
    const int pa = m->umpl->participant;
    ++m->pacpt[pa].progs;  // @pa's next target
    const int de = mh_getTeam(m, pa);
#if DEBUG
    printf("%d applies to %d\n", pa, de);
#endif
    int ret = dpmt_add_member(&m->dpmt[de], pa);
    if (ret == -1) {
        return -1;
    } else if (ret == 0) {
        int rm = dpmt_remove_member(&m->dpmt[de]);
        dpmt_add_member(&m->dpmt[de], pa);
        m->umpl->participant = rm;
        return +1;
    } else if (ret == 1) {
        umplis_t *tmp = m->umpl;
        m->umpl = m->umpl->next;
        free(tmp);
        return +1;
    }
    return -1;  // impossible
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
    dpmt->memnum++;
    return 1;
}

/* Delete the front node in dpmt->list.
 * Return -1 if the list is originally empty.
 * Otherwise, return the member being removed.
 */
int dpmt_remove_member(dpmt_t *dpmt)
{
    if (!dpmt->memnum)
        return -1;
    memlis_t *tmp = dpmt->head;
    int ret = tmp->member;
    dpmt->head = dpmt->head->next;
    free(tmp);
    dpmt->memnum--;
    return ret;
}
