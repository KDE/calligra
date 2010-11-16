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
#include <KoCellStyle.h>

#include <QFlags>

/**
*   The following classes deal with the table styles part, specifically
*   we deal with the elements that start at the a:tblStyleLst §20.1.4.2.27, 
*   you can find its part definition at Table Styles Part §14.2.9
*/
namespace MSOOXML
{

struct TableStyleProperties
{
    KoBorder::BorderData bottom;
    KoBorder::BorderData insideH;
    KoBorder::BorderData insideV;
    KoBorder::BorderData left;
    KoBorder::BorderData right;
    KoBorder::BorderData tl2br;
    KoBorder::BorderData top;
    KoBorder::BorderData tr2bl;
};

class MSOOXML_EXPORT TableStyle
{
public:
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
        Band1Horizontal,
        Band2Horizontal,
        Band1Vertical,
        Band2Vertical,
        WholeTbl
    };

    TableStyle();
    ~TableStyle();

    void setId(const QString& id);
    QString id() const;

    void addProperties(Type type, TableStyleProperties* properties);
    TableStyleProperties* properties(Type type) const;

private:
    QString m_id;
    QMap<Type, TableStyleProperties*> m_properties;
    //TODO handle the table background stored in the element TblBg
};

class MSOOXML_EXPORT TableStyleList
{
public:
    TableStyleList();
    ~TableStyleList();

    TableStyle tableStyle(const QString& id) const;
    void insertStyle(QString id, MSOOXML::TableStyle style);

private:
    QMap<QString, TableStyle> m_styles;
};

class MSOOXML_EXPORT TableStyleInstanceProperties
{
    friend class TableStyleInstance;
public:
    TableStyleInstanceProperties(int rowCount, int columnCount);
    ~TableStyleInstanceProperties();

    TableStyleInstanceProperties& rowBandSize(int size);
    TableStyleInstanceProperties& columnBandSize(int size);

    enum Role {
        FirstRow,
        FirstCol,
        LastCol,
        LastRow,
        NeCell,
        NwCell,
        SeCell,
        SwCell,
        RowBanded,
        ColumnBanded,
        WholeTbl
    };
    Q_DECLARE_FLAGS(Roles, Role)

    TableStyleInstanceProperties& roles(Roles roles);

private:
    int m_rowCount;
    int m_columnCount;
    int m_rowBandSize;
    int m_columnBandSize;
    Roles m_role;
};

class MSOOXML_EXPORT TableStyleInstance
{
public:
    TableStyleInstance(TableStyle* style, TableStyleInstanceProperties properties);
    ~TableStyleInstance();

    KoCellStyle::Ptr style(int row, int column);

private:
    void applyStyle(MSOOXML::TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column);
    void applyBordersStyle(MSOOXML::TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column);

    TableStyle* m_style;
    TableStyleInstanceProperties m_properties;
};

class MsooXmlImport;
class MSOOXML_EXPORT MsooXmlDrawingTableStyleContext : public MSOOXML::MsooXmlReaderContext
{
public:
    MsooXmlDrawingTableStyleContext(MSOOXML::MsooXmlImport* _import, const QString& _path, const QString& _file, MSOOXML::DrawingMLTheme* _themes, MSOOXML::TableStyleList* _styleList);
    virtual ~MsooXmlDrawingTableStyleContext();

    TableStyleList* styleList;

    //Those members are used by some methods included
    MsooXmlImport* import;
    QString path;
    QString file;
    MSOOXML::DrawingMLTheme* themes;
};

class MSOOXML_EXPORT MsooXmlDrawingTableStyleReader : public MsooXmlCommonReader
{
public:
    MsooXmlDrawingTableStyleReader(KoOdfWriters* writers);
    virtual ~MsooXmlDrawingTableStyleReader();

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
    KoFilter::ConversionStatus read_tcBdr();
    KoFilter::ConversionStatus read_Table_ln();

    //get read_ln and friends, it's a shame I have to get a lot of crap alongside
    #include <MsooXmlCommonReaderMethods.h>
    #include <MsooXmlCommonReaderDrawingMLMethods.h>
private:
    MsooXmlDrawingTableStyleContext* m_context;

    TableStyle m_currentStyle;
    KoBorder::BorderData m_currentBorder;
    TableStyleProperties m_currentTableStyleProperties;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(MSOOXML::TableStyleInstanceProperties::Roles)

#endif // MSOOXMLDRAWINGTABLESTYLEREADER_H
