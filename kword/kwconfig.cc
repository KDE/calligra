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

#include <kapp.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <kiconloader.h>
#include <klocale.h>
#include <knuminput.h>
#include <kspell.h>
#include <kdebug.h>


#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qcombobox.h>
#include <qlabel.h>

#include "kwconfig.h"
#include "kwview.h"
#include "kwunit.h"
#include "kwdoc.h"
#include "kwcanvas.h"
#include "kwviewmode.h"
#include <float.h>

KWConfig::KWConfig( KWView* parent )
  : KDialogBase(KDialogBase::IconList,i18n("Configure KWord") ,
		KDialogBase::Ok | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
  QVBox *page = addVBoxPage( i18n("Spelling"), i18n("Spell checker behavior"),
                        BarIcon("spellcheck", KIcon::SizeMedium) );
  _spellPage=new ConfigureSpellPage(parent, page);

  QVBox *page2 = addVBoxPage( i18n("Interface"), i18n("Interface"),
                              BarIcon("misc", KIcon::SizeMedium) );
  _interfacePage=new ConfigureInterfacePage(parent, page2);

   QVBox *page3 = addVBoxPage( i18n("Misc"), i18n("Misc"),
                              BarIcon("misc", KIcon::SizeMedium) );
  _miscPage=new ConfigureMiscPage(parent, page3);

  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));
}

void KWConfig::slotApply()
{
    _spellPage->apply();
    _interfacePage->apply();
    _miscPage->apply();
}

