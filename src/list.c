#include "../include/list.h"

#include <stdio.h>
#include <string.h>

// List and List Item {{{

ListItem *allc_listitem_skip(ListItem *self, ssize_t n)
{
    ListItem *p = self;
    if (n < 0) {
        for (; n != 0; n++) {
#ifdef ALLC_LIST_SAFE
            if (p->prev == NULL)
                ALLC_PANIC("Skipping past end of list.\n");
#endif
            p = p->prev;
        }
    } else {
        for (; n != 0; n--) {
#ifdef ALLC_LIST_SAFE
            if (p->next == NULL)
                ALLC_PANIC("Skipping past end of list.\n");
#endif
            p = p->next;
        }
    }
    return p;
}

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
        allc_list_item_del(self, p);
    }
}

// Create and Remove Items {{{

ListItem *allc_list_item_new(List *self, size_t size)
{
    ListItem *p = self->allocator.alloc(size + sizeof (ListItem));
    p->size = size;
    return p;
}

void allc_list_item_del(List *self, ListItem *p)
{
    self->allocator.free(p);
}

void allc_list_create_1(List *self, size_t size, void **out)
{
    ListItem *p = allc_list_item_new(self, size);
    p->prev = self->last;
    if (self->length == 0) {
        self->head = p;
        self->last = p;
    } else {
        self->last->next = p;
        self->last = p;
    }
    self->length += 1;
    *out = p->data;
}

void allc_list_create_n(List *self, size_t n, size_t size, void **out)
{
    if (self->length == 0) {
        allc_list_create_1(self, size, (void**) &out[0]);
        allc_list_create_n(self, n-1, size, (void**) &out[1]);
        return;
    }
    ListItem *arr[n];
    arr[0] = allc_list_item_new(self, size);
    out[0] = arr[0]->data;
    for (size_t i = 1; i < n; i++) {
        arr[i] = allc_list_item_new(self, size);
        arr[i]->prev = arr[i-1];
        arr[i-1]->next = arr[i];
        out[i] = arr[i]->data;
    }
    arr[0]->prev = self->last;
    self->last->next = arr[0];
    self->last = arr[n-1];
    self->length += n;
}

void allc_list_remove_1(List *self, ssize_t i)
{
    ListItem *p = allc_list_item_at(self, i);
    if (p->prev != NULL) { p->prev->next = p->next; }
    if (p->next != NULL) { p->next->prev = p->prev; }
    if (p == self->head) { self->head = p->next; }
    if (p == self->last) { self->last = p->prev; }
    self->length -= 1;
    self->allocator.free(p);
}

void allc_list_remove_n(List *self, ssize_t i, size_t n)
{
#ifdef ALLC_LIST_SAFE
    if (!(i+n < self->length))
        ALLC_PANIC("Trying to remove elements outside of bounds.\n");
    // NOTE: we really want to test |n-i| here but since n is always positive this is also true
    if (!(n-i < self->length))
        // This can also happen if you try to remove [-1..3] in a 4-item list
        ALLC_PANIC("Trying to remove more elements than in list.\n");
#endif
    ListItem *arr[n];
    arr[0] = allc_list_item_at(self, i);
    for (size_t i = 1; i < n; i++) {
        arr[i] = arr[i-1]->next;
    }
    if (arr[0]->prev != NULL)   { arr[0]->prev->next = arr[n-1]->next; }
    if (arr[n-1]->next != NULL) { arr[n-1]->next->prev = arr[0]->prev; }
    if (arr[0] == self->head)   { self->head = arr[n-1]->next; }
    if (arr[n-1] == self->last) { self->last = arr[n]->prev; }
    self->length -= n;
    for (size_t i = 0; i < n; i++) {
        self->allocator.free(arr[i]);
    }
}

// Accessing List Content {{{

void *allc_list_at(List *self, ssize_t i)
{
    ListItem *p = allc_list_item_at(self, i);
    return p->data;
}

void allc_list_slice(List *self, ssize_t i, ssize_t j, void **out)
{
    if (i < 0) { i += self->length; }
    if (j < 0) { j += self->length; }
    if (i == j) { return; }
#ifdef ALLC_LIST_SAFE
    if (j - i < 0)
        ALLC_PANIC("Invalid use of indices.\n");
#endif
    ListItem *item;
    item = allc_list_item_at(self, i);
    out[0] = item->data;
    for (ssize_t k = 1; k < j-i; k++) {
        item = item->next;
        out[k] = item->data;
    }
}

ListItem *allc_list_item_at(List *self, ssize_t i)
{
#ifdef ALLC_LIST_SAFE
    ssize_t length = self->length;
    if (!(-length <= i && i < length))
        ALLC_PANIC("Index out of bounds.\n");
#endif
    ListItem *p;
    if (i < 0) {
        i += 1;
        p = self->last;
    } else {
        p = self->head;
    }
    return allc_listitem_skip(p, i);
}

void allc_list_item_slice(List *self, ssize_t i, ssize_t j, void **out)
{
    ssize_t length = self->length;
    if (i < 0) { i += length; }
    if (j < 0) { j += length; }
    if (i == j) { return; }
#ifdef ALLC_LIST_SAFE
    if (j - i < 0)
        ALLC_PANIC("Invalid use of indices.\n");
    if (j - length < 0)
        ALLC_PANIC("Index out of bounds.\n");
#endif
    ListItem *item;
    out[0] = item = allc_list_item_at(self, i);
    for (ssize_t k = 1; k < j-i; k++) {
        out[k] = item = item->next;
    }
}

void allc_list_append(List *self, size_t size, void *elem)
{
    void *p;
    allc_list_create_1(self, size, (void **) &p);
    memcpy(p, elem, size);
}

