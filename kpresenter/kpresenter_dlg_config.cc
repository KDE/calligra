/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

#include "kpresenter_dlg_config.h"

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
#include <kprvariable.h>
#include <kprcanvas.h>
#include <tkcoloractions.h>

#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qtabwidget.h>

#include <styledia.h>
#include <confpiedia.h>
#include <confrectdia.h>
#include <confpolygondia.h>
#include <confpicturedia.h>

#include <kpresenter_dlg_config.h>
#include <kpresenter_view.h>
#include <kpresenter_doc.h>
#include <koUnit.h>

#include <float.h>
#include <kspell.h>
#include <knumvalidator.h>
#include <qlineedit.h>
#include <kprcommand.h>
#include <qvgroupbox.h>
#include <kfontdialog.h>
#include <klineedit.h>
#include <koRect.h>

KPConfig::KPConfig( KPresenterView* parent )
  : KDialogBase(KDialogBase::IconList,i18n("Configure KPresenter") ,
		KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel| KDialogBase::Default,
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

    page = addVBoxPage( i18n("Document"), i18n("Document Settings"),
                        BarIcon("document", KIcon::SizeMedium) );

    _defaultDocPage=new ConfigureDefaultDocPage(parent, page);

    page = addVBoxPage( i18n("Tools"), i18n("Default Tools Settings"),
                        BarIcon("configure", KIcon::SizeMedium) );

    _toolsPage=new ConfigureToolsPage(parent, page);

    connect( this, SIGNAL( okClicked() ),this, SLOT( slotApply() ) );
}

void KPConfig::openPage(int flags)
{
    if(flags & KP_INTERFACE)
        showPage( 0 );
    else if(flags & KP_COLOR)
        showPage(1 );
    else if(flags & KP_KSPELL)
        showPage(2);
    else if(flags & KP_MISC)
        showPage(3 );
    else if(flags & KP_DOC)
        showPage(4 );
    else if(flags & KP_TOOLS)
        showPage(5);
}

void KPConfig::slotApply()
{
    _interfacePage->apply();
    _colorBackground->apply();
    _spellPage->apply();
    _miscPage->apply();
    _defaultDocPage->apply();
    _toolsPage->apply();
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
        case 4:
            _defaultDocPage->slotDefault();
            break;
        case 5:
            _toolsPage->slotDefault();
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
    bool oldShowStatusBar = true;

    QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
    tmpQGroupBox->setTitle( i18n("Interface") );

    QVBoxLayout *lay1 = new QVBoxLayout( tmpQGroupBox );
    lay1->setMargin( 20 );
    lay1->setSpacing( 10 );

    int oldRastX = m_pView->kPresenterDoc()->rastX();
    int oldRastY = m_pView->kPresenterDoc()->rastY();

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldNbRecentFiles=config->readNumEntry("NbRecentFile",oldNbRecentFiles);
        ptIndent = config->readDoubleNumEntry("Indent", ptIndent);
        bShowRuler=config->readBoolEntry("Rulers",true);
        oldShowStatusBar = config->readBoolEntry( "ShowStatusBar" , true );

    }

    showRuler= new QCheckBox(i18n("Show rulers"),tmpQGroupBox);
    showRuler->setChecked(bShowRuler);
    lay1->addWidget(showRuler);

    showStatusBar = new QCheckBox(i18n("Show status bar"),tmpQGroupBox);
    showStatusBar->setChecked(oldShowStatusBar);
    lay1->addWidget(showStatusBar);


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
    double val = KoUnit::ptToUnit( ptIndent, unit );
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
    bool statusBar=showStatusBar->isChecked();

    KPresenterDoc * doc = m_pView->kPresenterDoc();

    config->setGroup( "Interface" );
    if( rastX != oldRastX || rastY != oldRastY ) {
        config->writeEntry( "RastX", rastX );
        config->writeEntry( "RastY", rastY );
        doc->setRasters( rastX, rastY, true );
        doc->repaint( false );
        oldRastX=rastX;
        oldRastY=rastY;
    }

    double newIndent = KoUnit::ptFromUnit( indent->value(), doc->getUnit() );
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
        oldNbRecentFiles=nbRecent;
    }
    bool refreshGUI=false;
    if(ruler != doc->showRuler())
    {
        config->writeEntry( "Rulers", ruler );
        doc->setShowRuler( ruler );
        refreshGUI=true;

    }
    if( statusBar != doc->showStatusBar() )
    {
        config->writeEntry( "ShowStatusBar", statusBar );
        doc->setShowStatusBar( statusBar );
        refreshGUI=true;
    }

    if( refreshGUI )
        doc->reorganizeGUI();

}

