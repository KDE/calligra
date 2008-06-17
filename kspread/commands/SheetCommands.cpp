/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

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

// Local
#include "SheetCommands.h"

#include "Damages.h"
#include "Doc.h"
#include "Localization.h"
#include "Map.h"
#include "Sheet.h"

using namespace KSpread;

// ----- RenameSheetCommand -----

RenameSheetCommand::RenameSheetCommand( Sheet* s, const QString &name )
{
  sheet = s;
  if( s ) oldName = s->sheetName();
  newName = name;
  setText(i18n("Rename Sheet"));
}

void RenameSheetCommand::redo()
{
  if( sheet )
    sheet->setSheetName( newName );
}

void RenameSheetCommand::undo()
{
  if( sheet )
    sheet->setSheetName( oldName );
}

// ----- HideSheetCommand -----

HideSheetCommand::HideSheetCommand( Sheet* sheet )
{
  doc = sheet->doc();
  sheetName = sheet->sheetName();
  QString n =  i18n("Hide Sheet %1", sheetName );
  if( n.length() > 64 ) n = i18n("Hide Sheet");
  setText(n);
}

void HideSheetCommand::redo()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( true );
}

void HideSheetCommand::undo()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( false );
}

// ----- ShowSheetCommand -----

ShowSheetCommand::ShowSheetCommand( Sheet* sheet )
{
  doc = sheet->doc();
  sheetName = sheet->sheetName();
  QString n =  i18n("Show Sheet %1", sheetName );
  if( n.length() > 64 ) n = i18n("Show Sheet");
  setText(n);
}

void ShowSheetCommand::redo()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( false );
}

void ShowSheetCommand::undo()
{
  Sheet* sheet = doc->map()->findSheet( sheetName );
  if( !sheet ) return;

  sheet->hideSheet( true );
}


// ----- AddSheetCommand -----

AddSheetCommand::AddSheetCommand(Sheet* sheet)
    : QUndoCommand(i18n("Add Sheet"))
    , m_sheet(sheet)
    , m_firstrun(true)
{
}

void AddSheetCommand::redo()
{
    if (m_firstrun)
    {
        m_sheet->map()->addSheet(m_sheet);
        m_firstrun = false;
    }
    else
    {
        m_sheet->map()->insertSheet(m_sheet);
        m_sheet->doc()->insertSheet(m_sheet);
    }
}

void AddSheetCommand::undo()
{
    // The sheet becomes a zombie, i.e. it is not deleted,
    // so that the sheet pointer used in other commands later on stays valid.
    m_sheet->map()->takeSheet(m_sheet);
    m_sheet->doc()->takeSheet(m_sheet);
}


// ----- DuplicateSheetCommand -----

DuplicateSheetCommand::DuplicateSheetCommand()
    : QUndoCommand(i18n("Duplicate Sheet"))
    , m_oldSheet(0)
    , m_newSheet(0)
    , m_firstrun(true)
{
}

void DuplicateSheetCommand::setSheet(Sheet* sheet)
{
    m_oldSheet = sheet;
}

void DuplicateSheetCommand::redo()
{
    // Once created the sheet stays alive forever. See comment in undo.
    if (m_firstrun)
    {
        m_newSheet = new Sheet(*m_oldSheet);
        m_newSheet->map()->insertSheet(m_newSheet);
        m_newSheet->map()->emitAddSheet(m_newSheet);
        m_firstrun = false;
    }
    else
    {
        m_newSheet->map()->insertSheet(m_newSheet);
        m_newSheet->doc()->insertSheet(m_newSheet);
    }
}

void DuplicateSheetCommand::undo()
{
    // The new sheet is not deleted, but just becomes a zombie,
    // so that the sheet pointer used in commands later on stays valid.
    m_newSheet->map()->takeSheet(m_newSheet);
    m_newSheet->doc()->takeSheet(m_newSheet);
}


// ----- RemoveSheetCommand -----

RemoveSheetCommand::RemoveSheetCommand( Sheet* s )
{
    sheet = s;
    doc = sheet->doc();
    setText(i18n("Remove Sheet"));
}

void RemoveSheetCommand::redo()
{
    sheet->map()->takeSheet( sheet );
    doc->takeSheet( sheet );
}

