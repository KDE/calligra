/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Fill.h"
#include "Actions.h"

#include <KLocalizedString>

#include "core/Cell.h"

using namespace Calligra::Sheets;

Fill::Fill(Actions *actions, FillManipulator::Direction dir)
    : CellAction(actions, actionName(dir), actionCaption(dir), QIcon(), QString())
    , m_dir(dir)
{
}

Fill::~Fill() = default;

void Fill::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    FillManipulator *command = new FillManipulator();
    command->setSheet(sheet);
    command->setDirection(m_dir);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool Fill::enabledForSelection(Selection *selection, const Cell &)
{
    if (selection->isSingular())
        return false;
    if (selection->isColumnSelected())
        return false;
    if (selection->isRowSelected())
        return false;
    return true;
}

QString Fill::actionName(FillManipulator::Direction dir)
{
    switch (dir) {
    case FillManipulator::Up:
        return "fillUp";
    case FillManipulator::Down:
        return "fillDown";
    case FillManipulator::Left:
        return "fillLeft";
    case FillManipulator::Right:
        return "fillRight";
    };
    return QString();
}

QString Fill::actionCaption(FillManipulator::Direction dir)
{
    switch (dir) {
    case FillManipulator::Up:
        return i18n("Fill &Up");
    case FillManipulator::Down:
        return i18n("Fill &Down");
    case FillManipulator::Left:
        return i18n("Fill &Left");
    case FillManipulator::Right:
        return i18n("Fill &Right");
    };
    return QString();
}

FillManipulator::FillManipulator()
{
    m_dir = Down;
    m_changeformat = true;
    setText(kundo2_i18n("Fill Selection"));
}

FillManipulator::~FillManipulator() = default;

Value FillManipulator::newValue(Element *element, int col, int row, bool *parse, Format::Type *fmtType)
{
    Q_UNUSED(fmtType);
    const int targetRow = row;
    const int targetCol = col;
    switch (m_dir) {
    case Up:
        row = element->rect().bottom();
        break;
    case Down:
        row = element->rect().top();
        break;
    case Left:
        col = element->rect().right();
        break;
    case Right:
        col = element->rect().left();
        break;
    };
    Cell cell(m_sheet, col, row); // the reference cell
    if (cell.isFormula()) {
        *parse = true;
        return Value(Cell(m_sheet, targetCol, targetRow).decodeFormula(cell.encodeFormula()));
    }
    return cell.value();
}

Style FillManipulator::newFormat(Element *element, int col, int row)
{
    switch (m_dir) {
    case Up:
        row = element->rect().bottom();
        break;
    case Down:
        row = element->rect().top();
        break;
    case Left:
        col = element->rect().right();
        break;
    case Right:
        col = element->rect().left();
        break;
    };
    return Cell(m_sheet, col, row).style();
}
