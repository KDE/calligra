// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2008 Thomas Zander <zander@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "Binding.h"
#include "BindingModel.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN Binding::Private : public QSharedData
{
public:
    BindingModel *model;
    Private(Binding *q)
        : model(new BindingModel(q))
    {
    }
    ~Private()
    {
        delete model;
    }
};

Binding::Binding()
    : d(new Private(this))
{
}

Binding::Binding(const Region &region)
    : d(new Private(this))
{
    Q_ASSERT(region.isValid());
    d->model->setRegion(region);
}

Binding::Binding(const Binding &other)

    = default;

Binding::~Binding() = default;

bool Binding::isEmpty() const
{
    return d->model->region().isEmpty();
}

Calligra::Sheets::BindingModel *Binding::model() const
{
    return d->model;
}

const Calligra::Sheets::Region &Binding::region() const
{
    return d->model->region();
}

void Binding::setRegion(const Region &region)
{
    d->model->setRegion(region);
}

void Binding::update(const Region &region)
{
    QRect rect;
    Region changedRegion;
    const QPoint offset = d->model->region().firstRange().topLeft();
    const QRect range = d->model->region().firstRange();
    const SheetBase *sheet = d->model->region().firstSheet();
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

void Binding::operator=(const Binding &other)
{
    d = other.d;
}

bool Binding::operator==(const Binding &other) const
{
    return d == other.d;
}

bool Binding::operator<(const Binding &other) const
{
    return d < other.d;
}
