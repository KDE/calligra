/*
 * This file is part of Office 2007 Filters for Calligra
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

#include <MsooXmlTheme.h>
#include <MsooXmlCommonReader.h>

#include <KoXmlWriter.h>
#include <KoBorder.h>     // Needed in DrawingMLMethods.h


class XlsxImport;
class XlsxXmlWorksheetReaderContext;
class XlsxXmlChartReaderContext;
class XlsxXmlEmbeddedPicture;
class Sheet;

namespace MSOOXML
{
    class MsooXmlDiagramReaderContext;
}


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

        KoXmlWriter * pictureWriter();


        /**
         * @return true if drawing object is anchored to cell
         * Presence of FromAnchor position is tested
         */
        bool isAnchoredToCell() const;

        /**
         * @return From anchor cell address
         * @see toCellAddress()
         */

        QString fromCellAddress() const;
        /**
         * @return End anchor cell address, in format like Sheetname.A5,
         * empty string if no ToAnchor position is available
         *
         */
        QString toCellAddress() const;

private:
        QRect positionRect() const;
        /**
        * Computes the cell address name in xlsx documents
        *
        * @return For sheetname "Sheet" and row 0, column 0 returns "Sheet.A1",
        * for row 0, column 1 "Sheet.B1" etc.
        * for row 1, column 0 "Sheet.A2" etc.
        */
        QString cellAddress(const QString &sheetname, int row, int column) const;

        KoXmlWriter* m_shapeBody;
};

class XlsxXmlDrawingReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    XlsxXmlDrawingReaderContext(XlsxXmlWorksheetReaderContext* _worksheetReaderContext, Sheet* _sheet, const QString& _path, const QString& _file);
    ~XlsxXmlDrawingReaderContext() override;

    XlsxImport* import;
    QString path; // contains the path to the file which is being processed (i.e. 'xl/drawings')
    QString file; // contains the name of the file which is being processed (i.e. 'drawing1.xml')
    const MSOOXML::DrawingMLTheme* themes;

    XlsxXmlWorksheetReaderContext* worksheetReaderContext;
    Sheet* sheet;
    quint32 m_groupDepthCounter; // How deep we currently are
};

class XlsxXmlDrawingReader : public MSOOXML::MsooXmlCommonReader
{
public:
    explicit XlsxXmlDrawingReader(KoOdfWriters *writers);
    ~XlsxXmlDrawingReader() override;
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

protected:
    KoFilter::ConversionStatus read_oneCellAnchor();
    KoFilter::ConversionStatus read_twoCellAnchor();
    KoFilter::ConversionStatus read_absoluteAnchor();
    KoFilter::ConversionStatus read_anchor(const QString& reference);
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
// It's saving service is used in XlsxXmlDrawingReader.cpp, it's created in MsooXmlCommonReaderDrawingMLImpl.h
class XlsxXmlEmbeddedPicture
{
public:
    XlsxXmlEmbeddedPicture();
    ~XlsxXmlEmbeddedPicture();

    /**
     * Use this pointer for KoXmlWriter for writing odf representing
     * the image anchored/embedded to element like cell.
     * The ownership of this pointer belong to XlsxXmlEmbeddedPicture class.
     * */
    KoXmlWriter * pictureWriter();

    /**
     * Save the .xml part of the picture (the picture itself isn't stored here)
     * @return true if saving was successful
     * */
    bool saveXml(KoXmlWriter *xmlWriter);

private:
    KoXmlWriter * m_pictureWriter;
    QBuffer m_pictureBuffer;
};

#endif
