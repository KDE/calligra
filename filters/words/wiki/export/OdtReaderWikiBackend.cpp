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

#include <KoOdfStyleManager.h>
#include <KoOdfStyle.h>
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
        QString stylename = reader.attributes().value("text:style-name").toString();
        KoOdfStyle *style = wikiContext->styleManager()->style(stylename);
        // Check out-line-level
        if (style->parent() == "Head_202") {
            wikiContext->outStream << "==";
        } else if (style->parent() == "Head_203") {
            wikiContext->outStream << "===";
        }

        //Push style to stack.
        wikiContext->pushStyle(style);
    }
    else {
        KoOdfStyle *style = wikiContext->popStyle();
        // Check out-line-level
        if (style->parent() == "Head_202") {
            wikiContext->outStream << "==";
        } else if (style->parent() == "Head_203") {
            wikiContext->outStream << "===";
        }
        // FIXME: Should do sth for level 1 .
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
        // FIXME: No style handleing yet.
    } else {
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
        // Check font weight and style
        KoOdfStyleProperties *stylePropertis = style->properties().value("style:text-properties");
        QString weightProperty = "fo:font-weight";
        QString styleProperty = "fo:font-style";
        if ((stylePropertis->attribute(weightProperty) == "bold") &&
                (stylePropertis->attribute(styleProperty) == "italic")) {
            wikiContext->outStream << "'''''";
        } else if (stylePropertis->attribute(weightProperty) == "bold") {
            wikiContext->outStream << "'''";
        } else if (stylePropertis->attribute(styleProperty) == "italic") {
            wikiContext->outStream << "''";
        }
        //Push style to stack
        wikiContext->pushStyle(style);
    }
    else {
        KoOdfStyle *style = wikiContext->popStyle();
        // Check font weight and style
        KoOdfStyleProperties *stylePropertis = style->properties().value("style:text-properties");
        QString weightProperty = "fo:font-weight";
        QString styleProperty = "fo:font-style";
        if ((stylePropertis->attribute(weightProperty) == "bold") &&
                (stylePropertis->attribute(styleProperty) == "italic")) {
            wikiContext->outStream << "'''''";
        } else if (stylePropertis->attribute(weightProperty) == "bold") {
            wikiContext->outStream << "'''";
        } else if (stylePropertis->attribute(styleProperty) == "italic") {
            wikiContext->outStream << "''";
        }
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
