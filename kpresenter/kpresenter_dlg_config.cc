/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <kapplication.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kcolorbutton.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qcheckbox.h>

#include <kpresenter_dlg_config.h>
#include <kpresenter_view.h>
#include <kpresenter_doc.h>
#include <koUnit.h>

#include <float.h>
#include <kspell.h>

KPConfig::KPConfig( KPresenterView* parent )
  : KDialogBase(KDialogBase::IconList,i18n("Configure KPresenter") ,
		KDialogBase::Ok | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
    QVBox *page = addVBoxPage( i18n("Interface"), i18n("Interface"),
                        BarIcon("misc", KIcon::SizeMedium) );
    _interfacePage=new configureInterfacePage( parent, page );
    page = addVBoxPage( i18n("Color"), i18n("Color"),
                        BarIcon("colorize", KIcon::SizeMedium) );
    _colorBackground = new configureColorBackground( parent, page );

    page = addVBoxPage( i18n("Spelling"), i18n("Spell checker behavior"),
                        BarIcon("spellcheck", KIcon::SizeMedium) );
    _spellPage=new ConfigureSpellPage(parent, page);

    page = addVBoxPage( i18n("Misc"), i18n("Misc"),
                        BarIcon("misc", KIcon::SizeMedium) );
    _miscPage=new ConfigureMiscPage(parent, page);

    connect( this, SIGNAL( okClicked() ),this, SLOT( slotApply() ) );
}

void KPConfig::slotApply()
{
    _interfacePage->apply();
    _colorBackground->apply();
    _spellPage->apply();
    _miscPage->apply();
}

void KPConfig::slotDefault()
{
    switch( activePageIndex() ) {
        case 0:
            _interfacePage->slotDefault();
            break;
        case 1:
            _colorBackground->slotDefault();
            break;
        case 2:
            _spellPage->slotDefault();
            break;
        case 3:
           _miscPage->slotDefault();
            break;
        default:
            break;
    }
}

configureInterfacePage::configureInterfacePage( KPresenterView *_view, QWidget *parent , char *name )
 :QWidget ( parent,name )
{
    m_pView=_view;
    config = KPresenterFactory::global()->config();

    KoUnit::Unit unit = m_pView->kPresenterDoc()->getUnit();

    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin( 5 );
    box->setSpacing( 10 );

    oldNbRecentFiles=10;
    double ptIndent = MM_TO_POINT(10.0);
    bool bShowRuler=true;

    QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
    tmpQGroupBox->setTitle( i18n("Interface") );

    QVBoxLayout *lay1 = new QVBoxLayout( tmpQGroupBox );
    lay1->setMargin( 20 );
    lay1->setSpacing( 10 );

    int oldRastX = m_pView->kPresenterDoc()->rastX();
    int oldRastY = m_pView->kPresenterDoc()->rastY();
    oldAutoSaveValue =  m_pView->kPresenterDoc()->defaultAutoSave()/60;

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldAutoSaveValue = config->readNumEntry( "AutoSave", oldAutoSaveValue );
        oldNbRecentFiles=config->readNumEntry("NbRecentFile",oldNbRecentFiles);
        ptIndent = config->readDoubleNumEntry("Indent", ptIndent);
        bShowRuler=config->readBoolEntry("Rulers",true);
    }

    showRuler= new QCheckBox(i18n("Show rulers"),tmpQGroupBox);
    showRuler->setChecked(bShowRuler);
    lay1->addWidget(showRuler);

    autoSave = new KIntNumInput( oldAutoSaveValue, tmpQGroupBox );
    autoSave->setRange( 0, 60, 1 );
    autoSave->setLabel( i18n("Auto save (min):") );
    autoSave->setSpecialValueText( i18n("No auto save") );
    autoSave->setSuffix( i18n("min") );
    lay1->addWidget( autoSave );

    eRastX = new KIntNumInput( oldRastX, tmpQGroupBox );
    eRastX->setRange( 1, 400, 1 );
    eRastX->setLabel( i18n("Horizontal Raster: ") );
    lay1->addWidget( eRastX );

    eRastY = new KIntNumInput( oldRastY, tmpQGroupBox );
    eRastY->setRange( 1, 400, 1 );
    eRastY->setLabel( i18n("Vertical Raster: ") );
    lay1->addWidget( eRastY );

    recentFiles=new KIntNumInput( oldNbRecentFiles, tmpQGroupBox );
    recentFiles->setRange(1, 20, 1);
    recentFiles->setLabel(i18n("Number of recent file:"));

    lay1->addWidget(recentFiles);

    QString suffix = KoUnit::unitName( unit ).prepend(' ');
    double val = KoUnit::userValue( ptIndent, unit );
    indent = new KDoubleNumInput( val, tmpQGroupBox );
    indent->setRange(0.1, 50, 0.1);
    indent->setPrecision(1);
    indent->setSuffix( suffix );
    indent->setLabel(i18n("Paragraph indent by toolbar buttons"));

    lay1->addWidget(indent);



    box->addWidget( tmpQGroupBox );
}

