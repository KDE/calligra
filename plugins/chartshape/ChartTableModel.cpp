/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "ChartTableModel.h"

// C
#include <cmath>

// Qt
#include <QDomDocument>
#include <QDomNode>

// Calligra
#include <KoGenStyles.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

// KoChart
#include "CellRegion.h"
#include "ChartDebug.h"
#include "OdfLoadingHelper.h"

namespace KoChart
{

ChartTableModel::ChartTableModel(QObject *parent /* = 0 */)
    : QStandardItemModel(parent)
{
}

ChartTableModel::~ChartTableModel() = default;

QVariant ChartTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            return CellRegion::columnName(section + 1);
        }
    }
    return QStandardItemModel::headerData(section, orientation, role);
}

QHash<QString, QVector<QRect>> ChartTableModel::cellRegion() const
{
    // FIXME: Unimplemented?
    return QHash<QString, QVector<QRect>>();
}

bool ChartTableModel::setCellRegion(const QString & /*regionName*/)
{
#if 0 // FIXME: What does this code do?
    int result = 0;

    const int size = regionName.size();
    for (int i = 0; i < size; i++) {
        result += (CellRegion::rangeCharToInt(regionName[i].toLatin1())
                   * std::pow(10.0, (size - i - 1)));
    }

    return result;
#endif
    return true;
}

bool ChartTableModel::isCellRegionValid(const QString &regionName) const
{
    Q_UNUSED(regionName);

    return true;
}

bool ChartTableModel::loadOdf(const KoXmlElement &tableElement, KoShapeLoadingContext &context)
{
    Q_UNUSED(context);

    debugChartOdf << "Load table";
    setRowCount(0);
    setColumnCount(0);

    // QDomDocument doc;
    // KoXml::asQDomElement(doc, tableElement);
    // QTextStream stream(stdout);
    // stream << doc.documentElement();

    int row = 0;
    KoXmlElement n;
    forEachElement(n, tableElement)
    {
        if (n.namespaceURI() != KoXmlNS::table)
            continue;

        if (n.localName() == "table-columns" || n.localName() == "table-header-columns") {
            int column = 0;
            KoXmlElement _n;
            forEachElement(_n, n)
            {
                if (_n.namespaceURI() != KoXmlNS::table || _n.localName() != "table-column")
                    continue;
                column += qMax(1, _n.attributeNS(KoXmlNS::table, "number-columns-repeated").toInt());
                if (column > columnCount())
                    setColumnCount(column);
            }
        } else if (n.localName() == "table-rows" || n.localName() == "table-header-rows") {
            if (n.localName() == "table-header-rows") {
                if (row >= 1) {
                    // There can only be one header-row and only at the very beginning.
                    // So, ignore all following header-rows to be sure our internal
                    // table doesn't start at the wrong offset or something like that.
                    continue;
                }
            }

            KoXmlElement _n;
            forEachElement(_n, n)
            {
                if (_n.namespaceURI() != KoXmlNS::table || _n.localName() != "table-row")
                    continue;

                // Add a row to the internal representation.
                setRowCount(row + 1);

                // Loop through all cells in a table row.
                int column = 0;
                KoXmlElement __n;
                forEachElement(__n, _n)
                {
                    if (__n.namespaceURI() != KoXmlNS::table || __n.localName() != "table-cell")
                        continue;

                    // We have a cell so be sure our column-counter is increased right now so
                    // any 'continue' coming now will leave with the correct value for the next
                    // cell we deal with.
                    ++column;

                    // If this row is wider than any previous one, then add another column.
                    if (column > columnCount())
                        setColumnCount(column);

                    const QString valueType = __n.attributeNS(KoXmlNS::office, "value-type");
                    QString valueString = __n.attributeNS(KoXmlNS::office, "value");
                    const KoXmlElement valueElement = __n.namedItemNS(KoXmlNS::text, "p").toElement();
                    if ((valueElement.isNull() || !valueElement.isElement()) && valueString.isEmpty())
                        continue;

                    // Read the actual value in the cell.
                    QVariant value;
                    if (valueString.isEmpty())
                        valueString = valueElement.text().trimmed();
                    if (valueType == "float")
                        value = valueString.toDouble();
                    else if (valueType == "boolean")
                        value = (bool)valueString.toInt();
                    else // if (valueType == "string")
                        value = valueString;

                    setData(index(row, column - 1), value);
                } // foreach table:table-cell
                ++row;

            } // foreach table:table-row
        }
    }
    debugChartOdf << "Loaded table:" << rowCount() << ',' << columnCount();
    return true;
}

bool ChartTableModel::saveOdf(KoXmlWriter &bodyWriter, KoGenStyles &mainStyles) const
{
    Q_UNUSED(bodyWriter);
    Q_UNUSED(mainStyles);
    // The save logic is in ChartShape::saveOdf
    return true;
}

}
