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
    enum SpecialType { Normal, Ligature, Bullet, SuperScript, LatexSymbol };
    SpecialType checkSpecial(Unicode, Unicode &res1, Unicode &res2);

    // detect some letter/accent combinations
    // (for e.g. latex layouts them separately)
    bool checkAccent(Unicode letter, Unicode accent, Unicode &res);
};

#endif
