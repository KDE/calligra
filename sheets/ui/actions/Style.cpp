/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Style.h"
#include "Actions.h"

#include "engine/CalculationSettings.h"
#include "engine/MapBase.h"
#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "ui/commands/StyleCommand.h"
#include "ui/dialogs/LayoutDialog.h"
#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"

#include <KoCanvasBase.h>
#include <KoColor.h>
#include <KoColorPopupAction.h>

#include <KLocalizedString>


using namespace Calligra::Sheets;


CellStyle::CellStyle(Actions *actions)
    : CellAction(actions, "cellStyle", i18n("Cell Format..."), koIcon("cell_layout"), i18n("Set the cell formatting"))
    , m_dlg(nullptr)
{
}

CellStyle::~CellStyle()
{
    if (m_dlg) delete m_dlg;
}

void CellStyle::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_dlg = new LayoutDialog(canvasWidget, sheet, nullptr, false);
    QRect range = selection->firstRange();
    CellStorage *cs = sheet->fullCellStorage();
    Style style = cs->style(range);
    bool multicell = ((range.width() > 1) || (range.height() > 1));
    m_dlg->setStyle(style, multicell);
    if (m_dlg->exec()) {
        Style style = m_dlg->style(multicell);
        StyleCommand* command = new StyleCommand();
        command->setSheet(sheet);
        command->add(*selection);
        command->setStyle(style);
        command->execute(selection->canvas());
    }
    delete m_dlg;
    m_dlg = nullptr;
}

QAction *CellStyle::createAction() {
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Format"));
    res->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_F));
    return res;
}



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


NumberFormat::NumberFormat(Actions *actions)
    : ToggleableCellAction(actions, "numeric", i18n("Number Format"), QIcon(), i18n("Set the cell formatting to a number"))
{
}

NumberFormat::~NumberFormat()
{
}

QAction *NumberFormat::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Number"));
    return res;
}

void NumberFormat::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Format Number"));
    Style s;
    s.setFormatType(selected ? Format::Number : Format::Generic);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool NumberFormat::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    Format::Type ft = style.formatType();
    return isNumber(ft);
}

ScientificFormat::ScientificFormat(Actions *actions)
    : ToggleableCellAction(actions, "scientific", i18n("Scientific Format"), koIcon("wizard_math"), i18n("Set the cell formatting to a scientific format"))
{
}

ScientificFormat::~ScientificFormat()
{
}

QAction *ScientificFormat::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Scientific"));
    return res;
}

void ScientificFormat::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Format Scientific"));
    Style s;
    s.setFormatType(selected ? Format::Scientific : Format::Generic);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool ScientificFormat::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    Format::Type ft = style.formatType();
    return (ft == Format::Scientific);
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

DateFormat::DateFormat(Actions *actions)
    : ToggleableCellAction(actions, "date", i18n("Date Format"), koIcon("view-calendar"), i18n("Set the cell formatting to a date"))
{
}

DateFormat::~DateFormat()
{
}

QAction *DateFormat::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Date"));
    return res;
}

void DateFormat::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Format Date"));
    Style s;
    s.setFormatType(selected ? Format::ShortDate : Format::Generic);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool DateFormat::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    Format::Type ft = style.formatType();
    return isDate(ft);
}

TimeFormat::TimeFormat(Actions *actions)
    : ToggleableCellAction(actions, "time", i18n("Time Format"), koIcon("player-time"), i18n("Set the cell formatting to a time"))
{
}

TimeFormat::~TimeFormat()
{
}

QAction *TimeFormat::createAction() {
    QAction *res = ToggleableCellAction::createAction();
    res->setIconText(i18n("Time"));
    return res;
}

void TimeFormat::executeToggled(bool selected, Selection *selection, Sheet *sheet, QWidget *)
{
    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Format Time"));
    Style s;
    s.setFormatType(selected ? Format::LongTime : Format::Generic);
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool TimeFormat::checkedForSelection(Selection *, const Cell &activeCell)
{
    const Style style = activeCell.style();
    Format::Type ft = style.formatType();
    return isTime(ft);
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


IncreasePrecision::IncreasePrecision(Actions *actions)
    : CellAction(actions, "increasePrecision", i18n("Increase Precision"), koIcon("format-precision-more"), i18n("Increase the decimal precision shown onscreen"))
{
}

IncreasePrecision::~IncreasePrecision()
{
}

void IncreasePrecision::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    PrecisionCommand* command = new PrecisionCommand();
    command->setSheet(sheet);
    command->add(*selection);
    command->execute(selection->canvas());
}

DecreasePrecision::DecreasePrecision(Actions *actions)
    : CellAction(actions, "decreasePrecision", i18n("Decrease Precision"), koIcon("format-precision-less"), i18n("Decrease the decimal precision shown onscreen"))
{
}

DecreasePrecision::~DecreasePrecision()
{
}

void DecreasePrecision::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    PrecisionCommand* command = new PrecisionCommand();
    command->setSheet(sheet);
    command->add(*selection);
    command->setDecrease(true);
    command->execute(selection->canvas());
}

PrecisionCommand::PrecisionCommand()
        : AbstractRegionCommand()
{
    setDecrease(false);
}

bool PrecisionCommand::performCommands()
{
    Style curStyle = m_sheet->fullCellStorage()->style(boundingRect().left(), boundingRect().top());
    int prec = curStyle.precision();

    if (prec < 0) prec = m_sheet->map()->calculationSettings()->defaultDecimalPrecision();
    if (prec < 0) prec = 2;
    prec += (m_decrease ? -1 : 1);
    if (prec > 10) prec = 10;
    if (prec < 0) prec = 0;

    Style style;
    style.setPrecision(prec);
    m_sheet->fullCellStorage()->setStyle(*this, style);
    return true;
}

void PrecisionCommand::setDecrease(bool decrease)
{
    m_decrease = decrease;
    if (!m_decrease)
        setText(kundo2_i18n("Increase Precision"));
    else
        setText(kundo2_i18n("Decrease Precision"));
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


FillColor::FillColor(Actions *actions)
    : CellAction(actions, "backgroundColor", i18n("Background Color"), koIcon("format-fill-color"), i18n("Set the background color"))
{
}

FillColor::~FillColor()
{
}

QAction *FillColor::createAction() {
    m_colorAction = new KoColorPopupAction(m_actions->tool());
    m_colorAction->setIcon(m_icon);
    m_colorAction->setToolTip(m_tooltip);
    m_colorAction->setText(m_caption);
    connect(m_colorAction, &KoColorPopupAction::colorChanged, this, &FillColor::triggeredFillColor);
    return m_colorAction;
}

void FillColor::triggeredFillColor(const KoColor &color) {
    CellToolBase *tool = m_actions->tool();
    Selection *selection = tool->selection();
    Sheet *sheet = selection->activeSheet();

    StyleCommand* command = new StyleCommand();
    command->setSheet(sheet);
    command->setText(kundo2_i18n("Change Background Color"));
    Style s;
    s.setBackgroundColor(color.toQColor());
    command->setStyle(s);
    command->add(*selection);
    command->execute(selection->canvas());
}






