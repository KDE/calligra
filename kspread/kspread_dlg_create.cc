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

#include <qprinter.h>

#include "kspread_dlg_create.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include "kspread_util.h"
#include <kapp.h>
#include <klocale.h>
#include <qlayout.h>


KSpreadcreate::KSpreadcreate( KSpreadView* parent, const QString _name,QString _formula)
	: QDialog( 0L, _name )
{
  m_pView = parent;
  setCaption( _name.ascii() );
  name=_name;
  old_formula=_formula;
  m_pView->canvasWidget()->setEditorActivate(false);
  init();
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
  connect( m_pView, SIGNAL( sig_selectionChanged( KSpreadTable*, const QRect& ) ),
	   this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );
}

void KSpreadcreate::init()
{


QString exp_funct;

  QVBoxLayout *lay1 = new QVBoxLayout( this );
  lay1->setMargin( 5 );
  lay1->setSpacing( 10 );
  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this);

  lay1->addWidget(tmpQLabel);
  if(name=="cos"||name=="sin"||name=="tan"||name=="acos"||name=="asin"||name=="atan"
  	||name=="cosh"||name=="sinh"||name=="tanh"||name=="acosh"||name=="asinh"||name=="atanh"
  	||name=="degre"||name=="radian")
  	{
  	nb_param=1;
  	if(name=="radian")
  		{
  		tmpQLabel->setText(i18n("Angle (deg)"));
  		}
  	else
  		{
  		tmpQLabel->setText(i18n("Angle (rd)"));
  		}
	f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
        exp_funct=name+"("+"Double"+")";
  	tmpQLabel->setText(exp_funct);
  	
        }
   else if(name=="true"||name=="false"||name=="PI")
   	{
   	nb_param=0;
        exp_funct=name+"()";
  	tmpQLabel->setText(exp_funct);
   	}
   else if(name=="RIGHT"||name=="LEFT")
        {
        nb_param=2;
        tmpQLabel->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
        tmpQLabel->setText(i18n("Number of characters"));

        s_param=new QLineEdit( this );
  	lay1->addWidget(s_param);
  	exp_funct=name+"("+"String,Double"+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
        }
   else if (name=="sqrt"||name=="ln"||name=="log"||name=="exp"
   	||name=="fabs"||name=="floor"||name=="ceil"||name=="ENT")
   	{
   	nb_param=1;
   	tmpQLabel->setText(i18n("Double"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	exp_funct=name+"("+"Double"+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else if(name=="sum"||name=="max"||name=="min"
   	||name=="mult"||name=="average"||name=="variante"||name=="ecartype")
   	{
   	nb_param=5;
   	tmpQLabel->setText(i18n("Double"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Double"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Double"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Double"));
        fo_param = new QLineEdit( this );
  	lay1->addWidget(fo_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Double"));
        fi_param = new QLineEdit( this );
  	lay1->addWidget(fi_param);
  	
  	exp_funct=name+"("+"Double,Double,..."+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else if (name=="IF")
   	{
   	nb_param=3;
   	tmpQLabel->setText(i18n("Exp logic"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("if true"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("if false"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param);
  	
  	exp_funct=name+"("+"Exp logic,if true,if false"+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else if (name=="NO")
   	{
   	 nb_param=1;
   	tmpQLabel->setText(i18n("Exp Logic"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	
  	exp_funct=name+"("+"Exp Logic"+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else if (name=="EXACT")
   	{
   	nb_param=2;
        tmpQLabel->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
	tmpQLabel->setText(i18n("Text"));
        s_param=new QLineEdit( this );
  	lay1->addWidget(s_param);
  	exp_funct=name+"("+"String,String"+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else if (name=="NBCAR")
   	{
   	nb_param=1;
   	tmpQLabel->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	exp_funct=name+"("+"String"+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else if(name=="conc")
   	{
   	nb_param=5;
   	tmpQLabel->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Text"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Text"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Text"));
        fo_param = new QLineEdit( this );
  	lay1->addWidget(fo_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Text"));
        fi_param = new QLineEdit( this );
  	lay1->addWidget(fi_param);
  	
  	exp_funct=name+"("+"String,String,..."+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else if(name=="STXT")
   	{
   	nb_param=3;
   	tmpQLabel->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Position of start"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param);
  	
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(i18n("Number of characters"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param);
  	exp_funct=name+"("+"String,Double,Double"+")";
  	tmpQLabel = new QLabel( this);
  	lay1->addWidget(tmpQLabel);
  	tmpQLabel->setText(exp_funct);
   	}
   else
	{
   	cout <<"erreur\n";
   	}
  QHBoxLayout *lay2 = new QHBoxLayout( lay1);
  lay2->setSpacing( 5 );

  m_pOk = new QPushButton( i18n("OK"), this );
  lay2->addWidget(m_pOk);
  m_pClose = new QPushButton( i18n("Close"), this );
  lay2->addWidget(m_pClose);

}


void KSpreadcreate::slotOk()
{
QString formula=create_formula();
cout <<"Formula : "<<formula.ascii()<<endl;
accept();
}

QString KSpreadcreate::create_formula()
{
QString formula;
formula=name+"(";
if( nb_param>0)
	formula+=+f_param->text();
if ( nb_param>1)
	{
	if(!s_param->text().isEmpty())
		formula+=","+s_param->text();
	}
if  ( nb_param>2)
	{
	if(!t_param->text().isEmpty())
		formula+=","+t_param->text();
	}
if( nb_param>3)
	{
	if(!fo_param->text().isEmpty())
		formula+=","+fo_param->text();
	}
if( nb_param>4)
	{
	if(!fi_param->text().isEmpty())
		formula+=","+fi_param->text();
	}
formula+=")";

return(formula);
}
void KSpreadcreate::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{

QString area = util_rangeName( _table, _selection );
if(f_param->hasFocus())
	{
	cout <<"toto\n";
	}
else
	{
	cout <<"erreur\n";
	}
cout <<"Area : " <<area.ascii()<<endl;
}
void KSpreadcreate::mousePressEvent( QMouseEvent *_ev )
{
if(f_param->hasFocus())
	{
	cout <<"focus\n";
	}
}
void KSpreadcreate::slotClose()
{
reject();
}

#include "kspread_dlg_create.moc"
