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

#ifndef _LAYOUT_FACTORY_REGISTRY_H_
#define _LAYOUT_FACTORY_REGISTRY_H_

#include <QPair>

class QString;

class Layout;
class LayoutFactory;

class LayoutFactoryRegistry
{
    LayoutFactoryRegistry();
    ~LayoutFactoryRegistry();
public:
    static LayoutFactoryRegistry* instance();
    void addFactory(LayoutFactory* _factory);
    Layout* createLayout(const QString& id) const;
    QList< QPair<QString, QString> > factories() const;
private:
    struct Private;
    Private* const d;
};

#endif
