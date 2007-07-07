/*
 * This file is part of KSpread
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ScriptingListener.h"

#include <klocale.h>

#include <Sheet.h>
#include <Cell.h>

using namespace KSpread;

namespace KSpread {
    /// \internal d-pointer class.
    class ScriptingCellListener::Private
    {
        public:
            KSpread::CellBinding* cellbinding;
    };
}

ScriptingCellListener::ScriptingCellListener(KSpread::Sheet *sheet, const QRect& area)
    : QObject()
    , d(new Private())
{
    d->cellbinding = new KSpread::CellBinding(sheet, area);
    connect(d->cellbinding, SIGNAL(changed(Cell)), this, SLOT(slotChanged(Cell)));
    sheet->addCellBinding(d->cellbinding);
}

ScriptingCellListener::~ScriptingCellListener()
{
    delete d->cellbinding;
    delete d;
}

void ScriptingCellListener::slotChanged(const Cell& cell)
{
    emit changed( cell.column(), cell.row() );
}

#include "ScriptingListener.moc"
