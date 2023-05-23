#include "_listitem.h"

// List Item {{{1
// --------------

ListItem *allc_listitem_new(Allocator allocator, size_t size)
{
    ListItem *p = allocator.alloc(size + sizeof (ListItem));
    p->size = size;
    return p;
}

void allc_listitem_del(Allocator allocator, ListItem *p)
{
    allocator.free(p);
}

ListItem *allc_listitem_jump(ListItem *self, ssize_t n)
{
    ListItem *p = self;
    if (n < 0) {
        for (; p != NULL && n != 0; n++) { p = p->prev; }
    } else {
        for (; p != NULL && n != 0; n--) { p = p->next; }
    }
    return p;
}

void allc_listitem_detach(ListItem *self)
{
    if (self->prev != NULL) { self->prev->next = self->next; }
    if (self->next != NULL) { self->next->prev = self->prev; }
}

void allc_listitem_attach_before(ListItem *self, ListItem *other)
{
    if (other == NULL) { return; }
    if (other->prev != NULL) { other->prev->next = self; }
    self->next = other;
    self->prev = other->prev;
    other->prev = self;
}

void allc_listitem_attach_after(ListItem *self, ListItem *other)
{
    if (other == NULL) { return; }
    if (other->next != NULL) { other->next->prev = self; }
    self->prev = other;
    self->next = other->next;
    other->next = self;
}

