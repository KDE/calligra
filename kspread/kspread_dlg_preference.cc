/* This file is part of the KDE project
   Copyright (C) 1999, 2000, 2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <qvbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qvgroupbox.h>

#include "kspread_dlg_preference.h"
#include "kspread_table.h"
#include "kspread_doc.h"
#include "kspread_canvas.h"
#include "kspread_tabbar.h"

#include <kconfig.h>
#include <kstatusbar.h>
#include <knuminput.h>
#include <kspell.h>

KSpreadpreference::KSpreadpreference( KSpreadView* parent, const char* /*name*/)
  : KDialogBase(KDialogBase::IconList,i18n("Configure KSpread") ,
		KDialogBase::Ok | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
  m_pView=parent;
  QVBox *page=addVBoxPage(i18n("Preferences"), QString::null,BarIcon("looknfeel",KIcon::SizeMedium));

  _preferenceConfig = new  preference(parent,page );
  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));

  QVBox *page2=addVBoxPage(i18n("Locale Parameters"), QString::null,BarIcon("gohome",KIcon::SizeMedium));
 _localePage=new parameterLocale(parent,page2 );

  QVBox *page3=addVBoxPage(i18n("Interface"), QString::null,BarIcon("signature", KIcon::SizeMedium) );
  _configure = new  configure(parent,page3 );

  QVBox * page4=addVBoxPage(i18n("Misc"), QString::null,BarIcon("misc",KIcon::SizeMedium) );
  _miscParameter = new  miscParameters(parent,page4 );

  QVBox *page5=addVBoxPage(i18n("Color"), QString::null,BarIcon("colorize",KIcon::SizeMedium) );
  _colorParameter=new colorParameters(parent,page5 );

  QVBox *page6=addVBoxPage(i18n("Page layout"), QString::null,BarIcon("edit",KIcon::SizeMedium) );
  _layoutPage=new configureLayoutPage(parent,page6 );

  QVBox *page7 = addVBoxPage( i18n("Spelling"), i18n("Spell checker behavior"),
                          BarIcon("spellcheck", KIcon::SizeMedium) );
  _spellPage=new configureSpellPage(parent,page7);

}

void KSpreadpreference::openPage(int flags)
{
    if(flags & KS_PREFERENCES)
        showPage( 0 );
    else if(flags & KS_LOCALE)
        showPage( 1 );
    else if(flags & KS_INTERFACE)
        showPage( 2 );
    else if(flags & KS_MISC)
        showPage( 3 );
    else if(flags & KS_COLOR)
        showPage( 4 );
    else if(flags & KS_LAYOUT)
        showPage( 5 );
    else if(flags & KS_SPELLING)
        showPage( 6 );
}

void KSpreadpreference::slotApply()
{
  _preferenceConfig->apply();
  _configure->apply();
  _miscParameter->apply();
  _colorParameter->apply();
  _layoutPage->apply();
  _spellPage->apply();
  _localePage->apply();
  m_pView->doc()->refreshInterface();
}

void KSpreadpreference::slotDefault()
{
    switch(activePageIndex())
    {
        case 0:
            _preferenceConfig->slotDefault();
            break;
        case 1:
            break;
        case 2:
            _configure->slotDefault();
            break;
        case 3:
            _miscParameter->slotDefault();
            break;
        case 4:
            _colorParameter->slotDefault();
            break;
        case 5:
            _layoutPage->slotDefault();
            break;
        case 6:
            _spellPage->slotDefault();
            break;
        default:
            break;
    }
}

