/* This file is part of the KDE project

   Copyright (C) 2013 Inge Wallin            <inge@lysator.liu.se>
   Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

// Calligra
#include <KoXmlReader.h>

#include <KoOdfStyleManager.h>
#include <KoOdfStyle.h>
#include <KoOdfListStyle.h>
#include <KoOdfStyleProperties.h>

// This filter
#include "OdfReaderWikiContext.h"
#include "WikiExportDebug.h"


#if 1
#define DEBUG_BACKEND() \
    debugWiki << (reader.isStartElement() ? "start": (reader.isEndElement() ? "end" : "other")) \
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


void OdtReaderWikiBackend::elementTextH(KoXmlStreamReader &reader,
                                        OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    if (reader.isStartElement()) {
        wikiContext->outlineLevel = reader.attributes().value("text:outline-level").toString().toInt();
        outputHeadingLevel(wikiContext);
    }
    else {
        outputHeadingLevel(wikiContext);
        wikiContext->outStream << "\n";

        wikiContext->outlineLevel = 0;
    }
}

void OdtReaderWikiBackend::elementTextP(KoXmlStreamReader &reader,
                                        OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    if (reader.isStartElement()) {
        QString stylename = reader.attributes().value("text:style-name").toString();

        KoOdfStyle *style = wikiContext->styleManager()->style(stylename, "paragraph");
        //Push style to stack
        wikiContext->pushStyle(style);
        outputTextStyle(reader, wikiContext);
    } else {
        outputTextStyle(reader, wikiContext);
        wikiContext->popStyle();

        // At the end of a paragraph, output two newlines.
        wikiContext->outStream << "\n";
    }
}

// ----------------------------------------------------------------
// Paragraph level functions: spans, annotations, notes, text content itself, etc.

void OdtReaderWikiBackend::elementTextSpan(KoXmlStreamReader &reader,
                                           OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    if (reader.isStartElement()) {
        QString stylename = reader.attributes().value("text:style-name").toString();
        KoOdfStyle *style = wikiContext->styleManager()->style(stylename, "text");
        //Push style to stack
        wikiContext->pushStyle(style);

        outputTextStyle(reader, wikiContext);
    }
    else {
        outputTextStyle(reader, wikiContext);
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
            wikiContext->pushListStyle(listStyle);
        }
        wikiContext->listLevelCounter++;
    }
    else {
        if (wikiContext->listLevelCounter == wikiContext->listStyleStack.count()) {
            wikiContext->popListStyle();
        }
        wikiContext->listLevelCounter--;
    }
}

void OdtReaderWikiBackend::elementTextS(KoXmlStreamReader &reader,
                                        OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    // Find out number of spaces.
    QString dummy = reader.attributes().value("text:c").toString();
    bool ok;
    quint32  numSpaces = dummy.toUInt(&ok);
    if (!ok) 
        numSpaces = 1;

    // Output the required number of spaces.
    for (quint32 i = 0; i < numSpaces; ++i) {
        wikiContext->outStream << ' ';
    }
}

void OdtReaderWikiBackend::elementTextListItem(KoXmlStreamReader &reader,
                                               OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    if (reader.isStartElement()) {
        KoOdfListStyle *listStyle = wikiContext->popListStyle();
        char symbol;
        if (listStyle->listLevelStyleType() == "text:list-level-style-bullet") {
            symbol = '*';
        }
        else if (listStyle->listLevelStyleType() == "text:list-level-style-number") {
            symbol = '#';
        }
        wikiContext->pushListStyle(listStyle);

        for (int level = 0; level < wikiContext->listLevelCounter; ++level) {
            wikiContext->outStream << symbol;
        }
        wikiContext->outStream << ' ';
    }
    else {
        wikiContext->outStream << '\n';
    }

}

void OdtReaderWikiBackend::characterData(KoXmlStreamReader &reader, OdfReaderContext *context)
{
    DEBUG_BACKEND();
    OdfReaderWikiContext *wikiContext = dynamic_cast<OdfReaderWikiContext*>(context);
    if (!wikiContext) {
        return;
    }

    wikiContext->outStream << reader.text().toString();
}


// ----------------------------------------------------------------
//                         private functions


void OdtReaderWikiBackend::outputTextStyle(KoXmlStreamReader &reader,
                                           OdfReaderWikiContext *wikiContext)
{
    KoOdfStyle *style = wikiContext->popStyle();
    KoOdfStyleProperties *styleProperties = style->properties().value("style:text-properties");
    if (!styleProperties) {
        wikiContext->pushStyle(style);
        return;
    }

    // Output italic and bold.
    QString fontWeightProperty = "fo:font-weight";
    QString fontStyleProperty = "fo:font-style";
    if ((styleProperties->attribute(fontWeightProperty) == "bold")
        && (styleProperties->attribute(fontStyleProperty) == "italic"))
    {
        wikiContext->outStream << "'''''";
    } else if (styleProperties->attribute(fontWeightProperty) == "bold") {
        wikiContext->outStream << "'''";
    } else if (styleProperties->attribute(fontStyleProperty) == "italic") {
        wikiContext->outStream << "''";
    }

    QString textPositionProperty = "style:text-position";
    QString textLineThroughProperty = "style:text-line-through-style";
    if (reader.isStartElement()) {
        // Output strikeout text.
        if (styleProperties->attribute(textLineThroughProperty) == "solid") {
            wikiContext->outStream << "<s>";
        }
        // Output sub and super script.
        if (styleProperties->attribute(textPositionProperty) == "sub") {
           wikiContext->outStream << "<sub>";
        }
        else if (styleProperties->attribute(textPositionProperty) == "super") {
            wikiContext->outStream << "<sup>";
        }
    }
    else {
        if (styleProperties->attribute(textLineThroughProperty)== "solid") {
            wikiContext->outStream << "</s>";
        }
        if (styleProperties->attribute(textPositionProperty) == "sub") {
           wikiContext->outStream << "</sub>";
        }
        else if (styleProperties->attribute(textPositionProperty) == "super") {
            wikiContext->outStream << "</sup>";
        }
    }
    wikiContext->pushStyle(style);
}

void OdtReaderWikiBackend::outputHeadingLevel(OdfReaderWikiContext *wikiContext)
{
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
