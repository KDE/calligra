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

#include "kspread_dlg_formula.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_util.h"
#include "kspread_layout.h"
#include "kspread_table.h"
#include "kspread_editors.h"
#include "kspread_doc.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <iostream.h>
#include <knumvalidator.h>

KSpreadDlgFormula::KSpreadDlgFormula( KSpreadView* parent, const char* name,const QString& formulaName)
    : QDialog( parent, name )
{

    m_pView = parent;
    setCaption( name );

    if ( !m_pView->canvasWidget()->editor() )
    {
        m_pView->canvasWidget()->createEditor( KSpreadCanvas::CellEditor );
	m_pView->canvasWidget()->editor()->setText( "=" );
    }

    ASSERT( m_pView->canvasWidget()->editor() );
    m_focus = 0;

    QGridLayout *grid1 = new QGridLayout(this,11,2,15,7);
    typeFunction=new QComboBox(this);
    grid1->addWidget(typeFunction,0,0);
    typeFunction->insertItem(i18n("All"));
    typeFunction->insertItem(i18n("Statistic"));
    typeFunction->insertItem(i18n("Trigonometric"));
    typeFunction->insertItem(i18n("Analytic"));
    typeFunction->insertItem(i18n("Logic"));
    typeFunction->insertItem(i18n("Text"));
    typeFunction->insertItem(i18n("Time and Date"));
    typeFunction->insertItem(i18n("Financial"));

    functions=new QListBox(this);
    grid1->addMultiCellWidget( functions,1,7,0,0 );

    selectFunction = new QPushButton(QString::null, this);
    selectFunction->setPixmap(BarIcon("down", KIcon::SizeSmall));
    grid1->addWidget(selectFunction,8,0);

    result= new QLineEdit(this);
    grid1->addMultiCellWidget(result,9,9,0,1);


    QGridLayout *grid2 = new QGridLayout(this,11,2,15,7);
    grid1->addMultiCell(grid2,0,8,1,1);

    grid2->setResizeMode (QLayout::Minimum);
    label1=new QLabel(this);
    grid2->addWidget(label1,0,1);

    firstElement=new QLineEdit(this);
    grid2->addWidget(firstElement,1,1);

    label2=new QLabel(this);
    grid2->addWidget(label2,2,1);

    secondElement=new QLineEdit(this);
    grid2->addWidget(secondElement,3,1);

    label3=new QLabel(this);
    grid2->addWidget(label3,4,1);

    thirdElement=new QLineEdit(this);
    grid2->addWidget(thirdElement,5,1);

    label4=new QLabel(this);
    grid2->addWidget(label4,6,1);

    fourElement=new QLineEdit(this);
    grid2->addWidget(fourElement,7,1);

    label5=new QLabel(this);
    grid2->addWidget(label5,8,1);

    fiveElement=new QLineEdit(this);
    grid2->addWidget(fiveElement,9,1);

    help=new QLabel(this);
    grid2->addWidget(help,10,1);

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();
    grid1->addWidget(bb,10,1);


    refresh_result=true;
    connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
    connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( typeFunction, SIGNAL( activated(const QString &) ),
             this, SLOT( slotActivated(const QString &) ) );
    connect( functions, SIGNAL( highlighted(const QString &) ),
             this, SLOT( slotselected(const QString &) ) );
    connect( functions, SIGNAL( doubleClicked(QListBoxItem *)),
             this ,SLOT( slotDoubleClicked(QListBoxItem *) ) );
    slotActivated(i18n("All"));
    connect(selectFunction, SIGNAL(clicked()),
          this,SLOT(slotSelectButton()));
    connect(firstElement,SIGNAL(textChanged ( const QString & )),
            this,SLOT(slotChangeText(const QString &)));
    connect(secondElement,SIGNAL(textChanged ( const QString & )),
            this,SLOT(slotChangeText(const QString &)));
    connect(thirdElement,SIGNAL(textChanged ( const QString & )),
            this,SLOT(slotChangeText(const QString &)));
    connect(fourElement,SIGNAL(textChanged ( const QString & )),
            this,SLOT(slotChangeText(const QString &)));
    connect(fiveElement,SIGNAL(textChanged ( const QString & )),
            this,SLOT(slotChangeText(const QString &)));
    connect( m_pView, SIGNAL( sig_chooseSelectionChanged( KSpreadTable*, const QRect& ) ),
	     this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );

    firstElement->hide();
    secondElement->hide();
    thirdElement->hide();
    fourElement->hide();
    fiveElement->hide();
    m_oldLength=0;
    // Save the name of the active table.
    m_tableName = m_pView->activeTable()->tableName();
    m_oldText = m_pView->canvasWidget()->editor()->text();
    m_column = m_pView->canvasWidget()->markerColumn();
    m_row = m_pView->canvasWidget()->markerRow();


    if(m_oldText.isEmpty())
        result->setText("=");
    else
        result->setText(m_oldText);

    m_pView->canvasWidget()->startChoose();

    qApp->installEventFilter( this );
    if(!formulaName.isEmpty())
        {
        functions->setCurrentItem(functions->index(functions->findItem(formulaName)));
        slotDoubleClicked(functions->findItem(formulaName));

        }
    if(functions->currentItem()==-1)
        selectFunction->setEnabled(false);
}

