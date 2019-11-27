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

#ifndef CHART_DATABASE_SELECTOR_FACTORY
#define CHART_DATABASE_SELECTOR_FACTORY

#include <QObject>
#include <KoShapeConfigFactoryBase.h>
#include "../Map.h"

class KoShape;

namespace Calligra
{
namespace Sheets
{
class Map;

/// factory to create a ChartDatabaseSelector widget
class ChartDatabaseSelectorFactory : public QObject, public KoShapeConfigFactoryBase
{
    Q_OBJECT
public:
    /// constructor
    ChartDatabaseSelectorFactory(Map *map) : QObject(map), m_map(map) {}
    ~ChartDatabaseSelectorFactory() override {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase* createConfigWidget(KoShape* shape) override;
    /// reimplemented method from superclass
    QString name() const override;

    /// reimplemented method from superclass
    bool showForShapeId(const QString &id) const override;
    /// reimplemented method from superclass
    int sortingOrder() const override {
        return 1;
    }

private:
    Map* m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif // CHART_DATABASE_SELECTOR_FACTORY
