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

#ifndef KWEF_BASECLASS_H
#define KWEF_BASECLASS_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qdom.h>

#include <KWEFStructures.h>

class KWEFBaseClass
{
    public:
        KWEFBaseClass(void) : inList(false) {}
        virtual ~KWEFBaseClass(void) {}
    public: //Non-virtual
    public: //virtual
//        virtual bool filter(const QString &filenameIn, const QString &filenameOut);
//        virtual QString escapeText(const QString& str) const;
        virtual QString getHtmlOpeningTagExtraAttributes(void) const;
//        virtual QString getDocType(void) const;
        virtual QString getBodyOpeningTagExtraAttributes(void) const;
        virtual void ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText);
        virtual QString processDocTagStylesOnly(QDomElement myNode);
        virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered);
        virtual QString getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout);
        virtual void processStyleTag (QDomNode myNode, void * tagData, QString &strStyles);
    protected:
        virtual void helpStyleProcessing(QDomNode myNode,LayoutData* layout);
    public: // Public variables
        bool inList; // Are we currently in a list?
        bool orderedList; // Is the current list ordered or not (undefined, if we are not in a list)
        CounterData::Style typeList; // What is the style of the current list (undefined, if we are not in a list)
    protected:
        QDomDocument qDomDocumentIn;
};

#endif /* KWEF_BASECLASS_H */
