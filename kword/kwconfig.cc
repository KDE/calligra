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
#include <qhbox.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qvgroupbox.h>
#include <qpushbutton.h>

#include "kwconfig.h"
#include "kwview.h"
#include "kwdoc.h"
#include "kwcanvas.h"
#include "kwviewmode.h"
#include "kwcommand.h"
#include "kwvariable.h"
#include "koeditpath.h"

#include <koVariable.h>
#include <kformulaconfigpage.h>

#include <float.h>
#include <kmessagebox.h>
#include <klistview.h>
#include <kstandarddirs.h>
#include <kdeversion.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kurlrequesterdlg.h>
#include <kfiledialog.h>
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
    KCommand * cmd = m_miscPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change config"));
        macro->addCommand(cmd);
    }

    cmd=m_defaultDocPage->apply();
    if ( cmd )
    {
        if ( !macro )
            macro = new KMacroCommand( i18n("Change config"));

        macro->addCommand( cmd );
    }
    m_formulaPage->apply();
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
    QGroupBox* tmpQGroupBox = new QGroupBox( box, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Spelling"));

    QGridLayout *grid1 = new QGridLayout(tmpQGroupBox, 6, 1, KDialog::marginHint(), KDialog::spacingHint());
    grid1->addRowSpacing( 0, KDialog::marginHint() + 5 );
    grid1->setRowStretch( 5, 10 );
    _spellConfig = new KSpellConfig(tmpQGroupBox, 0L, m_pView->kWordDocument()->getKSpellConfig(), false );
    grid1->addWidget(_spellConfig,1,0);

    _dontCheckUpperWord= new QCheckBox(i18n("Ignore uppercase words"),tmpQGroupBox);
    QWhatsThis::add( _dontCheckUpperWord, i18n("This option tells the spell-checker to accept words that are written in uppercase, such as KDE.") );
    grid1->addWidget(_dontCheckUpperWord,2,0);

    _dontCheckTitleCase= new QCheckBox(i18n("Ignore title case words"),tmpQGroupBox);
    QWhatsThis::add( _dontCheckTitleCase, i18n("This option tells the spell-checker to accept words starting with an uppercase letter, such as United States."));
    grid1->addWidget(_dontCheckTitleCase,3,0);

    cbBackgroundSpellCheck=new QCheckBox(i18n("Show misspelled words in document"),tmpQGroupBox);
    cbBackgroundSpellCheck->setChecked( m_pView->kWordDocument()->backgroundSpellCheckEnabled() );
    grid1->addWidget(cbBackgroundSpellCheck,4,0);

    clearIgnoreAllHistory= new QPushButton( i18n("Clear Ignore All Word History..."),tmpQGroupBox);
    grid1->addMultiCellWidget(clearIgnoreAllHistory,5,5,0,1);
    connect( clearIgnoreAllHistory, SIGNAL(clicked()),this, SLOT(slotClearIgnoreAllHistory()));

    if( config->hasGroup("KSpell kword") )
    {
        config->setGroup( "KSpell kword" );
        _dontCheckUpperWord->setChecked(config->readBoolEntry("KSpell_dont_check_upper_word",false));
        _dontCheckTitleCase->setChecked(config->readBoolEntry("KSpell_dont_check_title_case",false));
    }

}

void ConfigureSpellPage::apply()
{
  config->setGroup( "KSpell kword" );
  config->writeEntry ("KSpell_NoRootAffix",(int) _spellConfig->noRootAffix ());
  config->writeEntry ("KSpell_RunTogether", (int) _spellConfig->runTogether ());
  config->writeEntry ("KSpell_Dictionary", _spellConfig->dictionary ());
  config->writeEntry ("KSpell_DictFromList",(int)  _spellConfig->dictFromList());
  config->writeEntry ("KSpell_Encoding", (int)  _spellConfig->encoding());
  config->writeEntry ("KSpell_Client",  _spellConfig->client());

  KWDocument* doc = m_pView->kWordDocument();
  doc->setKSpellConfig(*_spellConfig);

  bool state=_dontCheckUpperWord->isChecked();
  config->writeEntry ("KSpell_dont_check_upper_word",(int)state);
  doc->setDontCheckUpperWord(state);

  state=_dontCheckTitleCase->isChecked();
  config->writeEntry("KSpell_dont_check_title_case",(int)state);
  doc->setDontCheckTitleCase(state);

  state=cbBackgroundSpellCheck->isChecked();
  config->writeEntry( "SpellCheck", (int)state );

  //FIXME reactivate just if there is a changes.
  doc->enableBackgroundSpellCheck( state );
  doc->reactivateBgSpellChecking();
}

