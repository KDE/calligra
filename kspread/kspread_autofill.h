/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_autofill_h__
#define __kspread_autofill_h__

class AutoFillDeltaSequence;
class AutoFillSequenceItem;
class AutoFillSequence;

#include <qarray.h>
#include <qstring.h>
#include <qlist.h>
#include <qstringlist.h>

class KSpreadCell;

class AutoFillSequenceItem
{
public:
    enum Type { INTEGER, FLOAT, STRING, DAY, MONTH, FORMULAR, OTHER };

    AutoFillSequenceItem( int _i );
    AutoFillSequenceItem( double _d );
    AutoFillSequenceItem( const QString &_str );

    bool getDelta( AutoFillSequenceItem *_seq, double &delta );

    QString getSuccessor( int _no, double _delta );

    Type getType() { return type; }
    int getIValue() { return ivalue; }
    double getDValue() { return dvalue; }
    QString getString() { return string; }
    int getIOtherEnd() {return otherEnd; }
    int getIOtherBegin() {return otherBegin; }

protected:
    int ivalue;
    double dvalue;
    int otherBegin;
    int otherEnd;

    QString string;

    Type type;

    static QStringList *month;
    static QStringList *day;
    static QStringList *other;
};

class AutoFillSequence
{
public:
    AutoFillSequence( KSpreadCell *_obj );

    int count() { return sequence.count(); }

    AutoFillSequenceItem* getFirst() { return sequence.first(); }
    AutoFillSequenceItem* getNext() { return sequence.next(); }

    bool matches( AutoFillSequence* _seq, AutoFillDeltaSequence *_delta );

    void fillCell( KSpreadCell *src, KSpreadCell *dest, AutoFillDeltaSequence *delta, int _block );

protected:
    QList<AutoFillSequenceItem> sequence;
};

class AutoFillDeltaSequence
{
public:
    AutoFillDeltaSequence( AutoFillSequence *_first, AutoFillSequence *_next );
    ~AutoFillDeltaSequence();

    bool isOk() { return ok; }

    bool equals( AutoFillDeltaSequence *_delta );

    QArray<double>* getSequence() { return sequence; }

    double getItemDelta( int _pos );

protected:
    bool ok;
    QArray<double>* sequence;
};

#endif
