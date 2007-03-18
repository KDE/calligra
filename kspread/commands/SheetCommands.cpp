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

#include "Damages.h"
#include "Doc.h"
#include "Localization.h"
#include "Map.h"
#include "Sheet.h"

#include "SheetCommands.h"

using namespace KSpread;

// ----- RenameSheetCommand -----

RenameSheetCommand::RenameSheetCommand( Sheet* s, const QString &name )
{
  sheet = s;
  if( s ) oldName = s->sheetName();
  newName = name;
}

QString RenameSheetCommand::name() const
{
  return i18n("Rename Sheet");
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

QString HideSheetCommand::name() const
{
    QString n =  i18n("Hide Sheet %1", sheetName );
    if( n.length() > 64 ) n = i18n("Hide Sheet");
    return n;
}

// ----- ShowSheetCommand -----

ShowSheetCommand::ShowSheetCommand( Sheet* sheet )
{
  doc = sheet->doc();
  sheetName = sheet->sheetName();
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

QString ShowSheetCommand::name() const
{
    QString n =  i18n("Show Sheet %1", sheetName );
    if( n.length() > 64 ) n = i18n("Show Sheet");
    return n;
}


// ----- AddSheetCommand -----

AddSheetCommand::AddSheetCommand( Sheet* s )
{
    sheet = s;
    doc = sheet->doc();
    doc->map()->addSheet( s );
}

void AddSheetCommand::redo()
{
    sheet->map()->insertSheet( sheet );
    doc->insertSheet( sheet );
}

void AddSheetCommand::undo()
{
    sheet->map()->takeSheet( sheet );
    doc->takeSheet( sheet );
}

QString AddSheetCommand::name() const
{
    return i18n("Add Sheet");
}


// ----- RemoveSheetCommand -----

RemoveSheetCommand::RemoveSheetCommand( Sheet* s )
{
    sheet = s;
    doc = sheet->doc();
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

QString RemoveSheetCommand::name() const
{
    return i18n("Remove Sheet");
}

// ----- SheetPropertiesCommand -----

SheetPropertiesCommand::SheetPropertiesCommand( Doc* d, Sheet* s )
{
    sheet = s;
    doc = d;
    oldDirection = newDirection = sheet->layoutDirection();
    oldAutoCalc = newAutoCalc = sheet->getAutoCalc();
    oldShowGrid = newShowGrid = sheet->getShowGrid();
    oldShowPageBorders = newShowPageBorders = sheet->isShowPageBorders();
    oldShowFormula = newShowFormula = sheet->getShowFormula();
    oldHideZero = newHideZero = sheet->getHideZero();
    oldShowFormulaIndicator = newShowFormulaIndicator = sheet->getShowFormulaIndicator();
    oldShowCommentIndicator = newShowCommentIndicator = sheet->getShowCommentIndicator();
    oldColumnAsNumber = newColumnAsNumber = sheet->getShowColumnNumber();
    oldLcMode = newLcMode = sheet->getLcMode();
    oldCapitalizeFirstLetter = newCapitalizeFirstLetter = sheet->getFirstLetterUpper();
}

QString SheetPropertiesCommand::name() const
{
    return i18n("Change Sheet Properties");
}

void SheetPropertiesCommand::setLayoutDirection( Sheet::LayoutDirection dir )
{
    newDirection = dir;
}

void SheetPropertiesCommand::setAutoCalc( bool b )
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
    sheet->setAutoCalc( newAutoCalc );
    sheet->setShowGrid( newShowGrid );
    sheet->setShowPageBorders( newShowPageBorders );
    sheet->setShowFormula( newShowFormula );
    sheet->setHideZero( newHideZero );
    sheet->setShowFormulaIndicator( newShowFormulaIndicator );
    sheet->setShowCommentIndicator( newShowCommentIndicator );
    sheet->setShowColumnNumber( newColumnAsNumber );
    sheet->setLcMode( newLcMode );
    sheet->setFirstLetterUpper( newCapitalizeFirstLetter );
    doc->addDamage( new SheetDamage( sheet, SheetDamage::PropertiesChanged ) );
}

void SheetPropertiesCommand::undo()
{
    sheet->setLayoutDirection( oldDirection );
    sheet->setAutoCalc( oldAutoCalc );
    sheet->setShowGrid( oldShowGrid );
    sheet->setShowPageBorders( oldShowPageBorders );
    sheet->setShowFormula( oldShowFormula );
    sheet->setHideZero( oldHideZero );
    sheet->setShowFormulaIndicator( oldShowFormulaIndicator );
    sheet->setShowCommentIndicator( oldShowCommentIndicator );
    sheet->setShowColumnNumber( oldColumnAsNumber );
    sheet->setLcMode( oldLcMode );
    sheet->setFirstLetterUpper( oldCapitalizeFirstLetter );
    doc->addDamage( new SheetDamage( sheet, SheetDamage::PropertiesChanged ) );
}
