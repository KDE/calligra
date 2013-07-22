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

#ifndef ODTREADERWIKIBACKEND_H
#define ODTREADERWIKIBACKEND_H

// Calligra
#include <KoXmlStreamReader.h>
#include <KoFilter.h>
#include <OdfTextReaderBackend.h>


class QByteArray;
class QSizeF;
class QStringList;
class KoStore;
class OdfReaderContext;
class OdfReaderWikiContext;


class OdtReaderWikiBackend : public OdfTextReaderBackend
{
 public:
    explicit OdtReaderWikiBackend();
    virtual ~OdtReaderWikiBackend();

    // ----------------------------------------------------------------
    // Text level functions: paragraphs, headings, sections, frames, objects, etc

    void elementTextH(KoXmlStreamReader &reader, OdfReaderContext *context);
    void elementTextP(KoXmlStreamReader &reader, OdfReaderContext *context);
    void elementTextList(KoXmlStreamReader &reader, OdfReaderContext *context);

    // ----------------------------------------------------------------
    // Paragraph level functions: spans, annotations, notes, text content itself, etc.

    void elementTextSpan(KoXmlStreamReader &reader, OdfReaderContext *context);
    void elementTextS(KoXmlStreamReader &reader, OdfReaderContext *context);

    void elementTextListItem(KoXmlStreamReader &reader, OdfReaderContext *context);

    void characterData(KoXmlStreamReader &reader, OdfReaderContext *context);

private:
    void checkFontStyle(OdfReaderWikiContext *wikiContext);
    void checkheadingLevel(OdfReaderWikiContext *wikiContext);
    void checkTextIndention(OdfReaderWikiContext *wikiContext);


};


#endif // ODTREADERWIKIBACKEND_H
