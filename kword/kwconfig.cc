/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <kconfig.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <knumvalidator.h>
#include <kspell.h>
#include <kfontdialog.h>
#include <kdebug.h>

#include <qcheckbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>
#include <qlistbox.h>

#include "kwconfig.h"
#include "kwview.h"
#include "kwdoc.h"
#include "kwcanvas.h"
#include "kwviewmode.h"
#include "kwcommand.h"
#include "kwvariable.h"
#include "koeditpath.h"
#include <koSconfig.h>

#include <kovariable.h>
#include <kformulaconfigpage.h>

#include <float.h>
#include <kmessagebox.h>
#include <klistview.h>
#include <kstandarddirs.h>
#include <kdeversion.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kurlrequesterdlg.h>
#include <kfiledialog.h>
#include <qtabwidget.h>
#include <keditlistbox.h>
#include <koSpellConfig.h>
#include <koGlobal.h>

// little helper stolen from kmail
// (Note: KDialogBase should have version of the methods that take a QString for the icon name)
static inline QPixmap loadIcon( const char * name ) {
  return KGlobal::instance()->iconLoader()
    ->loadIcon( QString::fromLatin1(name), KIcon::NoGroup, KIcon::SizeMedium );
}

KWConfig::KWConfig( KWView* parent )
  : KDialogBase(KDialogBase::IconList,i18n("Configure KWord") ,
		KDialogBase::Ok | KDialogBase::Apply | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
  QVBox *page2 = addVBoxPage( i18n("Interface"), i18n("Interface Settings"),
                              loadIcon("configure") );
  m_interfacePage=new ConfigureInterfacePage(parent, page2);

  QVBox *page4 = addVBoxPage( i18n("Document"), i18n("Document Settings"),
                              loadIcon("misc_doc") );

  m_defaultDocPage=new ConfigureDefaultDocPage(parent, page4);

  QVBox *page = addVBoxPage( i18n("Spelling"), i18n("Spell Checker Behavior"),
                        loadIcon("spellcheck") );
  m_spellPage=new ConfigureSpellPage(parent, page);

  QVBox *page5 = addVBoxPage( i18n("Formula"), i18n("Formula Defaults"),
                              loadIcon("kformula") );
  m_formulaPage=new KFormula::ConfigurePage( parent->kWordDocument()->getFormulaDocument(),
                                             this, KWFactory::global()->config(), page5 );

  QVBox *page3 = addVBoxPage( i18n("Misc"), i18n("Misc Settings"),
                              loadIcon("misc") );
  m_miscPage=new ConfigureMiscPage(parent, page3);

  QVBox *page6 = addVBoxPage( i18n("Path"), i18n("Path Settings"),
                              loadIcon("path") );
  m_pathPage=new ConfigurePathPage(parent, page6);

  m_doc = parent->kWordDocument();
  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));

}

void KWConfig::openPage(int flags)
{
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
}

void KWConfig::slotApply()
{
    KMacroCommand *macro = 0L;
    m_spellPage->apply();
    m_interfacePage->apply();
    m_pathPage->apply();
    KCommand* cmd = m_defaultDocPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change Config"));
        macro->addCommand(cmd);
    }
    m_formulaPage->apply();
    // Apply misc page last! (65972)
    cmd = m_miscPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change Config"));
        macro->addCommand( cmd );
    }

    if (macro)
        m_doc->addCommand( macro );
}

void KWConfig::slotDefault()
{
    switch(activePageIndex())
    {
    case 0:
        m_interfacePage->slotDefault();
        break;
    case 1:
        m_defaultDocPage->slotDefault();
        break;
    case 2:
        m_spellPage->slotDefault();
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
}

ConfigureSpellPage::ConfigureSpellPage( KWView *_view, QVBox *box, char *name )
    : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KWFactory::global()->config();

    m_spellConfigWidget = new KoSpellConfigWidget( box, m_pView->kWordDocument()->getKOSpellConfig(), true);
#if 0
    if( config->hasGroup("KSpell kword") )
    {
        config->setGroup( "KSpell kword" );
        m_spellConfigWidget->setDontCheckUpperWord(config->readBoolEntry("KSpell_dont_check_upper_word",false));
        m_spellConfigWidget->setDontCheckTitleCase(config->readBoolEntry("KSpell_dont_check_title_case",false));
    }
#endif
    m_spellConfigWidget->setBackgroundSpellCheck( m_pView->kWordDocument()->backgroundSpellCheckEnabled() );
    m_spellConfigWidget->addIgnoreList( m_pView->kWordDocument()->spellListIgnoreAll() );

}

