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

#ifndef EXPORTFILTERSTYLE_H
#define EXPORTFILTERSTYLE_H

//
// ClassExportFilterHtmlStyle (HTML 4.01 Strict, style using CSS2, no style sheets)
//
class ClassExportFilterHtmlStyle : public ClassExportFilterBase
{
    public:
        ClassExportFilterHtmlStyle (void) {}
        virtual ~ClassExportFilterHtmlStyle (void) {}
    public: //virtual
        virtual QString getDocType(void) const;
        virtual QString getBodyOpeningTagExtraAttributes(void) const;
        virtual void ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText);
        virtual QString getStyleElement(void);
        virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
        virtual QString getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout);
};

//
// ClassExportFilterXHtmlStyle (HTML 4.01 Strict, style using CSS2, no style sheets)
//

class ClassExportFilterXHtmlStyle : public ClassExportFilterHtmlStyle
{
    public:
        ClassExportFilterXHtmlStyle (void) {}
        virtual ~ClassExportFilterXHtmlStyle (void) {}
    public: //virtual
        virtual bool isXML(void) const {return true;}
        virtual QString getDocType(void) const;
        virtual QString getStyleElement(void);
};

#endif /* EXPORTFILTERSTYLE_H */