void ConfigureSpellPage::slotClearIgnoreAllHistory()
{
    int ret = KMessageBox::warningContinueCancel(0L,
                                                 i18n("Warning! You are about to erase the entire Ignore word history."));
    if (ret == KMessageBox::Continue)
        m_pView->kWordDocument()->clearIgnoreWordAll();
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
    _dontCheckTitleCase->setChecked(false);
    cbBackgroundSpellCheck->setChecked(false);
}

ConfigureInterfacePage::ConfigureInterfacePage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KWFactory::global()->config();
    KoUnit::Unit unit = m_pView->kWordDocument()->getUnit();
    QVGroupBox* gbInterfaceGroup = new QVGroupBox( i18n("Interface"), box, "GroupBox" );
    gbInterfaceGroup->setMargin( 10 );
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
                         "move the text caret, similar to other KDE applications. "
                         "If it is disabled, they move the scrollbars, similar to most other word processors." ) );

    recentFiles=new KIntNumInput( oldNbRecentFiles, gbInterfaceGroup );
    recentFiles->setRange(1, 20, 1);
    recentFiles->setLabel(i18n("Number of recent &files:"));
    QWhatsThis::add( recentFiles, i18n("The amount of files remembered in the file open dialog and in the "
                    "recent files menu item") );

    QString suffix = KoUnit::unitName( unit ).prepend(' ');
    gridX=new KDoubleNumInput( recentFiles, KoUnit::ptToUnit( ptGridX, unit ), gbInterfaceGroup );
    gridX->setRange(KoUnit::ptToUnit( 0.1, unit ),KoUnit::ptToUnit( 50, unit ), KoUnit::ptToUnit( 0.1, unit ));
    //laurent kdoublenuminput changed !!!!
    //setPrecision doen't work it return a value display * 10^precision !!!!
    //perhaps it's normal in new API ....
#if KDE_VERSION < 307
    gridX->setPrecision(1);
#endif
    gridX->setSuffix( suffix );
    gridX->setLabel(i18n("&Horizontal grid size:"));
    QWhatsThis::add( gridX, i18n("The grid size on which frames, tabs and other content snaps while "
                    "moving and scaling") );

    gridY=new KDoubleNumInput( gridX, KoUnit::ptToUnit( ptGridY, unit ), gbInterfaceGroup );
    gridY->setRange(KoUnit::ptToUnit( 0.1, unit ), KoUnit::ptToUnit( 50, unit ), KoUnit::ptToUnit( 0.1, unit ));
    //laurent kdoublenuminput changed !!!!
    //setPrecision doen't work it return a value display * 10^precision !!!!
    //perhaps it's normal in new API ....
#if KDE_VERSION < 307
    gridY->setPrecision(1);
#endif

    gridY->setLabel(i18n("&Vertical grid size:"));
    QWhatsThis::add( gridY, i18n("The grid size on which frames and other content snaps while "
                    "moving and scaling") );
    gridY->setSuffix( suffix );

    double val = KoUnit::ptToUnit( ptIndent, unit );
    indent = new KDoubleNumInput( gridY, val, gbInterfaceGroup );
    indent->setRange(KoUnit::ptToUnit( 0.1, unit ), KoUnit::ptToUnit( 50, unit ), KoUnit::ptToUnit( 0.1, unit ));
    //laurent kdoublenumvalidator changed !!!!
    //setPrecision doen't work it return a value display * 10^precision !!!!
    //perhaps it's normal in new API ....
