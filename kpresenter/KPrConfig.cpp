/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <KoUnitWidgets.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include "KPrVariableCollection.h"
#include "KPrCanvas.h"
#include <tkcoloractions.h>


#include <q3groupbox.h>
#include <QLabel>
#include <QLayout>
#include <q3vbox.h>
#include <kvbox.h>
#include <QCheckBox>
#include <QComboBox>
#include <qtabwidget.h>

//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3VBoxLayout>

#include "KPrPenStyleWidget.h"
#include "KPrBrushProperty.h"
#include "KPrPieProperty.h"
#include "KPrRectProperty.h"
#include "KPrPolygonProperty.h"

#include "KPrConfig.h"
#include "KPrView.h"
#include "KPrDocument.h"
#include "KPrPage.h"
#include <KoUnit.h>

#include <float.h>
#include <knumvalidator.h>
#include <QLineEdit>
#include "KPrCommand.h"
#include <kfontdialog.h>
#include <klineedit.h>
#include <KoRect.h>
#include <kmessagebox.h>
#include <kdeversion.h>
#include <kurlrequesterdlg.h>
#include <k3listview.h>
#include <kfiledialog.h>
#include <KoEditPath.h>

#include <kspell2/configwidget.h>
#include <kspell2/settings.h>
#include <kspell2/broker.h>
using namespace KSpell2;

KPrConfig::KPrConfig( KPrView* parent )
    : KPageDialog( parent )

{
    setFaceType(IconList);
    setCaption(i18n("Configure KPresenter"));
    setButtons(Ok|Apply|Cancel|Default);

    m_doc = parent->kPresenterDoc();
    KVBox *page = addVBoxPage( i18n("Interface"), i18n("Interface"),
                               BarIcon("misc", K3Icon::SizeMedium) );
    _interfacePage=new KPrConfigureInterfacePage( parent, page );
    page = addVBoxPage( i18n("Color"), i18n("Color"),
                        BarIcon("colorize", K3Icon::SizeMedium) );
    _colorBackground = new KPrConfigureColorBackground( parent, page );

    page = addVBoxPage( i18n("Spelling"), i18n("Spellchecker Behavior"),
                        BarIcon("spellcheck", K3Icon::SizeMedium) );
    _spellPage=new KPrConfigureSpellPage(parent, page);

    page = addVBoxPage( i18n("Misc"), i18n("Misc"),
                        BarIcon("misc", K3Icon::SizeMedium) );
    _miscPage=new KPrConfigureMiscPage(parent, page);

    page = addVBoxPage( i18n("Document"), i18n("Document Settings"),
                        BarIcon("kpresenter_kpr", K3Icon::SizeMedium) );

    _defaultDocPage=new KPrConfigureDefaultDocPage(parent, page);

    page = addVBoxPage( i18n("Tools"), i18n("Default Tools Settings"),
                        BarIcon("configure", K3Icon::SizeMedium) );

    _toolsPage=new KPrConfigureToolsPage(parent, page);

    page = addVBoxPage( i18n("Paths"), i18n("Path Settings"),
                        BarIcon("path") );

    m_pathPage=new KPrConfigurePathPage(parent, page);

    connect( this, SIGNAL( okClicked() ),this, SLOT( slotApply() ) );
}

void KPrConfig::openPage(int flags)
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
    else if(flags & KP_PATH)
        showPage(6);
}

void KPrConfig::slotApply()
{
    KMacroCommand *macro = 0L;
    _interfacePage->apply();
    _colorBackground->apply();
    if (_spellPage) _spellPage->apply();
    m_pathPage->apply();
    KCommand *cmd = _miscPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand(i18n("Change Config") );
        macro->addCommand( cmd );
    }
    cmd = _defaultDocPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand(i18n("Change Config") );

        macro->addCommand( cmd );
    }

    _toolsPage->apply();
    if ( macro )
        m_doc->addCommand( macro);
}

void KPrConfig::slotDefault()
{
    switch( activePageIndex() ) {
    case 0:
        _interfacePage->slotDefault();
        break;
    case 1:
        _colorBackground->slotDefault();
        break;
    case 2:
        if (_spellPage) _spellPage->slotDefault();
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
    case 6:
        m_pathPage->slotDefault();
        break;
    default:
        break;
    }
}

