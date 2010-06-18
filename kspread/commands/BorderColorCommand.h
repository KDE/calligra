/* This file is part of the KDE project
   Copyright 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_BORDER_COLOR_COMMAND
#define KSPREAD_BORDER_COLOR_COMMAND

#include <QPair>
#include <QColor>
#include "AbstractRegionCommand.h"
#include "Style.h"

namespace KSpread
{

/**
 * \ingroup Commands
 * \brief Applies a color to an exising border.
 */
class BorderColorCommand : public AbstractRegionCommand
{
public:
    BorderColorCommand();
    void setColor(const QColor& color) {
        m_color = color;
    }

protected:
    virtual bool preProcessing();
    virtual bool mainProcessing();
    virtual bool postProcessing();

private:
    QColor m_color;
    QList< QPair<QRectF, SharedSubStyle> > m_undoData;
};

}  // namespace KSpread

#endif // KSPREAD_BORDER_COLOR_COMMAND
