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

#ifndef EXPORTFILTERFULLPOWER_H
#define EXPORTFILTERFULLPOWER_H

class ClassExportFilterHtml : public KWEFBaseClass
{
    public:
        ClassExportFilterHtml (void) {}
        virtual ~ClassExportFilterHtml (void) {}
    public:
        inline bool isXML  (void) const { return m_xml; }
        inline bool isUTF8 (void) const { return m_utf8; }
        inline void setXML (const bool flag ) { m_xml=flag; }
        inline void setUTF8 (const bool flag ) { m_utf8=flag; }
        bool filter(const QString  &filenameIn, const QString  &filenameOut);
    public: //virtual
        virtual QString escapeText(const QString& str) const;
        virtual QString getHtmlOpeningTagExtraAttributes(void) const;
        virtual QString getBodyOpeningTagExtraAttributes(void) const;
        virtual void ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText);
        virtual QString processDocTagStylesOnly(QDomElement myNode);
        virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
        virtual QString getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout);
        virtual void processStyleTag (QDomNode myNode, void * tagData, QString &strStyles);
    protected:
        QString escapeCssIdentifier(const QString& strText) const;
        QString layoutToCss(LayoutData& layout) const;
        QString getDocType(void) const;
    protected:
        virtual void helpStyleProcessing(QDomNode myNode,LayoutData* layout);
    private:
        bool m_utf8;
        bool m_xml;
};

#endif /* EXPORTFILTERFULLPOWER_H */