KPrConfigureInterfacePage::KPrConfigureInterfacePage( KPrView *_view, QWidget *parent , char *name )
    :QWidget ( parent,name )
{
    Q3VBoxLayout *box = new Q3VBoxLayout( this, 0, 0 );

    m_pView=_view;
    config = KPrFactory::global()->config();

    KoUnit::Unit unit = m_pView->kPresenterDoc()->unit();

    oldNbRecentFiles=10;
    double ptIndent = MM_TO_POINT(10.0);
    bool bShowRuler=true;
    bool oldShowStatusBar = true;

    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldNbRecentFiles=config->readNumEntry("NbRecentFile",oldNbRecentFiles);
        ptIndent = config->readDoubleNumEntry("Indent", ptIndent);
        bShowRuler=config->readBoolEntry("Rulers",true);
        oldShowStatusBar = config->readBoolEntry( "ShowStatusBar" , true );

    }

    showRuler= new QCheckBox(i18n("Show rulers"),this);
    showRuler->setWhatsThis( i18n( "When checked, both vertical and horizontal rulers are shown on the KPresenter slide (this is the default). When unchecked, the rulers are not shown on any slide." ) );
    showRuler->setChecked(bShowRuler);
    box->addWidget(showRuler);

    showStatusBar = new QCheckBox(i18n("Show status bar"),this);
    showStatusBar->setWhatsThis( i18n( "Toggle the statusbar, which is shown by default." ) );
    showStatusBar->setChecked(oldShowStatusBar);
    box->addWidget(showStatusBar);


    recentFiles=new KIntNumInput( oldNbRecentFiles, this);
    recentFiles->setRange(1, 20, 1);
    recentFiles->setLabel(i18n("Number of recent files:"));
    recentFiles->setWhatsThis( i18n( "Set the number of recent files which will be opened using the File->Open Recent menu. Default is to remember 10 filenames. The maximum you can set is 20 and the minimum is 1." ) );

    box->addWidget(recentFiles);

    QString suffix = KoUnit::unitName( unit ).prepend(' ');
    indent = new KDoubleNumInput( this );
    indent->setValue( KoUnit::toUserValue( ptIndent, unit ) );
    indent->setRange(KoUnit::toUserValue( 0.1, unit ), KoUnit::toUserValue( 50, unit ), KoUnit::toUserValue( 0.1, unit ));


    indent->setSuffix( suffix );
    indent->setLabel(i18n("Text indentation depth:"));
    indent->setWhatsThis( i18n( "This setting is used by Increase Depth and Decrease Depth menu items (in the Text menu) to change the indentation depth. The Default is 1 centimeter." ) );

    box->addWidget(indent);

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    box->addItem( spacer);
}

