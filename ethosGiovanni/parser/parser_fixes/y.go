
//line parsercopy.y:2

package main
import __yyfmt__ "fmt"
//line parsercopy.y:3
		
import (
    "./nodetypes"
    "fmt"
    "./ast"
)

const (
    EOF = iota

    SKIP
    DIGIT
    ALPHA
    COMMENT_START
    ANY
)


//line parsercopy.y:23
type yySymType struct{
	yys int
    value []byte
    node  *ParseTree
}

const L_PAR = 57346
const R_PAR = 57347
const L_CUR = 57348
const R_CUR = 57349
const L_SQ = 57350
const R_SQ = 57351
const DOT = 57352
const SEMI = 57353
const COLON = 57354
const COMMA = 57355
const DOUBLE_QUOTE = 57356
const EQ = 57357
const EQ_COMP = 57358
const NOT_EQ_COMP = 57359
const L_COMP = 57360
const L_EQ_COMP = 57361
const G_COMP = 57362
const G_EQ_COMP = 57363
const PLUS = 57364
const MINUS = 57365
const MUL = 57366
const SLASH = 57367
const PATH = 57368
const INTEGER_LITERAL = 57369
const FLOAT_LITERAL = 57370
const STRING_LITERAL = 57371
const QUOTED_STRING_LITERAL = 57372
const BOOL_LITERAL = 57373
const DOLLAR = 57374
const IDENTIFIER = 57375
const NOT = 57376
const AND = 57377
const OR = 57378
const AMPERSAND = 57379
const PIPE = 57380
const TOKEN = 57381
const FOR = 57382
const IN = 57383
const IF = 57384
const ELSE = 57385
const BREAK = 57386
const CONTINUE = 57387
const RETURN = 57388
const SWITCH = 57389
const CASE = 57390
const FUNC = 57391
const EXPORT = 57392
const NEW = 57393
const UNDERSCORE = 57394
const NEW_LINE = 57395
const TMP = 57396

var yyToknames = []string{
	"L_PAR",
	"R_PAR",
	"L_CUR",
	"R_CUR",
	"L_SQ",
	"R_SQ",
	"DOT",
	"SEMI",
	"COLON",
	"COMMA",
	"DOUBLE_QUOTE",
	"EQ",
	"EQ_COMP",
	"NOT_EQ_COMP",
	"L_COMP",
	"L_EQ_COMP",
	"G_COMP",
	"G_EQ_COMP",
	"PLUS",
	"MINUS",
	"MUL",
	"SLASH",
	"PATH",
	"INTEGER_LITERAL",
	"FLOAT_LITERAL",
	"STRING_LITERAL",
	"QUOTED_STRING_LITERAL",
	"BOOL_LITERAL",
	"DOLLAR",
	"IDENTIFIER",
	"NOT",
	"AND",
	"OR",
	"AMPERSAND",
	"PIPE",
	"TOKEN",
	"FOR",
	"IN",
	"IF",
	"ELSE",
	"BREAK",
	"CONTINUE",
	"RETURN",
	"SWITCH",
	"CASE",
	"FUNC",
	"EXPORT",
	"NEW",
	"UNDERSCORE",
	"NEW_LINE",
	"TMP",
}
var yyStatenames = []string{}

const yyEofCode = 1
const yyErrCode = 2
const yyMaxDepth = 200

//line parsercopy.y:794

/*
var parsed *ParseTree

/*func newNode(t int) *ParseTree {
    node := new(ParseTree)
    node.Type = t
    return node
}*/

func Error(format string, params ...interface{}) {
    fmt.Printf(format, params...)
    os.Exit(0)
}

func Parse(lexer *Lexer) *ParseTree {
    yyParse(lexer)
    return parsed
}
//line yacctab:1
var yyExca = []int{
	-1, 1,
	1, -1,
	-2, 0,
	-1, 17,
	33, 47,
	-2, 56,
	-1, 92,
	33, 47,
	-2, 56,
}

const yyNprod = 166
const yyPrivate = 57344

var yyTokenNames []string
var yyStates []string

const yyLast = 473

