/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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

#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <KoUnitWidgets.h>
#include <knuminput.h>
#include <knumvalidator.h>
#include <kfontdialog.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kvbox.h>
#include <QCheckBox>
#include <QLabel>

#include <QComboBox>


#include <QPushButton>
#include <q3listbox.h>
#include <QLineEdit>
#include <QLayout>
//Added by qt3to4:
#include <QPixmap>
#include <Q3GridLayout>
#include <Q3Frame>
#include <Q3GroupBox>

#include "KWConfig.h"
#include "KWView.h"
#include "KWFrameSet.h"
#include "KWDocument.h"
#include "KWCanvas.h"
#include "KWGUI.h"
#include "KWViewMode.h"
#include "KWCommand.h"
#include "KWVariable.h"
#include "KoEditPath.h"
#include "KWPageManager.h"
#include "KWPage.h"

#include <KoVariable.h>
#include <kformulaconfigpage.h>

#include <kspell2/configwidget.h>
#include <kspell2/settings.h>
#include <kspell2/broker.h>
using namespace KSpell2;

#include <float.h>
#include <kmessagebox.h>
#include <k3listview.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kurlrequesterdlg.h>
#include <kfiledialog.h>
#include <QTabWidget>
#include <keditlistbox.h>
#include <KoGlobal.h>
#include <kvbox.h>

// little helper stolen from kmail
// (Note: KDialogBase should have version of the methods that take a QString for the icon name)
static inline QPixmap loadIcon( const char * name ) {
  return KGlobal::instance()->iconLoader()
    ->loadIcon( QString::fromLatin1(name), K3Icon::NoGroup, K3Icon::SizeMedium );
}

KWConfig::KWConfig( KWView* parent )
  : KPageDialog(parent)

{
    setCaption( i18n("Configure KWord") );
    setButtons( KDialog::Ok | KDialog::Apply | KDialog::Cancel| KDialog::Default );
    setDefaultButton( KDialog::Ok );
    setFaceType( KPageDialog::List );
    KVBox *page2 = new KVBox();
    KPageWidgetItem *pageItem = new KPageWidgetItem( page2, i18n("Interface") );
    pageItem->setHeader( i18n("Interface Settings") );
    pageItem->setIcon( loadIcon("configure") );
    addPage( pageItem );

  m_interfacePage=new ConfigureInterfacePage(parent, page2);

  KVBox *page4 = new KVBox();
  pageItem = new KPageWidgetItem( page4, i18n("Document") );
  pageItem->setHeader( i18n("Document Settings") );
  pageItem->setIcon( loadIcon("kword_kwd") );
  addPage( pageItem );


  m_defaultDocPage=new ConfigureDefaultDocPage(parent, page4);

  KVBox *page = new KVBox();
  pageItem = new KPageWidgetItem( page, i18n("Spelling") );
  pageItem->setHeader( i18n("Spell Checker Behavior") );
  pageItem->setIcon( loadIcon("spellcheck") );
  addPage( pageItem );

  m_spellPage = new ConfigureSpellPage(parent, page);

  KVBox *page5 = new KVBox();
  pageItem = new KPageWidgetItem( page5, i18n("Formula") );
  pageItem->setHeader( i18n("Formula Defaults") );
  pageItem->setIcon( loadIcon("kformula") );
  addPage( pageItem );
  m_formulaPage=new KFormula::ConfigurePage( parent->kWordDocument()->formulaDocument( false ),
                                             this, KWFactory::instance()->config(), page5 );

  KVBox *page3 = new KVBox();
  pageItem = new KPageWidgetItem( page3, i18n("Misc") );
  pageItem->setHeader( i18n("Misc Settings") );
  pageItem->setIcon( loadIcon("misc") );
  addPage( pageItem );

  m_miscPage=new ConfigureMiscPage(parent, page3);

  KVBox *page6 = new KVBox();
  pageItem = new KPageWidgetItem( page6, i18n("Path") );
  pageItem->setHeader( i18n("Path Settings") );
  pageItem->setIcon( loadIcon("path") );
  addPage( pageItem );

  m_pathPage=new ConfigurePathPage(parent, page6);

  m_doc = parent->kWordDocument();
  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));

  connect( m_interfacePage, SIGNAL( unitChanged( int ) ), SLOT( unitChanged( int ) ) );
  unitChanged( parent->kWordDocument()->unit() );
}

void KWConfig::unitChanged( int u )
{
    KoUnit::Unit unit = static_cast<KoUnit::Unit>(u);
    //m_spellPage->setUnit( unit );
    m_interfacePage->setUnit( unit );
    m_miscPage->setUnit( unit );
    m_defaultDocPage->setUnit( unit );
    //m_formulaPage->setUnit( unit );
    //m_pathPage->setUnit( unit );
}

void KWConfig::openPage(int flags)
{
#warning "kde4: port it"
#if 0
    if(flags & KW_KSPELL)
        showPage( 2 );
    else if(flags & KP_INTERFACE)
        showPage(0 );
    else if(flags & KP_MISC)
        showPage(4);
    else if(flags & KP_DOCUMENT)
        showPage(2 );
    else if(flags & KP_FORMULA)
        showPage(3);
    else if ( flags & KP_PATH )
        showPage(4);
#endif
}