void KWConfig::slotDefault()
{
    switch(activePageIndex())
    {
        case 0:
            _spellPage->slotDefault();
            break;
        case 1:
            _interfacePage->slotDefault();
            break;
        case 2:
            _miscPage->slotDefault();
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

  QGridLayout *grid1 = new QGridLayout(tmpQGroupBox, 5, 1, KDialog::marginHint(), KDialog::spacingHint());
  grid1->addRowSpacing( 0, KDialog::marginHint() + 5 );
  grid1->setRowStretch( 4, 10 );
  _spellConfig = new KSpellConfig(tmpQGroupBox, 0L, m_pView->kWordDocument()->getKSpellConfig(), false );
  grid1->addWidget(_spellConfig,1,0);
#ifdef KSPELL_HAS_IGNORE_UPPER_WORD
  _dontCheckUpperWord= new QCheckBox(i18n("Ignore uppercase words"),tmpQGroupBox);
  grid1->addWidget(_dontCheckUpperWord,2,0);

  _dontCheckTilteCase= new QCheckBox(i18n("Ignore title case words"),tmpQGroupBox);
  grid1->addWidget(_dontCheckTilteCase,3,0);

  if( config->hasGroup("KSpell kword") )
    {
        config->setGroup( "KSpell kword" );
        _dontCheckUpperWord->setChecked(config->readBoolEntry("KSpell_dont_check_upper_word",false));
        _dontCheckTilteCase->setChecked(config->readBoolEntry("KSpell_dont_check_title_case",false));
    }
#endif
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

  m_pView->kWordDocument()->setKSpellConfig(*_spellConfig);

#ifdef KSPELL_HAS_IGNORE_UPPER_WORD
  bool state=_dontCheckUpperWord->isChecked();
  config->writeEntry ("KSpell_dont_check_upper_word",(int)state);
  m_pView->kWordDocument()->setDontCheckUpperWord(state);

  state=_dontCheckTilteCase->isChecked();
  config->writeEntry("KSpell_dont_check_title_case",(int)state);
  m_pView->kWordDocument()->setDontCheckTitleCase(state);
#endif
}

void ConfigureSpellPage::slotDefault()
{
    _spellConfig->setNoRootAffix( 0);
    _spellConfig->setRunTogether(0);
    _spellConfig->setDictionary( "");
    _spellConfig->setDictFromList( FALSE);
    _spellConfig->setEncoding (KS_E_ASCII);
    _spellConfig->setClient (KS_CLIENT_ISPELL);
#ifdef KSPELL_HAS_IGNORE_UPPER_WORD
    _dontCheckUpperWord->setChecked(false);
    _dontCheckTilteCase->setChecked(false);
#endif
}

ConfigureInterfacePage::ConfigureInterfacePage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KWFactory::global()->config();
    KWUnit::Unit unit = m_pView->kWordDocument()->getUnit();
    /*QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin( 5 );
    box->setSpacing( 10 );*/
    QGroupBox* tmpQGroupBox = new QGroupBox( box, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Interface"));

    QVBoxLayout *lay1 = new QVBoxLayout(tmpQGroupBox);
    lay1->addSpacing( 10 );
    lay1->setMargin( KDialog::marginHint() );
    lay1->setSpacing( KDialog::spacingHint() );

    double ptGridX=10.0;
    double ptGridY=10.0;
    double ptIndent = MM_TO_POINT(10.0);
    bool bShowRuler=true;

    oldNbRecentFiles=10;
    oldAutoSaveValue=KoDocument::defaultAutoSave() / 60;
    int nbPagePerRow=4;
    if( config->hasGroup("Interface") )
    {
        config->setGroup( "Interface" );
        ptGridX=config->readDoubleNumEntry("GridX", ptGridX);
        ptGridY=config->readDoubleNumEntry("GridY", ptGridY);
        ptIndent = config->readDoubleNumEntry("Indent", ptIndent);
        oldNbRecentFiles=config->readNumEntry("NbRecentFile",oldNbRecentFiles);
        bShowRuler=config->readBoolEntry("Rulers",true);
        oldAutoSaveValue=config->readNumEntry("AutoSave",oldAutoSaveValue);
        nbPagePerRow=config->readNumEntry("nbPagePerRow",nbPagePerRow);
    }


    showRuler= new QCheckBox(i18n("Show rulers"),tmpQGroupBox);
    showRuler->setChecked(bShowRuler);
    lay1->addWidget(showRuler);

    autoSave = new KIntNumInput( oldAutoSaveValue, tmpQGroupBox );
    autoSave->setRange(0, 60, 1);
    autoSave->setLabel(i18n("Auto save (min):"));
    autoSave->setSpecialValueText(i18n("No auto save"));
    autoSave->setSuffix(i18n(" min"));

    lay1->addWidget(autoSave);

    recentFiles=new KIntNumInput( oldNbRecentFiles, tmpQGroupBox );
    recentFiles->setRange(1, 20, 1);
    recentFiles->setLabel(i18n("Number of recent file:"));

    lay1->addWidget(recentFiles);

    QString suffix = KWUnit::unitName( unit ).prepend(' ');
    gridX=new KDoubleNumInput( KWUnit::userValue( ptGridX, unit ), tmpQGroupBox );
    gridX->setRange(0.1, 50, 0.1);
    gridX->setFormat( "%.1f" );
    gridX->setSuffix( suffix );
    gridX->setLabel(i18n("X grid space"));
    lay1->addWidget(gridX);

    gridY=new KDoubleNumInput( KWUnit::userValue( ptGridY, unit ), tmpQGroupBox );
    gridY->setRange(0.1, 50, 0.1);
    gridY->setFormat( "%.1f" );
    gridY->setLabel(i18n("Y grid space"));
    gridY->setSuffix( suffix );
    lay1->addWidget(gridY);

    double val = KWUnit::userValue( ptIndent, unit );
    indent = new KDoubleNumInput( val, tmpQGroupBox );
    indent->setRange(0.1, 50, 0.1);
    indent->setFormat( "%.1f" );
    indent->setSuffix( suffix );
    indent->setLabel(i18n("Paragraph indent by toolbar buttons"));

    lay1->addWidget(indent);

    m_nbPagePerRow=new KIntNumInput(nbPagePerRow, tmpQGroupBox );
    m_nbPagePerRow->setRange(1, 10, 1);
    m_nbPagePerRow->setLabel(i18n("Preview mode - Number of pages per row:"));

    lay1->addWidget(m_nbPagePerRow);

    //box->addWidget( tmpQGroupBox);
}

void ConfigureInterfacePage::apply()
{
    KWDocument * doc = m_pView->kWordDocument();
    double valX=KWUnit::fromUserValue( gridX->value(), doc->getUnit() );
    //kdDebug() << "ConfigureInterfacePage::apply gridX->value()=" << gridX->value() << " valX=" << valX << endl;
    double valY=KWUnit::fromUserValue( gridY->value(), doc->getUnit() );
    int nbRecent=recentFiles->value();
    bool ruler=showRuler->isChecked();

    config->setGroup( "Interface" );
    if(valX!=doc->gridX())
    {
        //kdDebug() << "ConfigureInterfacePage::apply writing gridX=" << valX << endl;
        config->writeEntry( "GridX", valX, true, false, 'g', DBL_DIG /* 6 is not enough */ );
        doc->setGridX(valX);
    }
    if(valY!=doc->gridY())
    {
        config->writeEntry( "GridY", valY, true, false, 'g', DBL_DIG /* 6 is not enough */ );
        doc->setGridY(valY);
    }

    double newIndent = KWUnit::fromUserValue( indent->value(), doc->getUnit() );
    if( newIndent != doc->getIndentValue() )
    {
        config->writeEntry( "Indent", newIndent, true, false, 'g', DBL_DIG /* 6 is not enough */ );
        doc->setIndentValue( newIndent );
    }
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
    int autoSaveVal=autoSave->value();
    if(autoSaveVal!=oldAutoSaveValue)
    {
        config->writeEntry( "AutoSave", autoSaveVal );
        doc->setAutoSave(autoSaveVal*60);
    }
    int nbPageByRow=m_nbPagePerRow->value();
    if(nbPageByRow!=doc->getNbPagePerRow())
    {
        config->writeEntry("nbPagePerRow",nbPageByRow);
        m_pView->getGUI()->canvasWidget()->viewMode()->setPagesPerRow(nbPageByRow);
        doc->setNbPagePerRow(nbPageByRow);
        m_pView->getGUI()->canvasWidget()->refreshViewMode();
    }
}

void ConfigureInterfacePage::slotDefault()
{
    KWDocument * doc = m_pView->kWordDocument();
    gridX->setValue( KWUnit::userValue( 10, doc->getUnit() ) );
    gridY->setValue( KWUnit::userValue( 10, doc->getUnit() ) );
    m_nbPagePerRow->setValue(4);
    double newIndent = KWUnit::userValue( MM_TO_POINT( 10 ), doc->getUnit() );
    indent->setValue( newIndent );
    recentFiles->setValue(10);
    showRuler->setChecked(true);
    autoSave->setValue(KoDocument::defaultAutoSave()/60);
}


ConfigureMiscPage::ConfigureMiscPage( KWView *_view, QVBox *box, char *name )
 : QObject( box->parent(), name )
{
    m_pView=_view;
    config = KWFactory::global()->config();
    KWUnit::Unit unit = m_pView->kWordDocument()->getUnit();
    QGroupBox* tmpQGroupBox = new QGroupBox( box, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Misc"));

    QGridLayout *grid = new QGridLayout( tmpQGroupBox , 8, 1, KDialog::marginHint()+7, KDialog::spacingHint() );

    QString unitType=KWUnit::unitName(unit);
    if( config->hasGroup("Misc") )
    {
        config->setGroup( "Misc" );
        unitType=config->readEntry("Units",unitType);
    }

    QLabel *unitLabel= new QLabel(i18n("Unit:"),tmpQGroupBox);
    grid->addWidget(unitLabel,0,0);

    QStringList listUnit;
    listUnit << KWUnit::unitDescription( KWUnit::U_MM );
    listUnit << KWUnit::unitDescription( KWUnit::U_INCH );
    listUnit << KWUnit::unitDescription( KWUnit::U_PT );
    m_unit = new QComboBox( tmpQGroupBox );
    m_unit->insertStringList(listUnit);
    m_oldUnit=0;
    switch (KWUnit::unit( unitType ) )
    {
        case KWUnit::U_MM:
            m_oldUnit=0;
            break;
        case KWUnit::U_INCH:
            m_oldUnit=1;
            break;
        case KWUnit::U_PT:
        default:
            m_oldUnit=2;
    }
    m_unit->setCurrentItem(m_oldUnit);

    grid->addWidget(m_unit,1,0);

}

void ConfigureMiscPage::apply()
{
    config->setGroup( "Misc" );
    if(m_oldUnit!=m_unit->currentItem())
    {
        QString unitName;
        switch (m_unit->currentItem())
        {
            case 0:
                unitName=KWUnit::unitName(KWUnit::U_MM  );
                break;
            case 1:
                unitName=KWUnit::unitName(KWUnit::U_INCH  );
                break;
            case 2:
            default:
                unitName=KWUnit::unitName(KWUnit::U_PT );
        }

        config->writeEntry("Units",unitName);
    }
}

void ConfigureMiscPage::slotDefault()
{
    //todo
}

#include "kwconfig.moc"
