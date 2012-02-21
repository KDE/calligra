/* This file is part of the KDE project
 *
 * Copyright (C) 2012 Inge Wallin <inge@lysator.liu.se>
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

// Own
#include "Lightsource.h"

// Qt
#include <QString>
//#include <QPainter>
//#include <QTimer>
//#include <QPixmapCache>
//#include <QtCore/QBuffer>

// KDE
#include <KDebug>

// Calligra
#include <KoViewConverter.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoShapeLoadingContext.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoXmlNS.h>
#include <KoStoreDevice.h>
#include <KoUnit.h>
#include <KoGenStyle.h>
#include <KoFilterEffectStack.h>


static QVector3D stringToVector(QString &string);

Lightsource::Lightsource(KoXmlElement &lightElem)
{
    m_diffuseColor = QColor(lightElem.attributeNS(KoXmlNS::dr3d, "diffuse-color", "#ffffff"));
    QString direction = lightElem.attributeNS(KoXmlNS::dr3d, "direction", "");
    m_direction    = stringToVector(direction);
    m_enabled      = (lightElem.attributeNS(KoXmlNS::dr3d, "enabled", "") == "true");
    m_specular     = (lightElem.attributeNS(KoXmlNS::dr3d, "specular", "") == "true");
}

Lightsource::~Lightsource()
{
}


void Lightsource::saveOdf(KoXmlWriter &writer)
{
}


static QVector3D stringToVector(QString &string)
{
    QStringList elements = string.mid(1, string.size() - 2).split(' ', QString::SkipEmptyParts);
    if (elements.size() != 3)
        return QVector3D(0, 0, 1);
    else
        return QVector3D(elements[0].toDouble(), elements[2].toDouble(), elements[2].toDouble());
}
