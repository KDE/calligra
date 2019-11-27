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

#ifndef XLSXXMLDOCUMENTREADER_H
#define XLSXXMLDOCUMENTREADER_H

#include <MsooXmlTheme.h>
#include <MsooXmlReader.h>

#include <QMap>

class XlsxImport;
class XlsxComments;
class XlsxStyles;

//! Context for XlsxXmlDocumentReader
class XlsxXmlDocumentReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlDocumentReaderContext(XlsxImport& _import,
                                 MSOOXML::DrawingMLTheme* _themes,
                                 const QVector<QString>& _sharedStrings,
                                 const XlsxComments& _comments,
                                 const XlsxStyles& _styles,
                                 MSOOXML::MsooXmlRelationships& _relationships,
                                 const QString &_file, const QString &_path);
    XlsxImport *import;
    MSOOXML::DrawingMLTheme *themes;
    const QVector<QString>* sharedStrings;
    const XlsxComments* comments;
    const XlsxStyles* styles;
    QString file, path;

    struct AutoFilterCondition {
        QString field;
        QString value;
        QString opField;
    };

    struct AutoFilter {
        QString type; // empty, -and, -or
        QString area;
        QString field;
        QVector<AutoFilterCondition> filterConditions;
    };

    QVector<XlsxXmlDocumentReaderContext::AutoFilter> autoFilters;
};

//! A class reading MSOOXML XLSX markup - workbook.xml part.
class XlsxXmlDocumentReader : public MSOOXML::MsooXmlReader
{
public:
    explicit XlsxXmlDocumentReader(KoOdfWriters *writers);

    ~XlsxXmlDocumentReader() override;

    //! Reads/parses the file of format document.xml.
    //! The output goes mainly to KoXmlWriter* KoOdfWriters::body
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_workbook();
    KoFilter::ConversionStatus read_sheets();
    KoFilter::ConversionStatus read_sheet();

    XlsxXmlDocumentReaderContext* m_context;
private:
    void init();

    class Private;
    Private* const d;
};

#endif //XLSXXMLDOCUMENTREADER_H
