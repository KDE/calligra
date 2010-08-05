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

#include <MsooXmlReader.h>

#include <QPen>

/**
*   The following classes deal with the table styles part, specifically
*   we deal with the elements that start at the a:tblStyleLst §20.1.4.2.27, 
*   you can find its part definition at Table Styles Part §14.2.9
*/
namespace MSOOXML
{

class Border {
public:
    void setPen(const QPen& pen);
    QPen pen() const;

private:
    QPen m_pen;
};

class TableStyleProperties
{
public:
    enum BorderSide {
        Bottom,
        insideH,
        InsideV,
        Left,
        Right,
        TopLeftToBottomRight,
        Top,
        TopRightToBottomLeft
    };

    Border borderForSide(BorderSide side) const;
    void addBorder(Border border, BorderSide side);

private:
    QMap<BorderSide, Border> m_borders;
};

class TableStyle
{
public:
    enum Type {
        FirstRow,
        LastCol,
        LastRow,
        NeCell,
        NwCell,
        SeCell,
        SwCell,
        TblBg,
        WholeTbl
    };

    QString id() const;
    void setId(const QString& id);

    TableStyleProperties propertiesForType(Type type) const;
    void addProperties(TableStyleProperties properties);

    static Type typeFromString(const QString& string);
    static QString stringFromType(Type type);

private:
    QString m_id;
    QMap<Type, TableStyleProperties> m_properties;
};

class TableStyleList
{
public:
    TableStyle tableStyle(const QString& id) const;
    void insertStyle(QString id, MSOOXML::TableStyle style);

private:
    QMap<QString, TableStyle> m_styles;
};

class MsooXmlDrawingTableStyleReader : public MsooXmlReader
{
public:
    MsooXmlDrawingTableStyleReader(KoOdfWriters* writers);
    ~MsooXmlDrawingTableStyleReader();

    virtual KoFilter::ConversionStatus read(MsooXmlReaderContext* context = 0);

private:
    KoFilter::ConversionStatus read_tblStyleLst();
    KoFilter::ConversionStatus read_tblStyle();
    KoFilter::ConversionStatus read_wholeTbl();
    KoFilter::ConversionStatus read_tcStyle();
    KoFilter::ConversionStatus read_tcTxStyle();
    KoFilter::ConversionStatus read_bottom();
    KoFilter::ConversionStatus read_left();
    KoFilter::ConversionStatus read_right();
    KoFilter::ConversionStatus read_top();

    //get read_ln and friends, it's a shame I have to get a lot of crap alongside
    #include <MsooXmlCommonReaderDrawingMLMethods.h>

private:
    MsooXmlDrawingTablesStyleContext* m_context;

    Border m_currentBorder;
    TableStyleProperties::BorderSide m_currentBorderSide;

    TableStyleProperties m_currentStyleProperties;
    TableStyle::Type m_currentStylePropertiesType;

    TableStyle m_currentStyle;
};

class MsooXmlDrawingTablesStyleContext : public MSOOXML::MsooXmlReaderContext
{
public:
    TableStyleList styleList;
};

}
#endif // MSOOXMLDRAWINGTABLESTYLEREADER_H
