/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
    // Grab the current borders.
    QVector< QPair<QRectF, SharedSubStyle> > undoData = m_sheet->styleStorage()->currentData(*this);
    ConstIterator endOfList = constEnd();
    for (ConstIterator it = constBegin(); it != endOfList; ++it) {
        for (int i = 0; i < undoData.count(); ++i) {
            if (undoData[i].second->type() != Style::LeftPen &&
                    undoData[i].second->type() != Style::RightPen &&
                    undoData[i].second->type() != Style::TopPen &&
                    undoData[i].second->type() != Style::BottomPen &&
                    undoData[i].second->type() != Style::FallDiagonalPen &&
                    undoData[i].second->type() != Style::GoUpDiagonalPen) {
                undoData.removeAt(i--);
            }
        }
    }

    // And change their colors.
    Style style;
    for (int i = 0; i < undoData.count(); ++i) {
        style.clear();
        style.insertSubStyle(undoData[i].second);
        QPen pen;
        if (undoData[i].second->type() == Style::LeftPen) {
            pen = style.leftBorderPen();
            pen.setColor(m_color);
            style.setLeftBorderPen(pen);
        }
        if (undoData[i].second->type() == Style::RightPen) {
            pen = style.rightBorderPen();
            pen.setColor(m_color);
            style.setRightBorderPen(pen);
        }
        if (undoData[i].second->type() == Style::TopPen) {
            pen = style.topBorderPen();
            pen.setColor(m_color);
            style.setTopBorderPen(pen);
        }
        if (undoData[i].second->type() == Style::BottomPen) {
            pen = style.bottomBorderPen();
            pen.setColor(m_color);
            style.setBottomBorderPen(pen);
        }
        if (undoData[i].second->type() == Style::FallDiagonalPen) {
            pen = style.fallDiagonalPen();
            pen.setColor(m_color);
            style.setFallDiagonalPen(pen);
        }
        if (undoData[i].second->type() == Style::GoUpDiagonalPen) {
            pen = style.goUpDiagonalPen();
            pen.setColor(m_color);
            style.setGoUpDiagonalPen(pen);
        }
        m_sheet->fullCellStorage()->setStyle(Region(undoData[i].first.toRect()), style);
    }
    return true;
}


