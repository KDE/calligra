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

#ifndef SECTION_H
#define SECTION_H

#include <qarray.h>
#include <qstring.h>
#include <qlist.h>

#include <fib.h>
#include <myfile.h>
#include <paragraph.h>
#include <winworddoc.h>

class WinWordDoc;

class Section {

public:
    Section(const WinWordDoc * const parent, const unsigned char * const mainData,
            const FIB * const fib, const QArray<long> &sectionMarks,
            const QArray<long> &paragMarks, const QArray<long> &cellMarks,
            const QArray<long> &rowMarks);
    ~Section();

    const bool isOk() const { return m_success; }
    const bool convert();
    const QString section() const { return m_section; }

private:
    Section(const Section &);
    const Section &operator=(const Section &);

    QString m_section;
    QList<Paragraph> m_paragList;
    bool m_success;
    const WinWordDoc * const m_parent;
    const unsigned char * const m_mainData;
    const FIB * const m_fib;
    QArray<long> m_sectionMarks, m_paragMarks, m_cellMarks, m_rowMarks;
};
#endif // SECTION_H