void KWConfig::slotApply()
{
    KMacroCommand *macro = 0L;
    if (m_spellPage) m_spellPage->apply();
    m_interfacePage->apply();
    m_pathPage->apply();
    KCommand * cmd = m_miscPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change Config"));
        macro->addCommand(cmd);
    }

    cmd=m_defaultDocPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change Config"));

        macro->addCommand( cmd );
    }
    m_formulaPage->apply();
    if (macro)
        m_doc->addCommand( macro );
    KWFactory::instance()->config()->sync();
}

void KWConfig::slotDefault()
{
#warning "kde4: port it"
#if 0
    switch(activePageIndex())
    {
    case 0:
        m_interfacePage->slotDefault();
        break;
    case 1:
        m_defaultDocPage->slotDefault();
        break;
    case 2:
        if (m_spellPage) m_spellPage->slotDefault();
        break;
    case 3:
        m_formulaPage->slotDefault();
        break;
    case 4:
        m_miscPage->slotDefault();
        break;
    case 5:
        m_pathPage->slotDefault();
        break;
    default:
        break;
    }
#endif
}

////

ConfigureSpellPage::ConfigureSpellPage( KWView *view, KVBox *box, char *name )
    : QObject( box->parent(), name )
{
    m_pView=view;
    config = KWFactory::instance()->config();
    m_spellConfigWidget = new ConfigWidget( view->loader(), box );
    m_spellConfigWidget->setBackgroundCheckingButtonShown( true );
    m_spellConfigWidget->layout()->setMargin( 0 );
}

void ConfigureSpellPage::apply()
{
  KWDocument* doc = m_pView->kWordDocument();

  m_spellConfigWidget->save();

  m_pView->kWordDocument()->setSpellCheckIgnoreList(
      m_pView->loader()->settings()->currentIgnoreList() );
  //FIXME reactivate just if there are changes.
  doc->enableBackgroundSpellCheck( m_pView->loader()->settings()->backgroundCheckerEnabled() );
  doc->reactivateBgSpellChecking();
}

void ConfigureSpellPage::slotDefault()
{
    m_spellConfigWidget->slotDefault();
}

