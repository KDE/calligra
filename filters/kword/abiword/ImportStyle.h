// $Header$

/* This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#ifndef _IMPORT_STYLE_H
#define _IMPORT_STYLE_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qmap.h>

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
    StyleDataMap::Iterator useOrCreateStyle(const QString& strName);
};

#endif // _IMPORT_STYLE_H

