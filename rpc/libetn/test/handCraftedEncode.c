#include <ethos/generic/tree.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

enum {
        IterationCount = 10000000,
};

typedef struct AddrToIndex_s {
	void *ptr;
	size_t index;
	struct rbtree_node node;
} AddrToIndex;

static struct rbtree addrToIndex;

static int
_compareFn (const struct rbtree_node *node1, const struct rbtree_node *node2)
{
	const AddrToIndex *n1 = rbtree_container_of(node1, AddrToIndex, node);
	const AddrToIndex *n2 = rbtree_container_of(node2, AddrToIndex, node);

	return n1->ptr - n2->ptr;
}

static void
_freeTree (struct rbtree *tree)
{
	struct rbtree_node *node = rbtree_first(tree);
	while (node) {
		AddrToIndex *aiNode = rbtree_container_of (node, AddrToIndex, node);
		node = rbtree_next (node);
		free (aiNode);
	}
}

// Remember a pointer, if necessary.
static void
_rememberPointer(void *p)
{
	static size_t index = 0;

	AddrToIndex query;
	query.ptr = p;
	struct rbtree_node *n = rbtree_lookup(&query.node, &addrToIndex);
	if (n == NULL) {
		AddrToIndex *node = malloc (sizeof (AddrToIndex));

		node->ptr = p;
		node->index = index++;

		rbtree_insert(&node->node, &addrToIndex);
	}
}

static void
uint32encode (uint8_t *buf, uint32_t data)
{
	_freeTree (&addrToIndex);
	rbtree_init (&addrToIndex, _compareFn, 0);
	// Just like libetn; top-level pointer need not be added to tree.
	memcpy (buf, &data, sizeof (data));
}

static void
pointerencode (uint8_t *buf, void *data)
{
	_freeTree (&addrToIndex);
	rbtree_init (&addrToIndex, _compareFn, 0);
	// Just like libetn; top-level pointer need not be added to tree.
	memcpy (buf, &data, sizeof (data));
}

static void
tortureencode (uint8_t *buf, Torture data)
{
	_freeTree (&addrToIndex);
	rbtree_init (&addrToIndex, _compareFn, 0);
	// Just like libetn; top-level pointer need not be added to tree.

	void *ptr = &data;

	memcpy (buf, ptr, sizeof (int8_t));
	ptr += sizeof (int8_t);

	memcpy (buf, ptr, sizeof (uint8_t));
	ptr += sizeof (uint8_t);

	memcpy (buf, ptr, sizeof (int16_t));
	ptr += sizeof (int16_t);

	memcpy (buf, ptr, sizeof (uint16_t));
	ptr += sizeof (uint16_t);

	memcpy (buf, ptr, sizeof (int32_t));
	ptr += sizeof (int32_t);

	memcpy (buf, ptr, sizeof (uint32_t));
	ptr += sizeof (uint32_t);

	memcpy (buf, ptr, sizeof (int64_t));
	ptr += sizeof (int64_t);

	memcpy (buf, ptr, sizeof (uint64_t));
	ptr += sizeof (uint64_t);

	_rememberPointer (ptr);
	uint8_t code = 1;
	uint8_t index = 1;
	memcpy (buf, &code, sizeof (uint8_t));
	ptr += sizeof (uint8_t);
	memcpy (buf, &index, sizeof (EtnLength));
	ptr += sizeof (EtnLength);

	memcpy (buf, &data.tuple.len, sizeof (EtnLength)); 
	ptr += sizeof (EtnLength);
	memcpy (buf, data.tuple.array, data.tuple.len);
	ptr += data.tuple.len;

	memcpy (buf, &data.array, sizeof (ByteArray)); 
	ptr += sizeof (ByteArray);
}

#define benchmark(fn, name, size) \
{ \
	int i; \
	uint8_t buf[BUFSIZ]; \
	struct timeval now, later; \
	gettimeofday (&now, NULL); \
	for (i = 0; i < IterationCount - 1; i++) { \
		fn##encode (buf, fn##arg); \
	} \
	gettimeofday (&later, NULL); \
	printf ("%s: Ran %d iterations in %f seconds, total %lu bytes\n", \
	         name, \
	         IterationCount, \
	        (later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0, \
	         IterationCount * (unsigned long) size); \
}

int
main (int argc, char *argv[])
{
	uint32_t  uint32arg = 0;
	void     *pointerarg = &main;
	Torture   torturearg;

	torturearg.int8Ptr = &torturearg.int8;
        torturearg.tuple = (Slice) { (uint8_t *) HelloWorld, 1, strlen (HelloWorld), strlen (HelloWorld) };

	benchmark (uint32, "32-bit Integer", sizeof (uint32_t));
	benchmark (pointer, "Pointer", sizeof (void *));
	benchmark (torture, "Torture", sizeof (Torture) + strlen (HelloWorld));

	exit (EXIT_SUCCESS);
}