preference::preference( KSpreadView* _view, QVBox *box, char *name )
 :QObject ( box->parent(),name)
 {

  m_pView = _view;

  QGroupBox* tmpQGroupBox = new QVGroupBox( i18n("Table"), box, "GroupBox" );

  m_pFormula= new QCheckBox(i18n("Show &formula"),tmpQGroupBox);
  m_pFormula->setChecked(m_pView->activeTable()->getShowFormula());

  m_pGrid=new QCheckBox( i18n("Show &grid"), tmpQGroupBox );
  m_pGrid->setChecked( m_pView->activeTable()->getShowGrid() );

  m_pPrintGrid=new QCheckBox( i18n("&Print grid"), tmpQGroupBox );
  m_pPrintGrid->setChecked( m_pView->activeTable()->getPrintGrid() );

  m_pColumn=new QCheckBox(i18n("Show c&olumn number"),tmpQGroupBox);
  m_pColumn->setChecked(m_pView->activeTable()->getShowColumnNumber());

  m_pLcMode=new QCheckBox(i18n("&LC mode"),tmpQGroupBox);
  m_pLcMode->setChecked(m_pView->activeTable()->getLcMode());

  m_pAutoCalc= new QCheckBox(i18n("&Automatic recalculation"),tmpQGroupBox);
  m_pAutoCalc->setChecked(m_pView->activeTable()->getAutoCalc());

  m_pHideZero= new QCheckBox(i18n("&Hide zero"),tmpQGroupBox);
  m_pHideZero->setChecked(m_pView->activeTable()->getHideZero());

  m_pFirstLetterUpper= new QCheckBox(i18n("Convert first letter to &upper case"),tmpQGroupBox);
  m_pFirstLetterUpper->setChecked(m_pView->activeTable()->getFirstLetterUpper());

}


void preference::slotDefault()
{
  m_pFormula->setChecked(false);
  m_pAutoCalc->setChecked(true);
  m_pGrid->setChecked(true);
  m_pPrintGrid->setChecked(true);
  m_pColumn->setChecked(false);
  m_pLcMode->setChecked(false);
  m_pHideZero->setChecked(false);
  m_pFirstLetterUpper->setChecked(false);
}

void preference::apply()
{
  if(m_pView->activeTable()->getLcMode()==m_pLcMode->isChecked()
  && m_pView->activeTable()->getShowColumnNumber()==m_pColumn->isChecked()
  && m_pView->activeTable()->getShowFormula()==m_pFormula->isChecked()
  && m_pView->activeTable()->getAutoCalc()==m_pAutoCalc->isChecked()
  && m_pView->activeTable()->getShowGrid()==m_pGrid->isChecked()
  && m_pView->activeTable()->getPrintGrid()==m_pPrintGrid->isChecked()
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
        m_pView->activeTable()->setPrintGrid(m_pPrintGrid->isChecked());
        m_pView->activeTable()->setShowFormula(m_pFormula->isChecked());
        m_pView->activeTable()->setAutoCalc(m_pAutoCalc->isChecked());
        m_pView->activeTable()->setHideZero(m_pHideZero->isChecked());
        m_pView->activeTable()->setFirstLetterUpper(m_pFirstLetterUpper->isChecked());
  }
}

parameterLocale::parameterLocale( KSpreadView* _view, QVBox *box , char *name )
 :QObject ( box->parent(),name)
{
    m_pView = _view;
    m_bUpdateLocale=false;
  QGroupBox* tmpQGroupBox = new QVGroupBox( i18n("Parameters"), box, "GroupBox" );

  KLocale* locale=_view->doc()->locale();

  m_language=new QLabel( tmpQGroupBox,"label");
  m_language->setText( i18n("Language: %1").arg( locale->language() ));
  m_number=new QLabel( tmpQGroupBox,"label6");
  m_number->setText( i18n("Number: %1").arg( locale->formatNumber(12.55) ));
  m_date=new QLabel( tmpQGroupBox,"label1");
  m_date->setText( i18n("Date: %1").arg( locale->formatDate(QDate(2000,10,23)) ));
  m_shortDate=new QLabel( tmpQGroupBox,"label5");
  m_shortDate->setText( i18n("Short date: %1").arg( locale->formatDate(QDate(2000,10,23),true) ));
  m_time=new QLabel( tmpQGroupBox,"label2");
  m_time->setText( i18n("Time: %1").arg( locale->formatTime(QTime(15,10,53)) ));
  m_money=new QLabel( tmpQGroupBox,"label3");
  m_money->setText( i18n("Money: %1").arg( locale->formatMoney(12.55) ));

  m_updateButton=new QPushButton ( i18n("&Update to Locale System"), tmpQGroupBox);
  connect(m_updateButton, SIGNAL(clicked()),this,SLOT(updateDefaultSystemConfig()));
}

