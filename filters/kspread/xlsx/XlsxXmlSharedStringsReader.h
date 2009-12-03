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

#ifndef XLSXXMLSHAREDSTRINGSREADER_H
#define XLSXXMLSHAREDSTRINGSREADER_H

#include <MsooXmlReader.h>

class XlsxImport;

class XlsxXmlSharedStringsReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlSharedStringsReaderContext(QVector<QString>& _strings);
    QVector<QString>* strings;
};

//! A class reading MSOOXML XLSX markup - sharedStrings.xml part.
//! See ECMA-376, 12.3.15: Shared String Table Part
class XlsxXmlSharedStringsReader : public MSOOXML::MsooXmlReader
{
public:
    explicit XlsxXmlSharedStringsReader(KoOdfWriters *writers);

    virtual ~XlsxXmlSharedStringsReader();

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_sst();
    KoFilter::ConversionStatus read_si();
    KoFilter::ConversionStatus read_t();

    uint m_index;

    typedef KoFilter::ConversionStatus (XlsxXmlSharedStringsReader::*ReadMethod) ();
    QStack<ReadMethod> m_calls;

    XlsxXmlSharedStringsReaderContext* m_context;
private:
    void init();

    class Private;
    Private* const d;
};

#endif //XLSXXMLSHAREDSTRINGSREADER_H