#if KDE_VERSION < 307
    indent->setPrecision(1);
#endif

    indent->setSuffix( suffix );
    indent->setLabel(i18n("&Paragraph indent by toolbar buttons:"));
    QWhatsThis::add( indent, i18n("Configure the indent width used when using the Increase "
                    "or Decrease indentation buttons on a paragraph.<p>The lower the value "
                    "the more often the buttons will have to be pressed to gain the same "
                    "indentation") );

    m_nbPagePerRow=new KIntNumInput( indent, nbPagePerRow, gbInterfaceGroup );
    m_nbPagePerRow->setRange(1, 10, 1);
    m_nbPagePerRow->setLabel(i18n("Preview mode - Number of pa&ges per row:"));
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
    gbMiscGroup->setMargin( 10 );
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
        default:
            m_oldUnit=2;
    }
    m_unit->setCurrentItem(m_oldUnit);
    QString unitHelp = i18n("Select the unit type used every time a distance or width/height "
                "is displayed or entered. This one setting is for the whole of KWord");
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
    m_displayFieldCode->setChecked(doc->getVariableCollection()->variableSetting()->displayFiedCode());


    QVGroupBox* gbViewFormatting = new QVGroupBox( i18n("View Formatting"), box, "view_formatting" );
    gbViewFormatting->setMargin( 10 );
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
            default:
                doc->setUnit( KoUnit::U_PT );
                unitName=KoUnit::unitName(KoUnit::U_PT );
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
    if(doc->getVariableCollection()->variableSetting()->displayFiedCode()!=b)
    {
        if(!macroCmd)
        {
            macroCmd=new KMacroCommand(i18n("Change Display Field Code Command"));
        }
        KWChangeVariableSettingsCommand *cmd=new KWChangeVariableSettingsCommand( i18n("Change Display Field Code Command"), doc, doc->getVariableCollection()->variableSetting()->displayFiedCode() ,b, KWChangeVariableSettingsCommand::VS_DISPLAYFIELDCODE);
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
    gbDocumentDefaults->setMargin( 10 );
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
    //laurent kdoublenuminput changed !!!!
    //setPrecision doen't work it return a value display * 10^precision !!!!
    //perhaps it's normal in new API ....
#if KDE_VERSION < 307
    columnSpacing->setPrecision(1);
#endif
    columnSpacing->setSuffix( suffix );
    columnSpacing->setLabel(i18n("Default column spacing:"));
    QWhatsThis::add( columnSpacing, i18n("When setting a document to use more then one column "
                "This distance will be used to seperate the columns. This value is merely a default "
                "setting as the column spacing can be changed per document") );

    QWidget *fontContainer = new QWidget(gbDocumentDefaults);
    QGridLayout * fontLayout = new QGridLayout(fontContainer, 1, 3);

    fontLayout->setColStretch(0, 0);
    fontLayout->setColStretch(1, 1);
    fontLayout->setColStretch(2, 0);

    QLabel *fontTitle = new QLabel(i18n("Default font:"), fontContainer);

    font= new QFont( doc->defaultFont() );
    font->setPointSize( KoTextZoomHandler::layoutUnitPtToPt( font->pointSize() ) );

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

    oldAutoSaveValue=KoDocument::defaultAutoSave() / 60;
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        oldAutoSaveValue=config->readNumEntry("AutoSave",oldAutoSaveValue);
    }
    autoSave = new KIntNumInput( oldAutoSaveValue, gbDocumentSettings );
    autoSave->setRange(0, 60, 1);
    autoSave->setLabel(i18n("Autosave every (min):"));
    QWhatsThis::add( autoSave, i18n("A backup copy of the current document is created when a change "
                    "has been made. The interval used to create backup documents is set here.") );
    autoSave->setSpecialValueText(i18n("No autosave"));
    autoSave->setSuffix(i18n(" min"));

    m_oldBackupFile = true;
    m_oldBackupFileExtension = QString::fromLatin1("~");
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        m_oldBackupFile=config->readBoolEntry("BackupFile",m_oldBackupFile);
        m_oldBackupFileExtension=config->readEntry("BackupFileExtension",m_oldBackupFileExtension);
    }

    m_createBackupFile = new QCheckBox( i18n("Create Backup File"), gbDocumentSettings );

    m_createBackupFile->setChecked( m_oldBackupFile );

    QHBox* gbHorziGroupBox = new QHBox( gbDocumentSettings );

    new QLabel(i18n("Backup File Extension:"), gbHorziGroupBox);
    m_backupFileExtension = new QLineEdit(gbHorziGroupBox );
    m_backupFileExtension->setText( m_oldBackupFileExtension );

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
    gbDocumentCursor->setMargin( 10 );
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
    QString tmp = m_backupFileExtension->text();
    if ( tmp != m_oldBackupFileExtension)
    {
        if ( tmp.isEmpty())
            tmp = QString::fromLatin1("~");
        config->writeEntry( "BackupFileExtension", tmp );
        doc->setBackupFileExtension( tmp );
        m_oldBackupFileExtension = tmp;
    }

    state = m_cursorInProtectedArea->isChecked();
    if ( state != doc->cursorInProtectedArea() )
    {
        config->writeEntry( "cursorInProtectArea", state );
        doc->setCursorInProtectedArea( state );
    }

    state = m_directInsertCursor->isChecked();
    if ( state != doc->insertDirectCursor() )
    {
        config->writeEntry( "InsertDirectCursor", state );
        doc->setInsertDirectCursor( state );
    }

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
   m_backupFileExtension->setText( QString::fromLatin1("~") );
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

