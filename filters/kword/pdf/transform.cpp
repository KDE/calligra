#include "transform.h"

#include <kdebug.h>


namespace PDFImport
{

// classification of some Unicode characters
static const uint TABLE_SIZE = 0x0100;
static const uint NB_TABLES = 5;
static const uint OFFSET[NB_TABLES] = {
    0x00, 0x01, 0x20, 0x21, 0x22
};
static const char TABLE[NB_TABLES][TABLE_SIZE] = {
#define U Unknown
#define P Punctuation
#define S SymbolChar
#define D Digit
#define L Letter

#define H Hyphen
#define B Bullet
#define I SuperScript
#define Y SpecialSymbol
#define G Ligature

#define A Accent
#define E Punctuation_Accent
#define C Letter_CanHaveAccent

#define X LatexSpecial
//-------------------------------------------------------------------
{ // 0x0000 to 0x00FF
    U,U,U,U,U,U,U,U,U,P,P,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
//
    P,P,E,S,S,S,S,P,P,P,S,S,E,H,E,S, D,D,D,D,D,D,D,D,D,D,P,P,S,S,S,P,
//    ! " # $ % & ' ( ) * + , - . /  0 1 2 3 4 5 6 7 8 9 : ; < = > ?
    S,C,L,C,L,C,L,C,C,C,C,C,C,L,C,C, L,L,C,C,C,C,L,C,L,C,C,S,X,S,A,A,
//  @ A B C D E F G H I J K L M N O  P Q R S T U V W X Y Z [ \ ] ^ _
    E,C,L,C,L,C,L,C,C,C,C,C,C,L,C,C, L,L,C,C,C,C,L,C,L,C,C,S,S,S,A,U,
//  ` a b c d e f g h i j k l m n o  p q r s t u v w x y z { | } ~
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
//
    U,U,S,S,S,S,S,S,A,S,U,P,U,H,S,A, A,S,I,I,E,U,U,S,A,I,U,P,S,S,S,P,
//      ¢ £ ¤     § ¨     « ¬        ° ± ² ³   µ ¶ ·   ¹   » ¼     ¿
    L,L,L,L,L,L,C,L,L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,S,C,L,L,L,L,L,L,L,
//      Â   Ä           Ê Ë     Î Ï          Ô   Ö         Û Ü     ß
    L,L,L,L,L,L,C,L,L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,S,C,L,L,L,L,L,L,L
//  à   â   ä   æ ç è é ê ë     î ï  ð       ô   ö   ø     û ü   þ ÿ
},
//-------------------------------------------------------------------
{ // 0x0100 to 0x01FF
    L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,
    L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L, L,C,L,L,L,L,L,L,L,L,L,L,L,L,L,L,
    L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,
    L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L, L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,S,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    L,L,U,U,U,U,U,L,L,S,U,U,U,U,L,L, L,U,U,U,U,L,L,U,U,U,U,U,U,U,U,U,
    U,U,S,P,U,U,U,U,U,U,U,U,U,L,L,L, L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,L,
    L,L,L,L,L,L,L,L,L,L,L,L,L,L,U,U, L,U,U,U,L,L,U,U,L,L,L,L,L,L,L,L
},
//-------------------------------------------------------------------
{ // 0x2000 to 0x20FF
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,P,P,P,P,P,P,P,P,
    S,S,B,U,U,U,P,U,U,U,U,U,U,U,U,U, S,U,P,P,U,U,U,U,U,Y,Y,U,U,U,U,U,
    U,U,U,U,Y,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,S,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U
},
//-------------------------------------------------------------------
{ // 0x2100 to 0x21FF
    U,U,U,G,U,U,U,U,U,G,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,S,U,U,U,U,U,U,U,Y,Y,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, Y,Y,Y,Y,Y,U,U,U,U,U,Y,Y,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,Y,U, U,U,U,U,U,Y,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,Y,Y,Y, Y,Y,Y,Y,Y,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U
},
//-------------------------------------------------------------------
{ // 0x2200 to 0x22FF
    Y,U,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,Y,U,Y, U,Y,Y,U,U,Y,U,Y,U,Y,Y,U,U,Y,Y,U,
    Y,U,U,U,U,U,U,Y,Y,Y,Y,Y,U,U,U,U, U,U,U,U,Y,Y,Y,U,U,U,U,U,Y,U,U,U,
    U,Y,U,U,U,Y,U,Y,Y,Y,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,Y,Y,U,U,U,U,U,
    Y,Y,Y,U,Y,Y,U,U,U,U,U,U,U,U,Y,Y, Y,Y,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,Y,Y,Y,Y,Y,Y,Y,Y,U,U,U,U,U,U, U,U,U,U,U,Y,U,Y,U,U,U,U,U,U,U,U,
    U,U,U,U,Y,Y,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,Y,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,
    U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U, U,U,U,U,U,U,U,U,U,U,U,U,U,U,U,U
}
//-------------------------------------------------------------------
#undef U
#undef P
#undef S
#undef D
#undef L

#undef H
#undef B
#undef I
#undef Y

#undef A
#undef E
#undef C
#undef X
};

CharType type(Unicode u)
{
    uint offset = u / TABLE_SIZE;
    uint index = u % TABLE_SIZE;
    for (uint i=0; i<NB_TABLES; i++) {
        if ( offset==OFFSET[i] ) return (CharType)TABLE[i][index];
        if ( offset<OFFSET[i] ) break;
    }
    if ( u>=0xFB00 && u<=0xFB06 ) return Ligature;
    return Unknown;
}

//-----------------------------------------------------------------------------
static const Unicode LIGATURE_DATA[][MaxLigatureLength+1] = {
    { 0xFB00, 0x0066, 0x0066, 0x0000 }, // ff
    { 0xFB01, 0x0066, 0x0069, 0x0000 }, // fi
    { 0xFB02, 0x0066, 0x006C, 0x0000 }, // fl
    { 0xFB03, 0x0066, 0x0066, 0x0069 }, // ffi
    { 0xFB04, 0x0066, 0x0066, 0x006c }, // ffl
    // 0xFB05, 0xFB06 ??
    // latex does not generate a ligature for ffe...
    { 0x0000, 0x0000, 0x0000, 0x0000 }
};

uint checkLigature(Unicode u, Unicode res[MaxLigatureLength])
{
    if ( type(u)==Unknown ) kdDebug(30516) << "unknown char " << u << endl;
    if ( type(u)!=Ligature ) {
        res[0] = u;
        return 1;
    }

    uint i = 0;
    while ( LIGATURE_DATA[i][0]!=0 ) {
        if ( LIGATURE_DATA[i][0]==u ) {
            uint k = 0;
            for (; k<MaxLigatureLength; k++) {
                if ( LIGATURE_DATA[i][k+1]==0 ) break;
                res[k] = LIGATURE_DATA[i][k+1];
            }
            return k;
        }
        i++;
    }
    kdDebug(30516) << "undefined ligature !! " << u <<endl;
    res[0] = u;
    return 1;
}


//-----------------------------------------------------------------------------
static const Unicode SUPER_DATA[][2] = {
    { 0x00B9, 0x0031 }, // 1
    { 0x00B2, 0x0032 }, // 2
    { 0x00B3, 0x0033 }, // 3
    { 0x0000, 0x0000 }
};

static const Unicode BULLET_DATA[][2] = {
    { 0x2022, 0x00B7 }, // full circle
    { 0x0000, 0x0000 }
};

//static const SpecialData SPECIAL_DATA[] = {
// #### FIXME : add missing characters (table 0x20 to 0x22)
//    { 0x2190, SpecialSymbol, 0x00AC }, // <-
//    { 0x2192, SpecialSymbol, 0x00AE }, // ->
//    { 0x2212, SpecialSymbol, 0x002D }, // -
//    { 0x2217, SpecialSymbol, 0x002A }, // *
//    { 0x221A, SpecialSymbol, 0x00D6 }, // squareroot
//    { 0x2229, SpecialSymbol, 0x00C7 }, // intersection
//    { 0x222A, SpecialSymbol, 0x00C8 }, // union
//    { 0x0000, Unknown,       0x0000 }
//};

CharType checkSpecial(Unicode u, Unicode &res)
{
    CharType t = type(u);

    // special mapping
    switch (t) {
    case Unknown:
        kdDebug(30516) << "unknown special " << QString(QChar(u))
                       << " (" << u << ")" << endl;
        break;
    case SuperScript: {
        uint i = 0;
        for (;;) {
            if ( SUPER_DATA[i][0]==0 ) {
                kdDebug(30516) << "undefined superscript !!" << endl;
                break;
            }
            if ( SUPER_DATA[i][0]==u ) {
                res = SUPER_DATA[i][1];
                break;
            }
            i++;
        }
        break;
    }
    case Bullet:{
        uint i = 0;
        for (;;) {
            if ( BULLET_DATA[i][0]==0 ) {
                kdDebug(30516) << "undefined bullet !!" << endl;
                break;
            }
            if ( BULLET_DATA[i][0]==u ) {
                res = BULLET_DATA[i][1];
                break;
            }
            i++;
        }
        break;
    }

    default:
        break;
    }

    return t;
}


//-----------------------------------------------------------------------------
enum AccentType {
    NoAccent,
    Grave, Acute, Circumflex, Tilde, Diaeresis, Degree,
    Macron, LowLine, Dot, Comma, DQuote, Cedilla
};
struct AccentData {
    Unicode    u;
    AccentType type;
};
static const AccentData ACCENT_DATA[] = {
    { 0x0060, Grave      }, // `
    { 0x00B4, Acute      }, // '
    { 0x005E, Circumflex }, // ^
    { 0x007E, Tilde      }, // ~
    { 0x00A8, Diaeresis  }, // ¨
    { 0x00B0, Degree     }, // °
    { 0x00AF, Macron     }, // macron
    { 0x005F, LowLine    }, // _
    { 0x002E, Dot        }, // .
//    { 0x00B7, Dot        }, ????
    { 0x002C, Comma      }, // ,
    { 0x0022, DQuote     }, // "
    { 0x00B8, Cedilla    },  // cedilla
    { 0x0000, NoAccent   }
};

struct CombiData {
    AccentType type;
    Unicode    upper, lower;
};
static const CombiData A_DATA[] = {
    { Grave,      0x00C0, 0x00E0 },
    { Acute,      0x00C1, 0x00E1 },
    { Circumflex, 0x00C2, 0x00E2 },
    { Tilde,      0x00C3, 0x00E3 },
    { Diaeresis,  0x00C4, 0x00E4 },
    { Degree,     0x00C5, 0x00E5 },
    { Macron,     0x0100, 0x0101 },
    { Dot,        0x0226, 0x0227 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData C_DATA[] = {
    { Acute,      0x0106, 0x0107 },
    { Circumflex, 0x0108, 0x0109 },
    { Dot,        0x010A, 0x010B },
    { Cedilla,    0x00C7, 0x00E7 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData E_DATA[] = {
    { Grave,      0x00C8, 0x00E8 },
    { Acute,      0x00C9, 0x00E9 },
    { Circumflex, 0x00CA, 0x00EA },
    { Diaeresis,  0x00CB, 0x00EB },
    { Macron,     0x0112, 0x0113 },
    { Dot,        0x0116, 0x0117 },
    { Cedilla,    0x0228, 0x0229 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData G_DATA[] = {
    { Acute,      0x01F4, 0x01F5 },
    { Circumflex, 0x011C, 0x011D },
    { Comma,      0x0122, 0x0000 },
    { NoAccent,   0x0000, 0x0000 }
    };
static const CombiData H_DATA[] = {
    { Circumflex, 0x0124, 0x0125 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData I_DATA[] = {
    { Grave,      0x00CC, 0x0000 },
    { Acute,      0x00CD, 0x0000 },
    { Circumflex, 0x00CE, 0x0000 },
    { Tilde,      0x0128, 0x0000 },
    { Diaeresis,  0x00CF, 0x0000 },
    { Macron,     0x012A, 0x0000 },
    { Dot,        0x0130, 0x0000 },
    { Cedilla,    0x012E, 0x0000 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData J_DATA[] = {
    { Circumflex, 0x0134, 0x0135 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData K_DATA[] = {
    { Comma,      0x0136, 0x0137 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData L_DATA[] = {
    { Acute,      0x0139, 0x013A },
    { Comma,      0x013B, 0x013C },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData N_DATA[] = {
    { Acute,      0x0143, 0x0144 },
    { Comma,      0x0145, 0x0146 },
    { Cedilla,    0x00D1, 0x00F1 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData O_DATA[] = {
    { Grave,      0x00D2, 0x00F2 },
    { Acute,      0x00D3, 0x00F3 },
    { Circumflex, 0x00D4, 0x00F4 },
    { Tilde,      0x00D5, 0x00F5 },
    { Diaeresis,  0x00D6, 0x00F6 },
    { Macron,     0x014C, 0x014D },
    { DQuote,     0x0150, 0x0151 },
    { Dot,        0x022E, 0x022F },
    { NoAccent,   0x0000, 0x0000 },
};
static const CombiData R_DATA[] = {
    { Acute,      0x0154, 0x0155 },
    { Comma,      0x0156, 0x0157 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData S_DATA[] = {
    { Acute,      0x015A, 0x015B },
    { Circumflex, 0x015C, 0x015D },
    { Comma,      0x0218, 0x0219 },
    { Cedilla,    0x015E, 0x015F },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData T_DATA[] = {
    { Comma,      0x021A, 0x021B },
    { Cedilla,    0x0162, 0x0163 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData U_DATA[] = {
    { Grave,      0x00D9, 0x00F9 },
    { Acute,      0x00DA, 0x00FA },
    { Circumflex, 0x00DB, 0x00FB },
    { Tilde,      0x0168, 0x0169 },
    { Diaeresis,  0x00DC, 0x00FC },
    { Macron,     0x016A, 0x016B },
    { Dot,        0x016E, 0x016F },
    { DQuote,     0x0170, 0x0171 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData W_DATA[] = {
    { Circumflex, 0x0174, 0x0175 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData Y_DATA[] = {
    { Acute,      0x00DD, 0x00FD },
    { Circumflex, 0x0176, 0x0177 },
    { Diaeresis,  0x0178, 0x00FF },
    { Macron,     0x0232, 0x0233 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData Z_DATA[] = {
    { Acute,      0x0179, 0x017A },
    { Dot,        0x017B, 0x017C },
    { NoAccent,   0x0000, 0x0000 }
};

static const CombiData *LETTER_DATA[26] = {
    A_DATA, 0, C_DATA, 0, E_DATA, 0, G_DATA, H_DATA, I_DATA, J_DATA,
    K_DATA, L_DATA, 0, N_DATA, O_DATA, 0, 0, R_DATA, S_DATA, T_DATA,
    U_DATA, 0, W_DATA, 0, Y_DATA, Z_DATA
};

static const CombiData AE_DATA[] = {
    { Acute,      0x01FC, 0x01FD },
    { Macron,     0x01E2, 0x01E3 },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData NULL_DATA[] = {
    { Acute,      0x01FE, 0x01FF },
    { NoAccent,   0x0000, 0x0000 }
};
static const CombiData I_LOWER_DATA[] = {
    { Grave,      0x0000, 0x00EC },
    { Acute,      0x0000, 0x00ED },
    { Circumflex, 0x0000, 0x00EE },
    { Tilde,      0x0000, 0x0129 },
    { Diaeresis,  0x0000, 0x00EF },
    { Macron,     0x0000, 0x012B },
    { Dot,        0x0000, 0x0045 },
    { Cedilla,    0x0000, 0x012F },
    { NoAccent,   0x0000, 0x0000 }
};

struct SpecialCombiData {
    Unicode upper, lower;
    const CombiData *data;
};
static const SpecialCombiData SPECIAL_COMBI_DATA[] = {
    { 0x00C6, 0x00E6, AE_DATA      },
    { 0x00D8, 0x00E8, NULL_DATA    },
    { 0x0000, 0x0131, I_LOWER_DATA },
    { 0x0000, 0x0000, 0            }
};

Unicode checkCombi(Unicode letter, Unicode accent)
{
    // quick check
    if ( !isAccent( type(accent) ) ) return 0;
    if ( type(letter)!=Letter_CanHaveAccent ) return 0;

    // find accent
    uint i = 0;
    for (;;) {
        if ( ACCENT_DATA[i].u==0 ) return 0;
        if ( ACCENT_DATA[i].u==accent ) break;
        i++;
    }

    // find letter
    const CombiData *data = 0;
    bool upper = true;
    if ( letter>='A' && letter<='Z' )
        data = LETTER_DATA[letter-'A'];
    else if ( letter>='a' && letter<='z' ) {
        data = LETTER_DATA[letter-'a'];
        upper = false;
    } else {
        uint k = 0;
        for (;;) {
            if ( SPECIAL_COMBI_DATA[k].data==0 ) return 0;
            if ( letter==SPECIAL_COMBI_DATA[k].upper ) {
                data = SPECIAL_COMBI_DATA[k].data;
                break;
            } else if ( letter==SPECIAL_COMBI_DATA[k].lower ) {
                data = SPECIAL_COMBI_DATA[k].data;
                upper = false;
                break;
            }
            k++;
        }
    }
    if ( data==0 ) return 0;

    // find combi
    uint l = 0;
    while ( data[l].type!=NoAccent ) {
        if ( data[l].type==ACCENT_DATA[i].type )
            return (upper ? data[l].upper : data[l].lower);
        l++;
    }
    return 0;
}

}
