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

#include <kcombobox.h>
#include <kdialog.h>
#include <klocale.h>
#include <kvbox.h>

#include "Sheet.h"

#include "SheetPropertiesDialog.h"

using namespace KSpread;

SheetPropertiesDialog::SheetPropertiesDialog( QWidget* parent ):
    KDialog( parent )
{
  setCaption( i18n("Sheet Properties") );
  setObjectName( "sheetPropertiesDialog" );
  setModal( true );
  setButtons( Ok|Cancel|Default );

  KVBox* mainWidget = new KVBox();//makeVBoxMainWidget();
  setMainWidget( mainWidget );
  m_widget = new SheetPropertiesWidget( mainWidget );
  QWidget* spacer = new QWidget( mainWidget );
  spacer->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
  showButtonSeparator( true );
}

SheetPropertiesDialog::~SheetPropertiesDialog()
{
  delete m_widget;
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
  if( m_widget->directionComboBox->currentText() == i18n( "Left to Right" ) )
    return Sheet::LeftToRight;

  if( m_widget->directionComboBox->currentText() == i18n( "Right to Left" ) )
    return Sheet::RightToLeft;

  // fallback
  return Sheet::LeftToRight;
}

void SheetPropertiesDialog::setLayoutDirection( Sheet::LayoutDirection dir )
{
  switch( dir )
  {
    case Sheet::LeftToRight:
      m_widget->directionComboBox->setItemText( m_widget->directionComboBox->currentIndex(), i18n( "Left to Right" ) );
      break;
    case Sheet::RightToLeft:
      m_widget->directionComboBox->setItemText( m_widget->directionComboBox->currentIndex(), i18n( "Right to Left" ) );
      break;
    default: break;
  };
}

bool SheetPropertiesDialog::autoCalc() const
{
  return m_widget->autoCalcCheckBox->isChecked();
}

void SheetPropertiesDialog::setAutoCalc( bool b )
{
  m_widget->autoCalcCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showGrid() const
{
  return m_widget->showGridCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowGrid( bool b )
{
  m_widget->showGridCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showPageBorders() const
{
  return m_widget->showPageBordersCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowPageBorders( bool b )
{
  m_widget->showPageBordersCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showFormula() const
{
  return m_widget->showFormulaCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowFormula( bool b )
{
  m_widget->showFormulaCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::hideZero() const
{
  return m_widget->hideZeroCheckBox->isChecked();
}

void SheetPropertiesDialog::setHideZero( bool b )
{
  m_widget->hideZeroCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showFormulaIndicator() const
{
  return m_widget->showFormulaIndicatorCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowFormulaIndicator( bool b )
{
  m_widget->showFormulaIndicatorCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::showCommentIndicator() const
{
  return m_widget->showCommentIndicatorCheckBox->isChecked();
}

void SheetPropertiesDialog::setShowCommentIndicator( bool b )
{
  m_widget->showCommentIndicatorCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::columnAsNumber() const
{
  return m_widget->showColumnAsNumbersCheckBox->isChecked();
}

void SheetPropertiesDialog::setColumnAsNumber( bool b )
{
  m_widget->showColumnAsNumbersCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::lcMode() const
{
  return m_widget->useLCModeCheckBox->isChecked();
}

void SheetPropertiesDialog::setLcMode( bool b )
{
  m_widget->useLCModeCheckBox->setChecked( b );
}

bool SheetPropertiesDialog::capitalizeFirstLetter() const
{
  return m_widget->capitalizeFirstLetterCheckBox->isChecked();
}

void SheetPropertiesDialog::setCapitalizeFirstLetter( bool b )
{
  m_widget->capitalizeFirstLetterCheckBox->setChecked( b );
}

#include "SheetPropertiesDialog.moc"