void ConfigureSpellPage::apply()
{
    KOSpellConfig *_spellConfig = m_spellConfigWidget->spellConfig();
    config->setGroup( "KSpell kword" );
  config->writeEntry ("KSpell_NoRootAffix",(int) _spellConfig->noRootAffix ());
  config->writeEntry ("KSpell_RunTogether", (int) _spellConfig->runTogether ());
  config->writeEntry ("KSpell_Dictionary", _spellConfig->dictionary ());
  config->writeEntry ("KSpell_DictFromList",(int)  _spellConfig->dictFromList());
  config->writeEntry ("KSpell_Encoding", (int)  _spellConfig->encoding());
  config->writeEntry ("KSpell_Client",  _spellConfig->client());
  config->writeEntry( "KSpell_dont_check_title_case", (int)_spellConfig->dontCheckTitleCase());
  config->writeEntry ("KSpell_IgnoreCase",(int) _spellConfig->ignoreCase());
  config->writeEntry( "KSpell_IgnoreAccent", (int) _spellConfig->ignoreAccent());
  config->writeEntry( "KSpell_dont_check_upper_word", (int)_spellConfig->dontCheckUpperWord());
  config->writeEntry( "KSpell_SpellWordWithNumber", (int)_spellConfig->spellWordWithNumber());
  m_spellConfigWidget->saveDictionary();
  KWDocument* doc = m_pView->kWordDocument();
  doc->setKOSpellConfig(*_spellConfig);


  bool state=m_spellConfigWidget->backgroundSpellCheck();
  config->writeEntry( "SpellCheck", (int)state );

  m_pView->kWordDocument()->addIgnoreWordAllList( m_spellConfigWidget->ignoreList() );
  //FIXME reactivate just if there is a changes.
  doc->enableBackgroundSpellCheck( state );
  doc->reactivateBgSpellChecking();
}

void ConfigureSpellPage::slotDefault()
{
    m_spellConfigWidget->setDefault();
}

