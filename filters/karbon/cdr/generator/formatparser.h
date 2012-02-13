/* This file is part of the Calligra project, made within the KDE community.

   Copyright 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef FORMATPARSER_H
#define FORMATPARSER_H

// Qt
#include <QtCore/QXmlStreamReader>

class FormatDocument;
class QIODevice;
class QXmlStreamAttributes;


class FormatParser
{
private:
    explicit FormatParser( QIODevice* device );
public:
    static FormatDocument* parse( QIODevice* device );
private:
    void readFormat();
    void readIncludes();
    void readTypeDefs();
    void readEnums();
    void readRecords();

    bool isDeclaredStartOffsetCorrect( const QXmlStreamAttributes& attributes, int expectedStartOffset );
    bool isDeclaredEndOffsetCorrect( const QXmlStreamAttributes& attributes, int expectedEndOffset );
private:
    QXmlStreamReader mReader;
    FormatDocument* mDocument;
};

#endif
