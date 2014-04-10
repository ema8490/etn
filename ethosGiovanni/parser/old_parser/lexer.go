package parser

import (
	"ethos/util"
	"fmt"
	"regexp"
)

var matches = []int{
	SKIP,
	DIGIT,
	ALPHA,
	L_PAR,
	R_PAR,
	L_CUR,
	R_CUR,
	L_SQ,
	R_SQ,
	DOT,
	COMMA,
	SEMI,
	COLON,
	PIPE,
	DOLLAR,
	EQ,
	AMPERSAND,
	NOT,
	L_COMP,
	G_COMP,
	PLUS,
	MINUS,
	MUL,
	PATH,
	SLASH,
	DOUBLE_QUOTE,
	COMMENT_START,
	ANY,
}

var REs = map[int][]string{
	SKIP: []string{" ", "	", "\\n"},
	DIGIT:        []string{"[0-9]"},
	ALPHA:        []string{"[a-zA-Z]"},
	L_PAR:        []string{`\(`},
	R_PAR:        []string{`\)`},
	L_CUR:        []string{`\{`},
	R_CUR:        []string{`\}`},
	L_SQ:         []string{"\\["},
	R_SQ:         []string{"\\]"},
	DOT:          []string{"\\."},
	COMMA:        []string{","},
	SEMI:         []string{";"},
	COLON:        []string{":"},
	PIPE:         []string{"\\|"},
	EQ:           []string{"="},
	AMPERSAND:    []string{"&"},
	NOT:          []string{"!"},
	L_COMP:       []string{"<"},
	G_COMP:       []string{">"},
	PLUS:         []string{`\+`},
	MINUS:        []string{`-`},
	MUL:          []string{`\*`},
	SLASH:        []string{"/"},
	DOUBLE_QUOTE: []string{`"`},
	DOLLAR:       []string{"\\$"},
	//COMMENT_START: []string{"#"},
	ANY: []string{"[:graph:]"},
}

type handler func(value []byte, lex *Lexer, lval *yySymType) int

var handlers map[int]handler

type Lexer struct {
	input  string
	pos    int
	lineNo int
}

func (self *Lexer) Init(input string) {
	self.input = input
	self.lineNo = 1
	handlers = map[int]handler{
		ANY:          anysHandler,
		DIGIT:        digitsHandler,
		ALPHA:        alphasHandler,
		DOUBLE_QUOTE: doubleQuotesHandler,
		SLASH:        pathsHandler,
		DOT:          pathsHandler,
		DOLLAR:       dollarsHandler,
		PIPE:         pipesHandler,
		AMPERSAND:    ampersandsHandler,
		// COMMENT_START: commentStartsHandler,
	}
}

func (self *Lexer) Peek() (c []byte, more bool) {
	if self.pos < len(self.input) {
		return []byte{byte(self.input[self.pos])}, true
	}
	return nil, false
}

func (self *Lexer) Next() (c []byte, more bool) {
	if self.pos < len(self.input) {
		self.pos += 1
		c := self.input[self.pos-1]
		if c == '\n' {
			self.lineNo += 1
		}
		return []byte{byte(c)}, true
	}
	return nil, false
}

func (self *Lexer) Lex(lval *yySymType) int {
next:
	lval.value = []byte{}
	c, more := self.Next()

	if !more {
		return EOF
	}
	for _, id := range matches {
		for _, re := range REs[id] {
			matched, err := regexp.Match(re, c)
			if err != nil {
				util.Printf(err.String())
			}
			if matched {
				if handlers[id] != nil {
					id = handlers[id](c, self, lval)
				}
				if id == SKIP {
					goto next
				}
				// fmt.Printf("(%d)%s(%s)%t ", id, string(lval.value), string(c), id == AND)
				return id
			}
		}
	}
	return int(c[0])
}

func (self *Lexer) Error(e string) {
	panic(fmt.Sprintf("syntax error: %s, line: %d\n", e, self.lineNo))
}

func digitsHandler(value []byte, lex *Lexer, lval *yySymType) int {
	for p, more := lex.Peek(); more; p, more = lex.Peek() {
		matched, _ := regexp.Match(REs[DIGIT][0], p)
		if matched {
			lex.Next()
			value = append(value, p[0])
		} else {
			break
		}
	}
	lval.value = value
	return NUMBER_LITERAL
}

