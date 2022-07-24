/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "BorderColorCommand.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "core/StyleStorage.h"

using namespace Calligra::Sheets;

BorderColorCommand::BorderColorCommand()
        : AbstractRegionCommand()
{
    setText(kundo2_i18n("Change Border Color"));
}

bool BorderColorCommand::performCommands()
{
    QRect cur = firstRange();
    for (int y = cur.top(); y < cur.top() + cur.height(); ++y) {
        if (y > m_sheet->fullCellStorage()->rows()) break;
        for (int x = cur.left(); x < cur.left() + cur.width(); ++x) {
            if (y > m_sheet->fullCellStorage()->rows()) break;

            Style style = m_sheet->fullCellStorage()->style(x, y);
            Style newStyle;
            bool adjusted = false;
            QPen pen;
            pen = style.leftBorderPen();
            if (pen.style() != Qt::NoPen) {
                pen.setColor(m_color);
                newStyle.setLeftBorderPen(pen);
                adjusted = true;
            }
            pen = style.rightBorderPen();
            if (pen.style() != Qt::NoPen) {
                pen.setColor(m_color);
                newStyle.setRightBorderPen(pen);
                adjusted = true;
            }
            pen = style.topBorderPen();
            if (pen.style() != Qt::NoPen) {
                pen.setColor(m_color);
                newStyle.setTopBorderPen(pen);
                adjusted = true;
            }
            pen = style.bottomBorderPen();
            if (pen.style() != Qt::NoPen) {
                pen.setColor(m_color);
                newStyle.setBottomBorderPen(pen);
                adjusted = true;
            }
            pen = style.fallDiagonalPen();
            if (pen.style() != Qt::NoPen) {
                pen.setColor(m_color);
                newStyle.setFallDiagonalPen(pen);
                adjusted = true;
            }
            pen = style.goUpDiagonalPen();
            if (pen.style() != Qt::NoPen) {
                pen.setColor(m_color);
                newStyle.setGoUpDiagonalPen(pen);
                adjusted = true;
            }

            if (adjusted)
                m_sheet->fullCellStorage()->setStyle(Region(QPoint(x, y)), newStyle);
        }
    }
    return true;
}


