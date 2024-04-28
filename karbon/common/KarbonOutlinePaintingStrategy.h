/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBONOUTLINEPAINTINGSTRATEGY_H
#define KARBONOUTLINEPAINTINGSTRATEGY_H

#include "karboncommon_export.h"
#include <KoShapeManagerPaintingStrategy.h>

class KoShapeManager;
class KoShapeStroke;

class KARBONCOMMON_EXPORT KarbonOutlinePaintingStrategy : public KoShapeManagerPaintingStrategy
{
public:
    explicit KarbonOutlinePaintingStrategy(KoShapeManager *shapeManager);
    ~KarbonOutlinePaintingStrategy() override;
    /// reimplemented from KoShapeManagerPaintingStrategy
    void paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext) override;

private:
    KoShapeStroke *m_stroke;
};

#endif // KARBONOUTLINEPAINTINGSTRATEGY_H
