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
#include "BindingModel.h"

#include <QRect>

#include <kdebug.h>

#include "CellStorage.h"
#include "Map.h"
#include "Sheet.h"
#include "Value.h"

using namespace Calligra::Sheets;

class Binding::Private : public QSharedData
{
public:
    BindingModel* model;
    Private(Binding *q) : model(new BindingModel(q)) {}
    ~Private() { delete model; }
};


Binding::Binding()
    : d(new Private(this))
{
}

Binding::Binding(const Region& region)
    : d(new Private(this))
{
    Q_ASSERT(region.isValid());
    d->model->setRegion(region);
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

QAbstractItemModel* Binding::model() const
{
    return d->model;
}

const Calligra::Sheets::Region& Binding::region() const
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
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        if (sheet != (*it)->sheet())
            continue;
        rect = range & (*it)->rect();
        rect.translate(-offset.x(), -offset.y());
        if (rect.isValid()) {
            d->model->emitDataChanged(rect);
            changedRegion.add(rect, (*it)->sheet());
        }
    }
    d->model->emitChanged(changedRegion);
}

void Binding::operator=(const Binding & other)
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

QHash<QString, QVector<QRect> > BindingModel::cellRegion() const
{
    QHash<QString, QVector<QRect> > answer;
    Region::ConstIterator end = m_region.constEnd();
    for (Region::ConstIterator it = m_region.constBegin(); it != end; ++it) {
        if (!(*it)->isValid()) {
            continue;
        }
        answer[(*it)->name()].append((*it)->rect());
    }
    return answer;
}

bool BindingModel::setCellRegion(const QString& regionName)
{
    Q_ASSERT(m_region.isValid());
    Q_ASSERT(m_region.firstSheet());
    const Map* const map = m_region.firstSheet()->map();
    const Region region = Region(regionName, map);
    if (!region.isValid()) {
        kDebug() << qPrintable(regionName) << "is not a valid region.";
        return false;
    }
    // Clear the old binding.
    Region::ConstIterator end = m_region.constEnd();
    for (Region::ConstIterator it = m_region.constBegin(); it != end; ++it) {
        if (!(*it)->isValid()) {
            continue;
        }
        // FIXME Stefan: This may also clear other bindings!
        (*it)->sheet()->cellStorage()->setBinding(Region((*it)->rect(), (*it)->sheet()), Binding());
    }
    // Set the new region
    m_region = region;
    end = m_region.constEnd();
    for (Region::ConstIterator it = m_region.constBegin(); it != end; ++it) {
        if (!(*it)->isValid()) {
            continue;
        }
        (*it)->sheet()->cellStorage()->setBinding(Region((*it)->rect(), (*it)->sheet()), *m_binding);
    }
    return true;
}



//#include "moc_Binding.cpp"
