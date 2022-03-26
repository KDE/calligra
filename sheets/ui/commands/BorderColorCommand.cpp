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

bool BorderColorCommand::preProcessing()
{
    if (!m_firstrun)
        return true;
    m_sheet->fullCellStorage()->startUndoRecording();
    return AbstractRegionCommand::preProcessing();
}

bool BorderColorCommand::mainProcessing()
{
    if (!m_reverse) {
        if (m_firstrun) {
            // Grab the current data.
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
                m_undoData += undoData;
            }
        }

        // change colors
        Style style;
        for (int i = 0; i < m_undoData.count(); ++i) {
            style.clear();
            style.insertSubStyle(m_undoData[i].second);
            QPen pen;
            if (m_undoData[i].second->type() == Style::LeftPen) {
                pen = style.leftBorderPen();
                pen.setColor(m_color);
                style.setLeftBorderPen(pen);
            }
            if (m_undoData[i].second->type() == Style::RightPen) {
                pen = style.rightBorderPen();
                pen.setColor(m_color);
                style.setRightBorderPen(pen);
            }
            if (m_undoData[i].second->type() == Style::TopPen) {
                pen = style.topBorderPen();
                pen.setColor(m_color);
                style.setTopBorderPen(pen);
            }
            if (m_undoData[i].second->type() == Style::BottomPen) {
                pen = style.bottomBorderPen();
                pen.setColor(m_color);
                style.setBottomBorderPen(pen);
            }
            if (m_undoData[i].second->type() == Style::FallDiagonalPen) {
                pen = style.fallDiagonalPen();
                pen.setColor(m_color);
                style.setFallDiagonalPen(pen);
            }
            if (m_undoData[i].second->type() == Style::GoUpDiagonalPen) {
                pen = style.goUpDiagonalPen();
                pen.setColor(m_color);
                style.setGoUpDiagonalPen(pen);
            }
            m_sheet->fullCellStorage()->setStyle(Region(m_undoData[i].first.toRect()), style);
        }
    } else { // m_reverse
        KUndo2Command::undo(); // undo child commands
    }
    return true;
}

bool BorderColorCommand::postProcessing()
{
    if (m_firstrun)
        m_sheet->fullCellStorage()->stopUndoRecording(this);
    return true;
}