void configureInterfacePage::apply()
{
    unsigned int rastX = eRastX->value();
    unsigned int rastY = eRastY->value();
    bool ruler=showRuler->isChecked();
    KPresenterDoc * doc = m_pView->kPresenterDoc();

    config->setGroup( "Interface" );
    if( rastX != oldRastX || rastY != oldRastX ) {
        config->writeEntry( "RastX", rastX );
        config->writeEntry( "RastY", rastY );
        doc->setRasters( rastX, rastY, true );
        doc->repaint( false );
    }

    int autoSaveVal = autoSave->value();
    if( autoSaveVal != oldAutoSaveValue ) {
        config->writeEntry( "AutoSave", autoSaveVal );
        m_pView->kPresenterDoc()->setAutoSave( autoSaveVal*60 );
    }

    double newIndent = KoUnit::fromUserValue( indent->value(), doc->getUnit() );
    if( newIndent != doc->getIndentValue() )
    {
        config->writeEntry( "Indent", newIndent, true, false, 'g', DBL_DIG /* 6 is not enough */ );
        doc->setIndentValue( newIndent );
    }
    int nbRecent=recentFiles->value();
    if(nbRecent!=oldNbRecentFiles)
    {
        config->writeEntry( "NbRecentFile", nbRecent);
        m_pView->changeNbOfRecentFiles(nbRecent);
    }
    if(ruler != doc->showRuler())
    {
        config->writeEntry( "Rulers", ruler );
        doc->setShowRuler( ruler );
        doc->reorganizeGUI();
    }
}

void configureInterfacePage::slotDefault()
{
    eRastX->setValue( 10 );
    eRastY->setValue( 10 );
    autoSave->setValue( m_pView->kPresenterDoc()->defaultAutoSave()/60 );
    double newIndent = KoUnit::userValue( MM_TO_POINT( 10 ), m_pView->kPresenterDoc()->getUnit() );
    indent->setValue( newIndent );
    recentFiles->setValue(10);
    showRuler->setChecked(true);
}

configureColorBackground::configureColorBackground( KPresenterView* _view, QWidget *parent , char *name )
 :QWidget ( parent,name )
{
    m_pView = _view;
    config = KPresenterFactory::global()->config();

    oldBgColor = m_pView->kPresenterDoc()->txtBackCol();

    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin( 5 );
    box->setSpacing( 10 );

    QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
    tmpQGroupBox->setTitle( i18n("Objects in editing mode") );
    QGridLayout *grid1 = new QGridLayout( tmpQGroupBox, 5, 1, 15, 7);
    QLabel *lab = new QLabel( tmpQGroupBox, "label20" );
    lab->setText( i18n( "Background color:" ) );
    grid1->addWidget( lab, 0, 0 );

    bgColor = new KColorButton( tmpQGroupBox );
    bgColor->setColor( oldBgColor );
    grid1->addWidget( bgColor, 1, 0 );

    box->addWidget( tmpQGroupBox );
}

void configureColorBackground::apply()
{
    QColor _col = bgColor->color();
    KPresenterDoc * doc = m_pView->kPresenterDoc();
    if( oldBgColor != _col ) {
        config->setGroup( "KPresenter Color" );
        config->writeEntry( "BackgroundColor", _col );
        doc->setTxtBackCol( _col );
        doc->replaceObjs();
        doc->repaint( false );
    }
}

void configureColorBackground::slotDefault()
{
    bgColor->setColor( Qt::white );
}



