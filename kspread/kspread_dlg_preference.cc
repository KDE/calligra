/* This file is part of the KDE project
   Copyright (C) 1999, 2000 Montel Laurent <lmontel@mandrakesoft.com>

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
#include "kspread_canvas.h"
#include "kspread_tabbar.h"
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <kdialogbase.h>
#include <qvbox.h>
#include <qlabel.h>
#include <kconfig.h>
#include <kcolorbutton.h>
#include <qgrid.h>

KSpreadpreference::KSpreadpreference( KSpreadView* parent, const char* /*name*/)
	: KDialogBase(KDialogBase::IconList,
                                    i18n("Configure Kspread") ,
                                    KDialogBase::Ok | KDialogBase::Cancel| KDialogBase::Default,
                                    KDialogBase::Ok)

{
  m_pView=parent;
  QVBox *page=addVBoxPage(i18n("Preference"), QString::null,BarIcon("looknfeel",KIcon::SizeMedium));

  _preferenceConfig = new  preference(parent,page );
  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));

  page=addVBoxPage(i18n("Local Parameters"), QString::null,BarIcon("gohome",KIcon::SizeMedium));
  parameterLocale *_ParamLocal = new  parameterLocale(parent,page );
  page=addVBoxPage(i18n("Interface"), QString::null,BarIcon("colorize", KIcon::SizeMedium) );
  _configure = new  configure(parent,page );
  page=addVBoxPage(i18n("Misc"), QString::null,BarIcon("misc",KIcon::SizeMedium) );
  _miscParameter = new  miscParameters(parent,page );

  page=addVBoxPage(i18n("Color"), QString::null,BarIcon("misc",KIcon::SizeMedium) );
  _colorParameter=new colorParameters(parent,page );
}

void KSpreadpreference::slotApply()
{
_preferenceConfig->apply();
_configure->apply();
_miscParameter->apply();
_colorParameter->apply();
m_pView->doc()->refreshInterface();
}

