/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "KPrPageEffectDocker.h"
#include <KPrView.h>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QEvent>
#include <QPainter>
#include <klocale.h>

// hard code
#include "pageeffects/KPrSlideWipeEffect.h"

KPrPageEffectDocker::KPrPageEffectDocker( QWidget* parent, Qt::WindowFlags flags )
                   : QDockWidget( parent, flags )
{
    m_view = 0;

    setWindowTitle( i18n( "Page effects" ) );

    QWidget* base = new QWidget( this );

    // setup the effect preview
    m_preview = new QLabel( base );
    m_preview->installEventFilter( this );
    m_preview->setFrameShape( QFrame::Box );

    // setup the effect chooser combo box
    m_effectCombo = new QComboBox( base );
    m_effectCombo->addItem( i18n( "No Effect" ) );
    m_effectCombo->addItem( i18n( "Cover down effect" ) );
    connect( m_effectCombo, SIGNAL( currectIndexChanged( int ) ),
             this, SLOT( slotEffectChanged( int ) ) );

    // setup widget layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_effectCombo );
    layout->addWidget( m_preview);
    base->setLayout( layout );
    setWidget( base );
}

bool KPrPageEffectDocker::eventFilter( QObject* object, QEvent* event )
{
    if( event->type() == QEvent::Paint ) {
        QPainter p( m_preview );
        p.drawLine( 0, 0, m_preview->width(), m_preview->height() );
        p.drawLine( 0, m_preview->height(), m_preview->width(), 0 );
        return true;
    }
    else
        return QObject::eventFilter( object, event );  // standard event processing
}

void KPrPageEffectDocker::slotActivePageChanged()
{
    if ( !m_view )
        return;

    // get the active page and set the combo box according to the page's effect

/*
    QPainter p( m_activePageBuffer );

    KoViewConverter converter;
    m_view->kopaCanvas()->masterShapeManager()->paint( p, converter, false );
    m_view->kopaCanvas()->shapeManager()->paint( p, converter, false ); 
*/}

void KPrPageEffectDocker::slotEffectChanged( int index )
{
    // provide a preview of the chosen page effect
    if ( index == 0 ) // don't do nothing when it is NoEffect
        return;

    // TODO: Use the registry to obtain the effect object

    // start hard coded
//    KPrSlideWipeEffect effect;
//    effect->setup( m_preview );
}

void KPrPageEffectDocker::setView( KPrView* view )
{
    m_view = view;
    connect( view, SIGNAL( activePageChanged() ),
             this, SLOT( slotActivePageChanged() ) );
}

#include "KPrPageEffectDocker.moc"
