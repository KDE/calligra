/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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

#include <section.h>

Section::Section(const WinWordDoc * const parent, const unsigned char * const mainData,
                 const FIB * const fib, const QArray<int> &paragMarks,
                 const QArray<int> &cellMarks, const QArray<int> &rowMarks) :
                 m_parent(parent), m_mainData(mainData), m_fib(fib),
                 m_paragMarks(paragMarks), m_cellMarks(cellMarks),
                 m_rowMarks(rowMarks) {

    m_section=QString::null;
    m_success=true;
}

Section::~Section() {
}