void RemoveSheetCommand::undo()
{
    sheet->map()->insertSheet( sheet );
    doc->insertSheet( sheet );
}

// ----- SheetPropertiesCommand -----

SheetPropertiesCommand::SheetPropertiesCommand( Doc* d, Sheet* s )
{
    sheet = s;
    doc = d;
    oldDirection = newDirection = sheet->layoutDirection();
    oldAutoCalc = newAutoCalc = sheet->isAutoCalculationEnabled();
    oldShowGrid = newShowGrid = sheet->getShowGrid();
    oldShowPageBorders = newShowPageBorders = sheet->isShowPageBorders();
    oldShowFormula = newShowFormula = sheet->getShowFormula();
    oldHideZero = newHideZero = sheet->getHideZero();
    oldShowFormulaIndicator = newShowFormulaIndicator = sheet->getShowFormulaIndicator();
    oldShowCommentIndicator = newShowCommentIndicator = sheet->getShowCommentIndicator();
    oldColumnAsNumber = newColumnAsNumber = sheet->getShowColumnNumber();
    oldLcMode = newLcMode = sheet->getLcMode();
    oldCapitalizeFirstLetter = newCapitalizeFirstLetter = sheet->getFirstLetterUpper();
    setText(i18n("Change Sheet Properties"));
}

void SheetPropertiesCommand::setLayoutDirection( Qt::LayoutDirection dir )
{
    newDirection = dir;
}

void SheetPropertiesCommand::setAutoCalculationEnabled( bool b )
{
    newAutoCalc = b;
}

void SheetPropertiesCommand::setShowGrid( bool b )
{
    newShowGrid = b;
}

void SheetPropertiesCommand::setShowPageBorders( bool b )
{
    newShowPageBorders = b;
}

void SheetPropertiesCommand::setShowFormula( bool b )
{
    newShowFormula = b;
}

void SheetPropertiesCommand::setHideZero( bool b  )
{
    newHideZero = b;
}

void SheetPropertiesCommand::setShowFormulaIndicator( bool b )
{
    newShowFormulaIndicator = b;
}

void SheetPropertiesCommand::setShowCommentIndicator( bool b )
{
  newShowCommentIndicator = b;
}

void SheetPropertiesCommand::setColumnAsNumber( bool b  )
{
    newColumnAsNumber = b;
}

void SheetPropertiesCommand::setLcMode( bool b  )
{
    newLcMode = b;
}

void SheetPropertiesCommand::setCapitalizeFirstLetter( bool b )
{
    newCapitalizeFirstLetter = b;
}

void SheetPropertiesCommand::redo()
{
    sheet->setLayoutDirection( newDirection );
    sheet->setAutoCalculationEnabled( newAutoCalc );
    sheet->setShowGrid( newShowGrid );
    sheet->setShowPageBorders( newShowPageBorders );
    sheet->setShowFormula( newShowFormula );
    sheet->setHideZero( newHideZero );
    sheet->setShowFormulaIndicator( newShowFormulaIndicator );
    sheet->setShowCommentIndicator( newShowCommentIndicator );
    sheet->setShowColumnNumber( newColumnAsNumber );
    sheet->setLcMode( newLcMode );
    sheet->setFirstLetterUpper( newCapitalizeFirstLetter );
    sheet->map()->addDamage( new SheetDamage( sheet, SheetDamage::PropertiesChanged ) );
}

void SheetPropertiesCommand::undo()
{
    sheet->setLayoutDirection( oldDirection );
    sheet->setAutoCalculationEnabled( oldAutoCalc );
    sheet->setShowGrid( oldShowGrid );
    sheet->setShowPageBorders( oldShowPageBorders );
    sheet->setShowFormula( oldShowFormula );
    sheet->setHideZero( oldHideZero );
    sheet->setShowFormulaIndicator( oldShowFormulaIndicator );
    sheet->setShowCommentIndicator( oldShowCommentIndicator );
    sheet->setShowColumnNumber( oldColumnAsNumber );
    sheet->setLcMode( oldLcMode );
    sheet->setFirstLetterUpper( oldCapitalizeFirstLetter );
    sheet->map()->addDamage( new SheetDamage( sheet, SheetDamage::PropertiesChanged ) );
}