ConfigureInterfacePage::ConfigureInterfacePage( KWView *view, KVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=view;
    config = KWFactory::instance()->config();
    Q3GroupBox * gbInterfaceGroup = new Q3GroupBox(1, Qt::Horizontal, i18n("Interface"), box, "GroupBox" );
    //gbInterfaceGroup->setMargin( KDialog::marginHint() );
    gbInterfaceGroup->setInsideSpacing( KDialog::spacingHint() );

    double ptGridX=MM_TO_POINT(5.0 );
    double ptGridY=MM_TO_POINT(5.0 );
    double ptIndent = MM_TO_POINT(10.0);
    bool oldShowStatusBar = true;
    bool oldPgUpDownMovesCaret = false;
    bool oldShowScrollBar = true;
    oldNbRecentFiles=10;
    int nbPagePerRow=4;
    KoUnit::Unit unit = m_pView->kWordDocument()->unit();
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        ptGridX=config->readEntry("GridX", ptGridX);
        ptGridY=config->readEntry("GridY", ptGridY);
        ptIndent = config->readEntry("Indent", ptIndent);
        oldNbRecentFiles=config->readEntry("NbRecentFile", oldNbRecentFiles);
        nbPagePerRow=config->readEntry("nbPagePerRow", nbPagePerRow);
        oldShowStatusBar = config->readEntry( "ShowStatusBar", true );
        oldPgUpDownMovesCaret = config->readEntry( "PgUpDownMovesCaret", false );
        oldShowScrollBar = config->readEntry("ShowScrollBar", true);
    }

    KHBox *hbUnit = new KHBox(gbInterfaceGroup);
    hbUnit->setSpacing(KDialog::spacingHint());
    QLabel *unitLabel= new QLabel(i18n("&Units:"),hbUnit);

    m_unitCombo = new QComboBox( hbUnit );
    m_unitCombo->insertStringList( KoUnit::listOfUnitName() );
    connect(m_unitCombo, SIGNAL(activated(int)), this, SIGNAL(unitChanged(int)));
    unitLabel->setBuddy( m_unitCombo );
    QString unitHelp = i18n("Select the unit type used every time a distance or width/height "
                            "is displayed or entered. This one setting is for the whole of KWord: all dialogs, the rulers etc. "
                            "Note that KWord documents specify the unit which was used to create them, so this setting "
                            "only affects this document and all documents that will be created later.");
    unitLabel->setWhatsThis( unitHelp );
    m_unitCombo->setWhatsThis( unitHelp );

    showStatusBar = new QCheckBox(i18n("Show &status bar"),gbInterfaceGroup);
    showStatusBar->setChecked(oldShowStatusBar);
    showStatusBar->setWhatsThis( i18n("Show or hide the status bar. If enabled, the status bar is shown at the bottom, which displays various information."));

    showScrollBar = new QCheckBox( i18n("Show s&crollbar"), gbInterfaceGroup);
    showScrollBar->setChecked(oldShowScrollBar);
    showScrollBar->setWhatsThis( i18n("Show or hide the scroll bar. If enabled, the scroll bar is shown on the right and lets you scroll up and down, which is useful for navigating through the document."));

    pgUpDownMovesCaret = new QCheckBox(i18n("PageUp/PageDown &moves the caret"),gbInterfaceGroup);
    pgUpDownMovesCaret->setChecked(oldPgUpDownMovesCaret);
    pgUpDownMovesCaret->setWhatsThis( i18n(
                         "If this option is enabled, the PageUp and PageDown keys "
                         "move the text caret, as in other KDE applications. "
                         "If it is disabled, they move the scrollbars, as in most other word processors." ) );

    KHBox* hbRecent = new KHBox( gbInterfaceGroup );
    QString recentHelp = i18n("The number of files remembered in the file open dialog and in the "
                              "recent files menu item.");
    QLabel* labelRecent = new QLabel( i18n("Number of recent &files:"), hbRecent );
    labelRecent->setWhatsThis( recentHelp );
    recentFiles=new KIntNumInput( oldNbRecentFiles, hbRecent );
    recentFiles->setRange(1, 20, 1);
    labelRecent->setBuddy( recentFiles );
    recentFiles->setWhatsThis( recentHelp );

    KHBox* hbGridX = new KHBox( gbInterfaceGroup );
    QString gridxHelp = i18n("The grid size on which frames, tabs and other content snaps while "
                             "moving and scaling.");
    QLabel* labelGridX = new QLabel( i18n("&Horizontal grid size:"), hbGridX );
    labelGridX->setWhatsThis( gridxHelp );
    gridX=new KoUnitDoubleSpinBox( hbGridX,
                                   0.1,
                                   50,
                                   0.1,
                                   ptGridX,
                                   unit );
    labelGridX->setBuddy( gridX );
    gridX->setWhatsThis( gridxHelp );

    KHBox* hbGridY = new KHBox( gbInterfaceGroup );
    QString gridyHelp = i18n("The grid size on which frames and other content snaps while "
                             "moving and scaling.");
    QLabel* labelGridY = new QLabel( i18n("&Vertical grid size:"), hbGridY );
    labelGridY->setWhatsThis( gridyHelp );
    gridY=new KoUnitDoubleSpinBox( hbGridY,
                                   0.1,
                                   50,
                                   0.1,
                                   ptGridY,
                                   unit );
    labelGridY->setBuddy( gridY );

    gridY->setWhatsThis( gridyHelp );

    KHBox* hbIndent = new KHBox( gbInterfaceGroup );
    QString indentHelp = i18n("Configure the indent width used when using the 'Increase' "
                              "or 'Decrease' indentation buttons on a paragraph.<p>The lower the value, "
                              "the more often the buttons will have to be pressed to gain the same "
                              "indentation.");
    QLabel* labelIndent = new QLabel( i18n("&Paragraph indent by toolbar buttons:"), hbIndent );
    labelIndent->setWhatsThis( indentHelp );
    indent = new KoUnitDoubleSpinBox( hbIndent,
                                      0.1,
                                      5000,
                                      0.1,
                                      ptIndent,
                                      unit );
    labelIndent->setBuddy( indent );
    indent->setWhatsThis( indentHelp );

    KHBox* hbPagePerRow = new KHBox( gbInterfaceGroup );
    QString pagePerRowHelp = i18n("After selecting Preview Mode (from the \"View\" menu,) "
                                  "this is the number of pages KWord will "
                                  "position on one horizontal row.");
    QLabel* labelPagePerRow = new QLabel( i18n("Number of pa&ges per row in preview mode:" ), hbPagePerRow );
    labelPagePerRow->setWhatsThis( pagePerRowHelp );
    m_nbPagePerRow=new KIntNumInput( 0, nbPagePerRow, hbPagePerRow );
    m_nbPagePerRow->setRange(1, 10, 1);
    labelPagePerRow->setBuddy( m_nbPagePerRow );
    hbPagePerRow->setStretchFactor( m_nbPagePerRow, 1 );
    m_nbPagePerRow ->setWhatsThis( pagePerRowHelp );
}

