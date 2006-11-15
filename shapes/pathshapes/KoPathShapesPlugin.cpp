/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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
#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>

#include "KoPathShapesPlugin.h"
#include "star/KoStarShapeFactory.h"
#include "regularpolygon/KoRegularPolygonShapeFactory.h"
#include "rectangle/KoRectangleShapeFactory.h"

#include <kgenericfactory.h>

K_EXPORT_COMPONENT_FACTORY(pathshapes,
                           KGenericFactory<KoPathShapesPlugin>( "PathShapes" ) )

KoPathShapesPlugin::KoPathShapesPlugin( QObject *parent, const QStringList& )
    : QObject(parent)
{
    KoShapeRegistry::instance()->add( new KoStarShapeFactory( parent));
    KoShapeRegistry::instance()->add( new KoRegularPolygonShapeFactory( parent));
    KoShapeRegistry::instance()->add( new KoRectangleShapeFactory( parent));
}

#include "KoPathShapesPlugin.moc"