void parameterLocale::apply()
{
    if (m_bUpdateLocale)
        m_pView->doc()->refreshLocale();
}

void parameterLocale::updateDefaultSystemConfig()
{
    m_bUpdateLocale=true;
    static_cast<KSpreadLocale*>(m_pView->doc()->locale())->defaultSystemConfig( );
    KLocale* locale=m_pView->doc()->locale();
    m_money->setText( i18n("Money: %1").arg( locale->formatMoney(12.55) ));
    m_time->setText( i18n("Time: %1").arg( locale->formatTime(QTime(15,10,53)) ));
    m_shortDate->setText( i18n("Short date: %1").arg( locale->formatDate(QDate(2000,10,23),true) ));
    m_date->setText( i18n("Date: %1").arg( locale->formatDate(QDate(2000,10,23)) ));
    m_number->setText( i18n("Number: %1").arg( locale->formatNumber(12.55) ));
    m_language->setText( i18n("Language: %1").arg( locale->language() ));
}

configure::configure( KSpreadView* _view, QVBox *box , char *name )
 :QObject ( box->parent(),name)
 {
  m_pView = _view;

  bool vertical=true;
  bool horizontal=true;
  bool rowHeader=true;
  bool colHeader=true;
  bool tabbar=true;
  bool formulaBar=true;
  bool statusBar=true;

  QGroupBox* tmpQGroupBox = new QVGroupBox( i18n("Configuration"), box, "GroupBox" );

  config = KSpreadFactory::global()->config();
  int _page=1;

  oldRecent=10;
  oldAutoSaveValue=KoDocument::defaultAutoSave()/60;

  if( config->hasGroup("Parameters" ))
        {
        config->setGroup( "Parameters" );
        _page=config->readNumEntry( "NbPage" ,1) ;
        horizontal=config->readBoolEntry("Horiz ScrollBar",true);
        vertical=config->readBoolEntry("Vert ScrollBar",true);
        colHeader=config->readBoolEntry("Column Header",true);
        rowHeader=config->readBoolEntry("Row Header",true);
	tabbar=config->readBoolEntry("Tabbar",true);
	formulaBar=config->readBoolEntry("Formula bar",true);
        statusBar=config->readBoolEntry("Status bar",true);
        oldRecent=config->readNumEntry( "NbRecentFile" ,10);
        oldAutoSaveValue=config->readNumEntry("AutoSave",KoDocument::defaultAutoSave()/60);
        }
  nbPage=new KIntNumInput(_page, tmpQGroupBox , 10);
  nbPage->setRange(1, 10, 1);
  nbPage->setLabel(i18n("Number of pages open at the &beginning:"));

  nbRecentFile=new KIntNumInput(oldRecent, tmpQGroupBox , 10);
  nbRecentFile->setRange(1, 20, 1);
  nbRecentFile->setLabel(i18n("&Number of recent file:"));

  autoSaveDelay=new KIntNumInput(oldAutoSaveValue, tmpQGroupBox , 10);
  autoSaveDelay->setRange(0, 60, 1);
  autoSaveDelay->setLabel(i18n("Au&to save (min):"));
  autoSaveDelay->setSpecialValueText(i18n("No auto save"));
  autoSaveDelay->setSuffix(i18n("min"));

  showVScrollBar=new QCheckBox(i18n("Show &vertical scrollbar"),tmpQGroupBox);
  showVScrollBar->setChecked(vertical);
  showHScrollBar=new QCheckBox(i18n("Show &horizontal scrollbar"),tmpQGroupBox);
  showHScrollBar->setChecked(horizontal);


  showColHeader=new QCheckBox(i18n("Show c&olumn header"),tmpQGroupBox);
  showColHeader->setChecked(colHeader);
  showRowHeader=new QCheckBox(i18n("Show &row header"),tmpQGroupBox);
  showRowHeader->setChecked(rowHeader);

  showTabBar =new QCheckBox(i18n("Show ta&bs"),tmpQGroupBox);
  showTabBar->setChecked(tabbar);

  showFormulaBar =new QCheckBox(i18n("Sho&w formula toolbar"),tmpQGroupBox);
  showFormulaBar->setChecked(formulaBar);

  showStatusBar =new QCheckBox(i18n("Show stat&usbar"),tmpQGroupBox);
  showStatusBar->setChecked(statusBar);
}


