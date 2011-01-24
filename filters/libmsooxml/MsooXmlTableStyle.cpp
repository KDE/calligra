#include "MsooXmlTableStyle.h"

using namespace MSOOXML;

TableStyleInstanceProperties::TableStyleInstanceProperties(int rowCount, int columnCount)
: m_rowCount(rowCount)
, m_columnCount(columnCount)
, m_rowBandSize(1)
, m_columnBandSize(1)
, m_localStyles()
, m_localDefaultCellStyle(0)
{
}

TableStyleInstanceProperties::~TableStyleInstanceProperties()
{
}

TableStyleInstanceProperties& TableStyleInstanceProperties::setRowCount(int rowCount)
{
    m_rowCount = rowCount;

    return *this;
}

int TableStyleInstanceProperties::rowCount() const
{
    return m_rowCount;
}


TableStyleInstanceProperties& TableStyleInstanceProperties::setColumnCount(int columnCount)
{
    m_columnCount = columnCount;

    return *this;
}

int TableStyleInstanceProperties::columnCount() const
{
    return m_columnCount;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::setColumnBandSize(int size)
{
    Q_ASSERT(size >= 0);
    m_columnBandSize = size;

    return *this;
}

int TableStyleInstanceProperties::columnBandSize() const
{
    return m_columnBandSize;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::setRowBandSize(int size)
{
    m_rowBandSize = size;

    return *this;
}

int TableStyleInstanceProperties::rowBandSize() const
{
    return m_rowBandSize;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::setLocalStyles(const MSOOXML::LocalTableStyles& localStyles)
{
    m_localStyles = localStyles;

    return *this;
}

LocalTableStyles TableStyleInstanceProperties::localStyles() const
{
    return m_localStyles;
}

TableStyleInstanceProperties& TableStyleInstanceProperties::setLocalDefaulCelltStyle(TableStyleProperties* properties)
{
    m_localDefaultCellStyle = properties;

    return *this;
}

TableStyleProperties* TableStyleInstanceProperties::localDefaultCellStyle() const
{
    return m_localDefaultCellStyle;
}

TableStyleInstance::TableStyleInstance(TableStyleInstanceProperties properties)
: m_properties(properties)
{
}

TableStyleInstance::~TableStyleInstance()
{
}

void TableStyleInstance::applyStyle(TableStyleProperties* styleProperties, KoCellStyle::Ptr& style, int row, int column)
{
    if(!styleProperties) {
        return;
    }

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
    const int lastRow = m_properties.rowCount() - 1;
    const int lastColumn = m_properties.columnCount() - 1;

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


TableStyleList::TableStyleList()
: m_styles()
{
}

TableStyleList::~TableStyleList()
{
}

TableStyle* TableStyleList::tableStyle(const QString& id) const
{
    return m_styles.value(id);
}

void TableStyleList::insertStyle(QString id, TableStyle* style)
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
