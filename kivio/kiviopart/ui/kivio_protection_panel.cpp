/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kivio_protection_panel.h"
#include "kivio_protection_panel.moc"

#include "kivio_doc.h"
#include "kivio_layer.h"
#include "kivio_page.h"
#include "kivio_stencil.h"
#include "kivio_view.h"
#include "kivio_command.h"

#include <qbitarray.h>
#include <QCheckBox>
#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>

KivioProtectionPanel::KivioProtectionPanel( KivioView *view, QWidget *parent, const char *name )
   : KivioProtectionPanelBase(parent, name), m_pView(view)
{
   QObject::connect( m_checkWidth, SIGNAL(toggled(bool)), this, SLOT(togWidth(bool)) );
   QObject::connect( m_checkHeight, SIGNAL(toggled(bool)), this, SLOT(togHeight(bool)) );
   QObject::connect( m_checkAspect, SIGNAL(toggled(bool)), this, SLOT(togAspect(bool)) );
   QObject::connect( m_checkDeletion, SIGNAL(toggled(bool)), this, SLOT(togDelete(bool)) );
   QObject::connect( m_checkXPosition, SIGNAL(toggled(bool)), this, SLOT(togX(bool)) );
   QObject::connect( m_checkYPosition, SIGNAL(toggled(bool)), this, SLOT(togY(bool)) );
}

KivioProtectionPanel::~KivioProtectionPanel()
{
}

void KivioProtectionPanel::togWidth(bool on)
{
  KivioStencil* pStencil = m_pView->activePage()->selectedStencils()->first();
  KMacroCommand * macro = new KMacroCommand(i18n("Change Protection Attribute"));
  bool createMacro = false;

  while(pStencil) {
    if(pStencil->canProtect()->at(kpWidth) == true) {
      KivioChangeStencilProtectCommand* cmd = new KivioChangeStencilProtectCommand(i18n("Change Protection Attribute"),
          m_pView->activePage(), pStencil, on, KivioChangeStencilProtectCommand::KV_WIDTH);
      macro->addCommand(cmd);
      createMacro = true;
    } else {
      kDebug(43000) << "Attempt to set width-protection of a stencil which does not support it.\n";
    }

    pStencil = m_pView->activePage()->selectedStencils()->next();
  }

  if(createMacro) {
    macro->execute();
    m_pView->doc()->addCommand(macro);
  } else {
    delete macro;
  }
}

void KivioProtectionPanel::togHeight(bool on)
{
  KMacroCommand* macro = new KMacroCommand(i18n("Change Protection Attribute"));
  bool createMacro = false;
  KivioStencil* pStencil = m_pView->activePage()->selectedStencils()->first();

  while(pStencil) {
    if(pStencil->canProtect()->at(kpHeight) == true) {
      KivioChangeStencilProtectCommand* cmd = new KivioChangeStencilProtectCommand(i18n("Change Protection Attribute"),
          m_pView->activePage(), pStencil, on, KivioChangeStencilProtectCommand::KV_HEIGHT);
      macro->addCommand(cmd);
      createMacro = true;
    } else {
      kDebug(43000) << "Attempt to set height-protection of a stencil which does not support it.\n";
    }

    pStencil = m_pView->activePage()->selectedStencils()->next();
  }

  if(createMacro) {
    macro->execute();
    m_pView->doc()->addCommand(macro);
  } else {
    delete macro;
  }
}

void KivioProtectionPanel::togAspect(bool on)
{
  KMacroCommand* macro = new KMacroCommand(i18n("Change Protection Attribute"));
  bool createMacro = false;
  KivioStencil* pStencil = m_pView->activePage()->selectedStencils()->first();

  while(pStencil) {
    if(pStencil->canProtect()->at(kpAspect) == true) {
      KivioChangeStencilProtectCommand* cmd = new KivioChangeStencilProtectCommand(i18n("Change Protection Attribute"),
          m_pView->activePage(), pStencil, on, KivioChangeStencilProtectCommand::KV_ASPECT);
      macro->addCommand(cmd);
      createMacro = true;
    } else {
      kDebug(43000) << "Attempt to set height-protection of a stencil which does not support it.\n";
    }

    pStencil = m_pView->activePage()->selectedStencils()->next();
  }

  if(createMacro) {
    macro->execute();
    m_pView->doc()->addCommand(macro);
  } else {
      delete macro;
  }
}

