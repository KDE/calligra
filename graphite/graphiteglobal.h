/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

#ifndef GRAPHITE_GLOBAL_H
#define GRAPHITE_GLOBAL_H

#include <kimageeffect.h>

class QColor;

struct Gradient {
    QColor ca;
    QColor cb;
    KImageEffect::GradientType type;
    short xfactor;
    short yfactor;
    short ncols;
};


// This class is used to access some configurable values.
// We also use this class to save the rc file.
// Note: Follows the singleton pattern!
class GraphiteGlobal {

public:
    static GraphiteGlobal *self();

    const int &fuzzyBorder() const { return m_fuzzyBorder; }
    void setFuzzyBorder(const int &fuzzyBorder) { m_fuzzyBorder=fuzzyBorder; }

    // more to come...
    // maybe I'll add a init(...) method which takes a KConfig file
    // and initializes all the "global" vars.

private:
    GraphiteGlobal() : m_fuzzyBorder(2) {}
    GraphiteGlobal(const GraphiteGlobal &rhs);
    GraphiteGlobal &operator=(const GraphiteGlobal &rhs);

    ~GraphiteGlobal() {}

    static GraphiteGlobal *m_self;
    int m_fuzzyBorder;
};
#endif // GRAPHITE_GLOBAL_H