bool KSpreadDlgFormula::eventFilter( QObject* obj, QEvent* ev )
{
    if ( obj == firstElement && ev->type() == QEvent::FocusIn )
	m_focus = firstElement;
    else if ( obj == secondElement && ev->type() == QEvent::FocusIn )
	m_focus = secondElement;
    else if ( obj == thirdElement && ev->type() == QEvent::FocusIn )
	m_focus = thirdElement;
    else if ( obj == fourElement && ev->type() == QEvent::FocusIn )
	m_focus = fourElement;
    else if ( obj == fiveElement && ev->type() == QEvent::FocusIn )
	m_focus = fiveElement;
    else
	return FALSE;

    if ( m_focus )
	m_pView->canvasWidget()->startChoose();

    return FALSE;
}

void KSpreadDlgFormula::slotOk()
{
    m_pView->canvasWidget()->endChoose();
    // Switch back to the old table
    if(m_pView->activeTable()->tableName() !=  m_tableName )
        m_pView->changeTable( m_tableName );

    // Revert the marker to its original position
    m_pView->canvasWidget()->setMarkerColumn( m_column );
    m_pView->canvasWidget()->setMarkerRow( m_row );

    if( m_pView->canvasWidget()->editor()!=0)
        {
        ASSERT( m_pView->canvasWidget()->editor() );

        int pos=m_pView->canvasWidget()->editor()->cursorPosition()+ result->text().length();
        m_pView->canvasWidget()->editor()->setText( result->text() );
        m_pView->canvasWidget()->editor()->setFocus();
        m_pView->canvasWidget()->editor()->setCursorPosition( pos );
        }
    accept();
}

void KSpreadDlgFormula::slotClose()
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

void KSpreadDlgFormula::slotSelectButton()
{
if(functions->currentItem()!=-1)
        {
        slotDoubleClicked(functions->findItem(functions->text(functions->currentItem())));
        }
}

void KSpreadDlgFormula::slotChangeText(const QString &string)
{
    if(refresh_result)
    {

        QString tmp;
        if(firstElement->hasFocus()||m_focus==firstElement)
        {

            tmp=m_leftText+m_funcName+"(";

            if(funct.multiple)
            {
                tmp=create_formula(tmp);
            }
            else
            {
                if(!firstElement->text().isEmpty())
                        tmp=tmp+make_formula(firstElement->text(),funct.firstElementType);

                if(funct.nb_param>1)
                {
                        if( !secondElement->text().isEmpty())
                                tmp=tmp+","+make_formula(secondElement->text(),funct.secondElementType);
                        else if( !thirdElement->text().isEmpty()
                                || !fourElement->text().isEmpty()|| !fiveElement->text().isEmpty())
                                tmp=tmp+",";

                }
                if(funct.nb_param>2)
                {
                        if( !thirdElement->text().isEmpty())
                                tmp=tmp+","+make_formula(thirdElement->text(),funct.thirdElementType);
                        else if(!fourElement->text().isEmpty() || !fiveElement->text().isEmpty())
                                tmp=tmp+",";
                }
                if(funct.nb_param>3)
                {
                        if(!fourElement->text().isEmpty())
                                tmp=tmp+","+make_formula(fourElement->text(),funct.fourElementType);
                        else if( !fiveElement->text().isEmpty())
                                tmp=tmp+",";
                }
                if(funct.nb_param>4)
                {
                        if( !fiveElement->text().isEmpty())
                                tmp=tmp+","+make_formula(fiveElement->text(),funct.fiveElementType);
                }
            }
        }
        else if(secondElement->hasFocus()||m_focus==secondElement)
        {
            tmp=m_leftText+m_funcName+"(";
            if(funct.multiple)
            {
                tmp=create_formula(tmp);
            }
            else
            {

                tmp=tmp+make_formula(firstElement->text(),funct.firstElementType)+","+
                        make_formula(string,funct.secondElementType);
                if(funct.nb_param>2)
                {
                        if( !thirdElement->text().isEmpty())
                                tmp=tmp+","+make_formula(thirdElement->text(),funct.thirdElementType);
                        else if(!fourElement->text().isEmpty() || !fiveElement->text().isEmpty())
                                tmp=tmp+",";
                }
                if(funct.nb_param>3)
                {
                        if(!fourElement->text().isEmpty())
                                tmp=tmp+","+make_formula(fourElement->text(),funct.fourElementType);
                        else if( !fiveElement->text().isEmpty())
                                tmp=tmp+",";
                }
                if(funct.nb_param>4)
                {
                        if( !fiveElement->text().isEmpty())
                                tmp=tmp+","+make_formula(fiveElement->text(),funct.fiveElementType);
                }
            }
        }
        else if(thirdElement->hasFocus()||m_focus==thirdElement)
        {

            tmp=m_leftText+m_funcName+"(";

            if(funct.multiple)
            {
                 tmp=create_formula(tmp);
            }
            else
            {
                tmp=tmp+make_formula(firstElement->text(),funct.firstElementType)
                +","+make_formula(secondElement->text(),funct.secondElementType)+","+
                make_formula(string,funct.thirdElementType);

                if(funct.nb_param>3)
                {
                        if( !fourElement->text().isEmpty())
                                tmp=tmp+","+ make_formula(fourElement->text(),funct.fourElementType);
                        else if(!fiveElement->text().isEmpty())
                                tmp=tmp+",";
                }
                if(funct.nb_param>4)
                {
                        if( !fiveElement->text().isEmpty())
                                tmp=tmp+","+ make_formula(fiveElement->text(),funct.fiveElementType);
                }
            }
        }
        else if(fourElement->hasFocus()||m_focus==fourElement)
        {
            tmp=m_leftText+m_funcName+"(";

            if(funct.multiple)
            {
                tmp=create_formula(tmp);
            }
            else
            {
                tmp=tmp+make_formula(firstElement->text(),funct.firstElementType)+","+
                        make_formula(secondElement->text(),funct.secondElementType)+",";
                tmp=tmp+make_formula(thirdElement->text(),funct.thirdElementType)+","+
                        make_formula(string,funct.fourElementType);
                if(funct.nb_param>4)
                {
                        if( !fiveElement->text().isEmpty())
                                tmp=tmp+","+make_formula(fiveElement->text(),funct.fiveElementType);
                }
            }
        }
        else if(fiveElement->hasFocus()||m_focus==fiveElement)
        {
            tmp=m_leftText+m_funcName+"(";
            if(funct.multiple)
            {
                tmp=create_formula(tmp);
            }
            else
            {

                tmp=tmp+make_formula(firstElement->text(),funct.firstElementType)+","+
                        make_formula(secondElement->text(),funct.secondElementType)+",";
                tmp=tmp+make_formula(thirdElement->text(),funct.thirdElementType)+","+
                        make_formula(fourElement->text(),funct.fourElementType)+","+
                        make_formula(string,funct.fiveElementType);
            }
        }
        tmp=tmp+")"+m_rightText;
        result->setText(tmp);
    }
}

