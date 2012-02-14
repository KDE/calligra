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
#include <QtCore/QStringList>
#include <QtCore/QSet>


static const int maxReturnByValueTypeSize = 4;

static
const QString&
codeName( const QString& typeName )
{
    static QHash<QString,QString> mTypeTable;
    if( mTypeTable.isEmpty() )
    {
        struct BuiltInTypeDatum { char const* name; char const* codeName; };
        static const BuiltInTypeDatum builtInTypeData[] =
        {
            {"uchar",  "unsigned char"},
            {"sint8",  "qint8"},
            {"uint8",  "quint8"},
            {"sint16", "qint16"},
            {"uint16", "quint16"},
            {"sint32", "qint32"},
            {"uint32", "quint32"},
            {"sint64", "qint64"},
            {"uint64", "quint64"}
        };
        static const int builtInTypeDataSize = sizeof( builtInTypeData ) / sizeof( builtInTypeData[0] );
        for( int i = 0; i < builtInTypeDataSize; ++i )
        {
            const BuiltInTypeDatum& builtInTypeDatum = builtInTypeData[i];
            mTypeTable.insert( QLatin1String(builtInTypeDatum.name), QLatin1String(builtInTypeDatum.codeName) );
        }
    }

    const QHash<QString,QString>::ConstIterator it = mTypeTable.find( typeName );

    return (it!=mTypeTable.constEnd()) ? it.value() : typeName;
}

static inline
QString
memberName( const QString& fieldName )
{
    return QLatin1String("__") + fieldName;
}

static inline
QString
lengthFormula( const QString& fieldName )
{
    return fieldName.isEmpty() ? QString() : (fieldName + QLatin1String("()"));

}

class StructMemberAddress
{
public:
    StructMemberAddress() : mIsPointerMember(false), mLength(0), mIsAbsolute(true), mIsBroken(false) {}
    void insertStaticSizeMember( const QString& typeName, const QString& memberName,
                                 bool isPointerMember, int length );
    void insertDynamicSizeMember( const QString& typeName, const QString& memberName,
                                  const QString& lengthFormula );
    QString memberDeclaration() const;
    QString reference() const;
    bool isAbsolute() const { return mIsAbsolute; }
    bool isBroken() const { return mIsBroken; }
private:
    void updateAddress();
private:
    QString mTypeName;
    QString mMemberName;
    bool mIsPointerMember;

    int mLength; // -1 means it is a dynamic size
    QString mLengthFormula;

    QString mAddress;
    bool mIsAbsolute : 1;
    bool mIsBroken : 1;
};

void
StructMemberAddress::updateAddress()
{
    // for the first member there is nothing to do
    if( mTypeName.isEmpty() )
        return;

    if( mIsAbsolute )
    {
        // replace existing
        mAddress = QLatin1String("reinterpret_cast<const char*>(");
        if( ! mIsPointerMember || (mLength == -1) ) mAddress.append( QLatin1Char('&') );
        mAddress = mAddress + mMemberName + QLatin1Char(')');
    }
    if( mLength == -1 )
    {
        if( mLengthFormula.isEmpty() )
        {
            mIsBroken = true;
            return;
        }
        mAddress = mAddress+QLatin1String("+(sizeof(")+mTypeName+QLatin1String(")*")+mLengthFormula+QLatin1Char(')');
        mIsAbsolute = false;
    }
    else
        mAddress = mAddress+QLatin1String("+(sizeof(")+mTypeName+QLatin1String(")*")+QString::number(mLength)+QLatin1Char(')');
}

void
StructMemberAddress::insertStaticSizeMember( const QString& typeName, const QString& memberName,
                                             bool isPointerMember, int length )
{
    if( mIsBroken )
        return;

    updateAddress();

    mTypeName = typeName;
    mMemberName = memberName;
    mLength = length;
    mIsPointerMember = isPointerMember;
}

void
StructMemberAddress::insertDynamicSizeMember( const QString& typeName,
                                              const QString& memberName,
                                              const QString& lengthFormula )
{
    if( mIsBroken )
        return;

    updateAddress();

    mTypeName = typeName;
    mMemberName = memberName;
    mIsPointerMember = true;
    mLengthFormula = lengthFormula;
    mLength = -1;
}


QString
StructMemberAddress::reference() const
{
    if( mIsAbsolute )
        return mMemberName;

    return QLatin1String("*reinterpret_cast<const ")+mTypeName+("*>(") + mAddress + QLatin1Char(')');
}

