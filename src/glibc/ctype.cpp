#include "ctype.h"
#include "utils.hpp"

#pragma region ctype arrays

constexpr unsigned short CTYPE_B_ARRAY[] {
	// <-128>
	0,
	// <-127>
	0,
	// <-126>
	0,
	// <-125>
	0,
	// <-124>
	0,
	// <-123>
	0,
	// <-122>
	0,
	// <-121>
	0,
	// <-120>
	0,
	// <-119>
	0,
	// <-118>
	0,
	// <-117>
	0,
	// <-116>
	0,
	// <-115>
	0,
	// <-114>
	0,
	// <-113>
	0,
	// <-112>
	0,
	// <-111>
	0,
	// <-110>
	0,
	// <-109>
	0,
	// <-108>
	0,
	// <-107>
	0,
	// <-106>
	0,
	// <-105>
	0,
	// <-104>
	0,
	// <-103>
	0,
	// <-102>
	0,
	// <-101>
	0,
	// <-100>
	0,
	// <-99>
	0,
	// <-98>
	0,
	// <-97>
	0,
	// <-96>
	0,
	// <-95>
	0,
	// <-94>
	0,
	// <-93>
	0,
	// <-92>
	0,
	// <-91>
	0,
	// <-90>
	0,
	// <-89>
	0,
	// <-88>
	0,
	// <-87>
	0,
	// <-86>
	0,
	// <-85>
	0,
	// <-84>
	0,
	// <-83>
	0,
	// <-82>
	0,
	// <-81>
	0,
	// <-80>
	0,
	// <-79>
	0,
	// <-78>
	0,
	// <-77>
	0,
	// <-76>
	0,
	// <-75>
	0,
	// <-74>
	0,
	// <-73>
	0,
	// <-72>
	0,
	// <-71>
	0,
	// <-70>
	0,
	// <-69>
	0,
	// <-68>
	0,
	// <-67>
	0,
	// <-66>
	0,
	// <-65>
	0,
	// <-64>
	0,
	// <-63>
	0,
	// <-62>
	0,
	// <-61>
	0,
	// <-60>
	0,
	// <-59>
	0,
	// <-58>
	0,
	// <-57>
	0,
	// <-56>
	0,
	// <-55>
	0,
	// <-54>
	0,
	// <-53>
	0,
	// <-52>
	0,
	// <-51>
	0,
	// <-50>
	0,
	// <-49>
	0,
	// <-48>
	0,
	// <-47>
	0,
	// <-46>
	0,
	// <-45>
	0,
	// <-44>
	0,
	// <-43>
	0,
	// <-42>
	0,
	// <-41>
	0,
	// <-40>
	0,
	// <-39>
	0,
	// <-38>
	0,
	// <-37>
	0,
	// <-36>
	0,
	// <-35>
	0,
	// <-34>
	0,
	// <-33>
	0,
	// <-32>
	0,
	// <-31>
	0,
	// <-30>
	0,
	// <-29>
	0,
	// <-28>
	0,
	// <-27>
	0,
	// <-26>
	0,
	// <-25>
	0,
	// <-24>
	0,
	// <-23>
	0,
	// <-22>
	0,
	// <-21>
	0,
	// <-20>
	0,
	// <-19>
	0,
	// <-18>
	0,
	// <-17>
	0,
	// <-16>
	0,
	// <-15>
	0,
	// <-14>
	0,
	// <-13>
	0,
	// <-12>
	0,
	// <-11>
	0,
	// <-10>
	0,
	// <-9>
	0,
	// <-8>
	0,
	// <-7>
	0,
	// <-6>
	0,
	// <-5>
	0,
	// <-4>
	0,
	// <-3>
	0,
	// <-2>
	0,
	// <-1>
	0,
	// <0>
	_IScntrl,
	// <1>
	_IScntrl,
	// <2>
	_IScntrl,
	// <3>
	_IScntrl,
	// <4>
	_IScntrl,
	// <5>
	_IScntrl,
	// <6>
	_IScntrl,
	// <7>
	_IScntrl,
	// <8>
	_IScntrl,
	// <9>
	_ISspace | _ISblank | _IScntrl,
	// <10>
	_ISspace | _IScntrl,
	// <11>
	_ISspace | _IScntrl,
	// <12>
	_ISspace | _IScntrl,
	// <13>
	_ISspace | _IScntrl,
	// <14>
	_IScntrl,
	// <15>
	_IScntrl,
	// <16>
	_IScntrl,
	// <17>
	_IScntrl,
	// <18>
	_IScntrl,
	// <19>
	_IScntrl,
	// <20>
	_IScntrl,
	// <21>
	_IScntrl,
	// <22>
	_IScntrl,
	// <23>
	_IScntrl,
	// <24>
	_IScntrl,
	// <25>
	_IScntrl,
	// <26>
	_IScntrl,
	// <27>
	_IScntrl,
	// <28>
	_IScntrl,
	// <29>
	_IScntrl,
	// <30>
	_IScntrl,
	// <31>
	_IScntrl,
	// ' '
	_ISspace | _ISprint | _ISblank,
	// '!'
	_ISprint | _ISgraph | _ISpunct,
	// '"'
	_ISprint | _ISgraph | _ISpunct,
	// '#'
	_ISprint | _ISgraph | _ISpunct,
	// '$'
	_ISprint | _ISgraph | _ISpunct,
	// '%'
	_ISprint | _ISgraph | _ISpunct,
	// '&'
	_ISprint | _ISgraph | _ISpunct,
	// '''
	_ISprint | _ISgraph | _ISpunct,
	// '('
	_ISprint | _ISgraph | _ISpunct,
	// ')'
	_ISprint | _ISgraph | _ISpunct,
	// '*'
	_ISprint | _ISgraph | _ISpunct,
	// '+'
	_ISprint | _ISgraph | _ISpunct,
	// ','
	_ISprint | _ISgraph | _ISpunct,
	// '-'
	_ISprint | _ISgraph | _ISpunct,
	// '.'
	_ISprint | _ISgraph | _ISpunct,
	// '/'
	_ISprint | _ISgraph | _ISpunct,
	// '0'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '1'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '2'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '3'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '4'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '5'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '6'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '7'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '8'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// '9'
	_ISdigit | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// ':'
	_ISprint | _ISgraph | _ISpunct,
	// ';'
	_ISprint | _ISgraph | _ISpunct,
	// '<'
	_ISprint | _ISgraph | _ISpunct,
	// '='
	_ISprint | _ISgraph | _ISpunct,
	// '>'
	_ISprint | _ISgraph | _ISpunct,
	// '?'
	_ISprint | _ISgraph | _ISpunct,
	// '@'
	_ISprint | _ISgraph | _ISpunct,
	// 'A'
	_ISupper | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'B'
	_ISupper | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'C'
	_ISupper | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'D'
	_ISupper | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'E'
	_ISupper | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'F'
	_ISupper | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'G'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'H'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'I'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'J'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'K'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'L'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'M'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'N'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'O'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'P'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'Q'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'R'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'S'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'T'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'U'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'V'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'W'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'X'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'Y'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'Z'
	_ISupper | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// '['
	_ISprint | _ISgraph | _ISpunct,
	// '\'
	_ISprint | _ISgraph | _ISpunct,
	// ']'
	_ISprint | _ISgraph | _ISpunct,
	// '^'
	_ISprint | _ISgraph | _ISpunct,
	// '_'
	_ISprint | _ISgraph | _ISpunct,
	// '`'
	_ISprint | _ISgraph | _ISpunct,
	// 'a'
	_ISlower | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'b'
	_ISlower | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'c'
	_ISlower | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'd'
	_ISlower | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'e'
	_ISlower | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'f'
	_ISlower | _ISalpha | _ISxdigit | _ISprint | _ISgraph | _ISalnum,
	// 'g'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'h'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'i'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'j'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'k'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'l'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'm'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'n'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'o'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'p'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'q'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'r'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 's'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 't'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'u'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'v'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'w'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'x'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'y'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// 'z'
	_ISlower | _ISalpha | _ISprint | _ISgraph | _ISalnum,
	// '{'
	_ISprint | _ISgraph | _ISpunct,
	// '|'
	_ISprint | _ISgraph | _ISpunct,
	// '}'
	_ISprint | _ISgraph | _ISpunct,
	// '~'
	_ISprint | _ISgraph | _ISpunct,
	// <127>
	_IScntrl,
	// <128>
	0,
	// <129>
	0,
	// <130>
	0,
	// <131>
	0,
	// <132>
	0,
	// <133>
	0,
	// <134>
	0,
	// <135>
	0,
	// <136>
	0,
	// <137>
	0,
	// <138>
	0,
	// <139>
	0,
	// <140>
	0,
	// <141>
	0,
	// <142>
	0,
	// <143>
	0,
	// <144>
	0,
	// <145>
	0,
	// <146>
	0,
	// <147>
	0,
	// <148>
	0,
	// <149>
	0,
	// <150>
	0,
	// <151>
	0,
	// <152>
	0,
	// <153>
	0,
	// <154>
	0,
	// <155>
	0,
	// <156>
	0,
	// <157>
	0,
	// <158>
	0,
	// <159>
	0,
	// <160>
	0,
	// <161>
	0,
	// <162>
	0,
	// <163>
	0,
	// <164>
	0,
	// <165>
	0,
	// <166>
	0,
	// <167>
	0,
	// <168>
	0,
	// <169>
	0,
	// <170>
	0,
	// <171>
	0,
	// <172>
	0,
	// <173>
	0,
	// <174>
	0,
	// <175>
	0,
	// <176>
	0,
	// <177>
	0,
	// <178>
	0,
	// <179>
	0,
	// <180>
	0,
	// <181>
	0,
	// <182>
	0,
	// <183>
	0,
	// <184>
	0,
	// <185>
	0,
	// <186>
	0,
	// <187>
	0,
	// <188>
	0,
	// <189>
	0,
	// <190>
	0,
	// <191>
	0,
	// <192>
	0,
	// <193>
	0,
	// <194>
	0,
	// <195>
	0,
	// <196>
	0,
	// <197>
	0,
	// <198>
	0,
	// <199>
	0,
	// <200>
	0,
	// <201>
	0,
	// <202>
	0,
	// <203>
	0,
	// <204>
	0,
	// <205>
	0,
	// <206>
	0,
	// <207>
	0,
	// <208>
	0,
	// <209>
	0,
	// <210>
	0,
	// <211>
	0,
	// <212>
	0,
	// <213>
	0,
	// <214>
	0,
	// <215>
	0,
	// <216>
	0,
	// <217>
	0,
	// <218>
	0,
	// <219>
	0,
	// <220>
	0,
	// <221>
	0,
	// <222>
	0,
	// <223>
	0,
	// <224>
	0,
	// <225>
	0,
	// <226>
	0,
	// <227>
	0,
	// <228>
	0,
	// <229>
	0,
	// <230>
	0,
	// <231>
	0,
	// <232>
	0,
	// <233>
	0,
	// <234>
	0,
	// <235>
	0,
	// <236>
	0,
	// <237>
	0,
	// <238>
	0,
	// <239>
	0,
	// <240>
	0,
	// <241>
	0,
	// <242>
	0,
	// <243>
	0,
	// <244>
	0,
	// <245>
	0,
	// <246>
	0,
	// <247>
	0,
	// <248>
	0,
	// <249>
	0,
	// <250>
	0,
	// <251>
	0,
	// <252>
	0,
	// <253>
	0,
	// <254>
	0,
	// <255>
	0
};