ConfigurePathPage::ConfigurePathPage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    KWDocument * doc = m_pView->kWordDocument();
    config = KWFactory::global()->config();
    QVGroupBox* gbPathGroup = new QVGroupBox( i18n("Path"), box, "GroupBox" );
    gbPathGroup->setMargin( 10 );
    gbPathGroup->setInsideSpacing( KDialog::spacingHint() );

    m_pPathView = new KListView( gbPathGroup );
    m_pPathView->setResizeMode(QListView::NoColumn);
    m_pPathView->addColumn( i18n( "Type" ) );
    m_pPathView->addColumn( i18n( "Path" ) );
    (void) new QListViewItem( m_pPathView, i18n("Personal Expression"), doc->personalExpressionPath().join(";") );
    (void) new QListViewItem( m_pPathView, i18n("Picture path"),doc->picturePath() );

    m_modifyPath = new QPushButton( i18n("Modify path..."), gbPathGroup);
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
    if ( item && (item->text(0)==i18n("Personal Expression")))
    {
        KoEditPathDia * dlg = new KoEditPathDia( item->text( 1),   0L, "editpath");
        if (dlg->exec() )
        {
            item->setText(1, dlg->newPath());
        }
        delete dlg;
    }
    if ( item && (item->text(0)==i18n("Picture path")))
    {

        KURLRequesterDlg * dlg = new KURLRequesterDlg( item->text(1), 0L,
                      "picture path dlg");
        dlg->fileDialog()->setMode(KFile::Directory | KFile::LocalOnly);
        if ( dlg->exec() )
        {
            item->setText( 1, dlg->selectedURL().path());
        }
        delete dlg;
    }

}

void ConfigurePathPage::slotDefault()
{
    QListViewItem * item = m_pPathView->findItem(i18n("Personal Expression"), 0);
    if ( item )
        item->setText(1, KWFactory::global()->dirs()->resourceDirs("expression").join(";"));
    item = m_pPathView->findItem(i18n("Picture path"), 0);
    if ( item )
        item->setText(1, KGlobalSettings::documentPath());
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
    item = m_pPathView->findItem(i18n("Picture path"), 0);
    if ( item )
    {
        QString res = item->text(1 );
        if ( res != m_pView->kWordDocument()->picturePath())
        {
            config->setGroup( "Kword Path" );
            m_pView->kWordDocument()->setPicturePath( res );
            config->writeEntry( "picture path",res );
        }
    }
}

#include "kwconfig.moc"
