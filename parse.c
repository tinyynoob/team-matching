#include "parse.h"
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "log.h"

#define DEBUG 1

/* Read partcipant.csv and convert it to partcipant.tmp.
 * According to the information from csv, add data into the hash.
 */
void read_pacpt(struct whash *h_pacpt, struct whash *h_dpmt)
{
    setlocale(LC_ALL, "zh_TW.UTF-8");
    FILE *fout = fopen("partcipant.tmp", "w");
    fprintf(fout, "%u\n", h_pacpt->size);
    FILE *fin = fopen("partcipant.csv", "r");
    wchar_t buf[MAXLINELEN];
    while (fgetws(buf, MAXLINELEN, fin)) {
        wchar_t *state;
        wchar_t *entry = wcstok(buf, L",\n", &state);
        if (!entry)
            continue;
        uint32_t num = whash_search(h_pacpt, entry);
        if (num == UINT32_MAX) {  // if not found
            num = whash_insert(h_pacpt, entry);
            if (num == UINT32_MAX)  // if insertion failed
                exit(1);
        }

        while ((entry = wcstok(NULL, L",\n", &state))) {
            num = whash_search(h_dpmt, entry);
            if (num != UINT32_MAX) {
                fprintf(fout, "%u ", num);
            } else {
                num = whash_insert(h_dpmt, entry);
                if (num != UINT32_MAX)
                    fprintf(fout, "%u ", num);
                else  // insert failed
                    exit(1);
            }
        }
        fputc('\n', fout);
    }
    fclose(fin);
    fclose(fout);
}

/* Read department.csv and convert it to department.tmp.
 * According to the information from csv, add data into the hash.
 */
void read_dpmt(struct whash *h_dpmt, struct whash *h_pacpt)
{
    setlocale(LC_ALL, "zh_TW.UTF-8");
    FILE *fout = fopen("department.tmp", "w");
    fprintf(fout, "%u\n", h_dpmt->size);
    FILE *fin = fopen("department.csv", "r");
    wchar_t buf[MAXLINELEN];
    while (fgetws(buf, MAXLINELEN, fin)) {
        wchar_t *state;
        wchar_t *entry = wcstok(buf, L",\n", &state);
        if (!entry)
            continue;
        uint32_t num = whash_search(h_dpmt, entry);
        if (num == UINT32_MAX) {  // if not found
            num = whash_insert(h_dpmt, entry);
            if (num == UINT32_MAX)  // if insertion failed
                exit(1);
        }

        entry = wcstok(NULL, L",\n", &state);
        fprintf(fout, "%u ", (uint32_t) wcstoul(entry, NULL, 10));

        while ((entry = wcstok(NULL, L",\n", &state))) {
            num = whash_search(h_pacpt, entry);
            if (num != UINT32_MAX) {
                fprintf(fout, "%u ", num);
            } else {
                num = whash_insert(h_pacpt, entry);
                if (num != UINT32_MAX)
                    fprintf(fout, "%u ", num);
                else  // insert failed
                    exit(1);
            }
        }
        fputc('\n', fout);
    }
    fclose(fin);
    fclose(fout);
}

/* Return the line number of a file.
 */
uint32_t count_line(const char *pathname)
{
    setlocale(LC_ALL, "zh_TW.UTF-8");
    FILE *f = fopen(pathname, "r");
    uint32_t ans = 0;
    wchar_t buf[MAXLINELEN], *state;
    while (fgetws(buf, MAXLINELEN, f) && wcstok(buf, L"\n", &state))
        ans++;
    fclose(f);
#if DEBUG
    printf("%s has line %u.\n", pathname, ans);
#endif
    return ans;
}

/* Init the struct with empty table.
 */
struct whash *whash_init(uint32_t sz)
{
    struct whash *h = (struct whash *) malloc(sizeof(struct whash));
    h->size = sz;
    h->capacity = 1u << (log(h->size) + 1);
    h->serial = 0;
    h->table = (struct wentry *) malloc(sizeof(struct wentry) * h->capacity);
    for (int i = 0; i < h->capacity; i++) {
        h->table[i].name = NULL;
        h->table[i].number = UINT32_MAX;
    }
#if DEBUG
    printf("A whash is init with size = %u and capacity = %u.\n", h->size,
           h->capacity);
#endif
    return h;
}

/* If found, return the number of the wentry.
 * Otherwise not found, return UINT32_MAX.
 */
uint32_t whash_search(struct whash *h, const wchar_t *key)
{
#if DEBUG
    wprintf(L"Try to search %ls\n", key);
#endif
    uint32_t value = whash_func(key) & (h->capacity - 1);
    while (h->table[value].name) {
        if (!wcscmp(h->table[value].name, key))
            return h->table[value].number;
        value = (value + 1) & (h->capacity - 1);
    }
    return UINT32_MAX;
}

/* If the table is full, return UINT32_MAX.
 * Otherwise, return the number of the inserted wentry.
 */
uint32_t whash_insert(struct whash *h, const wchar_t *key)
{
#if DEBUG
    wprintf(L"Try to insert %ls\n", key);
#endif
    if (h->serial == h->capacity)  // no slot available
        return UINT32_MAX;
    uint32_t value = whash_func(key) & (h->capacity - 1);
    while (h->table[value].name)  // finding a slot
        value = (value + 1) & (h->capacity - 1);
    h->table[value].name = wcsdup(key);
    return h->table[value].number = h->serial++;
}
