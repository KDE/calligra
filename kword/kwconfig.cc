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

#include <qgroupbox.h>
#include <kiconloader.h>
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kdialogbase.h>
#include <qvbox.h>
#include <kconfig.h>
#include <qlabel.h>

#include "kwconfig.h"
#include "kwview.h"
#include "kwunit.h"
#include "kwdoc.h"

KWConfig::KWConfig( KWView* parent, const char* /*name*/)
  : KDialogBase(KDialogBase::IconList,i18n("Configure KWord") ,
		KDialogBase::Ok | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
  QVBox *page = addVBoxPage( i18n("Spelling"), i18n("Spell checker behavior"),
                          BarIcon("spellcheck", KIcon::SizeMedium) );
  _spellPage=new configureSpellPage(parent,page);
  page = addVBoxPage( i18n("Interface"), i18n("Interface"),
                          BarIcon("misc", KIcon::SizeMedium) );
  _interfacePage=new configureInterfacePage(parent,page);
  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));
}

void KWConfig::slotApply()
{
    _spellPage->apply();
    _interfacePage->apply();
}

void KWConfig::slotDefault()
{

    _interfacePage->slotDefault();
}

configureSpellPage::configureSpellPage( KWView *_view, QWidget *parent , char *name )
 :QWidget ( parent,name)
{
  m_pView=_view;
  config = KWFactory::global()->config();
  QVBoxLayout *box = new QVBoxLayout( this );
  box->setMargin( 5 );
  box->setSpacing( 10 );
  QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
  tmpQGroupBox->setTitle(i18n("Spelling"));
  QGridLayout *grid1 = new QGridLayout(tmpQGroupBox,8,1,15,7);
  _spellConfig  = new KSpellConfig(tmpQGroupBox ,0L ,m_pView->getGUI()->getDocument()->getKSpellConfig(), false );
  grid1->addWidget(_spellConfig,0,0);
  box->addWidget( tmpQGroupBox);
}

void configureSpellPage::apply()
{
  config->setGroup( "KSpell kword" );
  config->writeEntry ("KSpell_NoRootAffix",(int) _spellConfig->noRootAffix ());
  config->writeEntry ("KSpell_RunTogether", (int) _spellConfig->runTogether ());
  config->writeEntry ("KSpell_Dictionary", _spellConfig->dictionary ());
  config->writeEntry ("KSpell_DictFromList",(int)  _spellConfig->dictFromList());
  config->writeEntry ("KSpell_Encoding", (int)  _spellConfig->encoding());
  config->writeEntry ("KSpell_Client",  _spellConfig->client());
  m_pView->getGUI()->getDocument()->setKSpellConfig(*_spellConfig);
}

configureInterfacePage::configureInterfacePage( KWView *_view, QWidget *parent , char *name )
 :QWidget ( parent,name)
{
    m_pView=_view;
    config = KWFactory::global()->config();
    KWUnits unit=KWUnit::unitType( m_pView->getGUI()->getDocument()->getUnit() );
    QVBoxLayout *box = new QVBoxLayout( this );
    box->setMargin( 5 );
    box->setSpacing( 10 );
    QGroupBox* tmpQGroupBox = new QGroupBox( this, "GroupBox" );
    tmpQGroupBox->setTitle(i18n("Interface"));
    QGridLayout *grid1 = new QGridLayout(tmpQGroupBox,8,1,15,15);
    int m_iGridX=10;
    int m_iGridY=10;
    int m_iIndent=10;
    int m_iNumOfRecentFile=10;
    KWUnit tmpIndent;
    tmpIndent.setMM(10.0);
    if( config->hasGroup("Interface" ))
        {
            config->setGroup( "Interface" );
            m_iGridX=config->readNumEntry("GridX",10);
            m_iGridY=config->readNumEntry("GridY",10);
            double val=config->readDoubleNumEntry("Indent",POINT_TO_MM(10.0));
            tmpIndent.setPT(val);
            m_iNumOfRecentFile=config->readNumEntry("NbRecentFile",10);
        }
    m_iIndent=(int)tmpIndent.value( unit );
    QLabel *text=new QLabel(tmpQGroupBox);
    text->setText(i18n("X grid space"));
    grid1->addWidget(text,0,0);
    gridX=new KIntNumInput(m_iGridX, tmpQGroupBox , 10);
    gridX->setRange(1, 50, 1);
    grid1->addWidget(gridX,1,0);

    text=new QLabel(tmpQGroupBox);
    text->setText(i18n("Y grid space"));
    grid1->addWidget(text,2,0);

    gridY=new KIntNumInput(m_iGridY, tmpQGroupBox , 10);
    gridY->setRange(1, 50, 1);
    grid1->addWidget(gridY,3,0);

    QString unitText;
    switch ( unit )
      {
      case U_MM:
	unitText=i18n("in Millimeters (mm)");
	break;
      case U_INCH:
	unitText=i18n("in Inches (inch)");
	break;
      case U_PT:
      default:
	unitText=i18n("in points ( pt )" );
      }
    text=new QLabel(tmpQGroupBox);
    text->setText(i18n("Indent %1").arg(unitText));
    grid1->addWidget(text,4,0);

    indent=new KIntNumInput(m_iIndent, tmpQGroupBox , 10);
    indent->setRange(1, 50, 1);
    grid1->addWidget(indent,5,0);

    text=new QLabel(tmpQGroupBox);
    text->setText(i18n("Number of recent file:"));
    grid1->addWidget(text,6,0);

    oldNbRecentFiles=m_iNumOfRecentFile;
    recentFiles=new KIntNumInput(m_iNumOfRecentFile, tmpQGroupBox , 10);
    recentFiles->setRange(1, 20, 1);
    grid1->addWidget(recentFiles,7,0);

    box->addWidget( tmpQGroupBox);
}

void configureInterfacePage::apply()
{
    int valX=gridX->value();
    int valY=gridY->value();
    int nbRecent=recentFiles->value();

    KWUnit tmpIndent = KWUnit::createUnit( (double)indent->value(), KWUnit::unitType( m_pView->getGUI()->getDocument()->getUnit() ));
    config->setGroup( "Interface" );
    if(valX!=m_pView->getGUI()->getDocument()->gridX())
    {
        config->writeEntry( "GridX",valX );
        m_pView->getGUI()->getDocument()->setGridX(valX);
    }
    if(valY!=m_pView->getGUI()->getDocument()->gridY())
    {
        config->writeEntry( "GridY",valY );
        m_pView->getGUI()->getDocument()->setGridY(valY);
    }
    KWUnit _ind=m_pView->getGUI()->getDocument()->getIndentValue();
    if(tmpIndent.pt()!=_ind.pt())
    {
        config->writeEntry( "Indent", tmpIndent.pt());
        m_pView->getGUI()->getDocument()->setIndentValue( tmpIndent );
    }
    if(nbRecent!=oldNbRecentFiles)
    {
        config->writeEntry( "NbRecentFile", nbRecent);
        m_pView->changeNbOfRecentFiles(nbRecent);
    }
}

void configureInterfacePage::slotDefault()
{
    gridX->setValue(10);
    gridY->setValue(10);
    KWUnit tmpIndent;
    tmpIndent.setMM(10);
    int m_iIndent=(int)tmpIndent.value( KWUnit::unitType( m_pView->getGUI()->getDocument()->getUnit() ) );
    indent->setValue(m_iIndent);
}
#include "kwconfig.moc"
