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

#include "ImportStyle.h"

StyleData::StyleData(void): m_level(-1)
{
}

StyleDataMap::StyleDataMap(void)
{
}

void StyleDataMap::defineNewStyle(const QString& strName, const int level,
    const QString& strProps)
{
    // Despite its name, this method can be called multiple times
    // We must take care that it is still "stable" after that.
    StyleDataMap::Iterator it=find(strName);
    if (it==end())
    {
        // The style does not exist yet, so we must define it.
        it=insert(strName,StyleData());
    }
    StyleData& styleData=it.data();
    styleData.m_level=level;
    styleData.m_props+=strProps;
    styleData.m_props+=";"; // Security if other properties are appended later
}