ConfigureInterfacePage::ConfigureInterfacePage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KWFactory::global()->config();
    KoUnit::Unit unit = m_pView->kWordDocument()->getUnit();
    QVGroupBox* gbInterfaceGroup = new QVGroupBox( i18n("Interface"), box, "GroupBox" );
    gbInterfaceGroup->setMargin( KDialog::marginHint() );
    gbInterfaceGroup->setInsideSpacing( KDialog::spacingHint() );

    double ptGridX=10.0;
    double ptGridY=10.0;
    double ptIndent = MM_TO_POINT(10.0);
    bool oldShowStatusBar = true;
    bool oldPgUpDownMovesCaret = false;
    bool oldShowScrollBar = true;
    oldNbRecentFiles=10;
    int nbPagePerRow=4;
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        ptGridX=config->readDoubleNumEntry("GridX", ptGridX);
        ptGridY=config->readDoubleNumEntry("GridY", ptGridY);
        ptIndent = config->readDoubleNumEntry("Indent", ptIndent);
        oldNbRecentFiles=config->readNumEntry("NbRecentFile", oldNbRecentFiles);
        nbPagePerRow=config->readNumEntry("nbPagePerRow", nbPagePerRow);
        oldShowStatusBar = config->readBoolEntry( "ShowStatusBar", true );
        oldPgUpDownMovesCaret = config->readBoolEntry( "PgUpDownMovesCaret", false );
        oldShowScrollBar = config->readBoolEntry("ShowScrollBar", true);
    }

    showStatusBar = new QCheckBox(i18n("Show &status bar"),gbInterfaceGroup);
    showStatusBar->setChecked(oldShowStatusBar);

    showScrollBar = new QCheckBox( i18n("Show s&crollbar"), gbInterfaceGroup);
    showScrollBar->setChecked(oldShowScrollBar);

    pgUpDownMovesCaret = new QCheckBox(i18n("PageUp/PageDown &moves the caret"),gbInterfaceGroup);
    pgUpDownMovesCaret->setChecked(oldPgUpDownMovesCaret);
    QWhatsThis::add( pgUpDownMovesCaret, i18n(
                         "If this option is enabled, the PageUp and PageDown keys "
                         "move the text caret, as in other KDE applications. "
                         "If it is disabled, they move the scrollbars, as in most other word processors." ) );

    recentFiles=new KIntNumInput( oldNbRecentFiles, gbInterfaceGroup );
    recentFiles->setRange(1, 20, 1);
    recentFiles->setLabel(i18n("Number of recent &files:"));
    QWhatsThis::add( recentFiles, i18n("The number of files remembered in the file open dialog and in the "
                    "recent files menu item") );

    QString suffix = KoUnit::unitName( unit ).prepend(' ');
    gridX=new KDoubleNumInput( recentFiles, KoUnit::ptToUnit( ptGridX, unit ), gbInterfaceGroup );
    gridX->setRange(KoUnit::ptToUnit( 0.1, unit ),KoUnit::ptToUnit( 50, unit ), KoUnit::ptToUnit( 0.1, unit ));
    gridX->setSuffix( suffix );
    gridX->setLabel(i18n("&Horizontal grid size:"));
    QWhatsThis::add( gridX, i18n("The grid size on which frames, tabs and other content snaps while "
                    "moving and scaling") );

    gridY=new KDoubleNumInput( gridX, KoUnit::ptToUnit( ptGridY, unit ), gbInterfaceGroup );
    gridY->setRange(KoUnit::ptToUnit( 0.1, unit ), KoUnit::ptToUnit( 50, unit ), KoUnit::ptToUnit( 0.1, unit ));
    //laurent kdoublenuminput changed !!!!
    //setPrecision doen't work it return a value display * 10^precision !!!!
    //perhaps it's normal in new API ....

    gridY->setLabel(i18n("&Vertical grid size:"));
    QWhatsThis::add( gridY, i18n("The grid size on which frames and other content snaps while "
                    "moving and scaling") );
    gridY->setSuffix( suffix );

    double val = KoUnit::ptToUnit( ptIndent, unit );
    indent = new KDoubleNumInput( gridY, val, gbInterfaceGroup );
    indent->setRange(KoUnit::ptToUnit( 0.1, unit ), KoUnit::ptToUnit( 50, unit ), KoUnit::ptToUnit( 0.1, unit ));

    indent->setSuffix( suffix );
    indent->setLabel(i18n("&Paragraph indent by toolbar buttons:"));
    QWhatsThis::add( indent, i18n("Configure the indent width used when using the 'Increase' "
                    "or 'Decrease' indentation buttons on a paragraph.<p>The lower the value, "
                    "the more often the buttons will have to be pressed to gain the same "
                    "indentation.") );

    m_nbPagePerRow=new KIntNumInput( indent, nbPagePerRow, gbInterfaceGroup );
    m_nbPagePerRow->setRange(1, 10, 1);
    m_nbPagePerRow->setLabel(i18n("Number of pa&ges per row in preview mode:"));
    QWhatsThis::add(m_nbPagePerRow , i18n("After selecting preview mode (via the \"View\" "
                    "menu, option \"Preview mode\") this is the amount of pages KWord will "
                    "position on one horizontal row") );
}

void ConfigureInterfacePage::apply()
{
    KWDocument * doc = m_pView->kWordDocument();
    double valX=KoUnit::ptFromUnit( gridX->value(), doc->getUnit() );
    double valY=KoUnit::ptFromUnit( gridY->value(), doc->getUnit() );
    int nbRecent=recentFiles->value();

    bool statusBar=showStatusBar->isChecked();
    bool scrollBar = showScrollBar->isChecked();
    config->setGroup( "Interface" );
    if(valX!=doc->gridX())
    {
        config->writeEntry( "GridX", QMAX( 0.1, valX), true, false, 'g', DBL_DIG /* 6 is not enough */ );
        doc->setGridX(valX);
    }
    if(valY!=doc->gridY())
    {
        config->writeEntry( "GridY", QMAX( 0.1, valY), true, false, 'g', DBL_DIG /* 6 is not enough */ );
        doc->setGridY(valY);
    }
    double newIndent = KoUnit::ptFromUnit( indent->value(), doc->getUnit() );
    if( newIndent != doc->indentValue() )
    {
        config->writeEntry( "Indent", newIndent, true, false, 'g', DBL_DIG /* 6 is not enough */ );
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
        //necessary to recreate new view because in doc->switchViewMode
        //we delete viewmode that we want to apply
        doc->switchViewMode( KWViewMode::create( doc->viewMode()->type(), doc ) ); // force a refresh
    }
}

