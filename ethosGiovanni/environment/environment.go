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
package en

import (
	"ethos/parser"
)

type Environment interface {
	Copy(name string) *Environment
	Init(wd, name string)

	SetBreak(b bool)
	IsSetBreak() bool

	SetContinue(c bool)
	IsSetContinue() bool

	SetReturn(r bool, value interface{}, t int)
	IsSetReturn() bool
	GetReturn() (interface{}, int)

	IsInLoop() bool
	SetInLoop(l bool)

	SetPanic(p bool, err interface{}, o interface{})
	isSetPanic()
	GetPanic() (interface{}, interface{})

	SetHandler(h interface{})
	isSetHandler()
	GetHandler() interface{}
}

type StandardEnvironment struct {
	Symbols   *StackSymbolTable
	State     *StandardStateValues
	WD        string
	Name      string
	CallerEnv *StandardEnvironment
}

func (self *StandardEnvironment) Copy(name string) *StandardEnvironment {
	env := new(StandardEnvironment)
	env.Init(self.WD, name)
	env.Symbols = self.Symbols.Copy()
	return env
}

func (self *StandardEnvironment) Init(wd string, name string) {
	self.Symbols = new(StackSymbolTable)
	self.Symbols.Init()
	self.State = new(StandardStateValues)
	self.State.Values = make(map[string]interface{})
	self.WD = wd
	self.Name = name
}

func (self *StandardEnvironment) SetBreak(b bool) {
	self.State.Values["break"] = b
}

func (self *StandardEnvironment) IsSetBreak() bool {
	if self.State.Values["break"] != nil {
		return self.State.Values["break"].(bool)
	}
	return false
}

func (self *StandardEnvironment) SetContinue(c bool) {
	self.State.Values["continue"] = c
}

func (self *StandardEnvironment) IsSetContinue() bool {
	if self.State.Values["continue"] != nil {
		return self.State.Values["continue"].(bool)
	}
	return false
}

func (self *StandardEnvironment) SetInLoop(l bool) {
	self.State.Values["in_loop"] = l
}

func (self *StandardEnvironment) IsInLoop() bool {
	if self.State.Values["in_loop"] != nil {
		return self.State.Values["in_loop"].(bool)
	}
	return false
}

func (self *StandardEnvironment) SetReturn(r bool, value interface{}, t int) {
	self.State.Values["return"] = r
	if r {
		self.State.Values["return_value"] = value
		self.State.Values["return_type"] = t
	}
}

func (self *StandardEnvironment) IsSetReturn() bool {
	if self.State.Values["return"] != nil {
		return self.State.Values["return"].(bool)
	}
	return false
}

func (self *StandardEnvironment) GetReturn() (interface{}, int) {
	t, _ := self.State.Values["return_type"].(int)
	return self.State.Values["return_value"], t
}

func (self *StandardEnvironment) GetPanic() (interface{}, interface{}) {
	return self.State.Values["panic_err"], self.State.Values["panic_origin"]
}

func (self *StandardEnvironment) SetPanic(p bool, err interface{}, o interface{}) {
	self.State.Values["panic"] = p
	self.State.Values["panic_origin"] = o
	self.State.Values["panic_err"] = err
}

func (self *StandardEnvironment) IsSetPanic() bool {
	if self.State.Values["panic"] != nil {
		p, _ := self.State.Values["panic"].(bool)
		return p
	}
	return false
}

func (self *StandardEnvironment) GetHandler() interface{} {
	return self.State.Values["panic_handler"]
}

func (self *StandardEnvironment) SetHandler(h interface{}) {
	self.State.Values["panic_handler"] = h
}

func (self *StandardEnvironment) IsSetHandler() bool {
	return self.State.Values["panic_handler"] != nil
}

type SymbolTable interface {
	Put(id string, value interface{})
	Get() *Symbol
	Lookup(id string) *Symbol
	Peek() *Symbol
	Copy() *SymbolTable
	Init()
}

// StackSymbolTable

type StackSymbolTable struct {
	top *Symbol
}

func (self *StackSymbolTable) Init() {
}

func (self *StackSymbolTable) Put(id string, value interface{}, t int) {
	symbol := new(Symbol)
	symbol.Id, symbol.Value = id, value
	symbol.Type = t
	symbol.Prev = self.top
	self.top = symbol
}

func (self *StackSymbolTable) Lookup(id string) *Symbol {
	for current := self.top; current != nil; current = current.Prev {
		if current.Id == id {
			return current
		}
	}
	return nil
}

func (self *StackSymbolTable) Peek() *Symbol {
	return self.top
}

func (self *StackSymbolTable) Copy() *StackSymbolTable {
	table := new(StackSymbolTable)
	table.top = self.top
	return table
}

func (self *StackSymbolTable) List() []*Symbol {
	symbols := map[string]*Symbol{}
	for symbol := self.top; symbol != nil; symbol = symbol.Prev {
		if symbols[symbol.Id] == nil {
			symbols[symbol.Id] = symbol
		}
	}
	ret := []*Symbol{}
	for _, v := range symbols {
		ret = append(ret, v)
	}
	return ret
}

// HashSymbolTable: partial implementation

type HashSymbolTable struct {
	symbols map[string]*Symbol
	last    string
}

func (self *HashSymbolTable) Init() {
	self.symbols = map[string]*Symbol{}
}

func (self *HashSymbolTable) Put(id string, value interface{}, t int) {
	symbol := new(Symbol)
	symbol.Id, symbol.Value = id, value
	symbol.Type = t
	self.symbols[id] = symbol
	self.last = id
}

func (self *HashSymbolTable) Lookup(id string) *Symbol {
	return self.symbols[id]
}

func (self *HashSymbolTable) Peek() *Symbol {
	return self.symbols[self.last]
}

func (self *HashSymbolTable) Copy() *HashSymbolTable {
	table := new(HashSymbolTable)
	table.symbols = map[string]*Symbol{}
	for id, symbol := range self.symbols {
		table.symbols[id] = symbol.Copy()
	}
	table.last = self.last
	return table
}

func (self *HashSymbolTable) List() []*Symbol {
	symbols := []*Symbol{}
	for _, symbol := range self.symbols {
		symbols = append(symbols, symbol.Copy())
	}
	return symbols
}

// Symbol

type Symbol struct {
	Id       string      // identifier
	Prev     *Symbol     // previous symbol down the stack (used by StackSymbolTable)
	Value    interface{} // actual value
	Type     int         // actual type
	Exported bool        // exported from package
}

func (self *Symbol) Copy() *Symbol {
	s := new(Symbol)
	s.Id = self.Id
	s.Type = self.Type
	s.Exported = self.Exported
	value, ok := self.Value.(*parser.ParseTree)
	if ok {
		s.Value = value.Copy()
	} else {
		s.Value = self.Value
	}
	return s
}

// StateValues

type StateValues interface {
	Get(id string) interface{}
	Set(id string, value interface{})
}

type StandardStateValues struct {
	Values map[string]interface{}
}
