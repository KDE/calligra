/*
 * This file is part of Office 2007 Filters for Calligra
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

#ifndef XLSXXMLSHAREDSTRINGSREADER_H
#define XLSXXMLSHAREDSTRINGSREADER_H

#include "XlsxXmlCommonReader.h"


class XlsxXmlSharedStringsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    explicit XlsxXmlSharedStringsReaderContext(QVector<QString>& _strings, MSOOXML::DrawingMLTheme* _themes,
        QVector<QString>& _colorIndices);
    QVector<QString>* strings;
    MSOOXML::DrawingMLTheme* themes;
    QVector<QString>& colorIndices;
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
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_sst();
    KoFilter::ConversionStatus read_si();

    uint m_index;

    XlsxXmlSharedStringsReaderContext* m_context;
private:
    void init();

    class Private;
    Private* const d;
};

#endif //XLSXXMLSHAREDSTRINGSREADER_H