void configure::slotDefault()
{
  showHScrollBar->setChecked(true);
  showRowHeader->setChecked(true);
  showVScrollBar->setChecked(true);
  showColHeader->setChecked(true);
  showTabBar->setChecked(true);
  showFormulaBar->setChecked(true);
  showStatusBar->setChecked(true);
  nbPage->setValue(1);
  nbRecentFile->setValue(10);
  autoSaveDelay->setValue(KoDocument::defaultAutoSave()/60);
}


void configure::apply()
{
    config->setGroup( "Parameters" );
    config->writeEntry( "NbPage", nbPage->value());
    bool active=true;
    active=showHScrollBar->isChecked();
    if( m_pView->horzScrollBar()->isVisible()!=active)
    {
        config->writeEntry( "Horiz ScrollBar",active);
        if( active)
            m_pView->horzScrollBar()->show();
        else
            m_pView->horzScrollBar()->hide();
        m_pView->doc()->setShowHorizontalScrollBar(active);
    }
    active=showVScrollBar->isChecked();
    if( m_pView->vertScrollBar()->isVisible()!=active)
    {
        config->writeEntry( "Vert ScrollBar", active);
        if(active)
            m_pView->vertScrollBar()->show();
        else
            m_pView->vertScrollBar()->hide();
        m_pView->doc()->setShowVerticalScrollBar(active);

    }
    active=showColHeader->isChecked();
    if( m_pView->hBorderWidget()->isVisible()!=active)
    {
        config->writeEntry( "Column Header", active);
        if( active)
            m_pView->hBorderWidget()->show();
        else
            m_pView->hBorderWidget()->hide();
        m_pView->doc()->setShowColHeader(active);
    }

    active=showRowHeader->isChecked();
    if( m_pView->vBorderWidget()->isVisible()!=active)
    {
        config->writeEntry( "Row Header", active);
        if( active)
            m_pView->vBorderWidget()->show();
        else
            m_pView->vBorderWidget()->hide();
        m_pView->doc()->setShowRowHeader(active);
    }

    active=showTabBar->isChecked();
    if(m_pView->tabBar()->isVisible()!=active)
    {
        config->writeEntry( "Tabbar", active);
        if(active)
            m_pView->tabBar()->show();
        else
            m_pView->tabBar()->hide();
        m_pView->doc()->setShowTabBar(active);
    }

    active=showFormulaBar->isChecked();
    if(m_pView->posWidget()->isVisible()!=active)
    {
        config->writeEntry( "Formula bar",active);
        m_pView->editWidget()->showEditWidget(active);
        if(active)
            m_pView->posWidget()->show();
        else
            m_pView->posWidget()->hide();
        m_pView->doc()->setShowFormulaBar(active);
    }
    active=showStatusBar->isChecked();
    if(m_pView->statusBar() && m_pView->statusBar()->isVisible()!=active)
    {
        config->writeEntry( "Status bar",active);
        if(active)
            m_pView->statusBar()->show();
        else
            m_pView->statusBar()->hide();
        m_pView->doc()->setShowStatusBar(active);
    }
    int val=nbRecentFile->value();
    if( oldRecent!= val)
    {
       config->writeEntry( "NbRecentFile",val);
       m_pView->changeNbOfRecentFiles(val);
    }
    val=autoSaveDelay->value();
    if(val!=oldAutoSaveValue)
    {
        config->writeEntry( "AutoSave", val );
        m_pView->doc()->setAutoSave(val*60);
    }
}


