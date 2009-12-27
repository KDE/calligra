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

#ifndef XLSXXMLCOMMONREADER_H
#define XLSXXMLCOMMONREADER_H

#include <MsooXmlReader.h>

//! A class reading MSOOXML XLSX markup
//! This is a base class implementing reading elements common to some XLSX content types.
//! Currently this class is used by XlsxXmlSharedStringsReader and XlsxXmlDocumentReader.
class XlsxXmlCommonReader : public MSOOXML::MsooXmlReader
{
public:
    explicit XlsxXmlCommonReader(KoOdfWriters *writers);

    virtual ~XlsxXmlCommonReader();

protected:
    KoFilter::ConversionStatus read_t();
    KoFilter::ConversionStatus read_r();

    typedef KoFilter::ConversionStatus(XlsxXmlCommonReader::*ReadMethod)();
    QStack<ReadMethod> m_calls;

    QString m_text; //!< result of read_t() and read_r()
private:
    void init();

    class Private;
    Private* const d;
};

#endif //XLSXXMLSHAREDSTRINGSREADER_H
