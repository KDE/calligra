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
#include "kspread_dlg_assistant2.h"
#include "kspread_editors.h"
#include "kspread_tabbar.h"
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
  tab_name=m_pView->activeTable()->name();

  dx = m_pView->canvasWidget()->markerColumn();
  dy = m_pView->canvasWidget()->markerRow();
  if(m_pView->editWidget()->isActivate())
  	{
  	editor=editWidget;
  	old_text=m_pView->editWidget()->text();
  	}
  else if(m_pView->canvasWidget()->pointeur() != 0)
	{
	if(m_pView->canvasWidget()->EditorisActivate())
		{
		editor=editCell;
		old_text=m_pView->canvasWidget()->editEditor();
		}
	}
  else
  	{
  	cout <<"Err any editor is activate\n";
  	}
  init();
  connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
  connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

}

void KSpreadcreate::init()
{


QString exp_funct;
  QHBoxLayout *lay3 = new QHBoxLayout( this );
  lay3->setMargin( 5 );
  lay3->setSpacing( 10 );

  QGridLayout *lay1 = new QGridLayout( lay3,12,2);
  lay1->setSpacing( 5 );
  if(name=="cos"||name=="sin"||name=="tan"||name=="acos"||name=="asin"||name=="atan"
  	||name=="cosh"||name=="sinh"||name=="tanh"||name=="acosh"||name=="asinh"||name=="atanh"
  	||name=="degre"||name=="radian")
  	{
  	nb_param=1;
  	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
  	if(name=="radian")
  		{
  		f_text->setText(i18n("Angle (deg)"));
  		}
  	else
  		{
  		f_text->setText(i18n("Angle (rd)"));
  		}
	f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,2,0);
        exp_funct=name+"("+"Double"+")";
  	exp_text->setText(exp_funct);
  	edit[0]=type_double;
        }
   else if(name=="true"||name=="false"||name=="PI")
   	{
   	nb_param=0;
        exp_funct=name+"()";
        exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,0,0);
  	exp_text->setText(exp_funct);
   	}
   else if(name=="RIGHT"||name=="LEFT")
        {
        nb_param=2;
        f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
        f_text->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	s_text = new QLabel( this);
  	lay1->addWidget(s_text,2,0);
        s_text->setText(i18n("Number of characters"));

        s_param=new QLineEdit( this );
  	lay1->addWidget(s_param,3,0);
  	exp_funct=name+"("+"String,Double"+")";
  	
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,4,0);
  	exp_text->setText(exp_funct);
  	edit[0]=type_string;
  	edit[1]=type_double;

        }
   else if (name=="sqrt"||name=="ln"||name=="log"||name=="exp"
   	||name=="fabs"||name=="floor"||name=="ceil"||name=="ENT")
   	{
   	nb_param=1;
   	
   	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
   	f_text->setText(i18n("Double"));

        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	exp_funct=name+"("+"Double"+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,2,0);
  	exp_text->setText(exp_funct);
  	edit[0]=type_double;
   	}
   else if(name=="sum"||name=="max"||name=="min"
   	||name=="mult"||name=="average"||name=="variante"||name=="ecartype")
   	{
   	nb_param=5;
   	
   	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
   	f_text->setText(i18n("Double"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	s_text = new QLabel( this);
  	lay1->addWidget(s_text,2,0);
  	s_text->setText(i18n("Double"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param,3,0);
  	
  	t_text = new QLabel( this);
  	lay1->addWidget(t_text,4,0);
  	t_text->setText(i18n("Double"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param,5,0);
  	
  	fo_text = new QLabel( this);
  	lay1->addWidget(fo_text,6,0);
  	fo_text->setText(i18n("Double"));
        fo_param = new QLineEdit( this );
  	lay1->addWidget(fo_param,7,0);
  	
  	fi_text = new QLabel( this);
  	lay1->addWidget(fi_text,8,0);
  	fi_text->setText(i18n("Double"));
        fi_param = new QLineEdit( this );
  	lay1->addWidget(fi_param,9,0);
  	
  	exp_funct=name+"("+"Double,Double,..."+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,10,0);
  	exp_text->setText(exp_funct);
  	edit[0]=type_double;
  	edit[1]=type_double;
  	edit[2]=type_double;
  	edit[3]=type_double;
  	edit[4]=type_double;
   	}
   else if (name=="IF")
   	{
   	nb_param=3;
   	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
   	f_text->setText(i18n("Exp logic"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	s_text = new QLabel( this);
  	lay1->addWidget(s_text,2,0);
  	s_text->setText(i18n("if true"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param,3,0);
  	
  	t_text = new QLabel( this);
  	lay1->addWidget(t_text,4,0);
  	t_text->setText(i18n("if false"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param,5,0);
  	
  	exp_funct=name+"("+"Exp logic,if true,if false"+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,6,0);
  	exp_text->setText(exp_funct);
  	edit[0]=type_logic;
  	edit[1]=type_string;
  	edit[2]=type_string;
   	}
   else if (name=="NO")
   	{
   	 nb_param=1;
   	 f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
   	f_text->setText(i18n("Exp Logic"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	exp_funct=name+"("+"Exp Logic"+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,2,0);
  	exp_text->setText(exp_funct);
   	edit[0]=type_logic;
   	}
   else if (name=="EXACT")
   	{
   	nb_param=2;
   	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
        f_text->setText(i18n("Text"));

        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	s_text = new QLabel( this);
  	lay1->addWidget(s_text,2,0);
	s_text->setText(i18n("Text"));
        s_param=new QLineEdit( this );
  	lay1->addWidget(s_param,3,0);
  	exp_funct=name+"("+"String,String"+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,4,0);
  	exp_text->setText(exp_funct);
   	edit[0]=type_string;
   	edit[1]=type_string;
   	}
   else if (name=="NBCAR")
   	{
   	nb_param=1;
   	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
   	f_text->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	exp_funct=name+"("+"String"+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,2,0);
  	exp_text->setText(exp_funct);
   	edit[0]=type_string;
   	}
   else if(name=="conc")
   	{
   	nb_param=5;
   	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
   	f_text->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	s_text = new QLabel( this);
  	lay1->addWidget(s_text,2,0);
  	s_text->setText(i18n("Text"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param,3,0);
  	
  	t_text = new QLabel( this);
  	lay1->addWidget(t_text,4,0);
  	t_text->setText(i18n("Text"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param,5,0);
  	
  	fo_text = new QLabel( this);
  	lay1->addWidget(fo_text,6,0);
  	fo_text->setText(i18n("Text"));
        fo_param = new QLineEdit( this );
  	lay1->addWidget(fo_param,7,0);
  	
  	fi_text = new QLabel( this);
  	lay1->addWidget(fi_text,8,0);
  	fi_text->setText(i18n("Text"));
        fi_param = new QLineEdit( this );
  	lay1->addWidget(fi_param,9,0);
  	
  	exp_funct=name+"("+"String,String,..."+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,10,0);
  	exp_text->setText(exp_funct);
  	edit[0]=type_string;
  	edit[1]=type_string;
  	edit[2]=type_string;
  	edit[3]=type_string;
  	edit[4]=type_string;
   	}
   else if(name=="STXT")
   	{
   	nb_param=3;
   	f_text = new QLabel( this);
  	lay1->addWidget(f_text,0,0);
   	f_text->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param,1,0);
  	
  	s_text = new QLabel( this);
  	lay1->addWidget(s_text,2,0);
  	s_text->setText(i18n("Position of start"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param,3,0);
  	
  	t_text = new QLabel( this);
  	lay1->addWidget(t_text,4,0);
  	t_text->setText(i18n("Number of characters"));
	
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param,5,0);
  	exp_funct=name+"("+"String,Double,Double"+")";
  	exp_text = new QLabel( this);
  	lay1->addWidget(exp_text,6,0);
  	exp_text->setText(exp_funct);
   	edit[0]=type_string;
   	edit[1]=type_double;
   	edit[2]=type_double;
   	}
   else
	{
   	cout <<"erreur\n";
   	}
  m_pOk = new QPushButton( i18n("OK"), this );
  lay1->addWidget(m_pOk,11,0);
  m_pClose = new QPushButton( i18n("Close"), this );
  lay1->addWidget(m_pClose,11,1);
  if( nb_param>0)
  	 {
  	 f_select = new QPushButton( i18n("Select"), this );
         lay1->addWidget(f_select,1,1);
  	 connect( f_select, SIGNAL( clicked() ), this, SLOT( slotFselect() ) );
  	 }
if ( nb_param>1)
	{
	s_select = new QPushButton( i18n("Select"), this );
         lay1->addWidget(s_select,3,1);
	connect( s_select, SIGNAL( clicked() ), this, SLOT( slotSselect() ) );
	}
if  ( nb_param>2)
	{
	t_select = new QPushButton( i18n("Select"), this );
         lay1->addWidget(t_select,5,1);
	connect( t_select, SIGNAL( clicked() ), this, SLOT( slotTselect() ) );
	}
if( nb_param>3)
	{
	fo_select = new QPushButton( i18n("Select"), this );
         lay1->addWidget(fo_select,7,1);
 	connect( fo_select, SIGNAL( clicked() ), this, SLOT( slotFOselect() ) );
	}
if( nb_param>4)
	{
	fi_select = new QPushButton( i18n("Select"), this );
         lay1->addWidget(fi_select,9,1);
         connect( fi_select, SIGNAL( clicked() ), this, SLOT( slotFIselect() ) );
	}

}


void KSpreadcreate::slotOk()
{
QString formula=create_formula();
//cout <<"Formula : "<<formula.ascii()<<endl;
int pos;
if(m_pView->editWidget()->isActivate() )
	{
	pos=m_pView->editWidget()->cursorPosition()+ formula.length();
	m_pView->editWidget()->setText( m_pView->editWidget()->text().insert(m_pView->editWidget()->cursorPosition(),formula) );
	m_pView->editWidget()->setFocus();
	m_pView->editWidget()->setCursorPosition(pos);
	}
 if(m_pView->canvasWidget()->pointeur() != 0)
	{
	if(m_pView->canvasWidget()->EditorisActivate())
	    {
		pos=m_pView->canvasWidget()->posEditor()+ formula.length();
		m_pView->canvasWidget()->setEditor(m_pView->canvasWidget()->editEditor().insert(m_pView->canvasWidget()->posEditor(),formula) );
	    	m_pView->canvasWidget()->focusEditor();
	    	m_pView->canvasWidget()->setPosEditor(pos);
	    }
	}

	
if(m_pView->activeTable()->name() !=  t_name())
	{
	m_pView->tabBar()->setActiveTab( t_name());
	
	m_pView->changeTable(  t_name() );
	}
		

m_pView->activeTable()->unselect();
//table->setText( dy, dx , m_pRef->text() );
m_pView->canvasWidget()->hideMarker();
m_pView->canvasWidget()->setMarkerColumn(dx);
m_pView->canvasWidget()->setMarkerRow(dy);
m_pView->canvasWidget()->showMarker();

/*if(m_pView->canvasWidget()->pointeur() == 0&&editor==editCell)
	{
	cout <<"New cell\n";
	KSpreadCell  *cell = m_pView->activeTable()->cellAt( dx,dy );
	KSpreadCellEditor &m_editor= m_pView->canvasWidget()->pointeur();
	*//*m_pView->canvasWidget()->pointeur()*/
	/*cout <<"Inter\n";
	m_editor= new KSpreadTextEditor(cell,m_pView->canvasWidget());
	int w = cell->width( m_pView->canvasWidget()->markerColumn(), m_pView->canvasWidget() );
	int h = cell->height( m_pView->canvasWidget()->markerRow(), m_pView->canvasWidget() );
	int xpos = m_pView->activeTable()->columnPos( m_pView->canvasWidget()->markerColumn(), m_pView->canvasWidget() );
	int ypos = m_pView->activeTable()->rowPos( m_pView->canvasWidget()->markerRow(),m_pView->canvasWidget() );
	
	QPalette p = m_editor->palette();
	
	QColorGroup g( p.normal() );
	g.setColor( QColorGroup::Text, cell->textPen().color() );
	g.setColor( QColorGroup::Background, cell->KSpreadLayout::bgColor() );
	m_editor->setPalette( QPalette( g, p.disabled(), g ) );
	m_editor->setFont( cell->textFont() );
	
	m_editor->setGeometry( xpos, ypos, w, h );
	m_editor->setMinimumSize( QSize( w, h ) );
	m_editor->show();
	m_editor->setFocus();
	
        }
 else
 	{*/
	m_pView->editWidget()->setFocus();
	m_pView->editWidget()->setText(old_text+formula);	
        //}
accept();
}

QString KSpreadcreate::create_formula()
{
QString formula;
formula=name+"(";
if( nb_param>0)
	formula+=+make_formula(f_param->text(),0);
if ( nb_param>1)
	{
	if(!s_param->text().isEmpty())
		formula+=","+make_formula(s_param->text(),1);
	}
if  ( nb_param>2)
	{
	if(!t_param->text().isEmpty())
		formula+=","+make_formula(t_param->text(),2);
	}
if( nb_param>3)
	{
	if(!fo_param->text().isEmpty())
		formula+=","+make_formula(fo_param->text(),3);
	}
if( nb_param>4)
	{
	if(!fi_param->text().isEmpty())
		formula+=","+make_formula(fi_param->text(),4);
	}
formula+=")";

return(formula);
}


QString KSpreadcreate::make_formula(QString _text,int nb_line)
{
QString text=_text;
if(edit[nb_line]==type_string)
	{
	if(text.left(1)=="\"")
		{
		
		//find other "
		int pos_car=text.find("\"",1);
		cout <<"Index : "<<pos_car<<endl;
		if(pos_car==-1)
			{
			//so don't find "
			text+="\"";
			}
		else if(pos_car < text.length())
			{
			//todo
			}
		else
			{
			}
		}
	else if( text.toDouble()!=0)
		{
		}
	else if(text.isEmpty())
		{
		}
	else if(text.find(",",0)!=-1)
		{
		if(nb_line!=4)
			//When nb_line =4 =>function multi parameter
			// conc(string,string,.....)
			{
			text="\""+text+"\"";
			}
		}
	else if(text.find(":",0)!=-1)
		{
		//range
		cout <<"Range\n";
		}
	else
		{
		QString tmp;
		int p = text.find( "!" );
  		if ( p != -1 )
  			{
  			tmp = text;
  			}
  		else
  			{
  			tmp=m_pView->activeTable()->name();
  			tmp+= "!" +text;
  			}
		
		KSpreadPoint _cell=KSpreadPoint( tmp, m_pView->doc()->map() );
		if(!_cell.isValid())
			{
			text="\""+text+"\"";
			}
		}
		
	}
return text;
}


void KSpreadcreate::slotFselect()
{
set_nbbutton(1);
KSpreadassistant2 *dlg=new KSpreadassistant2(this,"Assistant");
dlg->show();
}

void KSpreadcreate::slotSselect()
{
set_nbbutton(2);
KSpreadassistant2 *dlg=new KSpreadassistant2(this,"Assistant");
dlg->show();
}

void KSpreadcreate::slotTselect()
{
set_nbbutton(3);
KSpreadassistant2 *dlg=new KSpreadassistant2(this,"Assistant");
dlg->show();
}

void KSpreadcreate::slotFOselect()
{
set_nbbutton(4);
KSpreadassistant2 *dlg=new KSpreadassistant2(this,"Assistant");
dlg->show();
}

void KSpreadcreate::slotFIselect()
{
set_nbbutton(5);
KSpreadassistant2 *dlg=new KSpreadassistant2(this,"Assistant");
dlg->show();
}


void KSpreadcreate::setText(QString text)
{
switch(nb_button())
	{
	case 1:
		f_param->setText(text);
		f_param->setFocus();
		break;
	case 2:
		s_param->setText(text);
		s_param->setFocus();
		break;
	case 3:
		t_param->setText(text);
		t_param->setFocus();
		break;
	case 4:
		fo_param->setText(text);
		fo_param->setFocus();
		break;
	case 5:
		fi_param->setText(text);
		fi_param->setFocus();
		break;
	default:
		cout <<"Err in setText()\n";
		break;
	}
}
void KSpreadcreate::slotClose()
{

reject();
}

#include "kspread_dlg_create.moc"
