
//line parser.y:2

package parser
import __yyfmt__ "fmt"
//line parser.y:3
		
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


//line parser.y:23
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
const NEW_LINE = 57394

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
	"NEW_LINE",
}
var yyStatenames = []string{}

const yyEofCode = 1
const yyErrCode = 2
const yyMaxDepth = 200

//line parser.y:819

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
	4, 58,
	8, 58,
	10, 58,
	-2, 53,
	-1, 55,
	4, 61,
	8, 61,
	10, 61,
	-2, 56,
	-1, 63,
	4, 59,
	8, 59,
	10, 59,
	-2, 79,
	-1, 90,
	4, 58,
	8, 58,
	10, 58,
	-2, 53,
	-1, 183,
	4, 60,
	8, 60,
	10, 60,
	-2, 54,
}

const yyNprod = 168
const yyPrivate = 57344

var yyTokenNames []string
var yyStates []string

const yyLast = 520

var yyAct = []int{

	90, 17, 161, 17, 96, 196, 17, 78, 3, 135,
	10, 74, 142, 63, 76, 11, 97, 79, 199, 42,
	52, 5, 50, 105, 131, 209, 104, 141, 31, 77,
	95, 154, 28, 12, 147, 107, 88, 145, 83, 82,
	98, 91, 27, 84, 81, 26, 99, 6, 114, 53,
	201, 73, 208, 71, 225, 108, 157, 123, 101, 85,
	92, 80, 100, 26, 102, 172, 187, 124, 54, 129,
	140, 72, 66, 67, 68, 69, 70, 134, 26, 44,
	146, 171, 72, 86, 87, 122, 170, 143, 84, 143,
	227, 125, 126, 127, 128, 224, 65, 89, 193, 185,
	149, 144, 231, 160, 107, 120, 121, 116, 117, 118,
	119, 112, 107, 113, 150, 158, 159, 114, 84, 151,
	152, 153, 244, 243, 108, 114, 186, 167, 43, 217,
	166, 165, 108, 168, 164, 169, 173, 174, 175, 176,
	177, 178, 192, 163, 251, 179, 180, 242, 189, 197,
	181, 182, 139, 239, 238, 143, 137, 188, 136, 233,
	205, 207, 194, 17, 115, 162, 73, 190, 71, 240,
	109, 98, 191, 156, 204, 223, 218, 206, 184, 183,
	202, 148, 106, 111, 211, 103, 72, 66, 67, 68,
	69, 70, 34, 26, 32, 2, 19, 216, 200, 110,
	75, 226, 215, 198, 220, 230, 222, 143, 155, 143,
	214, 232, 17, 17, 17, 24, 219, 229, 23, 228,
	245, 203, 94, 93, 22, 197, 21, 237, 16, 15,
	241, 14, 207, 211, 211, 211, 246, 234, 235, 236,
	13, 17, 98, 195, 132, 130, 248, 249, 250, 247,
	53, 64, 73, 210, 71, 62, 61, 60, 43, 59,
	58, 57, 138, 56, 55, 51, 49, 48, 47, 54,
	46, 45, 72, 66, 67, 68, 69, 70, 41, 26,
	44, 53, 40, 73, 39, 71, 29, 38, 30, 43,
	212, 213, 20, 25, 37, 36, 35, 65, 9, 33,
	54, 18, 8, 72, 66, 67, 68, 69, 70, 7,
	26, 44, 53, 4, 73, 1, 71, 29, 0, 30,
	43, 212, 213, 20, 25, 0, 0, 0, 65, 9,
	0, 54, 0, 0, 72, 66, 67, 68, 69, 70,
	0, 26, 44, 53, 0, 73, 0, 71, 29, 0,
	30, 43, 0, 0, 20, 25, 0, 0, 6, 65,
	9, 0, 54, 0, 0, 72, 66, 67, 68, 69,
	70, 0, 26, 44, 53, 0, 73, 221, 71, 29,
	0, 30, 0, 0, 0, 20, 25, 0, 0, 0,
	65, 9, 0, 54, 0, 0, 72, 66, 67, 133,
	69, 70, 0, 26, 44, 53, 0, 73, 0, 71,
	0, 0, 0, 0, 53, 0, 73, 0, 71, 0,
	0, 65, 0, 0, 54, 0, 0, 72, 66, 67,
	68, 69, 70, 54, 26, 44, 72, 66, 67, 68,
	69, 70, 0, 26, 44, 53, 0, 73, 0, 71,
	80, 0, 65, 0, 53, 0, 73, 0, 71, 0,
	0, 65, 0, 0, 54, 0, 0, 72, 66, 67,
	133, 69, 70, 54, 26, 44, 72, 66, 67, 68,
	69, 70, 0, 26, 115, 0, 73, 0, 71, 0,
	109, 0, 65, 0, 0, 0, 0, 0, 112, 0,
	113, 65, 0, 111, 0, 0, 72, 66, 67, 68,
	69, 70, 0, 26, 0, 0, 0, 0, 0, 110,
}
var yyPact = []int{

	308, -1000, 339, -1000, -3, 339, 12, -1000, -1000, -1000,
	-8, -13, -14, -1000, -1000, -1000, -1000, 73, -1000, 48,
	45, -1000, -1000, -1000, -1000, 410, -1000, -1000, 23, 30,
	410, 26, 20, 29, 480, -1000, -1000, -1000, -1000, -1000,
	-1000, -1000, 89, 56, 450, -1000, -1000, 69, -1000, -1000,
	-1000, -1000, -1000, 441, 450, -1000, 148, -1000, -1000, -1000,
	-1000, -1000, -1000, -1000, -1000, 41, -1000, -1000, -1000, -1000,
	-1000, 410, -1000, 410, -1000, -1000, -1000, -15, 28, -18,
	177, -1000, -1000, -1000, 401, 116, 116, 116, -21, -1000,
	-1000, 167, -1000, 15, 104, 103, -1000, -1000, 90, 159,
	410, 116, 410, 480, -1000, -1000, 93, -1000, -1000, -1000,
	-1000, 160, 60, 55, -1000, 36, 450, 450, 450, 450,
	450, 450, -1000, -1000, -1000, 450, 450, 450, 450, 174,
	173, -1000, 86, 114, -1000, -1000, 37, 450, 148, 410,
	166, 133, -1000, 85, 155, -1000, -1000, -1000, 30, -1000,
	-1000, -1000, -1000, -1000, -1000, -1000, 2, 450, 410, 30,
	30, 9, 246, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	-1000, -1000, 114, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	-1000, -1000, -1000, -1000, -1000, 36, 410, 148, 120, -1000,
	171, 370, -1000, 410, -1000, 170, -1000, 82, 47, -1000,
	2, 410, 159, 91, -1000, -1000, -1000, 73, 159, 152,
	-1000, 277, 277, 277, -1000, -1000, -1000, 148, -1000, 147,
	146, -1000, -1000, 163, 30, -1000, 140, 111, -1000, 110,
	-1000, 30, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	339, -1000, -1000, 159, 159, 159, -1000, 137, -1000, -1000,
	-1000, -1000,
}
var yyPgo = []int{

	0, 315, 195, 8, 313, 29, 0, 10, 21, 309,
	302, 15, 33, 301, 42, 28, 299, 296, 295, 294,
	287, 284, 282, 278, 19, 22, 271, 270, 268, 267,
	266, 265, 20, 264, 263, 9, 13, 262, 27, 12,
	261, 260, 259, 257, 256, 255, 251, 245, 24, 244,
	243, 5, 240, 231, 229, 228, 226, 224, 223, 2,
	222, 221, 220, 4, 16, 218, 215, 208, 203, 201,
	18, 198, 25, 196, 32, 194, 192, 185, 26, 23,
	182,
}
var yyR1 = []int{

	0, 1, 1, 1, 2, 2, 4, 4, 4, 3,
	3, 8, 8, 8, 9, 9, 9, 7, 7, 6,
	11, 13, 14, 14, 15, 15, 16, 16, 16, 16,
	16, 16, 16, 16, 19, 20, 21, 22, 17, 18,
	23, 24, 24, 24, 26, 27, 28, 28, 28, 29,
	30, 25, 25, 25, 25, 25, 25, 31, 34, 34,
	34, 34, 35, 35, 35, 35, 35, 35, 37, 38,
	38, 39, 39, 32, 32, 32, 32, 32, 32, 32,
	32, 40, 41, 42, 42, 43, 44, 36, 46, 45,
	47, 47, 48, 48, 49, 5, 50, 50, 51, 51,
	10, 10, 10, 10, 53, 53, 56, 58, 58, 57,
	60, 61, 62, 63, 63, 64, 64, 54, 54, 66,
	65, 52, 52, 55, 67, 67, 68, 68, 70, 70,
	71, 69, 59, 59, 72, 72, 72, 72, 72, 72,
	33, 33, 33, 12, 12, 12, 12, 73, 73, 74,
	74, 75, 75, 75, 75, 76, 76, 77, 77, 79,
	79, 79, 79, 79, 78, 78, 80, 80,
}
var yyR2 = []int{

	0, 2, 1, 1, 2, 1, 3, 3, 3, 2,
	1, 1, 1, 1, 2, 2, 2, 3, 3, 1,
	1, 1, 3, 1, 3, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 3, 3, 3, 3, 3, 3,
	2, 1, 1, 1, 3, 3, 1, 1, 1, 3,
	3, 1, 1, 1, 3, 2, 1, 2, 1, 1,
	3, 1, 3, 2, 4, 3, 2, 1, 3, 0,
	1, 3, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 3, 1, 3, 3,
	0, 1, 3, 1, 3, 7, 0, 1, 3, 1,
	1, 1, 1, 1, 1, 1, 5, 1, 3, 7,
	1, 1, 1, 0, 1, 3, 1, 1, 1, 3,
	5, 3, 2, 3, 3, 4, 0, 1, 2, 1,
	4, 3, 3, 2, 2, 2, 2, 1, 1, 1,
	5, 5, 4, 3, 3, 3, 1, 2, 1, 3,
	1, 3, 2, 2, 1, 2, 2, 2, 1, 1,
	1, 1, 1, 2, 2, 1, 2, 2,
}
var yyChk = []int{

	-1000, -1, -2, -3, -4, -8, 50, -9, -10, 52,
	-7, -11, -12, -52, -53, -54, -55, -6, -13, -73,
	46, -56, -57, -65, -66, 47, 33, -14, -74, 40,
	42, -15, -75, -16, -76, -17, -18, -19, -20, -21,
	-22, -23, -24, 12, 34, -26, -27, -28, -29, -30,
	-25, -31, -32, 4, 23, -33, -34, -40, -41, -42,
	-43, -44, -45, -36, -46, 51, 27, 28, 29, 30,
	31, 8, 26, 6, -3, -2, -3, -5, -6, -7,
	49, 52, 52, 52, 15, 11, 35, 36, -11, 52,
	-6, -11, 37, -58, -60, -6, -63, -64, -7, -11,
	36, 38, 35, -77, -78, -79, -80, -6, -32, 10,
	39, 23, 18, 20, -36, 4, 18, 19, 20, 21,
	16, 17, 29, -36, -25, 22, 23, 24, 25, -11,
	-47, -48, -49, 29, -25, -35, 10, 8, -37, 4,
	29, -38, -39, -11, -38, 52, 52, 52, 4, -11,
	-5, -12, -12, -12, 52, -67, 6, 41, 11, 13,
	13, -59, 6, -14, -74, -15, -78, -79, -78, -79,
	26, 26, 29, -24, -24, -24, -24, -24, -24, -24,
	-24, -25, -25, 5, 5, 13, 12, 29, -24, -35,
	-38, 6, 9, 13, 7, -50, -51, -6, -68, -70,
	-71, 48, -25, -61, -11, -6, -64, -6, 43, -72,
	7, -8, 44, 45, -48, -11, -35, 9, 5, -48,
	-39, 7, -39, 5, 13, 7, -69, 43, -70, -11,
	-59, 11, -59, 7, -72, -72, -72, -35, 7, 7,
	6, -51, 7, 12, 12, -62, -63, -3, -59, -59,
	-59, 7,
}
var yyDef = []int{

	0, -2, 2, 3, 5, 10, 0, 11, 12, 13,
	0, 0, 0, 100, 101, 102, 103, -2, 20, 146,
	0, 104, 105, 117, 118, 0, 19, 21, 148, 113,
	0, 23, 150, 25, 154, 26, 27, 28, 29, 30,
	31, 32, 33, 0, 0, 41, 42, 43, 46, 47,
	48, 51, 52, 90, 0, -2, 0, 73, 74, 75,
	76, 77, 78, -2, 80, 0, 81, 82, 83, 84,
	85, 69, 87, 69, 1, 4, 9, 0, 0, 0,
	0, 14, 15, 16, 0, 0, 0, 0, 0, 122,
	-2, 0, 147, 0, 0, 107, 110, 114, 116, 0,
	0, 0, 0, 153, 152, 158, 165, 159, 160, 161,
	162, 0, 0, 0, 79, 90, 0, 0, 0, 0,
	0, 0, 155, 156, 40, 0, 0, 0, 0, 0,
	0, 91, 93, 83, 55, 57, 0, 0, 67, 69,
	0, 0, 70, 72, 0, 6, 7, 8, 96, 17,
	18, 143, 144, 145, 121, 123, 126, 0, 0, 0,
	0, 119, 0, 22, 149, 24, 151, 157, 164, 163,
	166, 167, 0, 34, 35, 36, 37, 38, 39, 44,
	45, 49, 50, -2, 89, 0, 0, 63, 0, 66,
	0, 0, 86, 0, 88, 0, 97, 99, 0, 127,
	129, 0, 0, 0, 111, 108, 115, 0, 0, 0,
	133, 137, 138, 139, 92, 94, 62, 65, 68, 0,
	0, 142, 71, 0, 0, 124, 0, 0, 128, 0,
	106, 113, 120, 132, 134, 135, 136, 64, 140, 141,
	0, 98, 125, 0, 0, 0, 112, 0, 131, 130,
	109, 95,
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
	52,
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
		//line parser.y:182
		{  
	
	        }
	case 2:
		//line parser.y:185
		{
	
	        }
	case 3:
		//line parser.y:188
		{
	
	}
	case 4:
		//line parser.y:193
		{
	
	            }
	case 5:
		//line parser.y:196
		{
	
	}
	case 6:
		//line parser.y:201
		{   
	    
	          }
	case 7:
		//line parser.y:204
		{
	    
	          }
	case 8:
		//line parser.y:207
		{
	
	}
	case 9:
		//line parser.y:212
		{  
	    
	                }
	case 10:
		//line parser.y:215
		{
	
	}
	case 11:
		//line parser.y:220
		{
	
	          }
	case 12:
		//line parser.y:223
		{
	    
	          }
	case 13:
		//line parser.y:226
		{  // empty statement

	}
	case 14:
		//line parser.y:231
		{  // Assignment expression

	                      }
	case 15:
		//line parser.y:234
		{  // General expression (RHS of an assignment)

	                      }
	case 16:
		//line parser.y:237
		{  // Pipeline expressions 

	}
	case 17:
		//line parser.y:242
		{
	    
	            }
	case 18:
		//line parser.y:245
		{
	
	}
	case 19:
		//line parser.y:250
		{
	    
	}
	case 20:
		//line parser.y:255
		{
	
	}
	case 21:
		//line parser.y:259
		{
	
	}
	case 22:
		//line parser.y:263
		{
	
	              }
	case 23:
		//line parser.y:266
		{
	    
	}
	case 24:
		//line parser.y:270
		{
	
	                }
	case 25:
		//line parser.y:273
		{
	
	}
	case 26:
		//line parser.y:277
		{
	    
	            }
	case 27:
		//line parser.y:280
		{
	    
	            }
	case 28:
		//line parser.y:283
		{
	    
	            }
	case 29:
		//line parser.y:286
		{
	    
	            }
	case 30:
		//line parser.y:289
		{
	    
	            }
	case 31:
		//line parser.y:292
		{
	    
	            }
	case 32:
		//line parser.y:295
		{
	    
	            }
	case 33:
		//line parser.y:298
		{
	
	}
	case 34:
		//line parser.y:302
		{  
	
	}
	case 35:
		//line parser.y:306
		{    
	
	}
	case 36:
		//line parser.y:310
		{ 
	
	}
	case 37:
		//line parser.y:314
		{    
	
	}
	case 38:
		//line parser.y:318
		{  
	
	}
	case 39:
		//line parser.y:322
		{   
	
	}
	case 40:
		//line parser.y:326
		{   
	    
	}
	case 41:
		//line parser.y:330
		{ 
	
	    }
	case 42:
		//line parser.y:333
		{
	    
	    }
	case 43:
		//line parser.y:336
		{
	    
	}
	case 44:
		//line parser.y:340
		{    
	    
	}
	case 45:
		//line parser.y:344
		{   
	    
	}
	case 46:
		//line parser.y:348
		{ 
	    
	        }
	case 47:
		//line parser.y:351
		{
	    
	        }
	case 48:
		//line parser.y:354
		{
	    
	}
	case 49:
		//line parser.y:358
		{   
	    
	}
	case 50:
		//line parser.y:362
		{ 
	    
	}
	case 51:
		//line parser.y:367
		{
	
	      }
	case 52:
		//line parser.y:370
		{
	    
	      }
	case 53:
		//line parser.y:373
		{
	        
	      }
	case 54:
		//line parser.y:376
		{
	    
	      }
	case 55:
		//line parser.y:379
		{
	    
	      }
	case 56:
		//line parser.y:382
		{
	   
	}
	case 57:
		//line parser.y:387
		{  
	
	}
	case 58:
		//line parser.y:392
		{ 
	    
	            }
	case 59:
		//line parser.y:395
		{ 
	    
	            }
	case 60:
		//line parser.y:398
		{
	
	            }
	case 61:
		//line parser.y:401
		{
	
	}
	case 62:
		//line parser.y:406
		{ // Value access and more

	              }
	case 63:
		//line parser.y:409
		{ // Value access (chain end)

	              }
	case 64:
		//line parser.y:412
		{ // Square access and more

	              }
	case 65:
		//line parser.y:415
		{ // Square access (chain end)

	              }
	case 66:
		//line parser.y:418
		{ // function call and more

	              }
	case 67:
		//line parser.y:421
		{ // function call (chain end)

	}
	case 68:
		//line parser.y:426
		{
	
	}
	case 69:
		//line parser.y:431
		{ 
	              
	        }
	case 70:
		//line parser.y:434
		{
	    
	}
	case 71:
		//line parser.y:439
		{    
	                
	              }
	case 72:
		//line parser.y:442
		{
	    
	}
	case 73:
		//line parser.y:447
		{
	
	        }
	case 74:
		//line parser.y:450
		{
	
	        }
	case 75:
		//line parser.y:453
		{
	
	        }
	case 76:
		//line parser.y:456
		{
	
	        }
	case 77:
		//line parser.y:459
		{
	
	        }
	case 78:
		//line parser.y:462
		{
	
	        }
	case 79:
		//line parser.y:465
		{
	
	        }
	case 80:
		//line parser.y:468
		{
	
	}
	case 81:
		//line parser.y:472
		{
	
	}
	case 82:
		//line parser.y:476
		{
	
	}
	case 83:
		//line parser.y:480
		{
	    
	                }
	case 84:
		//line parser.y:483
		{
	
	}
	case 85:
		//line parser.y:487
		{
	
	}
	case 86:
		//line parser.y:492
		{
	    
	}
	case 87:
		//line parser.y:496
		{
	    
	}
	case 88:
		//line parser.y:500
		{
	   
	}
	case 89:
		//line parser.y:505
		{
	
	}
	case 90:
		//line parser.y:510
		{   
	                    
	                  }
	case 91:
		//line parser.y:513
		{
	
	}
	case 92:
		//line parser.y:518
		{   
	
	                    }
	case 93:
		//line parser.y:521
		{
	
	}
	case 94:
		//line parser.y:526
		{ //TODO

	}
	case 95:
		//line parser.y:531
		{
	
	}
	case 96:
		//line parser.y:536
		{
	                
	              }
	case 97:
		//line parser.y:539
		{
	
	}
	case 98:
		//line parser.y:544
		{
	
	                  }
	case 99:
		//line parser.y:547
		{
	
	}
	case 100:
		//line parser.y:552
		{
	    
	                  }
	case 101:
		//line parser.y:555
		{
	    
	                  }
	case 102:
		//line parser.y:558
		{
	    
	                  }
	case 103:
		//line parser.y:561
		{
	
	}
	case 104:
		//line parser.y:566
		{
	    
	      }
	case 105:
		//line parser.y:569
		{
	
	}
	case 106:
		//line parser.y:574
		{
	
	}
	case 107:
		//line parser.y:579
		{ // Only value
    
	                    }
	case 108:
		//line parser.y:582
		{ // Key and value

	}
	case 109:
		//line parser.y:587
		{
	
	}
	case 110:
		//line parser.y:592
		{
	
	}
	case 111:
		//line parser.y:597
		{
	
	}
	case 112:
		//line parser.y:602
		{
	
	}
	case 113:
		//line parser.y:607
		{
	    
	                }
	case 114:
		//line parser.y:610
		{
	    
	}
	case 115:
		//line parser.y:615
		{
	                    
	                  }
	case 116:
		//line parser.y:618
		{
	
	}
	case 117:
		//line parser.y:623
		{
	
	    }
	case 118:
		//line parser.y:626
		{
	
	    }
	case 119:
		//line parser.y:631
		{
	
	}
	case 120:
		//line parser.y:636
		{
	
	}
	case 121:
		//line parser.y:641
		{
	    
	        }
	case 122:
		//line parser.y:644
		{
	    
	}
	case 123:
		//line parser.y:649
		{
	
	}
	case 124:
		//line parser.y:654
		{
	    
	              }
	case 125:
		//line parser.y:657
		{
	
	}
	case 126:
		//line parser.y:662
		{
	    
	          }
	case 127:
		//line parser.y:665
		{
	  
	}
	case 128:
		//line parser.y:670
		{
	
	            }
	case 129:
		//line parser.y:673
		{
	
	}
	case 130:
		//line parser.y:678
		{
	
	}
	case 131:
		//line parser.y:683
		{
	
	}
	case 132:
		//line parser.y:688
		{
	    
	      }
	case 133:
		//line parser.y:691
		{ // It can be empty

	}
	case 134:
		//line parser.y:696
		{
	
	                      }
	case 135:
		//line parser.y:699
		{
	
	                      }
	case 136:
		//line parser.y:702
		{
	
	                      }
	case 137:
		//line parser.y:705
		{
	
	                      }
	case 138:
		//line parser.y:708
		{
	
	                      }
	case 139:
		//line parser.y:711
		{
	
	}
	case 140:
		//line parser.y:716
		{
	
	            }
	case 141:
		//line parser.y:719
		{
	
	            }
	case 142:
		//line parser.y:722
		{
	
	}
	case 143:
		//line parser.y:727
		{ // More pipelines on the same line
            
	          }
	case 144:
		//line parser.y:730
		{ // The second pipeline is executed if and only if there is no error in the first one
            
	          }
	case 145:
		//line parser.y:733
		{ // The second pipeline is executed if and only if there is an error in the first one????
            
	          }
	case 146:
		//line parser.y:736
		{
	
	}
	case 147:
		//line parser.y:741
		{
	    
	          }
	case 148:
		//line parser.y:744
		{
	
	}
	case 149:
		//line parser.y:749
		{
	    
	            }
	case 150:
		//line parser.y:752
		{
	
	}
	case 151:
		//line parser.y:757
		{
	                      
	                    }
	case 152:
		//line parser.y:760
		{
	    
	                    }
	case 153:
		//line parser.y:763
		{
	    
	                    }
	case 154:
		//line parser.y:766
		{
	    
	}
	case 155:
		//line parser.y:771
		{
	    
	        }
	case 156:
		//line parser.y:774
		{
	
	}
	case 157:
		//line parser.y:779
		{
	
	                }
	case 158:
		//line parser.y:782
		{
	
	}
	case 159:
		//line parser.y:787
		{
	            
	          }
	case 160:
		//line parser.y:790
		{
	   
	          }
	case 161:
		//line parser.y:793
		{ // Current directory
           
	          }
	case 162:
		//line parser.y:796
		{
	            
	          }
	case 163:
		//line parser.y:799
		{
	    
	}
	case 164:
		//line parser.y:804
		{
	    
	                  }
	case 165:
		//line parser.y:807
		{
	    
	}
	case 166:
		//line parser.y:812
		{
	    
	            }
	case 167:
		//line parser.y:815
		{
	
	}
	}
	goto yystack /* stack new state and value */
}
