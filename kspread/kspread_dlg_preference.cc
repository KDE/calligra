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
#include "kspread_doc.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kdialogbase.h>
#include <qvbox.h>
#include <qlabel.h>


KSpreadpreference::KSpreadpreference( KSpreadView* parent, const char* /*name*/)
	: KDialogBase(KDialogBase::IconList,
                                    i18n("Configure Kspread") ,
                                    KDialogBase::Ok | KDialogBase::Cancel,
                                    KDialogBase::Ok)

{
  QVBox *page=addVBoxPage(i18n("Preference"), QString::null);

  _preferenceConfig = new  preference(parent,page );
  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));

  page=addVBoxPage(i18n("Local Parameters"), QString::null);
  parameterLocale *_ParamLocal = new  parameterLocale(parent,page );
}

void KSpreadpreference::slotApply()
{
_preferenceConfig->apply();
}

 preference::preference( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name)
 {

  m_pView = _view;
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

  m_pFirstLetterUpper= new QCheckBox(i18n("Convert first letter to upper case"),tmpQGroupBox);
  lay1->addWidget(m_pFirstLetterUpper);
  m_pFirstLetterUpper->setChecked(m_pView->activeTable()->getFirstLetterUpper());

  box->addWidget( tmpQGroupBox);
}


void preference::apply()
{
  if(m_pView->activeTable()->getLcMode()==m_pLcMode->isChecked()
  && m_pView->activeTable()->getShowColumnNumber()==m_pColumn->isChecked()
  && m_pView->activeTable()->getShowFormular()==m_pFormula->isChecked()
  && m_pView->activeTable()->getAutoCalc()==m_pAutoCalc->isChecked()
  && m_pView->activeTable()->getShowGrid()==m_pGrid->isChecked()
  && m_pView->activeTable()->getHideZero()==m_pHideZero->isChecked()
  && m_pView->activeTable()->getFirstLetterUpper()==m_pFirstLetterUpper->isChecked())
  {
  //nothing
  }
  else
  {
        m_pView->activeTable()->setLcMode(m_pLcMode->isChecked());
        m_pView->activeTable()->setShowColumnNumber(m_pColumn->isChecked());
        m_pView->activeTable()->setShowGrid(m_pGrid->isChecked());
        m_pView->activeTable()->setShowFormular(m_pFormula->isChecked());
        m_pView->activeTable()->setAutoCalc(m_pAutoCalc->isChecked());
        m_pView->activeTable()->setHideZero(m_pHideZero->isChecked());
        m_pView->activeTable()->setFirstLetterUpper(m_pFirstLetterUpper->isChecked());
  }
}

parameterLocale::parameterLocale( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name)
{
  QVBoxLayout *box = new QVBoxLayout( this );
  box->setMargin( 5 );
  box->setSpacing( 10 );

  QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
  tmpQGroupBox->setTitle(i18n("Parameters"));
  QVBoxLayout *lay1 = new QVBoxLayout(tmpQGroupBox);
  lay1->setMargin( 20 );
  lay1->setSpacing( 10 );
  QLabel *label=new QLabel( tmpQGroupBox,"label");
  label->setText( i18n("Number : %1").arg( _view->doc()->locale()->formatNumber(12.55) ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label1");
  label->setText( i18n("Date : %1").arg( _view->doc()->locale()->formatDate(QDate(2000,10,23)) ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label2");
  label->setText( i18n("Time : %1").arg( _view->doc()->locale()->formatTime(QTime(15,10,53)) ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label3");
  label->setText( i18n("Money : %1").arg( _view->doc()->locale()->formatMoney(12.55) ));
  lay1->addWidget(label);
  box->addWidget( tmpQGroupBox);


}
#include "kspread_dlg_preference.moc"
