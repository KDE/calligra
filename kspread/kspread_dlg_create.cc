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
#include <kbuttonbox.h>

#include "kspread_dlg_create.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_table.h"
#include "kspread_util.h"

#include "kspread_editors.h"
#include "kspread_tabbar.h"

#include <kapp.h>
#include <klocale.h>

#include <qlayout.h>
#include <qapplication.h>

KSpreadcreate::KSpreadcreate( KSpreadView* parent, const QString& _name )
    : QDialog( parent, _name )
{
    m_pView = parent;
    setCaption( _name );

    f_param = 0;
    s_param = 0;
    t_param = 0;
    fo_param = 0;
    fi_param = 0;

    m_focus = 0;

    if ( !m_pView->canvasWidget()->editor() )
    {
	m_pView->canvasWidget()->createEditor( KSpreadCanvas::CellEditor );
	m_pView->canvasWidget()->editor()->setText( "=" );
    }

    ASSERT( m_pView->canvasWidget()->editor() );

    m_funcName = _name;
    first_element  = 0;

    // Save the name of the active table.
    m_tableName = m_pView->activeTable()->tableName();

    m_column = m_pView->canvasWidget()->markerColumn();
    m_row = m_pView->canvasWidget()->markerRow();

    m_oldText = m_pView->canvasWidget()->editor()->text();

    init();

    connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );

    connect( m_pView, SIGNAL( sig_chooseSelectionChanged( KSpreadTable*, const QRect& ) ),
	     this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );

    m_pView->canvasWidget()->startChoose();
    if(f_param!=0L)
        f_param->setFocus();

    qApp->installEventFilter( this );
}

KSpreadcreate::~KSpreadcreate()
{
}

bool KSpreadcreate::eventFilter( QObject* obj, QEvent* ev )
{
    if ( obj == f_param && ev->type() == QEvent::FocusIn )
	m_focus = f_param;
    else if ( obj == s_param && ev->type() == QEvent::FocusIn )
	m_focus = s_param;
    else if ( obj == t_param && ev->type() == QEvent::FocusIn )
	m_focus = t_param;
    else if ( obj == fo_param && ev->type() == QEvent::FocusIn )
	m_focus = fo_param;
    else if ( obj == fi_param && ev->type() == QEvent::FocusIn )
	m_focus = fi_param;
    else
	return FALSE;

    if ( m_focus )
	m_pView->canvasWidget()->startChoose();

    return FALSE;
}

