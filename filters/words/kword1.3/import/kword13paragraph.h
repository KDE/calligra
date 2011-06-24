/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef _FILTER_WORDS_1_3__WORDSPARAGRAPH_H
#define _FILTER_WORDS_1_3__WORDSPARAGRAPH_H

#include <QString>
#include <q3valuelist.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QTextStream>

#include "kword13layout.h"

class Words13Format;

/**
 * A paragraph
 */
class Words13Paragraph
{
public:
    explicit Words13Paragraph(void);
    ~Words13Paragraph(void);
public:
    void xmldump(QTextStream& iostream);
    void setText(const QString& str);
    void appendText(const QString& str);
    QString text(void) const;
public:
    /// \<LAYOUT\>
    Words13Layout m_layout;
    /// \<FORMATS\>
    Q3PtrList<Words13Format> m_formats;
private:
    QString m_text; ///< Text
};

/**
 * Group of paragraphs
 * (Mostly all paragraphs of a text frameset)
 */
class Words13ParagraphGroup : public Q3ValueList <Words13Paragraph>
{
public:
    void xmldump(QTextStream& iostream);
};

#endif //  _FILTER_WORDS_1_3__WORDSPARAGRAPH_H
