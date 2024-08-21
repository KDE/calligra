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

#ifndef DOCXXMLHEADERREADER_H
#define DOCXXMLHEADERREADER_H

#include <DocxXmlDocumentReader.h>

/*! We're deriving from DocxXmlDocumentReader because within the hdr
element, the content of the element is similar to the content of the
body (§17.2.2) element, and contains what is referred to as
block-level markup - markup which can exist as a sibling element to
paragraphs in a WordprocessingML document.
*/
//! A class reading headers
class DocxXmlHeaderReader : public DocxXmlDocumentReader
{
public:
    explicit DocxXmlHeaderReader(KoOdfWriters *writers);
    ~DocxXmlHeaderReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

    QString content();

protected:
    KoFilter::ConversionStatus read_hdr();

private:
    void init();
    class Private;
    Private *const d;
    QString m_content;
};

#endif // DOCXXMLHEADERREADER_H
