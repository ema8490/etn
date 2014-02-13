#include <stdio.h>
#include <stdlib.h>

#include "unitTest.h"
#include "testGraph.h"

int main (int argc, char *argv[])
{
	Torture torture;
	const char bytes[] = "Hello, world!";

	torture.int8 = 0;
	torture.uint8 = 1;
	torture.int16 = 2;
	torture.uint16 = 3;
	torture.int32 = 4;
	torture.uint32 = 5;
	torture.int64 = 6;
	torture.uint64 = 7;
	//      torture.float32 = 3.14;
	//      torture.float64 = 3.1415;
	//      torture.bool = false;
	torture.int8Ptr = &torture.int8;
	torture.tuple = stringAllocateInitialize(bytes);

	if (argc != 2) {
		fprintf (stderr, "Usage: %s HOST\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	runClient (argv[1], EtnToValue(&TortureType, &torture));

	exit (EXIT_SUCCESS);
}
