/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ChartDatabaseSelectorFactory.h"

#include <KLocalizedString>

#include "ChartDatabaseSelector.h"
#include <KoChartInterface.h>

using namespace Calligra::Sheets;

KoShapeConfigWidgetBase *ChartDatabaseSelectorFactory::createConfigWidget(KoShape *shape)
{
    ChartDatabaseSelector *widget = new ChartDatabaseSelector(m_map);
    widget->open(shape);
    return widget;
}

QString ChartDatabaseSelectorFactory::name() const
{
    return i18n("Database");
}

bool ChartDatabaseSelectorFactory::showForShapeId(const QString &id) const
{
    return id == ChartShapeId;
}
