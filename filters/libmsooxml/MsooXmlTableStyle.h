/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010-2011 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MSOOXMLTABLESTYLE_H
#define MSOOXMLTABLESTYLE_H

#include "komsooxml_export.h"

#include <KoBorder.h>
#include <KoCellStyle.h>
#include <KoGenStyle.h>

#include <QColor>
#include <QFlags>
#include <QMap>

namespace MSOOXML
{

/// Reading and storage

struct KOMSOOXML_EXPORT TableStyleProperties {
    TableStyleProperties()
        : target(Table)
    {
    }

    enum TargetLevel { Table, TableRow, TableColumn, TableCell };

    enum Property {
        BottomBorder = 1,
        InsideHBorder = 2,
        InsideVBorder = 4,
        LeftBorder = 8,
        RightBorder = 16,
        Tl2brBorder = 32,
        TopBorder = 64,
        Tr2blBorder = 128,
        BackgroundColor = 256,
        TopMargin = 512,
        RightMargin = 1024,
        BottomMargin = 2048,
        LeftMargin = 4096,
        VerticalAlign = 8192,
        GlyphOrientation = 16384,
        BackgroundOpacity = 32768
    };

    TargetLevel target;

    Q_DECLARE_FLAGS(Properties, Property)
    Properties setProperties;

    KoBorder::BorderData bottom;
    KoBorder::BorderData insideH;
    KoBorder::BorderData insideV;
    KoBorder::BorderData left;
    KoBorder::BorderData right;
    KoBorder::BorderData tl2br;
    KoBorder::BorderData top;
    KoBorder::BorderData tr2bl;

    QColor backgroundColor;
    qreal backgroundOpacity;

    qreal topMargin;
    qreal rightMargin;
    qreal bottomMargin;
    qreal leftMargin;

    QString verticalAlign;
    bool glyphOrientation;

    KoGenStyle textStyle;
    KoGenStyle paragraphStyle;
};

class KOMSOOXML_EXPORT TableStyle
{
public:
    TableStyle();
    virtual ~TableStyle();

    void setId(const QString &id);
    QString id() const;

private:
    QString m_id;
};

/// Instantiation classes

class KOMSOOXML_EXPORT LocalTableStyles
{
public:
    LocalTableStyles();
    ~LocalTableStyles();
    LocalTableStyles(LocalTableStyles &&) = default;
    LocalTableStyles &operator=(LocalTableStyles &&) = default;

    TableStyleProperties *localStyle(int row, int column) const;
    void setLocalStyle(MSOOXML::TableStyleProperties *properties, int row, int column);

private:
    // Non-copyable since we own the TableStyleProperties objects
    LocalTableStyles(const LocalTableStyles &) = delete;
    LocalTableStyles &operator=(const LocalTableStyles &) = delete;

    QMap<QPair<int, int>, TableStyleProperties *> m_properties;
};

class KOMSOOXML_EXPORT TableStyleConverterProperties
{
public:
    TableStyleConverterProperties();
    virtual ~TableStyleConverterProperties();

    void setRowCount(int rowCount);
    int rowCount() const;

    void setColumnCount(int columnCount);
    int columnCount() const;

    void setRowBandSize(int size);
    int rowBandSize() const;

    void setColumnBandSize(int size);
    int columnBandSize() const;

    /// LocalStyles is a collection of cell<->style relationships
    void setLocalStyles(LocalTableStyles localStyles);
    const LocalTableStyles &localStyles() const;

    /// LocalTableStyle is a style defined to be the default style of a table. Defined locally.
    void setLocalDefaulCelltStyle(MSOOXML::TableStyleProperties *properties);
    TableStyleProperties *localDefaultCellStyle() const;

private:
    int m_rowCount;
    int m_columnCount;
    int m_rowBandSize;
    int m_columnBandSize;
    LocalTableStyles m_localStyles;
    MSOOXML::TableStyleProperties *m_localDefaultCellStyle;
};

class KOMSOOXML_EXPORT TableStyleConverter
{
public:
    TableStyleConverter(int row, int column);
    virtual ~TableStyleConverter();

    virtual KoCellStyle::Ptr style(int row, int column, const QPair<int, int> &spans) = 0;

protected:
    void applyStyle(MSOOXML::TableStyleProperties *styleProperties, KoCellStyle::Ptr &style, int row, int column, const QPair<int, int> &spans);

    // NOTE: TESTING
    void reapplyTableLevelBordersStyle(MSOOXML::TableStyleProperties *properties,
                                       MSOOXML::TableStyleProperties *localProperties,
                                       MSOOXML::TableStyleProperties *exceptionProperties,
                                       KoCellStyle::Ptr &style,
                                       int row,
                                       int column,
                                       const QPair<int, int> &spans);

private:
    void applyTableLevelBordersStyle(MSOOXML::TableStyleProperties *properties, KoCellStyle::Ptr &style, int row, int column, const QPair<int, int> &spans);

    void applyRowLevelBordersStyle(MSOOXML::TableStyleProperties *properties, KoCellStyle::Ptr &style, int row, int column, const QPair<int, int> &spans);

    void applyColumnLevelBordersStyle(MSOOXML::TableStyleProperties *properties, KoCellStyle::Ptr &style, int row, int column, const QPair<int, int> &spans);

    void applyCellLevelBordersStyle(MSOOXML::TableStyleProperties *properties, KoCellStyle::Ptr &style);

    void applyBackground(MSOOXML::TableStyleProperties *properties, KoCellStyle::Ptr &style, int row, int column);

    int m_row;
    int m_column;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(MSOOXML::TableStyleProperties::Properties)

#endif