QString KSpreadDlgFormula::create_formula(QString tmp)
{
if(!firstElement->text().isEmpty())
        tmp=tmp+make_formula(firstElement->text(),funct.firstElementType);
if(!secondElement->text().isEmpty())
        if(!firstElement->text().isEmpty())
                tmp=tmp+","+make_formula(secondElement->text(),funct.secondElementType);
        else
                tmp=tmp+make_formula(secondElement->text(),funct.secondElementType);
if(!thirdElement->text().isEmpty())
        if(!secondElement->text().isEmpty()||!firstElement->text().isEmpty())
                tmp=tmp+","+make_formula(thirdElement->text(),funct.thirdElementType);
        else
                tmp=tmp+make_formula(thirdElement->text(),funct.thirdElementType);
if(!fourElement->text().isEmpty())
        if(!secondElement->text().isEmpty()||!firstElement->text().isEmpty()
        ||!thirdElement->text().isEmpty())
                tmp=tmp+","+make_formula(fourElement->text(),funct.fourElementType);
        else
                tmp=tmp+make_formula(fourElement->text(),funct.fourElementType);
if(!fiveElement->text().isEmpty())
        if(!secondElement->text().isEmpty()||!firstElement->text().isEmpty()
        ||!thirdElement->text().isEmpty()||!fourElement->text().isEmpty())
                tmp=tmp+","+make_formula(fiveElement->text(),funct.fiveElementType);
        else
                tmp=tmp+make_formula(fiveElement->text(),funct.fiveElementType);
return(tmp);
}
QString KSpreadDlgFormula::make_formula( const QString& _text,type_create elementType)
{
    QString text=_text;
    bool ok1=false;
    bool ok2=false;
    if(elementType==type_string)
    {
        text.toDouble(&ok1);
        text.toInt(&ok2);
        if(ok1||ok2)
        {
            text="\""+text+"\"";
        }
        else if(text.left(1)=="\"")
	{
	    //find other "
	    int pos_car=text.find("\"",1);
            // kdDebug(36001) << "Index : " << pos_car << endl;
            if(pos_car==-1)
	    {
		//so don't find "
		text+="\"";
	    }
            else if(pos_car < (int)text.length())
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
	    if(funct.nb_param==5)
		//When nb_param =4 =>function multi parameter
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


void KSpreadDlgFormula::slotDoubleClicked(QListBoxItem *)
{
    m_focus=0;
    m_oldLength=result->text().length();

    if(funct.nb_param==0)
    {
        help->setText(funct.help);
    }
    if(funct.nb_param>0)
    {
        m_focus=firstElement;
        firstElement->setFocus();
        firstElement->show();
        if(funct.firstElementType==type_double)
            firstElement->setValidator( new KFloatValidator( firstElement ) );
        else if( funct.firstElementType==type_int)
            firstElement->setValidator( new KIntValidator( firstElement ) );
        else
            firstElement->setValidator( 0 );

        label1->setText(funct.firstElementLabel);
        label1->show();
        help->setText(funct.help);
    }
    if(funct.nb_param>1)
    {
        secondElement->show();
        if(funct.secondElementType==type_double)
            secondElement->setValidator( new KFloatValidator( secondElement ) );
        else if( funct.secondElementType==type_int)
            secondElement->setValidator( new KIntValidator( secondElement ) );
        else
            secondElement->setValidator( 0 );
        label2->setText(funct.secondElementLabel);
        label2->show();
    }
    if(funct.nb_param>2)
    {
        thirdElement->show();
        if(funct.thirdElementType==type_double)
            thirdElement->setValidator( new KFloatValidator( thirdElement ) );
        else if( funct.thirdElementType==type_int)
            thirdElement->setValidator( new KIntValidator( thirdElement ) );
        else
            thirdElement->setValidator( 0 );
        label3->setText(funct.thirdElementLabel);
        label3->show();
    }
    if(funct.nb_param>3)
    {
        fourElement->show();
        if(funct.fourElementType==type_double)
            fourElement->setValidator( new KFloatValidator( fourElement ) );
        else if( funct.fourElementType==type_int)
            fourElement->setValidator( new KIntValidator( fourElement ) );
        else
            fourElement->setValidator( 0 );
        label4->setText(funct.fourElementLabel);
        label4->show();
    }
    if(funct.nb_param>4)
    {
        fiveElement->show();
        if(funct.fiveElementType==type_double)
            fiveElement->setValidator( new KFloatValidator( fiveElement ) );
        else if( funct.fiveElementType==type_int)
            fiveElement->setValidator( new KIntValidator( fiveElement ) );
        else
            fiveElement->setValidator( 0 );
        label5->setText(funct.fiveElementLabel);
        label5->show();
    }
    if(funct.nb_param>5)
    {
        kdDebug(36001) << "Error in param->nb_param" << endl;
    }

    if(result->cursorPosition()<m_oldLength)
    {
        m_rightText=result->text().right(m_oldLength-result->cursorPosition());
        m_leftText=result->text().left(result->cursorPosition());
    }
    else
    {
        m_rightText="";
        m_leftText=result->text();
    }
    int pos=result->cursorPosition();
    result->setText(m_leftText+functions->text(functions->currentItem())
                    +"()"+m_rightText);
    if(funct.nb_param==0)
    {
    	result->setFocus();
 	result->setCursorPosition(pos+functions->text(functions->currentItem()).length()+2);
    }
}

void KSpreadDlgFormula::slotselected(const QString &string)
{
    if(functions->currentItem()!=-1)
        selectFunction->setEnabled(true);
    refresh_result=false;
    m_funcName=string;
    changeFunction();
    //hide all element
    firstElement->hide();
    secondElement->hide();
    thirdElement->hide();
    fourElement->hide();
    fiveElement->hide();
    firstElement->clear();
    secondElement->clear();
    thirdElement->clear();
    fourElement->clear();
    fiveElement->clear();
    label1->hide();
    label2->hide();
    label3->hide();
    label4->hide();
    label5->hide();
    help->setText(funct.help);
    m_focus=0;
    refresh_result=true;
}

void KSpreadDlgFormula::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{
    if ( !m_focus )
	return;

    if ( _selection.left() == 0 )
	return;

    if ( _selection.left() >= _selection.right() && _selection.top() >= _selection.bottom() )
    {

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

void KSpreadDlgFormula::slotActivated(const QString & string)
{
    QStringList list_stat;
    list_stat+="average";
    list_stat+="variance";
    list_stat+="stddev";
    list_stat.sort();

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
    list_anal+="sign";
    list_anal+="INV";
    list_anal.sort();

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
    list_trig+="atan2";
    list_trig.sort();

    QStringList list_logic;
    list_logic+="if";
    list_logic+="not";
    list_logic+="AND";
    list_logic+="OR";
    list_logic+="NAND";
    list_logic+="NOR";
    list_logic.sort();

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
    list_text+="mid";
    list_text+="find";
    list_text.sort();

    QStringList list_date_time;
    list_date_time+="date";
    list_date_time+="day";
    list_date_time+="month";
    list_date_time+="time";
    list_date_time+="currentDate";
    list_date_time+="currentTime";
    list_date_time+="currentDateTime";
    list_date_time+="dayOfYear";
    list_date_time.sort();


    QStringList list_financial;
    list_financial+="compound";
    list_financial+="continuous";
    list_financial+="effective";
    list_financial+="nominal";
    list_financial+="FV";
    list_financial+="FV_annuity";
    list_financial+="PV";
    list_financial+="PV_annuity";
    list_financial.sort();

    if(string== i18n("Statistic") )
    {
	functions->clear();
	functions->insertStringList(list_stat);
    }
    if (string ==i18n("Trigonometric"))
    {
	functions->clear();
	functions->insertStringList(list_trig);
    }
    if (string ==i18n("Analytic"))
    {
	functions->clear();
	functions->insertStringList(list_anal);
    }
    if(string== i18n("Logic") )
    {
	functions->clear();
	functions->insertStringList(list_logic);
    }
    if(string== i18n("Text") )
    {
	functions->clear();
	functions->insertStringList(list_text);
    }
    if(string== i18n("Time and Date") )
    {
	functions->clear();
	functions->insertStringList(list_date_time);
    }
    if(string== i18n("Financial") )
    {
	functions->clear();
	functions->insertStringList(list_financial);
    }
    if(string == i18n("All"))
    {
	functions->clear();
	functions->insertStringList(list_stat);
	functions->insertStringList(list_trig);
	functions->insertStringList(list_anal);
	functions->insertStringList(list_text);
	functions->insertStringList(list_logic);
	functions->insertStringList(list_date_time);
	functions->insertStringList(list_financial);
    }
}

void KSpreadDlgFormula::changeFunction()
{
    param tmp;
    tmp.nb_param=0;
    tmp.multiple=false;
    if(m_funcName=="cos" || m_funcName=="sin" || m_funcName=="tan" || m_funcName=="acos" || m_funcName=="asin" || m_funcName=="atan" ||
       m_funcName=="cosh" || m_funcName=="sinh" || m_funcName=="tanh" || m_funcName=="acosh" || m_funcName=="asinh" || m_funcName=="atanh" ||
       m_funcName=="degree" || m_funcName=="radian" )
    {
  	tmp.nb_param = 1;
  	if( m_funcName == "radian")
	    tmp.firstElementLabel=i18n("Angle (deg)");
  	else
	    tmp.firstElementLabel=i18n("Angle (rd)");
	QString tmp1;
	if( m_funcName =="cos")
		{
		tmp1=i18n("The  cos()  function  returns  the cosine of x,\nwhere x is given in radians.\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="cos(0.98) equals 0.5502255.\ncos(0) equals 1.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="sin")
		{
		tmp1=i18n("The  sin()  function  returns  the sine of x,\nwhere x is given in radians.\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="sin(0.12) equals 0.11971221.\nsin(0) equals 0.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="tan")
		{
		tmp1=i18n("The  tan()  function  returns  the tangent of x,\nwhere x is given in radians.\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="tan(0.7) equals 0.84228838.\ntan(0) equals 0.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="acos")
		{
		tmp1=i18n("The  acos() function returns the arc cosine\nin radians and the value is mathematically\ndefined to be between 0 and PI(inclusive).\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="acos(0.8) equals 0.6435011.\nacos(0) equals 1.57079633.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="asin")
		{
		tmp1=i18n("The  asin() function returns the arc sine\nin radians and the value is mathematically\ndefined to be between -PI/2 and PI/2(inclusive).\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="asin(0.8) equals 0.92729522.\nasin(0) equals 0.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="atan")
		{
		tmp1=i18n("The  atan() function returns the arc tangent\nin radians and the value is mathematically\ndefined to be between -PI/2 and PI/2(inclusive).\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="atan(0.8) equals 0.67474094.\natan(0) equals 0.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="cosh")
		{
		tmp1=i18n("The  cosh()  function  returns the hyperbolic\ncosine of x,which is defined\nmathematically as (exp(x) + exp(-x)) / 2.\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="cosh(0.8) equals 1.33743495.\ncosh(0) equals 1.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="sinh")
		{
		tmp1=i18n("The  sinh()  function  returns the hyperbolic\nsine of x,which is defined\nmathematically as (exp(x) - exp(-x)) / 2.\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="sinh(0.8) equals 0.88810598.\nsinh(0) equals 0.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="tanh")
		{
		tmp1=i18n("The  tanh()  function  returns the hyperbolic\ntangent of x,which is defined\nmathematically as sinh(x)/cosh(x).\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="tanh(0.8) equals 0.66403677.\ntanh(0) equals 0.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="acosh")
		{
		tmp1=i18n("The  acosh()  function  calculates \nthe inverse hyperbolic cosine of x;\nthat is the value whose hyperbolic\ncosine  is x.   If  x  is less\nthan 1.0, acosh() returns not-a-number\n(NaN) and errno is set.\n");
	        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="acosh(5) equals 2.29243167.\nacosh(0) equals nan.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="asinh")
		{
		tmp1=i18n("The  asinh()  function  calculates\nthe inverse hyperbolic sine\n of x; that is the value whose\nhyperbolic sine is x.\n");
		tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="asinh(0.8) equals 0.73266826.\nasinh(0) equals 0.";
                tmp.help=tmp1;
		}
	else if( m_funcName=="atanh")
		{
		tmp1=i18n(" The  atanh()  function  calculates\nthe inverse hyperbolic tangent of x;\nthat is the value whose  hyperbolic\ntangent is  x.If  the  absolute value\n of x is greater than 1.0,acosh()\n returns not-a-number (NaN)\nand errno is set.\n"); 
		tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="atanh(0.8) equals 1.09861229.\natanh(0) equals 0.";
                tmp.help=tmp1;
		}
	else
		tmp.help=i18n("Help");
  	tmp.firstElementType=type_double;
    }
    else if( m_funcName == "PI" || m_funcName=="currentDate"
             || m_funcName=="currentTime" ||m_funcName=="currentDateTime")
    {
	tmp.nb_param = 0;
        tmp.help=i18n("Help");
    }
    else if( m_funcName=="right" || m_funcName=="left" || m_funcName=="REPT" )
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("Text");
        if( m_funcName=="right"||m_funcName=="left")
            tmp.secondElementLabel=i18n("Number of characters");
        else if(m_funcName=="REPT")
            tmp.secondElementLabel=i18n("Number of repetitions");

  	tmp.help=m_funcName+"("+"String,Int"+")";
  	tmp.firstElementType=type_string;
  	tmp.secondElementType=type_int;
    }
    else if( m_funcName=="lower" || m_funcName=="upper")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Text");

  	tmp.help=m_funcName+"("+"String"+")";
  	tmp.firstElementType=type_string;
    }
    else if ( m_funcName=="sqrt" || m_funcName=="ln" || m_funcName=="log" || m_funcName=="exp" ||
              m_funcName=="fabs" || m_funcName=="floor" || m_funcName=="ceil" || m_funcName=="ENT" )
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Double");
	QString tmp1;
        tmp.firstElementType=type_double;
        if(m_funcName=="ln")
        	{
        	tmp1=i18n("The ln() function returns the\nnatural logarithm of x.\n");
		tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="ln(0.8) equals -0.22314355.\nln(0) equals -inf.";
                tmp.help=tmp1;
                }
         else if(m_funcName=="log")
        	{
        	tmp1=i18n("The log() function returns\nthe base-10 logarithm of x.\n");
		tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="log(0.8) equals -0.09691001.\nlog(0) equals -inf.";
                tmp.help=tmp1;
                }
	else if(m_funcName=="sqrt")
        	{
        	tmp1=i18n("The  sqrt()  function returns\nthe non-negative square root\n of x.It fails and sets errno to\nEDOM, if x is  negative.\n");
		tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="sqrt(9) equals 3.\nsqrt(-9) equals nan.";
                tmp.help=tmp1;
                }
	else if(m_funcName=="exp")
        	{
        	tmp1=i18n("The exp() function returns\nthe value of e (the  base\nof natural logarithms)\nraised to the power of x.\n");
        	tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
	        tmp1+=i18n("Example : \n");
                tmp1+="sqrt(9) equals 3.\nsqrt(-9) equals nan.";
                tmp.help=tmp1;
                }
        else
        	{
        	tmp.help=m_funcName+"("+"Double"+")\n";
        	}

    }
    else if (m_funcName=="ISLOGIC"||m_funcName=="ISTEXT"||m_funcName=="ISNUM")
    {
   	tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Value");
  	tmp.help=m_funcName+"("+"Value"+")";

  	if(m_funcName=="ISTEXT")
            tmp.firstElementType=type_string;
  	else
            tmp.firstElementType=type_double;
    }
    else if( m_funcName=="sum" || m_funcName=="max" || m_funcName=="min" ||
             m_funcName=="multiply" || m_funcName=="average" || m_funcName=="variance" || m_funcName=="stddev")
    {
   	tmp.nb_param=5;
        tmp.firstElementLabel=i18n("Double");
  	tmp.secondElementLabel=i18n("Double");
  	tmp.thirdElementLabel=i18n("Double");
  	tmp.fourElementLabel=i18n("Double");
  	tmp.fiveElementLabel=i18n("Double");
        tmp.help=m_funcName+"("+"Double,Double,..."+")";
  	tmp.firstElementType=type_double;
  	tmp.secondElementType=type_double;
  	tmp.thirdElementType=type_double;
  	tmp.fourElementType=type_double;
  	tmp.fiveElementType=type_double;
        tmp.multiple=true;
    }
    else if (m_funcName=="if")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Exp logic");
        tmp.secondElementLabel=i18n("if true");
        tmp.thirdElementLabel=i18n("if false");
        tmp.help=m_funcName+"("+"Exp logic,if true,if false"+")";
        tmp.firstElementType=type_logic;
        tmp.firstElementType=type_string;
        tmp.firstElementType=type_string;
    }
    else if (m_funcName=="date" || m_funcName=="dayOfYear")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Year (int)");
        tmp.secondElementLabel=i18n("Month (int)");
        tmp.thirdElementLabel=i18n("Day (int)");
        tmp.help=m_funcName+"("+"Year,Month,Day"+")";
        tmp.firstElementType=type_int;
        tmp.secondElementType=type_int;
        tmp.thirdElementType=type_int;
    }
    else if (m_funcName=="time")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Hour (int)");
        tmp.secondElementLabel=i18n("Minute (int)");
        tmp.thirdElementLabel=i18n("Second (int)");
        tmp.help=m_funcName+"("+"Hour,Minute,Second"+")";
        tmp.firstElementType=type_int;
        tmp.secondElementLabel=type_int;
        tmp.thirdElementLabel=type_int;
    }
    else if (m_funcName=="day")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Day (int)");

        tmp.help=m_funcName+"("+"Day"+")";
        tmp.firstElementType=type_int;
    }
    else if (m_funcName=="month")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Month (int)");
        tmp.help=m_funcName+"("+"Month"+")";
        tmp.firstElementType=type_int;
    }
    else if (m_funcName=="fact")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("int");
        tmp.help=m_funcName+"("+"int"+")";
        tmp.firstElementType=type_int;
    }

    else if (m_funcName=="PERMUT"||m_funcName=="COMBIN")
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("int");
        tmp.secondElementLabel=i18n("int");
        tmp.help=m_funcName+"("+"int,int"+")";
        tmp.firstElementType=type_int;
        tmp.secondElementType=type_int;
    }
    else if (m_funcName=="not")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Exp Logic");
        tmp.help=m_funcName+"("+"Exp Logic"+")";
        tmp.firstElementType=type_logic;
    }
    else if (m_funcName=="EXACT"||m_funcName=="find")
    {
        tmp.nb_param=2;
        if(m_funcName=="find")
            tmp.firstElementLabel=i18n("Text search");
        else
            tmp.firstElementLabel=i18n("Text");
        tmp.secondElementLabel=i18n("Text");
        tmp.help=m_funcName+"("+"String,String"+")";
        tmp.firstElementType=type_string;
        tmp.secondElementType=type_string;
    }
    else if (m_funcName=="mid")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Text");
        tmp.secondElementLabel=i18n("Int");
        tmp.thirdElementLabel=i18n("Int");
        tmp.help=m_funcName+"("+"String,Int,Int"+")";
        tmp.firstElementType=type_string;
        tmp.secondElementType=type_int;
        tmp.thirdElementType=type_int;
    }
    else if (m_funcName=="len")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Text");
        tmp.help=m_funcName+"("+"String"+")";
        tmp.firstElementType=type_string;
    }
    else if(m_funcName=="join")
    {
        tmp.nb_param=5;
        tmp.firstElementLabel=i18n("String");
        tmp.secondElementLabel=i18n("String");
        tmp.thirdElementLabel=i18n("String");
        tmp.fourElementLabel=i18n("String");
        tmp.fiveElementLabel=i18n("String");
        tmp.help=m_funcName+"("+"string,string,..."+")";
        tmp.firstElementType=type_string;
        tmp.secondElementType=type_string;
        tmp.thirdElementType=type_string;
        tmp.fourElementType=type_string;
        tmp.fiveElementType=type_string;
        tmp.multiple=true;
    }

    else if(m_funcName=="AND"||m_funcName=="OR"||m_funcName=="NAND"||m_funcName=="NOR")
    {
        tmp.nb_param=5;
        tmp.firstElementLabel=i18n("Exp logic");
        tmp.secondElementLabel=i18n("Exp logic");
        tmp.thirdElementLabel=i18n("Exp logic");
        tmp.fourElementLabel=i18n("Exp logic");
        tmp.fiveElementLabel=i18n("Exp logic");
        tmp.help=m_funcName+"("+"Exp logic,Exp logic,..."+")";
        tmp.firstElementType=type_logic;
        tmp.secondElementType=type_logic;
        tmp.thirdElementType=type_logic;
        tmp.fourElementType=type_logic;
        tmp.fiveElementType=type_logic;
        tmp.multiple=true;
    }
    else if(m_funcName=="BINO" || m_funcName=="INVBINO")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Number of try");
  	if(m_funcName=="BINO")
            tmp.secondElementLabel=i18n("Number of success");
  	else if(m_funcName=="INVBINO")
            tmp.secondElementLabel=i18n("Number of failure");
  	if(m_funcName=="BINO")
            tmp.thirdElementLabel=i18n("Probabity of success");
 	else if(m_funcName=="INVBINO")
            tmp.thirdElementLabel=i18n("Probabity of failure");
  	tmp.help=m_funcName+"("+"Int,Int,Double"+")";
   	tmp.firstElementType=type_int;
   	tmp.secondElementType=type_int;
   	tmp.thirdElementType=type_double;
    }
    else if(m_funcName=="FV" || m_funcName=="PV" )
    {
        tmp.nb_param=3;
        if (m_funcName=="FV")
        {
            tmp.firstElementLabel=i18n("Present Value");
        }
        else
        {
            tmp.thirdElementLabel=i18n("Future Value");
        }
  	tmp.secondElementLabel=i18n("Interest Rate");
  	tmp.thirdElementLabel=i18n("Periods");
  	tmp.help=m_funcName+"("+"Double,Double,Double"+")";
        tmp.firstElementType=type_double;
   	tmp.secondElementType=type_double;
   	tmp.thirdElementType=type_double;
    }
    else if(m_funcName=="PV_annuity" || m_funcName=="FV_annuity")
    {
        tmp.nb_param=5;
        tmp.firstElementLabel=i18n("Payment Amount");
        tmp.secondElementLabel=i18n("Interest Rate (eff.)");
        tmp.thirdElementLabel=i18n("Periods");
        tmp.fourElementLabel=i18n("Initial Amount");
        tmp.fiveElementLabel=i18n("Paid Start Of Period");
        tmp.help=m_funcName+"("+"double,...,double,bool"+")";
        tmp.firstElementType=type_double;
        tmp.secondElementType=type_double;
        tmp.thirdElementType=type_double;
        tmp.fourElementType=type_double;
        tmp.fiveElementType=type_double;
    }
    else if(m_funcName=="compound" )
    {
   	tmp.nb_param=4;
   	tmp.firstElementLabel=i18n("Principal");
  	tmp.secondElementLabel=i18n("Interest Rate");
  	tmp.thirdElementLabel=i18n("Periods per Year");
  	tmp.fourElementLabel=i18n("Years");
  	tmp.help=m_funcName+"("+"Double,Double,..."+")";
  	tmp.firstElementType=type_double;
  	tmp.secondElementType=type_double;
  	tmp.thirdElementType=type_double;
        tmp.fiveElementType=type_double;
    }
    else if(m_funcName=="continuous" )
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Principal");
  	tmp.secondElementLabel=i18n("Interest Rate");
  	tmp.thirdElementLabel=i18n("Years");
  	tmp.help=m_funcName+"("+"Double,Double,Double"+")";
   	tmp.firstElementType=type_double;
   	tmp.secondElementType=type_double;
   	tmp.thirdElementType=type_double;
    }
    else if( m_funcName=="effective" || m_funcName=="nominal"  )
    {
        tmp.nb_param=2;
        if( m_funcName=="effective")
        {
            tmp.firstElementLabel=i18n("Nominal Rate");
        }
        else
        {
            tmp.firstElementLabel=i18n("Effective Rate");
        }
        tmp.secondElementLabel=i18n("Periods per Year");
  	tmp.help=m_funcName+"("+"Double,Double"+")";
  	tmp.firstElementType=type_double;
  	tmp.secondElementType=type_double;
    }
    else if(m_funcName=="STXT")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Text");
  	tmp.secondElementLabel=i18n("Position of start");
  	tmp.thirdElementLabel=i18n("Number of characters");
  	tmp.help=m_funcName+"("+"String,Int,Int"+")";
   	tmp.firstElementType=type_string;
   	tmp.secondElementType=type_int;
   	tmp.thirdElementType=type_int;
    }
    else if (m_funcName=="pow"  )
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("Double");
        tmp.secondElementLabel=i18n("Double");

        QString tmp1=i18n("The  pow(x,y)  function  returns\nthe value of x raised to the power of y.\n");
        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double,Double"+")\n";
        tmp1+=i18n("Example : \n");
        tmp1+="pow(1.2,3.4) equals 1.8572.\npow(2,3) equals 8";
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
        tmp.secondElementType=type_double;
    }
    else if (m_funcName=="MOD" )
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("Double");
        tmp.secondElementLabel=i18n("Double");

       
        tmp.help=m_funcName+"("+"Double,Double"+")";
        tmp.firstElementType=type_double;
        tmp.secondElementType=type_double;
    }
    else if (m_funcName=="sign")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Double");
        QString tmp1=i18n("This function return :\n-1 if the number is negative\n0 if the number is null\nand 1 if the number is positive.\n");
        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
        tmp1+=i18n("Example : \n");
        tmp1+="sign(5) equals 5.\nsign(-5) equals -1.\nsign(0) equals 0.\n";
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
    }    
    else if (m_funcName=="INV")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Double");
        QString tmp1=i18n("This function multiply by -1 each value\n");
        tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double"+")\n";
        tmp1+=i18n("Example : \n");
        tmp1+="INV(-5) equals 5.\nINV(-5) equals 5.\nINV(0) equals 0.\n";
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
    }


    else if (m_funcName=="atan2" )
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("Double");
        tmp.secondElementLabel=i18n("Double");
        QString tmp1=i18n("This function calculates the arc tangent\n of the two variables x and y.\nIt is similar to calculating the arc tangent of y/x,\n except that the signs of both arguments\n are used to determine the quadrant of the result.\n");
	tmp1+=i18n("Syntax : ") +m_funcName+"("+"Double,Double"+")\n";
	tmp1+=i18n("Example : \n");
        tmp1+="ATAN2(0.5,1.0) equals 1.107149.\nATAN2(-0.5,2.0) equals 1.815775.";
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
        tmp.secondElementType=type_double;
    }
    else
    {
        tmp.nb_param=0;
        tmp.help="Problem";
    }

    funct=tmp;
}

#include "kspread_dlg_formula.moc"
