/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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
#include "kspread_view.h"
#include "kspread_canvas.h"
#include <kapp.h>
#include <klocale.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <kbuttonbox.h>


KSpreaddlgformula::KSpreaddlgformula( KSpreadView* parent, const char* name )
	: QDialog( 0L, name )
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

  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  QObject::connect( type_formula, SIGNAL( highlighted(const QString &) ), this, SLOT( slotselected(const QString &) ) );
  QObject::connect( formula, SIGNAL( highlighted(const QString &) ), this, SLOT( slotselected_formula(const QString &) ) );

  resize( 350, 300 );
  
}




void KSpreaddlgformula::slotOk()
{

  QString math;
  math=formula->text(formula->currentItem());

  if ( m_pView->activeTable() != 0L )
    {
     	QString name_function;
     	QString string;
     	int pos;
     	string="";
     		
     	if(math=="variante" || math =="mult" || math =="average" || math =="sum" || math =="max" || math =="min")
	{
	    string=":";
	}
     	if(m_pView->editWidget()->isActivate() )
	{
	
	
	    name_function= math  + "(" + string + ")";
	    //last position of cursor + length of function +1 "("
	
	    pos=m_pView->editWidget()->cursorPosition()+ math.length()+1;
	    m_pView->editWidget()->setText( m_pView->editWidget()->text().insert(m_pView->editWidget()->cursorPosition(),name_function) );
	    m_pView->editWidget()->setFocus();
	    m_pView->editWidget()->setCursorPosition(pos);
	}
     	if(m_pView->canvasWidget()->pointeur() != 0)
	{
	    if(m_pView->canvasWidget()->EditorisActivate())
	    {
		name_function= math  + "(" + string + ")";
		pos=m_pView->canvasWidget()->posEditor()+ math.length()+1;
		m_pView->canvasWidget()->setEditor(m_pView->canvasWidget()->editEditor().insert(m_pView->canvasWidget()->posEditor(),name_function) );
	    	m_pView->canvasWidget()->focusEditor();
	    	m_pView->canvasWidget()->setPosEditor(pos);
	    }
	}
     	
    }
  accept();
}

void KSpreaddlgformula::slotClose()
{
if ( m_pView->activeTable() != 0L )
    {
    if(m_pView->editWidget()->isActivate() )
	{
	m_pView->editWidget()->setFocus();
	}
    if(m_pView->canvasWidget()->pointeur() != 0)
  	{
  	m_pView->canvasWidget()->focusEditor();
  	}
    }
  reject();
}

void KSpreaddlgformula::slotselected_formula(const QString & string)
{

}
void KSpreaddlgformula::slotselected(const QString & string)
{
QStringList list_stat;
list_stat+="average";
list_stat+="variante";

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
list_anal+="mult";

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


if(string== "Statistic" )
	{
	formula->clear();
	formula->insertStringList(list_stat);
	}
if (string =="Trigonometric")
	{
	formula->clear();
	formula->insertStringList(list_trig);
	}
if (string =="Analytic")
	{
	formula->clear();
	formula->insertStringList(list_anal);
	}
if(string == "All")
	{
	formula->clear();
	formula->insertStringList(list_stat);
	formula->insertStringList(list_trig);
	formula->insertStringList(list_anal);
	}		
}

#include "kspread_dlg_formula.moc"