miscParameters::miscParameters( KSpreadView* _view,QVBox *box, char *name )
 :QObject ( box->parent(),name)
 {
  m_pView = _view;


  QGroupBox* tmpQGroupBox = new QVGroupBox( i18n("Misc"), box, "GroupBox" );

  config = KSpreadFactory::global()->config();
  int _indent=10;
  bool m_bMsgError=false;
  bool m_bCommentIndicator=true;
  if( config->hasGroup("Parameters" ))
        {
        config->setGroup( "Parameters" );
        _indent=config->readNumEntry( "Indent" ,10) ;
        m_bMsgError=config->readBoolEntry( "Msg error" ,false) ;
	m_bCommentIndicator=config->readBoolEntry( "Comment Indicator",true);
        }

  QLabel *label=new QLabel(i18n("&Completion mode:"), tmpQGroupBox);

  typeCompletion=new QComboBox(tmpQGroupBox);
  label->setBuddy(typeCompletion);
  QStringList listType;
  listType+=i18n("None");
  listType+=i18n("Manual");
  listType+=i18n("Popup");
  listType+=i18n("Automatic");
  listType+=i18n("Semi-Automatic");
  typeCompletion->insertStringList(listType);
  typeCompletion->setCurrentItem(0);
  comboChanged=false;
  connect(typeCompletion,SIGNAL(activated( const QString & )),this,SLOT(slotTextComboChanged(const QString &)));

  valIndent=new KIntNumInput(_indent, tmpQGroupBox , 10);
  valIndent->setRange(1, 100, 1);
  valIndent->setLabel(i18n("&Value of indent:"));

  label=new QLabel(i18n("&Press enter to move selection to:"), tmpQGroupBox);
  typeOfMove=new QComboBox( tmpQGroupBox);
  label->setBuddy(typeOfMove);
  listType.clear();
  listType+=i18n("Bottom");
  listType+=i18n("Top");
  listType+=i18n("Right");
  listType+=i18n("Left");
  typeOfMove->insertStringList(listType);
  typeOfMove->setCurrentItem(0);
  msgError= new QCheckBox(i18n("&Show error message"),tmpQGroupBox);
  msgError->setChecked(m_bMsgError);

  label=new QLabel(i18n("&Method of calc:"), tmpQGroupBox);

  typeCalc=new QComboBox( tmpQGroupBox);
  label->setBuddy(typeCalc);
  QStringList listTypeCalc;
  listTypeCalc+=i18n("Sum");
  listTypeCalc+=i18n("Min");
  listTypeCalc+=i18n("Max");
  listTypeCalc+=i18n("Average");
  listTypeCalc+=i18n("Count");
  listTypeCalc+=i18n("None");
  typeCalc->insertStringList(listTypeCalc);
  typeCalc->setCurrentItem(0);
  commentIndicator=new QCheckBox(i18n("Show comment &indicator"),tmpQGroupBox);
  commentIndicator->setChecked(m_bCommentIndicator);

  initComboBox();
}

void miscParameters::slotTextComboChanged(const QString &)
{
  comboChanged=true;
}

void miscParameters::initComboBox()
{
  KGlobalSettings::Completion tmpCompletion=KGlobalSettings::CompletionAuto;
  if( config->hasGroup("Parameters" ))
    {
      config->setGroup( "Parameters" );
      tmpCompletion=( KGlobalSettings::Completion)config->readNumEntry( "Completion Mode" ,KGlobalSettings::CompletionAuto) ;
      config->writeEntry( "Completion Mode", (int)tmpCompletion);
    }
switch(tmpCompletion )
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
        case  KSpread::Bottom:
                typeOfMove->setCurrentItem(0);
                break;
        case  KSpread::Left:
                typeOfMove->setCurrentItem(3);
                break;
        case  KSpread::Top:
                typeOfMove->setCurrentItem(1);
                break;
        case  KSpread::Right:
                typeOfMove->setCurrentItem(2);
                break;
        default :
                typeOfMove->setCurrentItem(0);
                break;
        }

