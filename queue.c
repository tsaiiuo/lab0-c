#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Merge two list */
struct list_head *merge_two_queue(struct list_head *L1, struct list_head *L2)
{
    struct list_head *head = NULL;
    struct list_head **indirect = &head;

    for (; L1 && L2; indirect = &(*indirect)->next) {
        if (strcmp(list_entry(L1, element_t, list)->value,
                   list_entry(L2, element_t, list)->value) <= 0) {
            (*indirect) = L1;
            L1 = L1->next;
        } else {
            (*indirect) = L2;
            L2 = L2->next;
        }
    }
    // *indirect = (struct list_head *) ((uintptr_t) L1 | (uintptr_t) L2);
    *indirect = (L1) ? L1 : L2;
    return head;
}
/* Use merge_two_queue to implement mergesort */
struct list_head *merge_sort_queue(struct list_head *head)
{
    if (!head || !head->next)
        return head;


    struct list_head *slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }


    struct list_head *mid = slow->next;
    slow->next = NULL;


    struct list_head *left = merge_sort_queue(head);
    struct list_head *right = merge_sort_queue(mid);


    return merge_two_queue(left, right);
}
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
    struct list_head *node = NULL;
    struct list_head *safe = NULL;
    list_for_each_safe(node, safe, head) {
        element_t *curr = list_entry(node, element_t, list);
        q_release_element(curr);
    };
    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element) {
        return false;
    }

    new_element->value = strdup(s);
    if (!new_element->value) {
        // free(new_element);
        q_release_element(new_element);
        return false;
    }

    list_add(&(new_element->list), head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element) {
        return false;
    }

    new_element->value = strdup(s);
    if (!new_element->value) {
        // free(new_element);
        q_release_element(new_element);
        return false;
    }

    list_add_tail(&(new_element->list), head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove_element = list_entry(head->next, element_t, list);
    list_del(head->next);

    if (sp) {
        strncpy(sp, remove_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *remove_element = list_entry(head->prev, element_t, list);
    list_del(head->prev);

    if (sp) {
        strncpy(sp, remove_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return remove_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;
    }

    int len = 0;
    struct list_head *node = NULL;

    list_for_each(node, head) {
        len++;
    };

    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;
    for (; fast != head && fast->next != head;
         fast = fast->next->next, slow = slow->next) {
    }

    list_del(slow);
    element_t *mid = list_entry(slow, element_t, list);
    q_release_element(mid);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *curr = head->next;
    struct list_head *curr_next = NULL;
    for (; curr != head;) {
        bool check = false;
        const char *value = list_entry(curr, element_t, list)->value;
        for (curr_next = curr->next; curr_next != head;) {
            element_t *next_node = list_entry(curr_next, element_t, list);
            if (strcmp(value, next_node->value) == 0) {
                struct list_head *temp = curr_next->next;
                list_del(curr_next);
                q_release_element(next_node);
                curr_next = temp;
                check = true;
            } else {
                break;
            }
        }
        if (check) {
            element_t *curr_node = list_entry(curr, element_t, list);
            list_del(curr);
            q_release_element(curr_node);
            curr = curr_next;
            continue;
        }
        curr = curr->next;
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *curr = head->next;
    while (curr != head && curr->next != head) {
        struct list_head *temp = curr;
        list_move(curr, temp->next);
        curr = curr->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (q_size(head) == 0 || q_size(head) == 1) {
        return;
    }
    struct list_head *curr, *safe;

    list_for_each_safe(curr, safe, head) {
        list_move(curr, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https:  // leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || k == 1 || list_empty(head)) {
        return;
    }
    struct list_head *curr, *safe;
    struct list_head *temp = head;
    LIST_HEAD(dummy);
    int count = 0;
    list_for_each_safe(curr, safe, head) {
        count++;
        if (count == k) {
            list_cut_position(&dummy, temp, curr);
            q_reverse(&dummy);
            list_splice_init(&dummy, temp);
            count = 0;
            temp = safe->prev;
        }
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next = merge_sort_queue(head->next);

    if (!descend) {
        struct list_head *prev = head, *curr = head->next;
        while (curr) {
            curr->prev = prev;
            prev = curr;
            curr = curr->next;
        }
        prev->next = head;
        head->prev = prev;
    } else {
        struct list_head *prev = head, *curr = head->next,
                         *next = head->next->next;
        while (next) {
            curr->next = prev;
            curr->prev = next;
            prev = curr;
            curr = next;
            next = next->next;
        }
        curr->next = prev;
        curr->prev = head;
        head->next = curr;
        head->prev = head->next;
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head)
        return 0;
    struct list_head *p = head->next;
    element_t *p_ele = list_entry(p, element_t, list);
    while (p_ele->list.next != head) {
        element_t *pp_ele = list_entry(p_ele->list.next, element_t, list);
        if (strcmp(pp_ele->value, p_ele->value) < 0) {
            list_del(&pp_ele->list);
            q_release_element(pp_ele);
        } else {
            p_ele = pp_ele;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;
    struct list_head *p = head->prev;
    element_t *p_ele = list_entry(p, element_t, list);
    while (p_ele->list.prev != head) {
        element_t *pp_ele = list_entry(p_ele->list.prev, element_t, list);
        if (strcmp(pp_ele->value, p_ele->value) < 0) {
            list_del(&pp_ele->list);
            q_release_element(pp_ele);
        } else {
            p_ele = pp_ele;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    struct list_head *f_ptr = list_entry(head->next, queue_contex_t, chain)->q;
    for (struct list_head *c = head->next->next; c != head; c = c->next) {
        queue_contex_t *n = list_entry(c, queue_contex_t, chain);
        list_splice_init(n->q, f_ptr);
        n->size = 0;
    }
    queue_contex_t *f = list_entry(head->next, queue_contex_t, chain);
    q_sort(f_ptr, descend);
    f->size = q_size(f_ptr);
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return f->size;
}

void q_shuffle(struct list_head *head)
{
    int len = q_size(head);
    if (len <= 1)
        return;
    struct list_head *curr = head->prev;
    struct list_head *prev = NULL;
    for (int i = len - 1; i > 0; i--) {
        int ran = rand() % (i + 1);
        struct list_head *node;
        list_for_each(node, head) {
            if (ran == 0)
                break;
            ran--;
        }
        prev = node->prev;
        list_move(node, curr);
        list_move(curr, prev);
        curr = node->prev;
    }
}