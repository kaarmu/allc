#include "_listitem/_listitem.h"
#include "list.h"
#include <string.h>

// Setting `p = last` is a step back itself.
#define LET_LISTITEM_AT(item, list, i)          \
    do {                                        \
        item = i < 0 ? self->last : self->head; \
        if (i < 0) { i += 1; }                  \
        item = allc_listitem_jump(item, i);     \
    } while(0)

// List {{{1
// ---------

List allc_list_new(Allocator allocator)
{
    return (List) {
        .length = 0,
        .head = NULL,
        .last = NULL,
        .allocator = allocator,
    };
}

void allc_list_del(List *self)
{
    for (ListItem *q, *p = self->head; p != NULL; p = q) {
        q = p->next;
        allc_listitem_del(self->allocator, p);
    }
}

// List Operations {{{2
// --------------------

void allc_list_append(List *self, size_t size, void *elem)
{
    ListItem *item = allc_listitem_new(self->allocator, size);
    memcpy(item->data, elem, size);
    allc_listitem_attach_after(item, self->last);
    self->last = item;
    self->length += 1;
    if (self->last->prev == NULL) { self->head = self->last; }
}

void allc_list_insert(List *self, ssize_t i, size_t size, void *elem)
{
    if (i == self->length) { return allc_list_append(self, size, elem); }
    ListItem *item, *other;
    LET_LISTITEM_AT(other, self, i);
    item = allc_listitem_new(self->allocator, size);
    allc_listitem_attach_before(item, other);
    memcpy(item->data, elem, size);
    self->length += 1;
}

// Element Operations {{{2
// -----------------------

void *allc_list_at(List *self, ssize_t i)
{
    ListItem *p;
    LET_LISTITEM_AT(p, self, i);
    return p == NULL ? NULL : p->data;
}

void allc_list_remove(List *self, ssize_t i)
{
    ListItem *p;
    LET_LISTITEM_AT(p, self, i);
    if (self->head == p) { self->head = p->next; }
    if (self->last == p) { self->last = p->prev; }
    allc_listitem_detach(p);
    self->length -= 1;
    allc_listitem_del(self->allocator, p);
}

void *allc_list_iter_forw(void *elem)
{
    ListItem *item = elem - sizeof (ListItem);
    return item->next == NULL ? NULL : item->next->data;
}

void *allc_list_iter_back(void *elem)
{
    ListItem *item = elem - sizeof (ListItem);
    return item->prev == NULL ? NULL : item->prev->data;
}