void KSpreadpreference::slotDefault()
{
_configure->slotDefault();
_miscParameter->slotDefault();
_colorParameter->slotDefault();
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


void preference::slotDefault()
{
//todo
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
  label->setText( i18n("Language: %1").arg( _view->doc()->locale()->language() ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label6");
  label->setText( i18n("Number: %1").arg( _view->doc()->locale()->formatNumber(12.55) ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label1");
  label->setText( i18n("Date: %1").arg( _view->doc()->locale()->formatDate(QDate(2000,10,23)) ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label5");
  label->setText( i18n("Short date: %1").arg( _view->doc()->locale()->formatDate(QDate(2000,10,23),true) ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label2");
  label->setText( i18n("Time: %1").arg( _view->doc()->locale()->formatTime(QTime(15,10,53)) ));
  lay1->addWidget(label);
  label=new QLabel( tmpQGroupBox,"label3");
  label->setText( i18n("Money: %1").arg( _view->doc()->locale()->formatMoney(12.55) ));
  lay1->addWidget(label);
  box->addWidget( tmpQGroupBox);
}


configure::configure( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name)
 {
  m_pView = _view;
  bool vertical=true;
  bool horizontal=true;
  bool rowHeader=true;
  bool colHeader=true;
  bool tabbar=true;
  QVBoxLayout *box = new QVBoxLayout( this );
  box->setMargin( 5 );
  box->setSpacing( 10 );

  QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
  tmpQGroupBox->setTitle(i18n("Configuration"));
  QVBoxLayout *lay1 = new QVBoxLayout(tmpQGroupBox);
  lay1->setMargin( 20 );
  lay1->setSpacing( 10 );
  config = KSpreadFactory::global()->config();
  int _page=1;
  if( config->hasGroup("Parameters" ))
        {
        config->setGroup( "Parameters" );
        _page=config->readNumEntry( "NbPage" ,1) ;
        horizontal=config->readBoolEntry("Horiz ScrollBar",true);
        vertical=config->readBoolEntry("Vert ScrollBar",true);
        colHeader=config->readBoolEntry("Column Header",true);
        rowHeader=config->readBoolEntry("Row Header",true);
	tabbar=config->readBoolEntry("Tabbar",true);
        }

  nbPage=new KIntNumInput(_page, tmpQGroupBox , 10);
  nbPage->setRange(1, 10, 1);
  nbPage->setLabel(i18n("Number of pages open at the beginning:"));
  lay1->addWidget(nbPage);

  showVScrollBar=new QCheckBox(i18n("Show vertical scrollbar"),tmpQGroupBox);
  lay1->addWidget(showVScrollBar);
  showVScrollBar->setChecked(vertical);
  showHScrollBar=new QCheckBox(i18n("Show horizontal scrollbar"),tmpQGroupBox);
  lay1->addWidget(showHScrollBar);
  showHScrollBar->setChecked(horizontal);


  showColHeader=new QCheckBox(i18n("Show Column Header"),tmpQGroupBox);
  lay1->addWidget(showColHeader);
  showColHeader->setChecked(colHeader);
  showRowHeader=new QCheckBox(i18n("Show Row Header"),tmpQGroupBox);
  lay1->addWidget(showRowHeader);
  showRowHeader->setChecked(rowHeader);

  showTabBar =new QCheckBox(i18n("Show tabs"),tmpQGroupBox);
  lay1->addWidget(showTabBar);
  showTabBar->setChecked(tabbar);


  box->addWidget( tmpQGroupBox);

}


void configure::slotDefault()
{
  showHScrollBar->setChecked(true);
  showRowHeader->setChecked(true);
  showVScrollBar->setChecked(true);
  showColHeader->setChecked(true);
  showTabBar->setChecked(true);
  nbPage->setValue(1);
}


void configure::apply()
{
config->setGroup( "Parameters" );
config->writeEntry( "NbPage", nbPage->value());

if( m_pView->horzScrollBar()->isVisible()!=showHScrollBar->isChecked())
        {
        config->writeEntry( "Horiz ScrollBar", showHScrollBar->isChecked());
        if( showHScrollBar->isChecked())
                m_pView->horzScrollBar()->show();
        else
                m_pView->horzScrollBar()->hide();
        m_pView->doc()->setShowHorizontalScrollBar(showHScrollBar->isChecked());
        }
if( m_pView->vertScrollBar()->isVisible()!=showVScrollBar->isChecked())
        {
        config->writeEntry( "Vert ScrollBar", showVScrollBar->isChecked());
        if( showVScrollBar->isChecked())
                m_pView->vertScrollBar()->show();
        else
                m_pView->vertScrollBar()->hide();
        m_pView->doc()->setShowVerticalScrollBar(showVScrollBar->isChecked());

        }

if( m_pView->hBorderWidget()->isVisible()!=showColHeader->isChecked())
        {
        config->writeEntry( "Column Header", showColHeader->isChecked());
        if( showColHeader->isChecked())
                m_pView->hBorderWidget()->show();
        else
                m_pView->hBorderWidget()->hide();
        m_pView->doc()->setShowColHeader(showColHeader->isChecked());
        }

if( m_pView->vBorderWidget()->isVisible()!=showRowHeader->isChecked())
        {
        config->writeEntry( "Row Header", showRowHeader->isChecked());
        if( showRowHeader->isChecked())
                m_pView->vBorderWidget()->show();
        else
                m_pView->vBorderWidget()->hide();
        m_pView->doc()->setShowRowHeader(showRowHeader->isChecked());
        }

 if(m_pView->tabBar()->isVisible()!=showTabBar->isChecked())
   {
     config->writeEntry( "Tabbar", showTabBar->isChecked());
     if(showTabBar->isChecked())
       m_pView->tabBar()->show();
     else
       m_pView->tabBar()->hide();
     m_pView->doc()->setShowTabBar(showTabBar->isChecked());
   }
}


miscParameters::miscParameters( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name)
 {
  m_pView = _view;

  QVBoxLayout *box = new QVBoxLayout( this );
  box->setMargin( 5 );
  box->setSpacing( 10 );

  QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
  tmpQGroupBox->setTitle(i18n("Misc"));
  QVBoxLayout *lay1 = new QVBoxLayout(tmpQGroupBox);
  lay1->setMargin( 20 );
  lay1->setSpacing( 10 );
  config = KSpreadFactory::global()->config();
  int _indent=10;
  bool m_bMsgError=true;
  bool m_bCommentIndicator=true;
  if( config->hasGroup("Parameters" ))
        {
        config->setGroup( "Parameters" );
        _indent=config->readNumEntry( "Indent" ,10) ;
        m_bMsgError=config->readBoolEntry( "Msg error" ,true) ;
	m_bCommentIndicator=config->readBoolEntry( "Comment Indicator",true);
        }

  QLabel *label=new QLabel(tmpQGroupBox);
  label->setText(i18n("Completion mode:"));
  lay1->addWidget(label);

  typeCompletion=new QComboBox( tmpQGroupBox);
  QStringList listType;
  listType+=i18n("None");
  listType+=i18n("Manual");
  listType+=i18n("Popup");
  listType+=i18n("Automatic");
  listType+=i18n("Semi-Automatic");
  typeCompletion->insertStringList(listType);
  typeCompletion->setCurrentItem(0);
  lay1->addWidget(typeCompletion);


  valIndent=new KIntNumInput(_indent, tmpQGroupBox , 10);
  valIndent->setRange(1, 100, 1);
  valIndent->setLabel(i18n("Value of indent:"));
  lay1->addWidget(valIndent);

  label=new QLabel(tmpQGroupBox);
  label->setText(i18n("Press enter to move selection to:"));
  lay1->addWidget(label);
  typeOfMove=new QComboBox( tmpQGroupBox);
  listType.clear();
  listType+=i18n("Bottom");
  listType+=i18n("Top");
  listType+=i18n("Right");
  listType+=i18n("Left");
  typeOfMove->insertStringList(listType);
  typeOfMove->setCurrentItem(0);
  lay1->addWidget(typeOfMove);
  msgError= new QCheckBox(i18n("Show message of error"),tmpQGroupBox);
  msgError->setChecked(m_bMsgError);
  lay1->addWidget(msgError);

  label=new QLabel(tmpQGroupBox);
  label->setText(i18n("Method of calc:"));
  lay1->addWidget(label);

  typeCalc=new QComboBox( tmpQGroupBox);
  QStringList listTypeCalc;
  listTypeCalc+=i18n("Sum");
  listTypeCalc+=i18n("Min");
  listTypeCalc+=i18n("Max");
  listTypeCalc+=i18n("Average");
  listTypeCalc+=i18n("Count");
  typeCalc->insertStringList(listTypeCalc);
  typeCalc->setCurrentItem(0);
  lay1->addWidget(typeCalc);
  commentIndicator=new QCheckBox(i18n("Show comment indicator"),tmpQGroupBox);
  commentIndicator->setChecked(m_bCommentIndicator); 
  lay1->addWidget(commentIndicator);

  initComboBox();
  box->addWidget( tmpQGroupBox);

}

void miscParameters::initComboBox()
{
switch( m_pView->doc()->completionMode( ))
        {
        case  KGlobalSettings::CompletionNone:
                typeCompletion->setCurrentItem(0);
                break;
        case  KGlobalSettings::CompletionAuto:
                typeCompletion->setCurrentItem(3);
                break;
        case  KGlobalSettings::CompletionMan:
                typeCompletion->setCurrentItem(4);
                break;
        case  KGlobalSettings::CompletionShell:
                typeCompletion->setCurrentItem(1);
                break;
        case  KGlobalSettings::CompletionPopup:
                typeCompletion->setCurrentItem(2);
                break;
        default :
                typeCompletion->setCurrentItem(0);
                break;
        }
switch( m_pView->doc()->getMoveToValue( ))
        {
        case  Bottom:
                typeOfMove->setCurrentItem(0);
                break;
        case  Left:
                typeOfMove->setCurrentItem(3);
                break;
        case  Top:
                typeOfMove->setCurrentItem(1);
                break;
        case  Right:
                typeOfMove->setCurrentItem(2);
                break;
        default :
                typeOfMove->setCurrentItem(0);
                break;
        }

switch( m_pView->doc()->getTypeOfCalc())
        {
        case  Sum:
                typeCalc->setCurrentItem(0);
                break;
        case  Min:
                typeCalc->setCurrentItem(1);
                break;
        case  Max:
                typeCalc->setCurrentItem(2);
                break;
        case  Average:
                typeCalc->setCurrentItem(3);
                break;
        case  Count:
	        typeCalc->setCurrentItem(4);
                break;
        default :
                typeCalc->setCurrentItem(0);
                break;
        }

}

void miscParameters::slotDefault()
{
valIndent->setValue(10);
typeCompletion->setCurrentItem(3);
typeOfMove->setCurrentItem(0);
msgError->setChecked(true);
typeCalc->setCurrentItem(0);
commentIndicator->setChecked(true);
}


void miscParameters::apply()
{
config->setGroup( "Parameters" );
KGlobalSettings::Completion tmpCompletion=KGlobalSettings::CompletionNone;

switch(typeCompletion->currentItem())
        {
        case 0:
                tmpCompletion=KGlobalSettings::CompletionNone;
                break;
        case 1:
                tmpCompletion=KGlobalSettings::CompletionShell;
                break;
        case 2:
                tmpCompletion=KGlobalSettings::CompletionPopup;
                break;
        case 3:
                tmpCompletion=KGlobalSettings::CompletionAuto;
                break;
        case 4:
                tmpCompletion=KGlobalSettings::CompletionMan;
                break;
        }


if(tmpCompletion!=m_pView->doc()->completionMode())
        {
        m_pView->doc()->setCompletionMode(tmpCompletion);
        config->writeEntry( "Completion Mode", (int)tmpCompletion);
        }

MoveTo tmpMoveTo=Bottom;
switch(typeOfMove->currentItem())
        {
        case 0:
                tmpMoveTo=Bottom;
                break;
        case 1:
                tmpMoveTo=Top;
                break;
        case 2:
                tmpMoveTo=Right;
                break;
        case 3:
                tmpMoveTo=Left;
                break;
        }
if(tmpMoveTo!=m_pView->doc()->getMoveToValue())
        {
        m_pView->doc()->setMoveToValue(tmpMoveTo);
        config->writeEntry( "Move", (int)tmpMoveTo);
        }

MethodOfCalc tmpMethodCalc=Sum;
switch(typeCalc->currentItem())
        {
        case 0:
	  tmpMethodCalc =Sum;
	  break;
        case 1:
	  tmpMethodCalc=Min;
	  break;
        case 2:
	  tmpMethodCalc=Max;
	  break;
        case 3:
	  tmpMethodCalc=Average;
	  break;
	case 4:
	  tmpMethodCalc=Count;
	  break;
        }
if(tmpMethodCalc!=m_pView->doc()->getTypeOfCalc())
        {
        m_pView->doc()->setTypeOfCalc(tmpMethodCalc);
        config->writeEntry( "Method of Calc", (int)tmpMethodCalc);
	m_pView->resultOfCalc();
        }


if(valIndent->value()!=m_pView->doc()->getIndentValue())
        {
        m_pView->doc()->setIndentValue( valIndent->value());
        config->writeEntry( "Indent", valIndent->value());
        }

if(msgError->isChecked()!=m_pView->doc()->getShowMessageError())
        {
        m_pView->doc()->setShowMessageError( msgError->isChecked());
        config->writeEntry( "Msg error" ,(int)msgError->isChecked()) ;
        }
 if(commentIndicator->isChecked()!=m_pView->doc()->getShowCommentIndicator())
   {
     m_pView->doc()->setShowCommentIndicator( commentIndicator->isChecked());
     config->writeEntry( "Comment Indicator" ,(int)commentIndicator->isChecked()) ;
   }
}



colorParameters::colorParameters( KSpreadView* _view,QWidget *parent , char *name )
 :QWidget ( parent,name)
{
  m_pView = _view;
  config = KSpreadFactory::global()->config();

  QColor _gridColor(Qt::lightGray);
  if( config->hasGroup("Parameters" ))
        {
        config->setGroup( "Parameters" );
	_gridColor= config->readColorEntry("GridColor",&_gridColor);
	}

  QVBoxLayout *box = new QVBoxLayout( this );
  box->setMargin( 5 );
  box->setSpacing( 10 );

  QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
  tmpQGroupBox->setTitle(i18n("Color"));
  QGridLayout *grid1 = new QGridLayout(tmpQGroupBox,5,1,15,7);
  QLabel *lab=new QLabel( tmpQGroupBox,"label20");
  lab->setText( i18n("Grid Color:"));
  grid1->addWidget(lab,0,0);

  gridColor=new KColorButton(tmpQGroupBox);
  gridColor->setColor(_gridColor);
  grid1->addWidget(gridColor,1,0);

  box->addWidget( tmpQGroupBox);
}

void colorParameters::apply()
{
    QColor _col=gridColor->color();
   
    if(m_pView->doc()->defaultGridPen().color()!=_col)
        {
	 m_pView->doc()->changeDefaultGridPenColor( _col);
	 config->setGroup( "Parameters" );
	 config->writeEntry("GridColor",_col);
	}

}

void colorParameters::slotDefault()
{
    gridColor->setColor(lightGray);
}


#include "kspread_dlg_preference.moc"