var yyAct = []int{

	147, 8, 56, 9, 8, 98, 167, 92, 17, 202,
	217, 17, 108, 65, 145, 207, 146, 134, 49, 82,
	90, 99, 107, 81, 53, 93, 106, 27, 30, 52,
	101, 41, 100, 80, 25, 10, 97, 79, 78, 77,
	51, 109, 67, 50, 26, 209, 216, 163, 103, 94,
	102, 104, 26, 233, 178, 132, 124, 54, 110, 75,
	83, 73, 125, 195, 91, 74, 143, 87, 123, 144,
	126, 127, 128, 129, 137, 74, 55, 177, 176, 74,
	68, 69, 70, 71, 72, 153, 26, 43, 140, 235,
	148, 88, 89, 143, 114, 165, 115, 84, 84, 141,
	86, 156, 142, 231, 67, 121, 122, 117, 118, 119,
	120, 84, 150, 109, 252, 155, 84, 199, 154, 174,
	193, 109, 166, 157, 158, 159, 141, 251, 173, 142,
	110, 170, 172, 171, 194, 42, 175, 169, 110, 239,
	164, 160, 151, 149, 226, 189, 190, 187, 188, 179,
	180, 181, 182, 183, 184, 139, 250, 138, 185, 186,
	203, 198, 246, 245, 241, 212, 200, 168, 197, 8,
	100, 196, 162, 213, 215, 260, 17, 230, 223, 54,
	255, 75, 210, 73, 222, 174, 205, 192, 214, 191,
	219, 3, 86, 130, 3, 225, 131, 152, 55, 105,
	33, 74, 68, 69, 70, 71, 72, 31, 26, 43,
	237, 224, 228, 18, 135, 227, 229, 238, 232, 208,
	8, 8, 8, 240, 236, 234, 67, 17, 17, 17,
	242, 243, 244, 206, 2, 161, 23, 247, 76, 203,
	22, 248, 253, 100, 211, 254, 54, 215, 75, 218,
	73, 96, 95, 21, 42, 20, 259, 14, 256, 257,
	258, 13, 12, 11, 262, 55, 201, 58, 74, 68,
	69, 70, 71, 72, 133, 26, 43, 54, 66, 75,
	64, 73, 28, 63, 29, 42, 220, 221, 19, 24,
	62, 61, 15, 67, 60, 7, 55, 59, 261, 74,
	68, 69, 70, 71, 72, 249, 26, 43, 54, 204,
	75, 85, 73, 28, 57, 29, 42, 220, 221, 19,
	24, 48, 47, 15, 67, 46, 7, 55, 45, 44,
	74, 68, 69, 70, 71, 72, 40, 26, 43, 54,
	39, 75, 38, 73, 28, 37, 29, 36, 35, 34,
	19, 24, 32, 16, 15, 67, 6, 7, 55, 5,
	4, 74, 68, 69, 70, 71, 72, 1, 26, 43,
	54, 0, 75, 0, 73, 0, 0, 0, 0, 54,
	0, 75, 0, 73, 83, 0, 67, 0, 0, 55,
	0, 0, 74, 68, 69, 136, 71, 72, 55, 26,
	43, 74, 68, 69, 70, 71, 72, 0, 26, 116,
	0, 75, 0, 73, 0, 111, 0, 67, 0, 0,
	0, 0, 0, 114, 0, 115, 67, 0, 113, 0,
	0, 74, 68, 69, 70, 71, 72, 116, 26, 75,
	0, 73, 0, 111, 112, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 113, 0, 0, 74,
	68, 69, 70, 71, 72, 0, 26, 0, 0, 0,
	0, 0, 112,
}
var yyPact = []int{

	304, -1000, -1000, 304, -1000, -1000, -1000, -1000, -14, -15,
	-16, -1000, -1000, -1000, -1000, 11, -1000, 96, 56, 53,
	-1000, -1000, -1000, -1000, 175, -1000, -1000, 12, 19, 175,
	14, 10, 16, 405, -1000, -1000, -1000, -1000, -1000, -1000,
	-1000, 89, 39, 375, -1000, -1000, 48, -1000, -1000, -1000,
	189, 192, -1000, -1000, 366, 375, -1000, 147, 19, -1000,
	-1000, -1000, -1000, -1000, -1000, -1000, -1000, 40, -1000, -1000,
	-1000, -1000, -1000, 175, -1000, 175, -1000, -1000, -1000, -1000,
	132, 101, 131, 193, 335, 19, 175, 123, 123, 123,
	130, -1000, 188, 166, -1000, 6, 129, 82, -1000, -1000,
	109, 161, 175, 123, 175, 405, 76, -1000, -1000, -1000,
	-1000, -1000, -1000, 433, 52, 51, 25, 375, 375, 375,
	375, 375, 375, -1000, -1000, -1000, 375, 375, 375, 375,
	175, 175, 184, 182, -1000, 107, 122, -1000, 34, 375,
	-1000, 192, 189, 188, 162, 152, -1000, 104, 159, -1000,
	-1000, -1000, 19, -1000, -1000, -1000, 181, -1000, -1000, -1000,
	-1000, -1000, -3, 375, 175, 19, 19, 3, 242, -1000,
	-1000, -1000, 76, -1000, -1000, -1000, -1000, -1000, 122, -1000,
	-1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, 179,
	173, -1000, -1000, 25, 175, -1000, 135, 366, -1000, 175,
	-1000, 172, -1000, 90, 49, -1000, 46, -1000, -3, 175,
	161, 128, -1000, -1000, -1000, 83, 161, 157, -1000, 273,
	273, 273, -1000, -1000, -1000, -1000, -1000, 156, 155, -1000,
	161, 19, -1000, -1000, 149, 115, -1000, 102, -1000, 19,
	-1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, 176,
	-1000, 161, 161, 161, -1000, 175, -1000, -1000, -1000, 170,
	-1000, -9, -1000,
}
var yyPgo = []int{

	0, 367, 234, 190, 360, 359, 356, 0, 3, 35,
	353, 7, 33, 34, 28, 352, 349, 348, 347, 345,
	342, 340, 336, 31, 18, 329, 328, 325, 322, 321,
	43, 314, 40, 311, 29, 309, 13, 305, 298, 2,
	24, 297, 294, 291, 290, 283, 280, 278, 14, 274,
	267, 16, 266, 6, 9, 10, 263, 262, 261, 257,
	255, 253, 252, 251, 244, 242, 5, 21, 240, 236,
	235, 233, 225, 15, 219, 17, 214, 213, 27, 207,
	200, 199, 26, 12, 22,
}
var yyR1 = []int{

	0, 1, 2, 2, 3, 3, 3, 3, 4, 4,
	4, 7, 8, 8, 11, 10, 13, 13, 14, 14,
	15, 15, 15, 15, 15, 15, 15, 15, 18, 19,
	20, 21, 16, 17, 22, 23, 23, 23, 25, 26,
	27, 27, 27, 28, 29, 30, 32, 33, 35, 37,
	38, 31, 24, 24, 24, 24, 24, 24, 24, 24,
	40, 40, 40, 40, 40, 40, 40, 40, 41, 42,
	43, 43, 44, 45, 46, 36, 47, 34, 34, 50,
	50, 50, 48, 48, 51, 51, 12, 52, 52, 54,
	54, 53, 53, 55, 55, 55, 55, 55, 55, 6,
	6, 6, 5, 5, 5, 5, 57, 57, 60, 62,
	62, 61, 63, 64, 65, 66, 66, 67, 67, 58,
	58, 69, 68, 56, 56, 59, 70, 70, 71, 71,
	73, 73, 74, 72, 39, 39, 49, 49, 75, 75,
	76, 9, 9, 9, 9, 77, 77, 78, 78, 79,
	79, 79, 79, 80, 80, 82, 82, 83, 83, 81,
	81, 84, 84, 84, 84, 84,
}
var yyR2 = []int{

	0, 1, 2, 1, 1, 1, 1, 1, 2, 2,
	2, 1, 3, 3, 1, 1, 3, 1, 3, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 3, 3,
	3, 3, 3, 3, 2, 1, 1, 1, 3, 3,
	1, 1, 1, 3, 3, 3, 4, 0, 0, 0,
	0, 11, 1, 1, 1, 1, 1, 3, 2, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 3, 3, 1, 3, 2, 1, 4,
	4, 4, 0, 1, 3, 1, 5, 0, 1, 3,
	1, 3, 2, 2, 2, 2, 1, 1, 1, 3,
	3, 3, 1, 1, 1, 1, 1, 1, 5, 1,
	3, 7, 1, 1, 1, 0, 1, 3, 1, 1,
	1, 3, 5, 3, 2, 3, 3, 4, 0, 1,
	2, 1, 4, 3, 5, 5, 0, 1, 3, 1,
	3, 3, 3, 3, 1, 2, 1, 3, 1, 3,
	2, 2, 1, 2, 2, 2, 1, 2, 2, 2,
	1, 1, 1, 1, 1, 2,
}
var yyChk = []int{

	-1000, -1, -2, -3, -4, -5, -6, 53, -7, -8,
	-9, -56, -57, -58, -59, 50, -10, -11, -77, 46,
	-60, -61, -68, -69, 47, -13, 33, -78, 40, 42,
	-14, -79, -15, -80, -16, -17, -18, -19, -20, -21,
	-22, -23, 12, 34, -25, -26, -27, -28, -29, -24,
	-30, -32, -34, -40, 4, 23, -39, -31, -50, -41,
	-42, -43, -44, -45, -46, -36, -47, 51, 27, 28,
	29, 30, 31, 8, 26, 6, -2, 53, 53, 53,
	-12, -11, -8, 49, 15, -33, 4, 11, 35, 36,
	-7, 11, -11, -7, 37, -62, -63, -11, -66, -67,
	-8, -7, 36, 38, 35, -81, -82, -84, -83, -11,
	-40, 10, 39, 23, 18, 20, 4, 18, 19, 20,
	21, 16, 17, 29, -36, -24, 22, 23, 24, 25,
	4, 4, -7, -49, -75, -76, 29, -24, 10, 8,
	-34, -32, -30, -11, 29, -48, -51, -7, -48, 11,
	11, 11, 4, -7, -12, -34, -48, -9, -9, -9,
	11, -70, 6, 41, 11, 13, 13, -53, 6, -13,
	-78, -14, -82, -84, -83, -84, 26, 26, 29, -23,
	-23, -23, -23, -23, -23, -23, -23, -24, -24, -48,
	-48, 5, 5, 13, 12, 29, -23, 6, 9, 13,
	7, -52, -54, -11, -35, 5, -71, -73, -74, 48,
	-24, -64, -7, -11, -67, -11, 43, -55, 7, -3,
	44, 45, 5, 5, -75, -7, 9, -75, -48, -51,
	5, 13, -36, 7, -72, 43, -73, -7, -53, 11,
	-53, 7, -55, -55, -55, 7, 7, -53, -54, -37,
	7, 12, 12, -65, -66, 4, -53, -53, -53, -7,
	5, -38, -39,
}
var yyDef = []int{

	0, -2, 1, 3, 4, 5, 6, 7, 0, 0,
	0, 102, 103, 104, 105, 0, 11, -2, 144, 0,
	106, 107, 119, 120, 0, 15, 14, 146, 115, 0,
	17, 148, 19, 152, 20, 21, 22, 23, 24, 25,
	26, 27, 0, 0, 35, 36, 37, 40, 41, 42,
	52, 53, 54, 55, 136, 0, 59, 0, 78, 60,
	61, 62, 63, 64, 65, 66, 67, 0, 68, 69,
	70, 71, 72, 82, 75, 82, 2, 8, 9, 10,
	0, 0, 0, 0, 0, 0, 82, 0, 0, 0,
	0, 124, -2, 0, 145, 0, 0, 109, 112, 116,
	118, 0, 0, 0, 0, 151, 150, 160, 156, 161,
	162, 163, 164, 0, 0, 0, 136, 0, 0, 0,
	0, 0, 0, 153, 154, 34, 0, 0, 0, 0,
	82, 82, 0, 0, 137, 139, 70, 58, 0, 0,
	77, 0, 0, 47, 0, 0, 83, 85, 0, 99,
	100, 101, 87, 12, 13, 48, 0, 141, 142, 143,
	123, 125, 128, 0, 0, 0, 0, 121, 0, 16,
	147, 18, 149, 159, 155, 165, 157, 158, 0, 28,
	29, 30, 31, 32, 33, 38, 39, 43, 44, 0,
	0, 57, 74, 0, 0, 45, 0, 82, 73, 0,
	76, 0, 88, 90, 0, 81, 0, 129, 131, 0,
	0, 0, 113, 110, 117, 0, 0, 0, 92, 96,
	97, 98, 80, 79, 138, 140, 46, 0, 0, 84,
	0, 0, 49, 126, 0, 0, 130, 0, 108, 115,
	122, 91, 93, 94, 95, 134, 135, 86, 89, 0,
	127, 0, 0, 0, 114, 0, 133, 132, 111, 0,
	50, 0, 51,
}
var yyTok1 = []int{

	1,
}
var yyTok2 = []int{

	2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
	22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
	42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
	52, 53, 54,
}
var yyTok3 = []int{
	0,
}

