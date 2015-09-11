/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "LayoutFactoryRegistry.h"

#include <QMap>

#include "LayoutFactory.h"
#include "Layout.h"
#include "layouts/FreeLayout.h"
#include "layouts/ColumnLayout.h"

struct LayoutFactoryRegistry::Private {
    static LayoutFactoryRegistry* s_instance;
    QMap< QString, LayoutFactory* > factories;
};

LayoutFactoryRegistry* LayoutFactoryRegistry::Private::s_instance = 0;

LayoutFactoryRegistry::LayoutFactoryRegistry() : d(new Private)
{
    addFactory(new FreeLayoutFactory);
    addFactory(new ColumnLayoutFactory);
}

LayoutFactoryRegistry::~LayoutFactoryRegistry()
{
    delete d;
}

LayoutFactoryRegistry* LayoutFactoryRegistry::instance()
{
    if(!Private::s_instance) {
        Private::s_instance = new LayoutFactoryRegistry;
    }
    return Private::s_instance;
}

void LayoutFactoryRegistry::addFactory(LayoutFactory* _factory)
{
    d->factories[_factory->id()] = _factory;
}

Layout* LayoutFactoryRegistry::createLayout(const QString& id) const
{
    LayoutFactory* factory = d->factories.value(id);
    if(factory) {
        Layout* layout = factory->createLayout();
        Q_ASSERT(layout->id() == id);
        return layout;
    } else {
        return 0;
    }
}

QList< QPair<QString, QString> > LayoutFactoryRegistry::factories() const
{
    QList< QPair<QString, QString> > lists;
    foreach(LayoutFactory * factory, d->factories) {
        lists.push_back(QPair<QString, QString>(factory->id(), factory->name()));
    }
    return lists;
}
