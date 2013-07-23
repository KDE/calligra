/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin            <inge@lysator.liu.se>

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
   Boston, MA 02110-1301, USA.
*/


// Own
#include "OdtReaderWikiBackend.h"

// KDE
#include "kdebug.h"

// Calligra
#include <KoXmlReader.h>
#include <KoUnit.h>

#include <KoOdfStyleManager.h>
#include <KoOdfStyle.h>
#include <KoOdfListStyle.h>
#include <KoOdfStyleProperties.h>

// This filter
#include "OdfReaderWikiContext.h"


#if 1
#define DEBUG_BACKEND() \
    kDebug(30503) << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
    << reader.qualifiedName().toString()
#else
#define DEBUG_BACKEND() \
    //NOTHING
#endif


// ================================================================
//                 class OdtReaderWikiBackend


OdtReaderWikiBackend::OdtReaderWikiBackend()
    : OdfTextReaderBackend()
{
}

OdtReaderWikiBackend::~OdtReaderWikiBackend()
{
}


// ----------------------------------------------------------------
// Text level functions: paragraphs, headings, sections, frames, objects, etc


void OdtReaderWikiBackend::elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    if (reader.isStartElement()) {
        wikiContext->outlineLevel = reader.attributes().value("text:outline-level").toString().toInt();
        checkheadingLevel(reader, wikiContext);
    }
    else {
        checkheadingLevel(reader, wikiContext);
        wikiContext->outStream << "\n";
    }
}

void OdtReaderWikiBackend::elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }
    if (reader.isStartElement()) {
        QString stylename = reader.attributes().value("text:style-name").toString();
        KoOdfStyle *style = wikiContext->styleManager()->style(stylename);
        //Push style to stack
        wikiContext->pushStyle(style);

        checkTextIndention(reader, wikiContext);
        checkTextStyle(reader, wikiContext);
    } else {
        checkTextStyle(reader, wikiContext);
        wikiContext->popStyle();
        // At the end of a paragraph, output two newlines.
        wikiContext->outStream << "\n";
    }
}

// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, text content itself, etc.

void OdtReaderWikiBackend::elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }
    if (reader.isStartElement()) {
        QString stylename = reader.attributes().value("text:style-name").toString();
        KoOdfStyle *style = wikiContext->styleManager()->style(stylename);
        //Push style to stack
        wikiContext->pushStyle(style);

        checkTextStyle(reader, wikiContext);
    }
    else {
        checkTextStyle(reader, wikiContext);
        wikiContext->popStyle();
    }
}

void OdtReaderWikiBackend::elementTextList(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    if (reader.isStartElement()) {
        QString stylename = reader.attributes().value("text:style-name").toString();
        KoOdfListStyle *listStyle = wikiContext->styleManager()->listStyle(stylename);
        if (listStyle) {
            kDebug() << "List style name:" << stylename <<"List style level type :" <<listStyle->listLevelStyleType();
            wikiContext->pushListStyle(listStyle);
        }
            wikiContext->listLevelCounter++;
            kDebug() << "List level counter:" << wikiContext->listLevelCounter;
    }
    else {
        if (wikiContext->listLevelCounter == wikiContext->listStyleStack.count()) {
                    wikiContext->popListStyle();
        }
        wikiContext->listLevelCounter--;
        kDebug() << "List level counter:" << wikiContext->listLevelCounter;
    }
}

void OdtReaderWikiBackend::elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

#if 0
    QString dummy = element.attribute("text:c", "1");
    bool ok;
    int  numSpaces = dummy.toUInt(&ok);
    if (!ok) 
        numSpaces = 1;

    // At the end of a paragraph, output two newlines.
    wikiContext->outStream << "\n\n";
#endif
}