QString StructMemberAddress::memberDeclaration() const
{
    if( ! mIsAbsolute )
        return QString();

    if( mIsPointerMember ) // or add to members ourselves?
    {
        if( mLength == -1 )
            return mTypeName+QLatin1Char(' ')+mMemberName+QLatin1String(";\n");
        else
            return mTypeName+QLatin1Char(' ')+mMemberName+
                   QLatin1Char('[')+QString::number(mLength)+QLatin1String("];\n");
    }
    else
        return mTypeName+QLatin1Char(' ')+mMemberName+QLatin1String(";\n");
}



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
    // header
    mTextStream << QLatin1String("// Generated by cheadergenerator\n");
    // include guards begin
    const QString upperName = QString::fromLatin1("cdr4structs").toUpper() + QLatin1String("_H");
    mTextStream << QLatin1String("#ifndef ")<<upperName<<QLatin1Char('\n');
    mTextStream << QLatin1String("#define ")<<upperName<<QLatin1Char('\n');

    QSet<QString> includes;
    // add custom include for q-typedefs
    includes.insert( QLatin1String("QtCore/QtGlobal") );
    foreach( const IncludedType& includedType, mDocument->includedTypes() )
        includes.insert(includedType.includeName());
    writeIncludes( includes.toList() );

    writeTypeDefs( mDocument->typeDefByName() );

    foreach( const Enumeration& enumeration, mDocument->enumerations() )
        writeEnums( enumeration );


    foreach( const Record* record, mDocument->records() )
        writeRecord( record );

    // include guards end
    mTextStream << QLatin1String("#endif\n");
}

