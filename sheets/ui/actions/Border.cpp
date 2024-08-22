/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Border.h"
#include "Actions.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "ui/CellToolBase.h"
#include "ui/commands/StyleCommand.h"

#include <KoColor.h>
#include <KoColorPopupAction.h>

#include <KLocalizedString>

using namespace Calligra::Sheets;

BorderLeft::BorderLeft(Actions *actions)
    : CellAction(actions, "borderLeft", i18n("Border Left"), koIcon("format-border-set-left"), i18n("Set a left border to the selected area"))
{
}

BorderLeft::~BorderLeft() = default;

QAction *BorderLeft::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Left"));
    return res;
}

void BorderLeft::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    BorderColor *a = dynamic_cast<BorderColor *>(m_actions->cellAction("borderColor"));
    QColor color = a->selectedColor();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Border"));
    Style s;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        s.setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        s.setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderRight::BorderRight(Actions *actions)
    : CellAction(actions, "borderRight", i18n("Border Right"), koIcon("format-border-set-right"), i18n("Set a right border to the selected area"))
{
}

BorderRight::~BorderRight() = default;

QAction *BorderRight::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Right"));
    return res;
}

void BorderRight::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    BorderColor *a = dynamic_cast<BorderColor *>(m_actions->cellAction("borderColor"));
    QColor color = a->selectedColor();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Border"));
    Style s;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        s.setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    else
        s.setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderTop::BorderTop(Actions *actions)
    : CellAction(actions, "borderTop", i18n("Border Top"), koIcon("format-border-set-top"), i18n("Set a top border to the selected area"))
{
}

BorderTop::~BorderTop() = default;

QAction *BorderTop::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Top"));
    return res;
}

void BorderTop::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    BorderColor *a = dynamic_cast<BorderColor *>(m_actions->cellAction("borderColor"));
    QColor color = a->selectedColor();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Border"));
    Style s;
    s.setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderBottom::BorderBottom(Actions *actions)
    : CellAction(actions, "borderBottom", i18n("Border Bottom"), koIcon("format-border-set-bottom"), i18n("Set a bottom border to the selected area"))
{
}

BorderBottom::~BorderBottom() = default;

QAction *BorderBottom::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Bottom"));
    return res;
}

void BorderBottom::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    BorderColor *a = dynamic_cast<BorderColor *>(m_actions->cellAction("borderColor"));
    QColor color = a->selectedColor();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Border"));
    Style s;
    s.setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderAll::BorderAll(Actions *actions)
    : CellAction(actions, "borderAll", i18n("All Borders"), koIcon("format-border-set-all"), i18n("Set a border around all cells in the selected area"))
{
}

BorderAll::~BorderAll() = default;

QAction *BorderAll::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("All"));
    return res;
}

void BorderAll::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    BorderColor *a = dynamic_cast<BorderColor *>(m_actions->cellAction("borderColor"));
    QColor color = a->selectedColor();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Border"));
    Style s;
    s.setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    s.setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    s.setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    s.setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setHorizontalPen(QPen(color, 1, Qt::SolidLine));
    command->setVerticalPen(QPen(color, 1, Qt::SolidLine));
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderNone::BorderNone(Actions *actions)
    : CellAction(actions, "borderRemove", i18n("No Borders"), koIcon("format-border-set-none"), i18n("Remove all borders in the selected area"))
{
}

BorderNone::~BorderNone() = default;

QAction *BorderNone::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("None"));
    return res;
}

void BorderNone::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Border"));
    Style s;
    s.setTopBorderPen(QPen(Qt::NoPen));
    s.setBottomBorderPen(QPen(Qt::NoPen));
    s.setLeftBorderPen(QPen(Qt::NoPen));
    s.setRightBorderPen(QPen(Qt::NoPen));
    command->setHorizontalPen(QPen(Qt::NoPen));
    command->setVerticalPen(QPen(Qt::NoPen));
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderOutline::BorderOutline(Actions *actions)
    : CellAction(actions,
                 "borderOutline",
                 i18n("Border Outline"),
                 koIcon("format-border-set-external"),
                 i18n("Set a border to the outline of the selected area"))
{
}

BorderOutline::~BorderOutline() = default;

QAction *BorderOutline::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Outline"));
    return res;
}

void BorderOutline::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    BorderColor *a = dynamic_cast<BorderColor *>(m_actions->cellAction("borderColor"));
    QColor color = a->selectedColor();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Border"));
    Style s;
    s.setTopBorderPen(QPen(color, 1, Qt::SolidLine));
    s.setBottomBorderPen(QPen(color, 1, Qt::SolidLine));
    s.setLeftBorderPen(QPen(color, 1, Qt::SolidLine));
    s.setRightBorderPen(QPen(color, 1, Qt::SolidLine));
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderColor::BorderColor(Actions *actions)
    : CellAction(actions, "borderColor", i18n("Border Color"), koIcon("format-stroke-color"), i18n("Select a new border color"))
{
}

BorderColor::~BorderColor() = default;

QAction *BorderColor::createAction()
{
    m_colorAction = new KoColorPopupAction(m_actions->tool());
    m_colorAction->setIcon(m_icon);
    m_colorAction->setToolTip(m_tooltip);
    m_colorAction->setText(m_caption);
    connect(m_colorAction, &KoColorPopupAction::colorChanged, this, &BorderColor::triggeredBorderColor);
    return m_colorAction;
}

QColor BorderColor::selectedColor()
{
    return m_colorAction->currentColor();
}

void BorderColor::triggeredBorderColor(const KoColor &color)
{
    CellToolBase *tool = m_actions->tool();
    Selection *selection = tool->selection();
    Sheet *sheet = selection->activeSheet();

    BorderColorCommand *command = new BorderColorCommand();
    command->setSheet(sheet);
    QColor c = color.toQColor();
    command->setColor(c);
    command->add(*selection);
    command->execute(selection->canvas());
}

BorderColorCommand::BorderColorCommand()
    : AbstractRegionCommand()
{
    setText(kundo2_i18n("Change Border Color"));
}

bool BorderColorCommand::performCommands()
{
    QRect cur = firstRange();
    for (int y = cur.top(); y < cur.top() + cur.height(); ++y) {
        if (y > m_sheet->fullCellStorage()->rows())
            break;
        for (int x = cur.left(); x < cur.left() + cur.width(); ++x) {
            if (y > m_sheet->fullCellStorage()->rows())
                break;

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
