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

#include "Binding.h"

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

Binding::Binding(Sheet *sheet)
    : d(new Private)
{
    d->model = new BindingModel(sheet);
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
    return d->model->sheet() ? false : d->model->region().isEmpty();
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
    
    if (d->model->sheet())
    {
		Region::ConstIterator end(region.constEnd());
		for (Region::ConstIterator it = region.constBegin(); it != end; ++it)
		{
		    if (d->model->sheet() != (*it)->sheet())
		        continue;
		    rect = (*it)->rect();
		    if (rect.isValid())
		    {
		        d->model->emitDataChanged(rect);
		        changedRegion.add(rect, (*it)->sheet());
		    }
		}
    }
    else
    {
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
    : QAbstractTableModel()
    , KoChart::ChartModel()
    , m_region(region)
{
}

BindingModel::BindingModel( Sheet *sheet )
    : QAbstractTableModel()
    , KoChart::ChartModel()
    , m_sheet(sheet)
{
}

QVariant BindingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( (!m_sheet && m_region.isEmpty()) || (role != Qt::EditRole && role != Qt::DisplayRole))
        return QVariant();
    const QPoint offset = m_sheet ? QPoint(1, 1) : m_region.firstRange().topLeft();
    const int col = (orientation == Qt::Vertical) ? offset.x() : offset.x() + section;
    const int row = (orientation == Qt::Vertical) ? offset.y() + section : offset.y();
    const Sheet* sheet = m_sheet ? m_sheet : m_region.firstSheet();
    const Value value = sheet->cellStorage()->value(col, row);
    return value.asVariant();
}

int BindingModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (m_sheet)
    	return m_sheet->cellStorage()->rows();
    return m_region.isEmpty() ? 0 : m_region.firstRange().height();
}

int BindingModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (m_sheet)
    	return m_sheet->cellStorage()->columns();
    return m_region.isEmpty() ? 0 : m_region.firstRange().width();
}

QString BindingModel::regionToString( const QVector<QRect> &region ) const
{
	Region r;
	foreach( QRect rect, region )
	   r.add( rect, m_sheet );
	return r.name();
}

QVector<QRect> BindingModel::stringToRegion( const QString &string ) const
{
	const Region r( string, m_sheet->map() );
	return r.rects();
}

QVariant BindingModel::data(const QModelIndex& index, int role) const
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

const KSpread::Region& BindingModel::region() const
{
    return m_region;
}

void BindingModel::setRegion(const Region& region)
{
    m_region = region;
    m_sheet = 0;
}

Sheet *BindingModel::sheet() const
{
    return m_sheet;
}

void BindingModel::setSheet(Sheet *sheet)
{
	m_sheet = sheet;
	m_region = Region();
}

void BindingModel::emitDataChanged(const QRect& rect)
{
    const QPoint tl = rect.topLeft();
    const QPoint br = rect.bottomRight();
    emit dataChanged(index(tl.y(), tl.x()), index(br.y(), br.x()));
}

void BindingModel::emitChanged(const Region& region)
{
    emit changed(region);
}

#include "Binding.moc"
