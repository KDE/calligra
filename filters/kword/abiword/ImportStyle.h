/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef _IMPORT_STYLE_H
#define _IMPORT_STYLE_H

#include <q3valuelist.h>
#include <QString>
#include <QMap>

class StyleData
{
public:
    StyleData(void);
public:
    int m_level; // Depth of heading (as defined by AbiWord!)
    QString m_props; // properties
};

class StyleDataMap : public QMap<QString,StyleData>
{
public:
    StyleDataMap(void);
public:
    void defineNewStyle(const QString& strName, const int level, const QString& strProps);
    void defineNewStyleFromOld(const QString& strName, const QString& strOld,
        const int level, const QString& strProps);
    StyleDataMap::Iterator useOrCreateStyle(const QString& strName);
    void defineDefaultStyles(void);
private:
    QString getDefaultStyle(void);
};

#endif // _IMPORT_STYLE_H

