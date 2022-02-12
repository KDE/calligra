/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
    SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_BINDING_MODEL
#define CALLIGRA_SHEETS_BINDING_MODEL

#include "engine/Region.h"

#include <KoChartModel.h>

#include <QAbstractTableModel>

#include "sheets_core_export.h"

namespace Calligra
{
namespace Sheets
{
class Binding;

/**
 * A model for a cell range acting as data source.
 */
class CALLIGRA_SHEETS_CORE_EXPORT BindingModel : public QAbstractTableModel, public KoChart::ChartModel
{
    Q_OBJECT
    Q_INTERFACES(KoChart::ChartModel)
public:
    explicit BindingModel(Binding* binding, QObject *parent = 0);

    // QAbstractTableModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    // KoChart::ChartModel interface
    QHash<QString, QVector<QRect> > cellRegion() const override;
    bool setCellRegion(const QString& regionName) override;
    bool isCellRegionValid(const QString& regionName) const override;

    const Region& region() const;
    void setRegion(const Region& region);

    void emitDataChanged(const QRect& range);
    void emitChanged(const Region& region);

Q_SIGNALS:
    void changed(const Region& region);

private:
    Region m_region;
    Binding* m_binding;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_BINDING_MODEL
