/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_RIGHTTOLEFT_PAINTING_STRATEGY
#define CALLIGRA_SHEETS_RIGHTTOLEFT_PAINTING_STRATEGY

#include <KoShapeManagerPaintingStrategy.h>

class KoCanvasBase;

namespace Calligra
{
namespace Sheets
{

/**
 */
class RightToLeftPaintingStrategy : public KoShapeManagerPaintingStrategy
{
public:
    RightToLeftPaintingStrategy(KoShapeManager *shapeManager, KoCanvasBase *canvas);
    ~RightToLeftPaintingStrategy() override;

    /**
     * Paint the shape
     *
     * @param shape the shape to paint
     * @param painter the painter to paint to.
     * @param converter to convert between document and view coordinates.
     * @param paintContext painting context.
     */
    void paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintContext) override;

    /**
     * Adapt the rect the shape occupies
     *
     * @param shape the shape
     * @param rect rect which will be updated to give the rect the shape occupies.
     */
    void adapt(KoShape *shape, QRectF &rect) override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_RIGHTTOLEFT_PAINTING_STRATEGY
