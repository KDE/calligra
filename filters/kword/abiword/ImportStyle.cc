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

#include <qfontinfo.h>

#include <koGlobal.h>

#include "ImportStyle.h"

StyleData::StyleData(void): m_level(-1)
{
}

StyleDataMap::StyleDataMap(void)
{
}

QString StyleDataMap::getDefaultStyle(void)
{
    // We use QFontInfo, as it does not return -1 as point size
    QFontInfo fontInfo(KoGlobal::defaultFont());
    QString strReturn;

    // As something is upsetting KWord's style manager, we must define everything!
    strReturn += "font-family:";
    strReturn += fontInfo.family();
    strReturn += "; font-size:";
    strReturn += QString::number(fontInfo.pointSize());
    strReturn += "pt;";

    //strReturn += "font-style:normal; font-weight:normal; text-decoration:none; color:000000; bgcolor: FFFFFF;";
    
    // Note: the last property must have a semi-colon!

    return strReturn;
}

void StyleDataMap::defineNewStyle(const QString& strName, const int level,
    const QString& strProps)
{
    // Despite its name, this method can be called multiple times
    // We must take care that KWord just gets it only one time.
    StyleDataMap::Iterator it=find(strName);
    if (it==end())
    {
        // The style does not exist yet, so we must define it.
        it=insert(strName,StyleData());
    }
    StyleData& styleData=it.data();
    styleData.m_level=level;
    // We must add the default style, as KWord's style manager is sensitive
    styleData.m_props+=getDefaultStyle();
    styleData.m_props+=strProps;
    styleData.m_props+=";"; // Security if other properties are appended later
}

StyleDataMap::Iterator StyleDataMap::useOrCreateStyle(const QString& strName)
{
    // We are using a style but we ar enot sure if it is defined
    StyleDataMap::Iterator it=find(strName);
    if (it==end())
    {
        // The style is not yet defined!
        StyleData data;
        data.m_level=-1;
        // We must add the default style, as KWord's style manager is sensitive
        data.m_props=getDefaultStyle();
        it=insert(strName,data);
    }
    return it;
}
