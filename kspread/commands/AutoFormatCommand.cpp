/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "AutoFormatCommand.h"

#include "CellStorage.h"
#include "Sheet.h"
#include "Style.h"
#include "Region.h"

#include <klocale.h>

#include <QPen>

using namespace KSpread;

AutoFormatCommand::AutoFormatCommand()
{
    setText(i18n("Auto-Format"));
}

AutoFormatCommand::~AutoFormatCommand()
{
}

void AutoFormatCommand::setStyles(const QList<Style>& styles)
{
    m_styles = styles;
}

bool AutoFormatCommand::preProcessing()
{
    if (m_firstrun)
        m_sheet->cellStorage()->startUndoRecording();
    // always reset the style of the processed region
    Style defaultStyle;
    defaultStyle.setDefault();
    Region::ConstIterator end(constEnd());
    for (Region::ConstIterator it = constBegin(); it != end; ++it)
        m_sheet->cellStorage()->setStyle(Region((*it)->rect()), defaultStyle);
    if (m_firstrun)
        m_sheet->cellStorage()->stopUndoRecording(this);
    return true;
}

bool AutoFormatCommand::mainProcessing()
{
    if (m_reverse) {
        QUndoCommand::undo(); // undo child commands
        return true;
    }
    return AbstractRegionCommand::mainProcessing();
}

bool AutoFormatCommand::process(Element* element)
{
    const QRect rect = element->rect();

    // Top left corner
    if (!m_styles[0].isDefault())
        m_sheet->cellStorage()->setStyle(Region(rect.topLeft()), m_styles[0]);
    // Top row
    for (int col = rect.left() + 1; col <= rect.right(); ++col) {
        int pos = 1 + ((col - rect.left() - 1) % 2);
        Cell cell(m_sheet, col, rect.top());
        if (!cell.isPartOfMerged()) {
            Style style;
            if (!m_styles[pos].isDefault())
                style = m_styles[pos];

            Style tmpStyle = (col == rect.left() + 1) ? m_styles[1] : m_styles[2];
            if (!tmpStyle.isDefault())
                style.setLeftBorderPen(tmpStyle.leftBorderPen());

            m_sheet->cellStorage()->setStyle(Region(col, rect.top()), style);
        }
    }

    // Left column
    for (int row = rect.top() + 1; row <= rect.bottom(); ++row) {
        int pos = 4 + ((row - rect.top() - 1) % 2) * 4;
        Cell cell(m_sheet, rect.left(), row);
        if (!cell.isPartOfMerged()) {
            Style style;
            if (!m_styles[pos].isDefault())
                style = m_styles[pos];

            Style tmpStyle = (row == rect.top() + 1) ? m_styles[4] : m_styles[8];
            if (!tmpStyle.isDefault())
                style.setTopBorderPen(tmpStyle.topBorderPen());

            m_sheet->cellStorage()->setStyle(Region(rect.left(), row), style);
        }
    }

    // Body
    for (int col = rect.left() + 1; col <= rect.right(); ++col) {
        for (int row = rect.top() + 1; row <= rect.bottom(); ++row) {
            int pos = 5 + ((row - rect.top() - 1) % 2) * 4 + ((col - rect.left() - 1) % 2);
            Cell cell(m_sheet, col, row);
            if (!cell.isPartOfMerged()) {
                if (!m_styles[pos].isDefault())
                    m_sheet->cellStorage()->setStyle(Region(col, row), m_styles[pos]);

                Style style;
                if (col == rect.left() + 1)
                    style = m_styles[ 5 + ((row - rect.top() - 1) % 2) * 4 ];
                else
                    style = m_styles[ 6 + ((row - rect.top() - 1) % 2) * 4 ];

                if (!style.isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setLeftBorderPen(style.leftBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(col, row), tmpStyle);
                }

                if (row == rect.top() + 1)
                    style = m_styles[ 5 + ((col - rect.left() - 1) % 2)];
                else
                    style = m_styles[ 9 + ((col - rect.left() - 1) % 2)];

                if (!style.isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setTopBorderPen(style.topBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(col, row), tmpStyle);
                }
            }
        }
    }

    // Outer right border
    for (int row = rect.top(); row <= rect.bottom(); ++row) {
        Cell cell(m_sheet, rect.right(), row);
        if (!cell.isPartOfMerged()) {
            if (row == rect.top()) {
                if (!m_styles[3].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setRightBorderPen(m_styles[3].leftBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(rect.right(), row), tmpStyle);
                }
            } else if (row == rect.right()) {
                if (!m_styles[11].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setRightBorderPen(m_styles[11].leftBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(rect.right(), row), tmpStyle);
                }
            } else {
                if (!m_styles[7].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setRightBorderPen(m_styles[7].leftBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(rect.right(), row), tmpStyle);
                }
            }
        }
    }

    // Outer bottom border
    for (int col = rect.left(); col <= rect.right(); ++col) {
        Cell cell(m_sheet, col, rect.bottom());
        if (!cell.isPartOfMerged()) {
            if (col == rect.left()) {
                if (!m_styles[12].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setBottomBorderPen(m_styles[12].topBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(col, rect.bottom()), tmpStyle);
                }
            } else if (col == rect.right()) {
                if (!m_styles[14].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setBottomBorderPen(m_styles[14].topBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(col, rect.bottom()), tmpStyle);
                }
            } else {
                if (!m_styles[13].isDefault()) {
                    Style tmpStyle;
                    tmpStyle.setBottomBorderPen(m_styles[13].topBorderPen());
                    m_sheet->cellStorage()->setStyle(Region(col, rect.bottom()), tmpStyle);
                }
            }
        }
    }
    return true;
}