switch( m_pView->doc()->getTypeOfCalc())
        {
        case  SumOfNumber:
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
        case  NoneCalc:
	        typeCalc->setCurrentItem(5);
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
  msgError->setChecked(false);
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


    if(comboChanged)
    {
        m_pView->doc()->setCompletionMode(tmpCompletion);
        config->writeEntry( "Completion Mode", (int)tmpCompletion);
    }

    KSpread::MoveTo tmpMoveTo=KSpread::Bottom;
    switch(typeOfMove->currentItem())
    {
        case 0:
            tmpMoveTo=KSpread::Bottom;
            break;
        case 1:
            tmpMoveTo=KSpread::Top;
            break;
        case 2:
            tmpMoveTo=KSpread::Right;
            break;
        case 3:
            tmpMoveTo=KSpread::Left;
            break;
    }
    if(tmpMoveTo!=m_pView->doc()->getMoveToValue())
    {
        m_pView->doc()->setMoveToValue(tmpMoveTo);
        config->writeEntry( "Move", (int)tmpMoveTo);
    }

    MethodOfCalc tmpMethodCalc=SumOfNumber;
    switch(typeCalc->currentItem())
    {
        case 0:
            tmpMethodCalc =SumOfNumber;
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
        case 5:
            tmpMethodCalc=NoneCalc;
            break;

    }
    if(tmpMethodCalc!=m_pView->doc()->getTypeOfCalc())
    {
        m_pView->doc()->setTypeOfCalc(tmpMethodCalc);
        config->writeEntry( "Method of Calc", (int)tmpMethodCalc);
	m_pView->resultOfCalc();
        m_pView->initCalcMenu();
    }

    int val=valIndent->value();
    if(val!=m_pView->doc()->getIndentValue())
    {
        m_pView->doc()->setIndentValue( val);
        config->writeEntry( "Indent", val);
    }

    bool active=msgError->isChecked();
    if(active!=m_pView->doc()->getShowMessageError())
    {
        m_pView->doc()->setShowMessageError( active);
        config->writeEntry( "Msg error" ,(int)active);
    }

    active=commentIndicator->isChecked();
    if(active!=m_pView->doc()->getShowCommentIndicator())
    {
        m_pView->doc()->setShowCommentIndicator( active);
        config->writeEntry( "Comment Indicator" ,(int)active);
    }
}



colorParameters::colorParameters( KSpreadView* _view,QVBox *box , char *name )
 :QObject ( box->parent(),name)
{
  m_pView = _view;
  config = KSpreadFactory::global()->config();

  QColor _gridColor(Qt::lightGray);

  if ( config->hasGroup("KSpread Color" ) )
  {
    config->setGroup( "KSpread Color" );
    _gridColor = config->readColorEntry("GridColor",&_gridColor);
  }

  QGroupBox* tmpQGroupBox = new QVGroupBox( i18n("Color"), box, "GroupBox" );

  QLabel *label = new QLabel(i18n("&Grid Color:"), tmpQGroupBox,"label20" );

  gridColor = new KColorButton( tmpQGroupBox );
  label->setBuddy(gridColor);
  gridColor->setColor(_gridColor);

  QColor _pbColor(Qt::red);
  if ( config->hasGroup("KSpread Color" ) )
  {
    config->setGroup( "KSpread Color" );
    _pbColor = config->readColorEntry("PageBorderColor", &_pbColor);
  }

  QLabel * label2 = new QLabel( i18n("&Page Borders:"), tmpQGroupBox, "label21" );
  pageBorderColor = new KColorButton( tmpQGroupBox );
  label2->setBuddy(pageBorderColor);
  pageBorderColor->setColor(_pbColor);
}

