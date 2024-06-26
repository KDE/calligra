/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2007, 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPEMANAGERPAINTINGSTRATEGY_H
#define KOSHAPEMANAGERPAINTINGSTRATEGY_H

#include "flake_export.h"

class KoShapeManager;
class KoShape;
class KoViewConverter;
class KoShapePaintingContext;
class QPainter;
class QRectF;

/**
 * This implements the painting strategy for the KoShapeManager
 *
 * This is done to make it possible to have e.g. animations in Stage.
 *
 * This class implements the default strategy which is normally used.
 */
class FLAKE_EXPORT KoShapeManagerPaintingStrategy
{
public:
    explicit KoShapeManagerPaintingStrategy(KoShapeManager *shapeManager);
    virtual ~KoShapeManagerPaintingStrategy();

    /**
     * Paint the shape
     *
     * @param shape the shape to paint
     * @param painter the painter to paint to.
     * @param converter to convert between document and view coordinates.
     * @param forPrint if true, make sure only actual content is drawn and no decorations.
     */
    virtual void paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext);

    /**
     * Adapt the rect the shape occupies
     *
     * @param rect rect which will be updated to give the rect the shape occupies.
     */
    virtual void adapt(KoShape *shape, QRectF &rect);

    /**
     * Set the shape manager
     *
     * This is needed in case you cannot set the shape manager when creating the painting strategy.
     * It needs to be set before you paint otherwise nothing will be painted.
     *
     * @param shapeManager The shape manager to use in the painting strategy
     */
    void setShapeManager(KoShapeManager *shapeManager);

protected:
    KoShapeManager *shapeManager();

private:
    class Private;
    Private *const d;
};

#endif /* KOSHAPEMANAGERPAINTINGSTRATEGY_H */