void ConfigureInterfacePage::apply()
{
    KWDocument * doc = m_pView->kWordDocument();
    double valX = qMax( 0.1, gridX->value() );
    double valY = qMax( 0.1, gridY->value() );
    int nbRecent=recentFiles->value();

    bool statusBar=showStatusBar->isChecked();
    bool scrollBar = showScrollBar->isChecked();
    config->setGroup( "Interface" );
    bool updateView = false;
    if(valX!=doc->gridX())
    {
        config->writeEntry( "GridX", valX );
        doc->setGridX(valX);
        updateView= true;
    }
    if(valY!=doc->gridY())
    {
        config->writeEntry( "GridY", valY );
        doc->setGridY(valY);
        updateView= true;
    }
    double newIndent = indent->value();
    if( newIndent != doc->indentValue() )
    {
        config->writeEntry( "Indent", newIndent );
        doc->setIndentValue( newIndent );
    }
    if(nbRecent!=oldNbRecentFiles)
    {
        config->writeEntry( "NbRecentFile", nbRecent);
        m_pView->changeNbOfRecentFiles(nbRecent);
    }
    bool refreshGUI= false;

    if( statusBar != doc->showStatusBar() )
    {
        config->writeEntry( "ShowStatusBar", statusBar );
        doc->setShowStatusBar( statusBar );
        refreshGUI=true;
    }

    if( scrollBar != doc->showScrollBar() )
    {
        config->writeEntry( "ShowScrollBar", scrollBar );
        doc->setShowScrollBar( scrollBar );
        refreshGUI=true;
    }

    bool b = pgUpDownMovesCaret->isChecked();
    if ( b != doc->pgUpDownMovesCaret() )
    {
        config->writeEntry( "PgUpDownMovesCaret", b );
        doc->setPgUpDownMovesCaret( b );
    }

    if( refreshGUI )
        doc->reorganizeGUI();


    int nbPageByRow=m_nbPagePerRow->value();
    if(nbPageByRow!=doc->nbPagePerRow())
    {
        config->writeEntry("nbPagePerRow",nbPageByRow);
        m_pView->getGUI()->canvasWidget()->viewMode()->setPagesPerRow(nbPageByRow);
        doc->setNbPagePerRow(nbPageByRow);
        //m_pView->getGUI()->canvasWidget()->refreshViewMode();
        //necessary to recreate new view because in switchViewMode
        //we delete viewmode that we want to apply (LM)
        // This needs to be cleaned up .... (DF)
        doc->switchViewMode( doc->viewModeType() ); // force a refresh
    }

    config->setGroup( "Misc" );
    KoUnit::Unit unit = static_cast<KoUnit::Unit>( m_unitCombo->currentItem() );
    // It's already been set in the document, see unitChanged
    config->writeEntry( "Units", KoUnit::unitName( unit ) );
    if ( updateView )
        doc->repaintAllViews(false);
}

void ConfigureInterfacePage::setUnit( KoUnit::Unit unit )
{
    m_unitCombo->blockSignals( true );
    m_unitCombo->setCurrentItem( unit );
    m_unitCombo->blockSignals( false );
    // We need to set it in the doc immediately, because much code here uses doc->unit()
    m_pView->kWordDocument()->setUnit( unit );

    gridX->setUnit( unit );
    gridY->setUnit( unit );
    indent->setUnit( unit );
}

void ConfigureInterfacePage::slotDefault()
{
    KWDocument * doc = m_pView->kWordDocument();
    m_unitCombo->setCurrentItem( KoUnit::U_CM );
    emit unitChanged( m_unitCombo->currentItem() );
    gridX->setValue( KoUnit::toUserValue( MM_TO_POINT( 5.0 ),doc->unit() ) );
    gridY->setValue( KoUnit::toUserValue( MM_TO_POINT( 5.0 ),doc->unit() ) );
    m_nbPagePerRow->setValue(4);
    double newIndent = KoUnit::toUserValue( MM_TO_POINT( 10 ), doc->unit() );
    indent->setValue( newIndent );
    recentFiles->setValue(10);
    showStatusBar->setChecked(true);
    pgUpDownMovesCaret->setChecked(false);
    showScrollBar->setChecked( true);
}

////

