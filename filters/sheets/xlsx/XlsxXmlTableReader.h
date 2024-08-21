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

#ifndef XLSXXMLTABLEREADER_H
#define XLSXXMLTABLEREADER_H

#include <MsooXmlCommonReader.h>

class XlsxXmlTableReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlTableReaderContext();

    QString referenceArea;
    int headerStyleIndex;
    int dataStyleIndex;
    int totalsRowIndex;
    unsigned headerRowCount;
    unsigned totalsRowCount;
};

class XlsxXmlTableReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit XlsxXmlTableReader(KoOdfWriters *writers);
    ~XlsxXmlTableReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus read_table();

private:
    XlsxXmlTableReaderContext *m_context;
};

#endif
