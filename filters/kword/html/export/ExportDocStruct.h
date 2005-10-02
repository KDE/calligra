/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef EXPORTDOCSTRUCT_H
#define EXPORTDOCSTRUCT_H

#include <KWEFBaseWorker.h>
#include "ExportFilter.h"

class HtmlDocStructWorker : public HtmlWorker
{
public:
    HtmlDocStructWorker(void) { }
    virtual ~HtmlDocStructWorker(void) { }
public:
protected:
    virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
    virtual void openParagraph(const QString& strTag,
        const LayoutData& layout,QChar::Direction direction=QChar::DirL);
    virtual void closeParagraph(const QString& strTag,
        const LayoutData& layout);
    virtual void openSpan(const FormatData& formatOrigin, const FormatData& format);
    virtual void closeSpan(const FormatData& formatOrigin, const FormatData& format);
private:
    void openFormatData(const FormatData& formatOrigin,
        const FormatData& format, const bool force, const bool allowBold);
    void closeFormatData(const FormatData& formatOrigin,
        const FormatData& format, const bool force, const bool allowBold);
};

#endif /* EXPORTDOCSTRUCT_H */
