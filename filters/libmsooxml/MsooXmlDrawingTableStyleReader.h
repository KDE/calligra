/* This file is part of the KDE project
 * Copyright (C) 2010 Carlos Licea <carlos@kdab.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MSOOXMLDRAWINGTABLESTYLEREADER_H
#define MSOOXMLDRAWINGTABLESTYLEREADER_H

#include <MsooXmlCommonReader.h>
#include <MsooXmlThemesReader.h>

class KoGenStyles;

#include <QPen>
#include <QString>

/**
*   The following classes deal with the table styles part, specifically
*   we deal with the elements that start at the a:tblStyleLst §20.1.4.2.27, 
*   you can find its part definition at Table Styles Part §14.2.9
*/
namespace MSOOXML
{

class Border {
public:
    Border();
    ~Border();

    enum Side {
        NoSide,
        Bottom,
        Left,
        Right,
//         TopLeftToBottomRight,
        Top,
//         TopRightToBottomLeft
    };

    Side side() const;
    void setSide(Side side);

    QColor color() const;
    void setColor(const QColor& color);

    QString odfBorderName() const;

    enum Style {
        None,
        Solid,
        Dashed,
        Dotted,
        DashDot,
        DashDotDot
    };
    void setStyle(Style style);
    Style style() const;
    QString odfStyleName() const;

    void setWidth(qreal width);
    qreal width() const;

    QString odfStyleProperties() const;

private:
    QColor m_color;
    Side m_side;
    qreal m_width;
    Style m_style;
};

class TableStyleProperties
{
public:
    TableStyleProperties();
    ~TableStyleProperties();

    enum Type {
        NoType,
        FirstRow,
        FirstCol,
        LastCol,
        LastRow,
        NeCell,
        NwCell,
        SeCell,
        SwCell,
        WholeTbl
    };

    Type type() const;
    void setType(Type type);

    Border borderForSide(Border::Side side) const;
    void addBorder(Border border);

    /** 
    * @brief Save the style, note that the type of the style depends on the type 
    * of this styleProperties
    * @return the name of the saved style
    */
    QString saveStyle(KoGenStyles& styles);

    static Type typeFromString(const QString& string);
    static QString stringFromType(Type type);

private:
    //TODO see if we can take care of InsideH InsideV and how
    QMap<Border::Side, Border> m_borders;
    Type m_type;
};

class TableStyle
{
public:

    QString id() const;
    void setId(const QString& id);

    TableStyleProperties propertiesForType(TableStyleProperties::Type type) const;
    void addProperties(TableStyleProperties properties);

private:
    QString m_id;
    //TODO handle the table background stored in the element TblBg
    QMap<TableStyleProperties::Type, TableStyleProperties> m_properties;
};

class TableStyleList
{
public:
    TableStyle tableStyle(const QString& id) const;
    void insertStyle(QString id, MSOOXML::TableStyle style);

private:
    QMap<QString, TableStyle> m_styles;
};

class MsooXmlImport;
class MsooXmlDrawingTableStyleContext : public MSOOXML::MsooXmlReaderContext
{
public:
    TableStyleList styleList;

    //Should be mostly unused, those members are needed by some functions not used
    //in this file
    MsooXmlImport* import;
    const QString path;
    const QString file;
    const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;
};

class MsooXmlDrawingTableStyleReader : public MsooXmlCommonReader
{
public:
    MsooXmlDrawingTableStyleReader(KoOdfWriters* writers);
    ~MsooXmlDrawingTableStyleReader();

    virtual KoFilter::ConversionStatus read(MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus read_tblStyleLst();
    KoFilter::ConversionStatus read_tblStyle();
    KoFilter::ConversionStatus read_wholeTbl();
    KoFilter::ConversionStatus read_tcStyle();
    KoFilter::ConversionStatus read_tcTxStyle();
    KoFilter::ConversionStatus read_bottom();
    KoFilter::ConversionStatus read_left();
    KoFilter::ConversionStatus read_right();
    KoFilter::ConversionStatus read_top();
//     KoFilter::ConversionStatus read_insideV();
//     KoFilter::ConversionStatus read_insideH();
//     KoFilter::ConversionStatus read_tl2br();
//     KoFilter::ConversionStatus read_tr2bl();
    KoFilter::ConversionStatus read_tcBrd();

    //get read_ln and friends, it's a shame I have to get a lot of crap alongside
    #include <MsooXmlCommonReaderMethods.h>
    #include <MsooXmlCommonReaderDrawingMLMethods.h>

private:
    MsooXmlDrawingTableStyleContext* m_context;

    TableStyleProperties m_currentStyleProperties;
    TableStyle m_currentStyle;
};

}
#endif // MSOOXMLDRAWINGTABLESTYLEREADER_H