void configureInterfacePage::slotDefault()
{
    eRastX->setValue( 10 );
    eRastY->setValue( 10 );
    double newIndent = KoUnit::ptToUnit( MM_TO_POINT( 10 ), m_pView->kPresenterDoc()->getUnit() );
    indent->setValue( newIndent );
    recentFiles->setValue(10);
    showRuler->setChecked(true);
    showStatusBar->setChecked(true);
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
    tmpQGroupBox->setTitle( i18n("Objects in Editing Mode") );
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
        oldBgColor=_col;
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

  cbBackgroundSpellCheck=new QCheckBox(i18n("Show misspelled words in document"),tmpQGroupBox);
  oldSpellCheck=m_pView->kPresenterDoc()->backgroundSpellCheckEnabled();
  cbBackgroundSpellCheck->setChecked( oldSpellCheck );
  grid1->addWidget(cbBackgroundSpellCheck,4,0);

  clearIgnoreAllHistory= new QPushButton( i18n("Clear Ignore All Word History"),tmpQGroupBox);
  grid1->addMultiCellWidget(clearIgnoreAllHistory,5,5,0,1);
  connect( clearIgnoreAllHistory, SIGNAL(clicked()),this, SLOT(slotClearIgnoreAllHistory()));

  if( config->hasGroup("KSpell kpresenter") )
    {
        config->setGroup( "KSpell kpresenter" );
        _dontCheckUpperWord->setChecked(config->readBoolEntry("KSpell_dont_check_upper_word",false));
        _dontCheckTilteCase->setChecked(config->readBoolEntry("KSpell_dont_check_title_case",false));
    }

}

void ConfigureSpellPage::slotClearIgnoreAllHistory()
{
    m_pView->kPresenterDoc()->clearIgnoreWordAll();
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

  config->writeEntry( "SpellCheck", cbBackgroundSpellCheck->isChecked() );

  doc->setDontCheckTitleCase(state);

  state = cbBackgroundSpellCheck->isChecked();
  doc->reactivateBgSpellChecking(oldSpellCheck!=state);
  //FIXME reactivate just if there is a changes.
  if( oldSpellCheck!=state)
      doc->enableBackgroundSpellCheck( state );

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
    cbBackgroundSpellCheck->setChecked(false);
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

    KPresenterDoc* doc = m_pView->kPresenterDoc();

    m_displayLink=new QCheckBox(i18n("Display links"),tmpQGroupBox);
    grid->addWidget(m_displayLink,3,0);
    m_displayLink->setChecked(doc->getVariableCollection()->variableSetting()->displayLink());
    m_displayComment=new QCheckBox(i18n("Display comments"),tmpQGroupBox);
    m_displayComment->setChecked(doc->getVariableCollection()->variableSetting()->displayComment());
    grid->addWidget(m_displayComment,4,0);

    tmpQGroupBox = new QGroupBox( box, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Grid"));

    grid = new QGridLayout( tmpQGroupBox , 8, 1, KDialog::marginHint()+7, KDialog::spacingHint() );

    KoRect rect = doc->stickyPage()->getPageRect();
    QLabel *lab=new QLabel(i18n("Resolution X: (%1)").arg(doc->getUnitName()),  tmpQGroupBox);
    grid->addWidget(lab ,0,0);

    resolutionX = new KDoubleNumInput(tmpQGroupBox);
    resolutionX->setValue( KoUnit::ptToUnit( doc->getGridX(), doc->getUnit() ) );
    resolutionX->setRange( KoUnit::ptToUnit(10.0 , doc->getUnit()), KoUnit::ptToUnit(rect.width() , doc->getUnit()), 1, false);

    grid->addWidget(resolutionX ,1,0);

    lab=new QLabel(i18n("Resolution Y: (%1)").arg(doc->getUnitName()), tmpQGroupBox);
    grid->addWidget(lab ,2,0);

    resolutionY = new KDoubleNumInput(tmpQGroupBox);
    resolutionY->setValue( KoUnit::ptToUnit( doc->getGridY(), doc->getUnit() ) );
    resolutionY->setRange( KoUnit::ptToUnit(10.0 , doc->getUnit()), KoUnit::ptToUnit(rect.width() , doc->getUnit()), 1, false);

    grid->addWidget(resolutionY , 3,0);
}

void ConfigureMiscPage::apply()
{
    config->setGroup( "Misc" );
    int newUndo=m_undoRedoLimit->value();
    KPresenterDoc* doc = m_pView->kPresenterDoc();
    if(newUndo!=m_oldNbRedo)
    {
        config->writeEntry("UndoRedo",newUndo);
        doc->setUndoRedoLimit(newUndo);
        m_oldNbRedo=newUndo;
    }
    KMacroCommand * macroCmd=0L;
    bool b=m_displayLink->isChecked();
    bool b_new=doc->getVariableCollection()->variableSetting()->displayLink();
    if(doc->getVariableCollection()->variableSetting()->displayLink()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change display link command"));
        }

        KPrChangeDisplayLinkCommand *cmd=new KPrChangeDisplayLinkCommand( i18n("Change display link command"), doc, b_new ,b);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }
    if(macroCmd)
        doc->addCommand(macroCmd);

    b=m_displayComment->isChecked();
    if(doc->getVariableCollection()->variableSetting()->displayComment()!=b)
    {
        doc->getVariableCollection()->variableSetting()->setDisplayComment(b);
        doc->recalcVariables( VT_NOTE );
    }

    doc->setGridX( KoUnit::ptFromUnit( resolutionX->value(), doc->getUnit() ));
    doc->setGridY( KoUnit::ptFromUnit( resolutionY->value(), doc->getUnit() ));
    doc->repaint( false );
}

