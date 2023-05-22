/**
 * Item header for linked lists.
 *
 * Description
 * ===========
 * ListItem is an internal structure for linked lists in AllC. Every element in
 * the list is allocated with ListItem prefixed. ListItem contains the pointers
 * to the next/previous element in the list.
 *
 * Options
 * =======
 * Same as for `list.h`.
 *
 * Content
 * =======
 *
 * List Item
 * ---------
 *
 * [x] AllCListItem
 * [x] allc_listitem_new
 * [x] allc_listitem_del
 * [x] allc_listitem_jump
 * [x] allc_listitem_detach
 * [x] allc_listitem_attach_back
 * [x] allc_listitem_attach_front
 *
 * (Kaj Munhoz Arfvidsson, 2023)
 **/

#ifndef ALLC_LISTITEM__GUARD
#define ALLC_LISTITEM__GUARD

#include <sys/types.h>

#include "../allocator.h"

// List Item {{{1
// --------------

/**
 * An item in the list.
 *
 * This is a wrapper around every element in the list. This is why it is
 * important to manage elements only with the provided `allc_list_*` functions,
 * otherwise the list might behave unexpectedly.
 **/
typedef struct ListItem {
    struct ListItem *next;
    struct ListItem *prev;
    size_t size;
    unsigned char data[];
} ListItem;

/**
 * Allocate a new item.
 **/
extern ListItem *allc_listitem_new(Allocator allocator, size_t size);

/**
 * Delete the item.
 **/
extern void allc_listitem_del(Allocator allocator, ListItem *p);

/**
 * Jump forward or backward N steps in the linked list.
 **/
extern ListItem *allc_listitem_jump(ListItem *self, ssize_t n);

/**
 * Detach item from linked list.
 *
 * This operation makes sure any broken links are fixed such that...
 * ```
 * // before
 * a -> self -> b
 * // after
 * a -> b
 * ```
 **/
extern void allc_listitem_detach(ListItem *self);

/**
 * Attach item to linked list by placing it before `other`.
 *
 * Item is assumed to have NULL links.
 **/
extern void allc_listitem_attach_before(ListItem *self, ListItem *other);

/**
 * Attach item to linked list by placing it after `other`.
 *
 * Item is assumed to have NULL links.
 **/
extern void allc_listitem_attach_after(ListItem *self, ListItem *other);


#endif // ALLC_LISTITEM__GUARD