//line yaccpar:1

/*	parser for yacc output	*/

var yyDebug = 0

type yyLexer interface {
	Lex(lval *yySymType) int
	Error(s string)
}

const yyFlag = -1000

func yyTokname(c int) string {
	// 4 is TOKSTART above
	if c >= 4 && c-4 < len(yyToknames) {
		if yyToknames[c-4] != "" {
			return yyToknames[c-4]
		}
	}
	return __yyfmt__.Sprintf("tok-%v", c)
}

func yyStatname(s int) string {
	if s >= 0 && s < len(yyStatenames) {
		if yyStatenames[s] != "" {
			return yyStatenames[s]
		}
	}
	return __yyfmt__.Sprintf("state-%v", s)
}

func yylex1(lex yyLexer, lval *yySymType) int {
	c := 0
	char := lex.Lex(lval)
	if char <= 0 {
		c = yyTok1[0]
		goto out
	}
	if char < len(yyTok1) {
		c = yyTok1[char]
		goto out
	}
	if char >= yyPrivate {
		if char < yyPrivate+len(yyTok2) {
			c = yyTok2[char-yyPrivate]
			goto out
		}
	}
	for i := 0; i < len(yyTok3); i += 2 {
		c = yyTok3[i+0]
		if c == char {
			c = yyTok3[i+1]
			goto out
		}
	}

out:
	if c == 0 {
		c = yyTok2[1] /* unknown char */
	}
	if yyDebug >= 3 {
		__yyfmt__.Printf("lex %s(%d)\n", yyTokname(c), uint(char))
	}
	return c
}

