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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <qpair.h>
#include "CharTypes.h"

namespace PDFImport
{
    // check for special char
    enum CharType {
        Unknown = 0,
        Misc = 1, AlphaNumeric = 4, MiscSymbol = 5,

        Accent = 6,         // can serve as accent (latex)
        Hyphen = 7,         // can be an hyphen (can also be a bullet)
        SuperScript = 8,
        LatexSpecial = 9,   // special case of needed Symbol->Times mapping
                            // only '\' (0x005C)
        CanHaveAccent = 2,  // letter that can have an accent (latex)

        SpecialSymbol = 10, // char exits but do Times->Symbol mapping
        Bullet = 11         // can be a list bullet
    };
    bool checkLigature(Unicode, Unicode &res1, Unicode &res2);
    CharType checkSpecial(Unicode, Unicode &res);

    // detect some letter/accent combinations
    // (latex layouts them separately)
    Unicode checkCombi(Unicode letter, Unicode accent);
};

#endif