void KSpreadcreate::init()
{
    QString exp_funct;

    QVBoxLayout *lay1 = new QVBoxLayout( this, 6, 6 );
    // lay1->setSpacing( 5 );

    QLabel *tmp_label;
    if(m_funcName=="cos" || m_funcName=="sin" || m_funcName=="tan" || m_funcName=="acos" || m_funcName=="asin" || m_funcName=="atan" ||
       m_funcName=="cosh" || m_funcName=="sinh" || m_funcName=="tanh" || m_funcName=="acosh" || m_funcName=="asinh" || m_funcName=="atanh" ||
       m_funcName=="degre" || m_funcName=="radian" )
    {
  	nb_param = 1;
  	QLabel *tmp_label = new QLabel( this);
  	if( m_funcName == "radian")
	    tmp_label->setText(i18n("Angle (deg)"));
  	else
	    tmp_label->setText(i18n("Angle (rd)"));

      	lay1->addWidget(tmp_label);
	f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
	exp_funct=m_funcName+"("+"Double"+")";
  	tmp_label->setText(exp_funct);
  	edit[0]=type_double;
    }
    else if( m_funcName == "PI" || m_funcName=="currentDate"
    	|| m_funcName=="currentTime" ||m_funcName=="currentDateTime")
    {
	nb_param = 0;
	exp_funct=m_funcName+"()";
	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(exp_funct);
    }
   else if( m_funcName=="right" || m_funcName=="left" || m_funcName=="REPT" )
   {
        nb_param=2;
        tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
        tmp_label->setText(i18n("Text"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
        if( m_funcName=="right"||m_funcName=="left")
		tmp_label->setText(i18n("Number of characters"));
        else if(m_funcName=="REPT")
        	tmp_label->setText(i18n("Number of repetitions"));
        s_param=new QLineEdit( this );
  	lay1->addWidget(s_param);
  	exp_funct=m_funcName+"("+"String,Double"+")";

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(exp_funct);
  	edit[0]=type_string;
  	edit[1]=type_double;

   }
   else if ( m_funcName=="sqrt" || m_funcName=="ln" || m_funcName=="log" || m_funcName=="exp" ||
	     m_funcName=="fabs" || m_funcName=="floor" || m_funcName=="ceil" || m_funcName=="ENT" )
   {
       nb_param=1;

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Double"));

       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       exp_funct=m_funcName+"("+"Double"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
   }
   else if (m_funcName=="ISLOGIC"||m_funcName=="ISTEXT"||m_funcName=="ISNUM")
   	{
   	nb_param=1;

   	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
   	tmp_label->setText(i18n("Value"));

        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);

  	exp_funct=m_funcName+"("+"Value"+")";
  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(exp_funct);
  	if(m_funcName=="ISTEXT")
  		edit[0]=type_string;
  	else
  		edit[0]=type_double;
   	}
   else if( m_funcName=="sum" || m_funcName=="max" || m_funcName=="min" ||
	    m_funcName=="multiply" || m_funcName=="average" || m_funcName=="variance" || m_funcName=="stddev")
   {
   	nb_param=5;

   	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
   	tmp_label->setText(i18n("Double"));
        f_param = new QLineEdit( this );
  	lay1->addWidget(f_param);

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(i18n("Double"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param);

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(i18n("Double"));
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param);

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(i18n("Double"));
        fo_param = new QLineEdit( this );
  	lay1->addWidget(fo_param);

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(i18n("Double"));
        fi_param = new QLineEdit( this );
  	lay1->addWidget(fi_param);

  	exp_funct=m_funcName+"("+"Double,Double,..."+")";
  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label,10,0);
  	tmp_label->setText(exp_funct);
  	edit[0]=type_double;
  	edit[1]=type_double;
  	edit[2]=type_double;
  	edit[3]=type_double;
  	edit[4]=type_double;
   }
   else if (m_funcName=="if")
   {
       nb_param=3;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Exp logic"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("if true"));
       s_param = new QLineEdit( this );
       lay1->addWidget(s_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("if false"));
       t_param = new QLineEdit( this );
       lay1->addWidget(t_param);

       exp_funct=m_funcName+"("+"Exp logic,if true,if false"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_logic;
       edit[1]=type_string;
       edit[2]=type_string;
   }
   else if (m_funcName=="date" || m_funcName=="dayOfYear")
   {
       nb_param=3;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Year (int)"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Month (int)"));
       s_param = new QLineEdit( this );
       lay1->addWidget(s_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Day (int)"));
       t_param = new QLineEdit( this );
       lay1->addWidget(t_param);

       exp_funct=m_funcName+"("+"Year,Month,Day"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
       edit[1]=type_double;
       edit[2]=type_double;
   }
   else if (m_funcName=="time")
   {
       nb_param=3;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Hour (int)"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Minute (int)"));
       s_param = new QLineEdit( this );
       lay1->addWidget(s_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Second (int)"));
       t_param = new QLineEdit( this );
       lay1->addWidget(t_param);

       exp_funct=m_funcName+"("+"Hour,Minute,Second"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
       edit[1]=type_double;
       edit[2]=type_double;
   }
   else if (m_funcName=="day")
   {
       nb_param=1;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Day (int)"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       exp_funct=m_funcName+"("+"Day"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
   }
   else if (m_funcName=="month")
   {
       nb_param=1;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Month (int)"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       exp_funct=m_funcName+"("+"Month"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
   }
   else if (m_funcName=="fact")
   {
       nb_param=1;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("int"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       exp_funct=m_funcName+"("+"int"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
   }
   else if (m_funcName=="PERMUT"||m_funcName=="COMBIN")
   {
       nb_param=2;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("int"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("int"));
       s_param = new QLineEdit( this );
       lay1->addWidget(s_param);

       exp_funct=m_funcName+"("+"int,int"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
       edit[1]=type_double;
   }
   else if (m_funcName=="not")
   {
       nb_param=1;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Exp Logic"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       exp_funct=m_funcName+"("+"Exp Logic"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_logic;
   }
   else if (m_funcName=="EXACT")
   {
       nb_param=2;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));

       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       s_param=new QLineEdit( this );
       lay1->addWidget(s_param);
       exp_funct=m_funcName+"("+"String,String"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_string;
       edit[1]=type_string;
   }
   else if (m_funcName=="len")
   {
       nb_param=1;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);
       exp_funct=m_funcName+"("+"String"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_string;
   }
   else if(m_funcName=="join")
   {
       nb_param=5;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       s_param = new QLineEdit( this );
       lay1->addWidget(s_param);
  	
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       t_param = new QLineEdit( this );
       lay1->addWidget(t_param);
  	
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       fo_param = new QLineEdit( this );
       lay1->addWidget(fo_param);
  	
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       fi_param = new QLineEdit( this );
       lay1->addWidget(fi_param);
  	
       exp_funct=m_funcName+"("+"String,String,..."+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label,10,0);
       tmp_label->setText(exp_funct);
       edit[0]=type_string;
       edit[1]=type_string;
       edit[2]=type_string;
       edit[3]=type_string;
       edit[4]=type_string;
   }
   else if(m_funcName=="STXT")
   {
       nb_param=3;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Text"));
       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);
  	
  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(i18n("Position of start"));
        s_param = new QLineEdit( this );
  	lay1->addWidget(s_param);

  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(i18n("Number of characters"));
	
	t_param = new QLineEdit( this );
  	lay1->addWidget(t_param);
  	exp_funct=m_funcName+"("+"String,Double,Double"+")";
  	tmp_label = new QLabel( this);
  	lay1->addWidget(tmp_label);
  	tmp_label->setText(exp_funct);
   	edit[0]=type_string;
   	edit[1]=type_double;
   	edit[2]=type_double;
   }
   else if (m_funcName=="pow")
   {
       nb_param=2;
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Double"));

       f_param = new QLineEdit( this );
       lay1->addWidget(f_param);

       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(i18n("Double"));
       s_param=new QLineEdit( this );
       lay1->addWidget(s_param);
       exp_funct=m_funcName+"("+"Double,Double"+")";
       tmp_label = new QLabel( this);
       lay1->addWidget(tmp_label);
       tmp_label->setText(exp_funct);
       edit[0]=type_double;
       edit[1]=type_double;
   }
   else
       ASSERT( 0 );

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();

    lay1->addWidget( bb );


}


