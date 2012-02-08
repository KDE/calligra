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

#include "formatparser.h"

// generator
#include "formatdocument.h"
// Qt
#include <QtCore/QIODevice>

#include <QDebug>

FormatDocument*
FormatParser::parse( QIODevice* device )
{
    FormatParser parser( device );

    if( parser.mReader.hasError() )
        qDebug() << parser.mReader.errorString()
                 << parser.mReader.lineNumber() << QLatin1Char(':') << parser.mReader.columnNumber();

    return parser.mDocument;
}

FormatParser::FormatParser( QIODevice* device )
  : mReader( device )
  , mDocument( new FormatDocument )
{
    if( mReader.readNextStartElement() )
    {
        if (mReader.name() == QLatin1String("format") &&
           (mReader.attributes().value(QLatin1String("version")) == QLatin1String("1.0")) )
            readFormat();
        else
            mReader.raiseError( QObject::tr("The file is not a FORMAT version 1.0 file.") );
    }
    else
        mReader.raiseError( QObject::tr("The file is not a FORMAT version 1.0 file.") );

    if( mReader.hasError() )
    {
        delete mDocument;
        mDocument = 0;
    }
}

void
FormatParser::readFormat()
{
    while( mReader.readNextStartElement() )
    {
        const QStringRef tokenName = mReader.name();
        if( tokenName == QLatin1String("includes") )
            readIncludes();
        else if( tokenName == QLatin1String("typedefs") )
            readTypeDefs();
        else if( tokenName == QLatin1String("enums") )
            readEnums();
        else if( tokenName == QLatin1String("structs") )
            readStructs();
        else
            mReader.skipCurrentElement();
    }
}

void
FormatParser::readIncludes()
{
    const QString includeName = mReader.attributes().value(QLatin1String("name")).toString();

     while( mReader.readNextStartElement() )
     {
         if( mReader.name() == QLatin1String("file") )
         {
             const QString includeName = mReader.attributes().value(QLatin1String("name")).toString();
             while( mReader.readNextStartElement() )
             {
                 if( mReader.name() == QLatin1String("type") )
                 {
                    const QXmlStreamAttributes attributes = mReader.attributes();
                    const QString typeName = attributes.value(QLatin1String("name")).toString();
                    const int byteSize = attributes.value(QLatin1String("size")).toString().toInt();
                    mDocument->insertInclude(typeName, includeName);
qDebug() << "type:" <<typeName << byteSize <<includeName;
                }
                mReader.skipCurrentElement();
             }
         }
         else
             mReader.skipCurrentElement();
     }
}

void
FormatParser::readTypeDefs()
{
    while( mReader.readNextStartElement() )
    {
        if( mReader.name() == QLatin1String("type") )
        {
            const QXmlStreamAttributes attributes = mReader.attributes();
            const QString typeName = attributes.value(QLatin1String("name")).toString();
            const QString originalName = attributes.value(QLatin1String("original")).toString();
            mDocument->insertTypeDef(typeName, originalName);
qDebug() << "typedef:" <<typeName << originalName;
        }
//         else
            mReader.skipCurrentElement();
    }
}

void
FormatParser::readEnums()
{
    while( mReader.readNextStartElement() )
    {
        if( mReader.name() == QLatin1String("enum") )
        {
            Enumeration enumeration;
            const QString enumName = mReader.attributes().value(QLatin1String("name")).toString();
            enumeration.setName( enumName );
qDebug() << "enum:" <<enumName;
            while( mReader.readNextStartElement() )
            {
                if( mReader.name() == QLatin1String("item") )
                {
                    const QXmlStreamAttributes attributes = mReader.attributes();
                    const QString itemName = attributes.value(QLatin1String("name")).toString();
                    const int value = attributes.value(QLatin1String("value")).toString().toInt();
                    enumeration.insertItem(value, itemName);
qDebug() << "  item:" <<itemName <<value;
                }
//                 else
                    mReader.skipCurrentElement();
            }
            mDocument->appendEnumeration(enumeration);
        }
        else
            mReader.skipCurrentElement();
    }
}

void
FormatParser::readStructs()
{
    while( mReader.readNextStartElement() )
    {
        if( mReader.name() == QLatin1String("struct") )
        {
            Structure structure;
            const QXmlStreamAttributes attributes = mReader.attributes();
            const QString structName = attributes.value(QLatin1String("name")).toString();
            structure.setName( structName );
            const QString structBaseName = attributes.value(QLatin1String("base")).toString();
            structure.setBaseName( structBaseName );
qDebug() << "struct:" <<structName<<structBaseName;
            while( mReader.readNextStartElement() )
            {
                if( mReader.name() == QLatin1String("member") )
                {
                    const QXmlStreamAttributes attributes = mReader.attributes();
                    const QString itemName = attributes.value(QLatin1String("name")).toString();
                    const QString itemTypeId = attributes.value(QLatin1String("type")).toString();
                    StructureMember structureMember( itemName, itemTypeId );
                    structure.appendMember( structureMember );
qDebug() << "  member:" <<itemName <<itemTypeId;
                }
                else if( mReader.name() == QLatin1String("method") )
                {
                    while( mReader.readNextStartElement() )
                    {
                        if( mReader.name() == QLatin1String("code") )
                        {
                            const QString code = mReader.readElementText();
                            structure.appendMethod(code);
                            qDebug() << code << mReader.tokenString()<<mReader.qualifiedName();
                        }
                        else
                            mReader.skipCurrentElement();
                    }
                }
                if( mReader.tokenType() != QXmlStreamReader::EndElement )
                    mReader.skipCurrentElement();
            }
            mDocument->appendStructure(structure);
        }
        else
            mReader.skipCurrentElement();
    }
}
