#include "transform.h"

#include <kdebug.h>


namespace PDFImport
{

// first 256 Unicode characters
// numbers correspond to CharType of each character
static const char TABLE_0[256] = {
    0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//
    1,1,6,5,5,5,5,1,1,1,5,5,6,7,6,5, 4,4,4,4,4,4,4,4,4,4,1,1,5,5,5,1,
//    ! " # $ % & ' ( ) * + , - . /  0 1 2 3 4 5 6 7 8 9 : ; < = > ?
    5,2,4,2,4,2,4,2,2,2,2,2,2,4,2,2, 4,4,2,2,2,2,4,2,4,2,2,5,9,5,6,6,
//  @ A B C D E F G H I J K L M N O  P Q R S T U V W X Y Z [ \ ] ^ _
    6,2,4,2,4,2,4,2,2,2,2,2,2,4,2,2, 4,4,2,2,2,2,4,2,4,2,2,5,5,5,6,0,
//  ` a b c d e f g h i j k l m n o  p q r s t u v w x y z { | } ~
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//
    0,0,5,5,5,5,5,5,6,5,0,1,0,7,5,6, 6,5,8,8,6,0,0,5,6,8,0,1,5,5,5,1,
//      ¢ £ ¤     § ¨     « ¬        ° ± ² ³   µ ¶ ·   ¹   » ¼     ¿
    4,4,4,4,4,4,2,4,4,4,4,4,4,4,4,4, 4,4,4,4,4,4,4,5,2,4,4,4,4,4,4,4,
//      Â   Ä           Ê Ë     Î Ï          Ô   Ö                 ß
    4,4,4,4,4,4,2,4,4,4,4,4,4,4,4,4, 4,4,4,4,4,4,4,5,2,4,4,4,4,4,4,4
//  à   â   ä   æ ç è é ê ë     î ï  ð       ô   ö   ø           þ
};


//-----------------------------------------------------------------------------
static const Unicode LIGATURE_DATA[][3] = {
    { 0xFB01, 0x0066, 0x0069 }, // fi
    { 0xFB02, 0x0066, 0x006C }, // fl
    // #### ff ??
    // 0xFB03, 0xFB04, 0xFB05, 0xFB06 ??
    { 0x0000, 0x0000, 0x0000 }
};

bool checkLigature(Unicode u, Unicode &res1, Unicode &res2)
{
    // quick check
    if ( u<0xFB01 || u>0xFB06 ) return false;

    uint i = 0;
    while ( LIGATURE_DATA[i][0]!=0 ) {
        if ( LIGATURE_DATA[i][0]==u ) {
            res1 = LIGATURE_DATA[i][1];
            res2 = LIGATURE_DATA[i][2];
            return true;
        }
        i++;
    }
    return false;
}


//-----------------------------------------------------------------------------
static const Unicode SUPER_DATA[][2] = {
    { 0x00B9, 0x0031 }, // 1
    { 0x00B2, 0x0032 }, // 2
    { 0x00B3, 0x0033 }, // 3
    { 0x0000, 0x0000 }
};

struct SpecialData {
    Unicode     u;
    CharType    type;
    Unicode     res;
};
// also known :
// 0x0131 is 'i' without dot
static const SpecialData SPECIAL_DATA[] = {
    { 0x2022, Bullet,        0x00B7 }, // full circle

    // #### FIXME : add missing characters (table 0x20 to 0x22)
    { 0x2190, SpecialSymbol, 0x00AC }, // <-
    { 0x2192, SpecialSymbol, 0x00AE }, // ->
    { 0x2212, SpecialSymbol, 0x002D }, // -
    { 0x2217, SpecialSymbol, 0x002A }, // *
    { 0x221A, SpecialSymbol, 0x00D6 }, // squareroot
    { 0x2229, SpecialSymbol, 0x00C7 }, // intersection
    { 0x222A, SpecialSymbol, 0x00C8 }, // union

    { 0x0000, Unknown,       0x0000 }
};

CharType checkSpecial(Unicode u, Unicode &res)
{
    // find character type
    CharType type;
    res = u;
    if ( u<=0x00FF ) type = (CharType)TABLE_0[u];
    else {
        uint i = 0;
        while ( SPECIAL_DATA[i].u!=0 ) {
            if ( SPECIAL_DATA[i].u==u ) {
                type = SPECIAL_DATA[i].type;
                res = SPECIAL_DATA[i].res;
                break;
            }
            i++;
        }
        if ( SPECIAL_DATA[i].u==0 ) type = Unknown;
    }

    // special mapping
    switch (type) {
    case Unknown:
        kdDebug(30516) << "unknown special " << QString(QChar(u))
                       << " (" << u << ")" << endl;
        break;
    case SuperScript: {
        uint i = 0;
        for (;;) {
            Q_ASSERT( SUPER_DATA[i][0] );
            if ( SUPER_DATA[i][0]==u ) {
                res = SUPER_DATA[i][1];
                break;
            }
            i++;
        }
        break;
    }
    default:
        break;
    }

    return type;
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
    if ( accent>0x00FF ) return 0;
    if ( (CharType)TABLE_0[accent]!=Accent ) return 0;
    if ( letter<=0x00FF ) {
        if ( (CharType)TABLE_0[letter]!=CanHaveAccent ) return 0;
    } else if ( letter!=0x0131 ) return 0; // I without dot

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

};
