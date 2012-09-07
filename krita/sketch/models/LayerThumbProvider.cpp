/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Dan Leinir Turthra Jensen <admin@leinir.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "LayerThumbProvider.h"
#include "LayerModel.h"

class LayerThumbProvider::Private {
public:
    Private()
        : layerModel(0)
        , id(0)
    {};
    LayerModel* layerModel;
    int id;
};

LayerThumbProvider::LayerThumbProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    , d(new Private)
{
}

LayerThumbProvider::~LayerThumbProvider()
{
    delete d;
}

QImage LayerThumbProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    return d->layerModel->layerThumbnail(id);
}

void LayerThumbProvider::setLayerModel(LayerModel* model)
{
    d->layerModel = model;
}

void LayerThumbProvider::setLayerID(int id)
{
    d->id = id;
}

int LayerThumbProvider::layerID() const
{
    return d->id;
}

#include "LayerThumbProvider.moc"
