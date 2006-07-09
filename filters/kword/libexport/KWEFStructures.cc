/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (c) 2001 IABG mbH. All rights reserved.
                      Contact: Wolf-Michael Bolle <Bolle@IABG.de>

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

/*
   Part of the code is based on code licensed under the terms of the
   GNU Library General Public License version 2:
   Copyright 2001 Michael Johnson <mikej@xnet.com>
*/

#include <kdebug.h>

#include "KWEFStructures.h"


TableCell::~TableCell ( void )
{
#if 0
    if ( paraList) delete paraList;
#endif
}

void Table::addCell ( int c, int r, int _cols, int _rows, QValueList<ParaData> &p, FrameData &frameData )
{
   if ( c + _cols > cols )
   {
      cols = c + _cols;
   }

   cellList << TableCell ( c, r, _cols, _rows, new QValueList<ParaData> (p), frameData );
}

bool TabulatorData::operator == (const TabulatorData& other) const
{
    return ((m_type==other.m_type)
        && (m_ptpos==other.m_ptpos)
        && (m_filling==other.m_filling)
        && (m_width==other.m_width));
}

void VariableData::setLink(const QString& linkName, const QString& hrefName)
{
    propertyMap["link:linkName"]=linkName;
    propertyMap["link:hrefName"]=hrefName;
}

QString VariableData::getLinkName(void) const
{
    return propertyMap["link:linkName"];
}

QString VariableData::getHrefName(void) const
{
    return propertyMap["link:hrefName"];
}

void VariableData::setPgNum(const QString& subtype, const QString& value)
{
    propertyMap["pgnum:subtype"]=subtype;
    propertyMap["pgnum:value"]=value;
}

bool VariableData::isPageNumber(void) const
{
    const int num=propertyMap["pgnum:subtype"].toInt();
    return (num==0);
}

bool VariableData::isPageCount(void) const
{
    const int num=propertyMap["pgnum:subtype"].toInt();
    return (num==1);
}

void VariableData::setField(const QString& name, const QString& value)
{
    propertyMap["field:name"]=name;
    propertyMap["field:value"]=value;
}

QString VariableData::getFieldName(void) const
{
    return propertyMap["field:name"];
}

QString VariableData::getFieldValue(void) const
{
    return propertyMap["field:value"];
}

void VariableData::setFootnote( const QString& notetype, const QString& automatic,const QString& value, QValueList<ParaData>* para)
{
    propertyMap["footnote:value"] = value;
    propertyMap["footnote:auto"]  = automatic;
    propertyMap["footnote:notetype"]  = notetype;
    footnotePara = para;
}

bool VariableData::getFootnoteAuto(void) const
{
    return propertyMap["footnote:auto"]=="auto";
}

QString VariableData::getFootnoteValue(void) const
{
    return propertyMap["footnote:value"];
}

bool VariableData::getFootnoteType( void ) const
{
    return propertyMap["footnote:notetype"] == "footnote";
}
 
QValueList<ParaData>* VariableData::getFootnotePara(void) const
{
    return footnotePara;
}

void VariableData::setGenericData( const QString& key, const QString& data )
{
    propertyMap[ key ] = data ;
}

QString VariableData::getGenericData( const QString& key ) const
{
    return propertyMap[ key ];
}

Bookmark::Bookmark()
 : m_startparag(-1), m_endparag(-1), m_cursorIndexStart(-1), m_cursorIndexEnd(-1)
{
}

void CreateMissingFormatData (QString &paraText, ValueListFormatData &paraFormatDataList)
{
    ValueListFormatData::Iterator  paraFormatDataIt;
    int lastPos=0;   // last position

    for (paraFormatDataIt = paraFormatDataList.begin ();
        paraFormatDataIt != paraFormatDataList.end ();
        paraFormatDataIt++)
    {
        if ( (*paraFormatDataIt).pos > lastPos )
        {
#if 0
            kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                            << ", pos = " << (*paraFormatDataIt).pos
                            << ", len = " << (*paraFormatDataIt).len << " (bad)" << endl;
#endif

            // We must add a FormatData
            paraFormatDataList.insert ( paraFormatDataIt,
                FormatData ( lastPos, (*paraFormatDataIt).pos - lastPos, true ) );
        }
#if 0
        else
        {
            kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                            << ", pos = " << (*paraFormatDataIt).pos
                            << ", len = " << (*paraFormatDataIt).len << " (ok)" << endl;
        }
#endif


        /* A check for pos < lastPos might be interesting at this point */


        lastPos = (*paraFormatDataIt).pos + (*paraFormatDataIt).len;

    }

    // Add the last one if needed
    if ( (int) paraText.length () > lastPos )
    {
#if 0
        kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                        << ", total len = " << paraText.length () << " (bad)" << endl;
#endif

        paraFormatDataList.append (
            FormatData ( lastPos, paraText.length () - lastPos, true ) );
    }
#if 0
    else
    {
        kdDebug (30508) << "CreateMissingFormatData: lastPos = " << lastPos
                        << ", total len = " << paraText.length () << " (ok)" << endl;
    }
#endif
}
