/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
    Copyright (C) 2008 Thomas Zander <zander@kde.org>

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

#ifndef KSPREAD_BINDING_MODEL
#define KSPREAD_BINDING_MODEL

#include "Region.h"

#include <KoChartModel.h>

#include <QAbstractTableModel>

namespace KSpread
{
class Binding;

/**
 * A model for a cell range acting as data source.
 */
class BindingModel : public QAbstractTableModel, public KoChart::ChartModel
{
    Q_OBJECT
    Q_INTERFACES(KoChart::ChartModel)
public:
    explicit BindingModel(Binding* binding, QObject *parent = 0);

    // QAbstractTableModel interface
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    // KoChart::ChartModel interface
    virtual QHash<QString, QVector<QRect> > cellRegion() const;
    virtual bool setCellRegion(const QString& regionName);
    virtual bool isCellRegionValid(const QString& regionName) const;

    const Region& region() const;
    void setRegion(const Region& region);

    void emitDataChanged(const QRect& range);
    void emitChanged(const Region& region);

signals:
    void changed(const Region& region);

private:
    Region m_region;
    Binding* m_binding;
};

} // namespace KSpread

#endif // KSPREAD_BINDING_MODEL