void KPrConfigureInterfacePage::apply()
{
    bool ruler=showRuler->isChecked();
    bool statusBar=showStatusBar->isChecked();

    KPrDocument * doc = m_pView->kPresenterDoc();

    config->setGroup( "Interface" );

    double newIndent = KoUnit::fromUserValue( indent->value(), doc->unit() );
    if( newIndent != doc->getIndentValue() )
    {
        config->writeEntry( "Indent", newIndent );
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

void KPrConfigureInterfacePage::slotDefault()
{
    double newIndent = KoUnit::toUserValue( MM_TO_POINT( 10 ), m_pView->kPresenterDoc()->unit() );
    indent->setValue( newIndent );
    recentFiles->setValue(10);
    showRuler->setChecked(true);
    showStatusBar->setChecked(true);
}

KPrConfigureColorBackground::KPrConfigureColorBackground( KPrView* _view, QWidget *parent , char *name )
    :QWidget ( parent,name )
{
    m_pView = _view;
    config = KPrFactory::global()->config();

    oldBgColor = m_pView->kPresenterDoc()->txtBackCol();
    oldGridColor = m_pView->kPresenterDoc()->gridColor();
    Q3VBoxLayout *box = new Q3VBoxLayout( this, 0, 0 );

    QLabel *lab = new QLabel( this, "label20" );
    lab->setText( i18n( "Background object color:" ) );
    lab->setWhatsThis( i18n( "Change the background color of the text box. The background is white by default. If you have a dark background color and you want to put some white text on it, you can change the color of the text box so that you can see what you are typing. When you have finished, the area around the text will revert to the background color. The Defaults button restores the original settings." ) );
    box->addWidget(lab);

    bgColor = new KColorButton( oldBgColor,
                                oldBgColor,
                                this );
    bgColor->setColor( oldBgColor );
    box->addWidget(bgColor);


    lab = new QLabel( this, "label20" );
    lab->setText( i18n( "Grid color:" ) );
    lab->setWhatsThis( i18n( "Here you can change the grid color, which is black by default." ) );
    box->addWidget(lab);

    gridColor = new KColorButton( oldGridColor,
                                  oldGridColor,
                                  this );
    box->addWidget(gridColor);
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    box->addItem( spacer);

}

void KPrConfigureColorBackground::apply()
{
    KPrDocument * doc = m_pView->kPresenterDoc();
    bool repaintNeeded = false;
    QColor _col = bgColor->color();
    if( oldBgColor != _col ) {
        config->setGroup( "KPresenter Color" );
        config->writeEntry( "BackgroundColor", _col );
        doc->setTxtBackCol( _col );
        doc->replaceObjs();
        oldBgColor=_col;
        repaintNeeded = true;
    }
    _col = gridColor->color();
    if( oldGridColor != _col ) {
        config->setGroup( "KPresenter Color" );
        config->writeEntry( "GridColor", _col );
        doc->repaint( false );
        doc->setGridColor( _col );
        oldGridColor=_col;
        repaintNeeded = true;
    }
    if (repaintNeeded)
        doc->repaint( false );
}

void KPrConfigureColorBackground::slotDefault()
{
    bgColor->setColor( Qt::white );
    gridColor->setColor( Qt::black );
}



KPrConfigureSpellPage::KPrConfigureSpellPage( KPrView *_view, QWidget *parent, char *name )
    : QWidget( parent, name )
{
    m_pView=_view;
    config = KPrFactory::global()->config();
    m_spellConfigWidget = new ConfigWidget( _view->broker(), parent );
    m_spellConfigWidget->setBackgroundCheckingButtonShown( true );
}

void KPrConfigureSpellPage::apply()
{

    KPrDocument* doc = m_pView->kPresenterDoc();
    m_spellConfigWidget->save();


    m_pView->kPresenterDoc()->setSpellCheckIgnoreList(
        m_pView->broker()->settings()->currentIgnoreList() );
    //FIXME reactivate just if there are changes.
    doc->enableBackgroundSpellCheck( m_pView->broker()->settings()->backgroundCheckerEnabled() );
    doc->reactivateBgSpellChecking();
}

void KPrConfigureSpellPage::slotDefault()
{
    m_spellConfigWidget->slotDefault();
}

KPrConfigureMiscPage::KPrConfigureMiscPage( KPrView *_view, QWidget *parent, char *name )
    : QWidget( parent, name )
{
    Q3VBoxLayout *box = new Q3VBoxLayout( this, 0, 0 );

    m_pView=_view;
    config = KPrFactory::global()->config();

    Q3GroupBox* tmpQGroupBox = new Q3GroupBox( 0, Qt::Vertical, i18n("Misc"), this, "GroupBox" );
    tmpQGroupBox->layout()->setSpacing(KDialog::spacingHint());
    tmpQGroupBox->layout()->setMargin(KDialog::marginHint());
    Q3GridLayout *grid = new Q3GridLayout( tmpQGroupBox->layout(), 8, 1 );

    m_oldNbRedo=30;
    m_printNotes=true;
    if( config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        m_oldNbRedo=config->readNumEntry("UndoRedo",m_oldNbRedo);
        m_printNotes = config->readBoolEntry("PrintNotes", true);
    }

    m_undoRedoLimit=new KIntNumInput( m_oldNbRedo, tmpQGroupBox );
    m_undoRedoLimit->setLabel(i18n("Undo/redo limit:"));
    m_undoRedoLimit->setRange(10, 60, 1);
    m_undoRedoLimit->setWhatsThis( i18n( "Set the number of actions you can undo and redo (how many actions KPresenter keeps in its Undo buffer). This ranges from a minimum of 10 to a maximum of 60 (the default is 30). Once the number of actions reaches the number set here, earlier actions will be forgotten." ) );
    grid->addWidget(m_undoRedoLimit,0,0);

    KPrDocument* doc = m_pView->kPresenterDoc();

    m_displayLink=new QCheckBox(i18n("Display links"),tmpQGroupBox);
    m_displayLink->setWhatsThis( i18n( "When you want to include a link in your slide, you can use the Insert->Link... menu, which allows you to insert URL, mail or file links. If the option Display links is checked, all links will be active and displayed in a different color (this is the default behavior). If the option is unchecked, the links will be inactive and the same color as the text. This affects both the edited slides and the slide show." ) );
    grid->addWidget(m_displayLink,3,0);
    m_displayLink->setChecked(doc->getVariableCollection()->variableSetting()->displayLink());

    m_underlineLink=new QCheckBox(i18n("&Underline all links"),tmpQGroupBox);
    m_underlineLink->setChecked(doc->getVariableCollection()->variableSetting()->underlineLink());
    m_underlineLink->setWhatsThis( i18n( "If this is checked, all links will be underlined (this is the default). If it is not checked, the links will not be underlined." ) );
    grid->addWidget(m_underlineLink,4,0);


    m_displayComment=new QCheckBox(i18n("Display comments"),tmpQGroupBox);
    m_displayComment->setChecked(doc->getVariableCollection()->variableSetting()->displayComment());
    m_displayComment->setWhatsThis( i18n( "Comments are inserted in the text at the cursor using the Insert->Comment... menu. Comments can only be viewed in edit mode and not in the slide show. If this option is checked (default) then each comment will be shown as a small yellow rectangle. You can then right-click on them to edit them, remove them or copy the text." ) );
    grid->addWidget(m_displayComment,5,0);

    m_displayFieldCode=new QCheckBox(i18n("Display field code"),tmpQGroupBox);
    m_displayFieldCode->setChecked(doc->getVariableCollection()->variableSetting()->displayFieldCode());
    m_displayFieldCode->setWhatsThis( i18n( "In editor mode (not in slide show) this option will display all the variable codes as well as Link at links location. This is very useful to see what variable is displayed. Variables are inserted using the Insert -> Variable menu." ) );
    grid->addWidget(m_displayFieldCode,6,0);

    m_cbPrintNotes=new QCheckBox(i18n("Print slide notes"),tmpQGroupBox);
    m_cbPrintNotes->setChecked(m_printNotes);
    m_cbPrintNotes->setWhatsThis( i18n( "If checked, all notes will be printed on paper. The notes will all be printed separately on the last page, from the first slide to the last and finally the Master Page Note. You can see the notes for each slide using the View->Show notebar menu." ) );
    grid->addWidget(m_cbPrintNotes,7,0);

    box->addWidget(tmpQGroupBox);

    tmpQGroupBox = new Q3GroupBox( 0, Qt::Vertical, i18n("Grid"), this, "GroupBox" );
    tmpQGroupBox->layout()->setSpacing(KDialog::spacingHint());
    tmpQGroupBox->layout()->setMargin(KDialog::marginHint());
    grid = new Q3GridLayout( tmpQGroupBox->layout(), 8, 1 );

    KoRect rect = doc->masterPage()->getPageRect();
    QLabel *lab=new QLabel(i18n("Horizontal grid size:"), tmpQGroupBox);
    lab->setWhatsThis( i18n( "Set the space in millimeters between two horizontal points on the grid. Default is 5 millimeters." ) );
    grid->addWidget(lab,0,0);
    KoUnit::Unit unit = doc->unit();
    resolutionX = new KoUnitDoubleSpinBox(tmpQGroupBox, 5.0,rect.width(),1,doc->getGridX() );
    resolutionX->setUnit( unit );

    grid->addWidget(resolutionX,1,0);

    lab=new QLabel(i18n("Vertical grid size:"), tmpQGroupBox);
    lab->setWhatsThis( i18n( "Set the space in millimeters between two vertical points on the grid. Default is 5 millimeters." ) );
    grid->addWidget(lab,2,0);

    resolutionY = new KoUnitDoubleSpinBox(tmpQGroupBox, 5.0,rect.height(),1,doc->getGridY() );
    resolutionY->setUnit( unit );

    grid->addWidget(resolutionY, 3, 0);
    box->addWidget(tmpQGroupBox);

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    box->addItem( spacer);


}

KCommand * KPrConfigureMiscPage::apply()
{
    config->setGroup( "Misc" );
    int newUndo=m_undoRedoLimit->value();
    KPrDocument* doc = m_pView->kPresenterDoc();
    if(newUndo!=m_oldNbRedo)
    {
        config->writeEntry("UndoRedo",newUndo);
        doc->setUndoRedoLimit(newUndo);
        m_oldNbRedo=newUndo;
    }
    config->writeEntry("PrintNotes", m_cbPrintNotes->isChecked());

    KMacroCommand * macroCmd=0L;
    bool b=m_displayLink->isChecked();
    bool b_new=doc->getVariableCollection()->variableSetting()->displayLink();
    if(b_new!=b)
    {
        if(!macroCmd)
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));

        KPrChangeVariableSettingsCommand *cmd=new KPrChangeVariableSettingsCommand(
            i18n("Change Display Link Command"), doc, b_new, b, KPrChangeVariableSettingsCommand::VS_DISPLAYLINK);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }

    b=m_underlineLink->isChecked();
    if(doc->getVariableCollection()->variableSetting()->underlineLink()!=b)
    {
        if(!macroCmd)
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));

        KPrChangeVariableSettingsCommand *cmd=new KPrChangeVariableSettingsCommand(
            i18n("Change Display Link Command"), doc, doc->getVariableCollection()->variableSetting()->underlineLink(),
            b, KPrChangeVariableSettingsCommand::VS_UNDERLINELINK);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }

    b=m_displayComment->isChecked();
    if(doc->getVariableCollection()->variableSetting()->displayComment()!=b)
    {
        if(!macroCmd)
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));

        KPrChangeVariableSettingsCommand *cmd=new KPrChangeVariableSettingsCommand(
            i18n("Change Display Link Command"), doc, doc->getVariableCollection()->variableSetting()->displayComment(),
            b, KPrChangeVariableSettingsCommand::VS_DISPLAYCOMMENT);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }

    b=m_displayFieldCode->isChecked();
    if(doc->getVariableCollection()->variableSetting()->displayFieldCode()!=b)
    {
        if(!macroCmd)
            macroCmd=new KMacroCommand(i18n("Change Display Field Code Command"));

        KPrChangeVariableSettingsCommand *cmd=new KPrChangeVariableSettingsCommand(
            i18n("Change Display Field Code Command"), doc, doc->getVariableCollection()->variableSetting()->displayComment(),
            b, KPrChangeVariableSettingsCommand::VS_DISPLAYFIELDCODE);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }

    doc->setGridValue( resolutionX->value(),
                       resolutionY->value(), true);
    doc->repaint( false );

    config->sync();

    return macroCmd;
}

