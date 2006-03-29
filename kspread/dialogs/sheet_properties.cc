/* This file is part of the KDE project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include <q3vbox.h>
#include <qcheckbox.h>

#include <kcombobox.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <kvbox.h>
#include "kspread_sheet.h"
#include "sheet_properties_base.h"

#include "sheet_properties.h"

using namespace KSpread;

SheetPropertiesDialog::SheetPropertiesDialog( QWidget* parent ):
  KDialogBase( parent, "sheetPropertiesDialog", true, 
  i18n("Sheet Properties"), 
  KDialogBase::Ok|KDialogBase::Cancel|KDialogBase::Default )
{
  KVBox* mainWidget = makeVBoxMainWidget();
  d = new SheetPropertiesBase( mainWidget );
  QWidget* spacer = new QWidget( mainWidget );
  spacer->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
  enableButtonSeparator( true );
}

SheetPropertiesDialog::~SheetPropertiesDialog()
{
  delete d;
}

void SheetPropertiesDialog::slotDefault()
{
  setLayoutDirection( Sheet::LeftToRight );
  setAutoCalc( true );
  setShowGrid( true );
  setShowFormula( false );
  setHideZero( false );
  setShowFormulaIndicator( true );
  setShowCommentIndicator( true );
  setShowPageBorders( false );
  setColumnAsNumber( false );
  setLcMode( false );
  setCapitalizeFirstLetter( false );
}

Sheet::LayoutDirection SheetPropertiesDialog::layoutDirection() const
{
  if( d->directionComboBox->currentText() == i18n( "Left to Right" ) )
    return Sheet::LeftToRight;
  
  if( d->directionComboBox->currentText() == i18n( "Right to Left" ) )
    return Sheet::RightToLeft;
  
  // fallback
  return Sheet::LeftToRight;
}

void SheetPropertiesDialog::setLayoutDirection( Sheet::LayoutDirection dir )
{
  switch( dir )
  {
    case Sheet::LeftToRight:
      d->directionComboBox->setCurrentText( i18n( "Left to Right" ) );
      break;
    case Sheet::RightToLeft:
      d->directionComboBox->setCurrentText( i18n( "Right to Left" ) );
      break;
    default: break;
  };
}

bool SheetPropertiesDialog::autoCalc() const
{
  return d->autoCalcCheckBox->isChecked();
}

void SheetPropertiesDialog::setAutoCalc( bool b )
{
  d->autoCalcCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showGrid() const
{
  return d->showGridCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowGrid( bool b )
{
  d->showGridCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showPageBorders() const
{
  return d->showPageBordersCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setShowPageBorders( bool b )
{
  d->showPageBordersCheckBox->setChecked( b );
}   

bool SheetPropertiesDialog::showFormula() const
{
  return d->showFormulaCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setShowFormula( bool b )
{
  d->showFormulaCheckBox->setChecked( b );
}
    
bool SheetPropertiesDialog::hideZero() const
{
  return d->hideZeroCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setHideZero( bool b )
{
  d->hideZeroCheckBox->setChecked( b );
}
    
bool SheetPropertiesDialog::showFormulaIndicator() const
{
  return d->showFormulaIndicatorCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setShowFormulaIndicator( bool b )
{
  d->showFormulaIndicatorCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showCommentIndicator() const
{
  return d->showCommentIndicatorCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setShowCommentIndicator( bool b )
{
  d->showCommentIndicatorCheckBox->setChecked( b );
}
    
bool SheetPropertiesDialog::columnAsNumber() const
{
  return d->showColumnAsNumbersCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setColumnAsNumber( bool b )
{
  d->showColumnAsNumbersCheckBox->setChecked( b );
}
    
bool SheetPropertiesDialog::lcMode() const
{
  return d->useLCModeCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setLcMode( bool b )
{
  d->useLCModeCheckBox->setChecked( b );
}    
    
bool SheetPropertiesDialog::capitalizeFirstLetter() const
{
  return d->capitalizeFirstLetterCheckBox->isChecked();
}
    
void SheetPropertiesDialog::setCapitalizeFirstLetter( bool b )
{
  d->capitalizeFirstLetterCheckBox->setChecked( b );
}    
    
#include "sheet_properties.moc"
