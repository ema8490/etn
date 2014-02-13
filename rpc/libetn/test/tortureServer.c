#include <stdio.h>
#include <stdlib.h>

#include "unitTest.h"
#include "testGraph.h"

int main (int argc, char *argv[])
{
	Torture torture;

	runServer (EtnToValue(&TortureType, &torture));

	printf ("Decoded value is:\n");
	printf ("0x%x\n", torture.int8);
	printf ("0x%x\n", torture.uint8);
	printf ("0x%x\n", torture.int16);
	printf ("0x%x\n", torture.uint16);
	printf ("0x%x\n", torture.int32);
	printf ("0x%x\n", torture.uint32);
	printf ("0x%llx\n", torture.int64);
	printf ("0x%llx\n", torture.uint64);
	// printf ("0x%x\n", torture.float32);
	// printf ("0x%x\n", torture.float64);
	// printf ("0x%x\n", torture.bool);
	printf ("0x%x\n", *torture.int8Ptr);
	printf ("%s\n", torture.tuple.array);

	exit (EXIT_SUCCESS);
}
