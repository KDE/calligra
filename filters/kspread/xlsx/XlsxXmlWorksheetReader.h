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

#ifndef XLSXXMLWORKSHEETREADER_H
#define XLSXXMLWORKSHEETREADER_H

#include <MsooXmlThemesReader.h>
#include <MsooXmlCommonReader.h>
#include "XlsxSharedString.h"

#include <KoGenStyle.h>
#include <styles/KoCharacterStyle.h>

class XlsxXmlWorksheetReaderContext;
class XlsxStyles;

//! A class reading MSOOXML XLSX markup - xl/worksheets/sheet*.xml part.
class XlsxXmlWorksheetReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit XlsxXmlWorksheetReader(KoOdfWriters *writers);

    virtual ~XlsxXmlWorksheetReader();

    //! Reads/parses the XML.
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_worksheet();
    KoFilter::ConversionStatus read_sheetFormatPr();
    KoFilter::ConversionStatus read_cols();
    KoFilter::ConversionStatus read_col();
    KoFilter::ConversionStatus read_sheetData();
    KoFilter::ConversionStatus read_row();
    KoFilter::ConversionStatus read_c();
    KoFilter::ConversionStatus read_f();
    KoFilter::ConversionStatus read_v();

    typedef KoFilter::ConversionStatus(XlsxXmlWorksheetReader::*ReadMethod)();
    QStack<ReadMethod> m_calls;

    XlsxXmlWorksheetReaderContext* m_context;

    KoGenStyle m_tableStyle;
    //! for table:table-column
    uint m_columnCount;
    //! for table:table-row
    uint m_currentRow;
    //! for table:table-cell
    int m_currentColumn;
    //! Used in read_f() and read_v()
    QString m_value;

    //! Affects read_f(), set in read_c()
    bool m_convertFormula;

    //! Set in read_f() and used in read_c()
    QString m_formula;

    //! Filled by read_sheetFormatPr(), measured in pt
    QString m_defaultRowHeight;

    // for optimization
    static const char* officeValue;
    static const char* officeDateValue;
    static const char* officeTimeValue;
    static const char* officeStringValue;
    static const char* officeBooleanValue;

private:
    void init();

    void showWarningAboutWorksheetSize();
    void saveColumnStyle(const QString& widthString);
    KoFilter::ConversionStatus saveRowStyle(const QString& heightString);
    void appendTableColumns(int columns, const QString& width = QString());
    void appendTableCells(int cells);

#include <MsooXmlCommonReaderMethods.h>

    class Private;
    Private* const d;
};

class XlsxXmlWorksheetReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    //! Creates the context object.
    XlsxXmlWorksheetReaderContext(
        uint _worksheetNumber,
        const QString& _worksheetName,
        const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
        const XlsxSharedStringVector& _sharedStrings, const XlsxStyles& _styles);
    const uint worksheetNumber;
    QString worksheetName;
    const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;
    const XlsxSharedStringVector *sharedStrings;
    const XlsxStyles* styles;
};

#endif
