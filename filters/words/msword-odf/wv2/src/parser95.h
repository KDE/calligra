/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifndef PARSER95_H
#define PARSER95_H

#include "parser9x.h"
#include "wv2_export.h"

namespace wvWare
{

/**
 * This class is the main parser class for Word95 documents.
 */
class Parser95 : public Parser9x
{
public:
    Parser95( OLEStorage* storage, OLEStreamReader* wordDocument );
    virtual ~Parser95();

private:
    // don't copy or assing us
    Parser95( const Parser95& rhs );
    Parser95& operator=( const Parser95& rhs );

    void init();
};

} // namespace wvWare

#endif // PARSER95_H
