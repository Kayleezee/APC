/*************************************************************************************************
*
* Heidelberg University - APC Exercise 07
*
* Group :       APC03
* Participant : Christoph Klein
*               Klaus Naumann
*
* File :        rbtree.h
*
* Purpose :     RED-BLACK TREE
*
* Last Change : 13. Juni 2015
*
*************************************************************************************************/
#ifndef _RBTREE_H_
#include <pthread.h>

enum rbtree_color { RED , BLACK};

typedef struct rbtree_node_t {
    void* vKey;
    void* vValue;
    struct rbtree_node_t* parent;
    struct rbtree_node_t* left;
    struct rbtree_node_t* right;
    enum rbtree_color color;
} *rbtree_node;

typedef struct rbtree_t {
    rbtree_node root;
} *rbtree;

typedef int (*compare_func)(void* left, void* right);

// Create rbtree
rbtree rbtree_create();
// Compare function for integers
int int_compare(void* left, void* right);
// Search function
void* rbtree_lookup(rbtree t, void* key, compare_func compare);
// Add function
void rbtree_insert(rbtree t, void* key, void* value, compare_func compare);

#endif // _RBTREE_H_
