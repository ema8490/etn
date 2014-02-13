#include <ethos/generic/assert.h>
#include <stdlib.h>

#include "treeFns.h"

TreeNode *
treeBuild (uint32_t height)
{
	ASSERT(height > 0);

	static uint32_t nodeId = 0;
	TreeNode *node = malloc (sizeof (TreeNode));

	height--;

	node->id = nodeId++;

	if (height > 0) {
		node->left  = treeBuild (height);
		node->right = treeBuild (height);
	} else {
		node->left  = NULL;
		node->right = NULL;
	}

	return node;
}

bool
treeSame (TreeNode *t1, TreeNode *t2)
{
	if (t1 == NULL && t2 != NULL) {
		return false;
	} else if (t1 != NULL && t2 == NULL) {
		return false;
	} else if (t1 == NULL && t2 == NULL) {
		return true;
	} else if (t1->id != t2->id) {
		return false;
	} else {
		return treeSame (t1->right, t2->right)
		    && treeSame (t2->left,  t2->left);
	}
}

void
treeFree (TreeNode *tree)
{
	if (NULL != tree) {
		if (NULL != tree->left) {
			treeFree (tree->left);
		}
		if (NULL != tree->right) {
			treeFree (tree->right);
		}
		free (tree);
	}
}
