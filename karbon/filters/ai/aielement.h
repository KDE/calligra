/***************************************************************************
                          aielement.h  -  description
                             -------------------
    begin                : Sun Feb 17 2002
    copyright            : (C) 2002 by 
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
  Vector
    };

    AIElement();
    ~AIElement();
    AIElement( const AIElement& );
    AIElement( const QString&, Type type = String );
    AIElement( const QCString& );
    AIElement( const char* );
//    AIElement( const QValueList<AIElement>& );
    AIElement( const QValueVector<AIElement>& );
    AIElement( int );
    AIElement( uint );
    AIElement( double );

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
    const QValueVector<AIElement> toVector() const;

    // Custom types
    const QString toReference() const;
    const QString toOperator() const;

//    QValueListConstIterator<AIElement> listBegin() const;
//    QValueListConstIterator<AIElement> listEnd() const;
    QString& asString();
    QCString& asCString();
    int& asInt();
    uint& asUInt();
    double& asDouble();
//    QValueList<AIElement>& asList();
    QValueVector<AIElement>& asVector();

    // Custom types
    QString& asReference();
    QString& asToken();

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