void KivioProtectionPanel::togDelete(bool on)
{
  KMacroCommand * macro = new KMacroCommand(i18n("Change Protection Attribute"));
  bool createMacro=false;
  KivioStencil* pStencil = m_pView->activePage()->selectedStencils()->first();

  while(pStencil) {
    if(pStencil->canProtect()->at(kpDeletion) == true) {
      KivioChangeStencilProtectCommand* cmd = new KivioChangeStencilProtectCommand(i18n("Change Protection Attribute"),
          m_pView->activePage(), pStencil, on, KivioChangeStencilProtectCommand::KV_DELETE);
      macro->addCommand(cmd);
      createMacro = true;
    } else {
      kDebug(43000) << "Attempt to set delete-protection of a stencil which does not support it.\n";
    }

    pStencil = m_pView->activePage()->selectedStencils()->next();
  }

  if(createMacro) {
    macro->execute();
    m_pView->doc()->addCommand(macro);
  } else {
    delete macro;
  }
}

void KivioProtectionPanel::togX(bool on)
{
  KMacroCommand * macro = new KMacroCommand(i18n("Change Protection Attribute"));
  bool createMacro=false;
  KivioStencil* pStencil = m_pView->activePage()->selectedStencils()->first();

  while(pStencil) {
    if(pStencil->canProtect()->at(kpX) == true) {
      KivioChangeStencilProtectCommand* cmd = new KivioChangeStencilProtectCommand(i18n("Change Protection Attribute"),
          m_pView->activePage(), pStencil, on, KivioChangeStencilProtectCommand::KV_POSX);
      macro->addCommand(cmd);
      createMacro = true;
    } else {
      kDebug(43000) << "Attempt to set X-protection of a stencil which does not support it.\n";
    }

    pStencil = m_pView->activePage()->selectedStencils()->next();
  }

  if(createMacro) {
    macro->execute();
    m_pView->doc()->addCommand(macro);
  } else {
    delete macro;
  }
}


void KivioProtectionPanel::togY(bool on)
{
  KMacroCommand * macro = new KMacroCommand(i18n("Change Protection Attribute"));
  bool createMacro=false;
  KivioStencil* pStencil = m_pView->activePage()->selectedStencils()->first();

  while(pStencil) {
    if(pStencil->canProtect()->at(kpY) == true) {
      KivioChangeStencilProtectCommand* cmd = new KivioChangeStencilProtectCommand(i18n("Change Protection Attribute"),
          m_pView->activePage(), pStencil, on, KivioChangeStencilProtectCommand::KV_POSY);
      macro->addCommand(cmd);
      createMacro = true;
    } else {
      kDebug(43000) << "Attempt to set Y-protection of a stencil which does not support it.\n";
    }

    pStencil = m_pView->activePage()->selectedStencils()->next();
  }

  if(createMacro) {
    macro->execute();
    m_pView->doc()->addCommand(macro);
  } else {
    delete macro;
  }
}

