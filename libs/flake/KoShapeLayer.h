/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __KOSHAPELAYER_H__
#define __KOSHAPELAYER_H__

#include "KoShapeContainer.h"
#include "flake_export.h"

class KoShapeLayerPrivate;

/**
 * Provides arranging shapes into layers.
 * This makes it possible to have a higher key of a number of objects
 * in a document.
 * A layer is always invisible and unselectable.
 */
class FLAKE_EXPORT KoShapeLayer : public KoShapeContainer
{
public:
    /// The default constructor
    KoShapeLayer();
    /**
     * Constructor with custom model
     * @param model the custom modem
     */
    explicit KoShapeLayer(KoShapeContainerModel *model);

    /**
     * Empty implementation, as the layer itself is not visible
     */
    void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    bool hitTest(const QPointF &position) const override;
    QRectF boundingRect() const override;
    void saveOdf(KoShapeSavingContext &context) const override;
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    Q_DECLARE_PRIVATE(KoShapeLayer)
};

#endif // __KOSHAPELAYER_H__
