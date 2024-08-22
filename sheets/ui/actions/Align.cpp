/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Align.h"
#include "Actions.h"

#include "core/Style.h"
#include "ui/commands/StyleCommand.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

AlignLeft::AlignLeft(Actions *actions)
    : ToggleableCellAction(actions, "alignLeft", i18n("Align Left"), koIcon("format-justify-left"), i18n("Left justify the cell contents"))
{
}

AlignLeft::~AlignLeft() = default;

QAction *AlignLeft::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Left"));
    return res;
}

void AlignLeft::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Horizontal Alignment"));
    Style s;
    s.setHAlign(selected ? Style::Left : Style::HAlignUndefined);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool AlignLeft::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return (style.halign() == Style::Left);
}

AlignRight::AlignRight(Actions *actions)
    : ToggleableCellAction(actions, "alignRight", i18n("Align Right"), koIcon("format-justify-right"), i18n("Right justify the cell contents"))
{
}

AlignRight::~AlignRight() = default;

QAction *AlignRight::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Right"));
    return res;
}

void AlignRight::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Horizontal Alignment"));
    Style s;
    s.setHAlign(selected ? Style::Right : Style::HAlignUndefined);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool AlignRight::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return (style.halign() == Style::Right);
}

AlignCenter::AlignCenter(Actions *actions)
    : ToggleableCellAction(actions, "alignCenter", i18n("Align Center"), koIcon("format-justify-center"), i18n("Center the cell contents"))
{
}

AlignCenter::~AlignCenter() = default;

QAction *AlignCenter::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Center"));
    return res;
}

void AlignCenter::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Horizontal Alignment"));
    Style s;
    s.setHAlign(selected ? Style::Center : Style::HAlignUndefined);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool AlignCenter::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return (style.halign() == Style::Center);
}

AlignTop::AlignTop(Actions *actions)
    : ToggleableCellAction(actions, "alignTop", i18n("Align Top"), koIcon("format-align-vertical-top"), i18n("Align cell contents along the top of the cell"))
{
}

AlignTop::~AlignTop() = default;

QAction *AlignTop::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Top"));
    return res;
}

void AlignTop::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Vertical Alignment"));
    Style s;
    s.setVAlign(selected ? Style::Top : Style::VAlignUndefined);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool AlignTop::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return (style.valign() == Style::Top);
}

AlignBottom::AlignBottom(Actions *actions)
    : ToggleableCellAction(actions,
                           "alignBottom",
                           i18n("Align Bottom"),
                           koIcon("format-align-vertical-bottom"),
                           i18n("Align cell contents along the bottom of the cell"))
{
}

AlignBottom::~AlignBottom() = default;

QAction *AlignBottom::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Bottom"));
    return res;
}

void AlignBottom::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Vertical Alignment"));
    Style s;
    s.setVAlign(selected ? Style::Bottom : Style::VAlignUndefined);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool AlignBottom::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return (style.valign() == Style::Bottom);
}

AlignMiddle::AlignMiddle(Actions *actions)
    : ToggleableCellAction(actions,
                           "alignMiddle",
                           i18n("Align Middle"),
                           koIcon("format-align-vertical-center"),
                           i18n("Align cell contents centered in the cell"))
{
}

AlignMiddle::~AlignMiddle() = default;

QAction *AlignMiddle::createAction()
{
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Middle"));
    return res;
}

void AlignMiddle::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Vertical Alignment"));
    Style s;
    s.setVAlign(selected ? Style::Middle : Style::VAlignUndefined);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool AlignMiddle::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return (style.valign() == Style::Middle);
}