ConfigureMiscPage::ConfigureMiscPage( KWView *view, KVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=view;
    config = KWFactory::instance()->config();
    Q3GroupBox * gbMiscGroup = new Q3GroupBox(1, Qt::Horizontal, i18n("Misc"), box, "GroupBox" );
    //gbMiscGroup->setMargin( KDialog::marginHint() );
    gbMiscGroup->setInsideSpacing( KDialog::spacingHint() );

    m_oldNbRedo=30;

    // Don't load the unit from config file because the unit can come from the kword file
    // => unit can be different from config file

    if( config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        m_oldNbRedo=config->readEntry("UndoRedo",m_oldNbRedo);
    }

    KHBox* hbUndoRedo = new KHBox( gbMiscGroup );
    QLabel* labelUndoRedo = new QLabel( i18n("Undo/&redo limit:"), hbUndoRedo );
    QString undoHelp = i18n("Limit the number of undo/redo actions remembered. "
                            "A lower value helps to save memory, a higher value allows "
                            "you to undo and redo more editing steps.");
    m_undoRedoLimit=new KIntNumInput( m_oldNbRedo, hbUndoRedo );
    m_undoRedoLimit->setRange(1, 100, 1);
    labelUndoRedo->setBuddy( m_undoRedoLimit );
    m_undoRedoLimit->setWhatsThis( undoHelp );
    labelUndoRedo->setWhatsThis( undoHelp );

    KWDocument* doc = m_pView->kWordDocument();
    m_displayLink=new QCheckBox(i18n("Display &links"),gbMiscGroup);
    m_displayLink->setChecked(doc->variableCollection()->variableSetting()->displayLink());
    m_displayLink->setWhatsThis( i18n("If enabled, a link is highlighted as such and is clickable.\n\n"
                                         "You can insert a link from the Insert menu."));
    m_underlineLink=new QCheckBox(i18n("&Underline all links"),gbMiscGroup);
    m_underlineLink->setChecked(doc->variableCollection()->variableSetting()->underlineLink());
    m_underlineLink->setWhatsThis( i18n("If enabled, a link is underlined."));

    m_displayComment=new QCheckBox(i18n("Display c&omments"),gbMiscGroup);
    m_displayComment->setChecked(doc->variableCollection()->variableSetting()->displayComment());
    m_displayComment->setWhatsThis( i18n("If enabled, comments are indicated by a small yellow box.\n\n"
                                            "You can show and edit a comment from the context menu."));

    m_displayFieldCode=new QCheckBox(i18n("Display field code"),gbMiscGroup);
    m_displayFieldCode->setChecked(doc->variableCollection()->variableSetting()->displayFieldCode());
    m_displayFieldCode->setWhatsThis( i18n("If enabled, the type of link is displayed instead "
                                              "of displaying the link text.\n\n"
                                              "There are various types of link that can be inserted, "
                                              "such as hyperlinks, files, mail, news and bookmarks."));


    Q3GroupBox * gbViewFormatting = new Q3GroupBox(1, Qt::Horizontal, i18n("View Formatting"), box, "view_formatting" );
    gbViewFormatting->setWhatsThis( i18n("These settings can be used to select the formatting "
                                            "characters that should be shown.\n\n"
                                            "Note that the selected formatting characters are only "
                                            "shown if formatting characters are enabled in general, "
                                            "which can be done from the View menu."));
    //gbViewFormatting->setMargin( KDialog::marginHint() );
    gbViewFormatting->setInsideSpacing( KDialog::spacingHint() );

    m_oldFormattingEndParag = doc->viewFormattingEndParag();
    m_oldFormattingSpace = doc->viewFormattingSpace();
    m_oldFormattingTabs = doc->viewFormattingTabs();
    m_oldFormattingBreak = doc->viewFormattingBreak();

    m_cbViewFormattingEndParag = new QCheckBox( i18n("View formatting end paragraph"), gbViewFormatting);
    m_cbViewFormattingEndParag->setChecked(m_oldFormattingEndParag);

    m_cbViewFormattingSpace = new QCheckBox( i18n("View formatting space"), gbViewFormatting);
    m_cbViewFormattingSpace->setChecked(m_oldFormattingSpace);

    m_cbViewFormattingTabs = new QCheckBox( i18n("View formatting tabs"), gbViewFormatting);
    m_cbViewFormattingTabs->setChecked(m_oldFormattingTabs);

    m_cbViewFormattingBreak = new QCheckBox( i18n("View formatting break"), gbViewFormatting);
    m_cbViewFormattingBreak->setChecked(m_oldFormattingBreak);
}

ConfigureDefaultDocPage::~ConfigureDefaultDocPage()
{
    delete font;
}

