#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#define MAXLEN 32        // maximum length of each wchar-type name
#define MAXLINELEN 1024  // maximum length of one line in csv#include <stdint.h>

struct wentry {
    uint32_t number;
    wchar_t *name;
};

struct whash {
    uint32_t size;
    uint32_t capacity;
    struct wentry *table;
    uint32_t serial;  // serial number
};

// source: https://stackoverflow.com/questions/7666509/hash-function-for-string
static inline uint64_t whash_func(const wchar_t *key)
{
    uint64_t hash = 5381;
    int c;
    while (c = *(unsigned char *) key++)
        hash = ((hash << 5) + hash) + c;
    return hash;
}


void in_pacpt(struct whash *h_pacpt, struct whash *h_dpmt);
uint32_t count_line(const char *pathname);
struct whash *whash_init(uint32_t sz);
uint32_t whash_search(struct whash *h, const wchar_t *key);
uint32_t whash_insert(struct whash *h, const wchar_t *key);