void OdtReaderWikiBackend::elementTextListItem(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }
    if (reader.isStartElement()) {
        KoOdfListStyle *listStyle = wikiContext->popListStyle();
        QString symbol;
        if (listStyle->listLevelStyleType() == "text:list-level-style-bullet") {
            symbol = "*";
        }
        else if (listStyle->listLevelStyleType() == "text:list-level-style-number") {
            symbol = "#";
        }
        wikiContext->pushListStyle(listStyle);

        for (int level = 0; level < wikiContext->listLevelCounter; level++) {
            wikiContext->outStream << symbol.toUtf8();
        }
        wikiContext->outStream << " ";
    }
    else {
        wikiContext->outStream <<"\n";
    }

}

void OdtReaderWikiBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }
    //kDebug(30503) << reader.text().toString();

    wikiContext->outStream << reader.text().toString();
}

void OdtReaderWikiBackend::checkTextStyle(KoXmlStreamReader &reader, OdfReaderWikiContext *wikiContext)
{
    KoOdfStyle *style = wikiContext->popStyle();
    KoOdfStyleProperties *stylePropertis = style->properties().value("style:text-properties");
    if (!stylePropertis) {
        wikiContext->pushStyle(style);
        return;
    }

    // Check italic and bold.
    QString fontWeightProperty = "fo:font-weight";
    QString fontStyleProperty = "fo:font-style";
    if ((stylePropertis->attribute(fontWeightProperty) == "bold") &&
            (stylePropertis->attribute(fontStyleProperty) == "italic")) {
        wikiContext->outStream << "'''''";
    } else if (stylePropertis->attribute(fontWeightProperty) == "bold") {
        wikiContext->outStream << "'''";
    } else if (stylePropertis->attribute(fontStyleProperty) == "italic") {
        wikiContext->outStream << "''";
    }

    QString textPositionProperty = "style:text-position";
    QString textLineThroughProperty = "style:text-line-through-style";
    if (reader.isStartElement()) {
        // Check strike text.
        if (stylePropertis->attribute(textLineThroughProperty) == "solid") {
            wikiContext->outStream << "<s>";
        }
        // Check sub and super script.
        if (stylePropertis->attribute(textPositionProperty) == "sub") {
           wikiContext->outStream << "<sub>";
        }
        else if (stylePropertis->attribute(textPositionProperty) == "super") {
            wikiContext->outStream << "<sup>";
        }
    }
    else {
        if (stylePropertis->attribute(textLineThroughProperty)== "solid") {
            wikiContext->outStream << "</s>";
        }
        if (stylePropertis->attribute(textPositionProperty) == "sub") {
           wikiContext->outStream << "</sub>";
        }
        else if (stylePropertis->attribute(textPositionProperty) == "super") {
            wikiContext->outStream << "</sup>";
        }
    }
    wikiContext->pushStyle(style);
}

void OdtReaderWikiBackend::checkheadingLevel(KoXmlStreamReader &reader, OdfReaderWikiContext *wikiContext)
{
     Q_UNUSED(reader);
    int level = wikiContext->outlineLevel;
    if (level == 1) {
        wikiContext->outStream << "==";
    }
    else if (level == 2) {
        wikiContext->outStream << "===";
    }
    else if (level == 3) {
        wikiContext->outStream  << "====";
    }
}

void OdtReaderWikiBackend::checkTextIndention(KoXmlStreamReader &reader, OdfReaderWikiContext *wikiContext)
{
    Q_UNUSED(reader);
    KoOdfStyle *style = wikiContext->popStyle();
    // Check indenting text.
    KoOdfStyleProperties *styleProperies = style->properties().value("style:paragraph-properties");
    QString property = "fo:margin-left";
    if (!styleProperies->attribute(property).isEmpty()) {
        // FIXME: a BIG fixme i am not SURE that here i have done the right work.
        int indention = KoUnit::parseValue(styleProperies->attribute(property));
        if ((indention % 10) > 5) {
            indention = (indention / 10) + 1;
        }
        else {
            indention = indention / 10;
        }
        kDebug() << "Indention:" << indention << KoUnit::parseValue(styleProperies->attribute(property));
        for(int indent = 0; indent < indention; indent++) {
            wikiContext->outStream << ":";
        }
    }
    wikiContext->pushStyle(style);
}
