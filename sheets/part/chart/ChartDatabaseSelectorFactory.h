/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHART_DATABASE_SELECTOR_FACTORY
#define CHART_DATABASE_SELECTOR_FACTORY

#include <KoShapeConfigFactoryBase.h>
#include <QObject>

#include "core/Map.h"

class KoShape;

namespace Calligra
{
namespace Sheets
{

/// factory to create a ChartDatabaseSelector widget
class ChartDatabaseSelectorFactory : public QObject, public KoShapeConfigFactoryBase
{
    Q_OBJECT
public:
    /// constructor
    ChartDatabaseSelectorFactory(Map *map)
        : QObject(map)
        , m_map(map)
    {
    }
    ~ChartDatabaseSelectorFactory() override = default;

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase *createConfigWidget(KoShape *shape) override;
    /// reimplemented method from superclass
    QString name() const override;

    /// reimplemented method from superclass
    bool showForShapeId(const QString &id) const override;
    /// reimplemented method from superclass
    int sortingOrder() const override
    {
        return 1;
    }

private:
    Map *m_map;
};

} // namespace Sheets
} // namespace Calligra

#endif // CHART_DATABASE_SELECTOR_FACTORY