void KPrConfigureMiscPage::slotDefault()
{
    m_undoRedoLimit->setValue(30);
    m_displayLink->setChecked(true);
    m_displayComment->setChecked(true);
    m_underlineLink->setChecked(true);
    m_displayFieldCode->setChecked( false );
    m_cbPrintNotes->setChecked(true);
    //KPrDocument* doc = m_pView->kPresenterDoc();

    resolutionY->setValue( MM_TO_POINT( 5.0 ));
    resolutionX->setValue( MM_TO_POINT( 5.0 ));
}

KPrConfigureDefaultDocPage::KPrConfigureDefaultDocPage(KPrView *_view, QWidget *parent, char *name )
    : QWidget( parent, name )
{
    Q3VBoxLayout *box = new Q3VBoxLayout( this, 0, 0 );

    m_pView=_view;
    config = KPrFactory::global()->config();
    KPrDocument *doc = m_pView->kPresenterDoc();
    oldAutoSaveValue =  doc->defaultAutoSave()/60;
    m_oldBackupFile = true;
    m_oldLanguage = doc->globalLanguage();
    m_oldHyphenation = doc->globalHyphenation();
    if( config->hasGroup("Interface") ) {
        config->setGroup( "Interface" );
        oldAutoSaveValue = config->readNumEntry( "AutoSave", oldAutoSaveValue );
        m_oldBackupFile=config->readBoolEntry("BackupFile",m_oldBackupFile);
        m_oldLanguage = config->readEntry( "language", m_oldLanguage );
        m_oldHyphenation = config->readBoolEntry( "hyphenation", m_oldHyphenation);
    }

    Q3GroupBox* gbDocumentDefaults = new Q3GroupBox( i18n("Document Defaults"), this, "GroupBox" );
    //gbDocumentDefaults->setMargin( KDialog::marginHint() );
    gbDocumentDefaults->setInsideSpacing( 5 );

    QWidget *fontContainer = new QWidget(gbDocumentDefaults);
    Q3GridLayout * fontLayout = new Q3GridLayout(fontContainer, 1, 3);

    fontLayout->setColStretch(0, 0);
    fontLayout->setColStretch(1, 1);
    fontLayout->setColStretch(2, 0);

    QLabel *fontTitle = new QLabel(i18n("Default font:"), fontContainer);

    font= new QFont( doc->defaultFont() );

    QString labelName = font->family() + ' ' + QString::number(font->pointSize());
    fontName = new QLabel(labelName, fontContainer);
    fontName->setFont(*font);
    fontName->setFrameStyle(Q3Frame::StyledPanel | Q3Frame::Sunken);

    QPushButton *chooseButton = new QPushButton(i18n("Choose..."), fontContainer);
    chooseButton->setWhatsThis( i18n( "Click here if you want to set a new font. The KDE default Select Font dialog will then be displayed." ) );
    connect(chooseButton, SIGNAL(clicked()), this, SLOT(selectNewDefaultFont()));

    fontLayout->addWidget(fontTitle, 0, 0);
    fontLayout->addWidget(fontName, 0, 1);
    fontLayout->addWidget(chooseButton, 0, 2);

    QWidget *languageContainer = new QWidget(gbDocumentDefaults);
    Q3GridLayout * languageLayout = new Q3GridLayout(languageContainer, 1, 3);

    languageLayout->setColStretch(0, 0);
    languageLayout->setColStretch(1, 1);

    QLabel *languageTitle = new QLabel(i18n("Global language:"), languageContainer);
    languageTitle->setWhatsThis( i18n( "Use this drop down box to determine the default language for the document. This setting is used by the hyphenation and spelling tools." ) );

    m_globalLanguage = new QComboBox( languageContainer );
    m_globalLanguage->insertStringList( KoGlobal::listOfLanguages() );
    m_globalLanguage->setCurrentText( KoGlobal::languageFromTag( doc->globalLanguage() ) );

    languageLayout->addWidget(languageTitle, 0, 0);
    languageLayout->addWidget(m_globalLanguage, 0, 1);

    m_autoHyphenation = new QCheckBox( i18n("Automatic hyphenation"), gbDocumentDefaults);
    m_autoHyphenation->setWhatsThis( i18n( "Check this box if you want KPresenter to automatically hyphenate long words when it determines the word wrap in text frames. This is not set by default." ) );
    m_autoHyphenation->setChecked( m_oldHyphenation );

    box->addWidget(gbDocumentDefaults);

    Q3GroupBox* gbDocumentSettings = new Q3GroupBox( i18n("Document Settings"), this );
    //gbDocumentSettings->setMargin( KDialog::marginHint() );
    //gbDocumentSettings->setInsideSpacing( KDialog::spacingHint() );

    m_createBackupFile = new QCheckBox( i18n("Create backup file"), gbDocumentSettings);
    m_createBackupFile->setWhatsThis( i18n( "If checked, this will create a .<name>.kpr.autosave.kpr in the folder where your file is. This backup file can then be used in case of a problem.\nThe backup file is updated every time you save your document or every time there is an autosave." ) );
    m_createBackupFile->setChecked( m_oldBackupFile );


    autoSave = new KIntNumInput( oldAutoSaveValue, gbDocumentSettings );
    autoSave->setRange( 0, 60, 1 );
    autoSave->setLabel( i18n("Autosave (min):") );
    autoSave->setSpecialValueText( i18n("No autosave") );
    autoSave->setSuffix( i18n("min") );
    autoSave->setWhatsThis( i18n( "You can use this to adjust how often KPresenter saves a temporary file. If you set this value to No autosave, KPresenter will not autosave. You can adjust the autosave from 1 to 60 minutes." ) );

    new QLabel(i18n("Starting page number:"), gbDocumentSettings);
    m_oldStartingPage=doc->getVariableCollection()->variableSetting()->startingPageNumber();
    m_variableNumberOffset=new KIntNumInput(gbDocumentSettings);
    m_variableNumberOffset->setRange(1, 9999, 1, false);
    m_variableNumberOffset->setValue(m_oldStartingPage);
    m_variableNumberOffset->setWhatsThis( i18n( "Here you can change the number for the first page. It is set to 1 by default.\nTip: this is helpful if you have split a single document into multiple files." ) );

    new QLabel(i18n("Tab stop:"), gbDocumentSettings);
    m_oldTabStopWidth = doc->tabStopValue();
    KoRect rect = doc->masterPage()->getPageRect();
    m_tabStopWidth = new KoUnitDoubleSpinBox( gbDocumentSettings , MM_TO_POINT(2), rect.width() ,0.1, m_oldTabStopWidth );

    m_tabStopWidth->setUnit( doc->unit() );

    m_tabStopWidth->setWhatsThis( i18n( "Each KPresenter document has a default set of tab stops. If you add tab stops to your document, the newly added tab stops override the default ones. You can use this text box to define the spacing between default tab stops. As an example, if you enter 1.5 in this text box, and the unit of measurement is in centimeters, the first default tab stop will be located 1.5 cm to the right of the frame's left-hand margin. The second default tab stop will be located at 3 cm from the left-hand margin, and so on." ) );

    box->addWidget(gbDocumentSettings);
    Q3GroupBox* gbDocumentCursor = new Q3GroupBox( i18n("Cursor"), this );
    //gbDocumentCursor->setMargin( KDialog::marginHint() );
    //gbDocumentCursor->setInsideSpacing( KDialog::spacingHint() );

    m_cursorInProtectedArea= new QCheckBox(i18n("Cursor in protected area"),gbDocumentCursor);
    m_cursorInProtectedArea->setChecked(doc->cursorInProtectedArea());
    m_cursorInProtectedArea->setWhatsThis( i18n( "When this box is checked and you click in a protected frame within your document, a cursor will appear. When this box is unchecked, and you click in a protected frame, no cursor will be visible." ) );

    m_directInsertCursor= new QCheckBox(i18n("Direct insert cursor"),gbDocumentCursor);
    m_directInsertCursor->setChecked(doc->insertDirectCursor());
    m_directInsertCursor->setWhatsThis( i18n( "When this box is checked, you can select a section of text using your mouse. Move the mouse to a new area in your document and click once with the middle mouse button and a copy of the selected text will be copied and pasted to the new location in the document.\nWhen this box is unchecked, in order to copy text from one section to another, you must select the text, manually copy the text to the clipboard, then manually paste the text in the new location." ) );
    box->addWidget(gbDocumentCursor);

    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    box->addItem(spacer);

}

