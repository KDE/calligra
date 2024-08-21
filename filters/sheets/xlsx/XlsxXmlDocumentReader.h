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

#ifndef XLSXXMLDOCUMENTREADER_H
#define XLSXXMLDOCUMENTREADER_H

#include <MsooXmlReader.h>
#include <MsooXmlTheme.h>

#include <QMap>

class XlsxImport;
class XlsxComments;
class XlsxStyles;

//! Context for XlsxXmlDocumentReader
class XlsxXmlDocumentReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlDocumentReaderContext(XlsxImport &_import,
                                 MSOOXML::DrawingMLTheme *_themes,
                                 const QVector<QString> &_sharedStrings,
                                 const XlsxComments &_comments,
                                 const XlsxStyles &_styles,
                                 MSOOXML::MsooXmlRelationships &_relationships,
                                 const QString &_file,
                                 const QString &_path);
    XlsxImport *import;
    MSOOXML::DrawingMLTheme *themes;
    const QVector<QString> *sharedStrings;
    const XlsxComments *comments;
    const XlsxStyles *styles;
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
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext *context = nullptr) override;

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_workbook();
    KoFilter::ConversionStatus read_sheets();
    KoFilter::ConversionStatus read_sheet();

    XlsxXmlDocumentReaderContext *m_context;

private:
    void init();

    class Private;
    Private *const d;
};

#endif // XLSXXMLDOCUMENTREADER_H
