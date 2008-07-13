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

#include "Binding.h"
#include "Binding_p.h"

#include <QRect>

#include <kdebug.h>

#include "CellStorage.h"
#include "Map.h"
#include "Sheet.h"
#include "Value.h"

using namespace KSpread;

class Binding::Private : public QSharedData
{
public:
    Private() : model(0) {}
    ~Private() { delete model; }

    BindingModel* model;
};


Binding::Binding()
    : d(new Private)
{
}

Binding::Binding(const Region& region)
    : d(new Private)
{
    Q_ASSERT(region.isValid());
    d->model = new BindingModel(region);
}

Binding::Binding(const Binding& other)
    : d(other.d)
{
}

Binding::~Binding()
{
}

bool Binding::isEmpty() const
{
    return d->model->region().isEmpty();
}

KoChart::ChartModel* Binding::model() const
{
    return d->model;
}

const KSpread::Region& Binding::region() const
{
    return d->model->region();
}

void Binding::setRegion(const Region& region)
{
    d->model->setRegion(region);
}

void Binding::update(const Region& region)
{
    QRect rect;
    Region changedRegion;
    const QPoint offset = d->model->region().firstRange().topLeft();
    const QRect range = d->model->region().firstRange();
    const Sheet* sheet = d->model->region().firstSheet();
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it)
    {
        if (sheet != (*it)->sheet())
            continue;
        rect = range & (*it)->rect();
        rect.translate( -offset.x(), -offset.y() );
        if (rect.isValid())
        {
            d->model->emitDataChanged(rect);
            changedRegion.add(rect, (*it)->sheet());
        }
    }
    d->model->emitChanged(changedRegion);
}

void Binding::operator=(const Binding& other)
{
    d = other.d;
}

bool Binding::operator==(const Binding& other) const
{
    return d == other.d;
}

bool Binding::operator<(const Binding& other) const
{
    return d < other.d;
}

BindingModel::BindingModel(const Region& region)
    : KoChart::ChartModel(),
    m_model( new BindingModelModel(this) )
{
    m_model->setRegion(region);
    connect (m_model, SIGNAL(changed(const Region&)), this, SIGNAL(changed(const Region&)));
}

QString BindingModel::regionToString( const QVector<QRect> &region ) const
{
	Region r;
	foreach( QRect rect, region )
        r.add( rect, m_model->region().firstSheet() );
	return r.name();
}

QVector<QRect> BindingModel::stringToRegion( const QString &string ) const
{
    const Region r( string, m_model->region().firstSheet()->map() );
	return r.rects();
}

const KSpread::Region& BindingModel::region() const
{
    return m_model->region();
}

void BindingModel::setRegion(const Region& region)
{
    m_model->setRegion(region);
}

void BindingModel::emitDataChanged(const QRect& rect)
{
    m_model->emitDataChanged(rect);
}

void BindingModel::emitChanged(const Region& region)
{
    m_model->emitChanged(region);
}

QAbstractItemModel* BindingModel::model()
{
    return m_model;
}


/////// BindingModelModel

BindingModelModel::BindingModelModel(QObject *parent)
    : QAbstractTableModel(parent),
    m_sheet(0)
{
}

void BindingModelModel::emitChanged(const Region& region)
{
    emit changed(region);
}

void BindingModelModel::emitDataChanged(const QRect& rect)
{
    const QPoint tl = rect.topLeft();
    const QPoint br = rect.bottomRight();
    kDebug() << "emit QAbstractItemModel::dataChanged(" << index(tl.y(), tl.x()) << ", " << index(br.y(), br.x()) << ");";
    emit dataChanged(index(tl.y(), tl.x()), index(br.y(), br.x()));
}

QVariant BindingModelModel::data(const QModelIndex& index, int role) const
{
    if ((!m_sheet && m_region.isEmpty()) || (role != Qt::EditRole && role != Qt::DisplayRole))
        return QVariant();
    const QPoint offset = m_sheet ? QPoint(1,1) : m_region.firstRange().topLeft();
    const Sheet* sheet = m_sheet ? m_sheet : m_region.firstSheet();
    const Value value = sheet->cellStorage()->value(offset.x() + index.column(),
                                                     offset.y() + index.row());
    // KoChart::Value is either:
    //  - a double (interpreted as a value)
    //  - a QString (interpreted as a label)
    //  - a QDateTime (interpreted as a date/time value)
    //  - Invalid (interpreted as empty)
    QVariant variant;
    switch (value.type())
    {
        case Value::Float:
        case Value::Integer:
            if (value.format() == Value::fmt_DateTime ||
                 value.format() == Value::fmt_Date ||
                 value.format() == Value::fmt_Time)
            {
                variant.setValue<QDateTime>(value.asDateTime(sheet->map()->calculationSettings()));
                break;
            }
        case Value::Boolean:
        case Value::Complex:
        case Value::Array:
            variant.setValue<double>(numToDouble (value.asFloat()));
            break;
        case Value::String:
        case Value::Error:
            variant.setValue<QString>(value.asString());
            break;
        case Value::Empty:
        case Value::CellRange:
        default:
            break;
    }
    //kDebug() << index.column() <<"," << index.row() <<"," << variant;
    return variant;
}

const KSpread::Region& BindingModelModel::region() const
{
    return m_region;
}

QVariant BindingModelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( (m_region.isEmpty()) || (role != Qt::EditRole && role != Qt::DisplayRole))
        return QVariant();
    const QPoint offset = m_region.firstRange().topLeft();
    const int col = (orientation == Qt::Vertical) ? offset.x() : offset.x() + section;
    const int row = (orientation == Qt::Vertical) ? offset.y() + section : offset.y();
    const Sheet* sheet = m_region.firstSheet();
    const Value value = sheet->cellStorage()->value(col, row);
    return value.asVariant();
}

int BindingModelModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_region.isEmpty() ? 0 : m_region.firstRange().height();
}

int BindingModelModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_region.isEmpty() ? 0 : m_region.firstRange().width();
}

void BindingModelModel::setRegion(const Region& region)
{
    m_region = region;
}

#include "Binding.moc"
#include "Binding_p.moc"
