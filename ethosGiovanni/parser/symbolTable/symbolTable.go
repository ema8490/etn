package symbolTable

type Symbol struct {
	
	Id       string      // identifier
	Value    interface{} // actual value
	Type     int         // actual type
	Exported bool        // exported from package
	
}

type SymbolTable struct {
	
	prev    *SymbolTable 		//pointer to the previous symbol table
	symbols map[string]*Symbol 	//the very symbol table
	
}

var TopStackSymbolTable *SymbolTable

//Initializes the global variable TopStackSymbolTable
func Init() {
	
	TopStackSymbolTable = new(SymbolTable)
	
}

//looks for a symbol in the current and previous symbol tables
func (self *SymbolTable) LookUp(symbolId string) *Symbol {
	
	topStack := TopStackSymbolTable
	
	for topStack != nil {
		if(topStack.symbols[symbolId] != nil){
			return topStack.symbols[symbolId]
		}
	}
	
	return nil
	
}

//inserts a symbol in the current symbol table
func (self *SymbolTable) Insert(symbolId string, symbolValue interface{}, symbolType int, symbolExported bool) *Symbol {
	
	symbol := new(Symbol)
	
	//if TopStackSymbolTable.symbols[symbolId] == nil {
		symbol.Id = symbolId
		symbol.Value = symbolValue
		symbol.Type = symbolType
		symbol.Exported = symbolExported
		
		TopStackSymbolTable.symbols[symbolId] = symbol
		
		return symbol
	//}
	
	//return nil
	
}

//creates a new symbol table and puts it at the top of the stack
func (self *SymbolTable) EnterEnv() {
	
	symbolTable := new(SymbolTable)
	
	symbolTable.prev = TopStackSymbolTable
	TopStackSymbolTable = symbolTable
	
}

//leaves the actual symbol table and sets the previous symbol table as top of the stack
func (self *SymbolTable) LeaveEnv() {
	
	TopStackSymbolTable = TopStackSymbolTable.prev
	
}