void ConfigureInterfacePage::slotDefault()
{
    KWDocument * doc = m_pView->kWordDocument();
    gridX->setValue( KoUnit::ptToUnit( 10, doc->getUnit() ) );
    gridY->setValue( KoUnit::ptToUnit( 10, doc->getUnit() ) );
    m_nbPagePerRow->setValue(4);
    double newIndent = KoUnit::ptToUnit( MM_TO_POINT( 10 ), doc->getUnit() );
    indent->setValue( newIndent );
    recentFiles->setValue(10);
    showStatusBar->setChecked(true);
    pgUpDownMovesCaret->setChecked(false);
    showScrollBar->setChecked( true);
}


ConfigureMiscPage::ConfigureMiscPage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KWFactory::global()->config();
    KoUnit::Unit unit = m_pView->kWordDocument()->getUnit();
    QVGroupBox* gbMiscGroup = new QVGroupBox( i18n("Misc"), box, "GroupBox" );
    gbMiscGroup->setMargin( KDialog::marginHint() );
    gbMiscGroup->setInsideSpacing( KDialog::spacingHint() );

    m_oldNbRedo=30;
    QString unitType=KoUnit::unitName(unit);
    //#################"laurent
    //don't load unitType from config file because unit is
    //depend from kword file => unit can be different from config file

    if( config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        m_oldNbRedo=config->readNumEntry("UndoRedo",m_oldNbRedo);
    }

    QHBox *lay = new QHBox(gbMiscGroup);
    lay->setSpacing(KDialog::spacingHint());
    QLabel *unitLabel= new QLabel(i18n("Units:"),lay);

    QStringList listUnit;
    listUnit << KoUnit::unitDescription( KoUnit::U_MM );
    listUnit << KoUnit::unitDescription( KoUnit::U_INCH );
    listUnit << KoUnit::unitDescription( KoUnit::U_PT );
    listUnit << KoUnit::unitDescription( KoUnit::U_CM );
    listUnit << KoUnit::unitDescription( KoUnit::U_DM );
    listUnit << KoUnit::unitDescription( KoUnit::U_PI );
    listUnit << KoUnit::unitDescription( KoUnit::U_DD );
    listUnit << KoUnit::unitDescription( KoUnit::U_CC );


    m_unit = new QComboBox( lay );
    m_unit->insertStringList(listUnit);
    m_oldUnit=0;
    switch (KoUnit::unit( unitType ) )
    {
    case KoUnit::U_MM:
        m_oldUnit=0;
        break;
    case KoUnit::U_INCH:
        m_oldUnit=1;
        break;
    case KoUnit::U_PT:
        m_oldUnit = 2;
        break;
    case KoUnit::U_CM:
        m_oldUnit = 3;
        break;
    case KoUnit::U_DM:
        m_oldUnit = 4;
        break;
    case KoUnit::U_PI:
        m_oldUnit = 5;
        break;
    case KoUnit::U_DD:
        m_oldUnit = 6;
        break;
    case KoUnit::U_CC:
    default:
        m_oldUnit = 7;
    }
    m_unit->setCurrentItem(m_oldUnit);
    QString unitHelp = i18n("Select the unit type used every time a distance or width/height "
                            "is displayed or entered. This one setting is for the whole of KWord.");
    QWhatsThis::add( unitLabel, unitHelp);
    QWhatsThis::add( m_unit, unitHelp);

    m_undoRedoLimit=new KIntNumInput( m_oldNbRedo, gbMiscGroup );
    m_undoRedoLimit->setLabel(i18n("Undo/redo limit:"));
    m_undoRedoLimit->setRange(1, 100, 1);
    QWhatsThis::add( m_undoRedoLimit, i18n("Limit the amount of undo/redo actions remembered to save "
                                           "memory") );

    KWDocument* doc = m_pView->kWordDocument();
    m_displayLink=new QCheckBox(i18n("Display &links"),gbMiscGroup);
    m_displayLink->setChecked(doc->getVariableCollection()->variableSetting()->displayLink());
    m_underlineLink=new QCheckBox(i18n("&Underline all links"),gbMiscGroup);
    m_underlineLink->setChecked(doc->getVariableCollection()->variableSetting()->underlineLink());


    m_displayComment=new QCheckBox(i18n("Display c&omments"),gbMiscGroup);
    m_displayComment->setChecked(doc->getVariableCollection()->variableSetting()->displayComment());

    m_displayFieldCode=new QCheckBox(i18n("Display field code"),gbMiscGroup);
    m_displayFieldCode->setChecked(doc->getVariableCollection()->variableSetting()->displayFieldCode());


    QVGroupBox* gbViewFormatting = new QVGroupBox( i18n("View Formatting"), box, "view_formatting" );
    gbViewFormatting->setMargin( KDialog::marginHint() );
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
    if(m_oldUnit!=m_unit->currentItem())
    {
        QString unitName;
        m_oldUnit=m_unit->currentItem();
        switch (m_oldUnit)
        {
            case 0:
                unitName=KoUnit::unitName(KoUnit::U_MM  );
                doc->setUnit( KoUnit::U_MM );
                break;
            case 1:
                unitName=KoUnit::unitName(KoUnit::U_INCH  );
                doc->setUnit( KoUnit::U_INCH );
                break;
            case 2:
                doc->setUnit( KoUnit::U_PT );
                unitName=KoUnit::unitName(KoUnit::U_PT );
                break;
            case 3:
                doc->setUnit( KoUnit::U_CM );
                unitName=KoUnit::unitName(KoUnit::U_CM );
                break;
            case 4:
                doc->setUnit( KoUnit::U_DM );
                unitName=KoUnit::unitName(KoUnit::U_DM );
                break;
            case 5:
                doc->setUnit( KoUnit::U_PI );
                unitName=KoUnit::unitName(KoUnit::U_PI );
                break;
        case 6:
                doc->setUnit( KoUnit::U_DD );
                unitName=KoUnit::unitName(KoUnit::U_DD );
                break;
            case 7:
        default:
                doc->setUnit( KoUnit::U_CC );
                unitName=KoUnit::unitName(KoUnit::U_CC );
        }
        config->writeEntry("Units",unitName);
    }
    int newUndo=m_undoRedoLimit->value();
    if(newUndo!=m_oldNbRedo)
    {
        config->writeEntry("UndoRedo",newUndo);
        doc->setUndoRedoLimit(newUndo);
        m_oldNbRedo=newUndo;
    }
    KMacroCommand * macroCmd=0L;
    bool b=m_displayLink->isChecked();
    if(doc->getVariableCollection()->variableSetting()->displayLink()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Link Command"), doc, doc->getVariableCollection()->variableSetting()->displayLink() ,b, KWChangeVariableSettingsCommand::VS_DISPLAYLINK);

        cmd->execute();
        macroCmd->addCommand(cmd);
    }
    b=m_underlineLink->isChecked();
    if(doc->getVariableCollection()->variableSetting()->underlineLink()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Link Command"), doc, doc->getVariableCollection()->variableSetting()->underlineLink() ,b, KWChangeVariableSettingsCommand::VS_UNDERLINELINK);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }

    b=m_displayComment->isChecked();
    if(doc->getVariableCollection()->variableSetting()->displayComment()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Link Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Link Command"), doc, doc->getVariableCollection()->variableSetting()->displayComment() ,b, KWChangeVariableSettingsCommand::VS_DISPLAYCOMMENT);
        cmd->execute();
        macroCmd->addCommand(cmd);
    }
    b=m_displayFieldCode->isChecked();
    if(doc->getVariableCollection()->variableSetting()->displayFieldCode()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Field Code Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Field Code Command"), doc, doc->getVariableCollection()->variableSetting()->displayFieldCode() ,b, KWChangeVariableSettingsCommand::VS_DISPLAYFIELDCODE);
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
   m_unit->setCurrentItem(0);
}

ConfigureDefaultDocPage::ConfigureDefaultDocPage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    KWDocument * doc = m_pView->kWordDocument();
    config = KWFactory::global()->config();
    QVGroupBox* gbDocumentDefaults = new QVGroupBox( i18n("Document Defaults"), box, "GroupBox" );
    gbDocumentDefaults->setMargin( KDialog::marginHint() );
    gbDocumentDefaults->setInsideSpacing( KDialog::spacingHint() );

    double ptColumnSpacing=3;
    KoUnit::Unit unit = doc->getUnit();
    QString unitType=KoUnit::unitName(unit);
    if( config->hasGroup("Document defaults") )
    {
        config->setGroup( "Document defaults" );
        unitType=config->readEntry("Units",unitType);
        ptColumnSpacing=config->readDoubleNumEntry("ColumnSpacing",ptColumnSpacing);
        // loaded by kwdoc already defaultFont=config->readEntry("DefaultFont",defaultFont);
    }

    QString suffix = unitType.prepend(' ');
    columnSpacing=new KDoubleNumInput( KoUnit::ptToUnit( ptColumnSpacing, unit ), gbDocumentDefaults );
    columnSpacing->setRange(KoUnit::ptToUnit( 0.1, unit ), KoUnit::ptToUnit( 50, unit ), KoUnit::ptToUnit( 0.1, unit ));
    columnSpacing->setSuffix( suffix );
    columnSpacing->setLabel(i18n("Default column spacing:"));
    QWhatsThis::add( columnSpacing, i18n("When setting a document to use more than one column "
                "this distance will be used to separate the columns. This value is merely a default "
                "setting as the column spacing can be changed per document") );

    QWidget *fontContainer = new QWidget(gbDocumentDefaults);
    QGridLayout * fontLayout = new QGridLayout(fontContainer, 1, 3);

    fontLayout->setColStretch(0, 0);
    fontLayout->setColStretch(1, 1);
    fontLayout->setColStretch(2, 0);

    QLabel *fontTitle = new QLabel(i18n("Default font:"), fontContainer);

    font= new QFont( doc->defaultFont() );

    QString labelName = font->family() + ' ' + QString::number(font->pointSize());
    fontName = new QLabel(labelName, fontContainer);
    fontName->setFont(*font);
    fontName->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

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
        oldAutoSaveValue=config->readNumEntry("AutoSave",oldAutoSaveValue);
        m_oldLanguage = config->readEntry( "language", m_oldLanguage);
        m_oldHyphenation = config->readBoolEntry( "hyphenation", m_oldHyphenation);
    }


    QWidget *languageContainer = new QWidget(gbDocumentDefaults);
    QGridLayout * languageLayout = new QGridLayout(languageContainer, 1, 3);

    languageLayout->setColStretch(0, 0);
    languageLayout->setColStretch(1, 1);

    QLabel *languageTitle = new QLabel(i18n("Global language:"), languageContainer);

    m_globalLanguage = new QComboBox( languageContainer );
    m_globalLanguage->insertStringList( KoGlobal::listOfLanguages());


    m_globalLanguage->setCurrentItem(KoGlobal::languageIndexFromTag(m_oldLanguage));

    languageLayout->addWidget(languageTitle, 0, 0);
    languageLayout->addWidget(m_globalLanguage, 0, 1);

    m_autoHyphenation = new QCheckBox( i18n("Automatic hyphenation"), gbDocumentDefaults);
    m_autoHyphenation->setChecked( m_oldHyphenation );

    QVGroupBox* gbDocumentSettings = new QVGroupBox( i18n("Document Settings"), box );
    gbDocumentSettings->setMargin( KDialog::marginHint() );
    gbDocumentSettings->setInsideSpacing( KDialog::spacingHint() );

    autoSave = new KIntNumInput( oldAutoSaveValue, gbDocumentSettings );
    autoSave->setRange(0, 60, 1);
    autoSave->setLabel(i18n("Autosave every (min):"));
    QWhatsThis::add( autoSave, i18n("A backup copy of the current document is created when a change "
                    "has been made. The interval used to create backup documents is set here.") );
    autoSave->setSpecialValueText(i18n("No autosave"));
    autoSave->setSuffix(i18n(" min"));

    m_oldBackupFile = true;
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        m_oldBackupFile=config->readBoolEntry("BackupFile",m_oldBackupFile);
    }

    m_createBackupFile = new QCheckBox( i18n("Create backup file"), gbDocumentSettings);
    m_createBackupFile->setChecked( m_oldBackupFile );

    new QLabel(i18n("Starting page number:"), gbDocumentSettings);

    m_oldStartingPage=doc->getVariableCollection()->variableSetting()->startingPage();
    m_variableNumberOffset=new KIntNumInput(gbDocumentSettings);
    m_variableNumberOffset->setRange(1, 9999, 1, false);
    m_variableNumberOffset->setValue(m_oldStartingPage);


    new QLabel(i18n("Tab stop (%1):").arg(doc->getUnitName()), gbDocumentSettings);
    m_tabStopWidth = new KDoubleNumInput( gbDocumentSettings );
    m_tabStopWidth->setRange( KoUnit::ptToUnit( MM_TO_POINT(2),unit ), KoUnit::ptToUnit( doc->ptPaperWidth(), unit ) , 0.1, false);
    m_oldTabStopWidth = doc->tabStopValue();
    m_tabStopWidth->setValue( KoUnit::ptToUnit( m_oldTabStopWidth, doc->getUnit() ));

    QVGroupBox* gbDocumentCursor = new QVGroupBox( i18n("Cursor"), box );
    gbDocumentCursor->setMargin( KDialog::marginHint() );
    gbDocumentCursor->setInsideSpacing( KDialog::spacingHint() );

    m_cursorInProtectedArea= new QCheckBox(i18n("Cursor in protected area"),gbDocumentCursor);
    m_cursorInProtectedArea->setChecked(doc->cursorInProtectedArea());

    m_directInsertCursor= new QCheckBox(i18n("Direct insert cursor"),gbDocumentCursor);
    m_directInsertCursor->setChecked(doc->insertDirectCursor());
}

