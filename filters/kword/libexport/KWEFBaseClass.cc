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

#include <qdom.h>

#include "KWEFStructures.h"
#include "KWEFBaseClass.h"

void KWEFBaseClass::processStyleTag (QDomNode, void *, QString&)
{
}

void KWEFBaseClass::helpStyleProcessing(QDomNode myNode,LayoutData* layout)
{
}

void KWEFBaseClass::ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText)
{
}

QString KWEFBaseClass::getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)
{
    return QString::null;
}

QString KWEFBaseClass::getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout)
{
    return QString::null;
}

