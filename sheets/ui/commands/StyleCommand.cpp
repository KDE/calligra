/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "StyleCommand.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "engine/Damages.h"
#include "engine/MapBase.h"

using namespace Calligra::Sheets;

StyleCommand::StyleCommand(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_horizontalPen(QPen(QColor(), 0, Qt::NoPen))
    , m_verticalPen(QPen(QColor(), 0, Qt::NoPen))
    , m_horizontalPenChanged(false)
    , m_verticalPenChanged(false)
{
}

StyleCommand::~StyleCommand() = default;

bool StyleCommand::process(Element *element)
{
    const QRect range = element->rect();
    CellStorage *cs = m_sheet->fullCellStorage();

    if (m_style.isDefault())
        setText(kundo2_i18n("Reset Style"));
    else
        setText(kundo2_i18n("Change Style"));

    // special handling for the border
    bool hasLeftPen = m_style.hasAttribute(Style::LeftPen);
    bool hasRightPen = m_style.hasAttribute(Style::RightPen);
    bool hasTopPen = m_style.hasAttribute(Style::TopPen);
    bool hasBottomPen = m_style.hasAttribute(Style::BottomPen);
    const QPen leftPen = m_style.leftBorderPen();
    const QPen rightPen = m_style.rightBorderPen();
    const QPen topPen = m_style.topBorderPen();
    const QPen bottomPen = m_style.bottomBorderPen();
    m_style.clearAttribute(Style::LeftPen);
    m_style.clearAttribute(Style::RightPen);
    m_style.clearAttribute(Style::TopPen);
    m_style.clearAttribute(Style::BottomPen);

    // use the horizontal/vertical pens
    if (m_horizontalPenChanged) {
        m_style.setTopBorderPen(m_horizontalPen);
        m_style.setBottomBorderPen(m_horizontalPen);
        hasTopPen = hasBottomPen = true;
    }
    if (m_verticalPenChanged) {
        m_style.setLeftBorderPen(m_verticalPen);
        m_style.setRightBorderPen(m_verticalPen);
        hasLeftPen = hasRightPen = true;
    }

    // special handling for indentation: reset the indentation first
    if (m_style.hasAttribute(Style::Indentation)) {
        Style style;
        style.setIndentation(0);
        cs->setStyle(Region(range), style);
    }

    // set the actual style
    cs->setStyle(Region(range), m_style);

    // prepare to restore pens
    m_style.clearAttribute(Style::LeftPen);
    m_style.clearAttribute(Style::RightPen);
    m_style.clearAttribute(Style::TopPen);
    m_style.clearAttribute(Style::BottomPen);

    // set the outer border styles
    Style style;
    if (hasLeftPen) {
        style.setLeftBorderPen(leftPen);
        cs->setStyle(Region(QRect(range.left(), range.top(), 1, range.height())), style);
        m_style.setLeftBorderPen(leftPen); // restore pen
        // reset the border of the adjacent cell
        if (range.left() > 1) {
            Style tmpStyle;
            tmpStyle.setRightBorderPen(QPen(Qt::NoPen));
            const Region region(QRect(range.left() - 1, range.top(), 1, range.height()));
            cs->setStyle(region, tmpStyle);
            m_sheet->map()->addDamage(new CellDamage(m_sheet, region, CellDamage::Appearance));
        }
    }
    if (hasRightPen) {
        style.clear();
        style.setRightBorderPen(rightPen);
        cs->setStyle(Region(QRect(range.right(), range.top(), 1, range.height())), style);
        m_style.setRightBorderPen(rightPen); // restore pen
        // reset the border of the adjacent cell
        if (range.right() < KS_colMax) {
            Style tmpStyle;
            tmpStyle.setLeftBorderPen(QPen(Qt::NoPen));
            const Region region(QRect(range.right() + 1, range.top(), 1, range.height()));
            cs->setStyle(region, tmpStyle);
            m_sheet->map()->addDamage(new CellDamage(m_sheet, region, CellDamage::Appearance));
        }
    }
    if (hasTopPen) {
        style.clear();
        style.setTopBorderPen(topPen);
        cs->setStyle(Region(QRect(range.left(), range.top(), range.width(), 1)), style);
        m_style.setTopBorderPen(topPen); // restore pen
        // reset the border of the adjacent cell
        if (range.top() > 1) {
            Style tmpStyle;
            tmpStyle.setBottomBorderPen(QPen(Qt::NoPen));
            const Region region(QRect(range.left(), range.top() - 1, range.width(), 1));
            cs->setStyle(region, tmpStyle);
            m_sheet->map()->addDamage(new CellDamage(m_sheet, region, CellDamage::Appearance));
        }
    }
    if (hasBottomPen) {
        style.clear();
        style.setBottomBorderPen(bottomPen);
        cs->setStyle(Region(QRect(range.left(), range.bottom(), range.width(), 1)), style);
        m_style.setBottomBorderPen(bottomPen); // restore pen
        // reset the border of the adjacent cell
        if (range.bottom() < KS_rowMax) {
            Style tmpStyle;
            tmpStyle.setTopBorderPen(QPen(Qt::NoPen));
            const Region region(QRect(range.left(), range.bottom() + 1, range.width(), 1));
            cs->setStyle(region, tmpStyle);
            m_sheet->map()->addDamage(new CellDamage(m_sheet, region, CellDamage::Appearance));
        }
    }
    return true;
}
