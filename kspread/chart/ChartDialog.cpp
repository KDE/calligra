/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Thomas Zander <zander@kde.org>

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

#include "ChartDialog.h"

#include <koChart.h>

#include "ChartDatabaseSelectorFactory.h"

using namespace KSpread;

ChartDialog::ChartDialog(const QList<KChart::ChartShape*> &charts, QWidget *parent)
        : KPageDialog(parent)
{
    Q_UNUSED(charts);
    connect(this, SIGNAL(okClicked()), this, SLOT(okClicked()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(cancelClicked()));
}

ChartDialog::~ChartDialog()
{
}

void ChartDialog::okClicked()
{
}

void ChartDialog::cancelClicked()
{
}

// static
QList<KoShapeConfigFactoryBase*> ChartDialog::panels(Map *map)
{
    QList<KoShapeConfigFactoryBase*> answer;
    answer.append(new ChartDatabaseSelectorFactory(map));
    return answer;
}

#include "ChartDialog.moc"
