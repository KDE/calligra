/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef CALLIGRA_SHEETS_ABSTRACT_SELECTION_STRATEGY
#define CALLIGRA_SHEETS_ABSTRACT_SELECTION_STRATEGY

#include <KoInteractionStrategy.h>
#include "sheets_common_export.h"

#include <Qt>

class KoCanvasBase;

namespace Calligra
{
namespace Sheets
{
class CellToolBase;
class Selection;

/**
 * An abstract selection strategy.
 *
 * Derive from this class, if you are only interested in updating the selection on mouse movements.
 * For the case, that you will also need to select the cell, where the mouse press occurred, derive
 * from SelectionStrategy.
 *
 * \see SelectionStrategy
 */
class CALLIGRA_SHEETS_COMMON_EXPORT AbstractSelectionStrategy : public KoInteractionStrategy
{
public:
    /**
     * Constructor.
     */
    AbstractSelectionStrategy(CellToolBase *cellTool, const QPointF &position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    ~AbstractSelectionStrategy() override;

    void handleMouseMove(const QPointF& mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command* createCommand() override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

    /**
     * Checks, if there is a size grip for the (normal) selection at
     * \p position.
     * \param canvas the canvas
     * \param selection the selection
     * \param position the document coordinate (relative to the shape's origin)
     * to check
     * \return \c true if there is a size grip; \c false otherwise.
     */
    static bool hitTestSelectionSizeGrip(KoCanvasBase *canvas, Selection *selection,
                                         const QPointF &position);

    /**
     * Checks, if there is a size grip for the reference selection at
     * \p position.
     * \param canvas the canvas
     * \param selection the selection
     * \param position the document coordinate (relative to the shape's origin)
     * to check
     * \return \c true if there is a size grip; \c false otherwise.
     */
    static bool hitTestReferenceSizeGrip(KoCanvasBase *canvas, Selection *selection,
                                         const QPointF &position);

protected:
    CellToolBase *cellTool() const;
    Selection* selection() const;
    const QPointF& startPosition() const;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ABSTRACT_SELECTION_STRATEGY