KPrConfigureDefaultDocPage::~KPrConfigureDefaultDocPage()
{
    delete font;
}

KCommand *KPrConfigureDefaultDocPage::apply()
{
    config->setGroup( "Document defaults" );
    KPrDocument* doc = m_pView->kPresenterDoc();
    config->writeEntry("DefaultFont",font->toString());

    config->setGroup( "Interface" );
    int autoSaveVal = autoSave->value();
    if( autoSaveVal != oldAutoSaveValue ) {
        config->writeEntry( "AutoSave", autoSaveVal );
        m_pView->kPresenterDoc()->setAutoSave( autoSaveVal*60 );
        oldAutoSaveValue=autoSaveVal;
    }
    bool state =m_createBackupFile->isChecked();

    if(state!=m_oldBackupFile)
    {
        config->writeEntry( "BackupFile", state );
        doc->setBackupFile( state);
        m_oldBackupFile=state;
    }

    state = m_cursorInProtectedArea->isChecked();
    if ( state != doc->cursorInProtectedArea() )
    {
        config->writeEntry( "cursorInProtectArea", state );
        m_pView->kPresenterDoc()->setCursorInProtectedArea( state );
    }

    state = m_directInsertCursor->isChecked();
    if ( state != doc->insertDirectCursor() )
        doc->setInsertDirectCursor( state );

    //Laurent Todo add a message box to inform user that
    //global language will change after re-launch kword
    const QString lang = KoGlobal::tagOfLanguage( m_globalLanguage->currentText() );
    config->writeEntry( "language" , lang);
    m_oldLanguage = lang;
    //don't call this function otherwise we can have a textobject with
    // a default language and other textobject with other default language.
    //doc->setGlobalLanguage( lang );


    state = m_autoHyphenation->isChecked();
    config->writeEntry( "hyphenation", state  );
    m_oldHyphenation = state;

    KMacroCommand *macro = 0L;
    int newStartingPage=m_variableNumberOffset->value();
    if(newStartingPage!=m_oldStartingPage)
    {
        macro = new KMacroCommand( i18n("Change Starting Page Number") );
        KPrChangeStartingPageCommand *cmd = new KPrChangeStartingPageCommand( i18n("Change Starting Page Number"), doc, m_oldStartingPage,newStartingPage );
        cmd->execute();
        macro->addCommand( cmd);
        m_oldStartingPage=newStartingPage;
    }
    double newTabStop = m_tabStopWidth->value();
    if ( newTabStop != m_oldTabStopWidth)
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change Tab Stop Value") );
        KPrChangeTabStopValueCommand *cmd = new KPrChangeTabStopValueCommand( i18n("Change Tab Stop Value"), m_oldTabStopWidth, newTabStop, doc);
        cmd->execute();
        macro->addCommand( cmd );
        m_oldTabStopWidth = newTabStop;
    }
    return macro;
}

