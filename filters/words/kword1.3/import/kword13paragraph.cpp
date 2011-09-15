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


#include "kword13paragraph.h"
//Added by qt3to4:
#include <Q3ValueList>
#include <QTextStream>

Words13Paragraph::Words13Paragraph(void)
{
    m_formats.setAutoDelete(true);
}

Words13Paragraph::~Words13Paragraph(void)
{
}

void Words13Paragraph::xmldump(QTextStream& iostream)
{
    iostream << "    <paragraph>\n";
    iostream << "      <text>" << m_text << "</text>\n";
    m_layout.xmldump(iostream);
    iostream << "    </paragraph>\n";
}

void Words13Paragraph::setText(const QString& str)
{
    m_text = str;
}

void Words13Paragraph::appendText(const QString& str)
{
    m_text += str;
}

QString Words13Paragraph::text(void) const
{
    return m_text;
}


void Words13ParagraphGroup::xmldump(QTextStream& iostream)
{
    iostream << "   <paragraphgroup>\n";

    for (Q3ValueList <Words13Paragraph>::Iterator it = begin();
            it != end(); ++it) {
        (*it).xmldump(iostream);
    }

    iostream << "   </paragraphgroup>\n";
}

