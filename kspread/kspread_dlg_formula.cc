/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999,2000 Montel Laurent <montell@club-internet.fr>

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



#include "kspread_dlg_formula.h"
#include "kspread_dlg_create.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include <kapp.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>



KSpreaddlgformula::KSpreaddlgformula( KSpreadView* parent, const char* name )
	: QDialog( parent, name )
{
  m_pView = parent;


  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  QHBoxLayout *lay2 = new QHBoxLayout( lay1 );
  lay2->setSpacing( 5 );

  type_formula=new QListBox(this);
  lay2->addWidget( type_formula );
  formula=new QListBox(this);
  lay2->addWidget( formula );

  setCaption( i18n("Formula") );

  exp=new QLabel(this);
  exp->layout();
  lay1->addWidget(exp);

  KButtonBox *bb = new KButtonBox( this );
  bb->addStretch();
  m_pOk = bb->addButton( i18n("OK") );
  m_pOk->setDefault( TRUE );
  m_pClose = bb->addButton( i18n( "Close" ) );
  bb->layout();
  lay1->addWidget( bb );

  type_formula->insertItem(i18n("All"));
  type_formula->insertItem(i18n("Statistic"));
  type_formula->insertItem(i18n("Trigonometric"));
  type_formula->insertItem(i18n("Analytic"));
  type_formula->insertItem(i18n("Logic"));
  type_formula->insertItem(i18n("Text"));
  type_formula->insertItem(i18n("Time and Date"));
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  QObject::connect( type_formula, SIGNAL( highlighted(const QString &) ), this, SLOT( slotselected(const QString &) ) );
  QObject::connect( formula, SIGNAL( doubleClicked(QListBoxItem *)),this ,SLOT( slotOk() ) );
  resize( 350, 300 );

}




void KSpreaddlgformula::slotOk()
{

  QString math;
  math=formula->text(formula->currentItem());

  if ( m_pView->activeTable() != 0L )
    {
    KSpreadcreate* dlg = new KSpreadcreate( m_pView, math );
    dlg->show();
    }

  accept();
}

void KSpreaddlgformula::slotClose()
{
    reject();
}



void KSpreaddlgformula::slotselected(const QString & string)
{
QStringList list_stat;
list_stat+="average";
list_stat+="variance";
list_stat+="stddev";

QStringList list_anal;
list_anal+="sum";
list_anal+="sqrt";
list_anal+="ln";
list_anal+="log";
list_anal+="exp";
list_anal+="fabs";
list_anal+="floor";
list_anal+="ceil";
list_anal+="max";
list_anal+="min";
list_anal+="multiply";
list_anal+="ENT";
list_anal+="PI";
list_anal+="pow";
list_anal+="MOD";


QStringList list_trig;
list_trig+="cos";
list_trig+="sin";
list_trig+="tan";
list_trig+="acos";
list_trig+="asin";
list_trig+="atan";
list_trig+="cosh";
list_trig+="sinh";
list_trig+="tanh";
list_trig+="acosh";
list_trig+="asinh";
list_trig+="atanh";
list_trig+="degree";
list_trig+="radian";
list_trig+="PERMUT";
list_trig+="COMBIN";
list_trig+="fact";
list_trig+="BINO";
list_trig+="INVBINO";

QStringList list_logic;
list_logic+="if";
list_logic+="not";
list_logic+="AND";
list_logic+="OR";
list_logic+="NAND";
list_logic+="NOR";

QStringList list_text;
list_text+="join";
list_text+="right";
list_text+="left";
list_text+="len";
list_text+="EXACT";
list_text+="STXT";
list_text+="REPT";
list_text+="lower";
list_text+="upper";

QStringList list_date_time;
list_date_time+="date";
list_date_time+="day";
list_date_time+="month";
list_date_time+="time";
list_date_time+="currentDate";
list_date_time+="currentTime";
list_date_time+="currentDateTime";
list_date_time+="dayOfYear";

if(string== i18n("Statistic") )
	{
	formula->clear();
	formula->insertStringList(list_stat);
	}
if (string ==i18n("Trigonometric"))
	{
	formula->clear();
	formula->insertStringList(list_trig);
	}
if (string ==i18n("Analytic"))
	{
	formula->clear();
	formula->insertStringList(list_anal);
	}
if(string== i18n("Logic") )
	{
	formula->clear();
	formula->insertStringList(list_logic);
	}
if(string== i18n("Text") )
	{
	formula->clear();
	formula->insertStringList(list_text);
	}
if(string== i18n("Time and Date") )
	{
	formula->clear();
	formula->insertStringList(list_date_time);
	}
if(string == i18n("All"))
	{
	formula->clear();
	formula->insertStringList(list_stat);
	formula->insertStringList(list_trig);
	formula->insertStringList(list_anal);
	formula->insertStringList(list_text);
	formula->insertStringList(list_logic);
	formula->insertStringList(list_date_time);
	}
}

#include "kspread_dlg_formula.moc"
