/*
	The Environment represents the state
	during evaluation, that is: the current
	working directory, the SymbolTable and
	a few flags that the evaluators use for
	control flow (return, break, continue).

	An Environment can be copied, creating a
	new scope, that has access to the original
	scope but cannot pollute it with symbols
	defined in the inner scope.

	The SymbolTable is organized as a simple
	stack. This implements lexical scoping.
	The dynamic call stack is encoded
	keeping track of the CallerEnv,	and used
	together with an (possibly set) Handler
	for panic handling.
*/
package util

import (
	"fmt"
	"os"
)

const (
	VERBOSE = iota
	TEST
)

var mode = VERBOSE

func SetMode(m int) {
	mode = m
}

func Printf(format string, params ...interface{}) {
	if mode == VERBOSE {
		fmt.Printf(format, params...)
	}
}

func Fatalf(format string, params ...interface{}) {
	Printf(format, params...)
	os.Exit(0)
}
