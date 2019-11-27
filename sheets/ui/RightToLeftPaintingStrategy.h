/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
