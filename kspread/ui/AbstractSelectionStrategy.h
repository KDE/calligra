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

#ifndef KSPREAD_ABSTRACT_SELECTION_STRATEGY
#define KSPREAD_ABSTRACT_SELECTION_STRATEGY

#include <KoInteractionStrategy.h>

#include <Qt>

class KoCanvasBase;

namespace KSpread
{
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
class AbstractSelectionStrategy : public KoInteractionStrategy
{
public:
    /**
     * Constructor.
     */
    AbstractSelectionStrategy(KoToolBase* parent, Selection* selection,
                              const QPointF position, Qt::KeyboardModifiers modifiers);

    /**
     * Destructor.
     */
    virtual ~AbstractSelectionStrategy();

    virtual void handleMouseMove(const QPointF& mouseLocation, Qt::KeyboardModifiers modifiers);
    virtual QUndoCommand* createCommand();
    virtual void finishInteraction(Qt::KeyboardModifiers modifiers);

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
    Selection* selection() const;
    const QPointF& startPosition() const;

private:
    class Private;
    Private * const d;
};

} // namespace KSpread

#endif // KSPREAD_ABSTRACT_SELECTION_STRATEGY
