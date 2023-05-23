#include "_listitem.h"
#include "../list.h"

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
}

// Element Operations {{{2
// -----------------------

void *allc_list_at(List *self, ssize_t i)
{
    ListItem *p =  i < 0 ? self->last : self->head;
    if (i < 0) { i += 1; } // Setting `p = last` is a step back itself
    p = allc_listitem_jump(p, i);
    return p->data;
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
