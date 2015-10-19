#include "MsooXmlDrawingTableStyle.h"

//#define MSOOXMLDRAWING_DEBUG_TABLES
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
#include "MsooXmlDebug.h"
#endif

using namespace MSOOXML;

DrawingTableStyleConverterProperties::DrawingTableStyleConverterProperties()
: TableStyleConverterProperties()
, m_role(DrawingTableStyle::WholeTbl)
{
}

DrawingTableStyleConverterProperties::~DrawingTableStyleConverterProperties()
{
}

DrawingTableStyleConverterProperties::Roles DrawingTableStyleConverterProperties::roles() const
{
    return m_role;
}

void DrawingTableStyleConverterProperties::setRoles(DrawingTableStyleConverterProperties::Roles roles)
{
    m_role = roles;
}

DrawingTableStyleConverter::DrawingTableStyleConverter(MSOOXML::DrawingTableStyleConverterProperties const& properties, DrawingTableStyle* style)
: TableStyleConverter(properties.rowCount(), properties.columnCount())
, m_style(style)
, m_properties(properties)
{
}

DrawingTableStyleConverter::~DrawingTableStyleConverter()
{
}

void DrawingTableStyleConverter::applyStyle(MSOOXML::DrawingTableStyle::Type type, KoCellStyle::Ptr& style,
                                            int row, int column, const QPair<int, int> &spans)
{
    if (!m_style) {
        return;
    }

    TableStyleProperties* styleProperties = m_style->properties(type);

//     if (type == DrawingTableStyle::WholeTbl) {
//         styleProperties->bordersToEdgesOnly = true;
//     }

    TableStyleConverter::applyStyle(styleProperties, style, row, column, spans);
}

