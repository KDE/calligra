/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef DOCXXMLFONTTABLEREADER_H
#define DOCXXMLFONTTABLEREADER_H

#include <KoFontFace.h>
#include <MsooXmlReader.h>

class KoGenStyles;

//! A context structure for DocxXmlFontTableReader
class DocxXmlFontTableReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    explicit DocxXmlFontTableReaderContext(KoGenStyles &_styles);
    KoGenStyles *styles;
};

//! A class reading MSOOXML DOCX markup - fontTable.xml part.
/*! Information about the fonts used in the document.
    See ECMA-376, 17.8 Fonts.
 */
class DocxXmlFontTableReader : public MSOOXML::MsooXmlReader
{
public:
    explicit DocxXmlFontTableReader(KoOdfWriters *writers);

    DocxXmlFontTableReader(QIODevice *io, KoOdfWriters *writers);

    ~DocxXmlFontTableReader() override;

    //! Reads/parses the file. The output goes mainly to KoGenStyles* KoOdfWriters::mainStyles
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus read_fonts();
    KoFilter::ConversionStatus read_font();
    KoFilter::ConversionStatus read_family();
    KoFilter::ConversionStatus read_pitch();

    DocxXmlFontTableReaderContext *m_context;

    KoFontFace m_currentFontFace;
};

#endif