ConfigureSpellPage::ConfigureSpellPage( KPresenterView *_view, QVBox *box, char *name )
    : QObject( box->parent(), name )
{
  m_pView=_view;
  config = KPresenterFactory::global()->config();
  QGroupBox* tmpQGroupBox = new QGroupBox( box, "GroupBox" );
  tmpQGroupBox->setTitle(i18n("Spelling"));

  QGridLayout *grid1 = new QGridLayout(tmpQGroupBox, 5, 1, KDialog::marginHint(), KDialog::spacingHint());
  grid1->addRowSpacing( 0, KDialog::marginHint() + 5 );
  grid1->setRowStretch( 4, 10 );
  _spellConfig = new KSpellConfig(tmpQGroupBox, 0L, m_pView->kPresenterDoc()->getKSpellConfig(), false );
  grid1->addWidget(_spellConfig,1,0);

  _dontCheckUpperWord= new QCheckBox(i18n("Ignore uppercase words"),tmpQGroupBox);
  grid1->addWidget(_dontCheckUpperWord,2,0);

  _dontCheckTilteCase= new QCheckBox(i18n("Ignore title case words"),tmpQGroupBox);
  grid1->addWidget(_dontCheckTilteCase,3,0);

  if( config->hasGroup("KSpell kpresenter") )
    {
        config->setGroup( "KSpell kpresenter" );
        _dontCheckUpperWord->setChecked(config->readBoolEntry("KSpell_dont_check_upper_word",false));
        _dontCheckTilteCase->setChecked(config->readBoolEntry("KSpell_dont_check_title_case",false));
    }

}

void ConfigureSpellPage::apply()
{
  config->setGroup( "KSpell kpresenter" );
  config->writeEntry ("KSpell_NoRootAffix",(int) _spellConfig->noRootAffix ());
  config->writeEntry ("KSpell_RunTogether", (int) _spellConfig->runTogether ());
  config->writeEntry ("KSpell_Dictionary", _spellConfig->dictionary ());
  config->writeEntry ("KSpell_DictFromList",(int)  _spellConfig->dictFromList());
  config->writeEntry ("KSpell_Encoding", (int)  _spellConfig->encoding());
  config->writeEntry ("KSpell_Client",  _spellConfig->client());
  KPresenterDoc *doc=m_pView->kPresenterDoc();
  doc->setKSpellConfig(*_spellConfig);

  bool state=_dontCheckUpperWord->isChecked();
  config->writeEntry ("KSpell_dont_check_upper_word",(int)state);
  doc->setDontCheckUpperWord(state);

  state=_dontCheckTilteCase->isChecked();
  config->writeEntry("KSpell_dont_check_title_case",(int)state);
  doc->setDontCheckTitleCase(state);
}

void ConfigureSpellPage::slotDefault()
{
    _spellConfig->setNoRootAffix( 0);
    _spellConfig->setRunTogether(0);
    _spellConfig->setDictionary( "");
    _spellConfig->setDictFromList( FALSE);
    _spellConfig->setEncoding (KS_E_ASCII);
    _spellConfig->setClient (KS_CLIENT_ISPELL);
    _dontCheckUpperWord->setChecked(false);
    _dontCheckTilteCase->setChecked(false);

}

ConfigureMiscPage::ConfigureMiscPage( KPresenterView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KPresenterFactory::global()->config();
    QGroupBox* tmpQGroupBox = new QGroupBox( box, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Misc"));

    QGridLayout *grid = new QGridLayout( tmpQGroupBox , 8, 1, KDialog::marginHint()+7, KDialog::spacingHint() );

    m_oldNbRedo=30;
    if( config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        m_oldNbRedo=config->readNumEntry("UndoRedo",m_oldNbRedo);
    }

    m_undoRedoLimit=new KIntNumInput( m_oldNbRedo, tmpQGroupBox );
    m_undoRedoLimit->setLabel(i18n("Undo/redo limit:"));
    m_undoRedoLimit->setRange(10, 60, 1);
    grid->addWidget(m_undoRedoLimit,0,0);
}

void ConfigureMiscPage::apply()
{
    config->setGroup( "Misc" );
    int newUndo=m_undoRedoLimit->value();
    if(newUndo!=m_oldNbRedo)
    {
        config->writeEntry("UndoRedo",newUndo);
        m_pView->kPresenterDoc()->setUndoRedoLimit(newUndo);
    }
}

void ConfigureMiscPage::slotDefault()
{
   m_undoRedoLimit->setValue(30);
}



#include <kpresenter_dlg_config.moc>
