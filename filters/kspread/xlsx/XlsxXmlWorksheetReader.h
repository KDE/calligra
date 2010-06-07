/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
class XlsxXmlDocumentReaderContext;
class XlsxComments;
class XlsxStyles;
class XlsxImport;

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
    KoFilter::ConversionStatus read_mergeCell();
    KoFilter::ConversionStatus read_mergeCells();
    KoFilter::ConversionStatus read_drawing();
    KoFilter::ConversionStatus read_hyperlink();
    KoFilter::ConversionStatus read_hyperlinks();
    KoFilter::ConversionStatus read_picture();

    XlsxXmlWorksheetReaderContext* m_context;

    KoGenStyle m_tableStyle;
    //! for table:table-column
    int m_columnCount;
    //! for table:table-row
    int m_currentRow;
    //! for table:table-cell
    int m_currentColumn;
    //! Used in read_f() and read_v()
    QString m_value;
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
    QString processRowStyle(const QString& heightString = QString());
    void appendTableColumns(int columns, const QString& width = QString());
    void appendTableCells(int cells);
    //! Saves annotation element (comments) for cell specified by @a col and @a row it there is any annotation defined.
    void saveAnnotation(int col, int row);

#include <MsooXmlCommonReaderMethods.h>
#include <MsooXmlCommonReaderDrawingMLMethods.h>

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
        const QString& _state,
        const QString _path, const QString _file,
        const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
        const XlsxSharedStringVector& _sharedStrings,
        const XlsxComments& _comments,
        const XlsxStyles& _styles,
        MSOOXML::MsooXmlRelationships& _relationships,
        XlsxImport* _import);

    const uint worksheetNumber;
    QString worksheetName;
    QString state;
    const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;
    const XlsxSharedStringVector *sharedStrings;
    const XlsxComments* comments;
    const XlsxStyles* styles;

    XlsxImport* import;
    const QString path;
    const QString file;
};

#endif
