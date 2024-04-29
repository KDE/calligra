/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCHART_CHARTTABLEMODEL_H
#define KCHART_CHARTTABLEMODEL_H

// Qt
#include <QStandardItemModel>
#include <QVector>

// Interface
#include <interfaces/KoChartModel.h>

// KoChart
#include "ChartShape.h"

class QString;

// FIXME: Should it inherit QAbstractTableModel instead?

namespace KoChart
{

/**
 * @brief The ChartTableModel class is used to store a data table inside a chart document.  It's main difference to a normal table model is that it can load and
 * store to ODF.
 */
class ChartTableModel : public QStandardItemModel, public KoChart::ChartModel
{
    Q_OBJECT
    Q_INTERFACES(KoChart::ChartModel)

public:
    explicit ChartTableModel(QObject *parent = nullptr);
    ~ChartTableModel();

    QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const override;

    // KoChart::ChartModel interface
    QHash<QString, QVector<QRect>> cellRegion() const override;
    bool setCellRegion(const QString &regionName) override;
    bool isCellRegionValid(const QString &regionName) const override;

    // Load and save the contained data in an ODF DOM tree.
    // FIXME: Move saving/loading logic to helper class like TableLoaderAndSaver,
    // this class is outdated and should be removed.
    bool loadOdf(const KoXmlElement &tableElement, KoShapeLoadingContext &context);
    bool saveOdf(KoXmlWriter &bodyWriter, KoGenStyles &mainStyles) const;
};

}

#endif // KCHART_TABLEMODEL_H
