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

#ifndef EXPORTCSS_H
#define EXPORTCSS_H

#include <QMap>

#include <KWEFBaseWorker.h>
#include "ExportFilter.h"

class StyleMap : public QMap<QString,LayoutData>
{
public:
    StyleMap(void) {}
    ~StyleMap(void) {}
};

class HtmlCssWorker : public HtmlWorker
{
public:
    HtmlCssWorker(void) { }
    virtual ~HtmlCssWorker(void) { }
public:
    virtual bool doOpenStyles(void); ///< HTML's \<style\>
    virtual bool doCloseStyles(void); ///< HTML's \</style\>
    virtual bool doFullDefineStyle(LayoutData& layout);
    virtual bool doFullPaperFormat(const int format,
        const double width, const double height, const int orientation);
    virtual bool doFullPaperBorders (const double top, const double left,
        const double bottom, const double right); ///< Like KWord's \<PAPERBORDERS\>
protected:
    virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
    virtual void openParagraph(const QString& strTag, const LayoutData& layout,
                               QChar::Direction direction=QChar::DirL);
    virtual void closeParagraph(const QString& strTag, const LayoutData& layout);
    virtual void openSpan(const FormatData& formatOrigin, const FormatData& format);
    virtual void closeSpan(const FormatData& formatOrigin, const FormatData& format);
private:
    QString layoutToCss(const LayoutData& layoutOrigin,const LayoutData& layout,
        const bool force) const;
    QString escapeCssIdentifier(const QString& strText) const;
    QString textFormatToCss(const TextFormatting& formatOrigin,
        const TextFormatting& formatData, const bool force) const;
private:
    QString m_strPageSize;
    QString m_strPaperBorders;
    StyleMap m_styleMap;
};

#endif /* EXPORTCSS_H */
