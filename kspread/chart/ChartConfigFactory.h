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

#ifndef KSPREAD_CHART_CONFIG_FACTORY
#define KSPREAD_CHART_CONFIG_FACTORY

#include <KoShapeConfigFactory.h>

class KoShape;

namespace KSpread
{

/// factory to create a ChartDatabaseSelector widget
class ChartDatabaseSelectorFactory : public KoShapeConfigFactory
{
public:
    /// constructor
    ChartDatabaseSelectorFactory() {}
    ~ChartDatabaseSelectorFactory() {}

    /// reimplemented method from superclass
    KoShapeConfigWidgetBase* createConfigWidget(KoShape* shape);
    /// reimplemented method from superclass
    QString name() const;

    /// reimplemented method from superclass
    bool showForShapeId(const QString &id) const;
    /// reimplemented method from superclass
    int sortingOrder() const { return 1; }

private:
};

} // namespace KSpread

#endif // KSPREAD_CHART_CONFIG_FACTORY
