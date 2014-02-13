#include <ethos/dual/string.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/tree.h>
#include <ethos/generic/etn.h>
#include <stdint.h>

typedef struct TypeToHash_s {
    EtnType *type;
    uint8_t hash[HashSize];
    struct rbtree_node node;
} TypeToHash;

static struct TypeTable {
    struct rbtree hashToType, typeToHash;
} tt;

static int
_compareHash (const struct rbtree_node *node1, const struct rbtree_node *node2)
{
    const TypeToHash *n1 = rbtree_container_of(node1, TypeToHash, node);
    const TypeToHash *n2 = rbtree_container_of(node2, TypeToHash, node);

    return memcmp(n1->hash, n2->hash, HashSize);
}

static int
_compareType (const struct rbtree_node *node1, const struct rbtree_node *node2)
{
    const TypeToHash *n1 = rbtree_container_of(node1, TypeToHash, node);
    const TypeToHash *n2 = rbtree_container_of(node2, TypeToHash, node);

    return n1->type - n2->type;
}

static bool initialized = false;

inline static void
_initialize (void)
{
    if (initialized == false)
	{
	    rbtree_init (&tt.hashToType, _compareHash, 0);
	    rbtree_init (&tt.typeToHash, _compareType, 0);
	    initialized = true;
	}
}

int 
etnRegisterType(EtnType *t, uint8_t *h)
{
    TypeToHash query, *insert;
	
    _initialize ();

    memcpy(query.hash, h, HashSize);
    TypeToHash *tr = (TypeToHash *) rbtree_lookup(&query.node, &tt.hashToType);
    if (tr != NULL)
	{
	    return 0;
	}

    insert = malloc(sizeof(TypeToHash));
    ASSERT (insert);

    insert->type = t;
    memcpy(insert->hash, h, HashSize);
    rbtree_insert((struct rbtree_node *) &insert->node, &tt.hashToType);
    rbtree_insert((struct rbtree_node *) &insert->node, &tt.typeToHash);
    return 1;
}

EtnType *
etnTypeForHash(uint8_t *h)
{
    EtnType *fnval = NULL;

    if (initialized == true)
	{
	    TypeToHash query;
	    memcpy(query.hash, h, HashSize);
	    struct rbtree_node *tr = rbtree_lookup(&query.node, &tt.hashToType);
	    if (tr != NULL)
		{
		    fnval = rbtree_container_of(tr, TypeToHash, node)->type;
		}
	}
    return fnval;
}

Status
etnHashForType(EtnType *t, uint8_t *h)
{
    Status status = StatusNotImplemented;
    if (initialized == true)
	{
	    TypeToHash query;
	    query.type = t;
	    struct rbtree_node *tr = rbtree_lookup(&query.node, &tt.typeToHash);
	    if (tr != NULL)
		{
		    status = StatusOk;
		    memcpy(h, rbtree_container_of(tr, TypeToHash, node)->hash, HashSize);
		}
	}
    return status;
}
