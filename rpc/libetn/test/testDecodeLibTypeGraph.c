#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

#include <stdio.h>
static bool
_predicate (void)
{
	Slice types;
	loadTypeGraph (&types, TypeGraphPath);

	String *byteTuple     = stringAllocateInitialize ("[]byte");
	String *_uint32       = stringAllocateInitialize ("uint32");
	String *annotationPtr = stringAllocateInitialize ("*Annotation");
	String *hash          = stringAllocateInitialize ("TGHashValue");
	String *byte          = stringAllocateInitialize ("byte");
	String *hashTuple     = stringAllocateInitialize ("[]TGHashValue");
	String *typeNode      = stringAllocateInitialize ("TypeNode");
	String *string        = stringAllocateInitialize ("string");
	String *_struct       = stringAllocateInitialize ("struct");
	String *stringTuple   = stringAllocateInitialize ("[]string");
	String *annotation    = stringAllocateInitialize ("Annotation");

	return types.len == 12
	    && ! stringCompare(((TypeNode *)types.array)[0].name, byteTuple)
	    && ! stringCompare(((TypeNode *)types.array)[1].name, _uint32)
	    && ! stringCompare(((TypeNode *)types.array)[2].name, annotationPtr)
	    && ! stringCompare(((TypeNode *)types.array)[3].name, hash)
	    && ! stringCompare(((TypeNode *)types.array)[4].name, byte)
	    && ! stringCompare(((TypeNode *)types.array)[5].name, hashTuple)
	    && ! stringCompare(((TypeNode *)types.array)[6].name, typeNode)
	    && ! stringCompare(((TypeNode *)types.array)[7].name, string)
	    && ! stringCompare(((TypeNode *)types.array)[8].name, _struct)
	    && ! stringCompare(((TypeNode *)types.array)[9].name, stringTuple)
	    && ! stringCompare(((TypeNode *)types.array)[10].name, annotation)
	    && ! stringCompare(((TypeNode *)types.array)[11].name, _struct);
}

UnitTest testDecodeLibTypeGraph = { __FILE__, _predicate };
