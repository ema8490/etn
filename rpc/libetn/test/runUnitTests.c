#include <stdlib.h>
#include <ethos/dual/xmalloc.h>
#include <ethos/generic/debug.h>

#include "unitTest.h"

extern UnitTest testAdam; // The original test.
extern UnitTest testByteArray;
extern UnitTest testUint32Array;
extern UnitTest testString;
extern UnitTest testStringEmpty;
extern UnitTest testTuple;
extern UnitTest testTupleTuple;
extern UnitTest testTupleEmpty;
extern UnitTest testBigString;
extern UnitTest testUints;
extern UnitTest testStrings;
extern UnitTest testStringsArray;
extern UnitTest testStringAndAny;
extern UnitTest testStringAndAnyTuple;
extern UnitTest testRedBlackTree;
extern UnitTest testStructLoop;
extern UnitTest testStructContainingTuple;
extern UnitTest testUnion;
extern UnitTest testAnotherStructLoop;
extern UnitTest testPointer;
extern UnitTest testTree;
extern UnitTest testPointIntoMiddle;
extern UnitTest testTorture;
extern UnitTest testDecodeLibTypeGraph;
extern UnitTest testDecodeTestTypeGraph;
extern UnitTest testDecodeRandom;
extern UnitTest testSizeTorture;
extern UnitTest testVerifyByte;
extern UnitTest testVerifyArray;
extern UnitTest testVerifyString;
extern UnitTest testVerifyUints;
extern UnitTest testVerifyStrings;
extern UnitTest testVerifyStringsArray;
extern UnitTest testVerifyStringAndAny;
extern UnitTest testVerifyStringAndAnyTuple;
extern UnitTest testVerifyUnion;
extern UnitTest testVerifyInterface;
extern UnitTest testVerifyTorture;
extern UnitTest testVerifyRandom;

int main (int argc, char *argv[])
{
	// debugCategoryOn(0xffffffff);

	xmallocInit ();	

	runTest (testAdam);
	runTest (testByteArray);
	runTest (testUint32Array);
	runTest (testString);
	runTest (testStringEmpty);
	runTest (testTuple);
	runTest (testTupleEmpty);
	runTest (testTupleTuple);
	runTest (testUints);
	runTest (testStrings);
	runTest (testStringsArray);
	runTest (testStringAndAny);
	runTest (testStringAndAnyTuple);
	runTest (testRedBlackTree);
	runTest (testStructLoop);
	runTest (testStructContainingTuple);
	runTest (testUnion);
	runTest (testAnotherStructLoop);
	runTest (testPointer);
	runTest (testTree);
	runTest (testPointIntoMiddle);
	runTest (testTorture);
	runTest (testDecodeLibTypeGraph);
	runTest (testDecodeTestTypeGraph);
	runTest (testDecodeRandom);
	runTest (testSizeTorture);
	runTest (testBigString);
	runTest (testVerifyByte);
	runTest (testVerifyArray);
	runTest (testVerifyString);
	runTest (testVerifyUints);
	runTest (testVerifyStrings);
	runTest (testVerifyStringsArray);
	runTest (testVerifyStringAndAny);
	runTest (testVerifyStringAndAnyTuple);
	runTest (testVerifyUnion);
	runTest (testVerifyInterface);
	runTest (testVerifyTorture);
	runTest (testVerifyRandom);

	exit(EXIT_SUCCESS);
}