void colorParameters::apply()
{
  QColor _col = gridColor->color();
  if ( m_pView->doc()->defaultGridPen().color() != _col )
  {
    m_pView->doc()->changeDefaultGridPenColor( _col );
    config->setGroup( "KSpread Color" );
    config->writeEntry( "GridColor", _col );
  }

  QColor _pbColor = pageBorderColor->color();
  if ( m_pView->doc()->pageBorderColor() != _pbColor )
  {
    m_pView->doc()->changePageBorderColor( _pbColor );
    config->setGroup( "KSpread Color" );
    config->writeEntry( "PageBorderColor", _pbColor );
  }
}

void colorParameters::slotDefault()
{
  gridColor->setColor( lightGray );
  pageBorderColor->setColor( red );
}



configureLayoutPage::configureLayoutPage( KSpreadView* _view,QVBox *box , char *name )
 :QObject ( box->parent(),name)
{
  m_pView = _view;

  QGroupBox* tmpQGroupBox = new QGroupBox( i18n("Default Parameters"), box, "GroupBox" );

  QGridLayout *grid1 = new QGridLayout(tmpQGroupBox,8,1, KDialog::marginHint()+10, KDialog::spacingHint());
  grid1->addRowSpacing( 0, KDialog::marginHint()  );
  grid1->setRowStretch( 7, 10 );

  config = KSpreadFactory::global()->config();

  QLabel *label=new QLabel(i18n("Default page &size:"), tmpQGroupBox);

  grid1->addWidget(label,0,0);

  defaultSizePage=new QComboBox( tmpQGroupBox);
  label->setBuddy(defaultSizePage);
  defaultSizePage->insertStringList( KoPageFormat::allFormats() );
  defaultSizePage->setCurrentItem(1);
  grid1->addWidget(defaultSizePage,1,0);

  label=new QLabel(i18n("Default page &orientation:"), tmpQGroupBox);
  grid1->addWidget(label,2,0);

  defaultOrientationPage=new QComboBox( tmpQGroupBox);
  label->setBuddy(defaultOrientationPage);
  QStringList listType;
  listType+=i18n( "Portrait" );
  listType+=i18n( "Landscape" );
  defaultOrientationPage->insertStringList(listType);
  defaultOrientationPage->setCurrentItem(0);
  grid1->addWidget(defaultOrientationPage,3,0);

  label=new QLabel(tmpQGroupBox);
  label->setText(i18n("Default page &units:"));
  grid1->addWidget(label,4,0);
  defaultUnit=new QComboBox( tmpQGroupBox);
  label->setBuddy(defaultUnit);
  listType.clear();
  listType=i18n( "Millimeters (mm)" ) ;
  listType+=i18n( "Points (pt)" ) ;
  listType+=i18n( "Inches (in)" );
  defaultUnit->insertStringList(listType);
  defaultUnit->setCurrentItem(0);
  grid1->addWidget(defaultUnit,5,0);
  initCombo();

}

void configureLayoutPage::slotDefault()
{
  defaultSizePage->setCurrentItem(1);
  defaultOrientationPage->setCurrentItem(0);
  defaultUnit->setCurrentItem(0);
}

void configureLayoutPage::initCombo()
{
  paper=1;
  orientation=0;
  unit=0;
  if( config->hasGroup("KSpread Page Layout" ))
    {
      config->setGroup( "KSpread Page Layout" );
      paper=config->readNumEntry( "Default size page" ,1);
      orientation=config->readNumEntry( "Default orientation page" ,0);
      unit=config->readNumEntry( "Default unit page" ,0);
    }
  defaultSizePage->setCurrentItem(paper);
  defaultOrientationPage->setCurrentItem(orientation);
  defaultUnit->setCurrentItem(unit);
}


void configureLayoutPage::apply()
{
  config->setGroup( "KSpread Page Layout" );

 if(paper!=defaultSizePage->currentItem())
   {
     unsigned int sizePage=defaultSizePage->currentItem();
     config->writeEntry( "Default size page", sizePage);
     m_pView->activeTable()->setPaperFormat((KoFormat)sizePage);
   }
 if(orientation!=defaultOrientationPage->currentItem())
   {
     unsigned int orientationPage=defaultOrientationPage->currentItem();
     config->writeEntry( "Default orientation page", orientationPage);
     m_pView->activeTable()->setPaperOrientation((KoOrientation)orientationPage);
   }
 if(unit!=defaultUnit->currentItem())
   {
     unsigned int unitPage=defaultUnit->currentItem();
     config->writeEntry( "Default unit page", unitPage);
     m_pView->doc()->setUnit((KoUnit::Unit)unitPage);
   }
}

