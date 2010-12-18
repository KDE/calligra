/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DOCXXMLENDNOTEREADER_H
#define DOCXXMLENDNOTEREADER_H

#include <DocxXmlDocumentReader.h>

/*! We're deriving from DocxXmlDocumentReader because within the hdr
element, the content of the element is similar to the content of the
body (§17.2.2) element, and contains what is referred to as
block-level markup - markup which can exist as a sibling element to
paragraphs in a WordprocessingML document.
*/
//! A class reading headers
class DocxXmlEndnoteReader : public DocxXmlDocumentReader
{
public:
    explicit DocxXmlEndnoteReader(KoOdfWriters *writers);
    virtual ~DocxXmlEndnoteReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_endnotes();
    KoFilter::ConversionStatus read_endnote();

private:
    void init();
    class Private;
    Private* const d;
};

#endif //DOCXXMLENDNOTEREADER_H
