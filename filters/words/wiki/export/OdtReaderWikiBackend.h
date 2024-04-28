/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2013 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ODTREADERWIKIBACKEND_H
#define ODTREADERWIKIBACKEND_H

// Calligra
#include <KoFilter.h>
#include <KoXmlStreamReader.h>
#include <OdfTextReaderBackend.h>

class OdfReaderContext;
class OdfReaderWikiContext;

class OdtReaderWikiBackend : public OdfTextReaderBackend
{
public:
    explicit OdtReaderWikiBackend();
    ~OdtReaderWikiBackend() override;

    // ----------------------------------------------------------------
    // Text level functions: paragraphs, headings, sections, frames, objects, etc

    void elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextList(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    // ----------------------------------------------------------------
    // Paragraph level functions: spans, annotations, notes, text content itself, etc.

    void elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context) override;
    void elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    void elementTextListItem(KoXmlStreamReader &reader, OdfReaderContext *context) override;

    void characterData(KoXmlStreamReader &reader, OdfReaderContext *context) override;

private:
    void outputTextStyle(KoXmlStreamReader &reader, OdfReaderWikiContext *wikiContext);
    void outputHeadingLevel(OdfReaderWikiContext *wikiContext);
};

#endif // ODTREADERWIKIBACKEND_H
