#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	Slice types;
	loadTypeGraph (&types, TestTypesPath);

	// FIXME: need better checks, but will assert out on decode failure.
	return true;
}

UnitTest testDecodeTestTypeGraph = { __FILE__, _predicate };
