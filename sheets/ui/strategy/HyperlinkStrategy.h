/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_HYPERLINK_STRATEGY
#define CALLIGRA_SHEETS_HYPERLINK_STRATEGY

#include "../AbstractSelectionStrategy.h"

class QRectF;

namespace Calligra
{
namespace Sheets
{

/**
 * A strategy for visiting a hyperlink.
 *
 * If the mouse is released on the rectangle, that the link text occupies, the link will be visited.
 *
 * \todo For the case, that this region is left, a drag is initiated.
 */
class CALLIGRA_SHEETS_UI_EXPORT HyperlinkStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    HyperlinkStrategy(CellToolBase *cellTool, const QPointF &position, Qt::KeyboardModifiers modifiers, const QString &url, const QRectF &textRect);

    /**
     * Destructor.
     */
    ~HyperlinkStrategy() override;

    void handleMouseMove(const QPointF &mouseLocation, Qt::KeyboardModifiers modifiers) override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_HYPERLINK_STRATEGY
