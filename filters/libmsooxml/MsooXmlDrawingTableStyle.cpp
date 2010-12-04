#include "MsooXmlDrawingTableStyle.h"
#include <QDebug>

using namespace MSOOXML;

TableStyleInstanceProperties::TableStyleInstanceProperties(int rowCount, int columnCount)
: m_rowCount(rowCount)
, m_columnCount(columnCount)
, m_rowBandSize(1)
, m_columnBandSize(1)
, m_role(TableStyle::WholeTbl)
{
}

TableStyleInstanceProperties::~TableStyleInstanceProperties()
{
}

TableStyleInstanceProperties& TableStyleInstanceProperties::columnBandSize(int size)
{
    Q_ASSERT(size >= 0);
    m_columnBandSize = size;

    return *this;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::roles(TableStyleInstanceProperties::Roles roles)
{
    m_role = roles;

    return *this;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::rowBandSize(int size)
{
    m_rowBandSize = size;

    return *this;
}

TableStyleInstance::TableStyleInstance(TableStyle* style, TableStyleInstanceProperties properties)
: m_style(style)
, m_properties(properties)
{
    Q_ASSERT(m_style);
}

TableStyleInstance::~TableStyleInstance()
{
}

KoCellStyle::Ptr TableStyleInstance::style(int row, int column)
{
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_properties.m_rowCount);
    Q_ASSERT(column >= 0);
    Q_ASSERT(column < m_properties.m_columnCount);

    //TODO can we magically improve the creation of the styles?
    //For now I'll take the naive approach and say no. There are
    //way, way too many things to take into account so, reusing
    //the styles doesn't seem feasible.

    KoCellStyle::Ptr cellStyle = KoCellStyle::create();

    //This is a somehow tangled process.
    //First we have to apply the properties in the following order:
    //* Whole table
    //* Banded columns, even column banding
    //* Banded rows, even row banding
    //* First row, last row
    //* First column, last column
    //* Top left, top right, bottom left, bottom right
    //
    //Note that Subsequent properties that apply override previous ones.
    //
    //Note the standard doesn't say what happens to colliding properties
    //in the same hierarchy level (say, it's the first column and last column,
    //or it's first row and last row at once, I assume left to right priority
    //on the elements in the previous list.)
    //
    //See MSOOXML Table Styles §17.7.6 for details

     const TableStyleInstanceProperties::Roles& role = m_properties.m_role;
     const int lastRow = m_properties.m_rowCount - 1 ;
     const int lastColumn = m_properties.m_columnCount - 1 ;

    applyStyle(TableStyle::WholeTbl, cellStyle, row, column);

    if(role & TableStyleInstanceProperties::ColumnBanded) {
        //Is the column in the even band?
        if( (column % (m_properties.m_columnBandSize * 2)) < m_properties.m_columnBandSize) {
            applyStyle(TableStyle::Band1Vertical, cellStyle, row, column);
        }
        else {
            applyStyle(TableStyle::Band2Vertical, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::RowBanded) {
        //Is the row in the even band?
        if( (row % (m_properties.m_rowBandSize * 2)) < m_properties.m_columnBandSize) {
            applyStyle(TableStyle::Band1Horizontal, cellStyle, row, column);
        }
        else {
            applyStyle(TableStyle::Band2Horizontal, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::FirstRow) {
        if(row == 0) {
            applyStyle(TableStyle::FirstRow, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::LastRow) {
        if(row == lastRow) {
            applyStyle(TableStyle::FirstRow, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::FirstCol) {
        if(column == 0) {
            applyStyle(TableStyle::FirstCol, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::LastCol) {
        if(column == lastColumn) {
            applyStyle(TableStyle::LastCol, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::NeCell) {
        if(row == 0 && column == 0) {
            applyStyle(TableStyle::NwCell, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::NwCell) {
        if(row == 0 && column == lastColumn) {
            applyStyle(TableStyle::NeCell, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::SeCell) {
        if(row == lastRow && column == 0) {
            applyStyle(TableStyle::SwCell, cellStyle, row, column);
        }
    }

    if(role & TableStyleInstanceProperties::SwCell) {
        if(row == lastRow && column == lastColumn) {
            applyStyle(TableStyle::SeCell, cellStyle, row, column);
        }
    }

    return cellStyle;
}

void TableStyleInstance::applyStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    applyBordersStyle(styleProperties, style, row, column);
    applyBackground(styleProperties, style, row, column);
}

void TableStyleInstance::applyStyle(TableStyle::Type type, KoCellStyle::Ptr& style, int row, int column)
{
    TableStyleProperties* const styleProperties = m_style->properties(type);
    if(!styleProperties) {
        return;
    }

    //TODO apply other properties

    applyBordersStyle(styleProperties, style, row, column);
    applyBackground(styleProperties, style, row, column);
}

void TableStyleInstance::applyBackground(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);

    if(styleProperties->setProperties & TableStyleProperties::BackgroundColor) {
        style->setBackgroundColor(styleProperties->backgroundColor);
    }
}

void TableStyleInstance::applyBordersStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    const int lastRow = m_properties.m_rowCount - 1;
    const int lastColumn = m_properties.m_columnCount - 1;

    //Borders, are a bit tricky too; we have to take into account whether the cell 
    //has borders facing other cells or facing the border of the table.

    TableStyleProperties::Properties setProperties = styleProperties->setProperties;

    if(setProperties & TableStyleProperties::TopBorder) {
        KoBorder::BorderData* topData;
        if(row == 0) {
            topData = &styleProperties->top;
        }
        else {
            topData = &styleProperties->insideH;
        }
        style->borders()->setTopBorderColor(topData->color);
        style->borders()->setTopBorderSpacing(topData->spacing);
        style->borders()->setTopBorderStyle(topData->style);
        style->borders()->setTopBorderWidth(topData->width);
    }

    if(setProperties & TableStyleProperties::BottomBorder) {
        KoBorder::BorderData* bottomData;
        if(row == lastRow) {
            bottomData = &styleProperties->bottom;
        }
        else {
            bottomData = &styleProperties->insideH;
        }
        style->borders()->setBottomBorderColor(bottomData->color);
        style->borders()->setBottomBorderSpacing(bottomData->spacing);
        style->borders()->setBottomBorderStyle(bottomData->style);
        style->borders()->setBottomBorderWidth(bottomData->width);
    }

    if(setProperties & TableStyleProperties::LeftBorder) {
        KoBorder::BorderData* leftData;
        if(column == 0) {
            leftData = &styleProperties->left;
        }
        else {
            leftData = &styleProperties->insideV;
        }
        style->borders()->setLeftBorderColor(leftData->color);
        style->borders()->setLeftBorderSpacing(leftData->spacing);
        style->borders()->setLeftBorderStyle(leftData->style);
        style->borders()->setLeftBorderWidth(leftData->width);
    }

    if(setProperties & TableStyleProperties::RightBorder) {
        KoBorder::BorderData* rightData;
        if(column == lastColumn) {
            rightData = &styleProperties->right;
        }
        else {
            rightData = &styleProperties->insideV;
        }
        style->borders()->setRightBorderColor(rightData->color);
        style->borders()->setRightBorderSpacing(rightData->spacing);
        style->borders()->setRightBorderStyle(rightData->style);
        style->borders()->setRightBorderWidth(rightData->width);
    }
}

TableStyle::TableStyle()
: m_id()
, m_properties()
{
}

TableStyle::~TableStyle()
{
//     qDeleteAll(m_properties.values());
}

void TableStyle::setId(const QString& id)
{
    m_id = id;
}

QString TableStyle::id() const
{
    return m_id;
}

void TableStyle::addProperties(TableStyle::Type type, TableStyleProperties* properties)
{
    m_properties.insert(type, properties);
}

TableStyleProperties* TableStyle::properties(TableStyle::Type type) const
{
    return m_properties.value(type);
}

TableStyleList::TableStyleList()
: m_styles()
{
}

TableStyleList::~TableStyleList()
{
}

TableStyle TableStyleList::tableStyle(const QString& id) const
{
    return m_styles.value(id);
}

void TableStyleList::insertStyle(QString id, TableStyle style)
{
    m_styles.insert(id, style);
}

LocalTableStyles::LocalTableStyles()
{
}

LocalTableStyles::~LocalTableStyles()
{
}

TableStyleProperties* LocalTableStyles::localStyle(int row, int column)
{
    const QPair<int,int> key(row,column);
    return m_properties.value(key);
}

void LocalTableStyles::setLocalStyle(TableStyleProperties* properties, int row, int column)
{
    const QPair<int,int> key(row,column);
    m_properties.insert(key, properties);
}

