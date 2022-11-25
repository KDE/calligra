/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Style.h"
#include "Actions.h"

#include "ui/commands/StyleCommand.h"
#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"

#include <KoCanvasBase.h>

#include <KLocalizedString>



using namespace Calligra::Sheets;


Bold::Bold(Actions *actions)
    : ToggleableCellAction(actions, "bold", i18n("Bold"), koIcon("format-text-bold"), i18n("Bold"))
{
}

Bold::~Bold()
{
}

QAction *Bold::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    return res;
}

void Bold::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontBold(selected);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
    if (m_actions->tool()->editor()) {
        const Cell cell = Cell(sheet, selection->marker());
        m_actions->tool()->editor()->setEditorFont(cell.style().font(), true, selection->canvas()->viewConverter());
    }
}

bool Bold::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return style.bold();
}

Italic::Italic(Actions *actions)
    : ToggleableCellAction(actions, "italic", i18n("Italic"), koIcon("format-text-italic"), i18n("Italic"))
{
}

Italic::~Italic()
{
}

QAction *Italic::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    return res;
}

void Italic::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontItalic(selected);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
    if (m_actions->tool()->editor()) {
        const Cell cell = Cell(sheet, selection->marker());
        m_actions->tool()->editor()->setEditorFont(cell.style().font(), true, selection->canvas()->viewConverter());
    }
}

bool Italic::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return style.italic();
}

Underline::Underline(Actions *actions)
    : ToggleableCellAction(actions, "underline", i18n("Underline"), koIcon("format-text-underline"), i18n("Underline"))
{
}

Underline::~Underline()
{
}

QAction *Underline::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    return res;
}

void Underline::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontUnderline(selected);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
    if (m_actions->tool()->editor()) {
        const Cell cell = Cell(sheet, selection->marker());
        m_actions->tool()->editor()->setEditorFont(cell.style().font(), true, selection->canvas()->viewConverter());
    }
}

bool Underline::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return style.underline();
}

Strikeout::Strikeout(Actions *actions)
    : ToggleableCellAction(actions, "strikeOut", i18n("Strike Out"), koIcon("format-text-strikethrough"), i18n("Strike Out"))
{
}

Strikeout::~Strikeout()
{
}

void Strikeout::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontStrikeOut(selected);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
    if (m_actions->tool()->editor()) {
        const Cell cell = Cell(sheet, selection->marker());
        m_actions->tool()->editor()->setEditorFont(cell.style().font(), true, selection->canvas()->viewConverter());
    }
}

bool Strikeout::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return style.strikeOut();
}

WrapText::WrapText(Actions *actions)
    : ToggleableCellAction(actions, "wrapText", i18n("Wrap Text"), koIcon("multirow"), i18n("Make the cell text wrap onto multiple lines"))
{
}

WrapText::~WrapText()
{
}

QAction *WrapText::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Wrap"));
    return res;
}

void WrapText::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Wrap Text"));
    Style s;
    s.setWrapText(selected);
    s.setVerticalText(false);
    s.setAngle(0);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool WrapText::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return style.wrapText();
}

VerticalText::VerticalText(Actions *actions)
    : ToggleableCellAction(actions, "verticalText", i18n("Vertical Text"), koIcon("format-text-direction-vertical"), i18n("Print cell contents vertically"))
{
}

VerticalText::~VerticalText()
{
}

void VerticalText::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Vertical Text"));
    Style s;
    s.setVerticalText(selected);
    s.setWrapText(false);
    s.setAngle(0);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool VerticalText::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    return style.verticalText();
}

PercentFormat::PercentFormat(Actions *actions)
    : ToggleableCellAction(actions, "percent", i18n("Percent Format"), koIcon("format-number-percent"), i18n("Set the cell formatting to look like a percentage"))
{
}

PercentFormat::~PercentFormat()
{
}

QAction *PercentFormat::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Percent"));
    return res;
}

void PercentFormat::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Format Percent"));
    Style s;
    s.setFormatType(selected ? Format::Percentage : Format::Generic);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool PercentFormat::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    Format::Type ft = style.formatType();
    return (ft == Format::Percentage);

}

MoneyFormat::MoneyFormat(Actions *actions)
    : ToggleableCellAction(actions, "currency", i18n("Money Format"), koIcon("format-currency"), i18n("Set the cell formatting to look like your local currency"))
{
}

MoneyFormat::~MoneyFormat()
{
}

QAction *MoneyFormat::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Money"));
    return res;
}

void MoneyFormat::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Format Money"));
    Style s;
    s.setFormatType(selected ? Format::Money : Format::Generic);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool MoneyFormat::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    Format::Type ft = style.formatType();
    return (ft == Format::Money);
}

IncreaseFontSize::IncreaseFontSize(Actions *actions)
    : CellAction(actions, "increaseFontSize", i18n("Increase Font Size"), koIcon("format-font-size-more"), QString())
{
}

IncreaseFontSize::~IncreaseFontSize()
{
}

void IncreaseFontSize::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    const Style style = Cell(sheet, selection->marker()).style();
    const int size = style.fontSize();
    if (size >= 300) return;

    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontSize(size + 1);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

DecreaseFontSize::DecreaseFontSize(Actions *actions)
    : CellAction(actions, "decreaseFontSize", i18n("Decrease Font Size"), koIcon("format-font-size-less"), QString())
{
}

DecreaseFontSize::~DecreaseFontSize()
{
}

void DecreaseFontSize::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    const Style style = Cell(sheet, selection->marker()).style();
    const int size = style.fontSize();
    if (size <= 1) return;

    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Font"));
    Style s;
    s.setFontSize(size - 1);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

DefaultStyle::DefaultStyle(Actions *actions)
    : CellAction(actions, "setDefaultStyle", i18n("Default"), QIcon(), i18n("Resets to the default format"))
{
}

DefaultStyle::~DefaultStyle()
{
}

void DefaultStyle::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Set Default Style"));
    Style s;
    s.setDefault();
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}







