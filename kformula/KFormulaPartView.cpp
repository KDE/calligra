/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
                 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "KFormulaPartView.h"
#include "KFormulaPartDocument.h"
#include "KFormulaPartViewAdaptor.h"
#include "KFormulaPartFactory.h"
#include "KFormulaConfigDialog.h"
#include "KFormulaCanvas.h"
#include <KoCanvasControllerWidget.h>
#include <KoZoomHandler.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktip.h>
#include <klocale.h>
#include <kicon.h>
#include <kglobal.h>
#include <kstandarddirs.h>

KFormulaPartView::KFormulaPartView( KFormulaPartDocument* doc, QWidget* parent )
        : KoView( doc, parent )
{
    m_partDocument = doc;

    m_dbus = new KFormulaPartViewAdaptor( this );
    QDBusConnection::sessionBus().registerObject( '/' + objectName(), this );

    setComponentData( KFormulaPartFactory::global() );

    m_formulaCanvas = new KFormulaCanvas( this, m_partDocument );
    m_zoomHandler = new KoZoomHandler();
    m_canvasController = new KoCanvasControllerWidget( this );
    m_canvasController->setCanvas( m_formulaCanvas );

    if ( !doc->isReadWrite() )
        setXMLFile("kformula_readonly.rc");
    else
        setXMLFile("kformula.rc");

    setupActions();

    KStandardAction::tipOfDay( this, SLOT( slotShowTip() ), actionCollection() );
}

KFormulaPartView::~KFormulaPartView()
{
    delete m_formulaCanvas;
    delete m_zoomHandler;
    delete m_canvasController;
    delete m_dbus;
}

KFormulaPartViewAdaptor* KFormulaPartView::dbusObject()
{
    return m_dbus;
}

void KFormulaPartView::setupActions()
{
/*    KGlobal::dirs()->addResourceType( "toolbar", KStandardDirs::kde_default("data") +
                                      "kformula/pics/" );

    m_cutAction;
    m_copyAction;
    m_pasteAction;

    m_addBracketAction = new KAction( KIcon("paren"), i18n("Add Bracket"),
                                      actionCollection(), "addbracket" );
//    connect();
//    m_addBracketAction->set... ;
    m_addFractionAction = new KAction( KIcon("frac"), i18n("Add Fraction"),
                                       actionCollection(), "addfraction" );
    m_addRootAction = new KAction( KIcon("sqrt"), i18n("Add Root"),
                                   actionCollection(), "addroot" );
    m_addSumAction = new KAction( KIcon("sum"), i18n("Add Sum"),
                                  actionCollection(), "addsum" );
    m_addProductAction = new KAction( KIcon("prod"), i18n("Add Product"),
                                      actionCollection(), "addproduct" );
    m_addIntegralAction = new KAction( KIcon("int"), i18n("Add Integral"),
                                       actionCollection(), "addintegral" );
    m_addMatrixAction = new KAction( KIcon("matrix"), i18n("Add Matrix"),
                                     actionCollection(), "addmatrix" );
    m_addUpperLeftAction;
    m_addLowerLeftAction;
    m_addUpperRightAction;
    m_addLowerRightAction;
    m_addGenericUpperAction;
    m_addGenericLowerAction;
    m_removeEnclosingAction;

    KStandardAction::preferences( this, SLOT(configure()), actionCollection(), "configure" );*/
}

void KFormulaPartView::focusInEvent( QFocusEvent* )
{
//    m_formulaCanvas->setFocus();
}

void KFormulaPartView::slotShowTipOnStart()
{
    KTipDialog::showTip( this );
}

void KFormulaPartView::slotShowTip()
{
    KTipDialog::showTip( this, "", true );
}

void KFormulaPartView::setEnabled( bool enabled )
{
    Q_UNUSED( enabled );
/*    m_addBracketAction->setEnabled( enabled );
    m_addFractionAction->setEnabled( enabled );
    m_addRootAction->setEnabled( enabled );
    m_addSumAction->setEnabled( enabled );
    m_addIntegralAction->setEnabled( enabled);
    m_addMatrixAction->setEnabled( enabled);
    m_addUpperLeftAction->setEnabled( enabled );
    m_addLowerLeftAction->setEnabled( enabled );
    m_addUpperRightAction->setEnabled( enabled );
    m_addLowerRightAction->setEnabled( enabled );
    m_addGenericUpperAction->setEnabled( enabled );
    m_addGenericLowerAction->setEnabled( enabled );
    m_removeEnclosingAction->setEnabled( enabled );*/
}

void KFormulaPartView::resizeEvent( QResizeEvent * )
{
    static_cast<KoCanvasControllerWidget*>(m_canvasController)->setGeometry( 0, 0, width(), height() );
}

void KFormulaPartView::cursorChanged( bool visible, bool selecting )
{
    m_cutAction->setEnabled( visible && selecting );
    m_copyAction->setEnabled( visible && selecting );
    m_removeEnclosingAction->setEnabled( !selecting );
}

void KFormulaPartView::sizeSelected( int size )
{
    Q_UNUSED( size );
//    document()->getFormula()->setFontSize( size );
//    m_formulaCanvas->setFocus();
}

void KFormulaPartView::configure()
{
    KFormulaConfigDialog configDialog( this );
    configDialog.exec();
}

void KFormulaPartView::updateReadWrite( bool )
{
}

KoViewConverter* KFormulaPartView::viewConverter()
{
    return m_zoomHandler;
}

#include "KFormulaPartView.moc"
