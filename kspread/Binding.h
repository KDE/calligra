/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_BINDING
#define KSPREAD_BINDING

#include <interfaces/KoChartModel.h>
#include <QSharedDataPointer>
#include <QVariant>
#include <QAbstractItemModel>

#include "kspread_export.h"
#include "Region.h"

namespace KSpread
{

class BindingModel;

class BindingModelContainer : public KoChart::ChartModel
{
    Q_OBJECT

public:
    BindingModelContainer(const Region& region);

    const Region& region() const;
    void setRegion(const Region& region);

    void emitDataChanged(const QRect& range);
    void emitChanged(const Region& region);

    // Reimplemented methods from KoChartModel
    virtual QString regionToString( const QVector<QRect> &region ) const;
    virtual QVector<QRect> stringToRegion( const QString &string ) const;
    virtual QHash<QString, QVector<QRect> > cellRegion() const;
    virtual bool setCellRegion(const QString& regionName);
    virtual QAbstractItemModel* model();

signals:
    void changed(const Region& region);

private:
    BindingModel *m_model;
};

/**
 * Abstracts read-only access to the ValueStorage.
 * Useful for KChart (or other apps, that want read-only access to KSpread's data).
 *
 * If a cell in the region is updated, the BindingManager informs this Binding, which
 * in turn informs the model it holds.
 */
class KSPREAD_EXPORT Binding
{
public:
    Binding();
    explicit Binding(const Region& region);
    Binding( const Binding& other );
    ~Binding();

    bool isEmpty() const;

    KoChart::ChartModel* model() const;

    const Region& region() const;
    void setRegion(const Region& region);

    void update(const Region& region);

    void operator=( const Binding& other );
    bool operator==( const Binding& other ) const;
    bool operator<( const Binding& other ) const;

private:
    class Private;
    QExplicitlySharedDataPointer<Private> d;
};

} // namespace KSpread

Q_DECLARE_TYPEINFO( KSpread::Binding, Q_MOVABLE_TYPE );

#endif // KSPREAD_BINDING
