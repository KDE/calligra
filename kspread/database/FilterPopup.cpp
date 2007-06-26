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

#include "FilterPopup.h"

#include <QCheckBox>
#include <QList>
#include <QVBoxLayout>

#include <klocale.h>

#include "CellStorage.h"
#include "DatabaseRange.h"
#include "Doc.h"
#include "Region.h"
#include "Sheet.h"
#include "ValueConverter.h"

using namespace KSpread;

class FilterPopup::Private
{
public:
    QList<QCheckBox*> items;
};

FilterPopup::FilterPopup(QWidget* parent, const DatabaseRange& database)
    : QFrame(parent, Qt::Popup)
    , d(new Private)
{
    setBackgroundRole(QPalette::Base);
    setFrameStyle(QFrame::Panel | QFrame::Raised);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(3);
    layout->setSpacing(0);

    layout->addWidget(new QCheckBox(i18n("All"), this));
    layout->addWidget(new QCheckBox(i18n("Top 10"), this));
    layout->addWidget(new QCheckBox(i18n("Empty"), this));
    layout->addWidget(new QCheckBox(i18n("Non-empty"), this));
    layout->addSpacing(3);

    const Sheet* sheet = (*database.range().constBegin())->sheet();
    const QRect range = database.range().lastRange();
    QCheckBox* item;
    // FIXME Stefan: Horizontal/vertical filtering
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
        const Value value = sheet->cellStorage()->value(range.left()/*FIXME*/, row);
        item = new QCheckBox(this);
        item->setText(sheet->doc()->converter()->asString(value).asString());
        layout->addWidget(item);
        d->items.append(item);
    }
}

FilterPopup::~FilterPopup()
{
    delete d;
}