void KPrConfigureDefaultDocPage::slotDefault()
{
    autoSave->setValue( m_pView->kPresenterDoc()->defaultAutoSave()/60 );
    m_variableNumberOffset->setValue(1);
    m_cursorInProtectedArea->setChecked(true);
    m_tabStopWidth->setValue( MM_TO_POINT(15));
    m_createBackupFile->setChecked( true );
    m_directInsertCursor->setChecked( false );
    m_globalLanguage->setCurrentText( KoGlobal::languageFromTag( KGlobal::locale()->language() ) );
    m_autoHyphenation->setChecked( false );
}

void KPrConfigureDefaultDocPage::selectNewDefaultFont() {
    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
    KFontDialog dlg( this, false, true, list, true );
    dlg.setFont(*font);
    int result = dlg.exec();
    if (KDialog::Accepted == result) {
        delete font;
        font = new QFont(dlg.font());
        fontName->setText(font->family() + ' ' + QString::number(font->pointSize()));
        fontName->setFont(*font);
        m_pView->kPresenterDoc()->setDefaultFont( *font );
    }
}

KPrConfigureToolsPage::KPrConfigureToolsPage( KPrView *_view, QWidget *parent, char *name )
    : QWidget( parent, name )
{
    Q3VBoxLayout *box = new Q3VBoxLayout( this, 0, 0 );

    m_pView = _view;
    config = KPrFactory::global()->config();
    m_pView->getCanvas()->deSelectAllObj();

    QTabWidget *tab = new QTabWidget(this);

    KoPenCmd::Pen pen( m_pView->getPen(), m_pView->getLineBegin(), m_pView->getLineEnd() );
    m_confPenDia = new KPrPenStyleWidget(tab, 0, pen, true );
    tab->addTab( m_confPenDia, i18n( "Outl&ine" ) );

    KPrBrushCmd::Brush brush( m_pView->getBrush(),
                           m_pView->getGColor1(),
                           m_pView->getGColor2(),
                           m_pView->getGType(),
                           m_pView->getFillType(),
                           m_pView->getGUnbalanced(),
                           m_pView->getGXFactor(),
                           m_pView->getGYFactor() );
    m_brushProperty = new KPrBrushProperty( this, 0, brush );
    tab->addTab( m_brushProperty, i18n( "&Fill" ) );

    KPrRectValueCmd::RectValues rectValues;
    rectValues.xRnd = m_pView->getRndX();
    rectValues.yRnd = m_pView->getRndY();
    m_rectProperty = new KPrRectProperty( this, 0, rectValues );
    tab->addTab( m_rectProperty, i18n( "&Rectangle" ) );

    KPrPolygonSettingCmd::PolygonSettings polygonSettings;
    polygonSettings.checkConcavePolygon = m_pView->getCheckConcavePolygon();
    polygonSettings.cornersValue = m_pView->getCornersValue();
    polygonSettings.sharpnessValue = m_pView->getSharpnessValue();
    m_polygonProperty = new KPrPolygonProperty( this, 0, polygonSettings );
    tab->addTab( m_polygonProperty, i18n( "Polygo&n" ) );

    KPrPieValueCmd::PieValues pieValues;
    pieValues.pieType = m_pView->getPieType();
    pieValues.pieAngle = m_pView->getPieAngle();
    pieValues.pieLength = m_pView->getPieLength();
    m_pieProperty = new KPrPieProperty( this, 0, pieValues );
    tab->addTab( m_pieProperty, i18n( "&Pie" ) );

    box->addWidget(tab);
}

