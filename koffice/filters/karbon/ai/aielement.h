/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef AIELEMENT_H
#define AIELEMENT_H

// #include <qvaluelist.h>
#include <qvaluevector.h>

class QString;
class QCString;

/**
  *@author 
  */

class AIElement {
public:
    enum Type {
	Invalid,
//	List,
	String,
	Int,
  UInt,
	Double,
	CString,
  // Custom Types
  Operator,
  Reference,
  ElementArray,
  Block,
  ByteArray,
  Byte
    };

    AIElement();
    ~AIElement();
    AIElement( const AIElement& );
    AIElement( const QString&, Type type = String );
    AIElement( const QCString& );
    AIElement( const char* );
//    AIElement( const QValueList<AIElement>& );
    AIElement( const QValueVector<AIElement>&, Type type = ElementArray);
    AIElement( int );
    AIElement( uint );
    AIElement( double );
    AIElement( const QByteArray& );
    AIElement( uchar );

    AIElement& operator= ( const AIElement& );
    bool operator==( const AIElement& ) const;
    bool operator!=( const AIElement& ) const;

    Type type() const;
    const char* typeName() const;

    bool canCast( Type ) const;
    bool cast( Type );

    bool isValid() const;

    void clear();

    const QString toString() const;
    const QCString toCString() const;
    int toInt( bool * ok=0 ) const;
    uint toUInt( bool * ok=0 ) const;
    double toDouble( bool * ok=0 ) const;
//    const QValueList<AIElement> toList() const;
    const QValueVector<AIElement> toElementArray() const;
    const QValueVector<AIElement> toBlock() const;

    // Custom types
    const QString toReference() const;
    const QString toOperator() const;
    const QByteArray toByteArray() const;
    uchar toByte( bool * ok=0 ) const;

//    QValueListConstIterator<AIElement> listBegin() const;
//    QValueListConstIterator<AIElement> listEnd() const;
    QString& asString();
    QCString& asCString();
    int& asInt();
    uint& asUInt();
    double& asDouble();
//    QValueList<AIElement>& asList();
    QValueVector<AIElement>& asElementArray();
    QValueVector<AIElement>& asBlock();

    // Custom types
    QString& asReference();
    QString& asToken();
    QByteArray& asByteArray();
    uchar& asByte();

    static const char* typeToName( Type typ );
    static Type nameToType( const char* name );

private:
    void detach();

    class Private : public QShared
    {
    public:
        Private();
        Private( Private* );
        ~Private();

        void clear();

        Type typ;
        union
        {
	    uint u;
	    int i;
	    double d;
      uchar b;
	    void *ptr;
        } value;
    };

    Private* d;
};

inline AIElement::Type AIElement::type() const
{
    return d->typ;
}

inline bool AIElement::isValid() const
{
    return (d->typ != Invalid);
}

#endif