func yyParse(yylex yyLexer) int {
	var yyn int
	var yylval yySymType
	var yyVAL yySymType
	yyS := make([]yySymType, yyMaxDepth)

	Nerrs := 0   /* number of errors */
	Errflag := 0 /* error recovery flag */
	yystate := 0
	yychar := -1
	yyp := -1
	goto yystack

ret0:
	return 0

ret1:
	return 1

yystack:
	/* put a state and value onto the stack */
	if yyDebug >= 4 {
		__yyfmt__.Printf("char %v in %v\n", yyTokname(yychar), yyStatname(yystate))
	}

	yyp++
	if yyp >= len(yyS) {
		nyys := make([]yySymType, len(yyS)*2)
		copy(nyys, yyS)
		yyS = nyys
	}
	yyS[yyp] = yyVAL
	yyS[yyp].yys = yystate

yynewstate:
	yyn = yyPact[yystate]
	if yyn <= yyFlag {
		goto yydefault /* simple state */
	}
	if yychar < 0 {
		yychar = yylex1(yylex, &yylval)
	}
	yyn += yychar
	if yyn < 0 || yyn >= yyLast {
		goto yydefault
	}
	yyn = yyAct[yyn]
	if yyChk[yyn] == yychar { /* valid shift */
		yychar = -1
		yyVAL = yylval
		yystate = yyn
		if Errflag > 0 {
			Errflag--
		}
		goto yystack
	}

yydefault:
	/* default state action */
	yyn = yyDef[yystate]
	if yyn == -2 {
		if yychar < 0 {
			yychar = yylex1(yylex, &yylval)
		}

		/* look through exception table */
		xi := 0
		for {
			if yyExca[xi+0] == -1 && yyExca[xi+1] == yystate {
				break
			}
			xi += 2
		}
		for xi += 2; ; xi += 2 {
			yyn = yyExca[xi+0]
			if yyn < 0 || yyn == yychar {
				break
			}
		}
		yyn = yyExca[xi+1]
		if yyn < 0 {
			goto ret0
		}
	}
	if yyn == 0 {
		/* error ... attempt to resume parsing */
		switch Errflag {
		case 0: /* brand new error */
			yylex.Error("syntax error")
			Nerrs++
			if yyDebug >= 1 {
				__yyfmt__.Printf("%s", yyStatname(yystate))
				__yyfmt__.Printf(" saw %s\n", yyTokname(yychar))
			}
			fallthrough

		case 1, 2: /* incompletely recovered error ... try again */
			Errflag = 3

			/* find a state where "error" is a legal shift action */
			for yyp >= 0 {
				yyn = yyPact[yyS[yyp].yys] + yyErrCode
				if yyn >= 0 && yyn < yyLast {
					yystate = yyAct[yyn] /* simulate a shift of "error" */
					if yyChk[yystate] == yyErrCode {
						goto yystack
					}
				}

				/* the current p has no shift on "error", pop stack */
				if yyDebug >= 2 {
					__yyfmt__.Printf("error recovery pops state %d\n", yyS[yyp].yys)
				}
				yyp--
			}
			/* there is no state on the stack with an error shift ... abort */
			goto ret1

		case 3: /* no shift yet; clobber input char */
			if yyDebug >= 2 {
				__yyfmt__.Printf("error recovery discards %s\n", yyTokname(yychar))
			}
			if yychar == yyEofCode {
				goto ret1
			}
			yychar = -1
			goto yynewstate /* try again in the same state */
		}
	}

	/* reduction by production yyn */
	if yyDebug >= 2 {
		__yyfmt__.Printf("reduce %v in:\n\t%v\n", yyn, yyStatname(yystate))
	}

	yynt := yyn
	yypt := yyp
	_ = yypt // guard against "declared and not used"

	yyp -= yyR2[yyn]
	yyVAL = yyS[yyp+1]

	/* consult goto table to find next state */
	yyn = yyR1[yyn]
	yyg := yyPgo[yyn]
	yyj := yyg + yyS[yyp].yys + 1

	if yyj >= yyLast {
		yystate = yyAct[yyg]
	} else {
		yystate = yyAct[yyj]
		if yyChk[yystate] != -yyn {
			yystate = yyAct[yyg]
		}
	}
	// dummy call; replaced with literal code
	switch yynt {

	case 1:
		//line parsercopy.y:184
		{
	
	}
	case 2:
		//line parsercopy.y:188
		{ //DONE
    
	                  }
	case 3:
		//line parsercopy.y:191
		{
	
	}
	case 4:
		//line parsercopy.y:195
		{
	
	            }
	case 5:
		//line parsercopy.y:198
		{
	    
	            }
	case 6:
		//line parsercopy.y:201
		{
	    
	            }
	case 7:
		//line parsercopy.y:204
		{
	
	}
	case 8:
		//line parsercopy.y:208
		{
	
	                }
	case 9:
		//line parsercopy.y:211
		{
	
	                }
	case 10:
		//line parsercopy.y:217
		{
	
	}
	case 11:
		//line parsercopy.y:221
		{  //DONE

	}
	case 12:
		//line parsercopy.y:225
		{    //DONE
    
	            }
	case 13:
		//line parsercopy.y:228
		{
	
	}
	case 14:
		//line parsercopy.y:232
		{    //DONE
    
	}
	case 15:
		//line parsercopy.y:236
		{   //DONE

	}
	case 16:
		//line parsercopy.y:240
		{
	
	                }
	case 17:
		//line parsercopy.y:243
		{
	    
	}
	case 18:
		//line parsercopy.y:247
		{
	
	                  }
	case 19:
		//line parsercopy.y:250
		{
	
	}
	case 20:
		//line parsercopy.y:254
		{ //DONE
    
	              }
	case 21:
		//line parsercopy.y:257
		{
	    
	              }
	case 22:
		//line parsercopy.y:260
		{
	    
	              }
	case 23:
		//line parsercopy.y:263
		{
	    
	              }
	case 24:
		//line parsercopy.y:266
		{
	    
	              }
	case 25:
		//line parsercopy.y:269
		{
	    
	              }
	case 26:
		//line parsercopy.y:272
		{
	    
	              }
	case 27:
		//line parsercopy.y:275
		{
	
	}
	case 28:
		//line parsercopy.y:279
		{ //DONE 

	}
	case 29:
		//line parsercopy.y:283
		{    //DONE

	}
	case 30:
		//line parsercopy.y:287
		{ //DONE

	}
	case 31:
		//line parsercopy.y:291
		{    //DONE

	}
	case 32:
		//line parsercopy.y:295
		{  //DONE

	}
	case 33:
		//line parsercopy.y:299
		{   //DONE

	}
	case 34:
		//line parsercopy.y:303
		{   //DONE
    
	}
	case 35:
		//line parsercopy.y:307
		{ //DONE

	      }
	case 36:
		//line parsercopy.y:310
		{
	    
	      }
	case 37:
		//line parsercopy.y:313
		{
	    
	}
	case 38:
		//line parsercopy.y:317
		{    //DONE
    
	      }
	case 39:
		//line parsercopy.y:321
		{   //DONE
    
	      }
	case 40:
		//line parsercopy.y:325
		{ //DONE
    
	          }
	case 41:
		//line parsercopy.y:328
		{
	    
	          }
	case 42:
		//line parsercopy.y:331
		{
	    
	}
	case 43:
		//line parsercopy.y:335
		{   //DONE
    
	}
	case 44:
		//line parsercopy.y:339
		{ //DONE
    
	}
	case 45:
		//line parsercopy.y:347
		{
	    
	}
	case 46:
		//line parsercopy.y:351
		{   //DONE
    
	}
	case 47:
		//line parsercopy.y:355
		{ 
	    
	              }
	case 48:
		//line parsercopy.y:358
		{ 
	    
	              }
	case 49:
		//line parsercopy.y:361
		{ 
	    
	              }
	case 50:
		//line parsercopy.y:364
		{
	
	              }
	case 51:
		//line parsercopy.y:367
		{
	
	}
	case 52:
		//line parsercopy.y:371
		{
	
	        }
	case 53:
		//line parsercopy.y:374
		{
	
	        }
	case 54:
		//line parsercopy.y:377
		{
	
	        }
	case 55:
		//line parsercopy.y:380
		{
	    
	        }
	case 56:
		//line parsercopy.y:383
		{
	        
	        }
	case 57:
		//line parsercopy.y:386
		{
	    
	        }
	case 58:
		//line parsercopy.y:389
		{
	    
	        }
	case 59:
		//line parsercopy.y:392
		{
	   
	}
	case 60:
		//line parsercopy.y:396
		{
	
	          }
	case 61:
		//line parsercopy.y:399
		{
	
	          }
	case 62:
		//line parsercopy.y:402
		{
	
	          }
	case 63:
		//line parsercopy.y:405
		{
	
	          }
	case 64:
		//line parsercopy.y:408
		{
	
	          }
	case 65:
		//line parsercopy.y:411
		{
	
	          }
	case 66:
		//line parsercopy.y:414
		{
	
	          }
	case 67:
		//line parsercopy.y:417
		{
	
	}
	case 68:
		//line parsercopy.y:421
		{
	
	}
	case 69:
		//line parsercopy.y:425
		{
	
	}
	case 70:
		//line parsercopy.y:429
		{
	    
	                  }
	case 71:
		//line parsercopy.y:432
		{
	
	}
	case 72:
		//line parsercopy.y:436
		{
	
	}
	case 73:
		//line parsercopy.y:440
		{
	    
	}
	case 74:
		//line parsercopy.y:444
		{
	
	}
	case 75:
		//line parsercopy.y:448
		{
	    
	}
	case 76:
		//line parsercopy.y:452
		{
	   
	}
	case 77:
		//line parsercopy.y:456
		{
	
	                    }
	case 78:
		//line parsercopy.y:459
		{
	
	}
	case 79:
		//line parsercopy.y:468
		{
	    
	                }
	case 80:
		//line parsercopy.y:471
		{
	    
	                }
	case 81:
		//line parsercopy.y:481
		{
	    
	}
	case 82:
		//line parsercopy.y:485
		{ //DONE
              
	              }
	case 83:
		//line parsercopy.y:488
		{
	    
	}
	case 84:
		//line parsercopy.y:492
		{    //DONE
                
	                }
	case 85:
		//line parsercopy.y:495
		{
	    
	}
	case 86:
		//line parsercopy.y:503
		{
	
	}
	case 87:
		//line parsercopy.y:507
		{
	                
	                }
	case 88:
		//line parsercopy.y:510
		{
	
	}
	case 89:
		//line parsercopy.y:514
		{
	
	                    }
	case 90:
		//line parsercopy.y:517
		{
	
	}
	case 91:
		//line parsercopy.y:521
		{
	    
	        }
	case 92:
		//line parsercopy.y:524
		{
	
	}
	case 93:
		//line parsercopy.y:528
		{
	
	              }
	case 94:
		//line parsercopy.y:531
		{
	
	              }
	case 95:
		//line parsercopy.y:534
		{
	
	              }
	case 96:
		//line parsercopy.y:537
		{
	
	              }
	case 97:
		//line parsercopy.y:540
		{
	
	              }
	case 98:
		//line parsercopy.y:543
		{
	
	}
	case 99:
		//line parsercopy.y:547
		{   //DONE
    
	          }
	case 100:
		//line parsercopy.y:550
		{
	    
	          }
	case 101:
		//line parsercopy.y:553
		{
	
	}
	case 102:
		//line parsercopy.y:564
		{
	    
	                    }
	case 103:
		//line parsercopy.y:567
		{
	    
	                    }
	case 104:
		//line parsercopy.y:570
		{
	    
	                    }
	case 105:
		//line parsercopy.y:573
		{
	
	}
	case 106:
		//line parsercopy.y:577
		{
	    
	        }
	case 107:
		//line parsercopy.y:580
		{
	
	}
	case 108:
		//line parsercopy.y:584
		{
	
	}
	case 109:
		//line parsercopy.y:588
		{
	    
	                      }
	case 110:
		//line parsercopy.y:591
		{
	
	}
	case 111:
		//line parsercopy.y:595
		{
	
	}
	case 112:
		//line parsercopy.y:599
		{
	
	}
	case 113:
		//line parsercopy.y:603
		{
	
	}
	case 114:
		//line parsercopy.y:607
		{
	
	}
	case 115:
		//line parsercopy.y:611
		{
	    
	                  }
	case 116:
		//line parsercopy.y:614
		{
	    
	}
	case 117:
		//line parsercopy.y:618
		{
	                    
	                    }
	case 118:
		//line parsercopy.y:621
		{
	
	}
	case 119:
		//line parsercopy.y:625
		{
	
	      }
	case 120:
		//line parsercopy.y:628
		{
	
	      }
	case 121:
		//line parsercopy.y:632
		{
	
	}
	case 122:
		//line parsercopy.y:636
		{
	
	}
	case 123:
		//line parsercopy.y:640
		{
	    
	          }
	case 124:
		//line parsercopy.y:643
		{
	    
	}
	case 125:
		//line parsercopy.y:647
		{
	
	}
	case 126:
		//line parsercopy.y:651
		{
	    
	                }
	case 127:
		//line parsercopy.y:654
		{
	
	}
	case 128:
		//line parsercopy.y:658
		{
	    
	            }
	case 129:
		//line parsercopy.y:661
		{
	  
	}
	case 130:
		//line parsercopy.y:665
		{
	
	              }
	case 131:
		//line parsercopy.y:668
		{
	
	}
	case 132:
		//line parsercopy.y:672
		{
	
	}
	case 133:
		//line parsercopy.y:676
		{
	
	}
	case 134:
		//line parsercopy.y:680
		{
	
	              }
	case 135:
		//line parsercopy.y:683
		{
	
	}
	case 136:
		//line parsercopy.y:688
		{   //DONE
                    
	                    }
	case 137:
		//line parsercopy.y:691
		{
	
	}
	case 138:
		//line parsercopy.y:695
		{   //DONE

	                      }
	case 139:
		//line parsercopy.y:698
		{
	
	}
	case 140:
		//line parsercopy.y:702
		{ //TODO

	}
	case 141:
		//line parsercopy.y:706
		{
	            
	            }
	case 142:
		//line parsercopy.y:709
		{
	            
	            }
	case 143:
		//line parsercopy.y:712
		{
	            
	            }
	case 144:
		//line parsercopy.y:715
		{
	
	}
	case 145:
		//line parsercopy.y:719
		{
	    
	            }
	case 146:
		//line parsercopy.y:722
		{
	
	}
	case 147:
		//line parsercopy.y:726
		{
	    
	              }
	case 148:
		//line parsercopy.y:729
		{
	
	}
	case 149:
		//line parsercopy.y:733
		{
	                      
	                      }
	case 150:
		//line parsercopy.y:736
		{
	    
	                      }
	case 151:
		//line parsercopy.y:739
		{
	    
	                      }
	case 152:
		//line parsercopy.y:742
		{
	    
	}
	case 153:
		//line parsercopy.y:746
		{
	    
	          }
	case 154:
		//line parsercopy.y:749
		{
	          //tuple_literal COLON{

	}
	case 155:
		//line parsercopy.y:754
		{
	    
	                }
	case 156:
		//line parsercopy.y:757
		{
	    
	}
	case 157:
		//line parsercopy.y:761
		{
	    
	              }
	case 158:
		//line parsercopy.y:764
		{
	
	}
	case 159:
		//line parsercopy.y:768
		{
	
	                    }
	case 160:
		//line parsercopy.y:771
		{
	
	}
	case 161:
		//line parsercopy.y:775
		{
	            
	            }
	case 162:
		//line parsercopy.y:778
		{
	   
	            }
	case 163:
		//line parsercopy.y:781
		{
	           
	        }
	case 164:
		//line parsercopy.y:787
		{
	            
	            }
	case 165:
		//line parsercopy.y:790
		{
	    
	}
	}
	goto yystack /* stack new state and value */
}
