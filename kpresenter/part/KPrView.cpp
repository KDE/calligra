/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrView.h"

#include <klocale.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>

#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPACanvas.h>

#include "KPrDocument.h"
#include "KPrPage.h"
#include "KPrMasterPage.h"
#include "KPrPageApplicationData.h"
#include "KPrViewModePresentation.h"
#include "commands/KPrAnimationCreateCommand.h"
#include "commands/KPrPageEffectSetCommand.h"
#include "pageeffects/KPrSlideWipeEffect.h"
#include "dockers/KPrPageEffectDocker.h"
#include "dockers/KPrPageEffectDockerFactory.h"
#include "shapeanimations/KPrAnimationMoveAppear.h"

KPrView::KPrView( KPrDocument *document, QWidget *parent )
: KoPAView( document, parent )
, m_presentationMode( new KPrViewModePresentation( this, m_canvas ))
, m_normalMode( 0 )
{
    initGUI();
    initActions();
}

KPrView::~KPrView()
{
    delete m_presentationMode;
}

KoViewConverter * KPrView::viewConverter()
{
    KPrViewModePresentation * mode = dynamic_cast<KPrViewModePresentation *>( viewMode() );

    return mode ? mode->viewConverter() : KoPAView::viewConverter();
}

void KPrView::initGUI()
{ 
    // add page effect docker to the main window
    KPrPageEffectDockerFactory factory;
    KPrPageEffectDocker* docker;
    docker = qobject_cast<KPrPageEffectDocker*>( createDockWidget( &factory ) );
    docker->setView( this );
}

void KPrView::initActions()
{
    if ( !m_doc->isReadWrite() )
       setXMLFile( "kpresenter_readonly.rc" );
    else
       setXMLFile( "kpresenter.rc" );

    // do special kpresenter stuff here
    m_actionStartPresentation = new KAction( i18n( "Start Presentation" ), this );
    actionCollection()->addAction( "view_mode", m_actionStartPresentation );
    connect( m_actionStartPresentation, SIGNAL( activated() ), this, SLOT( startPresentation() ) );

    m_actionCreateAnimation = new KAction( "Create Appear Animation", this );
    actionCollection()->addAction( "edit_createanimation", m_actionCreateAnimation );
    connect( m_actionCreateAnimation, SIGNAL( activated() ), this, SLOT( createAnimation() ) );

    m_actionCreatePageEffect = new KAction( "Create Page Effect", this );
    actionCollection()->addAction( "edit_createpageeffect", m_actionCreatePageEffect );
    connect( m_actionCreatePageEffect, SIGNAL( activated() ), this, SLOT( createPageEffect() ) );

    m_actionDeletePageEffect = new KAction( "Delete Page Effect", this );
    actionCollection()->addAction( "edit_deletepageeffect", m_actionDeletePageEffect );
    connect( m_actionDeletePageEffect, SIGNAL( activated() ), this, SLOT( deletePageEffect() ) );
}

void KPrView::startPresentation()
{
    setViewMode( m_presentationMode );
}

void KPrView::createAnimation()
{
    static int animationcount = 0;
    KoSelection * selection = m_canvas->shapeManager()->selection();
    QList<KoShape*> selectedShapes = selection->selectedShapes();
    foreach( KoShape * shape, selectedShapes )
    {
        KPrShapeAnimation * animation = new KPrAnimationMoveAppear( shape, animationcount );
        KPrDocument * doc = dynamic_cast<KPrDocument *>( m_doc );
        Q_ASSERT( doc );
        KPrAnimationCreateCommand * command = new KPrAnimationCreateCommand( doc, animation );
        m_canvas->addCommand( command );
    }
    animationcount = ( animationcount + 1 ) % 3;
}

void KPrView::createPageEffect()
{
    // this does not work in master pages
    if ( dynamic_cast<KPrPage *>( activePage() ) ) {
        KPrPageEffectSetCommand * command = new KPrPageEffectSetCommand( activePage(), new KPrSlideWipeEffect( 5000 ) );
        m_canvas->addCommand( command );
    }
}

void KPrView::deletePageEffect()
{
    // this does not work in master pages
    if ( dynamic_cast<KPrPage *>( activePage() ) && KPrPage::pageData( activePage() )->pageEffect() ) {
        KPrPageEffectSetCommand * command = new KPrPageEffectSetCommand( activePage(), 0 );
        m_canvas->addCommand( command );
    }
}

#include "KPrView.moc"