void ConfigureMiscPage::slotDefault()
{
   m_undoRedoLimit->setValue(30);
   m_displayLink->setChecked(true);
   m_displayComment->setChecked(true);
   KPresenterDoc* doc = m_pView->kPresenterDoc();

   resolutionY->setValue( KoUnit::ptToUnit( MM_TO_POINT( 10.0), doc->getUnit() ) );
   resolutionX->setValue( KoUnit::ptToUnit( MM_TO_POINT( 10.0 ), doc->getUnit() ) );

}


ConfigureDefaultDocPage::ConfigureDefaultDocPage(KPresenterView *_view, QVBox *box, char *name )
    : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KPresenterFactory::global()->config();
    QVGroupBox* gbDocumentDefaults = new QVGroupBox( i18n("Document Defaults"), box, "GroupBox" );
    gbDocumentDefaults->setMargin( 10 );
    gbDocumentDefaults->setInsideSpacing( 5 );

    QString defaultFont="Sans serif,12,-1,5,50,0,0,0,0,0";
    if( config->hasGroup("Document defaults") )
    {
        config->setGroup( "Document defaults" );
        defaultFont=config->readEntry("DefaultFont",defaultFont);
    }

    QWidget *fontContainer = new QWidget(gbDocumentDefaults);
    QGridLayout * fontLayout = new QGridLayout(fontContainer, 1, 3);

    fontLayout->setColStretch(0, 0);
    fontLayout->setColStretch(1, 1);
    fontLayout->setColStretch(2, 0);

    QLabel *fontTitle = new QLabel(i18n("Default font"), fontContainer);

    font= new QFont();
    font->fromString(defaultFont);

    QString labelName = font->family() + ' ' + QString::number(font->pointSize());
    fontName = new QLabel(labelName, fontContainer);
    fontName->setFont(*font);
    fontName->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    QPushButton *chooseButton = new QPushButton(i18n("Choose..."), fontContainer);
    connect(chooseButton, SIGNAL(clicked()), this, SLOT(selectNewDefaultFont()));

    fontLayout->addWidget(fontTitle, 0, 0);
    fontLayout->addWidget(fontName, 0, 1);
    fontLayout->addWidget(chooseButton, 0, 2);



    QVGroupBox* gbDocumentSettings = new QVGroupBox( i18n("Document Settings"), box );
    gbDocumentSettings->setMargin( 10 );
    gbDocumentSettings->setInsideSpacing( KDialog::spacingHint() );

    oldAutoSaveValue =  m_pView->kPresenterDoc()->defaultAutoSave()/60;

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldAutoSaveValue = config->readNumEntry( "AutoSave", oldAutoSaveValue );
    }

    autoSave = new KIntNumInput( oldAutoSaveValue, gbDocumentSettings );
    autoSave->setRange( 0, 60, 1 );
    autoSave->setLabel( i18n("Auto save (min):") );
    autoSave->setSpecialValueText( i18n("No auto save") );
    autoSave->setSuffix( i18n("min") );

    new QLabel(i18n("Starting page number:"), gbDocumentSettings);
    KPresenterDoc* doc = m_pView->kPresenterDoc();
    m_oldStartingPage=doc->getVariableCollection()->variableSetting()->startingPage();
    m_variableNumberOffset=new QLineEdit(gbDocumentSettings);
    m_variableNumberOffset->setValidator(new KIntValidator(0,9999,m_variableNumberOffset));
    m_variableNumberOffset->setText(QString::number(m_oldStartingPage));
}

