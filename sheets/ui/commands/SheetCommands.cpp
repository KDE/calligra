/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2004 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SheetCommands.h"

#include "core/Sheet.h"
#include "engine/Damages.h"
#include "engine/MapBase.h"

using namespace Calligra::Sheets;

// ----- RenameSheetCommand -----

RenameSheetCommand::RenameSheetCommand(Sheet *s, const QString &name)
{
    sheet = s;
    if (s)
        oldName = s->sheetName();
    newName = name;
    setText(kundo2_i18n("Rename Sheet"));
}

void RenameSheetCommand::redo()
{
    if (sheet)
        sheet->setSheetName(newName);
}

void RenameSheetCommand::undo()
{
    if (sheet)
        sheet->setSheetName(oldName);
}

// ----- HideSheetCommand -----

HideSheetCommand::HideSheetCommand(Sheet *sheet)
{
    map = sheet->map();
    sheetName = sheet->sheetName();
    KUndo2MagicString n = kundo2_i18n("Hide Sheet %1", sheetName);
    if (n.toString().length() > 64)
        n = kundo2_i18n("Hide Sheet");
    setText(n);
}

void HideSheetCommand::redo()
{
    SheetBase *sheet = map->findSheet(sheetName);
    if (!sheet)
        return;

    sheet->hideSheet(true);
}

void HideSheetCommand::undo()
{
    SheetBase *sheet = map->findSheet(sheetName);
    if (!sheet)
        return;

    sheet->hideSheet(false);
}

// ----- ShowSheetCommand -----

ShowSheetCommand::ShowSheetCommand(Sheet *sheet, KUndo2Command *parent)
    : KUndo2Command(parent)
{
    map = sheet->map();
    sheetName = sheet->sheetName();
    KUndo2MagicString n = kundo2_i18n("Show Sheet %1", sheetName);
    if (n.toString().length() > 64)
        n = kundo2_i18n("Show Sheet");
    setText(n);
}

void ShowSheetCommand::redo()
{
    SheetBase *sheet = map->findSheet(sheetName);
    if (!sheet)
        return;

    sheet->hideSheet(false);
}

void ShowSheetCommand::undo()
{
    SheetBase *sheet = map->findSheet(sheetName);
    if (!sheet)
        return;

    sheet->hideSheet(true);
}

// ----- AddSheetCommand -----

AddSheetCommand::AddSheetCommand(Sheet *sheet)
    : KUndo2Command(kundo2_i18n("Add Sheet"))
    , m_sheet(sheet)
    , m_firstrun(true)
{
}

void AddSheetCommand::redo()
{
    if (m_firstrun) {
        m_sheet->map()->addSheet(m_sheet);
        m_firstrun = false;
    } else {
        m_sheet->map()->reviveSheet(m_sheet);
    }
}

void AddSheetCommand::undo()
{
    // The sheet becomes a zombie, i.e. it is not deleted,
    // so that the sheet pointer used in other commands later on stays valid.
    m_sheet->map()->removeSheet(m_sheet);
}

// ----- DuplicateSheetCommand -----

DuplicateSheetCommand::DuplicateSheetCommand()
    : KUndo2Command(kundo2_i18n("Duplicate Sheet"))
    , m_oldSheet(nullptr)
    , m_newSheet(nullptr)
    , m_firstrun(true)
{
}

void DuplicateSheetCommand::setSheet(Sheet *sheet)
{
    m_oldSheet = sheet;
}

void DuplicateSheetCommand::redo()
{
    // Once created the sheet stays alive forever. See comment in undo.
    if (m_firstrun) {
        m_newSheet = new Sheet(*m_oldSheet);
        m_newSheet->map()->addSheet(m_newSheet);
        m_firstrun = false;
    } else {
        m_newSheet->map()->reviveSheet(m_newSheet);
    }
}

void DuplicateSheetCommand::undo()
{
    // The new sheet is not deleted, but just becomes a zombie,
    // so that the sheet pointer used in commands later on stays valid.
    m_newSheet->map()->removeSheet(m_newSheet);
}

// ----- RemoveSheetCommand -----

RemoveSheetCommand::RemoveSheetCommand(Sheet *s)
{
    sheet = s;
    map = sheet->map();
    setText(kundo2_i18n("Remove Sheet"));
}

