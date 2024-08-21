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

#ifndef XLSXXMLSHAREDSTRINGSREADER_H
#define XLSXXMLSHAREDSTRINGSREADER_H

#include "XlsxXmlCommonReader.h"

class XlsxXmlSharedStringsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    explicit XlsxXmlSharedStringsReaderContext(QVector<QString> &_strings, MSOOXML::DrawingMLTheme *_themes, QVector<QString> &_colorIndices);
    QVector<QString> *strings;
    MSOOXML::DrawingMLTheme *themes;
    QVector<QString> &colorIndices;
};

//! A class reading MSOOXML XLSX markup - sharedStrings.xml part.
//! See ECMA-376, 12.3.15: Shared String Table Part
class XlsxXmlSharedStringsReader : public XlsxXmlCommonReader
{
public:
    explicit XlsxXmlSharedStringsReader(KoOdfWriters *writers);

    ~XlsxXmlSharedStringsReader() override;

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_sst();
    KoFilter::ConversionStatus read_si();

    uint m_index;

    XlsxXmlSharedStringsReaderContext *m_context;

private:
    void init();

    class Private;
    Private *const d;
};

#endif // XLSXXMLSHAREDSTRINGSREADER_H
