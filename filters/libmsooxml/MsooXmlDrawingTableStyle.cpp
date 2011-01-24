#include "MsooXmlDrawingTableStyle.h"
#include <QDebug>

using namespace MSOOXML;

DrawingTableStyleInstanceProperties::DrawingTableStyleInstanceProperties(int rowCount, int columnCount)
: TableStyleInstanceProperties(rowCount, columnCount)
, m_role(DrawingTableStyle::WholeTbl)
{
}

DrawingTableStyleInstanceProperties::~DrawingTableStyleInstanceProperties()
{
}

DrawingTableStyleInstanceProperties::Roles DrawingTableStyleInstanceProperties::roles() const
{
    return m_role;
}

DrawingTableStyleInstanceProperties& DrawingTableStyleInstanceProperties::setRoles(DrawingTableStyleInstanceProperties::Roles roles)
{
    m_role = roles;

    return *this;
}

DrawingTableStyleInstance::DrawingTableStyleInstance(DrawingTableStyle* style, DrawingTableStyleInstanceProperties properties)
: TableStyleInstance(properties)
, m_style(style)
, m_properties(properties)
{
}

DrawingTableStyleInstance::~DrawingTableStyleInstance()
{
}

void DrawingTableStyleInstance::applyStyle(MSOOXML::DrawingTableStyle::Type type, KoCellStyle::Ptr& style, int row, int column)
{
    if(!m_style) {
        return;
    }

    TableStyleProperties* const styleProperties = m_style->properties(type);
    TableStyleInstance::applyStyle(styleProperties, style, row, column);
}

KoCellStyle::Ptr DrawingTableStyleInstance::style(int row, int column)
{
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_properties.rowCount());
    Q_ASSERT(column >= 0);
    Q_ASSERT(column < m_properties.columnCount());

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

     const DrawingTableStyleInstanceProperties::Roles& role = m_properties.roles();
     const int lastRow = m_properties.rowCount() - 1 ;
     const int lastColumn = m_properties.columnCount() - 1 ;

    applyStyle(DrawingTableStyle::WholeTbl, cellStyle, row, column);

    TableStyleInstance::applyStyle(m_properties.localDefaultCellStyle(), cellStyle, row, column);

    if(role & DrawingTableStyleInstanceProperties::ColumnBanded) {
        //Is the column in the even band?
        if( (column % (m_properties.columnBandSize() * 2)) < m_properties.columnBandSize()) {
            applyStyle(DrawingTableStyle::Band1Vertical, cellStyle, row, column);
        }
        else {
            applyStyle(DrawingTableStyle::Band2Vertical, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::RowBanded) {
        //Is the row in the even band?
        if( (row % (m_properties.rowBandSize() * 2)) < m_properties.columnBandSize()) {
            applyStyle(DrawingTableStyle::Band1Horizontal, cellStyle, row, column);
        }
        else {
            applyStyle(DrawingTableStyle::Band2Horizontal, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::FirstRow) {
        if(row == 0) {
            applyStyle(DrawingTableStyle::FirstRow, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::LastRow) {
        if(row == lastRow) {
            applyStyle(DrawingTableStyle::FirstRow, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::FirstCol) {
        if(column == 0) {
            applyStyle(DrawingTableStyle::FirstCol, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::LastCol) {
        if(column == lastColumn) {
            applyStyle(DrawingTableStyle::LastCol, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::NeCell) {
        if(row == 0 && column == 0) {
            applyStyle(DrawingTableStyle::NwCell, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::NwCell) {
        if(row == 0 && column == lastColumn) {
            applyStyle(DrawingTableStyle::NeCell, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::SeCell) {
        if(row == lastRow && column == 0) {
            applyStyle(DrawingTableStyle::SwCell, cellStyle, row, column);
        }
    }

    if(role & DrawingTableStyleInstanceProperties::SwCell) {
        if(row == lastRow && column == lastColumn) {
            applyStyle(DrawingTableStyle::SeCell, cellStyle, row, column);
        }
    }

    TableStyleProperties* localStyle = m_properties.localStyles().localStyle(row, column);
    if(localStyle) {
        TableStyleInstance::applyStyle(localStyle, cellStyle, row, column);
    }

    return cellStyle;
}

DrawingTableStyle::DrawingTableStyle()
: m_properties()
{
}

DrawingTableStyle::~DrawingTableStyle()
{
//     qDeleteAll(m_properties.values());
}

void DrawingTableStyle::addProperties(DrawingTableStyle::Type type, TableStyleProperties* properties)
{
    m_properties.insert(type, properties);
}

TableStyleProperties* DrawingTableStyle::properties(DrawingTableStyle::Type type) const
{
    return m_properties.value(type);
}