void RemoveSheetCommand::redo()
{
    sheet->map()->removeSheet(sheet);
}

void RemoveSheetCommand::undo()
{
    sheet->map()->reviveSheet(sheet);
}

// ----- SheetPropertiesCommand -----

SheetPropertiesCommand::SheetPropertiesCommand(Sheet *s)
{
    sheet = s;
    map = s->map();
    oldDirection = newDirection = sheet->layoutDirection();
    oldAutoCalc = newAutoCalc = sheet->isAutoCalculationEnabled();
    oldShowGrid = newShowGrid = sheet->getShowGrid();
    oldShowPageOutline = newShowPageOutline = sheet->isShowPageOutline();
    oldShowFormula = newShowFormula = sheet->getShowFormula();
    oldHideZero = newHideZero = sheet->getHideZero();
    oldShowFormulaIndicator = newShowFormulaIndicator = sheet->getShowFormulaIndicator();
    oldShowCommentIndicator = newShowCommentIndicator = sheet->getShowCommentIndicator();
    oldColumnAsNumber = newColumnAsNumber = sheet->getShowColumnNumber();
    oldLcMode = newLcMode = sheet->getLcMode();
    oldCapitalizeFirstLetter = newCapitalizeFirstLetter = sheet->getFirstLetterUpper();
    setText(kundo2_i18n("Change Sheet Properties"));
}

void SheetPropertiesCommand::setLayoutDirection(Qt::LayoutDirection dir)
{
    newDirection = dir;
}

void SheetPropertiesCommand::setAutoCalculationEnabled(bool b)
{
    newAutoCalc = b;
}

void SheetPropertiesCommand::setShowGrid(bool b)
{
    newShowGrid = b;
}

void SheetPropertiesCommand::setShowPageOutline(bool b)
{
    newShowPageOutline = b;
}

void SheetPropertiesCommand::setShowFormula(bool b)
{
    newShowFormula = b;
}

void SheetPropertiesCommand::setHideZero(bool b)
{
    newHideZero = b;
}

void SheetPropertiesCommand::setShowFormulaIndicator(bool b)
{
    newShowFormulaIndicator = b;
}

void SheetPropertiesCommand::setShowCommentIndicator(bool b)
{
    newShowCommentIndicator = b;
}

void SheetPropertiesCommand::setColumnAsNumber(bool b)
{
    newColumnAsNumber = b;
}

void SheetPropertiesCommand::setLcMode(bool b)
{
    newLcMode = b;
}

void SheetPropertiesCommand::setCapitalizeFirstLetter(bool b)
{
    newCapitalizeFirstLetter = b;
}

void SheetPropertiesCommand::redo()
{
    sheet->setLayoutDirection(newDirection);
    sheet->setAutoCalculationEnabled(newAutoCalc);
    sheet->setShowGrid(newShowGrid);
    sheet->setShowPageOutline(newShowPageOutline);
    sheet->setShowFormula(newShowFormula);
    sheet->setHideZero(newHideZero);
    sheet->setShowFormulaIndicator(newShowFormulaIndicator);
    sheet->setShowCommentIndicator(newShowCommentIndicator);
    sheet->setShowColumnNumber(newColumnAsNumber);
    sheet->setLcMode(newLcMode);
    sheet->setFirstLetterUpper(newCapitalizeFirstLetter);
    sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::PropertiesChanged));
}

void SheetPropertiesCommand::undo()
{
    sheet->setLayoutDirection(oldDirection);
    sheet->setAutoCalculationEnabled(oldAutoCalc);
    sheet->setShowGrid(oldShowGrid);
    sheet->setShowPageOutline(oldShowPageOutline);
    sheet->setShowFormula(oldShowFormula);
    sheet->setHideZero(oldHideZero);
    sheet->setShowFormulaIndicator(oldShowFormulaIndicator);
    sheet->setShowCommentIndicator(oldShowCommentIndicator);
    sheet->setShowColumnNumber(oldColumnAsNumber);
    sheet->setLcMode(oldLcMode);
    sheet->setFirstLetterUpper(oldCapitalizeFirstLetter);
    sheet->map()->addDamage(new SheetDamage(sheet, SheetDamage::PropertiesChanged));
}
