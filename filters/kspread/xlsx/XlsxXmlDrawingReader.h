/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef XLSXXMLDRAWINGREADER_H
#define XLSXXMLDRAWINGREADER_H

#include <MsooXmlThemesReader.h>
#include <MsooXmlCommonReader.h>

class KoXmlWriter;
class XlsxImport;
class XlsxXmlWorksheetReaderContext;
class XlsxXmlChartReaderContext;

//#include <KoGenStyle.h>
//#include <styles/KoCharacterStyle.h>

namespace MSOOXML
{
class MsooXmlRelationships;
}

class XlsxXmlDrawingReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlDrawingReaderContext(XlsxXmlWorksheetReaderContext* _worksheetReaderContext, const QString& _path, const QString& _file);
    virtual ~XlsxXmlDrawingReaderContext();

    XlsxImport* import;
    QString path;
    QString file;
    const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;

    XlsxXmlWorksheetReaderContext* worksheetReaderContext;
    QList<XlsxXmlChartReaderContext*> charts;

    enum AnchorType {
        NoAnchor,
        FromAnchor,
        ToAnchor
    };

    struct Position {
        int m_row, m_col, m_rowOff, m_colOff;
        Position() : m_row(0), m_col(0), m_rowOff(0), m_colOff(0) {}
    };

    QMap<AnchorType, Position> m_positions;
};

class XlsxXmlDrawingReader : public MSOOXML::MsooXmlCommonReader
{
public:
    XlsxXmlDrawingReader(KoOdfWriters *writers);
    virtual ~XlsxXmlDrawingReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_from();
    KoFilter::ConversionStatus read_to();
    KoFilter::ConversionStatus read_col();
    KoFilter::ConversionStatus read_row();
    KoFilter::ConversionStatus read_colOff();
    KoFilter::ConversionStatus read_rowOff();
    KoFilter::ConversionStatus read_graphicFrame();
    KoFilter::ConversionStatus read_graphic2();
    KoFilter::ConversionStatus read_graphicData2();
    KoFilter::ConversionStatus read_chart2();
    KoFilter::ConversionStatus read_diagram();
private:
    XlsxXmlDrawingReaderContext *m_context;
    XlsxXmlDrawingReaderContext::AnchorType m_anchorType;

    int m_chartNumber;
    
// #include <MsooXmlCommonReaderMethods.h>
// #include <MsooXmlCommonReaderDrawingMLMethods.h>
// #include <MsooXmlDrawingReaderTableMethods.h>

    Q_DISABLE_COPY(XlsxXmlDrawingReader)
};

#endif