ConfigureDefaultDocPage::~ConfigureDefaultDocPage()
{
    delete font;
}

void ConfigureDefaultDocPage::apply()
{
    config->setGroup( "Document defaults" );
    KPresenterDoc* doc = m_pView->kPresenterDoc();
    config->writeEntry("DefaultFont",font->toString());

    config->setGroup( "Interface" );
    int autoSaveVal = autoSave->value();
    if( autoSaveVal != oldAutoSaveValue ) {
        config->writeEntry( "AutoSave", autoSaveVal );
        m_pView->kPresenterDoc()->setAutoSave( autoSaveVal*60 );
        oldAutoSaveValue=autoSaveVal;
    }
    int newStartingPage=m_variableNumberOffset->text().toInt();
    if(newStartingPage!=m_oldStartingPage)
    {
        KPrChangeStartingPageCommand *cmd = new KPrChangeStartingPageCommand( i18n("Change starting page number"), doc, m_oldStartingPage,newStartingPage );
        cmd->execute();
        doc->addCommand(cmd);
        m_oldStartingPage=newStartingPage;
    }

}

void ConfigureDefaultDocPage::slotDefault()
{
    autoSave->setValue( m_pView->kPresenterDoc()->defaultAutoSave()/60 );
    m_variableNumberOffset->setText(QString::number(1));

}

void ConfigureDefaultDocPage::selectNewDefaultFont() {
    QStringList list;
    KFontChooser::getFontList(list,  KFontChooser::SmoothScalableFonts);
    KFontDialog dlg( m_pView, "Font Selector", false, true, list, true );
    dlg.setFont(*font);
    int result = dlg.exec();
    if (KDialog::Accepted == result) {
        delete font;
        font = new QFont(dlg.font());
        fontName->setText(font->family() + ' ' + QString::number(font->pointSize()));
        fontName->setFont(*font);
    }
}

ConfigureToolsPage::ConfigureToolsPage( KPresenterView *_view, QVBox *box, char *name )
    : QObject( box->parent(), name )
{
    m_pView = _view;
    config = KPresenterFactory::global()->config();
    m_pView->getCanvas()->deSelectAllObj();

    QTabWidget *tab = new QTabWidget(box);
    box->setMargin( 5 );
    box->setSpacing( 10 );

    m_confPenDia = new ConfPenDia(tab, 0, StyleDia::SdAll);
    m_confPenDia->setPen(m_pView->getCanvas()->activePage()->getPen(m_pView->getPen()));
    m_confPenDia->setLineBegin(m_pView->getCanvas()->activePage()->getLineBegin(m_pView->getLineBegin()));
    m_confPenDia->setLineEnd(m_pView->getCanvas()->activePage()->getLineEnd(m_pView->getLineEnd()));
    tab->addTab(m_confPenDia, i18n("&Pen"));

    m_confBrushDia = new ConfBrushDia(tab, 0, StyleDia::SdAll);
    m_confBrushDia->setBrush(m_pView->getCanvas()->activePage()->getBrush(m_pView->getBrush()));
    m_confBrushDia->setFillType(m_pView->getCanvas()->activePage()->getFillType(m_pView->getFillType()));
    m_confBrushDia->setGradient(m_pView->getCanvas()->activePage()->getGColor1(m_pView->getGColor1()),
                                m_pView->getCanvas()->activePage()->getGColor2(m_pView->getGColor2()),
                                m_pView->getCanvas()->activePage()->getGType(m_pView->getGType()),
                                m_pView->getCanvas()->activePage()->getGUnbalanced(m_pView->getGUnbalanced()),
                                m_pView->getCanvas()->activePage()->getGXFactor(m_pView->getGXFactor()),
                                m_pView->getCanvas()->activePage()->getGYFactor(m_pView->getGYFactor()));
    tab->addTab(m_confBrushDia, i18n("&Brush"));

    m_confPieDia = new ConfPieDia(tab, "ConfPageDia");
    m_confPieDia->setType(m_pView->getCanvas()->activePage()->getPieType(m_pView->getPieType()));
    m_confPieDia->setAngle(m_pView->getCanvas()->activePage()->getPieAngle(m_pView->getPieAngle()));
    m_confPieDia->setLength(m_pView->getCanvas()->activePage()->getPieLength(m_pView->getPieLength()));
    m_confPieDia->setPenBrush(m_pView->getCanvas()->activePage()->getPen(m_pView->getPen()),
                            m_pView->getCanvas()->activePage()->getBrush(m_pView->getBrush()));
    tab->addTab(m_confPieDia, i18n("P&ie"));

    bool _checkConcavePolygon;
    int _cornersValue;
    int _sharpnessValue;

    if (!m_pView->getCanvas()->activePage()->getPolygonSettings(&_checkConcavePolygon, &_cornersValue, &_sharpnessValue))
    {
        _checkConcavePolygon = m_pView->getCheckConcavePolygon();
        _cornersValue = m_pView->getCornersValue();
        _sharpnessValue = m_pView->getSharpnessValue();
    }

    m_confPolygonDia = new ConfPolygonDia(tab, "ConfPolygonDia", _checkConcavePolygon, _cornersValue, _sharpnessValue );
    m_confPolygonDia->setPenBrush(m_pView->getCanvas()->activePage()->getPen(m_pView->getPen()),
                                  m_pView->getCanvas()->activePage()->getBrush(m_pView->getBrush()));
    tab->addTab(m_confPolygonDia, i18n("P&olygon"));

    m_confRectDia = new ConfRectDia(tab, "ConfRectDia" );
    m_confRectDia->setRnds(m_pView->getCanvas()->activePage()->getRndX(m_pView->getRndX()),
                           m_pView->getCanvas()->activePage()->getRndY(m_pView->getRndY()));
    m_confRectDia->setPenBrush(m_pView->getCanvas()->activePage()->getPen(m_pView->getPen()),
                               m_pView->getCanvas()->activePage()->getBrush(m_pView->getBrush()));
    tab->addTab(m_confRectDia, i18n("&Rectangle"));
}

