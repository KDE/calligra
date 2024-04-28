/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODTREADERASCIIBACKEND_H
#define ODTREADERASCIIBACKEND_H

// Calligra
#include <KoFilter.h>
#include <KoXmlStreamReader.h>
#include <OdfTextReaderBackend.h>

class OdfReaderContext;

class OdtReaderAsciiBackend : public OdfTextReaderBackend
{
public:
    explicit OdtReaderAsciiBackend();
    ~OdtReaderAsciiBackend() override;

    // Called before and after the actual traversal.
    // FIXME: NYI

    // Called when there is a document level element with children.

    // ----------------------------------------------------------------
    // Text level functions: paragraphs, headings, sections, frames, objects, etc

    void elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    // ----------------------------------------------------------------
    // Paragraph level functions: spans, annotations, notes, text content itself, etc.

    void elementTextLineBreak(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    void characterData(KoXmlStreamReader &reader, OdfReaderContext *context) override;
};

#endif // ODTREADERASCIIBACKEND_H
