/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000 Montel Laurent <montell@club-internet.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qprinter.h>
#include <qgroupbox.h>

#include "kspread_dlg_preference.h"
#include "kspread_view.h"
#include "kspread_table.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>


KSpreadpreference::KSpreadpreference( KSpreadView* parent, const char* name)
	: QDialog( parent, name,TRUE )
{
  m_pView = parent;

  setCaption( i18n("Preference") );
  QVBoxLayout *box = new QVBoxLayout( this );
  box->setMargin( 5 );
  box->setSpacing( 10 );

  QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
  tmpQGroupBox->setTitle(i18n("Table"));
  QVBoxLayout *lay1 = new QVBoxLayout(tmpQGroupBox);
  lay1->setMargin( 20 );
  lay1->setSpacing( 10 );

  m_pFormula= new QCheckBox(i18n("Show formula"),tmpQGroupBox);
  lay1->addWidget(m_pFormula);
  m_pFormula->setChecked(m_pView->activeTable()->getShowFormular());

  m_pGrid=new QCheckBox(i18n("Show Grid"),tmpQGroupBox);
  lay1->addWidget(m_pGrid);
  m_pGrid->setChecked(m_pView->activeTable()->getShowGrid());

  m_pColumn=new QCheckBox(i18n("Show column number"),tmpQGroupBox);
  lay1->addWidget(m_pColumn);
  m_pColumn->setChecked(m_pView->activeTable()->getShowColumnNumber());

  m_pLcMode=new QCheckBox(i18n("LC mode"),tmpQGroupBox);
  lay1->addWidget(m_pLcMode);
  m_pLcMode->setChecked(m_pView->activeTable()->getLcMode());

  m_pAutoCalc= new QCheckBox(i18n("Automatic Recalculation"),tmpQGroupBox);
  lay1->addWidget(m_pAutoCalc);
  m_pAutoCalc->setChecked(m_pView->activeTable()->getAutoCalc());

  m_pHideZero= new QCheckBox(i18n("Hide Zero"),tmpQGroupBox);
  lay1->addWidget(m_pHideZero);
  m_pHideZero->setChecked(m_pView->activeTable()->getHideZero());

  box->addWidget( tmpQGroupBox);

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  box->addWidget( bb);
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

}


void KSpreadpreference::slotOk()
{
  if(m_pView->activeTable()->getLcMode()==m_pLcMode->isChecked()
  && m_pView->activeTable()->getShowColumnNumber()==m_pColumn->isChecked()
  && m_pView->activeTable()->getShowFormular()==m_pFormula->isChecked()
  && m_pView->activeTable()->getAutoCalc()==m_pAutoCalc->isChecked()
  && m_pView->activeTable()->getShowGrid()==m_pGrid->isChecked()
  && m_pView->activeTable()->getHideZero()==m_pHideZero->isChecked())
  {
  //if there is any changes
  //so it's not necessary to refresh table
  //=> reject()
        reject();
  }
  else
  {
        m_pView->activeTable()->setLcMode(m_pLcMode->isChecked());
        m_pView->activeTable()->setShowColumnNumber(m_pColumn->isChecked());
        m_pView->activeTable()->setShowGrid(m_pGrid->isChecked());
        m_pView->activeTable()->setShowFormular(m_pFormula->isChecked());
        m_pView->activeTable()->setAutoCalc(m_pAutoCalc->isChecked());
        m_pView->activeTable()->setHideZero(m_pHideZero->isChecked());
        accept();
  }
}


void KSpreadpreference::slotClose()
{
reject();
}

#include "kspread_dlg_preference.moc"