void ConfigureToolsPage::apply()
{
    m_pView->setPieType(m_confPieDia->getType());
    m_pView->setPieAngle(m_confPieDia->getAngle());
    m_pView->setPieLength(m_confPieDia->getLength());
    m_pView->setCheckConcavePolygon(m_confPolygonDia->getCheckConcavePolygon());
    m_pView->setCornersValue(m_confPolygonDia->getCornersValue());
    m_pView->setSharpnessValue(m_confPolygonDia->getSharpnessValue());
    m_pView->setRndX(m_confRectDia->getRndX());
    m_pView->setRndY(m_confRectDia->getRndY());
    m_pView->setPen(m_confPenDia->getPen());
    m_pView->setBrush(m_confBrushDia->getBrush());
    m_pView->setLineBegin(m_confPenDia->getLineBegin());
    m_pView->setLineEnd(m_confPenDia->getLineEnd());
    m_pView->setFillType(m_confBrushDia->getFillType());
    m_pView->setGColor1(m_confBrushDia->getGColor1());
    m_pView->setGColor2(m_confBrushDia->getGColor2());
    m_pView->setGType(m_confBrushDia->getGType());
    m_pView->setGUnbalanced(m_confBrushDia->getGUnbalanced());
    m_pView->setGXFactor(m_confBrushDia->getGXFactor());
    m_pView->setGYFactor(m_confBrushDia->getGYFactor());
    m_pView->getActionBrushColor()->setCurrentColor((m_confBrushDia->getBrush()).color());
    m_pView->getActionPenColor()->setCurrentColor((m_confPenDia->getPen()).color());
    m_confRectDia->setPenBrush(m_confPenDia->getPen(),
                               m_confBrushDia->getBrush());
    m_confPolygonDia->setPenBrush(m_confPenDia->getPen(),
                                  m_confBrushDia->getBrush());
    m_confPieDia->setPenBrush(m_confPenDia->getPen(),
                              m_confBrushDia->getBrush());
}

void ConfigureToolsPage::slotDefault()
{
    m_confPieDia->setType(PT_PIE);
    m_confPieDia->setLength(90 * 16);
    m_confPieDia->setAngle(45 * 16);
    m_confPolygonDia->setCheckConcavePolygon(false);
    m_confPolygonDia->setCornersValue(3);
    m_confPolygonDia->setSharpnessValue(0);
    m_confRectDia->setRnds(0, 0);
    m_confPenDia->setPen(QPen(black, 1, SolidLine));
    m_confBrushDia->setBrush(QBrush(white, SolidPattern));
    m_confPenDia->setLineBegin(L_NORMAL);
    m_confPenDia->setLineEnd(L_NORMAL);
    m_confBrushDia->setGradient(red, green, BCT_GHORZ, false, 100, 100);
    m_confBrushDia->setFillType(FT_BRUSH);
    m_pView->getActionBrushColor()->setCurrentColor((m_confBrushDia->getBrush()).color());
    m_pView->getActionPenColor()->setCurrentColor((m_confPenDia->getPen()).color());
}

#include <kpresenter_dlg_config.moc>