func alphasHandler(value []byte, lex *Lexer, lval *yySymType) int {
	for p, more := lex.Peek(); more; p, more = lex.Peek() {
		matchedA, _ := regexp.Match(REs[ALPHA][0], p)
		matchedD, _ := regexp.Match(REs[DIGIT][0], p)
		if matchedA || matchedD {
			lex.Next()
			value = append(value, p[0])
		} else {
			break
		}
	}

	lval.value = value

	switch string(value) {
	case "true", "false":
		return BOOL_LITERAL
	case "for":
		return FOR
	case "in":
		return IN
	case "if":
		return IF
	case "else":
		return ELSE
	case "break":
		return BREAK
	case "continue":
		return CONTINUE
	case "return":
		return RETURN
	case "switch":
		return SWITCH
	case "case":
		return CASE
	case "func":
		return FUNC
	case "export":
		return EXPORT
	case "var":
		return VAR
	case "new":
		return NEW
	}

	return STRING_LITERAL
}

func dollarsHandler(value []byte, lex *Lexer, lval *yySymType) int {
	for p, more := lex.Peek(); more; p, more = lex.Peek() {
		matchedA, _ := regexp.Match(REs[ALPHA][0], p)
		matchedD, _ := regexp.Match(REs[DIGIT][0], p)
		if matchedA || matchedD {
			lex.Next()
			value = append(value, p[0])
		} else {
			break
		}
	}

	lval.value = value[1:]

	return IDENTIFIER
}

func anysHandler(value []byte, lex *Lexer, lval *yySymType) int {
	fmt.Print("das\n")
	for p, more := lex.Peek(); more; p, more = lex.Peek() {
		matchedAny, _ := regexp.Match(REs[ANY][0], p)
		fmt.Printf("any: \"%s\" %t ", p, matchedAny)
		if matchedAny {
			lex.Next()
			value = append(value, p[0])
		} else {
			break
		}
	}

	lval.value = value

	return TOKEN
}

func doubleQuotesHandler(value []byte, lex *Lexer, lval *yySymType) int {
	for p, more := lex.Peek(); more; p, more = lex.Peek() {
		value = append(value, p[0])
		lex.Next()
		matched, _ := regexp.Match(REs[DOUBLE_QUOTE][0], p)
		if matched {
			lval.value = value[1 : len(value)-1]
			return QUOTED_STRING_LITERAL
		}
	}
	return EOF
}

func ampersandsHandler(value []byte, lex *Lexer, lval *yySymType) int {
	p, more := lex.Peek()
	if !more {
		return AMPERSAND
	}
	matched, _ := regexp.Match("&", p)
	if matched {
		lex.Next()
		return AND
	}
	return AMPERSAND
}

func pipesHandler(value []byte, lex *Lexer, lval *yySymType) int {
	p, more := lex.Peek()
	if !more {
		return PIPE
	}
	matched, _ := regexp.Match("\\|", p)
	if matched {
		lex.Next()
		return OR
	}
	return PIPE
}

func pathsHandler(value []byte, lex *Lexer, lval *yySymType) int {
	p, _ := lex.Peek()
	slashNext, _ := regexp.Match(REs[SLASH][0], p)
	dotNext, _ := regexp.Match(REs[DOT][0], p)
	dotFirst, _ := regexp.Match(REs[DOT][0], value[:1])

	if dotFirst && (!dotNext && !slashNext) {
		return DOT
	}

	for p, more := lex.Peek(); more; p, more = lex.Peek() {
		skip := false
		for i := 0; i < len(REs[SKIP]); i++ {
			m, _ := regexp.Match(REs[SKIP][i], p)
			skip = skip || m
			if skip {
				break
			}
		}
		stop, _ := regexp.Match("[^/\\.A-Za-z0-9]", p)
		if skip || stop {
			break
		} else {
			lex.Next()
			value = append(value, p[0])
		}
	}

	if len(value) == 1 && string(value[0]) == REs[SLASH][0] {
		return SLASH
	}

	lval.value = value

	return PATH
}

// skips the whole line after #
func commentStartsHandler(value []byte, lex *Lexer, lval *yySymType) int {
	for p, more := lex.Peek(); more; p, more = lex.Peek() {
		lex.Next()
		if p[0] == byte('\n') {
			return SKIP
		}
	}
	return EOF
}
