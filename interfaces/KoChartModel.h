/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KO_CHART_MODEL
#define KO_CHART_MODEL

#include <QHash>
#include <QVector>
#include <QtPlugin>

class QRect;

namespace KoChart
{

/**
 * Item data role used to retrieve the string representing the data area
 * of a row or a column.
 *
 * Example:
 * The area string of the 3rd row would be retrieved with this line of code:
 * QString area = model->headerData( 2, Qt::Vertical, SECTION_AREA_ROLE );
 * ("$Table2.$D9:$D13", for instance)
 */
const int SECTION_AREA_ROLE = 32;

/**
 * Item data role used to retrieve the string representing the data cell
 * of a header. The header data usually is a name for the dataset.
 *
 * Example:
 * The area string of the name of the data series that has its y-values
 * in the 5th column could be retrieved with this line of code:
 * QString cell = model->headerData( 4, Qt::Horizontal, HEADER_AREA_ROLE );
 * ("$Table1.$C8", for instance)
 */
const int HEADER_AREA_ROLE = 33;

/**
 * The ChartModel class implements a model that can be filled and
 * passed on to KChart to provide the data used within the chart.
 */
class ChartModel
{
public:
    virtual ~ChartModel() = default;

    /**
     * \return the cell region in ranges ordered by sheet name
     */
    virtual QHash<QString, QVector<QRect>> cellRegion() const = 0;

    /**
     * Sets the cell region.
     * \return \c true on success
     */
    virtual bool setCellRegion(const QString &regionName) = 0;

    /**
     * \return \c true if the cell region is valid
     */
    virtual bool isCellRegionValid(const QString &regionName) const = 0;
};

} // Namespace KoChart

Q_DECLARE_INTERFACE(KoChart::ChartModel, "org.calligra.KoChart.ChartModel:1.0")

#endif // KO_CHART_MODEL
