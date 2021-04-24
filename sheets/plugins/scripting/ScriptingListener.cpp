// This file is part of KSpread
// SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "ScriptingListener.h"

#include <Binding.h>
#include <BindingModel.h>
#include <CellStorage.h>
#include <Sheet.h>

#include <QAbstractItemModel>

using namespace Calligra::Sheets;

namespace Calligra
{
namespace Sheets
{
/// \internal d-pointer class.
class ScriptingCellListener::Private
{
public:
    Calligra::Sheets::Sheet* sheet;
    Calligra::Sheets::Binding* cellbinding;
};
} // namespace Sheets
} // namespace Calligra

ScriptingCellListener::ScriptingCellListener(Calligra::Sheets::Sheet *sheet, const QRect& area)
        : QObject()
        , d(new Private())
{
    d->sheet = sheet;
    d->cellbinding = new Calligra::Sheets::Binding(Region(area, sheet));
    connect(d->cellbinding->model(), &BindingModel::changed, this, &ScriptingCellListener::slotChanged);
    sheet->cellStorage()->setBinding(Region(area, sheet), *d->cellbinding);
}

ScriptingCellListener::~ScriptingCellListener()
{
    delete d->cellbinding;
    delete d;
}

void ScriptingCellListener::slotChanged(const Region& region)
{
    Region::ConstIterator end(region.constEnd());

    QVariantList ranges;
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it)
        ranges << (*it)->rect();
    emit regionChanged(ranges);

    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        const QRect r((*it)->rect());
        for (int row = r.top(); row <= r.bottom(); ++row)
            for (int col = r.left(); col <= r.right(); ++col)
                emit cellChanged(col, row);
    }
}
