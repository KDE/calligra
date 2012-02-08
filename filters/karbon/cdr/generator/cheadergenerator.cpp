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

#include "cheadergenerator.h"

// generator
#include "formatdocument.h"
// Qt
#include <QtCore/QIODevice>


bool
CHeaderGenerator::write( FormatDocument* document, QIODevice* device )
{
    CHeaderGenerator generator( document, device );

    return true; // TODO: how to learn about error?
}

CHeaderGenerator::CHeaderGenerator( FormatDocument* document, QIODevice* device )
  : mDocument( document)
  , mTextStream( device )
{
    const QString upperName = QString::fromLatin1("cdr4structs").toUpper() + QLatin1String("_H");
    mTextStream << QLatin1String("#ifndef ")<<upperName<<QLatin1Char('\n');
    mTextStream << QLatin1String("#define ")<<upperName<<QLatin1Char('\n');

    foreach( const QString& include, mDocument->includeByType() )
        writeInclude( include );

    writeTypeDefs( mDocument->typeDefByName() );

    foreach( const Enumeration& enumeration, mDocument->enumerations() )
        writeEnums( enumeration );

    foreach( const Structure& structure, mDocument->structures() )
        writeStructure( structure );

    mTextStream << QLatin1String("#endif\n");
}

void
CHeaderGenerator::writeInclude( const QString& fileName )
{
    mTextStream << QLatin1String("#include <") << fileName << QLatin1String(">\n");
}

void
CHeaderGenerator::writeTypeDefs( const QHash<QString,QString>& typeDefByName )
{
    QHash<QString,QString>::ConstIterator begin = typeDefByName.constBegin();
    QHash<QString,QString>::ConstIterator end = typeDefByName.constEnd();
    QHash<QString,QString>::ConstIterator it = begin;
    for( ; it != end; ++it )
    {
        mTextStream << QLatin1String("typedef ") << it.value() << QLatin1String(" ") << it.key() << QLatin1String(";\n");;
    }
}

void
CHeaderGenerator::writeEnums( const Enumeration& enumeration )
{
    mTextStream << QLatin1String("enum ") << enumeration.name() << QLatin1String("\n{");

    const QMap<int,QString>& items = enumeration.items();
    QMap<int,QString>::ConstIterator begin = items.constBegin();
    QMap<int,QString>::ConstIterator end = items.constEnd();
    QMap<int,QString>::ConstIterator it = begin;
    for( ; it != end; ++it )
    {
        const char* lineBreak = (it!=begin)?",\n":"\n";
        mTextStream << QLatin1String(lineBreak)
                    << QLatin1String("    ") << it.value() << QLatin1String(" = ") << it.key();
    }

    mTextStream << QLatin1String("\n};\n");
}

void
CHeaderGenerator::writeStructure( const Structure& structure )
{
    mTextStream << QLatin1String("struct ") << structure.name();
    if( ! structure.baseName().isEmpty() )
        mTextStream << QLatin1String(" : public ") << structure.baseName();
    mTextStream << QLatin1String("\n{\n");

    // methods
    foreach( const QString& method, structure.methods() )
        mTextStream << QLatin1String("    ") << method << QLatin1Char('\n');

    // members
    foreach( const StructureMember& member, structure.members() )
        mTextStream << QLatin1String("    ") << member.typeId() << QLatin1Char(' ') << member.name() << QLatin1String(";\n");

    mTextStream << QLatin1String("};\n");
}