void
CHeaderGenerator::writeIncludes( const QStringList& fileNames )
{
    foreach( const QString& fileName, fileNames )
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
        mTextStream << QLatin1String("typedef ") << codeName(it.value()) << QLatin1String(" ") << it.key() << QLatin1String(";\n");;
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
CHeaderGenerator::writeRecord( const Record* record )
{
    mTextStream << QLatin1String("struct ") << record->name();
    if( ! record->baseName().isEmpty() )
        mTextStream << QLatin1String(" : public ") << record->baseName();
    mTextStream << QLatin1String("\n{\n");

    QStringList getters;
    QStringList members;

    // methods
    foreach( const QString& method, record->methods() )
        mTextStream << QLatin1String("    ") << method << QLatin1Char('\n');

    StructMemberAddress structMemberAddress;

    // members
    foreach( const AbstractRecordField* field, record->fields() )
    {
        if( field->typeId() == PlainFieldId )
        {
            const PlainRecordField* plainField = static_cast<const PlainRecordField*>( field );
            const QString& typeName = codeName(plainField->typeId());
            const QString memberName = ::memberName(plainField->name());
            const int memberSize = mDocument->sizeOfType(plainField->typeId());
            structMemberAddress.insertStaticSizeMember( typeName, memberName, false, memberSize );

            // member
            if( structMemberAddress.isAbsolute() )
                members.append( structMemberAddress.memberDeclaration() );
            // access method
            if( memberSize <= maxReturnByValueTypeSize )
                // return by value
                getters.append( typeName+QLatin1Char(' ')+plainField->name() +
                                QLatin1String("() const { return ")+structMemberAddress.reference()+
                                QLatin1String("; }\n") );
            else
                // return by reference
                getters.append( QLatin1String("const ")+typeName+QLatin1String("& ")+plainField->name() +
                                QLatin1String("() const { return ")+structMemberAddress.reference()+
                                QLatin1String("; }\n") );
        }
        else if( field->typeId() == ArrayFieldId )
        {
            const ArrayRecordField* arrayField = static_cast<const ArrayRecordField*>( field );
            const QString& typeName = codeName(arrayField->typeId());
            const QString memberName = ::memberName(arrayField->name());
            const int typeSize = mDocument->sizeOfType(arrayField->typeId());
            structMemberAddress.insertStaticSizeMember( typeName, memberName, true, arrayField->arraySize() );

            // member
            if( structMemberAddress.isAbsolute() )
                members.append( structMemberAddress.memberDeclaration() );
            // access method
            getters.append( QLatin1String("const ")+typeName+QLatin1String("* ")+arrayField->name() +
                            QLatin1String("Ptr() const { return ")+structMemberAddress.reference()+
                            QLatin1String("; }\n") );
            if( typeSize <= maxReturnByValueTypeSize )
                // return by value
                getters.append( typeName+QLatin1Char(' ')+arrayField->name()+
                                QLatin1String("( int i ) const { return ")+structMemberAddress.reference()+
                                QLatin1String("[i]; }\n") );
            else
                // return by reference
                getters.append( QLatin1String("const ")+typeName+QLatin1String("& ")+arrayField->name()+
                                QLatin1String("( int i ) const { return ")+structMemberAddress.reference()+
                                QLatin1String("[i]; }\n") );
        }
        else if( field->typeId() == DynArrayFieldId )
        {
            const DynArrayRecordField* arrayField = static_cast<const DynArrayRecordField*>( field );
            const QString& typeName = codeName(arrayField->typeId());
            const QString memberName = ::memberName(arrayField->name());
            const int typeSize = mDocument->sizeOfType(arrayField->typeId());
            const QString lengthFormula = ::lengthFormula(arrayField->lengthField());
            structMemberAddress.insertDynamicSizeMember( typeName, memberName, lengthFormula );

            // member
            if( structMemberAddress.isAbsolute() )
                members.append( structMemberAddress.memberDeclaration() );
            // access method
            getters.append( QLatin1String("const ")+typeName+QLatin1String("* ")+arrayField->name() +
                            QLatin1String("Ptr() const { return &")+structMemberAddress.reference()+
                            QLatin1String("; }\n") );
            if( typeSize <= maxReturnByValueTypeSize )
                // return by value
                getters.append( typeName+QLatin1Char(' ')+arrayField->name()+
                                QLatin1String("( int i ) const { return (&")+structMemberAddress.reference()+
                                QLatin1String(")[i]; }\n") );
            else
                // return by reference
                getters.append( QLatin1String("const ")+typeName+QLatin1String("& ")+arrayField->name()+
                                QLatin1String("( int i ) const { return (&")+structMemberAddress.reference()+
                                QLatin1String(")[i]; }\n") );
        }
        else if( field->typeId() == Text8BitFieldId )
        {
            const Text8BitRecordField* textField = static_cast<const Text8BitRecordField*>( field );
            const QString memberName = ::memberName(textField->name());
            const int length = textField->length();
            structMemberAddress.insertStaticSizeMember( QLatin1String("char"), memberName, true, length );

            // member
            if( structMemberAddress.isAbsolute() )
                members.append( structMemberAddress.memberDeclaration() );
            // access method, TODO: add check for length and no \0
            getters.append( QLatin1String("const char* ")+textField->name()+
                            QLatin1String("() const { return ")+structMemberAddress.reference()+
                            QLatin1String("; }\n") );
        }
        else if( field->typeId() == DynText8BitFieldId )
        {
            const DynText8BitRecordField* textField = static_cast<const DynText8BitRecordField*>( field );
            const QString memberName = ::memberName(textField->name());
            const QString lengthFormula = ::lengthFormula(textField->lengthField());
            structMemberAddress.insertDynamicSizeMember( QLatin1String("char"), memberName, lengthFormula );

            // member
            if( structMemberAddress.isAbsolute() )
                members.append( structMemberAddress.memberDeclaration() );
            // access method
            getters.append( QLatin1String("const char* ")+textField->name()+
                            QLatin1String("() const { return &")+structMemberAddress.reference()+
                            QLatin1String("; }\n") );
        }
        else if( field->typeId() == DynBlobFieldId )
        {
            const DynBlobRecordField* blobField = static_cast<const DynBlobRecordField*>( field );
            const QString memberName = ::memberName(blobField->name());
            const QString lengthFormula = ::lengthFormula(blobField->lengthField());
            structMemberAddress.insertDynamicSizeMember( QLatin1String("char"), memberName, lengthFormula );

            // member
            if( structMemberAddress.isAbsolute() )
                members.append( structMemberAddress.memberDeclaration() );
            // access method
            getters.append( QLatin1String("const char* ")+blobField->name()+
                            QLatin1String("() const { return &")+structMemberAddress.reference()+
                            QLatin1String("; }\n") );
        }
        else if( field->typeId() == UnionFieldId )
        {
            const UnionRecordField* unionField = static_cast<const UnionRecordField*>( field );
            const QString memberName = ::memberName(unionField->name());
            int memberSize = 1;
            structMemberAddress.insertStaticSizeMember( QLatin1String("char"), memberName, false, memberSize );

            // member
            if( structMemberAddress.isAbsolute() )
                members.append( structMemberAddress.memberDeclaration() );
            // access methods
            foreach( const RecordFieldUnionVariant& variant, unionField->variants() )
            {
                const QString& typeName = codeName(variant.typeId());
                getters.append( QLatin1String("const ")+typeName+QLatin1String("& ")+variant.name() +
                                QLatin1String("() const { return reinterpret_cast<const ")+typeName+
                                ("&>(")+structMemberAddress.reference()+QLatin1String("); }\n") );
                memberSize = qMax( memberSize, mDocument->sizeOfType(variant.typeId()) );
            }
        }
    }

    mTextStream << QLatin1String("public:\n");
    foreach( const QString& getter, getters )
        mTextStream << QLatin1String("    ") << getter;
    mTextStream << QLatin1String("private:\n");
    foreach( const QString& member, members )
        mTextStream << QLatin1String("    ") << member;
    mTextStream << QLatin1String("};\n");
}
