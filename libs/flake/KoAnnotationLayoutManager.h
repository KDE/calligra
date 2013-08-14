/* This file is part of the KDE project
 * Copyright (C) 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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
#ifndef KOANNOTATIONLAYOUTMANAGER_H
#define KOANNOTATIONLAYOUTMANAGER_H

#include "flake_export.h"

#include <QObject>
#include <QPointF>
#include <KoViewConverter.h>
#include <KoShapeManager.h>

class QPainter;

class KoShape;

class FLAKE_EXPORT KoAnnotationLayoutManager: public QObject
{
    Q_OBJECT
public:
    KoAnnotationLayoutManager(QObject *parent = 0);
    virtual ~KoAnnotationLayoutManager();

    void paintConnections(QPainter &painter, KoViewConverter *viewConverter);

    void setShapeManager(KoShapeManager *shapeManager);

public slots:
    /// register the position of an annotation shape.
    void registerAnnotationRefPosition(KoShape *annotationShape, QPointF refPosition);

    /// Remove annotation shape.
    void removeAnnotationShape(KoShape *annotationShape);

    /// Update layout annotation shapes.
    void updateLayout(KoShape *shape);

private:
    /// layout annotation shapes
    void layoutAnnotationShapes();


private:
    class Private;
    Private * const d;
};

#endif // KOANNOTATIONLAYOUTMANAGER_H

