/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TextCase.h"
#include "Actions.h"

#include <KLocalizedString>

#include "core/Cell.h"

using namespace Calligra::Sheets;

TextCase::TextCase(Actions *actions, CaseManipulator::CaseMode mode)
    : CellAction(actions, actionName(mode), actionCaption(mode), actionIcon(mode), actionToolTip(mode))
    , m_mode(mode)
{
}

TextCase::~TextCase() = default;

QAction *TextCase::createAction()
{
    QAction *res = CellAction::createAction();
    QString itext;
    switch (m_mode) {
    case CaseManipulator::Upper:
        itext = i18n("Upper");
        break;
    case CaseManipulator::Lower:
        itext = i18n("Lower");
        break;
    case CaseManipulator::FirstUpper:
        itext = i18n("First Letter Upper");
        break;
    };
    res->setIconText(itext);
    return res;
}

void TextCase::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    CaseManipulator *command = new CaseManipulator;

    switch (m_mode) {
    case CaseManipulator::Upper:
        command->setText(kundo2_i18n("Switch to uppercase"));
        break;
    case CaseManipulator::Lower:
        command->setText(kundo2_i18n("Switch to lowercase"));
        break;
    case CaseManipulator::FirstUpper:
        command->setText(kundo2_i18n("First letter uppercase"));
        break;
    };
    command->changeMode(m_mode);
    command->add(*selection);
    command->setSheet(sheet);
    command->execute(selection->canvas());
}

QString TextCase::actionName(CaseManipulator::CaseMode mode)
{
    switch (mode) {
    case CaseManipulator::Upper:
        return "toUpperCase";
    case CaseManipulator::Lower:
        return "toLowerCase";
    case CaseManipulator::FirstUpper:
        return "firstLetterToUpperCase";
    };
    return QString();
}

QIcon TextCase::actionIcon(CaseManipulator::CaseMode mode)
{
    switch (mode) {
    case CaseManipulator::Upper:
        return koIconWanted("no icon in Kate, but LO has one", "format-text-uppercase");
    case CaseManipulator::Lower:
        return koIconWanted("no icon in Kate, but LO has one", "format-text-lowercase");
    case CaseManipulator::FirstUpper:
        return koIcon("format-text-capitalize");
    };
    return QIcon();
}

QString TextCase::actionCaption(CaseManipulator::CaseMode mode)
{
    switch (mode) {
    case CaseManipulator::Upper:
        return i18n("Upper Case");
    case CaseManipulator::Lower:
        return i18n("Lower Case");
    case CaseManipulator::FirstUpper:
        return i18n("Convert First Letter to Upper Case");
    };
    return QString();
}

QString TextCase::actionToolTip(CaseManipulator::CaseMode mode)
{
    switch (mode) {
    case CaseManipulator::Upper:
        return i18n("Convert all letters to upper case");
    case CaseManipulator::Lower:
        return i18n("Convert all letters to lower case");
    case CaseManipulator::FirstUpper:
        return i18n("Capitalize the first letter");
    };
    return QString();
}

CaseManipulator::CaseManipulator()
{
    m_mode = Upper;
    setText(kundo2_i18n("Change Case"));
}

CaseManipulator::~CaseManipulator() = default;

Value CaseManipulator::newValue(Element *element, int col, int row, bool *parse, Format::Type *)
{
    Q_UNUSED(element)
    // if we are here, we know that we want the change
    *parse = false;
    QString str = Cell(m_sheet, col, row).value().asString();
    switch (m_mode) {
    case Upper:
        str = str.toUpper();
        break;
    case Lower:
        str = str.toLower();
        break;
    case FirstUpper:
        if (str.length() > 0)
            str = str.at(0).toUpper() + str.right(str.length() - 1);
        break;
    };
    return Value(str);
}

bool CaseManipulator::wantChange(Element *element, int col, int row)
{
    Q_UNUSED(element)
    Cell cell(m_sheet, col, row);
    // don't change cells with a formula
    if (cell.isFormula())
        return false;
    // don't change cells containing other things than strings
    if (!cell.value().isString())
        return false;
    // original version was dismissing text starting with '!' and '*', is this
    // necessary ?
    return true;
}
