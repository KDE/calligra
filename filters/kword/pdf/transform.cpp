#include "transform.h"

#include <kdebug.h>


namespace PDFImport
{

SpecialType checkSpecial(Unicode u, Unicode &res1, Unicode &res2)
{
    if ( u>=0x0020 && u<=0x003F ) return Normal; // ponctuation
    if ( u>=0x0041 && u<=0x005A ) return Normal; // 'A' < u < 'Z'
    if ( u>=0x0061 && u<=0x007A ) return Normal; // 'a' < u < 'z'

    kdDebug(30516) << "check special " << QString(QChar(u))
                   << " (" << u << ")" << endl;

    struct SpecialData {
        Unicode     u;
        SpecialType type;
        Unicode     r1, r2;
    };
    static const SpecialData SPECIAL_DATA[] = {
        { 0xFB01, Ligature,    0x0066, 0x0069 }, // fi
        { 0xFB02, Ligature,    0x0066, 0x006C }, // fl
//        { 0x, Ligature, 0x0066, 0x0066 }, // ff
        { 0x2022, Bullet,      0x00B7, 0x0000 }, // full circle
        { 0x00B9, SuperScript, 0x0031, 0x0000 }, // 1
        { 0x00B2, SuperScript, 0x0032, 0x0000 }, // 2
        { 0x00B3, SuperScript, 0x0033, 0x0000 }, // 3
        { 0x005C, LatexSymbol, 0x0000, 0x0000 }, // '\'
        { 0x0000, Normal,      0x0000, 0x0000 }
    };

    uint i = 0;
    while ( SPECIAL_DATA[i].u!=0 ) {
        if ( SPECIAL_DATA[i].u==u ) {
            res1 = SPECIAL_DATA[i].r1;
            res2 = SPECIAL_DATA[i].r2;
            return SPECIAL_DATA[i].type;
        }
        i++;
    }

    return Normal;
}

bool checkAccent(Unicode letter, Unicode accent, Unicode &res)
{
    enum Accent { Grave = 0, Acute, Circumflex, Tilde, Diaeresis, Degree,
                  Macron, LowLine, Dot, Comma, DQuote, Cedilla, Nb_Accent };
    // #### FIXME 0x00B7 == Dot ?
    static const Unicode ACCENTS[Nb_Accent] = {
        0x0060, // `
        0x00B4, // '
        0x005E, // ^
        0x007E, // ~
        0x00A8, // ¨
        0x00B0, // °
        0x00AF, // macron
        0x005F, // _
        0x002E, // .
        0x002C, // ,
        0x0022, // "
        0x00B8  // cedilla
    };

    struct AccentData {
        Accent  accent;
        Unicode upper, lower;
    };
    static const AccentData A_DATA[] = {
        { Grave,      0x00C0, 0x00E0 },
        { Acute,      0x00C1, 0x00E1 },
        { Circumflex, 0x00C2, 0x00E2 },
        { Tilde,      0x00C3, 0x00E3 },
        { Diaeresis,  0x00C4, 0x00E4 },
        { Degree,     0x00C5, 0x00E5 },
        { Macron,     0x0100, 0x0101 },
        { Dot,        0x0226, 0x0227 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData C_DATA[] = {
        { Acute,      0x0106, 0x0107 },
        { Circumflex, 0x0108, 0x0109 },
        { Dot,        0x010A, 0x010B },
        { Cedilla,    0x00C7, 0x00E7 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData E_DATA[] = {
        { Grave,      0x00C8, 0x00E8 },
        { Acute,      0x00C9, 0x00E9 },
        { Circumflex, 0x00CA, 0x00EA },
        { Diaeresis,  0x00CB, 0x00EB },
        { Macron,     0x0112, 0x0113 },
        { Dot,        0x0116, 0x0117 },
        { Cedilla,    0x0228, 0x0229 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData G_DATA[] = {
        { Acute,      0x01F4, 0x01F5 },
        { Circumflex, 0x011C, 0x011D },
        { Comma,      0x0122, 0x0000 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData H_DATA[] = {
        { Circumflex, 0x0124, 0x0125 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData I_DATA[] = {
        { Grave,      0x00CC, 0x0000 },
        { Acute,      0x00CD, 0x0000 },
        { Circumflex, 0x00CE, 0x0000 },
        { Tilde,      0x0128, 0x0000 },
        { Diaeresis,  0x00CF, 0x0000 },
        { Macron,     0x012A, 0x0000 },
        { Dot,        0x0130, 0x0000 },
        { Cedilla,    0x012E, 0x0000 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData J_DATA[] = {
        { Circumflex, 0x0134, 0x0135 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData K_DATA[] = {
        { Comma,      0x0136, 0x0137 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData L_DATA[] = {
        { Acute,      0x0139, 0x013A },
        { Comma,      0x013B, 0x013C },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData N_DATA[] = {
        { Acute,      0x0143, 0x0144 },
        { Comma,      0x0145, 0x0146 },
        { Cedilla,    0x00D1, 0x00F1 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData O_DATA[] = {
        { Grave,      0x00D2, 0x00F2 },
        { Acute,      0x00D3, 0x00F3 },
        { Circumflex, 0x00D4, 0x00F4 },
        { Tilde,      0x00D5, 0x00F5 },
        { Diaeresis,  0x00D6, 0x00F6 },
        { Macron,     0x014C, 0x014D },
        { DQuote,     0x0150, 0x0151 },
        { Dot,        0x022E, 0x022F },
        { Nb_Accent,  0x0000, 0x0000 },
    };
    static const AccentData R_DATA[] = {
        { Acute,      0x0154, 0x0155 },
        { Comma,      0x0156, 0x0157 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData S_DATA[] = {
        { Acute,      0x015A, 0x015B },
        { Circumflex, 0x015C, 0x015D },
        { Comma,      0x0218, 0x0219 },
        { Cedilla,    0x015E, 0x015F },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData T_DATA[] = {
        { Comma,      0x021A, 0x021B },
        { Cedilla,    0x0162, 0x0163 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData U_DATA[] = {
        { Grave,      0x00D9, 0x00F9 },
        { Acute,      0x00DA, 0x00FA },
        { Circumflex, 0x00DB, 0x00FB },
        { Tilde,      0x0168, 0x0169 },
        { Diaeresis,  0x00DC, 0x00FC },
        { Macron,     0x016A, 0x016B },
        { Dot,        0x016E, 0x016F },
        { DQuote,     0x0170, 0x0171 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData W_DATA[] = {
        { Circumflex, 0x0174, 0x0175 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData Y_DATA[] = {
        { Acute,      0x00DD, 0x00FD },
        { Circumflex, 0x0176, 0x0177 },
        { Diaeresis,  0x0178, 0x00FF },
        { Macron,     0x0232, 0x0233 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData Z_DATA[] = {
        { Acute,      0x0179, 0x017A },
        { Dot,        0x017B, 0x017C },
        { Nb_Accent,  0x0000, 0x0000 }
    };

    static const AccentData *LETTER_DATA[26] = {
        A_DATA, 0, C_DATA, 0, E_DATA, 0, G_DATA, H_DATA, I_DATA, J_DATA,
        K_DATA, L_DATA, 0, N_DATA, O_DATA, 0, 0, R_DATA, S_DATA, T_DATA,
        U_DATA, 0, W_DATA, 0, Y_DATA, Z_DATA
    };

    static const AccentData AE_DATA[] = {
        { Acute,      0x01FC, 0x01FD },
        { Macron,     0x01E2, 0x01E3 },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData NULL_DATA[] = {
        { Acute,      0x01FE, 0x01FF },
        { Nb_Accent,  0x0000, 0x0000 }
    };
    static const AccentData I_LOWER_DATA[] = {
        { Grave,      0x0000, 0x00EC },
        { Acute,      0x0000, 0x00ED },
        { Circumflex, 0x0000, 0x00EE },
        { Tilde,      0x0000, 0x0129 },
        { Diaeresis,  0x0000, 0x00EF },
        { Macron,     0x0000, 0x012B },
        { Dot,        0x0000, 0x0045 },
        { Cedilla,    0x0000, 0x012F },
        { Nb_Accent,  0x0000, 0x0000 }
    };

    struct Data {
        Unicode upper, lower;
        const AccentData *data;
    };
    static const Data DATA[] = {
        { 0x00C6, 0x00E6, AE_DATA      },
        { 0x00D8, 0x00E8, NULL_DATA    },
        { 0x0000, 0x0131, I_LOWER_DATA },
        { 0x0000, 0x0000, 0            }
    };

    // find accent
    uint i = 0;
    for (; i<Nb_Accent; i++)
        if ( ACCENTS[i]==accent ) break;
    if ( i==Nb_Accent ) return false;

//    kdDebug(30516) << "letter=" << QString(QChar(letter)) << " accent="
//                   << (accent & 0x00FF) << " found=" << i << endl;

    // find letter
    const AccentData *data = 0;
    bool upper = true;
    if ( letter>='A' && letter<='Z' )
        data = LETTER_DATA[letter-'A'];
    else if ( letter>='a' && letter<='z' ) {
        data = LETTER_DATA[letter-'a'];
        upper = false;
    } else {
        uint k = 0;
        for (;;) {
            if ( DATA[k].data==0 ) return false;
            if ( letter==DATA[k].upper ) {
                data = DATA[k].data;
                break;
            } else if ( letter==DATA[k].lower ) {
                data = DATA[k].data;
                upper = false;
                break;
            }
            k++;
        }
    }
    if ( data==0 ) return false;

    // find combi
    uint l = 0;
    while ( data[l].accent!=Nb_Accent ) {
        if ( data[l].accent==Accent(i) ) {
            res = (upper ? data[l].upper : data[l].lower);
            return true;
        }
        l++;
    }
    return false;
}

};