configureSpellPage::configureSpellPage( KSpreadView* _view,QVBox *box , char *name )
 :QObject ( box->parent(),name)
{
  m_pView = _view;

  config = KSpreadFactory::global()->config();
  QGroupBox* tmpQGroupBox = new QGroupBox( i18n("Spelling"), box, "GroupBox" );
  QGridLayout *grid1 = new QGridLayout(tmpQGroupBox,4,1, KDialog::marginHint()+10, KDialog::spacingHint());
  grid1->addRowSpacing( 0, KDialog::marginHint() + 5 );
  grid1->setRowStretch( 4, 10 );

  _spellConfig  = new KSpellConfig(tmpQGroupBox, 0L ,m_pView->doc()->getKSpellConfig(), false );
  grid1->addWidget(_spellConfig,1,0);

  m_dontCheckUpperWord= new QCheckBox(i18n("Ignore uppercase words"),tmpQGroupBox);
  grid1->addWidget(m_dontCheckUpperWord,2,0);

  m_dontCheckTitleCase= new QCheckBox(i18n("Ignore title case words"),tmpQGroupBox);
  grid1->addWidget(m_dontCheckTitleCase,3,0);

  clearIgnoreAllHistory= new QPushButton( i18n("Clear Ignore All Word History"),tmpQGroupBox);
  grid1->addMultiCellWidget(clearIgnoreAllHistory,5,5,0,1);
  connect( clearIgnoreAllHistory, SIGNAL(clicked()),this, SLOT(slotClearIgnoreAllHistory()));


  if( config->hasGroup("KSpell kspread") )
  {
    config->setGroup( "KSpell kspread" );
    m_dontCheckUpperWord->setChecked(config->readBoolEntry("KSpell_IgnoreUppercaseWords", false));
    m_dontCheckTitleCase->setChecked(config->readBoolEntry("KSpell_IgnoreTitleCaseWords", false));
  }
}

void configureSpellPage::slotClearIgnoreAllHistory()
{
    m_pView->doc()->clearIgnoreWordAll();
}

void configureSpellPage::apply()
{
  config->setGroup( "KSpell kspread" );
  config->writeEntry ("KSpell_NoRootAffix",(int) _spellConfig->noRootAffix ());
  config->writeEntry ("KSpell_RunTogether", (int) _spellConfig->runTogether ());
  config->writeEntry ("KSpell_Dictionary", _spellConfig->dictionary ());
  config->writeEntry ("KSpell_DictFromList",(int)  _spellConfig->dictFromList());
  config->writeEntry ("KSpell_Encoding", (int)  _spellConfig->encoding());
  config->writeEntry ("KSpell_Client",  _spellConfig->client());

  bool state = m_dontCheckUpperWord->isChecked();
  m_pView->doc()->setDontCheckUpperWord(state);
  config->writeEntry ("KSpell_IgnoreUppercaseWords", (int) state );

  state = m_dontCheckTitleCase->isChecked();
  m_pView->doc()->setDontCheckTitleCase(state);
  config->writeEntry ("KSpell_IgnoreTitleCaseWords", (int) state );

  m_pView->doc()->setKSpellConfig(*_spellConfig);
}

void configureSpellPage::slotDefault()
{
    _spellConfig->setNoRootAffix( 0);
    _spellConfig->setRunTogether(0);
    _spellConfig->setDictionary( "");
    _spellConfig->setDictFromList( FALSE);
    _spellConfig->setEncoding (KS_E_ASCII);
    _spellConfig->setClient (KS_CLIENT_ISPELL);
}

#include "kspread_dlg_preference.moc"
