/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOANNOTATIONLAYOUTMANAGER_H
#define KOANNOTATIONLAYOUTMANAGER_H

#include "flake_export.h"

#include <QObject>

class KoShapeManager;
class KoCanvasBase;

class QPainter;
class QPointF;

class KoShape;

class FLAKE_EXPORT KoAnnotationLayoutManager : public QObject
{
    Q_OBJECT
public:
    static const qreal shapeSpace; // Distance between annotation shapes.
    static const qreal shapeWidth; // Annotation shapes width.
    // Connection point of lines from shape to this point and from this point to refText position.
    static const qreal connectionPointLines;

    explicit KoAnnotationLayoutManager(QObject *parent = nullptr);
    ~KoAnnotationLayoutManager() override;

    void setShapeManager(KoShapeManager *shapeManager);

    void setCanvasBase(KoCanvasBase *canvas);

    void setViewContentWidth(qreal width);

    void paintConnections(QPainter &painter);
    // Return true if shape is in annotation shapes list.
    bool isAnnotationShape(KoShape *shape) const;

public Q_SLOTS:
    /// register the position of an annotation shape.
    void registerAnnotationRefPosition(KoShape *annotationShape, const QPointF &refPosition);

    /// Remove annotation shape.
    void removeAnnotationShape(KoShape *annotationShape);

    /// Update layout annotation shapes. Called when shape size changed.
    void updateLayout(KoShape *shape);

Q_SIGNALS:
    void hasAnnotationsChanged(bool hasAnnotations);

private:
    /// layout annotation shapes
    void layoutAnnotationShapes();

private:
    class Private;
    Private *const d;
};

#endif // KOANNOTATIONLAYOUTMANAGER_H
