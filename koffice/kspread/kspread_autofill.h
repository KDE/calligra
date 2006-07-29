/* This file is part of the KDE project
   
   Copyright 2003 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 Norbert Andres <nandres@web.de>
   Copyright 2001-2002 Laurent Montel <montel@kde.org>
   Copyright 2001 David Faure <faure@kde.org>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1998-1999 Torben Weis <weis@kde.org>
   
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

#ifndef __kspread_autofill_h__
#define __kspread_autofill_h__

#include <qdatetime.h>
#include <qmemarray.h>
#include <qstring.h>
#include <qptrlist.h>
#include <qstringlist.h>

namespace KSpread
{
class AutoFillDeltaSequence;
class AutoFillSequenceItem;
class AutoFillSequence;
class Cell;

class AutoFillSequenceItem
{
public:
    enum Type { INTEGER, FLOAT, STRING, DAY, SHORTDAY,
      MONTH, SHORTMONTH, FORMULA, OTHER, DATE, TIME };

    AutoFillSequenceItem( int _i );
    AutoFillSequenceItem( double _d );
    AutoFillSequenceItem( const QString &_str );

    bool getDelta( AutoFillSequenceItem *_seq, double &delta );

    QString getSuccessor( int _no, double _delta );
    QString getPredecessor( int _no, double _delta );

    Type getType()const { return m_Type; }
    int getIValue()const { return m_IValue; }
    double getDValue()const { return m_DValue; }
    QString getString()const { return m_String; }
    int getIOtherEnd()const {return m_OtherEnd; }
    int getIOtherBegin()const {return m_OtherBegin; }
    static QStringList *other;
    static QStringList *month;
    static QStringList *day;
    static QStringList *shortMonth;
    static QStringList *shortDay;
protected:
    int    m_IValue;
    double m_DValue;
    int    m_OtherBegin;
    int    m_OtherEnd;

    QString m_String;

    Type m_Type;
};

class AutoFillSequence
{
public:
    AutoFillSequence( Cell *_obj );

    int count()const { return sequence.count(); }

    AutoFillSequenceItem* getFirst() { return sequence.first(); }
    AutoFillSequenceItem* getNext() { return sequence.next(); }

    bool matches( AutoFillSequence* _seq, AutoFillDeltaSequence *_delta );

    void fillCell( Cell *src, Cell *dest, AutoFillDeltaSequence *delta, int _block, bool down = true );

protected:
    QPtrList<AutoFillSequenceItem> sequence;
};

class AutoFillDeltaSequence
{
public:
    AutoFillDeltaSequence( AutoFillSequence *_first, AutoFillSequence *_next );
    ~AutoFillDeltaSequence();

    bool isOk()const { return m_ok; }

    bool equals( AutoFillDeltaSequence *_delta );

    QMemArray<double>* getSequence() { return m_sequence; }

    double getItemDelta( int _pos );

protected:
    bool m_ok;
    QMemArray<double>* m_sequence;
};

} // namespace KSpread

#endif
