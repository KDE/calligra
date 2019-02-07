/* Part of the Calligra project
 * Copyright (C) 2019 Dag Andersen <danders@get2net.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QObject>
#include <QMap>
#include <QString>

class CollectionItemModel;

class StencilBoxDockerLoader : public QObject
{
    Q_OBJECT
public:
    StencilBoxDockerLoader(StencilBoxDocker *parent) : q(parent) {}

    bool addCollection(const QString& path);

    QMap<QString, CollectionItemModel*> m_modelMap;

public Q_SLOTS:
    void loadShapeCollections();

Q_SIGNALS:
    void resultReady();

private:
    StencilBoxDocker *q;
};

