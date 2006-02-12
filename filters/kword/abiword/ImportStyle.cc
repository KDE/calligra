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

#include <qfontinfo.h>

#include <kglobalsettings.h>
#include <kdebug.h>

#include <KoGlobal.h>

#include "ImportStyle.h"

StyleData::StyleData(void): m_level(-1)
{
}

StyleDataMap::StyleDataMap(void)
{
}

QString StyleDataMap::getDefaultStyle(void)
{
    // We use QFontInfo, as it return real values
    QFontInfo fontInfo(KoGlobal::defaultFont());
    QString strReturn;

    strReturn += "font-family:";
    strReturn += fontInfo.family(); // TODO: should be "Times New Roman"
    strReturn += "; font-size: 12pt;";
    // Note: the last property must have a semi-colon!

    return strReturn;
}

void StyleDataMap::defineNewStyleFromOld(const QString& strName, const QString& strOld,
    const int level, const QString& strProps)
{
    if (strOld.isEmpty())
    {
        defineNewStyle(strName,level,strProps);
        return;
    }

    StyleDataMap::Iterator it=find(strOld);
    if (it==end())
    {
        defineNewStyle(strName,level,strProps);
    }
    else
    {
        QString strAllProps=it.data().m_props;
        strAllProps+=strProps;
        defineNewStyle(strName,level,strAllProps);
    }
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
    styleData.m_props+=getDefaultStyle();
    if (!strProps.isEmpty())
    {
        styleData.m_props+=strProps;
        styleData.m_props+=";"; // Security if other properties are appended later
    }
}

StyleDataMap::Iterator StyleDataMap::useOrCreateStyle(const QString& strName)
{
    // We are using a style but we are not sure if it is defined
    StyleDataMap::Iterator it=find(strName);
    if (it==end())
    {
        // The style is not yet defined!
        StyleData data;
        data.m_level=-1;
        data.m_props=getDefaultStyle();
        it=insert(strName,data);
    }
    return it;
}

void StyleDataMap::defineDefaultStyles(void)
{
    // Add a few of AbiWord predefined style sheets
    // AbiWord file: src/text/ptbl/xp/pt_PT_Styles.cpp
    defineNewStyle("Normal",-1,QString::null);
    // TODO: font should be "Arial"
    // TODO: "keep with next"
    QString strHeading("font-weight: bold; margin-top: 22pt; margin-bottom: 3pt; ");
    defineNewStyle("Heading 1",1,strHeading+"font-size: 17pt");
    defineNewStyle("Heading 2",2,strHeading+"font-size: 14pt");
    defineNewStyle("Heading 3",3,strHeading+"font-size: 12pt");
    defineNewStyle("Block Text",-1,"margin-left: 1in; margin-right: 1in; margin-bottom: 6pt");
    QFontInfo fixedInfo(KGlobalSettings::fixedFont());
    QString strPlainText=QString("font-family: %1")
        .arg(fixedInfo.family()); // TODO: should be "Courier New"
    kdDebug(30506) << "Plain Text: " << strPlainText << endl;
    defineNewStyle("Plain Text",-1,strPlainText);
    // TODO: all list and numbered types
}
