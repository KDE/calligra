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
#include <QtCore/QScopedPointer>

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
        else if( tokenName == QLatin1String("records") )
            readRecords();
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
                    IncludedType includedType;
                    includedType.setName(typeName);
                    includedType.setIncludeName(includeName);
                    includedType.setSize(byteSize);
                    mDocument->appendIncludedType(includedType);
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
FormatParser::readRecords()
{
    while( mReader.readNextStartElement() )
    {
        if( mReader.name() == QLatin1String("record") )
        {
            QScopedPointer<Record> record( new Record );
            const QXmlStreamAttributes attributes = mReader.attributes();
            const int recordSize = attributes.value(QLatin1String("size")).toString().toInt();
            const QString recordName = attributes.value(QLatin1String("name")).toString();
            record->setName( recordName );
            const QStringRef recordBaseNameStringRef = attributes.value(QLatin1String("base"));
            if( ! recordBaseNameStringRef.isEmpty() )
            {
                const QString recordBaseName = recordBaseNameStringRef.toString();
                const Record* baseRecord = mDocument->record(recordBaseName);
                if( baseRecord == 0 )
                {
                    mReader.raiseError( QLatin1String("Record ")+recordBaseName+QLatin1String(" used as base not yet known.") );
                    break;
                }
                record->setBaseName( recordBaseName );
                record->setSize( baseRecord->size() );
            }
qDebug() << "record:" <<record->name()<<record->baseName();
            while( mReader.readNextStartElement() )
            {
                if( mReader.name() == QLatin1String("field") )
                {
                    const QXmlStreamAttributes attributes = mReader.attributes();
                    const QString fieldName = attributes.value(QLatin1String("name")).toString();
                    const QString fieldType = attributes.value(QLatin1String("type")).toString();
                    // check offsets
                    const int startOffset = attributes.value(QLatin1String("start")).toString().toInt();
                    const int endOffset = attributes.value(QLatin1String("end")).toString().toInt();
                    if( startOffset != record->size() )
                    {
                        mReader.raiseError( QLatin1String("Startoffset ")+QString::number(startOffset)+QLatin1String(" is not aligned to last field.") );
                        break;
                    }
                    int fieldSize = mDocument->sizeOfType(fieldType);
                    if( fieldSize == -1 )
                    {
                        mReader.raiseError( fieldType+QLatin1String(" is not yet known.") );
                        break;
                    }
                    if( (endOffset-startOffset+1) != fieldSize )
                    {
                        mReader.raiseError( QLatin1String("Endoffset ")+QString::number(endOffset)+QLatin1String(" does not match the size of the type.") );
                        break;
                    }

                    PlainRecordField* plainField = new PlainRecordField( fieldName, fieldType );
                    record->appendField( plainField, fieldSize );
qDebug() << "  field:" <<plainField->name() <<plainField->typeId();
                }
                else if( mReader.name() == QLatin1String("array") )
                {
                    const QXmlStreamAttributes attributes = mReader.attributes();
                    const QString fieldName = attributes.value(QLatin1String("name")).toString();
                    const QString arrayType = attributes.value(QLatin1String("type")).toString();
                    const int arrayLength = attributes.value(QLatin1String("length")).toString().toInt();
                    // check offsets
                    const int startOffset = attributes.value(QLatin1String("start")).toString().toInt();
                    const int endOffset = attributes.value(QLatin1String("end")).toString().toInt();
                    if( startOffset != record->size() )
                    {
                        mReader.raiseError( QLatin1String("Startoffset ")+QString::number(startOffset)+QLatin1String(" is not aligned to last field.") );
                        break;
                    }
                    int fieldSize = mDocument->sizeOfType(arrayType);
                    if( fieldSize == -1 )
                    {
                        mReader.raiseError( arrayType+QLatin1String(" is not yet known.") );
                        break;
                    }
                    fieldSize *= arrayLength;
                    if( (endOffset-startOffset+1) != fieldSize )
                    {
                        mReader.raiseError( QLatin1String("Endoffset ")+QString::number(endOffset)+QLatin1String(" does not match the size of the type.") );
                        break;
                    }

                    ArrayRecordField* arrayField = new ArrayRecordField( fieldName, arrayType, arrayLength );
                    record->appendField( arrayField, fieldSize );
qDebug() << "  array:" <<arrayField->name() <<arrayField->typeId() << arrayField->arraySize();
                }
                else if( mReader.name() == QLatin1String("dynarray") )
                {
                    const QXmlStreamAttributes attributes = mReader.attributes();
                    const QString fieldName = attributes.value(QLatin1String("name")).toString();
                    const QString arrayType = attributes.value(QLatin1String("type")).toString();
//                     const int arrayLength = attributes.value(QLatin1String("length")).toString().toInt();
                    // check offsets
                    const int startOffset = attributes.value(QLatin1String("start")).toString().toInt();
                    if( startOffset != record->size() )
                    {
                        mReader.raiseError( QLatin1String("Startoffset ")+QString::number(startOffset)+QLatin1String(" is not aligned to last field.") );
                        break;
                    }
                    int fieldSize = mDocument->sizeOfType(arrayType);
                    if( fieldSize == -1 )
                    {
                        mReader.raiseError( arrayType+QLatin1String(" is not yet known.") );
                        break;
                    }

                    DynArrayRecordField* dynArrayField = new DynArrayRecordField( fieldName, arrayType );
                    record->appendField( dynArrayField, 0 );
qDebug() << "  dynarray:" <<dynArrayField->name() <<dynArrayField->typeId();
                }
                else if( mReader.name() == QLatin1String("extension") )
                {
                    const QXmlStreamAttributes attributes = mReader.attributes();
                    const int startOffset = attributes.value(QLatin1String("start")).toString().toInt();
                    if( startOffset != record->size() )
                    {
                        mReader.raiseError( QLatin1String("Startoffset ")+QString::number(startOffset)+QLatin1String(" is not aligned to last field.") );
                        break;
                    }
                    const QString itemName = attributes.value(QLatin1String("name")).toString();
                    if( ! itemName.isEmpty() )
                    {
                        const QString itemTypeId = attributes.value(QLatin1String("type")).toString();
                        PlainRecordField* recordField = new PlainRecordField( itemName, itemTypeId );
                        record->appendField( recordField, 0 );
qDebug() << "  extension:" <<recordField->name() <<recordField->typeId();
                    }
                }
                else if( mReader.name() == QLatin1String("method") )
                {
                    while( mReader.readNextStartElement() )
                    {
                        if( mReader.name() == QLatin1String("code") )
                        {
                            const QString code = mReader.readElementText();
                            record->appendMethod(code);
                            qDebug() << code << mReader.tokenString()<<mReader.qualifiedName();
                        }
                        else
                            mReader.skipCurrentElement();
                    }
                }
                if( mReader.tokenType() != QXmlStreamReader::EndElement )
                    mReader.skipCurrentElement();
            }
            if( recordSize != record->size() )
            {
                mReader.raiseError( QLatin1String("Size of the record ")+QString::number(recordSize)+QLatin1String(" does not match the size of its fields.") );
                break;
            }
            mDocument->appendRecord(record.take());
        }
        else
            mReader.skipCurrentElement();
    }
}
