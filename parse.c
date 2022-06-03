#include "parse.h"
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // memset
#include <wchar.h>
#include <wctype.h>  // iswdigit
#include "log.h"

#define DEBUG 1

/* Read partcipant.csv and convert it to partcipant.tmp.
 * According to the information from csv, add data into the hash.
 */
bool read_pacpt(struct whash *h_pacpt, struct whash *h_dpmt)
{
    setlocale(LC_ALL, "zh_TW.UTF-8");
    FILE *fout = fopen("partcipant.tmp", "w");
    fprintf(fout, "%u\n", h_pacpt->size);
    FILE *fin = fopen("partcipant.csv", "r");
    wchar_t buf[MAXLINELEN];
    int32_t row = 0;  // first row has row 1
    bool *dup_check = (bool *) calloc(sizeof(bool), h_dpmt->capacity);
    while (fgetws(buf, MAXLINELEN, fin)) {
        row++;
        wchar_t *state;
        wchar_t *entry = wcstok(buf, L",\t\n", &state);
        if (!entry)
            continue;
        if (whash_search(h_pacpt, entry) == UINT32_MAX) {  // if not found
            fwprintf(stderr,
                     L"Error: At row %d, \"%ls\" is new. Please retry.\n", row,
                     entry);
            goto fail;
        }

        memset(dup_check, 0, sizeof(bool) * h_dpmt->capacity);
        while ((entry = wcstok(NULL, L",\t\n", &state))) {
            uint32_t num = whash_search(h_dpmt, entry);
            if (num == UINT32_MAX) {  // if not found
                fwprintf(stderr,
                         L"Error: \"%ls\" at row %d in partcipant.csv is not "
                         L"an entry in department.csv.\n",
                         entry, row);
                goto fail;
            } else if (dup_check[num]) {  // if duplicate
                fwprintf(stderr,
                         L"Error: There is duplicate \"%ls\" at row %d.\n",
                         entry, row);
                goto fail;
            } else {
                dup_check[num] = true;
                fprintf(fout, "%u ", num);
            }
        }
        fputc('\n', fout);
    }
    fclose(fin);
    fclose(fout);
    free(dup_check);
    return true;
fail:;
    fclose(fin);
    fclose(fout);
    free(dup_check);
    return false;
}

/* Read department.csv and convert it to department.tmp.
 * According to the information from csv, add data into the hash.
 */
bool read_dpmt(struct whash *h_dpmt, struct whash *h_pacpt)
{
    setlocale(LC_ALL, "zh_TW.UTF-8");
    FILE *fout = fopen("department.tmp", "w");
    fprintf(fout, "%u\n", h_dpmt->size);
    FILE *fin = fopen("department.csv", "r");
    wchar_t buf[MAXLINELEN];
    int32_t row = 0;  // first row has row 1
    bool *dup_check = (bool *) calloc(sizeof(bool), h_pacpt->capacity);
    while (fgetws(buf, MAXLINELEN, fin)) {
        row++;
        wchar_t *state;
        wchar_t *entry = wcstok(buf, L",\n", &state);
        if (!entry)
            continue;
        if (whash_search(h_dpmt, entry) == UINT32_MAX) {  // if not found
            fwprintf(stderr,
                     L"Error: At row %d, \"%ls\" is new. Please retry.\n", row,
                     entry);
            goto fail;
        }

        entry = wcstok(NULL, L",\t\n", &state);
        for (int i = 0; i < wcslen(entry); i++) {
            if (!iswdigit(entry[i])) {
                fwprintf(stderr,
                         L"Error: \"%ls\" at row %d is not a valid number.\n",
                         entry, row);
                goto fail;
            }
        }
        fprintf(fout, "%u ", (uint32_t) wcstoul(entry, NULL, 10));

        memset(dup_check, 0, sizeof(bool) * h_pacpt->capacity);
        while ((entry = wcstok(NULL, L",\t\n", &state))) {
            uint32_t num = whash_search(h_pacpt, entry);
            if (num == UINT32_MAX) {  // if not found
                fwprintf(stderr,
                         L"Error: \"%ls\" at row %d in department.csv is not "
                         L"an entry in partcipant.csv.\n",
                         entry, row);
                goto fail;
            } else if (dup_check[num]) {  // if duplicate
                fwprintf(stderr,
                         L"Error: There is duplicate \"%ls\" at row %d.\n",
                         entry, row);
                goto fail;
            } else {
                dup_check[num] = true;
                fprintf(fout, "%u ", num);
            }
        }
        fputc('\n', fout);
    }
    fclose(fin);
    fclose(fout);
    free(dup_check);
    return true;
fail:;
    fclose(fin);
    fclose(fout);
    free(dup_check);
    return false;
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
    if (ans > MAXLINENUM)
        fprintf(stderr, "File %s has too many lines. May cause error.\n",
                pathname);
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

/*
 * Scan the csv file and insert the entries to their whash struct.
 */
bool scan_wentry(struct whash *h, const char *pathname)
{
    FILE *f = fopen(pathname, "r");
    wchar_t buf[MAXLINELEN];
    int32_t row = 0;  // first row has row 1
    while (fgetws(buf, MAXLINELEN, f)) {
        row++;
        wchar_t *state;
        wchar_t *entry = wcstok(buf, L",\t\n", &state);
        if (!entry)
            continue;
        if (whash_search(h, entry) != UINT32_MAX) {
            fwprintf(stderr, L"Error: \"%ls\" duplicate at row %d.\n", entry,
                     row);
            fclose(f);
            return false;
        } else if (whash_insert(h, entry) == UINT32_MAX) {
            fprintf(stderr, "The file size seems to be changed.\n");
            fclose(f);
            return false;
        }
    }
    fclose(f);
    return true;
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
