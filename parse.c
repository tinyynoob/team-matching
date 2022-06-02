#include "parse.h"
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "log.h"

/* Read partcipant.csv and convert it to partcipant.tmp.
 * Return a whash struct, which stores some information from original csv.
 */
struct whash *incsv()
{
    setlocale(LC_ALL, "zh_TW.UTF-8");
    struct whash *h_pacpt = (struct whash *) malloc(sizeof(struct whash));
    struct whash *h_dpmt = (struct whash *) malloc(sizeof(struct whash));
    wchar_t buf[MAXLINELEN];
    /* First-pass: Computing size for our hash tables */
    FILE *pf = fopen("partcipant.csv", "r");
    uint32_t sz = 0;
    while (fgetws(buf, MAXLINELEN, pf))
        sz++;
    fclose(pf);
    h_pacpt->capacity = 1u << (log(sz) + 1);
    h_pacpt->serial = 0;
    h_pacpt->table =
        (struct wentry *) calloc(sizeof(struct wentry), h_pacpt->capacity);

    FILE *df = fopen("department.csv", "r");
    sz = 0;
    while (fgetws(buf, MAXLINELEN, df))
        sz++;
    fclose(df);
    h_dpmt->capacity = 1u << (log(sz) + 1);
    h_dpmt->serial = 0;
    h_dpmt->table =
        (struct wentry *) calloc(sizeof(struct wentry), h_dpmt->capacity);
    /* Second-pass */

    // developing...
    // How to return 2 pointer?
}

/* If found, return the number of the wentry.
 * Otherwise not found, return UINT32_MAX.
 */
uint32_t whash_search(struct whash *h, const wchar_t *key)
{
    uint32_t value = whash_func(key) & (h->capacity - 1);
    while (h->table + value) {  // if it points to non-NULL
        if (!wcscmp(h->table[value].name, key))
            return h->table[value].number;
        value = (value + 1) & (h->capacity - 1);
    }
    return UINT32_MAX;
}

bool whash_insert(struct whash *h, const wchar_t *key)
{
    if (h->serial == h->capacity)  // no slot available
        return false;
    uint32_t value = whash_func(key) & (h->capacity - 1);
    while (h->table + value)  // if it points to non-NULL
        value = (value + 1) & (h->capacity - 1);
    wcscpy(h->table[value].name, key);
    h->table[value].number = h->serial++;
    return true;
}
