#ifndef MSOOXMLTABLESTYLE_H
#define MSOOXMLTABLESTYLE_H

#include "msooxml_export.h"

#include <KoCellStyle.h>

#include <QColor>
#include <QFlags>
#include <QMap>

namespace MSOOXML {

/// Reading and storage

struct MSOOXML_EXPORT TableStyleProperties
{
    enum Property {
        BottomBorder = 1,
        InsideHBorder = 2,
        InsideVBorder = 4,
        LeftBorder = 8,
        RightBorder = 16,
        Tl2brBorder = 32,
        TopBorder = 64,
        Tr2blBorder = 128,
        BackgroundColor = 256
    };
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
};

class MSOOXML_EXPORT TableStyle
{
public:

    TableStyle();
    virtual ~TableStyle();

    void setId(const QString& id);
    QString id() const;

private:
    QString m_id;
};

class MSOOXML_EXPORT TableStyleList
{
public:
    TableStyleList();
    ~TableStyleList();

    TableStyle* tableStyle(const QString& id) const;
    void insertStyle(QString id, MSOOXML::TableStyle* style);

private:
    QMap<QString, TableStyle*> m_styles;
};

/// Instantiation classes

class MSOOXML_EXPORT LocalTableStyles
{
public:
    LocalTableStyles();
    ~LocalTableStyles();

    TableStyleProperties* localStyle(int row, int column);
    void setLocalStyle(MSOOXML::TableStyleProperties* properties, int row, int column);

private:
    QMap<QPair<int,int>, TableStyleProperties*> m_properties;
};

class MSOOXML_EXPORT TableStyleInstanceProperties
{
public:
    TableStyleInstanceProperties(int rowCount, int columnCount);
    virtual ~TableStyleInstanceProperties();

    TableStyleInstanceProperties& setRowCount(int rowCount);
    int rowCount() const;

    TableStyleInstanceProperties& setColumnCount(int columnCount);
    int columnCount() const;

    TableStyleInstanceProperties& setRowBandSize(int size);
    int rowBandSize() const;

    TableStyleInstanceProperties& setColumnBandSize(int size);
    int columnBandSize() const;

    ///LocalStyles is a collection of cell<->style relationships
    TableStyleInstanceProperties& setLocalStyles(const LocalTableStyles& localStyles);
    LocalTableStyles localStyles() const;

    ///LocalTableStyle is a style defined to be the default style of a table. Defined locally.
    TableStyleInstanceProperties& setLocalDefaulCelltStyle(MSOOXML::TableStyleProperties* properties);
    TableStyleProperties* localDefaultCellStyle() const;

private:
    int m_rowCount;
    int m_columnCount;
    int m_rowBandSize;
    int m_columnBandSize;
    LocalTableStyles m_localStyles;
    MSOOXML::TableStyleProperties* m_localDefaultCellStyle;
};

class MSOOXML_EXPORT TableStyleInstance
{
public:
    TableStyleInstance(TableStyleInstanceProperties properties);
    virtual ~TableStyleInstance();

    virtual KoCellStyle::Ptr style(int row, int column) = 0;

protected:
    void applyStyle(MSOOXML::TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column);

private:
    void applyBordersStyle(MSOOXML::TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column);
    void applyBackground(MSOOXML::TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column);
    TableStyleInstanceProperties m_properties;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(MSOOXML::TableStyleProperties::Properties)

#endif
