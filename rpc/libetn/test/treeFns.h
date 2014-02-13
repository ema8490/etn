#ifndef __TREEFNS_H__
#define __TREEFNS_H__

#include "testGraph.h"

TreeNode *treeBuild (uint32_t height);
bool      treeSame  (TreeNode *t1, TreeNode *t2);
void      treeFree  (TreeNode *tree);

#endif
