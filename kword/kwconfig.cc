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

#include "kwconfig.h"
#include "kwview.h"
#include "kwdoc.h"
#include <kiconloader.h>
#include <qlayout.h>
#include <kapp.h>
#include <klocale.h>
#include <kdialogbase.h>
#include <qvbox.h>
#include <kconfig.h>



KWConfig::KWConfig( KWView* parent, const char* /*name*/)
  : KDialogBase(KDialogBase::IconList,i18n("Configure KWord") ,
		KDialogBase::Ok | KDialogBase::Cancel| KDialogBase::Default,
		KDialogBase::Ok)

{
  QVBox *page = addVBoxPage( i18n("Spelling"), i18n("Spell checker behavior"),
                          BarIcon("spellcheck", KIcon::SizeMedium) );
  _spellPage=new configureSpellPage(parent,page);
  connect(this, SIGNAL(okClicked()),this,SLOT(slotApply()));
}

void KWConfig::slotApply()
{
  _spellPage->apply();
}

void KWConfig::slotDefault()
{
  //todo
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
#include "kwconfig.moc"
