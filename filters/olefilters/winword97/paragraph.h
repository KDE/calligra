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

#ifndef PARAGRAPH_H
#define PARAGRAPH_H

#include <qobject.h>
#include <qarray.h>
#include <qstring.h>

#include <fib.h>

class Section;

class Paragraph : public QObject {

    Q_OBJECT

public:
    Paragraph(const Section * const parent, const unsigned char * const mainData,
              const FIB * const fib, const QArray<long> &rowMarks,
              const QArray<long> &cellMarks);
    ~Paragraph();

    const bool isOk() const { return m_success; }
    const bool convert();
    const QString paragraph() const { return m_paragraph; }

private:
    Paragraph(const Paragraph &);
    const Paragraph &operator=(const Paragraph &);

    QString m_paragraph;
    bool m_success;
    const Section * const m_parent;
    const unsigned char * const m_mainData;
    const FIB * const m_fib;
    QArray<long> m_rowMarks, m_cellMarks;
};
#endif // PARAGRAPH_H