KCommand *ConfigureMiscPage::apply()
{
    KWDocument * doc = m_pView->kWordDocument();
    config->setGroup( "Misc" );
    int newUndo=m_undoRedoLimit->value();
    if(newUndo!=m_oldNbRedo)
    {
        config->writeEntry("UndoRedo",newUndo);
        doc->setUndoRedoLimit(newUndo);
        m_oldNbRedo=newUndo;
    }
    KMacroCommand * macroCmd=0L;
    bool b=m_displayLink->isChecked();
    if(doc->variableCollection()->variableSetting()->displayLink()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Link Command"), doc, doc->variableCollection()->variableSetting()->displayLink() ,b, KWChangeVariableSettingsCommand::VS_DISPLAYLINK);

        cmd->execute();
        macroCmd->addCommand(cmd);
    }
    b=m_underlineLink->isChecked();
    if(doc->variableCollection()->variableSetting()->underlineLink()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Link Command"), doc, doc->variableCollection()->variableSetting()->underlineLink() ,b, KWChangeVariableSettingsCommand::VS_UNDERLINELINK);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }

    b=m_displayComment->isChecked();
    if(doc->variableCollection()->variableSetting()->displayComment()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Link Command"), doc, doc->variableCollection()->variableSetting()->displayComment() ,b, KWChangeVariableSettingsCommand::VS_DISPLAYCOMMENT);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }
    b=m_displayFieldCode->isChecked();
    if(doc->variableCollection()->variableSetting()->displayFieldCode()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Field Code Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Field Code Command"), doc, doc->variableCollection()->variableSetting()->displayFieldCode() ,b, KWChangeVariableSettingsCommand::VS_DISPLAYFIELDCODE);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }

    bool state =m_cbViewFormattingEndParag->isChecked();
    bool needRepaint = false;
    if ( state != m_oldFormattingEndParag )
    {
        doc->setViewFormattingEndParag(state);
        needRepaint = true;
        m_oldFormattingEndParag = state;
    }
    state = m_cbViewFormattingSpace->isChecked();
    if ( state != m_oldFormattingSpace)
    {
        doc->setViewFormattingSpace(state);
        needRepaint = true;
        m_oldFormattingSpace = state;
    }
    state = m_cbViewFormattingBreak->isChecked();
    if ( state != m_oldFormattingBreak)
    {
        doc->setViewFormattingBreak(state);
        needRepaint = true;
        m_oldFormattingBreak = state;
    }
    state = m_cbViewFormattingTabs->isChecked();
    if ( state != m_oldFormattingTabs )
    {
        doc->setViewFormattingTabs(state);
        needRepaint = true;
        m_oldFormattingTabs= state;
    }
    if ( needRepaint )
    {
        doc->layout();
        doc->repaintAllViews();
    }
    return macroCmd;
}

void ConfigureMiscPage::slotDefault()
{
   m_undoRedoLimit->setValue(30);
   m_displayLink->setChecked(true);
   m_displayComment->setChecked(true);
   m_underlineLink->setChecked(true);
   m_cbViewFormattingEndParag->setChecked(true);
   m_cbViewFormattingSpace->setChecked(true);
   m_cbViewFormattingTabs->setChecked(true);
   m_cbViewFormattingBreak->setChecked(true);
   m_displayFieldCode->setChecked( false );
}

void ConfigureMiscPage::setUnit( KoUnit::Unit )
{
}

////

ConfigureDefaultDocPage::ConfigureDefaultDocPage( KWView *view, KVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=view;
    KWDocument * doc = m_pView->kWordDocument();
    config = KWFactory::instance()->config();
    Q3GroupBox * gbDocumentDefaults = new Q3GroupBox(1, Qt::Horizontal, i18n("Document Defaults"), box, "GroupBox" );
    //gbDocumentDefaults->setMargin( KDialog::marginHint() );
    gbDocumentDefaults->setInsideSpacing( KDialog::spacingHint() );

    double ptColumnSpacing=3;
    KoUnit::Unit unit = doc->unit();
    if( config->hasGroup("Document defaults") )
    {
        config->setGroup( "Document defaults" );
        ptColumnSpacing=config->readEntry("ColumnSpacing",ptColumnSpacing);
        // loaded by kwdoc already defaultFont=config->readEntry("DefaultFont",defaultFont);
    }


    KHBox* hbColumnSpacing = new KHBox( gbDocumentDefaults );
    QLabel* columnSpacingLabel = new QLabel( i18n("Default column spacing:"), hbColumnSpacing );
    m_columnSpacing = new KoUnitDoubleSpinBox( hbColumnSpacing,
                                               0.1,
                                               50,
                                               0.1,
                                               ptColumnSpacing,
                                               unit );
    columnSpacingLabel->setBuddy( m_columnSpacing );
    m_columnSpacing->setWhatsThis( i18n("When setting a document to use more than one column "
                "this distance will be used to separate the columns. This value is merely a default "
                "setting as the column spacing can be changed per document") );

    QWidget *fontContainer = new QWidget(gbDocumentDefaults);
    Q3GridLayout * fontLayout = new Q3GridLayout(fontContainer, 1, 3);

    fontLayout->setSpacing(KDialog::spacingHint());
    fontLayout->setColumnStretch(0, 0);
    fontLayout->setColumnStretch(1, 1);
    fontLayout->setColumnStretch(2, 0);

    QLabel *fontTitle = new QLabel(i18n("Default font:"), fontContainer);

    font= new QFont( doc->defaultFont() );

    QString labelName = font->family() + ' ' + QString::number(font->pointSize());
    fontName = new QLabel(labelName, fontContainer);
    fontName->setFont(*font);
    fontName->setFrameStyle(Q3Frame::StyledPanel | Q3Frame::Sunken);

    QPushButton *chooseButton = new QPushButton(i18n("Choose..."), fontContainer);
    connect(chooseButton, SIGNAL(clicked()), this, SLOT(selectNewDefaultFont()));

    fontLayout->addWidget(fontTitle, 0, 0);
    fontLayout->addWidget(fontName, 0, 1);
    fontLayout->addWidget(chooseButton, 0, 2);


    oldAutoSaveValue=KoDocument::defaultAutoSave() / 60;
    m_oldLanguage = doc->globalLanguage();
    m_oldHyphenation = doc->globalHyphenation();
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        oldAutoSaveValue=config->readEntry("AutoSave",oldAutoSaveValue);
        m_oldLanguage = config->readEntry( "language", m_oldLanguage);
        m_oldHyphenation = config->readEntry( "hyphenation", m_oldHyphenation);
    }


    QWidget *languageContainer = new QWidget(gbDocumentDefaults);
    Q3GridLayout * languageLayout = new Q3GridLayout(languageContainer, 1, 3);

    languageLayout->setSpacing(KDialog::spacingHint());
    languageLayout->setColumnStretch(0, 0);
    languageLayout->setColumnStretch(1, 1);

    QLabel *languageTitle = new QLabel(i18n("Global language:"), languageContainer);

    m_globalLanguage = new QComboBox( languageContainer );
    m_globalLanguage->insertStringList( KoGlobal::listOfLanguages() );
    m_globalLanguage->setCurrentText( KoGlobal::languageFromTag( m_oldLanguage ) );

    languageLayout->addWidget(languageTitle, 0, 0);
    languageLayout->addWidget(m_globalLanguage, 0, 1);

    m_autoHyphenation = new QCheckBox( i18n("Automatic hyphenation"), gbDocumentDefaults);
    m_autoHyphenation->setChecked( m_oldHyphenation );

    Q3GroupBox * gbDocumentSettings = new Q3GroupBox(1, Qt::Horizontal, i18n("Document Settings"), box );
    //gbDocumentSettings->setMargin( KDialog::marginHint() );
    gbDocumentSettings->setInsideSpacing( KDialog::spacingHint() );

    KHBox* hbAutoSave = new KHBox( gbDocumentSettings );
    QLabel* labelAutoSave = new QLabel( i18n("Autosave every (min):"), hbAutoSave );
    autoSave = new KIntNumInput( oldAutoSaveValue, hbAutoSave );
    autoSave->setRange(0, 60, 1);
    labelAutoSave->setBuddy(autoSave);
    autoSave->setWhatsThis( i18n("A backup copy of the current document is created when a change "
                    "has been made. The interval used to create backup documents is set here.") );
    autoSave->setSpecialValueText(i18n("No autosave"));
    autoSave->setSuffix(i18n(" min"));

    m_oldBackupFile = true;
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        m_oldBackupFile=config->readEntry("BackupFile",m_oldBackupFile);
    }

    m_createBackupFile = new QCheckBox( i18n("Create backup file"), gbDocumentSettings);
    m_createBackupFile->setChecked( m_oldBackupFile );

    KHBox* hbStartingPage = new KHBox( gbDocumentSettings );
    QLabel* labelStartingPage = new QLabel(i18n("Starting page number:"), hbStartingPage);

    m_oldStartingPage=doc->variableCollection()->variableSetting()->startingPageNumber();
    m_variableNumberOffset=new KIntNumInput(hbStartingPage);
    m_variableNumberOffset->setRange(-1000, 9999, 1, false);
    m_variableNumberOffset->setValue(m_oldStartingPage);
    labelStartingPage->setBuddy( m_variableNumberOffset );

    KHBox* hbTabStop = new KHBox( gbDocumentSettings );
    tabStop = new QLabel(i18n("Tab stop (%1):",doc->unitName()), hbTabStop);
    m_tabStopWidth = new KoUnitDoubleSpinBox( hbTabStop,
                                              MM_TO_POINT(2),
                                              doc->pageManager()->page(doc->startPage())->width(),
                                              0.1,
                                              doc->tabStopValue(),
                                              unit );
    m_oldTabStopWidth = doc->tabStopValue();

    Q3GroupBox * gbDocumentCursor = new Q3GroupBox(1, Qt::Horizontal, i18n("Cursor"), box );
    //gbDocumentCursor->setMargin( KDialog::marginHint() );
    gbDocumentCursor->setInsideSpacing( KDialog::spacingHint() );

    m_cursorInProtectedArea= new QCheckBox(i18n("Cursor in protected area"),gbDocumentCursor);
    m_cursorInProtectedArea->setChecked(doc->cursorInProtectedArea());

//     m_directInsertCursor= new QCheckBox(i18n("Direct insert cursor"),gbDocumentCursor);
//     m_directInsertCursor->setChecked(doc->insertDirectCursor());
}

KCommand *ConfigureDefaultDocPage::apply()
{
    config->setGroup( "Document defaults" );
    KWDocument * doc = m_pView->kWordDocument();
    double colSpacing = m_columnSpacing->value();
    if ( colSpacing != doc->defaultColumnSpacing() )
    {
        config->writeEntry( "ColumnSpacing", colSpacing );
        doc->setDefaultColumnSpacing(colSpacing);
    }
    config->writeEntry("DefaultFont",font->toString());

    config->setGroup( "Interface" );
    int autoSaveVal=autoSave->value();
    if(autoSaveVal!=oldAutoSaveValue)
    {
        config->writeEntry( "AutoSave", autoSaveVal );
        doc->setAutoSave(autoSaveVal*60);
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
        doc->setCursorInProtectedArea( state );
    }

//     state = m_directInsertCursor->isChecked();
//     if ( state != doc->insertDirectCursor() )
//         doc->setInsertDirectCursor( state );

    //Laurent Todo add a message box to inform user that
    //global language will change after re-launch kword
    QString lang = KoGlobal::tagOfLanguage( m_globalLanguage->currentText() );
    config->writeEntry( "language" , lang);
    m_oldLanguage = lang;
    //don't call this fiunction otherwise we can have a textobject with
    // a default language and other textobject with other default language.
    //doc->setGlobalLanguage( lang );

    state = m_autoHyphenation->isChecked();
    config->writeEntry( "hyphenation", state  );
    m_oldHyphenation = state;

    KMacroCommand * macroCmd=0L;
    int newStartingPage=m_variableNumberOffset->value();
    if(newStartingPage!=m_oldStartingPage)
    {
        macroCmd=new KMacroCommand(i18n("Change Starting Page Number"));
        KWChangeStartingPageCommand *cmd = new KWChangeStartingPageCommand( i18n("Change Starting Page Number"), doc, m_oldStartingPage,newStartingPage );
        cmd->execute();
        macroCmd->addCommand(cmd);
        m_oldStartingPage=newStartingPage;
    }
    double newTabStop = m_tabStopWidth->value();
    if ( newTabStop != m_oldTabStopWidth)
    {
        if ( !macroCmd )
            macroCmd=new KMacroCommand(i18n("Change Tab Stop Value"));


        KWChangeTabStopValueCommand *cmd = new KWChangeTabStopValueCommand( i18n("Change Tab Stop Value"), m_oldTabStopWidth, newTabStop, doc);
        cmd->execute();
        macroCmd->addCommand(cmd);
        m_oldTabStopWidth = newTabStop;
    }

    return macroCmd;
}

void ConfigureDefaultDocPage::slotDefault()
{
   m_columnSpacing->setValue( 3 );
   autoSave->setValue( KoDocument::defaultAutoSave() / 60 );
   m_variableNumberOffset->setValue(1);
   m_cursorInProtectedArea->setChecked(true);
   m_tabStopWidth->setValue( MM_TO_POINT(15) );
   m_createBackupFile->setChecked( true );
//    m_directInsertCursor->setChecked( false );
   m_globalLanguage->setCurrentText( KoGlobal::languageFromTag( KGlobal::locale()->language() ) );
   m_autoHyphenation->setChecked( false );
}

void ConfigureDefaultDocPage::selectNewDefaultFont() {
    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
    KFontDialog dlg( (QWidget *)this->parent(), false, true, list, true );
	dlg.setObjectName("Font Selector");
    dlg.setFont(*font);
    int result = dlg.exec();
    if (KDialog::Accepted == result) {
        delete font;
        font = new QFont(dlg.font());
        fontName->setText(font->family() + ' ' + QString::number(font->pointSize()));
        fontName->setFont(*font);
        m_pView->kWordDocument()->setDefaultFont( *font );
    }
}

void ConfigureDefaultDocPage::setUnit( KoUnit::Unit unit )
{
    m_columnSpacing->setUnit( unit );
    m_tabStopWidth->setUnit( unit );
    tabStop->setText( i18n( "Tab stop:" ) );
}

////

ConfigurePathPage::ConfigurePathPage( KWView *view, KVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=view;
    KWDocument * doc = m_pView->kWordDocument();
    config = KWFactory::instance()->config();
    Q3GroupBox * gbPathGroup = new Q3GroupBox(1, Qt::Horizontal, i18n("Path"), box, "GroupBox" );
    //gbPathGroup->setMargin( KDialog::marginHint() );
    gbPathGroup->setInsideSpacing( KDialog::spacingHint() );

    m_pPathView = new K3ListView( gbPathGroup );
    m_pPathView->setResizeMode(Q3ListView::NoColumn);
    m_pPathView->addColumn( i18n( "Type" ) );
    m_pPathView->addColumn( i18n( "Path" ), 400 ); // not too big by default
    (void) new Q3ListViewItem( m_pPathView, i18n("Personal Expression"), doc->personalExpressionPath().join(";") );
    (void) new Q3ListViewItem( m_pPathView, i18n("Backup Path"),doc->backupPath() );

    m_modifyPath = new QPushButton( i18n("Modify Path..."), gbPathGroup);
    connect( m_modifyPath, SIGNAL( clicked ()), this, SLOT( slotModifyPath()));
    connect( m_pPathView, SIGNAL( doubleClicked (Q3ListViewItem *, const QPoint &, int  )), this, SLOT( slotModifyPath()));
    connect( m_pPathView, SIGNAL( selectionChanged ( Q3ListViewItem * )), this, SLOT( slotSelectionChanged(Q3ListViewItem * )));
    slotSelectionChanged(m_pPathView->currentItem());
}

void ConfigurePathPage::slotSelectionChanged(Q3ListViewItem * item)
{
    m_modifyPath->setEnabled( item );
}

void ConfigurePathPage::slotModifyPath()
{
    Q3ListViewItem *item = m_pPathView->currentItem ();
    if ( item )
    {
        if ( item->text(0)==i18n("Personal Expression"))
        {
            KoEditPathDia * dlg = new KoEditPathDia( item->text( 1),   0L, "editpath");
            if (dlg->exec() )
                item->setText(1, dlg->newPath());
            delete dlg;
        }
        if ( item->text(0)==i18n("Backup Path"))
        {
            KoChangePathDia *dlg = new KoChangePathDia( item->text(1), 0L, "backup path" );
            if (dlg->exec() )
                item->setText(1, dlg->newPath());
            delete dlg;
        }
    }
}

void ConfigurePathPage::slotDefault()
{
    Q3ListViewItem * item = m_pPathView->findItem(i18n("Personal Expression"), 0);
    if ( item )
        item->setText(1, KWFactory::instance()->dirs()->resourceDirs("expression").join(";"));
    item = m_pPathView->findItem(i18n("Backup Path"), 0);
    if ( item )
        item->setText(1, QString::null );
}

void ConfigurePathPage::apply()
{
    Q3ListViewItem * item = m_pPathView->findItem(i18n("Personal Expression"), 0);
    if ( item )
    {
        QStringList lst = QStringList::split(QString(";"), item->text(1));
        if ( lst != m_pView->kWordDocument()->personalExpressionPath())
        {
            m_pView->kWordDocument()->setPersonalExpressionPath(lst);
            config->setGroup( "Kword Path" );
            config->writePathEntry( "expression path", lst);
        }
    }
    item = m_pPathView->findItem(i18n("Backup Path"), 0);
    if ( item )
    {
        QString res = item->text(1 );
        if ( res != m_pView->kWordDocument()->backupPath())
        {
            config->setGroup( "Kword Path" );
            m_pView->kWordDocument()->setBackupPath( res );
            config->writePathEntry( "backup path",res );
        }
    }

}

////

#include "KWConfig.moc"
