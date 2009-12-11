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

#ifndef DOCXXMLSTYLESREADER_H
#define DOCXXMLSTYLESREADER_H

#include <MsooXmlReader.h>

#include <KoGenStyles.h>

//! A class reading MSOOXML DOCX markup - styles.xml part.
/*! @todo generalize for other MSOOXML subformats.
 */
class DocxXmlStylesReader : public MSOOXML::MsooXmlReader
{
public:
    DocxXmlStylesReader(KoOdfWriters *writers);

    DocxXmlStylesReader(QIODevice* io, KoOdfWriters *writers);

    virtual ~DocxXmlStylesReader();

    //! Reads/parses the file. The output goes mainly to KoGenStyles* KoOdfWriters::mainStyles
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_docDefaults();
    KoFilter::ConversionStatus read_pPrDefault();
    KoFilter::ConversionStatus read_rPrDefault();
    KoFilter::ConversionStatus read_pPr();
    KoFilter::ConversionStatus read_rPr();
    KoFilter::ConversionStatus read_lang();
    KoFilter::ConversionStatus read_rFonts();

    KoGenStyle m_defaultParagraphStyle;

    typedef KoFilter::ConversionStatus(DocxXmlStylesReader::*ReadMethod)();
    QStack<ReadMethod> m_calls;

};

#endif