KoCellStyle::Ptr DrawingTableStyleConverter::style(int row, int column, const QPair<int, int> &spans)
{
    Q_ASSERT(row >= 0);
    Q_ASSERT(row < m_properties.rowCount());
    Q_ASSERT(column >= 0);
    Q_ASSERT(column < m_properties.columnCount());

    //TODO: can we magically improve the creation of the styles?  For now I'll
    //take the naive approach and say no.  There are way, way too many things
    //to take into account so, reusing the styles doesn't seem feasible.

    KoCellStyle::Ptr cellStyle = KoCellStyle::create();

    // When specified, the conditional formats are applied in the following
    // order (therefore subsequent formats override properties on previous
    // formats):
    //
    // * Whole table
    // * Banded columns, even column banding
    // * Banded rows, even row banding
    // * First row, last row
    // * First column, last column
    // * Top left, top right, bottom left, bottom right
    //
    // NOTE: The standard doesn't say what happens to colliding properties in
    // the same hierarchy level (say, it's the first column and last column, or
    // it's first row and last row at once, I assume left to right priority on
    // the elements in the previous list.)
    //
    // See MSOOXML Table Styles §17.7.6 for details.

    const DrawingTableStyleConverterProperties::Roles& role = m_properties.roles();
    const int lastRow = m_properties.rowCount() - 1;
    const int lastColumn = m_properties.columnCount() - 1;

#ifdef MSOOXMLDRAWING_DEBUG_TABLES
    debugMsooXml << "==> [styles.xml] TABLE-level:";
#endif
    applyStyle(DrawingTableStyle::WholeTbl, cellStyle, row, column, spans);
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
    debugMsooXml << "<== [END] [styles.xml] TABLE-level:";
#endif
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
    debugMsooXml << "==> [local] TABLE-level:";
#endif
    TableStyleConverter::applyStyle(m_properties.localDefaultCellStyle(), cellStyle, row, column, spans);
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
    debugMsooXml << "<== [END] [local] TABLE-level:";
#endif

    if (role & DrawingTableStyleConverterProperties::ColumnBanded) {
        // If last column is activated and it's last column, banding is not applied
        if (!(role & DrawingTableStyleConverterProperties::LastCol && column == lastColumn)) {
            // First column active means that the banding is shifted by one
            if (role & DrawingTableStyleConverterProperties::FirstCol) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
                debugMsooXml << "====> [ColumnBanded] FirstCol:";
#endif
                // Banding not applied if it's the first column
                if (column != 0) {
                    // Banding reversed
                    if ((column % (m_properties.columnBandSize() * 2)) < m_properties.columnBandSize()) {
                        applyStyle(DrawingTableStyle::Band2Vertical, cellStyle, row, column, spans);
                    }
                    else {
                        applyStyle(DrawingTableStyle::Band1Vertical, cellStyle, row, column, spans);
                    }
                }
            }
            else {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
                debugMsooXml << "====> [ColumnBanded] Other:";
#endif
                //Is the column in the even band?
                if ((column % (m_properties.columnBandSize() * 2)) < m_properties.columnBandSize()) {
                    applyStyle(DrawingTableStyle::Band1Vertical, cellStyle, row, column, spans);
                }
                else {
                    applyStyle(DrawingTableStyle::Band2Vertical, cellStyle, row, column, spans);
                }
            }
        }
    }

    if (role & DrawingTableStyleConverterProperties::RowBanded) {
        // If last row is activated and it's last row, banding is not applied
        if (!(role & DrawingTableStyleConverterProperties::LastRow && row == lastRow)) {
            // First row active means that the banding is shifted by one
            if (role & DrawingTableStyleConverterProperties::FirstRow) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
                debugMsooXml << "====> [RowBanded] FirstRow:";
#endif
                // Banding not applied if it's the first row
                if (row != 0) {
                    // In case the first row is activated, the banding applying is reversed
                    if( (row % (m_properties.rowBandSize() * 2)) < m_properties.rowBandSize()) {
                        applyStyle(DrawingTableStyle::Band2Horizontal, cellStyle, row, column, spans);
                    }
                    else {
                        applyStyle(DrawingTableStyle::Band1Horizontal, cellStyle, row, column, spans);
                    }
                }
            }
            else {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
                debugMsooXml << "====> [RowBanded] Other:";
#endif
                //Is the row in the even band?
                if( (row % (m_properties.rowBandSize() * 2)) < m_properties.rowBandSize()) {
                    applyStyle(DrawingTableStyle::Band1Horizontal, cellStyle, row, column, spans);
                }
                else {
                    applyStyle(DrawingTableStyle::Band2Horizontal, cellStyle, row, column, spans);
                }
            }
        }
    }

    //NOTE: According to test data, at least for the first table cell the
    //FirstCol format applies before the FirstRow format.  Also the previous
    //approach with RowBanded cells might be an option.

    if(role & DrawingTableStyleConverterProperties::FirstCol) {
        if(column == 0) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> FirstCol:";
#endif
            applyStyle(DrawingTableStyle::FirstCol, cellStyle, row, column, spans);
        }
    }

    if (role & DrawingTableStyleConverterProperties::FirstRow) {
        if(row == 0) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> FirstRow:";
#endif
            applyStyle(DrawingTableStyle::FirstRow, cellStyle, row, column, spans);
        }
    }

    if (role & DrawingTableStyleConverterProperties::LastRow) {
        if(row == lastRow) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> LastRow:";
#endif
            applyStyle(DrawingTableStyle::LastRow, cellStyle, row, column, spans);
        }
    }



    if(role & DrawingTableStyleConverterProperties::LastCol) {
        if(column == lastColumn) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> LastCol:";
#endif
            applyStyle(DrawingTableStyle::LastCol, cellStyle, row, column, spans);
        }
    }

    if(role & DrawingTableStyleConverterProperties::NeCell) {
        if(row == 0 && column == 0) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> NeCell:";
#endif
            applyStyle(DrawingTableStyle::NeCell, cellStyle, row, column, spans);
        }
    }

    if(role & DrawingTableStyleConverterProperties::NwCell) {
        if(row == 0 && column == lastColumn) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> NwCell:";
#endif
            applyStyle(DrawingTableStyle::NwCell, cellStyle, row, column, spans);
        }
    }

    if(role & DrawingTableStyleConverterProperties::SeCell) {
        if(row == lastRow && column == 0) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> SeCell:";
#endif
            applyStyle(DrawingTableStyle::SeCell, cellStyle, row, column, spans);
        }
    }

    if(role & DrawingTableStyleConverterProperties::SwCell) {
        if(row == lastRow && column == lastColumn) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
            debugMsooXml << "====> SwCell:";
#endif
            applyStyle(DrawingTableStyle::SwCell, cellStyle, row, column, spans);
        }
    }

    TableStyleProperties* localProperties = m_properties.localStyles().localStyle(row, -1);
    if (localProperties) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
        debugMsooXml << "==> ROW-level:";
#endif
        TableStyleConverter::applyStyle(localProperties, cellStyle, row, column, spans);
        localProperties = 0;
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
        debugMsooXml << "<== [END] ROW-level:";
#endif
    }

    localProperties = m_properties.localStyles().localStyle(row, column);
    if (localProperties) {
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
        debugMsooXml << "==> CELL-level:";
#endif
        TableStyleConverter::applyStyle(localProperties, cellStyle, row, column, spans);
        localProperties = 0;
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
        debugMsooXml << "<== [END] CELL-level:";
#endif
    }

    //RE-APPLY table-level border properties from tblBorders
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
    debugMsooXml << "==> [REAPPLY]: TABLE/ROW-level properties";
#endif
    TableStyleProperties* tableProperties = 0;
    if (m_style) {
        tableProperties = m_style->properties(DrawingTableStyle::WholeTbl);
    }
    reapplyTableLevelBordersStyle(tableProperties,
                                  m_properties.localDefaultCellStyle(),
                                  m_properties.localStyles().localStyle(row, -1),
                                  cellStyle, row, column, spans);
#ifdef MSOOXMLDRAWING_DEBUG_TABLES
    debugMsooXml << "<== [END] [REAPPLY]: TABLE/ROW-level properties";
#endif

    return cellStyle;
}

DrawingTableStyle::DrawingTableStyle()
: m_properties()
{
}

DrawingTableStyle::~DrawingTableStyle()
{
//     FIXME: we crash because of this.
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
