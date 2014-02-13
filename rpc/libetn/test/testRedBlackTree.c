#include <stdlib.h>
#include <ethos/generic/tree.h>

#include "unitTest.h"

enum {
	TreeSize = 100
};

typedef struct KeyValuePair_s {
	int key;
	int val;
	struct rbtree_node node;
} KeyValuePair_t;

static int 
_compareFn (const struct rbtree_node *node1, const struct rbtree_node *node2)
{
	const KeyValuePair_t *kvNode1 = rbtree_container_of(node1, KeyValuePair_t, node);
	const KeyValuePair_t *kvNode2 = rbtree_container_of(node2, KeyValuePair_t, node);

	return kvNode1->key - kvNode2->key;
}

static bool
_predicate (void)
{
	int i;
	KeyValuePair_t n;
	struct rbtree tree;
	KeyValuePair_t *node;
	struct rbtree_node *result;

	rbtree_init (&tree, _compareFn, 0);

	for (i = 0; i < TreeSize; i++) {
		node = malloc (sizeof (KeyValuePair_t));

		node->key = i;
		node->val = TreeSize + i;

		rbtree_insert ((struct rbtree_node *) &node->node, &tree);
	}

	// Lookup the nodes.
	for (i = 0; i < TreeSize; i++) {
		KeyValuePair_t *kvResult;
		n.key = i;
		kvResult = rbtree_container_of (rbtree_lookup ((struct rbtree_node *) &n.node, &tree), KeyValuePair_t, node);
		if (kvResult->key != i || kvResult->val != TreeSize + i) {
			return false;
		}
	}

	// This lookup should fail.
	n.key = TreeSize;
	result = rbtree_lookup ((struct rbtree_node *) &n.node, &tree);
	if (result != NULL) {
		return false;
	}

	//iterate (rbtree_first(&tree), iterateFn);
	result = rbtree_first(&tree);
	while (result) {
		KeyValuePair_t *kvResult = rbtree_container_of (result, KeyValuePair_t, node);
		struct rbtree_node *n = result;
		result = rbtree_next (result);
		rbtree_remove (n, &tree);
		free (kvResult);
	}

	// This lookup should fail because we just cleared the tree.
	n.key = TreeSize;
	n.key = 0;
	result = rbtree_lookup ((struct rbtree_node *) &n.node, &tree);
	if (result != NULL) {
		return false;
	}

	return true;
}

UnitTest testRedBlackTree = { __FILE__, _predicate };
