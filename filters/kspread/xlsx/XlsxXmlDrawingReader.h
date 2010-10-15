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

#include <KoXmlWriter.h>

class XlsxImport;
class XlsxXmlWorksheetReaderContext;
class XlsxXmlChartReaderContext;
class XlsxXmlEmbeddedPicture;
class Sheet;

namespace MSOOXML
{
    class MsooXmlDiagramReaderContext;
}

//#include <KoGenStyle.h>
//#include <styles/KoCharacterStyle.h>

class XlsxShape {
};

class XlsxDrawingObject {
    public:
        Sheet* m_sheet;
        enum Type {
            Unknown,
            Chart,
            Diagram,
            Picture,
            Shape
        };
        Type m_type;
        union {
            XlsxXmlChartReaderContext* m_chart;
            MSOOXML::MsooXmlDiagramReaderContext* m_diagram;
            XlsxXmlEmbeddedPicture* m_picture;
            XlsxShape* m_shape;
        };
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
        explicit XlsxDrawingObject(Sheet* sheet) : m_sheet(sheet), m_type(Unknown), m_shapeBody(0) {}
        ~XlsxDrawingObject() { delete m_shapeBody; }
        void setPicture(XlsxXmlEmbeddedPicture* picture) { m_type = Picture; m_picture = picture; }
        void setChart(XlsxXmlChartReaderContext* chart) { m_type = Chart; m_chart = chart; }
        void setDiagram(MSOOXML::MsooXmlDiagramReaderContext* diagram) { m_type = Diagram; m_diagram = diagram; }
        KoXmlWriter* setShape(XlsxShape* shape);
        void save(KoXmlWriter* xmlWriter);
    private:
        QRect positionRect() const;
        QString cellAddress(const QString &sheetname, int row, int column) const;
        QString fromCellAddress() const;
        QString toCellAddress() const;
        KoXmlWriter* m_shapeBody;
};

class XlsxXmlDrawingReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlDrawingReaderContext(XlsxXmlWorksheetReaderContext* _worksheetReaderContext, Sheet* _sheet, const QString& _path, const QString& _file);
    virtual ~XlsxXmlDrawingReaderContext();

    XlsxImport* import;
    QString path; // contains the path to the file which is being processed (i.e. 'xl/drawings')
    QString file; // contains the name of the file which is being processed (i.e. 'drawing1.xml')
    const MSOOXML::DrawingMLTheme* themes;

    XlsxXmlWorksheetReaderContext* worksheetReaderContext;
    Sheet* sheet;
};

class XlsxXmlDrawingReader : public MSOOXML::MsooXmlCommonReader
{
public:
    XlsxXmlDrawingReader(KoOdfWriters *writers);
    virtual ~XlsxXmlDrawingReader();
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_anchor(const QStringRef &name);
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
    XlsxDrawingObject *m_currentDrawingObject;
    XlsxDrawingObject::AnchorType m_anchorType;
    int m_chartNumber;

#include <MsooXmlCommonReaderMethods.h>
#include <MsooXmlCommonReaderDrawingMLMethods.h>
// #include <MsooXmlDrawingReaderTableMethods.h>

    Q_DISABLE_COPY(XlsxXmlDrawingReader)
};

// This class is used for storing information about embedded pictures.
// It is filled in XlsxXmlDrawingReader.cpp, some processing is done on it in XlsxXmlWorksheetReader.cpp.
class XlsxXmlEmbeddedPicture
{
public:
    XlsxXmlEmbeddedPicture();

    bool saveXml(KoXmlWriter *xmlWriter);   // save the .xml part of the picture (the picture itself isn't stored here)
    void setImageXml(const QString imageXml);

    QString m_imageXml;                               // draw:image of the image
};

#endif
