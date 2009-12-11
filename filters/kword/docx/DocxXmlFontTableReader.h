/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
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
    DocxXmlFontTableReaderContext(KoGenStyles& _styles);
    KoGenStyles* styles;
};

//! A class reading MSOOXML DOCX markup - fontTable.xml part.
/*! Information about the fonts used in the document.
    See ECMA-376, 17.8 Fonts.
 */
class DocxXmlFontTableReader : public MSOOXML::MsooXmlReader
{
public:
    DocxXmlFontTableReader(KoOdfWriters *writers);

    DocxXmlFontTableReader(QIODevice* io, KoOdfWriters *writers);

    virtual ~DocxXmlFontTableReader();

    //! Reads/parses the file. The output goes mainly to KoGenStyles* KoOdfWriters::mainStyles
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_fonts();
    KoFilter::ConversionStatus read_font();
    KoFilter::ConversionStatus read_family();
    KoFilter::ConversionStatus read_pitch();

    DocxXmlFontTableReaderContext* m_context;

    KoFontFace m_currentFontFace;

    typedef KoFilter::ConversionStatus(DocxXmlFontTableReader::*ReadMethod)();
    QStack<ReadMethod> m_calls;

};

#endif