KPrConfigureToolsPage::~KPrConfigureToolsPage()
{
}

void KPrConfigureToolsPage::apply()
{
    KoPenCmd::Pen pen = m_confPenDia->getPen();
    m_pView->setPen( pen.pen );
    m_pView->setLineBegin( pen.lineBegin );
    m_pView->setLineEnd( pen.lineEnd );
    m_pView->getActionPenColor()->setCurrentColor( pen.pen.color() );

    KPrBrushCmd::Brush brush = m_brushProperty->getBrush();
    m_pView->setBrush( brush.brush );
    m_pView->setFillType( brush.fillType );
    m_pView->setGColor1( brush.gColor1 );
    m_pView->setGColor2( brush.gColor2 );
    m_pView->setGType( brush.gType );
    m_pView->setGUnbalanced( brush.unbalanced );
    m_pView->setGXFactor( brush.xfactor );
    m_pView->setGYFactor( brush.yfactor );
    m_pView->getActionBrushColor()->setCurrentColor( brush.brush.color() );

    KPrRectValueCmd::RectValues rectValues = m_rectProperty->getRectValues();
    m_pView->setRndX( rectValues.xRnd );
    m_pView->setRndY( rectValues.yRnd );

    KPrPolygonSettingCmd::PolygonSettings polygonSettings = m_polygonProperty->getPolygonSettings();
    m_pView->setCheckConcavePolygon( polygonSettings.checkConcavePolygon );
    m_pView->setCornersValue( polygonSettings.cornersValue );
    m_pView->setSharpnessValue( polygonSettings.sharpnessValue );

    KPrPieValueCmd::PieValues pieValues = m_pieProperty->getPieValues();
    m_pView->setPieType( pieValues.pieType );
    m_pView->setPieAngle( pieValues.pieAngle );
    m_pView->setPieLength( pieValues.pieLength );

    //TODO set pen brush in m_rectProperty
    //TODO set pen brush in m_polygonProperty
    //TODO set pen brush in m_pieProperty
}

