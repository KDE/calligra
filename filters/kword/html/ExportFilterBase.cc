// $Header$

/*
   This file is part of the KDE project
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

#include "ExportFilterBase.h"

// Most of the code is in htmlexport.cc

void CreateMissingFormatData(QString &paraText, ValueListFormatData &paraFormatDataList)
{
    ValueListFormatData::Iterator  paraFormatDataIt;
    int lastPos=0; // last position

    paraFormatDataIt = paraFormatDataList.begin ();
    while (paraFormatDataIt != paraFormatDataList.end ())
    {
        if ((*paraFormatDataIt).pos>lastPos)
        {
            //We must add a FormatData
            FormatData formatData(lastPos,(*paraFormatDataIt).pos-lastPos);
            formatData.missing=true;
            paraFormatDataList.insert(paraFormatDataIt,formatData);
        }
        lastPos=(*paraFormatDataIt).pos+(*paraFormatDataIt).realLen;
        paraFormatDataIt++; // To the next one, please!
    }
    // Add the last one if needed
    if ((int)paraText.length()>lastPos)
    {
        FormatData formatData(lastPos,paraText.length()-lastPos);
        formatData.missing=true;
        paraFormatDataList.append(formatData);
    }
}

bool ClassExportFilterBase::isXML(void) const
{
    return false;
}

QString ClassExportFilterBase::getStyleElement(void)
{
    return QString::null; //Default is no style
}
