/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#ifndef __AUTOFILL_H__
#define __AUTOFILL_H__

#include <QList>
#include <QString>
#include <QStringList>

namespace KSpread
{
class Cell;

/**
 * A cell content for auto-filling.
 */
class AutoFillSequenceItem
{
public:
    enum Type { INTEGER, FLOAT, STRING, DAY, SHORTDAY,
      MONTH, SHORTMONTH, FORMULA, OTHER, DATE, TIME };

    explicit AutoFillSequenceItem( const Cell* cell );

    double delta( AutoFillSequenceItem *_seq, bool *ok = 0 ) const;

    QVariant nextValue( int _no, double _delta );
    QVariant prevValue( int _no, double _delta );

    Type type()          const { return m_Type; }
    int getIValue()      const { return m_IValue; }
    double getDValue()   const { return m_DValue; }
    QString getString()  const { return m_String; }
    int getIOtherEnd()   const {return m_OtherEnd; }
    int getIOtherBegin() const {return m_OtherBegin; }

    static QStringList *other;
    static QStringList *month;
    static QStringList *day;
    static QStringList *shortMonth;
    static QStringList *shortDay;

protected:
    int     m_IValue;
    double  m_DValue;
    int     m_OtherBegin;
    int     m_OtherEnd;
    QString m_String;
    Type    m_Type;
};

/**
 * A sequence of cell contents for auto-filling.
 */
class AutoFillSequence : public QList<AutoFillSequenceItem*>
{
public:
    AutoFillSequence();
    AutoFillSequence( const QList<AutoFillSequenceItem*>& );
    ~AutoFillSequence();

    QList<double> createDeltaSequence( int intervalLength ) const;
};

} // namespace KSpread

#endif
