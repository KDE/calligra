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

#ifndef CALLIGRA_SHEETS_HYPERLINK_STRATEGY
#define CALLIGRA_SHEETS_HYPERLINK_STRATEGY

#include "AbstractSelectionStrategy.h"
#include "sheets_common_export.h"

#include <Qt>
#include <QRectF>

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
class CALLIGRA_SHEETS_COMMON_EXPORT HyperlinkStrategy : public AbstractSelectionStrategy
{
public:
    /**
     * Constructor.
     */
    HyperlinkStrategy(CellToolBase *cellTool,
                      const QPointF& position, Qt::KeyboardModifiers modifiers,
                      const QString& url, const QRectF& textRect);

    /**
     * Destructor.
     */
    ~HyperlinkStrategy() override;

    void handleMouseMove(const QPointF& mouseLocation, Qt::KeyboardModifiers modifiers) override;
    void finishInteraction(Qt::KeyboardModifiers modifiers) override;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_HYPERLINK_STRATEGY
