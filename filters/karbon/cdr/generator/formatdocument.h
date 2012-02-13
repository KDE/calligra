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


class IncludedType
{
public:
    IncludedType() : mSize(0) {}
    void setName( const QString& name ) { mName = name; }
    void setIncludeName( const QString& includeName ) { mIncludeName = includeName; }
    void setSize( int size ) { mSize = size; }
    const QString& name() const { return mName; }
    const QString& includeName() const { return mIncludeName; }
    int size() const { return mSize; }
private:
    QString mName;
    QString mIncludeName;
    /// size of the type in bytes
    int mSize;
};

enum RecordFieldTypeId
{
    PlainFieldId,
    ArrayFieldId,
    DynArrayFieldId,
    Text8BitFieldId,
    DynText8BitFieldId,
    DynBlobFieldId,
    UnionFieldId
};

class AbstractRecordField
{
protected:
    explicit AbstractRecordField(RecordFieldTypeId typeId) : mTypeId( typeId ) {}
private:
    AbstractRecordField( const AbstractRecordField& );
    AbstractRecordField& operator=( const AbstractRecordField& );
public:
    virtual ~AbstractRecordField() {}
public:
    RecordFieldTypeId typeId() const { return mTypeId; }
private:
    RecordFieldTypeId mTypeId;
};

class PlainRecordField : public AbstractRecordField
{
public:
    PlainRecordField( const QString& name, const QString& typeId )
    : AbstractRecordField(PlainFieldId), mName( name ), mTypeId( typeId ) {}
public:
    const QString& name() const { return mName; }
    const QString& typeId() const { return mTypeId; }
private:
    const QString mName;
    const QString mTypeId;
};

class ArrayRecordField : public AbstractRecordField
{
public:
    ArrayRecordField( const QString& name, const QString& typeId, int arraySize )
    : AbstractRecordField(ArrayFieldId), mName( name ), mTypeId( typeId ), mArraySize( arraySize ) {}
public:
    const QString& name() const { return mName; }
    const QString& typeId() const { return mTypeId; }
    int arraySize() const { return mArraySize; }
private:
    QString mName;
    QString mTypeId;
    int mArraySize;
};

class DynArrayRecordField : public AbstractRecordField
{
public:
    DynArrayRecordField( const QString& name, const QString& typeId )
    : AbstractRecordField(DynArrayFieldId), mName( name ), mTypeId( typeId ) {}
public:
    const QString& name() const { return mName; }
    const QString& typeId() const { return mTypeId; }
private:
    QString mName;
    QString mTypeId;
};

class Text8BitRecordField : public AbstractRecordField
{
public:
    Text8BitRecordField( const QString& name, int length )
    : AbstractRecordField(Text8BitFieldId), mName( name ), mLength( length ) {}
public:
    const QString& name() const { return mName; }
    int length() const { return mLength; }
private:
    QString mName;
    int mLength;
};

class DynText8BitRecordField : public AbstractRecordField
{
public:
    DynText8BitRecordField( const QString& name )
    : AbstractRecordField(DynText8BitFieldId), mName( name ) {}
public:
    const QString& name() const { return mName; }
private:
    QString mName;
};


class DynBlobRecordField : public AbstractRecordField
{
public:
    DynBlobRecordField( const QString& name )
    : AbstractRecordField(DynBlobFieldId), mName( name ) {}
public:
    const QString& name() const { return mName; }
private:
    QString mName;
};


class RecordFieldUnionVariant
{
public:
    void setName( const QString& name ) { mName = name; }
    void setTypeId( const QString& typeId ) { mTypeId = typeId; }
    const QString& name() const { return mName; }
    const QString& typeId() const { return mTypeId; }
private:
    QString mName;
    QString mTypeId;
};


class UnionRecordField : public AbstractRecordField
{
public:
    UnionRecordField( const QString& name )
    : AbstractRecordField(UnionFieldId), mName( name ) {}
public:
    void appendVariant( const RecordFieldUnionVariant& variant ) { mVariants.append(variant); }
    const QString& name() const { return mName; }
    const QVector<RecordFieldUnionVariant>& variants() const { return mVariants; }
private:
    const QString mName;
    QVector<RecordFieldUnionVariant> mVariants;
};


class Record
{
public:
    Record() : mSize(0) {}
    ~Record() { qDeleteAll(mFields); }
    void setName( const QString& name ) { mName = name; }
    void setBaseName( const QString& baseName ) { mBaseName = baseName; }
    /// pass size!=0 only if field is part of static size
    void appendField( AbstractRecordField* field, int size ) { mFields.append(field); mSize += size; }
    void appendMethod( const QString& method ) { mMethods.append(method); }
    void setSize( int size ) { mSize = size; }
    const QString& name() const { return mName; }
    const QString& baseName() const { return mBaseName; }
    const QVector<AbstractRecordField*>& fields() const { return mFields; }
    const QVector<QString>& methods() const { return mMethods; }
    int size() const { return mSize; }
    bool isNull() const { return mName.isNull(); }
private:
    QString mName;
    QString mBaseName;
    QVector<AbstractRecordField*> mFields;
    QVector<QString> mMethods;
    /// size of the record in bytes
    int mSize;
};


class FormatDocument
{
public:
    ~FormatDocument() { qDeleteAll(mRecords); }
    void appendRecord( Record* record ) { mRecords.append(record); }
    void appendIncludedType( const IncludedType& includedType ) { mIncludedTypes.append(includedType); }
    void appendEnumeration( const Enumeration& enumeration ) { mEnumeration.append(enumeration); }
    void insertTypeDef( const QString& typeName, const QString& originalName )
    { mTypeDefByName.insert(typeName, originalName); }
    const QHash<QString,QString>& typeDefByName() const { return mTypeDefByName; }
    const QVector<IncludedType>& includedTypes() const { return mIncludedTypes; }
    const QVector<Enumeration>& enumerations() const { return mEnumeration; }
    const QVector<Record*>& records() const { return mRecords; }
    const Record* record( const QString& name ) const;
    int sizeOfType(const QString& typeName ) const;
private:
    QHash<QString,QString> mTypeDefByName;
    QVector<IncludedType> mIncludedTypes;
    QVector<Enumeration> mEnumeration;
    QVector<Record*> mRecords;
};

#endif
