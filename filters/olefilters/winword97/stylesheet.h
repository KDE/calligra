/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <wtrobin@carinthia.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <myfile.h>
#include <misc.h>
#include <fib.h>
#include <kdebug.h>

struct STYLE {
    bool paragStyle;              // is it a paragraph style?
    unsigned long fcPAPX;         // position of the PAPX grpprl in the table stream if there is one
    unsigned short lenPAPX;       // length of the grpprl
    unsigned long fcCHPX;         // CHPX position
    unsigned short lenCHPX;       // length
};

struct STD {                      // used in QMap (for efficiency)
    unsigned short istd;
    unsigned short istdBase;
    QString name;
    STYLE style;
};

class StyleSheet {

public:
    StyleSheet(const myFile &t, const FIB * const f);
    ~StyleSheet();

    const QValueList<STYLE> chain(const unsigned short &sti);    // returns a list with all the PAPX/CHPX/... which
                                                                 // have to be applied (one after the other!)
    const QString styleName(const unsigned short &sti);          // returns the name of a specific style

private:
    StyleSheet(const StyleSheet &);
    const StyleSheet &operator=(const StyleSheet &);

    const bool chain_rec(const unsigned short &istd);            // recursively walks back to the NULL-Style
    
    myFile table;
    const FIB * const fib;
    QMap<unsigned long, STD> styleMap;
    QValueList<STYLE> _chain;                                    // the list with all the styles
    unsigned short lastSti;                                      // don't calculate the chain again if the
                                                                 // list is still correct!
    unsigned short stdOffset;                                    // start of the STD array in the table stream
    unsigned short cstd;                                         // count of styles stored
    unsigned short stdBaseLength;                                // length of the "static" STD part
};
#endif // STYLESHEET_H
