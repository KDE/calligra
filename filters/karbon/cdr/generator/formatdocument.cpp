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

#include "formatdocument.h"


struct BuiltInTypeDatum { char const* name; int size; bool isInteger; };
static const BuiltInTypeDatum builtInTypeData[] =
{
    {"char",   1, false},
    {"uchar",  1, false},
    {"sint8",  1, true},
    {"uint8",  1, true},
    {"sint16", 2, true},
    {"uint16", 2, true},
    {"sint32", 4, true},
    {"uint32", 4, true},
    {"sint64", 8, true},
    {"uint64", 8, true},
    {"float",  4, false}
};
static const int builtInTypeDataSize = sizeof( builtInTypeData ) / sizeof( builtInTypeData[0] );

const AbstractRecordField*
Record::field( const QString& name ) const
{
    const AbstractRecordField* result = 0;

    foreach( const AbstractRecordField* field, mFields )
    {
        if( field->name() == name )
        {
            result = field;
            break;
        }
    }

    return result;
}


const Record*
FormatDocument::record( const QString& name ) const
{
    const Record* result = 0;

    foreach( const Record* record, mRecords )
    {
        if( record->name() == name )
        {
            result = record;
            break;
        }
    }

    return result;
}

const QString&
FormatDocument::realTypeName(const QString& typeName) const
{
    const QString* realTypeName = &typeName;

    while( true )
    {
        QHash<QString,QString>::ConstIterator it = mTypeDefByName.find(*realTypeName);
        if( it == mTypeDefByName.constEnd() )
            break;

        realTypeName = &(it.value());
    }

    return *realTypeName;
}

int
FormatDocument::sizeOfType(const QString& typeName) const
{
    int result = -1;

    const QString& realTypeName = this->realTypeName( typeName );

    foreach( const IncludedType& includedType, mIncludedTypes )
    {
        if( includedType.name() == realTypeName )
        {
            result = includedType.size();
            break;
        }
    }

    if( result == -1 )
    {
        foreach( const Record* record, mRecords )
        {
            if( record->name() == realTypeName )
            {
                result = record->size();
                break;
            }
        }
    }

    if( result == -1 )
    {
        for( int i = 0; i < builtInTypeDataSize; ++i)
        {
            const BuiltInTypeDatum& builtInTypeDatum = builtInTypeData[i];
            if( QLatin1String(builtInTypeDatum.name) == realTypeName )
            {
                result = builtInTypeDatum.size;
                break;
            }
        }
    }

    return result;
}

bool
FormatDocument::isIntegerType(const QString& typeName) const
{
    bool result = false;

    const QString& realTypeName = this->realTypeName( typeName );

    // TODO: check also included types
    for( int i = 0; i < builtInTypeDataSize; ++i)
    {
        const BuiltInTypeDatum& builtInTypeDatum = builtInTypeData[i];
        if( QLatin1String(builtInTypeDatum.name) == realTypeName )
        {
            result = builtInTypeDatum.isInteger;
            break;
        }
    }
    return result;
}