void KPrConfigureToolsPage::slotDefault()
{
    KoPenCmd::Pen pen( KoPen(Qt::black, 1.0, Qt::SolidLine), L_NORMAL, L_NORMAL );
    m_confPenDia->setPen( pen );
    m_pView->getActionPenColor()->setCurrentColor( pen.pen.color() );

    KPrBrushCmd::Brush brush( QBrush( Qt::white, Qt::SolidPattern ), Qt::red, Qt::green,
                           BCT_GHORZ, FT_BRUSH, false, 100, 100 );
    m_brushProperty->setBrush( brush );
    m_pView->getActionBrushColor()->setCurrentColor( brush.brush.color() );

    KPrRectValueCmd::RectValues rectValues;
    rectValues.xRnd = 0;
    rectValues.yRnd = 0;
    m_rectProperty->setRectValues( rectValues );

    KPrPolygonSettingCmd::PolygonSettings polygonSettings;
    polygonSettings.checkConcavePolygon = false;
    polygonSettings.cornersValue = 3;
    polygonSettings.sharpnessValue = 0;
    m_polygonProperty->setPolygonSettings( polygonSettings );

    KPrPieValueCmd::PieValues pieValues;
    pieValues.pieType = PT_PIE;
    pieValues.pieAngle = 45 * 16;
    pieValues.pieLength = 270 * 16;
}

KPrConfigurePathPage::KPrConfigurePathPage( KPrView *_view, QWidget *parent, char *name )
    : QWidget( parent, name )
{
    Q3VBoxLayout *box = new Q3VBoxLayout( this, 0, 0 );

    m_pView=_view;
    KPrDocument* doc = m_pView->kPresenterDoc();
    config = KPrFactory::global()->config();

    m_pPathView = new K3ListView( this );
    m_pPathView->setResizeMode(Q3ListView::NoColumn);
    m_pPathView->addColumn( i18n( "Type" ) );
    m_pPathView->addColumn( i18n( "Path" ) );
    (void) new Q3ListViewItem( m_pPathView, i18n("Picture Path"),doc->picturePath() );
    (void) new Q3ListViewItem( m_pPathView, i18n("Backup Path"),doc->backupPath() );
    m_pPathView->setWhatsThis( i18n( "There are two paths that are set here: the Backup Path and the Picture Path. The Backup path is the folder where your backup files are saved and the Picture Path is the folder where your pictures are saved." ) );
    box->addWidget(m_pPathView);

    m_modifyPath = new QPushButton( i18n("Modify Path..."), this);
    connect( m_modifyPath, SIGNAL( clicked ()), this, SLOT( slotModifyPath()));
    connect( m_pPathView, SIGNAL( doubleClicked (Q3ListViewItem *, const QPoint &, int )),
             this, SLOT( slotModifyPath()));
    connect( m_pPathView, SIGNAL( selectionChanged ( Q3ListViewItem * )),
             this, SLOT( slotSelectionChanged(Q3ListViewItem * )));
    slotSelectionChanged(m_pPathView->currentItem());
    m_modifyPath->setWhatsThis( i18n( "When you click this button, a small dialog will appear and, if Default path is unchecked, you can either enter a path yourself or choose one using the standard KDE file dialog." ) );
    box->addWidget(m_modifyPath);

}

void KPrConfigurePathPage::slotSelectionChanged(Q3ListViewItem * item)
{
    m_modifyPath->setEnabled( item );
}

void KPrConfigurePathPage::slotModifyPath()
{
    Q3ListViewItem *item = m_pPathView->currentItem ();
    if ( item )
    {
        if ( item->text(0)==i18n("Picture Path"))
        {
            KUrlRequesterDlg * dlg = new KUrlRequesterDlg( item->text(1), 0L);
            dlg->fileDialog()->setMode(KFile::Directory | KFile::LocalOnly);
            if ( dlg->exec() )
                item->setText( 1, dlg->selectedURL().path());
            delete dlg;
        }
        else if ( item->text(0)==i18n("Backup Path"))
        {
            KoChangePathDia *dlg = new KoChangePathDia( item->text(1), 0L,
                                                        "backup path" );
            if (dlg->exec() )
                item->setText(1, dlg->newPath());
            delete dlg;
        }
    }
}

void KPrConfigurePathPage::slotDefault()
{
    Q3ListViewItem * item = m_pPathView->findItem(i18n("Picture Path"), 0);
    if ( item )
        item->setText(1, KGlobalSettings::documentPath());
    item = m_pPathView->findItem(i18n("Backup Path"), 0);
    if ( item )
        item->setText(1, QString::null );
}

void KPrConfigurePathPage::apply()
{
    Q3ListViewItem *item = m_pPathView->findItem(i18n("Backup Path"), 0);
    if ( item )
    {
        QString res = item->text(1 );
        if ( res != m_pView->kPresenterDoc()->backupPath())
        {
            config->setGroup( "Kpresenter Path" );
            m_pView->kPresenterDoc()->setBackupPath( res );
            config->writePathEntry( "backup path",res );
        }
    }
    item = m_pPathView->findItem(i18n("Picture Path"), 0);
    if ( item )
    {
        QString res = item->text(1 );
        if ( res != m_pView->kPresenterDoc()->picturePath())
        {
            config->setGroup( "Kpresenter Path" );
            m_pView->kPresenterDoc()->setPicturePath( res );
            config->writePathEntry( "picture path",res );
        }
    }
}

////

#include "KPrConfig.moc"
