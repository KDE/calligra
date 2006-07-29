/* This file is part of the KDE project
 Copyright (C) 2003 Alexander Dymo <adymo@mksat.net>

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
#ifndef COMMDEFS_H
#define COMMDEFS_H

#include <qapplication.h>
#include <qcanvas.h>
#include <qstringlist.h>

namespace Kudesigner
{

enum RttiValues {
    Rtti_Box = 1700,
    Rtti_Section = 1701,
    Rtti_Band = 1702,
    Rtti_KugarTemplate = 1800,
    Rtti_ReportHeader = 1801,
    Rtti_PageHeader = 1802,
    Rtti_DetailHeader = 1803,
    Rtti_Detail = 1804,
    Rtti_DetailFooter = 1805,
    Rtti_PageFooter = 1806,
    Rtti_ReportFooter = 1807,
    Rtti_TextBox = 2000,
    Rtti_ReportItem = 2001,
    Rtti_Label = 2002,
    Rtti_Field = 2003,
    Rtti_Special = 2004,
    Rtti_Calculated = 2005,
    Rtti_Line = 2006
};

QString rttiName( int rtti );

class Config
{
public:
    static int holderSize()
    {
        return m_holderSize;
    }
    static void setHolderSize( int holderSize )
    {
        m_holderSize = holderSize;
    }
    static int gridSize();
    static void setGridSize( int gridSize );

private:
    static int m_holderSize;
    static int m_gridSize;
};

}

//Holder size for report items. TODO: configurable
const int HolderSize = 6;

const int DefaultItemWidth = 50;
const int DefaultItemHeight = 20;

#endif
