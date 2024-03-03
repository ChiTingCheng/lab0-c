#include "queue.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *curr, *node;
    list_for_each_entry_safe (curr, node, head, list)
        q_release_element(curr);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    INIT_LIST_HEAD(&(node->list));
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *node = (element_t *) malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }
    INIT_LIST_HEAD(&(node->list));
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !sp || list_empty(head))
        return NULL;
    element_t *node = list_first_entry(head, element_t, list);
    size_t sz = sizeof(char) * strlen(node->value) + 1;
    if (sz > bufsize) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    } else {
        strncpy(sp, node->value, sz - 1);
        sp[sz - 1] = '\0';
    }
    list_del(&node->list);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || !sp || list_empty(head))
        return NULL;
    element_t *node = list_last_entry(head, element_t, list);
    size_t sz = sizeof(char) * strlen(node->value) + 1;
    if (sz > bufsize) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    } else {
        strncpy(sp, node->value, sz - 1);
        sp[sz - 1] = '\0';
    }
    list_del(&node->list);
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int counter = 0;
    struct list_head *curr;
    list_for_each (curr, head)
        counter++;
    return counter;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *tmp = head;
    int num = ceil(q_size(head) / 2) + 1;
    while (num) {
        tmp = tmp->next;
        num--;
    }
    element_t *node = list_entry(tmp, element_t, list);
    list_del(&node->list);
    q_release_element(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    struct list_head *node = NULL, *safe = NULL;
    bool dup = false;
    list_for_each_safe (node, safe, head) {
        element_t *cur = list_entry(node, element_t, list);
        element_t *cmp = list_entry(node->next, element_t, list);
        bool match = (node->next != head && !strcmp(cur->value, cmp->value));
        if (match || dup) {
            list_del(node);
            q_release_element(cur);
        }
        dup = match;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_is_singular(head))
        return;
    struct list_head *node = NULL, *safe = NULL, *tmp = head;
    for (node = (head)->next, safe = node->next;
         node != (head) && node->next != (head);
         node = safe, safe = node->next) {
        tmp->next = safe;
        node->next = safe->next;
        safe->prev = tmp;
        node->prev = safe;
        safe->next = node;
        tmp = node;
        safe = node->next;
    }
    if (q_size(head) % 2) {
        node->prev = tmp;
    } else {
        head->prev = tmp;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL;
    struct list_head *safe = NULL;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    head->next = head->prev;
    head->prev = safe;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL;
    struct list_head *safe = NULL;
    struct list_head *tmp = head;
    int num = q_size(head) / k;
    int counter = k - 1;
    for (; num != 0; num--) {
        node = tmp->next;
        safe = node->next;
        for (; counter != 0; counter--) {
            list_del(safe);
            list_add(safe, tmp);
            safe = node->next;
        }
        tmp = node;
        counter = k - 1;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *node = head->prev, *safe = node->prev;
    while (safe != (head)) {
        element_t *cur = list_entry(node, element_t, list);
        element_t *cmp = list_entry(safe, element_t, list);
        if (strcmp(cmp->value, cur->value) > 0) {
            list_del(safe);
            q_release_element(cmp);
        } else {
            node = safe;
        }
        safe = node->prev;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *node = head->prev, *safe = node->prev;
    while (safe != (head)) {
        element_t *cur = list_entry(node, element_t, list);
        element_t *cmp = list_entry(safe, element_t, list);
        if (strcmp(cmp->value, cur->value) < 0) {
            list_del(safe);
            q_release_element(cmp);
        } else {
            node = safe;
        }
        safe = node->prev;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
