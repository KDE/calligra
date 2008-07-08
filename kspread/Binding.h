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
#include <QAbstractTableModel>
#include <QSharedDataPointer>
#include <QVariant>

#include "kspread_export.h"
#include "Region.h"

namespace KSpread
{

class BindingModel : public QAbstractTableModel, public KoChart::ChartModel
{
    Q_OBJECT

public:
	BindingModel(const Region& region);
    BindingModel(Sheet *sheet);

    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const;
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

    const Region& region() const;
    void setRegion(const Region& region);
    
    Sheet *sheet() const;
    void setSheet(Sheet *sheet);

    void emitDataChanged(const QRect& range);
    void emitChanged(const Region& region);
    
    // Reimplemented methods from KoChartModel
    QString regionToString( const QVector<QRect> &region ) const;
    QVector<QRect> stringToRegion( const QString &string ) const;

Q_SIGNALS:
    void changed(const Region& region);

private:
	Region m_region;
    Sheet *m_sheet;
};

/**
 * Abstracts read-only access to the ValueStorage.
 * Useful for KChart (or other apps, that want read-only access to KSpread's data).
 */
class KSPREAD_EXPORT Binding
{
public:
    Binding();
    explicit Binding(Sheet *sheet);
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