constexpr int32_t CTYPE_TOUPPER_ARRAY[] {
	// <-128>
	128,
	// <-127>
	129,
	// <-126>
	130,
	// <-125>
	131,
	// <-124>
	132,
	// <-123>
	133,
	// <-122>
	134,
	// <-121>
	135,
	// <-120>
	136,
	// <-119>
	137,
	// <-118>
	138,
	// <-117>
	139,
	// <-116>
	140,
	// <-115>
	141,
	// <-114>
	142,
	// <-113>
	143,
	// <-112>
	144,
	// <-111>
	145,
	// <-110>
	146,
	// <-109>
	147,
	// <-108>
	148,
	// <-107>
	149,
	// <-106>
	150,
	// <-105>
	151,
	// <-104>
	152,
	// <-103>
	153,
	// <-102>
	154,
	// <-101>
	155,
	// <-100>
	156,
	// <-99>
	157,
	// <-98>
	158,
	// <-97>
	159,
	// <-96>
	160,
	// <-95>
	161,
	// <-94>
	162,
	// <-93>
	163,
	// <-92>
	164,
	// <-91>
	165,
	// <-90>
	166,
	// <-89>
	167,
	// <-88>
	168,
	// <-87>
	169,
	// <-86>
	170,
	// <-85>
	171,
	// <-84>
	172,
	// <-83>
	173,
	// <-82>
	174,
	// <-81>
	175,
	// <-80>
	176,
	// <-79>
	177,
	// <-78>
	178,
	// <-77>
	179,
	// <-76>
	180,
	// <-75>
	181,
	// <-74>
	182,
	// <-73>
	183,
	// <-72>
	184,
	// <-71>
	185,
	// <-70>
	186,
	// <-69>
	187,
	// <-68>
	188,
	// <-67>
	189,
	// <-66>
	190,
	// <-65>
	191,
	// <-64>
	192,
	// <-63>
	193,
	// <-62>
	194,
	// <-61>
	195,
	// <-60>
	196,
	// <-59>
	197,
	// <-58>
	198,
	// <-57>
	199,
	// <-56>
	200,
	// <-55>
	201,
	// <-54>
	202,
	// <-53>
	203,
	// <-52>
	204,
	// <-51>
	205,
	// <-50>
	206,
	// <-49>
	207,
	// <-48>
	208,
	// <-47>
	209,
	// <-46>
	210,
	// <-45>
	211,
	// <-44>
	212,
	// <-43>
	213,
	// <-42>
	214,
	// <-41>
	215,
	// <-40>
	216,
	// <-39>
	217,
	// <-38>
	218,
	// <-37>
	219,
	// <-36>
	220,
	// <-35>
	221,
	// <-34>
	222,
	// <-33>
	223,
	// <-32>
	224,
	// <-31>
	225,
	// <-30>
	226,
	// <-29>
	227,
	// <-28>
	228,
	// <-27>
	229,
	// <-26>
	230,
	// <-25>
	231,
	// <-24>
	232,
	// <-23>
	233,
	// <-22>
	234,
	// <-21>
	235,
	// <-20>
	236,
	// <-19>
	237,
	// <-18>
	238,
	// <-17>
	239,
	// <-16>
	240,
	// <-15>
	241,
	// <-14>
	242,
	// <-13>
	243,
	// <-12>
	244,
	// <-11>
	245,
	// <-10>
	246,
	// <-9>
	247,
	// <-8>
	248,
	// <-7>
	249,
	// <-6>
	250,
	// <-5>
	251,
	// <-4>
	252,
	// <-3>
	253,
	// <-2>
	254,
	// <-1>
	-1,
	// <0>
	0,
	// <1>
	1,
	// <2>
	2,
	// <3>
	3,
	// <4>
	4,
	// <5>
	5,
	// <6>
	6,
	// <7>
	7,
	// <8>
	8,
	// <9>
	9,
	// <10>
	10,
	// <11>
	11,
	// <12>
	12,
	// <13>
	13,
	// <14>
	14,
	// <15>
	15,
	// <16>
	16,
	// <17>
	17,
	// <18>
	18,
	// <19>
	19,
	// <20>
	20,
	// <21>
	21,
	// <22>
	22,
	// <23>
	23,
	// <24>
	24,
	// <25>
	25,
	// <26>
	26,
	// <27>
	27,
	// <28>
	28,
	// <29>
	29,
	// <30>
	30,
	// <31>
	31,
	// ' '
	32,
	// '!'
	33,
	// '"'
	34,
	// '#'
	35,
	// '$'
	36,
	// '%'
	37,
	// '&'
	38,
	// '''
	39,
	// '('
	40,
	// ')'
	41,
	// '*'
	42,
	// '+'
	43,
	// ','
	44,
	// '-'
	45,
	// '.'
	46,
	// '/'
	47,
	// '0'
	48,
	// '1'
	49,
	// '2'
	50,
	// '3'
	51,
	// '4'
	52,
	// '5'
	53,
	// '6'
	54,
	// '7'
	55,
	// '8'
	56,
	// '9'
	57,
	// ':'
	58,
	// ';'
	59,
	// '<'
	60,
	// '='
	61,
	// '>'
	62,
	// '?'
	63,
	// '@'
	64,
	// 'A'
	65,
	// 'B'
	66,
	// 'C'
	67,
	// 'D'
	68,
	// 'E'
	69,
	// 'F'
	70,
	// 'G'
	71,
	// 'H'
	72,
	// 'I'
	73,
	// 'J'
	74,
	// 'K'
	75,
	// 'L'
	76,
	// 'M'
	77,
	// 'N'
	78,
	// 'O'
	79,
	// 'P'
	80,
	// 'Q'
	81,
	// 'R'
	82,
	// 'S'
	83,
	// 'T'
	84,
	// 'U'
	85,
	// 'V'
	86,
	// 'W'
	87,
	// 'X'
	88,
	// 'Y'
	89,
	// 'Z'
	90,
	// '['
	91,
	// '\'
	92,
	// ']'
	93,
	// '^'
	94,
	// '_'
	95,
	// '`'
	96,
	// 'A'
	65,
	// 'B'
	66,
	// 'C'
	67,
	// 'D'
	68,
	// 'E'
	69,
	// 'F'
	70,
	// 'G'
	71,
	// 'H'
	72,
	// 'I'
	73,
	// 'J'
	74,
	// 'K'
	75,
	// 'L'
	76,
	// 'M'
	77,
	// 'N'
	78,
	// 'O'
	79,
	// 'P'
	80,
	// 'Q'
	81,
	// 'R'
	82,
	// 'S'
	83,
	// 'T'
	84,
	// 'U'
	85,
	// 'V'
	86,
	// 'W'
	87,
	// 'X'
	88,
	// 'Y'
	89,
	// 'Z'
	90,
	// '{'
	123,
	// '|'
	124,
	// '}'
	125,
	// '~'
	126,
	// <127>
	127,
	// <128>
	128,
	// <129>
	129,
	// <130>
	130,
	// <131>
	131,
	// <132>
	132,
	// <133>
	133,
	// <134>
	134,
	// <135>
	135,
	// <136>
	136,
	// <137>
	137,
	// <138>
	138,
	// <139>
	139,
	// <140>
	140,
	// <141>
	141,
	// <142>
	142,
	// <143>
	143,
	// <144>
	144,
	// <145>
	145,
	// <146>
	146,
	// <147>
	147,
	// <148>
	148,
	// <149>
	149,
	// <150>
	150,
	// <151>
	151,
	// <152>
	152,
	// <153>
	153,
	// <154>
	154,
	// <155>
	155,
	// <156>
	156,
	// <157>
	157,
	// <158>
	158,
	// <159>
	159,
	// <160>
	160,
	// <161>
	161,
	// <162>
	162,
	// <163>
	163,
	// <164>
	164,
	// <165>
	165,
	// <166>
	166,
	// <167>
	167,
	// <168>
	168,
	// <169>
	169,
	// <170>
	170,
	// <171>
	171,
	// <172>
	172,
	// <173>
	173,
	// <174>
	174,
	// <175>
	175,
	// <176>
	176,
	// <177>
	177,
	// <178>
	178,
	// <179>
	179,
	// <180>
	180,
	// <181>
	181,
	// <182>
	182,
	// <183>
	183,
	// <184>
	184,
	// <185>
	185,
	// <186>
	186,
	// <187>
	187,
	// <188>
	188,
	// <189>
	189,
	// <190>
	190,
	// <191>
	191,
	// <192>
	192,
	// <193>
	193,
	// <194>
	194,
	// <195>
	195,
	// <196>
	196,
	// <197>
	197,
	// <198>
	198,
	// <199>
	199,
	// <200>
	200,
	// <201>
	201,
	// <202>
	202,
	// <203>
	203,
	// <204>
	204,
	// <205>
	205,
	// <206>
	206,
	// <207>
	207,
	// <208>
	208,
	// <209>
	209,
	// <210>
	210,
	// <211>
	211,
	// <212>
	212,
	// <213>
	213,
	// <214>
	214,
	// <215>
	215,
	// <216>
	216,
	// <217>
	217,
	// <218>
	218,
	// <219>
	219,
	// <220>
	220,
	// <221>
	221,
	// <222>
	222,
	// <223>
	223,
	// <224>
	224,
	// <225>
	225,
	// <226>
	226,
	// <227>
	227,
	// <228>
	228,
	// <229>
	229,
	// <230>
	230,
	// <231>
	231,
	// <232>
	232,
	// <233>
	233,
	// <234>
	234,
	// <235>
	235,
	// <236>
	236,
	// <237>
	237,
	// <238>
	238,
	// <239>
	239,
	// <240>
	240,
	// <241>
	241,
	// <242>
	242,
	// <243>
	243,
	// <244>
	244,
	// <245>
	245,
	// <246>
	246,
	// <247>
	247,
	// <248>
	248,
	// <249>
	249,
	// <250>
	250,
	// <251>
	251,
	// <252>
	252,
	// <253>
	253,
	// <254>
	254,
	// <255>
	255
};

