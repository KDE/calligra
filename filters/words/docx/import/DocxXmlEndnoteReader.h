/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
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
    ~DocxXmlEndnoteReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus read_endnotes();
    KoFilter::ConversionStatus read_endnote();

private:
    void init();
    class Private;
    Private *const d;
};

#endif // DOCXXMLENDNOTEREADER_H