void KSpreadcreate::slotOk()
{
    m_pView->canvasWidget()->endChoose();

    // It was no formula ? Make it one
    if(m_oldText.find("=",0)==-1)
	m_oldText="="+m_oldText;
    // Text was empty ?
    else if(m_oldText.isEmpty())
	m_oldText="=";

    // Switch back to the old table
    if(m_pView->activeTable()->tableName() !=  m_tableName )
	m_pView->changeTable( m_tableName );

    // Revert the marker to its original position
    m_pView->canvasWidget()->setMarkerColumn( m_column );
    m_pView->canvasWidget()->setMarkerRow( m_row );

    ASSERT( m_pView->canvasWidget()->editor() );

    QString formula = create_formula();
    int pos=m_pView->canvasWidget()->editor()->cursorPosition()+ formula.length();
    m_pView->canvasWidget()->editor()->setText( m_oldText + formula );
    // m_pView->canvasWidget()->editor()->setFocus();
    m_pView->canvasWidget()->editor()->setCursorPosition( pos );

    accept();
}

QString KSpreadcreate::create_formula()
{
    QString formula;
    formula=m_funcName+"(";
    if( nb_param>0)
    {
	if(!f_param->text().isEmpty())
	{
	    formula+=make_formula(f_param->text(),0);
	    first_element=1;
	}
    }
    if ( nb_param>1)
    {
	if(!s_param->text().isEmpty())
	{
	    if(first_element==1)
		formula+=","+make_formula(s_param->text(),1);
	    else
		formula+=make_formula(s_param->text(),1);

	    first_element=1;
	}

    }
    if  ( nb_param>2)
    {
	if(!t_param->text().isEmpty())
	{
	    if(first_element==1)
		formula+=","+make_formula(t_param->text(),2);
	    else
		formula+=make_formula(t_param->text(),2);
	    first_element=1;
	}
    }
    if( nb_param>3)
    {
	if(!fo_param->text().isEmpty())
	{
	    if(first_element==1)
		formula+=","+make_formula(fo_param->text(),3);
	    else
		formula+=make_formula(fo_param->text(),3);
	    first_element=1;
	}
    }
    if( nb_param>4)
    {
	if(!fi_param->text().isEmpty())
	{
	    if(first_element==1)
		formula+=","+make_formula(fi_param->text(),4);
	    else
		formula+=make_formula(fi_param->text(),4);
	    first_element=1;
	}
    }
    formula+=")";

    return formula;
}


QString KSpreadcreate::make_formula( const QString& _text,int nb_line)
{
    QString text=_text;
    if(edit[nb_line]==type_string)
    {
	if(text.left(1)=="\"")
	{
	    //find other "
	    int pos_car=text.find("\"",1);
	    // cout <<"Index : "<<pos_car<<endl;
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
	else if( text.toDouble()!=0||text.isEmpty()||text.find(":",0)!=-1)
	{
	}
	else if(text.find(",",0)!=-1)
	{
	    if(nb_line!=4)
		//When nb_line =4 =>function multi parameter
		// join(string,string,.....)
	    {
		text="\""+text+"\"";
	    }
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
		tmp=m_pView->activeTable()->tableName();
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


void KSpreadcreate::slotClose()
{
    m_pView->canvasWidget()->endChoose();

    // Switch back to the old table
    if(m_pView->activeTable()->tableName() !=  m_tableName )
	m_pView->changeTable( m_tableName );

    // Revert the marker to its original position
    m_pView->canvasWidget()->setMarkerColumn( m_column );
    m_pView->canvasWidget()->setMarkerRow( m_row );

    reject();
}

void KSpreadcreate::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{
    if ( !m_focus )
	return;

    if ( _selection.left() == 0 )
	return;

    if ( _selection.left() >= _selection.right() && _selection.top() >= _selection.bottom() )
    {
	// f_param->setText("Arsch");
	int dx = _selection.right();
	int dy = _selection.bottom();
	QString tmp;
	tmp.setNum( dy );
	tmp = _table->tableName() + "!" + util_columnLabel( dx ) + tmp;
	m_focus->setText( tmp );
    }
    else
    {
	QString area = util_rangeName( _table, _selection );
	m_focus->setText( area );
    }
}

#include "kspread_dlg_create.moc"
