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

#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtabwidget.h>

#include "kspread_dlg_formula.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_util.h"
#include "kspread_layout.h"
#include "kspread_table.h"
#include "kspread_editors.h"
#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapp.h>
#include <kdebug.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <knumvalidator.h>
#include <kdialog.h>

KSpreadDlgFormula::KSpreadDlgFormula( KSpreadView* parent, const char* name,const QString& formulaName)
    : QDialog( parent, name )
{
    m_pView = parent;
    m_focus = 0;

    setCaption( name );

    KSpreadCell* cell = m_pView->activeTable()->cellAt( m_pView->canvasWidget()->markerColumn(),
							m_pView->canvasWidget()->markerRow() );
    QString m_oldText=cell->text();
    // Make shure that there is a cell editor running.
    if ( !m_pView->canvasWidget()->editor() )
    {
        m_pView->canvasWidget()->createEditor( KSpreadCanvas::CellEditor );
        if(cell->text().isEmpty())
                m_pView->canvasWidget()->editor()->setText( "=" );
        else
                if(cell->text().at(0)!='=')
                        m_pView->canvasWidget()->editor()->setText( "="+cell->text()+"+" );
                else
                        m_pView->canvasWidget()->editor()->setText( cell->text()+"+" );
    }

    ASSERT( m_pView->canvasWidget()->editor() );

    QGridLayout *grid1 = new QGridLayout(this,11,2,15,7);

    searchFunct = new KLineEdit(this);
    grid1->addWidget( searchFunct, 0, 0 );

    typeFunction = new QComboBox(this);
    typeFunction->insertItem(i18n("All"));
    typeFunction->insertItem(i18n("Statistic"));
    typeFunction->insertItem(i18n("Trigonometric"));
    typeFunction->insertItem(i18n("Analytic"));
    typeFunction->insertItem(i18n("Logic"));
    typeFunction->insertItem(i18n("Text"));
    typeFunction->insertItem(i18n("Time and Date"));
    typeFunction->insertItem(i18n("Financial"));
    grid1->addWidget( typeFunction, 1, 0 );

    functions = new QListBox(this);
    grid1->addWidget( functions, 2, 0 );

    selectFunction = new QPushButton( this );
    selectFunction->setPixmap( BarIcon( "down", KIcon::SizeSmall ) );
    grid1->addWidget( selectFunction, 3, 0 );

    result = new QLineEdit( this );
    grid1->addMultiCellWidget( result, 4, 4, 0, 1 );

    m_tabwidget = new QTabWidget( this );
    grid1->addMultiCellWidget( m_tabwidget, 0, 2, 1, 1 );

    m_browser = new QTextBrowser( m_tabwidget );
    m_browser->setMinimumWidth( 300 );
    
    m_tabwidget->addTab( m_browser, i18n("&Help") );
    int index = m_tabwidget->currentPageIndex();
    
    m_input = new QWidget( m_tabwidget );
    QVBoxLayout *grid2 = new QVBoxLayout( m_input, KDialog::marginHint(), KDialog::spacingHint() );

    // grid2->setResizeMode (QLayout::Minimum);
    
    label1 = new QLabel(m_input);
    grid2->addWidget( label1 );

    firstElement=new QLineEdit(m_input);
    grid2->addWidget( firstElement );

    label2=new QLabel(m_input);
    grid2->addWidget( label2 );

    secondElement=new QLineEdit(m_input);
    grid2->addWidget( secondElement );

    label3=new QLabel(m_input);
    grid2->addWidget( label3 );

    thirdElement=new QLineEdit(m_input);
    grid2->addWidget( thirdElement );

    label4=new QLabel(m_input);
    grid2->addWidget( label4 );

    fourElement=new QLineEdit(m_input);
    grid2->addWidget( fourElement );

    label5=new QLabel(m_input);
    grid2->addWidget( label5 );

    fiveElement=new QLineEdit(m_input);
    grid2->addWidget( fiveElement );

    grid2->addStretch( 10 );
    
    m_tabwidget->addTab( m_input, i18n("&Edit") );
    m_tabwidget->setTabEnabled( m_input, FALSE );
    
    m_tabwidget->setCurrentPage( index );
    
    // Create the Ok and Cancel buttons
    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();
    grid1->addMultiCellWidget( bb, 5, 5, 0, 1 );

    refresh_result = true;

    connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
    connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( typeFunction, SIGNAL( activated(const QString &) ),
             this, SLOT( slotActivated(const QString &) ) );
    connect( functions, SIGNAL( highlighted(const QString &) ),
             this, SLOT( slotSelected(const QString &) ) );
    connect( functions, SIGNAL( selected(const QString &) ),
             this, SLOT( slotSelected(const QString &) ) );
    connect( functions, SIGNAL( doubleClicked(QListBoxItem * ) ),
             this ,SLOT( slotDoubleClicked(QListBoxItem *) ) );

    slotActivated(i18n("All"));

    connect( selectFunction, SIGNAL(clicked()),
	     this,SLOT(slotSelectButton()));

    connect( firstElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( secondElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( thirdElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( fourElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( fiveElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));

    connect( m_pView, SIGNAL( sig_chooseSelectionChanged( KSpreadTable*, const QRect& ) ),
             this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );

    // Save the name of the active table.
    m_tableName = m_pView->activeTable()->tableName();
    // Save the cells current text.
    QString tmp_oldText = m_pView->canvasWidget()->editor()->text();
    // Position of the cell.
    m_column = m_pView->canvasWidget()->markerColumn();
    m_row = m_pView->canvasWidget()->markerRow();

    if( tmp_oldText.isEmpty() )
        result->setText("=");
    else
    {
        if( tmp_oldText.at(0)!='=')
	    result->setText( "=" + tmp_oldText );
        else
	    result->setText( tmp_oldText );
    }

    // Allow the user to select cells on the spreadsheet.
    m_pView->canvasWidget()->startChoose();

    qApp->installEventFilter( this );

    // Was a function name passed along with the constructor ? Then activate it.
    if( !formulaName.isEmpty() )
    {
        functions->setCurrentItem( functions->index( functions->findItem( formulaName ) ) );
        slotDoubleClicked( functions->findItem( formulaName ) );
    }
    else
    {
	// Set keyboard focus to allow selection of a formula.
        searchFunct->setFocus();
    }

    // Add auto completion.
    searchFunct->setCompletionMode( KGlobalSettings::CompletionAuto );
    searchFunct->setCompletionObject( &listFunct, true );

    if( functions->currentItem() == -1 )
        selectFunction->setEnabled( false );

    connect( searchFunct, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( slotSearchText(const QString &) ) );
    connect( searchFunct, SIGNAL( returnPressed() ),
	     this, SLOT( slotPressReturn() ) );
}

void KSpreadDlgFormula::slotPressReturn()
{
    if( !functions->currentText().isEmpty() )
        slotDoubleClicked( functions->findItem( functions->currentText() ) );
}

void KSpreadDlgFormula::slotSearchText(const QString &_text)
{
    QString result = listFunct.makeCompletion( _text );
    if( !result.isNull() )
        functions->setCurrentItem( functions->index( functions->findItem( result ) ) );
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
    if( m_pView->activeTable()->tableName() !=  m_tableName )
    {
        KSpreadTable *table=m_pView->doc()->map()->findTable(m_tableName);
        if( table)
	    table->setActiveTable();
    }

    // Revert the marker to its original position
    m_pView->canvasWidget()->activeTable()->setMarker( QPoint( m_column, m_row ) );

    // If there is still an editor then set the text.
    // Usually the editor is always in place.
    if( m_pView->canvasWidget()->editor() != 0 )
    {
        ASSERT( m_pView->canvasWidget()->editor() );
        QString tmp = result->text();
        if( tmp.at(0) != '=')
	    tmp = "=" + tmp;
        int pos = m_pView->canvasWidget()->editor()->cursorPosition()+ tmp.length();
        m_pView->canvasWidget()->editor()->setText( tmp );
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
    {
        KSpreadTable *table=m_pView->doc()->map()->findTable(m_tableName);
        if( !table )
	    return;
	table->setActiveTable();
    }


    // Revert the marker to its original position
    m_pView->canvasWidget()->activeTable()->setMarker( QPoint( m_column, m_row ) );
    // If there is still an editor then reset the text.
    // Usually the editor is always in place.
    if( m_pView->canvasWidget()->editor() != 0 )
    {
        ASSERT( m_pView->canvasWidget()->editor() );
        m_pView->canvasWidget()->editor()->setText( m_oldText );
        m_pView->canvasWidget()->editor()->setFocus();
    }

    reject();
}

void KSpreadDlgFormula::slotSelectButton()
{
    if( functions->currentItem() != -1 )
    {
        slotDoubleClicked(functions->findItem(functions->text(functions->currentItem())));
    }
}

void KSpreadDlgFormula::slotChangeText( const QString& )
{
    // Test the lock
    if( !refresh_result )
	return;
    
    if ( m_focus == 0 )
	return;

    QString tmp = m_leftText+m_funcName+"(";
    tmp += createFormula();
    tmp = tmp+ ")" + m_rightText;
    
    result->setText( tmp );
}

QString KSpreadDlgFormula::createFormula()
{
    QString tmp( "" );
    
    bool first = TRUE;
    
    int count = funct.nb_param;
    if ( funct.multiple )
	count = 5;
    
    if(!firstElement->text().isEmpty() && count >= 1 )
    {
        tmp=tmp+createParameter(firstElement->text(),funct.firstElementType);
	first = FALSE;
    }
    
    if(!secondElement->text().isEmpty() && count >= 2 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+","+createParameter(secondElement->text(),funct.secondElementType);
	else
	    tmp=tmp+createParameter(secondElement->text(),funct.secondElementType);
    }
    if(!thirdElement->text().isEmpty() && count >= 3 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+","+createParameter(thirdElement->text(),funct.thirdElementType);
        else
	    tmp=tmp+createParameter(thirdElement->text(),funct.thirdElementType);
    }
    if(!fourElement->text().isEmpty() && count >= 4 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+","+createParameter(fourElement->text(),funct.fourElementType);
        else
	    tmp=tmp+createParameter(fourElement->text(),funct.fourElementType);
    }
    if(!fiveElement->text().isEmpty() && count >= 5 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+","+createParameter(fiveElement->text(),funct.fiveElementType);
        else
	    tmp=tmp+createParameter(fiveElement->text(),funct.fiveElementType);
    }
    
    return(tmp);
}

QString KSpreadDlgFormula::createParameter( const QString& _text, KSpreadParameterType elementType )
{
    if ( _text.isEmpty() )
	return QString( "" );
    
    QString text;
    
    switch( elementType )
    {
    case type_string:
	{			
	    // Does the text start with quotes?
	    if ( _text[0] == '"' )
	    {
		text = "\"";
	    
		// Escape quotes
		QString tmp = _text;
		int pos;
		while( ( pos = tmp.find( '"', 1 ) ) != -1 )
		    tmp.replace( pos, 1, "\\\"" );
	    
		text += tmp;
		text += "\"";
	    }
	    else
	    {
		KSpreadPoint p = KSpreadPoint( _text, m_pView->doc()->map() );
		KSpreadRange r = KSpreadRange( _text, m_pView->doc()->map() );
		if( !p.isValid() && !r.isValid() )
		{
		    text = "\"";
	    
		    // Escape quotes
		    QString tmp = _text;
		    int pos;
		    while( ( pos = tmp.find( '"', 1 ) ) != -1 )
			tmp.replace( pos, 1, "\\\"" );
	    
		    text += tmp;
		    text += "\"";
		}
		else
		    text = _text;
            }
        }
	return text;
    case type_double:
	return _text;
    case type_int:
	return _text;
    case type_logic:
	return _text;
    }
    
    // Never reached
    return text;
}

void KSpreadDlgFormula::slotDoubleClicked( QListBoxItem* item )
{
    if ( !item )
	return;
    
    m_focus = 0;
    int old_length = result->text().length();

    // Show help
    m_browser->setText( funct.help );
    m_tabwidget->setTabEnabled( m_input, TRUE );
    m_tabwidget->setCurrentPage( 1 );
	
    //
    // Show as many QLineEdits as needed.
    //
    if( funct.nb_param > 0 )
    {
        m_focus = firstElement;
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
    }
    else
    {
	label1->hide();
	firstElement->hide();
    }
    
    if( funct.nb_param > 1 )
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
    else
    {
	label2->hide();
	secondElement->hide();
    }
	
    if( funct.nb_param > 2 )
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
    else
    {
	label3->hide();
	thirdElement->hide();
    }
    
    if( funct.nb_param > 3 )
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
    else
    {
	label4->hide();
	fourElement->hide();
    }

    if( funct.nb_param > 4 )
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
    else
    {
	label5->hide();
	fiveElement->hide();
    }
    
    if( funct.nb_param > 5 )
    {
        kdDebug(36001) << "Error in param->nb_param" << endl;
    }

    //
    // Put the new function call in the result.
    //
    if( result->cursorPosition() < old_length )
    {
        m_rightText=result->text().right(old_length-result->cursorPosition());
        m_leftText=result->text().left(result->cursorPosition());
    }
    else
    {
        m_rightText="";
        m_leftText=result->text();
    }
    int pos = result->cursorPosition();
    result->setText( m_leftText+functions->text( functions->currentItem() ) + "()" + m_rightText);
    
    //
    // Put focus somewhere is there are no QLineEdits visible
    //
    if( funct.nb_param == 0 )
    {
        result->setFocus();
        result->setCursorPosition(pos+functions->text(functions->currentItem()).length()+2);
    }
}

void KSpreadDlgFormula::slotSelected( const QString& function )
{
    if( functions->currentItem() !=- 1 )
        selectFunction->setEnabled( TRUE );
    
    // Lock
    refresh_result = false;
    
    m_funcName = function;
    changeFunction();

    // Set the help text
    m_browser->setText( funct.help );
    
    m_focus=0;

    m_tabwidget->setCurrentPage( 0 );
    m_tabwidget->setTabEnabled( m_input, FALSE );
	
    // Unlock
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

void KSpreadDlgFormula::slotActivated( const QString& category )
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
    list_anal+="DECBIN";
    list_anal+="DECHEX";
    list_anal+="DECOCT";
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
    list_logic+="ISLOGIC";
    list_logic+="ISTEXT";
    list_logic+="ISNUM";
    list_logic.sort();

    QStringList list_text;
    list_text+="join";
    list_text+="right";
    list_text+="left";
    list_text+="len";
    list_text+="EXACT";
    //list_text+="STXT";
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
    if( category == i18n("Statistic") )
    {
        functions->clear();
        functions->insertStringList(list_stat);
        listFunct.setItems(list_stat);
    }
    else if (category == i18n("Trigonometric"))
    {
        functions->clear();
        functions->insertStringList(list_trig);
        listFunct.setItems(list_trig);
    }
    else if (category == i18n("Analytic"))
    {
        functions->clear();
        functions->insertStringList(list_anal);
        listFunct.setItems(list_anal);
    }
    else if( category == i18n("Logic") )
    {
        functions->clear();
        functions->insertStringList(list_logic);
        listFunct.setItems(list_logic);
    }
    else if( category == i18n("Text") )
    {
        functions->clear();
        functions->insertStringList(list_text);
        listFunct.setItems(list_text);
    }
    else if( category == i18n("Time and Date") )
    {
        functions->clear();
        functions->insertStringList(list_date_time);
        listFunct.setItems(list_date_time);
    }
    else if( category == i18n("Financial") )
    {
        functions->clear();
        functions->insertStringList(list_financial);
        listFunct.setItems(list_financial);
    }
    else if(category == i18n("All"))
    {
        QStringList tmp;
        tmp+=list_stat;
        tmp+=list_trig;
        tmp+=list_anal;
        tmp+=list_text;
        tmp+=list_logic;
        tmp+=list_date_time;
        tmp+=list_financial;
        tmp.sort();
        functions->clear();
        functions->insertStringList(tmp);
        listFunct.setItems(tmp);
    }
    
    // Go to the first function in the list.
    functions->setCurrentItem(0);
    slotSelected( functions->text(0) );
}

void KSpreadDlgFormula::changeFunction()
{
    KSpreadFunctionDescription tmp;
    tmp.nb_param=0;
    tmp.multiple=false;
    QString tmp1;
    QString tmp2;
    if(m_funcName=="cos" || m_funcName=="sin" || m_funcName=="tan" || m_funcName=="acos" || m_funcName=="asin" || m_funcName=="atan" ||
       m_funcName=="cosh" || m_funcName=="sinh" || m_funcName=="tanh" || m_funcName=="acosh" || m_funcName=="asinh" || m_funcName=="atanh" ||
       m_funcName=="degree" || m_funcName=="radian" )
    {
        tmp.nb_param = 1;
        if( m_funcName == "radian")
            tmp.firstElementLabel=i18n("Angle (deg)");
        else
            tmp.firstElementLabel=i18n("Angle (rd)");

        if( m_funcName =="cos")
                {
                tmp1=i18n("The cos() function returns the cosine of x,\nwhere x is given in radians.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("cos(0.98) equals 0.5502255.\ncos(0) equals 1.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="sin")
                {
                tmp1=i18n("The sin() function returns the sine of x,\nwhere x is given in radians.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("sin(0.12) equals 0.11971221.\nsin(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="tan")
                {
                tmp1=i18n("The tan() function returns the tangent of x,\nwhere x is given in radians.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("tan(0.7) equals 0.84228838.\ntan(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="acos")
                {
                tmp1=i18n("The acos() function returns the arc cosine\n"
                          "in radians and the value is mathematically\n"
                          "defined to be between 0 and PI(inclusive).\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("acos(0.8) equals 0.6435011.\nacos(0) equals 1.57079633.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="asin")
                {
                tmp1=i18n("The asin() function returns the arc sine\n"
                          "in radians and the value is mathematically\n"
                          "defined to be between -PI/2 and PI/2 (inclusive).\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("asin(0.8) equals 0.92729522.\nasin(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="atan")
                {
                tmp1=i18n("The atan() function returns the arc tangent\n"
                          "in radians and the value is mathematically\n"
                          "defined to be between -PI/2 and PI/2 (inclusive).\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("atan(0.8) equals 0.67474094.\natan(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="cosh")
                {
                tmp1=i18n("The cosh() function returns the hyperbolic\n"
                          "cosine of x, which is defined\n"
                          "mathematically as (exp(x) + exp(-x)) / 2.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("cosh(0.8) equals 1.33743495.\ncosh(0) equals 1.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="sinh")
                {
                tmp1=i18n("The sinh() function returns the hyperbolic\n"
			  "sine of x, which is defined\n"
			  "mathematically as (exp(x) - exp(-x)) / 2.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("sinh(0.8) equals 0.88810598.\nsinh(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="tanh")
                {
                tmp1=i18n("The tanh() function returns the hyperbolic\n"
                          "tangent of x, which is defined\n"
                          "mathematically as sinh(x)/cosh(x).\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("tanh(0.8) equals 0.66403677.\ntanh(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="acosh")
                {
                tmp1=i18n("The acosh() function calculates\n"
                          "the inverse hyperbolic cosine of x;\n"
                          "that is the value whose hyperbolic\n"
                          "cosine is x. If x is less\n"
                          "than 1.0, acosh() returns not-a-number\n"
                          "(NaN) and errno is set.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("acosh(5) equals 2.29243167.\nacosh(0) equals nan.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="asinh")
                {
                tmp1=i18n("The asinh() function  calculates\n"
                          "the inverse hyperbolic sine\n"
                          "of x; that is the value whose\n"
                          "hyperbolic sine is x.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("asinh(0.8) equals 0.73266826.\nasinh(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="atanh")
                {
                tmp1=i18n("The atanh() function calculates\n"
                          "the inverse hyperbolic tangent of x;\n"
                          "that is the value whose hyperbolic\n"
                          "tangent is x. If the absolute value\n"
                          "of x is greater than 1.0, atanh()\n"
                          "returns not-a-number (NaN)\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("atanh(0.8) equals 1.09861229.\natanh(0) equals 0.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="degree")
                {
                tmp1=i18n("This function transforms\n"
                        "a radian angle to degree angle.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("degree(75) equals 44.69 \ndegree(1) equals 57.29.");
                tmp.help=tmp1;
                }
        else if( m_funcName=="radian")
                {
                tmp1=i18n("This function transforms\n"
                        "a degree angle to radian angle.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("radian(75) equals 1.308 \nradian(90) equals 1.5707.");
                tmp.help=tmp1;
                }
        else
                tmp.help=i18n("Help");
        tmp.firstElementType=type_double;
    }
    else if( m_funcName == "PI")
    {
        tmp1=i18n("The PI() function returns the PI value\n");
        tmp1+=i18n("Syntax : %1()\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("PI() equals 3.141592654..... ");
        tmp.help=tmp1;
        tmp.nb_param = 0;
    }
    else if( m_funcName=="currentDate")
    {
        tmp.nb_param = 0;
        tmp1=i18n("The currentDate() function returns the current\n"
                "date formatted with local parameters.\n");
        tmp1+=i18n("Syntax : %1()\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("currentDate() returns saturday 15 July 2000");
        tmp.help=tmp1;

    }
    else if(m_funcName=="currentDateTime")
    {
        tmp.nb_param = 0;
        tmp1=i18n("The currentDateTime() function returns\n"
                "the current date and time formatted\n"
                "with local parameters.\n");
        tmp1+=i18n("Syntax : %1()\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("currentDateTime() returns\nSaturday 15 July 2000 19:12:01\n");
        tmp.help=tmp1;
    }
    else if( m_funcName=="currentTime")
    {
        tmp.nb_param = 0;
        QString tmp1=i18n("The currentTime() function returns the current time\n"
                        "formatted with local parameters.\n");
        tmp1+=i18n("Syntax : %1()\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("currentTime() returns 19:12:01");
        tmp.help=tmp1;
    }
    else if( m_funcName=="right" || m_funcName=="left" || m_funcName=="REPT" )
    {
        tmp.nb_param=2;
        tmp2=i18n("Syntax : %1(String,Int)\n").arg(m_funcName);
        tmp2+=i18n("Example : \n");
        tmp.firstElementLabel=i18n("Text");
        if( m_funcName=="right"||m_funcName=="left")
            tmp.secondElementLabel=i18n("Number of characters");
        else if(m_funcName=="REPT")
            tmp.secondElementLabel=i18n("Number of repetitions");
        if( m_funcName=="right")
        {
                tmp1+=i18n("The right() function returns a substring\n"
                "that contains the len rightmost characters\n"
                "of the string. The whole string is returned\n"
                "if len exceeds the length of the string.\n");
                tmp1+=tmp2;
                tmp1+=i18n("right(\"hello\",2) returns lo\nright(\"kspread\",10) returns kspread");
        }
        else if(m_funcName=="left")
        {
                tmp1+=i18n("The left() function returns a substring\n"
                "that contains the len leftmost characters\n"
                "of the string. The whole string is returned\n"
                "if len exceeds the length of the string.\n");
                tmp1+=tmp2;
                tmp1+=i18n("left(\"hello\",2) returns he\nleft(\"kspread\",10) returns kspread");

        }
        else if (m_funcName=="REPT" )
        {
                tmp1=i18n("The REPT() function repeats the first parameter\n"
                        "as often as told by the second parameter.\n");
                tmp1+=i18n("Syntax : %1(String,Int)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("REPT(\"kspread\",3) returns kspreadkspreadkspread.");
        }
        else
        {
                tmp1+="help";
                tmp1+=tmp2;
        }
        tmp.help=tmp1;
        tmp.firstElementType=type_string;
        tmp.secondElementType=type_int;
    }
    else if( m_funcName=="lower" || m_funcName=="upper")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Text");
        tmp2=i18n("Syntax : %1(String)\n").arg(m_funcName);
        tmp2+=i18n("Example : \n");
        if( m_funcName=="upper")
        {
                tmp1=i18n("The upper() function converts\n"
                          "a string to upper case\n");
                tmp1+=tmp2;
                tmp1+=i18n("upper(\"hello\") returns HELLO\nupper(\"HELLO\") returns HELLO");
        }
        else if(m_funcName=="lower")
        {
                tmp1=i18n("The lower() function converts\n"
                        "a string to lower case\n");
                tmp1+=tmp2;
                tmp1+=i18n("lower(\"HELLO\") returns hello\nlower(\"Hello\") returns hello");
        }
        tmp.help=tmp1;

        tmp.firstElementType=type_string;
    }
    else if ( m_funcName=="sqrt" || m_funcName=="ln" || m_funcName=="log" || m_funcName=="exp" ||
              m_funcName=="fabs" || m_funcName=="floor" || m_funcName=="ceil" || m_funcName=="ENT" )
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Double");

        tmp.firstElementType=type_double;
        if(m_funcName=="ln")
                {
                tmp1=i18n("The ln() function returns the\n"
                          "natural logarithm of x.\n");
                tmp1+=i18n("Syntax : %1(Double)").arg(m_funcName);
                tmp1+=i18n("\nExample : \n");
                tmp1+=i18n("ln(0.8) equals -0.22314355.\nln(0) equals -inf.");
                tmp.help=tmp1;
                }
         else if(m_funcName=="log")
                {
                tmp1=i18n("The log() function returns\nthe base-10 logarithm of x.\n");
                tmp1+=i18n("Syntax : %1(Double)").arg(m_funcName);
                tmp1+=i18n("\nExample : \n");
                tmp1+=i18n("log(0.8) equals -0.09691001.\nlog(0) equals -inf.");
                tmp.help=tmp1;
                }
        else if(m_funcName=="sqrt")
                {
                tmp1=i18n("The sqrt() function returns\n"
                          "the non-negative square root\n"
                          "of x. It fails and sets errno to\n"
                          "EDOM, if x is negative.\n");
                tmp1+=i18n("Syntax : %1(Double)").arg(m_funcName);
                tmp1+=i18n("\nExample : \n");
                tmp1+=i18n("sqrt(9) equals 3.\nsqrt(-9) equals nan.");
                tmp.help=tmp1;
                }
        else if(m_funcName=="exp")
                {
                tmp1=i18n("The exp() function returns\n"
                          "the value of e (the  base\n"
                          "of natural logarithms)\n"
                          "raised to the power of x.\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("exp(9) equals 8 103.08392758.\nexp(-9) equals 0.00012341.");
                tmp.help=tmp1;
                }
        else if(m_funcName=="ceil")
                {
                tmp1=i18n("The ceil() function rounds x \n"
                           "upwards to the nearest integer, \n"
                           "returning that value as a double.\n");
                tmp1+=i18n("Syntax : %1(Double)").arg(m_funcName);
                tmp1+=i18n("\nExample : \n");
                tmp1+=i18n("ceil(12.5) equals 13.\nceil(-12.5) equals -12.");
                tmp.help=tmp1;
                }
        else if(m_funcName=="fabs")
                {
                tmp1=i18n("The fabs() function returns\n"
                          "the absolute value of the \n"
                          "floating-point number x.\n");
                tmp1+=i18n("Syntax : %1(Double)").arg(m_funcName);
                tmp1+=i18n("\nExample : \n");
                tmp1+=i18n("fabs(12.5) equals 12.5.\nfabs(-12.5) equals 12.5.");
                tmp.help=tmp1;
                }
        else if(m_funcName=="floor")
                {
                tmp1=i18n("The floor() function rounds\n"
                          "x downwards to the nearest integer, \n"
                          "returning that value as a double.\n");
                tmp1+=i18n("Syntax : %1(Double)").arg(m_funcName);
                tmp1+=i18n("\nExample : \n");
                tmp1+=i18n("floor(12.5) equals 12.\nfloor(-12.5) equals -13.");
                tmp.help=tmp1;
                }
        else if(m_funcName=="ENT")
                {
                tmp1=i18n("The ENT() function returns\n"
                        "the integer part of the value\n");
                tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("ENT(12.55) equals 12\nENT(15) equals 15.");
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
        tmp2=i18n("Syntax : %1(value)\n").arg(m_funcName);
        tmp2+=i18n("Example : \n");
        tmp.firstElementLabel=i18n("Value");
        if(m_funcName=="ISNUM")
        {
                tmp1=i18n("The ISNUM() function returns True\n"
                        "if the parameter is a numerical value\n"
                        "otherwise it returns False\n");
                tmp1+=tmp2;
                tmp1+=i18n("ISNUM(12) returns True\nISNUM(hello) returns False");
        }
        else if( m_funcName=="ISLOGIC")
        {
                tmp1=i18n("The ISLOGIC() function returns True\n"
                        "if the parameter is a bool value\n"
                        "otherwise returns False\n");
                tmp1+=tmp2;
                tmp1+=i18n("ISLOGIC(A1>A2) returns True\nISLOGIC(12) returns False");
        }
        else if( m_funcName=="ISTEXT")
        {
                tmp1=i18n("The ISTEXT() function returns True\n"
                        "if the parameter is a string\n"
                        "and otherwise returns False\n");
                tmp1+=tmp2;
                tmp1+=i18n("ISTEXT(12) returns False\nISTEXT(\"hello\") returns True");

        }
        if(m_funcName=="ISTEXT")
            tmp.firstElementType=type_string;
        else
            tmp.firstElementType=type_double;
        tmp.help=tmp1;
    }
    else if( m_funcName=="sum" || m_funcName=="max" || m_funcName=="min" ||
             m_funcName=="multiply" || m_funcName=="average" || m_funcName=="variance" || m_funcName=="stddev")
    {

        tmp2=i18n("Syntax : %1(Double,Double,Double....)\n").arg(m_funcName);
        tmp2+=i18n("Example : \n");
        tmp.nb_param=5;
        tmp.firstElementLabel=i18n("Double");
        tmp.secondElementLabel=i18n("Double");
        tmp.thirdElementLabel=i18n("Double");
        tmp.fourElementLabel=i18n("Double");
        tmp.fiveElementLabel=i18n("Double");
        if( m_funcName=="sum")
        {
                tmp1=i18n("The sum() function calculates the sum\n"
                "of all the values given as parameters.\n"
                "You can calculate the sum of a range (sum(A1:B5))\n"
                "or a list of values like (sum(12,5,12.5))\n");
                tmp1+=tmp2;
                tmp1+=i18n("sum(12,5,7) equals 24\nsum(12.5,2) equals 14.5");
        }
        else if(m_funcName=="average")
        {
                tmp1=i18n("The average() function calculates the average\n"
                "of all the values given as parameters.\n"
                "You can calculate the average of a range ( average(A1:B5))\n"
                "or a list of values like (average(12,5,12.5))\n");
                tmp1+=tmp2;
                tmp1+=i18n("average(12,5,7) equals 8\naverage(12.5,2) equals 7.25");
        }
        else if(m_funcName=="max")
        {
                tmp1=i18n("The max() function returns the biggest value\n"
                        "given in the parameters.\n");
                tmp1+=tmp2;
                tmp1+=i18n("max(12,5,7) returns 12\nmax(12.5,2) returns 12.5");
        }
        else if( m_funcName=="min")
        {
                tmp1=i18n("The min() function returns the smallest\n"
                        "value given in the parameters.\n");
                tmp1+=tmp2;
                tmp1+=i18n("min(12,5,7) returns 5\nmin(12.5,2) returns 2");

        }
        else if(m_funcName=="multiply")
        {
                tmp1=i18n("The multiply() function multiplies all the values\n"
                        "given in the parameters.\n"
                        "You can multiply values given by a range ( multiply(A1:B5))\n"
                        "or list of values like (multiply(12,5,12.5))\n");
                tmp1+=tmp2;
                tmp1+=i18n("multiply(12,5,7) equals 420\nmultiply(12.5,2) equals 25");

        }
        else if(m_funcName=="variance")
        {
                tmp1=i18n("The variance() function calculates the variance\n"
                        "of each parameter which are member of a population.\n");
                tmp1+=tmp2;
                tmp1+=i18n("variance(12,5,7) equals 8.666..\nvariance(15,80,3) equals 1144.22...");
        }
        else
        {
                tmp1="Help\n";
                tmp1+=tmp2;
        }

        tmp.help=tmp1;
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
        tmp1+=i18n("The if() function is a conditional function.\n"
                "This function returns the second parameter\n"
                "if the condition is True, otherwise it returns\n"
                "the third parameter.\n");

        tmp1+=i18n("Syntax : %1(bool expr,if true,if false)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("A1=4, A2=6, if(A1>A2,5,3) returns 3\n");
        tmp.help=tmp1;
        tmp.firstElementType=type_logic;
        tmp.firstElementType=type_string;
        tmp.firstElementType=type_string;
    }
    else if( m_funcName=="dayOfYear")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Year (int)");
        tmp.secondElementLabel=i18n("Month (int)");
        tmp.thirdElementLabel=i18n("Day (int)");
        QString tmp1=i18n("The dayOfYear() function returns the\n"
                        "number of days in the year (1...365).\n");
        tmp1+=i18n("Syntax : %1(Year,Month,Day)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("dayOfYear(2000,12,1) returns 336\ndayOfYear(2000,2,29) return 60");
        tmp.help=tmp1;
        tmp.firstElementType=type_int;
        tmp.secondElementType=type_int;
        tmp.thirdElementType=type_int;
    }
    else if (m_funcName=="date")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Year (int)");
        tmp.secondElementLabel=i18n("Month (int)");
        tmp.thirdElementLabel=i18n("Day (int)");
        tmp1=i18n("The date() function returns the date\n"
                "formatted with local parameters\n");
        tmp1+=i18n("Syntax : %1(Year,Month,Day)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("date(2000,5,5) returns Friday 05 May 2000");
        tmp.help=tmp1;
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
        tmp1=i18n("The time() function returns the time\n"
                "formatted with local parameters\n");
        tmp1+=i18n("Syntax : %1(Hour,Minute,Second)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("time(8,5,5) returns 08:05:05");
        tmp.help=tmp1;
        tmp.firstElementType=type_int;
        tmp.secondElementLabel=type_int;
        tmp.thirdElementLabel=type_int;
    }
    else if (m_funcName=="day")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Day (int)");
        tmp.firstElementLabel=i18n("Int");
        tmp1=i18n("The day() function returns name of day\n"
                "of the week (1..7).\n");
        tmp1+=i18n("Syntax : %1(Int)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("day(1) returns Monday (if the week starts on Monday)");
        tmp.help=tmp1;
        tmp.firstElementType=type_int;
    }
    else if (m_funcName=="month")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Month (Int)");
        tmp1=i18n("The month() function returns name\n"
                        "of the month (1...12).\n");
        tmp1+=i18n("Syntax : %1(Int)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("month(5) returns May");
        tmp.help=tmp1;
        tmp.firstElementType=type_int;
    }
    else if (m_funcName=="fact")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("int");
        tmp1=i18n("The fact() function calculates the factorial of the\n"
                "parameter. The mathematical expression is (value)!.\n");
        tmp1+=i18n("Syntax : %1(Int)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("fact(10) returns 3628800\nfact(0) returns 1");
        tmp.help=tmp1;
        tmp.firstElementType=type_int;

    }

    else if (m_funcName=="PERMUT"||m_funcName=="COMBIN")
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("int");
        tmp.secondElementLabel=i18n("int");
        if(m_funcName=="COMBIN")
        {
                tmp1=i18n("The COMBIN() function calculates a number\n"
                "of combination. The first parameter is the total\n"
                "number of element. The second parameter is the number\n"
                "of element chosen. These two parameters should be positive,\n"
                "otherwise the function returns an error.\n"
                "The first parameter should be bigger than the second one\n"
                "otherwise the function returns an error, too.\n");
                tmp1+=i18n("Syntax : %1(Int,Int)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("COMBIN(12,5) returns 792\nCOMBIN(5,5) returns 1");


        }
        else
        {
                tmp1=i18n("The PERMUT() function returns the number of\n"
                        "permutations. The first parameter is the number of\n"
                        "elements, and the second parameter is the number of\n"
                        "elements used in the permutation.\n");
                tmp1+=i18n("Syntax : %1(Int,Int)\n").arg(m_funcName);
                tmp1+=i18n("Example : \n");
                tmp1+=i18n("PERMUT(8,5) equals 6720\nPERMUT(1,1) equals 1");
        }
        tmp.help=tmp1;
        tmp.firstElementType=type_int;
        tmp.secondElementType=type_int;
    }
    else if (m_funcName=="not")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Exp Logic");
        tmp1+=i18n("The not() function returns True\n"
        "if the value is False and returns False\n"
        "if the value is True\n");
        tmp1+=i18n("Syntax : %1(Exp Logic)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("not(False) returns True\nnot(True) returns False");
        tmp.help=tmp1;

        tmp.firstElementType=type_logic;
    }
    else if (m_funcName=="EXACT"||m_funcName=="find")
    {
        tmp.nb_param=2;
        tmp2=i18n("Syntax : %1(String,String)\n").arg(m_funcName);
        tmp2+=i18n("Example : \n");
        if(m_funcName=="find")
            tmp.firstElementLabel=i18n("Text search");
        else
            tmp.firstElementLabel=i18n("Text");
        tmp.secondElementLabel=i18n("Text");
        if(m_funcName=="EXACT")
        {
                tmp1=i18n("The EXACT() function returns True\n"
                "if these two strings are equal,\n"
                "otherwise returns False.\n");
                tmp1+=tmp2;
                tmp1+=i18n("EXACT(\"Koffice\",\"Koffice\") returns True\n"
                "EXACT(\"kspread\",\"Koffice\") returns False");


        }
        else if(m_funcName=="find")
        {
                tmp1+=i18n("The find() function searches a substring.\n"
                        "This function returns True if this substring\n"
                        "exists otherwise it returns False.\n");
                tmp1+=tmp2;
                tmp1+=i18n("find(\"Kof\",\"Koffice\") returns True\n"
                "find(\"kspread\",\"Koffice\") returns False");
        }
        tmp.help=tmp1;
        tmp.firstElementType=type_string;
        tmp.secondElementType=type_string;
    }
    else if (m_funcName=="mid")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Text");
        tmp.secondElementLabel=i18n("Position (int)");
        tmp.thirdElementLabel=i18n("Length (int)");
        tmp1=i18n("The mid() function returns a substring that contains\n"
                 "the len characters of this string, starting\n"
                "at position index.\n");
        tmp1+=i18n("Syntax : ") +m_funcName+"("+i18n("String,Position,Length")+")\n";
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("mid(\"Koffice\",2,3) returns ffi");
        tmp.help=tmp1;
        tmp.firstElementType=type_string;
        tmp.secondElementType=type_int;
        tmp.thirdElementType=type_int;
    }
    else if (m_funcName=="len")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Text");
        tmp1+=i18n("The len() function returns\n"
                "the length of the string\n");
        tmp1+=i18n("Syntax : %1(String)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("len(\"hello\") returns 5\nlen(\"kspread\") returns 7");
        tmp.help=tmp1;

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
        tmp1+=i18n("The join() function returns string which is\n"
                   "the concatenation of the strings passed as parameters\n");
        tmp1+=i18n("Syntax : %1(String,String,String....)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("join(\"kspread\",\"koffice\",\"kde\") returns kspreadkofficekde\n");
        tmp.help=tmp1;
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
        tmp2=i18n("Syntax : %1(Exp logic,Exp logic,...)\n").arg(m_funcName);
        tmp2+=i18n("Example : \n");

        tmp.firstElementLabel=i18n("Exp logic");
        tmp.secondElementLabel=i18n("Exp logic");
        tmp.thirdElementLabel=i18n("Exp logic");
        tmp.fourElementLabel=i18n("Exp logic");
        tmp.fiveElementLabel=i18n("Exp logic");
        if( m_funcName=="AND")
        {
                tmp1=i18n("The AND() function returns True if all\n"
                "the values are true, otherwise returns False\n");
                tmp1+=tmp2;
                tmp1+=i18n("AND(True,True,True) returns True\nAND(True,False) returns False");

        }
        else if( m_funcName=="OR")
        {
                tmp1=i18n("The OR() function returns True if at least\n"
                "one value is true, otherwise returns False\n");
                tmp1+=tmp2;
                tmp1+=i18n("OR(True,False,False) returns True\nOR(False,False) returns False");

        }
        else if(m_funcName=="NOR")
        {
                tmp1=i18n("The NOR() function returns True\n"
                "if all the values given as parameters are of type bool\n"
                "and have the value false. Otherwise it returns False.\n");
                tmp1+=tmp2;
                tmp1+=i18n("NOR(True,False,False) returns False\nNOR(False,False) returns True");

        }
        else if(m_funcName=="NAND")
        {
                tmp1=i18n("The NAND() function returns True\n"
                "if at least one value is not true,\n"
                "otherwise returns False\n");
                tmp1+=tmp2;
                tmp1+=i18n("NAND(True,False,False) returns True\nNAND(True,True) returns False");
        }
        tmp.help=tmp1;

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
        tmp.firstElementLabel=i18n("Number of trials");
        if(m_funcName=="BINO")
            tmp.secondElementLabel=i18n("Number of success");
        else if(m_funcName=="INVBINO")
            tmp.secondElementLabel=i18n("Number of failure");
        if(m_funcName=="BINO")
            tmp.thirdElementLabel=i18n("Probability of success");
        else if(m_funcName=="INVBINO")
            tmp.thirdElementLabel=i18n("Probability of failure");
        tmp2=i18n("Syntax : %1(Int,Int,Double)\n").arg(m_funcName);
        tmp2+=i18n("Example : \n");
        if(m_funcName=="BINO")
        {
                tmp1=i18n("The BINO function returns the binomial distribution.\n"
                        "The first parameter is the number of trial, the second\n"
                        "parameter is the number of success, and the third is\n"
                        "the probability of success. The number of trials should\n"
                        "be bigger than the number of successes.\n");
                tmp1+=tmp2;
                tmp1+=i18n("BINO(12,9,0.8) returns 0.236223201");

        }
        else if(m_funcName=="INVBINO")
        {
                tmp.secondElementLabel=i18n("Number of failure");
                tmp.thirdElementLabel=i18n("Probability of failure");
                tmp1=i18n("The INVBINO function returns the negative binomial\n"
                        "distribution. The first parameter is the number of trial,\n"
                        "the second parameter is the number of failure, and\n"
                        "the third is the probability of failure. The number of\n"
                        "trials should be bigger than the number of failure.\n");

                tmp1+=tmp2;
                tmp1+=i18n("INVBINO(12,3,0.8) returns 0.236223201");

        }
        tmp.help=tmp1;
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
        tmp.fiveElementType=type_logic;
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
    /*else if(m_funcName=="STXT")
    {
        tmp.nb_param=3;
        tmp.firstElementLabel=i18n("Text");
        tmp.secondElementLabel=i18n("Position of start");
        tmp.thirdElementLabel=i18n("Number of characters");
        tmp.nb_param=3;

        tmp1=i18n("The STXT functions returns a substring.\n"
                "The first parameter is the string, the second\n"
                "is the position of start and the third is\n"
                "the number of characters chosen.\n");
        tmp1+=i18n("Syntax : %1(String,Int,Int)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("STXT(\"kspread\",2,2) returns pr\n");
        tmp.help=tmp1;
        tmp.firstElementType=type_string;
        tmp.secondElementType=type_int;
        tmp.thirdElementType=type_int;
    } */
    else if (m_funcName=="pow"  )
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("Double");
        tmp.secondElementLabel=i18n("Double");

        QString tmp1=i18n("The pow(x,y) function returns\n"
                          "the value of x raised to the power of y.\n");
        tmp1+=i18n("Syntax : %1(Double,Double)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("pow(1.2,3.4) equals 1.8572.\npow(2,3) equals 8");
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
        tmp.secondElementType=type_double;
    }
    else if (m_funcName=="MOD" )
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("Double");
        tmp.secondElementLabel=i18n("Double");
        tmp1=i18n("The MOD() function returns the remainder\n"
                "after division if the second parameter is\n"
                "null the function returns #DIV/0\n");
        tmp1+=i18n("Syntax : %1(Double,Double)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("MOD(12,5) returns 2\n MOD(5,5) returns 0");
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
        tmp.secondElementType=type_double;
    }
    else if (m_funcName=="sign")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Double");
        QString tmp1=i18n("This function returns :\n"
                          "-1 if the number is negative\n"
                          "0 if the number is null\n"
                          "and 1 if the number is positive.\n");
        tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("sign(5) equals 5.\nsign(-5) equals -1.\nsign(0) equals 0.\n");
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
    }
    else if (m_funcName=="INV")
    {
        tmp.nb_param=1;
        tmp.firstElementLabel=i18n("Double");
        QString tmp1=i18n("This function multiplies each value by -1\n");
        tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("INV(-5) equals 5.\nINV(-5) equals 5.\nINV(0) equals 0.\n");
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
    }


    else if (m_funcName=="atan2" )
    {
        tmp.nb_param=2;
        tmp.firstElementLabel=i18n("Double");
        tmp.secondElementLabel=i18n("Double");
        QString tmp1=i18n("This function calculates the arc tangent\n"
                          "of the two variables x and y.\n"
                          "It is similar to calculating the arc tangent of y/x,\n"
                          "except that the signs of both arguments\n"
                          "are used to determine the quadrant of the result.\n");
        tmp1+=i18n("Syntax : %1(Double)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        tmp1+=i18n("atan2(0.5,1.0) equals 1.107149.\natan2(-0.5,2.0) equals 1.815775.");
        tmp.help=tmp1;
        tmp.firstElementType=type_double;
        tmp.secondElementType=type_double;
    }
    else if (m_funcName=="DECBIN" ||m_funcName=="DECHEX" || m_funcName=="DECOCT")
    {
        tmp.nb_param=1;
        if(m_funcName=="DECBIN")
               tmp1=i18n("This function converts a value \n"
                "from decimal format to binary format.(base 2)\n");
        else if( m_funcName=="DECHEX")
               tmp1=i18n("This function converts a value \n"
                "from decimal format to hexadecimal format.(base 16)\n");
        else if( m_funcName=="DECOCT")
               tmp1=i18n("This function converts a value \n"
                "from decimal format to octal format.(base 8)\n");

        tmp.firstElementLabel=i18n("Int");
        tmp1+=i18n("Syntax : %1(Int)\n").arg(m_funcName);
        tmp1+=i18n("Example : \n");
        if(m_funcName=="DECBIN")
               tmp1+=i18n("DECBIN(12) returns 1100 \nDECBIN(55) returns 110111.");
        else if( m_funcName=="DECHEX")
                tmp1+=i18n("DECHEX(12) returns c \nDECHEX(55) returns 37.");
        else if( m_funcName=="DECOCT")
                tmp1+=i18n("DECOCT(12) returns 14 \nDECOCT(55) returns 67.");
        tmp.firstElementType=type_int;
        tmp.help=tmp1;
    }

    else
    {
        tmp.nb_param=0;
        tmp.help="Problem";
    }

    funct=tmp;
}

#include "kspread_dlg_formula.moc"
