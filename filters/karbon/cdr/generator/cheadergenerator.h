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

#ifndef CHEADERGENERATOR_H
#define CHEADERGENERATOR_H

// Qt
#include <QtCore/QTextStream>

class FormatDocument;
class Record;
class Enumeration;
class QIODevice;
class QStringList;
template<typename T1, typename T2> class QHash;


class CHeaderGenerator
{
private:
    CHeaderGenerator( FormatDocument* document, QIODevice* device );
public:
    static bool write( FormatDocument* document, QIODevice* device );
private:
    void writeIncludes( const QStringList& fileName );
    void writeTypeDefs( const QHash<QString,QString>& typeDefByName );
    void writeEnums( const Enumeration& enumeration );
    void writeRecord( const Record* record );
private:
    FormatDocument* mDocument;
    QTextStream mTextStream;
};

#endif
