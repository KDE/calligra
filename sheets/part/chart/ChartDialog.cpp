/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ChartDialog.h"

#include "ChartDatabaseSelectorFactory.h"

using namespace Calligra::Sheets;

ChartDialog::ChartDialog(const QList<KoChart::ChartShape *> &charts, QWidget *parent)
    : KPageDialog(parent)
{
    Q_UNUSED(charts);
}

ChartDialog::~ChartDialog() = default;

// static
QList<KoShapeConfigFactoryBase *> ChartDialog::panels(Map *map)
{
    QList<KoShapeConfigFactoryBase *> answer;
    answer.append(new ChartDatabaseSelectorFactory(map));
    return answer;
}
