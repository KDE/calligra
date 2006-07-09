/* This file is part of the KDE project
   Copyright (C) 2001, 2004 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef _IMPORT_HELPERS_H
#define _IMPORT_HELPERS_H

#include <qstring.h>
#include <qmap.h>

/**
 * \file ImportHelpers.h
 * Helpers for the AbiWord import filter
 *
 * Rough rule for including code in this file:
 *   use nothing of QT except QMap, QValueList and QString
 */

class AbiProps
{
public:
    AbiProps() {};
    AbiProps(QString newValue) : m_value(newValue) {};
    virtual ~AbiProps() {};
public:
    inline QString getValue(void) const { return m_value; }
private:
    QString m_value;
};

class AbiPropsMap : public QMap<QString,AbiProps>
{
public:
    AbiPropsMap() {};
    virtual ~AbiPropsMap() {};
public:
    bool setProperty(const QString& newName, const QString& newValue);
    void splitAndAddAbiProps(const QString& strProps);
};

inline double CentimetresToPoints(const double d)
{
    return d * 72.0 / 2.54;
}

inline double MillimetresToPoints(const double d)
{
    return d * 72.0 / 25.4;
}

inline double InchesToPoints(const double d)
{
    return d * 72.0;
}

inline double PicaToPoints(const double d)
{
    // 1 pica = 12 pt
    return d * 12.0;
}

/**
 * Transform a value with unit (for example "12cm") into a double
 * @param _str the value as string
 * @param atleast was there a + character after the unit to denote an "at-least" property
 * @return the value as double (in points)
 */
double ValueWithLengthUnit( const QString& _str, bool* atleast = NULL );

#endif // _IMPORT_HELPERS_H
