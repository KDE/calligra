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

#ifndef FORMATDOCUMENT_H
#define FORMATDOCUMENT_H

// Qt
#include <QtCore/QVector>
#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QString>


class Enumeration
{
public:
    const QString& name() const { return mName; }
    const QMap<int,QString>& items() const { return mItems; }
public:
    void setName( const QString& name )
    { mName = name; }
    void insertItem( int value, const QString& name )
    { mItems.insert(value, name); }
private:
    QString mName;
    QMap<int,QString> mItems;
};


class StructureMember
{
public:
    StructureMember() {}
    StructureMember( const QString& name, const QString& typeId ) : mName( name ), mTypeId( typeId ) {}
    const QString& name() const { return mName; }
    const QString& typeId() const { return mTypeId; }
private:
    QString mName;
    QString mTypeId;
};


class Structure
{
public:
    void setName( const QString& name ) { mName = name; }
    void setBaseName( const QString& baseName ) { mBaseName = baseName; }
    void appendMember( const StructureMember& member ) { mMembers.append(member); }
    void appendMethod( const QString& method ) { mMethods.append(method); }
    const QString& name() const { return mName; }
    const QString& baseName() const { return mBaseName; }
    const QVector<StructureMember>& members() const { return mMembers; }
    const QVector<QString>& methods() const { return mMethods; }
private:
    QString mName;
    QString mBaseName;
    QVector<StructureMember> mMembers;
    QVector<QString> mMethods;
};


class FormatDocument
{
public:
    void appendStructure( const Structure& structure ) { mStructures.append(structure); }
    void appendEnumeration( const Enumeration& enumeration ) { mEnumeration.append(enumeration); }
    void insertTypeDef( const QString& typeName, const QString& originalName )
    { mTypeDefByName.insert(typeName, originalName); }
    void insertInclude( const QString& typeName, const QString& include )
    { mIncludeByType.insert(typeName, include); }
    const QHash<QString,QString>& typeDefByName() const { return mTypeDefByName; }
    const QHash<QString,QString>& includeByType() const { return mIncludeByType; }
    const QVector<Enumeration>& enumerations() const { return mEnumeration; }
    const QVector<Structure>& structures() const { return mStructures; }
private:
    QHash<QString,QString> mTypeDefByName;
    QHash<QString,QString> mIncludeByType;
    QVector<Enumeration> mEnumeration;
    QVector<Structure> mStructures;
};

#endif