void KivioProtectionPanel::updateCheckBoxes()
{
  // If there are no selected stencils, then disable all checkboxes,
  // and uncheck them
  if(m_pView->activePage()->selectedStencils()->count() == 0)
  {
    m_checkAspect->setEnabled(false);
    m_checkXPosition->setEnabled(false);
    m_checkYPosition->setEnabled(false);
    m_checkDeletion->setEnabled(false);
    m_checkHeight->setEnabled(false);
    m_checkWidth->setEnabled(false);

    m_checkAspect->setChecked(false);
    m_checkXPosition->setChecked(false);
    m_checkYPosition->setChecked(false);
    m_checkDeletion->setChecked(false);
    m_checkHeight->setChecked(false);
    m_checkWidth->setChecked(false);

    return;
  }

  QBitArray bits( NUM_PROTECTIONS );
  int i;

  // Assume all protections are valid by default
  for(i = 0; i < NUM_PROTECTIONS; i++) {
    bits.setBit(i);
  }


  KivioStencil* pStencil = m_pView->activePage()->selectedStencils()->first();

  while(pStencil) {
    // Now build an array representing which protection fields are valid
    // since there could be multiple types of stencils selected with varying
    // protection abilities
    for(i = 0; i < NUM_PROTECTIONS; i++) {
      if(pStencil->canProtect()->at(i) == false) {
        bits.clearBit(i);
      }
    }

    pStencil = m_pView->activePage()->selectedStencils()->next();
  }

  QObject::disconnect(m_checkWidth, SIGNAL(toggled(bool)), this, SLOT(togWidth(bool)));
  QObject::disconnect(m_checkHeight, SIGNAL(toggled(bool)), this, SLOT(togHeight(bool)));
  QObject::disconnect(m_checkAspect, SIGNAL(toggled(bool)), this, SLOT(togAspect(bool)));
  QObject::disconnect(m_checkDeletion, SIGNAL(toggled(bool)), this, SLOT(togDelete(bool)));
  QObject::disconnect(m_checkXPosition, SIGNAL(toggled(bool)), this, SLOT(togX(bool)));
  QObject::disconnect(m_checkYPosition, SIGNAL(toggled(bool)), this, SLOT(togY(bool)));

  // Enable and disable the check boxes based on what protections are available
  for(i = 0; i < NUM_PROTECTIONS; i++) {
    bool on = bits.at(i);

    switch(i) {
      case kpX:
        m_checkXPosition->setEnabled(on);
        break;

      case kpY:
        m_checkYPosition->setEnabled(on);
        break;

      case kpWidth:
        m_checkWidth->setEnabled(on);
        break;

      case kpHeight:
        m_checkHeight->setEnabled(on);
        break;

      case kpDeletion:
        m_checkDeletion->setEnabled(on);
        break;

      case kpAspect:
        m_checkAspect->setEnabled(on);
        break;

      default:
        break;
    }
  }



  // If we have more than one stencil selected, then clear all checkboxes
  if(m_pView->activePage()->selectedStencils()->count() > 1) {
    m_checkAspect->setChecked(false);
    m_checkXPosition->setChecked(false);
    m_checkYPosition->setChecked(false);
    m_checkDeletion->setChecked(false);
    m_checkHeight->setChecked(false);
    m_checkWidth->setChecked(false);
  } else { // Otherwise fill in the checkboxes with the single selection
    pStencil = m_pView->activePage()->selectedStencils()->first();

    m_checkAspect->setChecked( pStencil->protection()->at(kpAspect) );
    m_checkDeletion->setChecked( pStencil->protection()->at(kpDeletion) );
    m_checkXPosition->setChecked( pStencil->protection()->at(kpX) );
    m_checkYPosition->setChecked( pStencil->protection()->at(kpY) );
    m_checkWidth->setChecked( pStencil->protection()->at(kpWidth) );
    m_checkHeight->setChecked( pStencil->protection()->at(kpHeight) );
  }

  QObject::connect(m_checkWidth, SIGNAL(toggled(bool)), this, SLOT(togWidth(bool)));
  QObject::connect(m_checkHeight, SIGNAL(toggled(bool)), this, SLOT(togHeight(bool)));
  QObject::connect(m_checkAspect, SIGNAL(toggled(bool)), this, SLOT(togAspect(bool)));
  QObject::connect(m_checkDeletion, SIGNAL(toggled(bool)), this, SLOT(togDelete(bool)));
  QObject::connect(m_checkXPosition, SIGNAL(toggled(bool)), this, SLOT(togX(bool)));
  QObject::connect(m_checkYPosition, SIGNAL(toggled(bool)), this, SLOT(togY(bool)));
}