constexpr int32_t CTYPE_TOLOWER_ARRAY[] {
	// <-128>
	128,
	// <-127>
	129,
	// <-126>
	130,
	// <-125>
	131,
	// <-124>
	132,
	// <-123>
	133,
	// <-122>
	134,
	// <-121>
	135,
	// <-120>
	136,
	// <-119>
	137,
	// <-118>
	138,
	// <-117>
	139,
	// <-116>
	140,
	// <-115>
	141,
	// <-114>
	142,
	// <-113>
	143,
	// <-112>
	144,
	// <-111>
	145,
	// <-110>
	146,
	// <-109>
	147,
	// <-108>
	148,
	// <-107>
	149,
	// <-106>
	150,
	// <-105>
	151,
	// <-104>
	152,
	// <-103>
	153,
	// <-102>
	154,
	// <-101>
	155,
	// <-100>
	156,
	// <-99>
	157,
	// <-98>
	158,
	// <-97>
	159,
	// <-96>
	160,
	// <-95>
	161,
	// <-94>
	162,
	// <-93>
	163,
	// <-92>
	164,
	// <-91>
	165,
	// <-90>
	166,
	// <-89>
	167,
	// <-88>
	168,
	// <-87>
	169,
	// <-86>
	170,
	// <-85>
	171,
	// <-84>
	172,
	// <-83>
	173,
	// <-82>
	174,
	// <-81>
	175,
	// <-80>
	176,
	// <-79>
	177,
	// <-78>
	178,
	// <-77>
	179,
	// <-76>
	180,
	// <-75>
	181,
	// <-74>
	182,
	// <-73>
	183,
	// <-72>
	184,
	// <-71>
	185,
	// <-70>
	186,
	// <-69>
	187,
	// <-68>
	188,
	// <-67>
	189,
	// <-66>
	190,
	// <-65>
	191,
	// <-64>
	192,
	// <-63>
	193,
	// <-62>
	194,
	// <-61>
	195,
	// <-60>
	196,
	// <-59>
	197,
	// <-58>
	198,
	// <-57>
	199,
	// <-56>
	200,
	// <-55>
	201,
	// <-54>
	202,
	// <-53>
	203,
	// <-52>
	204,
	// <-51>
	205,
	// <-50>
	206,
	// <-49>
	207,
	// <-48>
	208,
	// <-47>
	209,
	// <-46>
	210,
	// <-45>
	211,
	// <-44>
	212,
	// <-43>
	213,
	// <-42>
	214,
	// <-41>
	215,
	// <-40>
	216,
	// <-39>
	217,
	// <-38>
	218,
	// <-37>
	219,
	// <-36>
	220,
	// <-35>
	221,
	// <-34>
	222,
	// <-33>
	223,
	// <-32>
	224,
	// <-31>
	225,
	// <-30>
	226,
	// <-29>
	227,
	// <-28>
	228,
	// <-27>
	229,
	// <-26>
	230,
	// <-25>
	231,
	// <-24>
	232,
	// <-23>
	233,
	// <-22>
	234,
	// <-21>
	235,
	// <-20>
	236,
	// <-19>
	237,
	// <-18>
	238,
	// <-17>
	239,
	// <-16>
	240,
	// <-15>
	241,
	// <-14>
	242,
	// <-13>
	243,
	// <-12>
	244,
	// <-11>
	245,
	// <-10>
	246,
	// <-9>
	247,
	// <-8>
	248,
	// <-7>
	249,
	// <-6>
	250,
	// <-5>
	251,
	// <-4>
	252,
	// <-3>
	253,
	// <-2>
	254,
	// <-1>
	-1,
	// <0>
	0,
	// <1>
	1,
	// <2>
	2,
	// <3>
	3,
	// <4>
	4,
	// <5>
	5,
	// <6>
	6,
	// <7>
	7,
	// <8>
	8,
	// <9>
	9,
	// <10>
	10,
	// <11>
	11,
	// <12>
	12,
	// <13>
	13,
	// <14>
	14,
	// <15>
	15,
	// <16>
	16,
	// <17>
	17,
	// <18>
	18,
	// <19>
	19,
	// <20>
	20,
	// <21>
	21,
	// <22>
	22,
	// <23>
	23,
	// <24>
	24,
	// <25>
	25,
	// <26>
	26,
	// <27>
	27,
	// <28>
	28,
	// <29>
	29,
	// <30>
	30,
	// <31>
	31,
	// ' '
	32,
	// '!'
	33,
	// '"'
	34,
	// '#'
	35,
	// '$'
	36,
	// '%'
	37,
	// '&'
	38,
	// '''
	39,
	// '('
	40,
	// ')'
	41,
	// '*'
	42,
	// '+'
	43,
	// ','
	44,
	// '-'
	45,
	// '.'
	46,
	// '/'
	47,
	// '0'
	48,
	// '1'
	49,
	// '2'
	50,
	// '3'
	51,
	// '4'
	52,
	// '5'
	53,
	// '6'
	54,
	// '7'
	55,
	// '8'
	56,
	// '9'
	57,
	// ':'
	58,
	// ';'
	59,
	// '<'
	60,
	// '='
	61,
	// '>'
	62,
	// '?'
	63,
	// '@'
	64,
	// 'a'
	97,
	// 'b'
	98,
	// 'c'
	99,
	// 'd'
	100,
	// 'e'
	101,
	// 'f'
	102,
	// 'g'
	103,
	// 'h'
	104,
	// 'i'
	105,
	// 'j'
	106,
	// 'k'
	107,
	// 'l'
	108,
	// 'm'
	109,
	// 'n'
	110,
	// 'o'
	111,
	// 'p'
	112,
	// 'q'
	113,
	// 'r'
	114,
	// 's'
	115,
	// 't'
	116,
	// 'u'
	117,
	// 'v'
	118,
	// 'w'
	119,
	// 'x'
	120,
	// 'y'
	121,
	// 'z'
	122,
	// '['
	91,
	// '\'
	92,
	// ']'
	93,
	// '^'
	94,
	// '_'
	95,
	// '`'
	96,
	// 'a'
	97,
	// 'b'
	98,
	// 'c'
	99,
	// 'd'
	100,
	// 'e'
	101,
	// 'f'
	102,
	// 'g'
	103,
	// 'h'
	104,
	// 'i'
	105,
	// 'j'
	106,
	// 'k'
	107,
	// 'l'
	108,
	// 'm'
	109,
	// 'n'
	110,
	// 'o'
	111,
	// 'p'
	112,
	// 'q'
	113,
	// 'r'
	114,
	// 's'
	115,
	// 't'
	116,
	// 'u'
	117,
	// 'v'
	118,
	// 'w'
	119,
	// 'x'
	120,
	// 'y'
	121,
	// 'z'
	122,
	// '{'
	123,
	// '|'
	124,
	// '}'
	125,
	// '~'
	126,
	// <127>
	127,
	// <128>
	128,
	// <129>
	129,
	// <130>
	130,
	// <131>
	131,
	// <132>
	132,
	// <133>
	133,
	// <134>
	134,
	// <135>
	135,
	// <136>
	136,
	// <137>
	137,
	// <138>
	138,
	// <139>
	139,
	// <140>
	140,
	// <141>
	141,
	// <142>
	142,
	// <143>
	143,
	// <144>
	144,
	// <145>
	145,
	// <146>
	146,
	// <147>
	147,
	// <148>
	148,
	// <149>
	149,
	// <150>
	150,
	// <151>
	151,
	// <152>
	152,
	// <153>
	153,
	// <154>
	154,
	// <155>
	155,
	// <156>
	156,
	// <157>
	157,
	// <158>
	158,
	// <159>
	159,
	// <160>
	160,
	// <161>
	161,
	// <162>
	162,
	// <163>
	163,
	// <164>
	164,
	// <165>
	165,
	// <166>
	166,
	// <167>
	167,
	// <168>
	168,
	// <169>
	169,
	// <170>
	170,
	// <171>
	171,
	// <172>
	172,
	// <173>
	173,
	// <174>
	174,
	// <175>
	175,
	// <176>
	176,
	// <177>
	177,
	// <178>
	178,
	// <179>
	179,
	// <180>
	180,
	// <181>
	181,
	// <182>
	182,
	// <183>
	183,
	// <184>
	184,
	// <185>
	185,
	// <186>
	186,
	// <187>
	187,
	// <188>
	188,
	// <189>
	189,
	// <190>
	190,
	// <191>
	191,
	// <192>
	192,
	// <193>
	193,
	// <194>
	194,
	// <195>
	195,
	// <196>
	196,
	// <197>
	197,
	// <198>
	198,
	// <199>
	199,
	// <200>
	200,
	// <201>
	201,
	// <202>
	202,
	// <203>
	203,
	// <204>
	204,
	// <205>
	205,
	// <206>
	206,
	// <207>
	207,
	// <208>
	208,
	// <209>
	209,
	// <210>
	210,
	// <211>
	211,
	// <212>
	212,
	// <213>
	213,
	// <214>
	214,
	// <215>
	215,
	// <216>
	216,
	// <217>
	217,
	// <218>
	218,
	// <219>
	219,
	// <220>
	220,
	// <221>
	221,
	// <222>
	222,
	// <223>
	223,
	// <224>
	224,
	// <225>
	225,
	// <226>
	226,
	// <227>
	227,
	// <228>
	228,
	// <229>
	229,
	// <230>
	230,
	// <231>
	231,
	// <232>
	232,
	// <233>
	233,
	// <234>
	234,
	// <235>
	235,
	// <236>
	236,
	// <237>
	237,
	// <238>
	238,
	// <239>
	239,
	// <240>
	240,
	// <241>
	241,
	// <242>
	242,
	// <243>
	243,
	// <244>
	244,
	// <245>
	245,
	// <246>
	246,
	// <247>
	247,
	// <248>
	248,
	// <249>
	249,
	// <250>
	250,
	// <251>
	251,
	// <252>
	252,
	// <253>
	253,
	// <254>
	254,
	// <255>
	255
};

#pragma endregion

namespace {
	const unsigned short* CTYPE_B_PTR = CTYPE_B_ARRAY + 128;
	const int32_t* CTYPE_TOLOWER_PTR = CTYPE_TOLOWER_ARRAY + 128;
	const int32_t* CTYPE_TOUPPER_PTR = CTYPE_TOUPPER_ARRAY + 128;
}

extern "C" EXPORT size_t __ctype_get_mb_cur_max() {
	return 4;
}

EXPORT const unsigned short** __ctype_b_loc() {
	return &CTYPE_B_PTR;
}

EXPORT const int32_t** __ctype_tolower_loc() {
	return &CTYPE_TOLOWER_PTR;
}

EXPORT const int32_t** __ctype_toupper_loc() {
	return &CTYPE_TOUPPER_PTR;
}
