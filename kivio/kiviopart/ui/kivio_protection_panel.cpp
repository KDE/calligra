#include "kivio_protection_panel.h"

#include "kivio_doc.h"
#include "kivio_layer.h"
#include "kivio_page.h"
#include "kivio_stencil.h"
#include "kivio_view.h"

#include <qbitarray.h>
#include <qcheckbox.h>
#include <kaction.h>
#include <kdebug.h>

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
   KivioStencil *pStencil;

   pStencil = m_pView->activePage()->selectedStencils()->first();
   while( pStencil )
   {
      if( pStencil->canProtect()->at(kpWidth)==true )
      {
	 pStencil->protection()->setBit( kpWidth, on );
      }
      else
      {
	 kdDebug() << "Attempt to set width-protection of a stencil which does not support it.\n";
      }

      pStencil = m_pView->activePage()->selectedStencils()->next();
   }
}

void KivioProtectionPanel::togHeight(bool on)
{
   KivioStencil *pStencil;

   pStencil = m_pView->activePage()->selectedStencils()->first();
   while( pStencil )
   {
      if( pStencil->canProtect()->at(kpHeight)==true )
      {
	 pStencil->protection()->setBit( kpHeight, on );
      }
      else
      {
	 kdDebug() << "Attempt to set height-protection of a stencil which does not support it.\n";
      }

      pStencil = m_pView->activePage()->selectedStencils()->next();
   }
}

void KivioProtectionPanel::togAspect(bool on)
{
   KivioStencil *pStencil;

   pStencil = m_pView->activePage()->selectedStencils()->first();
   while( pStencil )
   {
      if( pStencil->canProtect()->at(kpAspect)==true )
      {
	 pStencil->protection()->setBit( kpAspect, on );
      }
      else
      {
	 kdDebug() << "Attempt to set height-protection of a stencil which does not support it.\n";
      }

      pStencil = m_pView->activePage()->selectedStencils()->next();
   }
}

void KivioProtectionPanel::togDelete(bool on)
{
   KivioStencil *pStencil;

   pStencil = m_pView->activePage()->selectedStencils()->first();
   while( pStencil )
   {
      if( pStencil->canProtect()->at(kpDeletion)==true )
      {
	 pStencil->protection()->setBit( kpDeletion, on );
      }
      else
      {
	 kdDebug() << "Attempt to set delete-protection of a stencil which does not support it.\n";
      }

      pStencil = m_pView->activePage()->selectedStencils()->next();
   }
}

void KivioProtectionPanel::togX(bool on)
{
   KivioStencil *pStencil;

   pStencil = m_pView->activePage()->selectedStencils()->first();
   while( pStencil )
   {
      if( pStencil->canProtect()->at(kpX)==true )
      {
	 pStencil->protection()->setBit( kpX, on );
      }
      else
      {
	 kdDebug() << "Attempt to set X-protection of a stencil which does not support it.\n";
      }

      pStencil = m_pView->activePage()->selectedStencils()->next();
   }
}


void KivioProtectionPanel::togY(bool on)
{
   KivioStencil *pStencil;

   pStencil = m_pView->activePage()->selectedStencils()->first();
   while( pStencil )
   {
      if( pStencil->canProtect()->at(kpY)==true )
      {
	 pStencil->protection()->setBit( kpY, on );
      }
      else
      {
	 kdDebug() << "Attempt to set Y-protection of a stencil which does not support it.\n";
      }

      pStencil = m_pView->activePage()->selectedStencils()->next();
   }
}

void KivioProtectionPanel::updateCheckBoxes()
{
   KivioStencil *pStencil;
   QBitArray bits( NUM_PROTECTIONS );
   int i;

   // If there are no selected stencils, then disable all checkboxes,
   // and uncheck them
   if( m_pView->activePage()->selectedStencils()->count() == 0 )
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


   // Assume all protections are valid by default
   for( i=0; i<NUM_PROTECTIONS; i++ )
   {
      bits.setBit( i );
   }


   pStencil = m_pView->activePage()->selectedStencils()->first();
   while( pStencil )
   {
      // Now build an array representing which protection fields are valid
      // since there could be multiple types of stencils selected with varying
      // protection abilities
      for( i=0; i<NUM_PROTECTIONS; i++ )
      {
	 if( pStencil->canProtect()->at(i)==false )
	 {
	    bits.clearBit(i);
	 }
      }

      pStencil = m_pView->activePage()->selectedStencils()->next();
   }

   QObject::disconnect( m_checkWidth, SIGNAL(toggled(bool)), this, SLOT(togWidth(bool)) );
   QObject::disconnect( m_checkHeight, SIGNAL(toggled(bool)), this, SLOT(togHeight(bool)) );
   QObject::disconnect( m_checkAspect, SIGNAL(toggled(bool)), this, SLOT(togAspect(bool)) );
   QObject::disconnect( m_checkDeletion, SIGNAL(toggled(bool)), this, SLOT(togDelete(bool)) );
   QObject::disconnect( m_checkXPosition, SIGNAL(toggled(bool)), this, SLOT(togX(bool)) );
   QObject::disconnect( m_checkYPosition, SIGNAL(toggled(bool)), this, SLOT(togY(bool)) );

   // Enable and disable the check boxes based on what protections are available
   for( i=0; i<NUM_PROTECTIONS; i++ )
   {
      bool on = bits.at(i);

      switch( i )
      {
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
   if( m_pView->activePage()->selectedStencils()->count() > 1 )
   {
      m_checkAspect->setChecked(false);
      m_checkXPosition->setChecked(false);
      m_checkYPosition->setChecked(false);
      m_checkDeletion->setChecked(false);
      m_checkHeight->setChecked(false);
      m_checkWidth->setChecked(false);
   }
   // Otherwise fill in the checkboxes with the single selection
   else
   {
      pStencil = m_pView->activePage()->selectedStencils()->first();

      m_checkAspect->setChecked( pStencil->protection()->at(kpAspect) );
      m_checkDeletion->setChecked( pStencil->protection()->at(kpDeletion) );
      m_checkXPosition->setChecked( pStencil->protection()->at(kpX) );
      m_checkYPosition->setChecked( pStencil->protection()->at(kpY) );
      m_checkWidth->setChecked( pStencil->protection()->at(kpWidth) );
      m_checkHeight->setChecked( pStencil->protection()->at(kpHeight) );
   }

   QObject::connect( m_checkWidth, SIGNAL(toggled(bool)), this, SLOT(togWidth(bool)) );
   QObject::connect( m_checkHeight, SIGNAL(toggled(bool)), this, SLOT(togHeight(bool)) );
   QObject::connect( m_checkAspect, SIGNAL(toggled(bool)), this, SLOT(togAspect(bool)) );
   QObject::connect( m_checkDeletion, SIGNAL(toggled(bool)), this, SLOT(togDelete(bool)) );
   QObject::connect( m_checkXPosition, SIGNAL(toggled(bool)), this, SLOT(togX(bool)) );
   QObject::connect( m_checkYPosition, SIGNAL(toggled(bool)), this, SLOT(togY(bool)) );

}