KCommand *ConfigureDefaultDocPage::apply()
{
    config->setGroup( "Document defaults" );
    KWDocument * doc = m_pView->kWordDocument();
    int colSpacing=(int)KoUnit::ptFromUnit( columnSpacing->value(), doc->getUnit() );
    if(colSpacing!=doc->defaultColumnSpacing())
    {
        config->writeEntry( "ColumnSpacing",colSpacing , true, false, 'g', DBL_DIG /* 6 is not enough */ );
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

    state = m_directInsertCursor->isChecked();
    if ( state != doc->insertDirectCursor() )
        doc->setInsertDirectCursor( state );

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
    double newTabStop = KoUnit::ptFromUnit( m_tabStopWidth->value(), doc->getUnit() );
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
   columnSpacing->setValue(KoUnit::ptToUnit( 3, m_pView->kWordDocument()->getUnit() ));
   autoSave->setValue(KoDocument::defaultAutoSave()/60);
   m_variableNumberOffset->setValue(1);
   m_cursorInProtectedArea->setChecked(true);
   m_tabStopWidth->setValue(KoUnit::ptToUnit( MM_TO_POINT(15), m_pView->kWordDocument()->getUnit()));
   m_createBackupFile->setChecked( true );
   m_directInsertCursor->setChecked( false );
   m_globalLanguage->setCurrentItem(KoGlobal::languageIndexFromTag(KGlobal::locale()->language()));
   m_autoHyphenation->setChecked( false );
}

void ConfigureDefaultDocPage::selectNewDefaultFont() {
    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
    KFontDialog dlg( (QWidget *)this->parent(), "Font Selector", false, true, list, true );
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

ConfigurePathPage::ConfigurePathPage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    KWDocument * doc = m_pView->kWordDocument();
    config = KWFactory::global()->config();
    QVGroupBox* gbPathGroup = new QVGroupBox( i18n("Path"), box, "GroupBox" );
    gbPathGroup->setMargin( KDialog::marginHint() );
    gbPathGroup->setInsideSpacing( KDialog::spacingHint() );

    m_pPathView = new KListView( gbPathGroup );
    m_pPathView->setResizeMode(QListView::NoColumn);
    m_pPathView->addColumn( i18n( "Type" ) );
    m_pPathView->addColumn( i18n( "Path" ) );
    (void) new QListViewItem( m_pPathView, i18n("Personal Expression"), doc->personalExpressionPath().join(";") );
    (void) new QListViewItem( m_pPathView, i18n("Picture Path"),doc->picturePath() );
    (void) new QListViewItem( m_pPathView, i18n("Backup Path"),doc->backupPath() );
#if 0 // KWORD_HORIZONTAL_LINE
    (void) new QListViewItem( m_pPathView, i18n("Horizontal Line Path"),doc->horizontalLinePath().join(";") );
#endif

    m_modifyPath = new QPushButton( i18n("Modify Path..."), gbPathGroup);
    connect( m_modifyPath, SIGNAL( clicked ()), this, SLOT( slotModifyPath()));
    connect( m_pPathView, SIGNAL( doubleClicked (QListViewItem *, const QPoint &, int  )), this, SLOT( slotModifyPath()));
    connect( m_pPathView, SIGNAL( selectionChanged ( QListViewItem * )), this, SLOT( slotSelectionChanged(QListViewItem * )));
    slotSelectionChanged(m_pPathView->currentItem());
}

void ConfigurePathPage::slotSelectionChanged(QListViewItem * item)
{
    m_modifyPath->setEnabled( item );
}

void ConfigurePathPage::slotModifyPath()
{
    QListViewItem *item = m_pPathView->currentItem ();
    if ( item )
    {
        if ( item->text(0)==i18n("Personal Expression"))
        {
            KoEditPathDia * dlg = new KoEditPathDia( item->text( 1),   0L, "editpath");
            if (dlg->exec() )
                item->setText(1, dlg->newPath());
            delete dlg;
        }
#if 0 // KWORD_HORIZONTAL_LINE
        if ( item->text(0)==i18n("Horizontal Line Path"))
        {
            KoEditPathDia * dlg = new KoEditPathDia( item->text( 1),   0L, "editpath");
            if (dlg->exec() )
                item->setText(1, dlg->newPath());
            delete dlg;
        }
#endif
        if ( item->text(0)==i18n("Picture Path"))
        {

            KURLRequesterDlg * dlg = new KURLRequesterDlg( item->text(1), 0L,
                                                           "picture path dlg");
            dlg->fileDialog()->setMode(KFile::Directory | KFile::LocalOnly);
            if ( dlg->exec() )
                item->setText( 1, dlg->selectedURL().path());
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
    QListViewItem * item = m_pPathView->findItem(i18n("Personal Expression"), 0);
    if ( item )
        item->setText(1, KWFactory::global()->dirs()->resourceDirs("expression").join(";"));
    item = m_pPathView->findItem(i18n("Picture Path"), 0);
    if ( item )
        item->setText(1, KGlobalSettings::documentPath());
    item = m_pPathView->findItem(i18n("Backup Path"), 0);
    if ( item )
        item->setText(1, QString::null );
#if 0 // KWORD_HORIZONTAL_LINE
    item = m_pPathView->findItem(i18n("Horizontal Line Path"), 0);
    if ( item )
        item->setText(1, KWFactory::global()->dirs()->resourceDirs("horizontalLine").join(";") );
#endif
}

void ConfigurePathPage::apply()
{
    QListViewItem * item = m_pPathView->findItem(i18n("Personal Expression"), 0);
    if ( item )
    {
        QStringList lst = QStringList::split(QString(";"), item->text(1));
        if ( lst != m_pView->kWordDocument()->personalExpressionPath())
        {
            m_pView->kWordDocument()->setPersonalExpressionPath(lst);
            config->setGroup( "Kword Path" );
            config->writeEntry( "expression path", lst);
        }
    }
#if 0 // KWORD_HORIZONTAL_LINE
    item = m_pPathView->findItem(i18n("Horizontal Line Path"), 0);
    if ( item )
    {
        QStringList lst = QStringList::split(QString(";"), item->text(1));
        if ( lst != m_pView->kWordDocument()->horizontalLinePath())
        {
            m_pView->kWordDocument()->setHorizontalLinePath(lst);
            config->setGroup( "Kword Path" );
            config->writeEntry( "horizontal line path", lst);
        }
    }
#endif
    item = m_pPathView->findItem(i18n("Picture Path"), 0);
    if ( item )
    {
        QString res = item->text(1 );
        if ( res != m_pView->kWordDocument()->picturePath())
        {
            config->setGroup( "Kword Path" );
            m_pView->kWordDocument()->setPicturePath( res );
#if KDE_IS_VERSION(3,1,3)
            config->writePathEntry( "picture path",res );
#else
            config->writeEntry( "picture path",res );
#endif
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
#if KDE_IS_VERSION(3,1,3)
            config->writePathEntry( "backup path",res );
#else
            config->writeEntry( "backup path",res );
#endif
        }
    }

}

#include "kwconfig.moc"
