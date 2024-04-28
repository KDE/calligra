/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ABSTRACT_SELECTION_STRATEGY
#define CALLIGRA_SHEETS_ABSTRACT_SELECTION_STRATEGY

#include "sheets_ui_export.h"
#include <KoInteractionStrategy.h>

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
class CALLIGRA_SHEETS_UI_EXPORT AbstractSelectionStrategy : public KoInteractionStrategy
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

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    KUndo2Command *createCommand() override;
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
    static bool hitTestSelectionSizeGrip(KoCanvasBase *canvas, Selection *selection, const QPointF &position);

    /**
     * Checks, if there is a size grip for the reference selection at
     * \p position.
     * \param canvas the canvas
     * \param selection the selection
     * \param position the document coordinate (relative to the shape's origin)
     * to check
     * \return \c true if there is a size grip; \c false otherwise.
     */
    static bool hitTestReferenceSizeGrip(KoCanvasBase *canvas, Selection *selection, const QPointF &position);

protected:
    CellToolBase *cellTool() const;
    Selection *selection() const;
    const QPointF &startPosition() const;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ABSTRACT_SELECTION_STRATEGY
