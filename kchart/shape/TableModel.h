/* This file is part of the KDE project

   Copyright 2008 Johannes Simon <johannes.simon@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#ifndef KCHART_TABLEMODEL_H
#define KCHART_TABLEMODEL_H


// Qt
#include <QVector>
#include <QStandardItemModel>

// Interface
#include <interfaces/KoChartModel.h>

// KChart
#include "ChartShape.h"


class QString;

// FIXME: Rename to ChartTableModel.

namespace KChart {

/**
 * @brief The TableModel class is used to store a data table inside a chart document.
 */
class CHARTSHAPELIB_EXPORT TableModel : public QStandardItemModel, public KoChart::ChartModel
{
    Q_OBJECT
    Q_INTERFACES(KoChart::ChartModel)

public:
    TableModel( QObject *parent = 0 );
    ~TableModel();

    // KoChart::ChartModel interface
    virtual QHash<QString, QVector<QRect> > cellRegion() const;
    virtual bool setCellRegion(const QString& regionName);
    virtual bool isCellRegionValid(const QString& regionName) const;

    // Load and save the contained data in an ODF DOM tree.
    void loadOdf( const KoXmlElement &tableElement, 
                  KoShapeLoadingContext &context );
    bool saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles ) const;
};

}

#endif // KCHART_TABLEMODEL_H
