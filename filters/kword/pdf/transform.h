/*
 * Copyright (c) 2003 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <qpair.h>
#include "CharTypes.h"

namespace PDFImport
{
    // check for special char (numerical values should be < 256)
    enum CharType {
        Unknown = 0,
        Punctuation = 1,
        SymbolChar = 2,
        Digit = 3,
        Letter = 4,

        Hyphen = 5,        // symbol that can be an hyphen or a bullet
        Bullet = 6,        // symbol that can be a bullet
        SuperScript = 7,
        SpecialSymbol = 8, // symbol that appears in Times font (0x22??)
        Ligature = 13,

        // latex
        Punctuation_Accent = 9, // punctuation that can be an accent
        Accent = 10,            // other accent
        Letter_CanHaveAccent = 11, // letter that can have an accent

        LatexSpecial = 12 // special case of needed Symbol->Times mapping
                          // only '\' (0x005C)
    };

    inline bool isPunctuation(CharType type) {
        return ( type==Punctuation || type==Hyphen
                 || type==Punctuation_Accent );
    }
    inline bool isSymbol(CharType type) {
        return ( type==SymbolChar || type==Hyphen || type==Bullet
                 || type==SpecialSymbol || type==Accent
                 || type==LatexSpecial );
    }
    inline bool isLetter(CharType type) {
        return ( type==Letter || type==Letter_CanHaveAccent );
    }
    inline bool isAlphaNumeric(CharType type) {
        return ( type==Digit || isLetter(type) );
    }
    inline bool isAccent(CharType type) {
        return ( type==Punctuation_Accent || type==Accent );
    }

    CharType type(Unicode);

    enum { MaxLigatureLength = 3 };
    uint checkLigature(Unicode, Unicode res[MaxLigatureLength]);

    CharType checkSpecial(Unicode, Unicode &res);

    // detect some letter/accent combinations
    // (latex layouts them separately)
    Unicode checkCombi(Unicode letter, Unicode accent);
}

#endif
