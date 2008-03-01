/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QEvent>
#include <QPainter>
#include <klocale.h>

#include <KoPACanvas.h>
#include "KPrView.h"
#include "KPrPage.h"
#include "KPrPageApplicationData.h"
#include "pageeffects/KPrPageEffectRegistry.h"
#include "pageeffects/KPrPageEffectFactory.h"
#include "commands/KPrPageEffectSetCommand.h"

static const char* s_subTypes[] = {
    I18N_NOOP( "From Left" ),
    I18N_NOOP( "From Right" ),
    I18N_NOOP( "From Top" ),
    I18N_NOOP( "From Bottom" ),
    I18N_NOOP( "To Left" ),
    I18N_NOOP( "To Right" ),
    I18N_NOOP( "To Top" ),
    I18N_NOOP( "To Bottom" ),
    I18N_NOOP( "From Top Left" ),
    I18N_NOOP( "From Top Right" ),
    I18N_NOOP( "From Bottom Left" ),
    I18N_NOOP( "From Bottom Right" ),
    I18N_NOOP( "From Twelve Clockwise" ),
    I18N_NOOP( "From Three Clockwise" ),
    I18N_NOOP( "From Six Clockwise" ),
    I18N_NOOP( "From Nine Clockwise" ),
    I18N_NOOP( "From Twelve Counterclockwise" ),
    I18N_NOOP( "From Three Counterclockwise" ),
    I18N_NOOP( "From Six Counterclockwise" ),
    I18N_NOOP( "From Nine Counterclockwise" ),
    I18N_NOOP( "Double Barn Door" ),
    I18N_NOOP( "Double Diamond" ),
    I18N_NOOP( "Corners In" ),
    I18N_NOOP( "Corners In Reverse" ),
    I18N_NOOP( "Corners Out" ),
    I18N_NOOP( "Corners Out Reverse" ),
    I18N_NOOP( "Two Blades Vertical Clockwise" ),
    I18N_NOOP( "Two Blades Horizontal Clockwise" ),
    I18N_NOOP( "Four Blades Horizontal Clockwise" ),
    I18N_NOOP( "Two Blades Vertical Counterclockwise" ),
    I18N_NOOP( "Two Blades Horizontal Counterclockwise" ),
    I18N_NOOP( "Four Blades Horizontal Counterclockwise" ),
    I18N_NOOP( "Clockwise Top" ),
    I18N_NOOP( "Clockwise Right" ),
    I18N_NOOP( "Clockwise Bottom" ),
    I18N_NOOP( "Clockwise Left" ),
    I18N_NOOP( "Clockwise Top Left" ),
    I18N_NOOP( "Counterclockwise Bottom Left" ),
    I18N_NOOP( "Clockwise Bottom Right" ),
    I18N_NOOP( "Counterclockwise Top Right" ),
    I18N_NOOP( "Counterclockwise Top" ),
    I18N_NOOP( "Counterclockwise Right" ),
    I18N_NOOP( "Counterclockwise Bottom" ),
    I18N_NOOP( "Counterclockwise Left" ),
    I18N_NOOP( "Counterclockwise Top Left" ),
    I18N_NOOP( "Clockwise Bottom Left" ),
    I18N_NOOP( "Counterclockwise Bottom Right" ),
    I18N_NOOP( "Clockwise Top Right" ),
    I18N_NOOP( "Center Right" ),
    I18N_NOOP( "Center Top" ),
    I18N_NOOP( "Center Left" ),
    I18N_NOOP( "Center Bottom" ),
    I18N_NOOP( "Fan Out Vertical" ),
    I18N_NOOP( "Fan Out Horizontal" ),
    I18N_NOOP( "Vertical" ),
    I18N_NOOP( "Vertical Reverse" ),
    I18N_NOOP( "Horizontal" ),
    I18N_NOOP( "Horizontal Reverse" ),
    I18N_NOOP( "Diagonal Bottom Left" ),
    I18N_NOOP( "Diagonal Bottom Left Reverse" ),
    I18N_NOOP( "Diagonal Top Left" ),
    I18N_NOOP( "Diagonal Top Left Reverse" ),
    I18N_NOOP( "Top Left Vertical" ),
    I18N_NOOP( "Top Left Horizontal" ),
    I18N_NOOP( "Top Right Vertical" ),
    I18N_NOOP( "Top Right Horizontal" ),
    I18N_NOOP( "Bottom Left Vertical" ),
    I18N_NOOP( "Bottom Left Horizontal" ),
    I18N_NOOP( "Bottom Right Vertical" ),
    I18N_NOOP( "Bottom Right Horizontal" ),
    I18N_NOOP( "Clockwise Top Left In" ),
    I18N_NOOP( "Clockwise Top Left Out" ),
    I18N_NOOP( "Clockwise Top Right In" ),
    I18N_NOOP( "Clockwise Top Right Out" ),
    I18N_NOOP( "Clockwise Bottom Left In" ),
    I18N_NOOP( "Clockwise Bottom Left Out" ),
    I18N_NOOP( "Clockwise Bottom Right In" ),
    I18N_NOOP( "Clockwise Bottom Right Out" ),
    I18N_NOOP( "Counterclockwise Top Left In" ),
    I18N_NOOP( "Counterclockwise Top Left Out" ),
    I18N_NOOP( "Counterclockwise Top Right In" ),
    I18N_NOOP( "Counterclockwise Top Right Out" ),
    I18N_NOOP( "Counterclockwise Bottom Left In" ),
    I18N_NOOP( "Counterclockwise Bottom Left Out" ),
    I18N_NOOP( "Counterclockwise Bottom Right In" ),
    I18N_NOOP( "Counterclockwise Bottom Right Out" ),
    I18N_NOOP( "Vertical Top Same In" ),
    I18N_NOOP( "Vertical Top Same Out" ),
    I18N_NOOP( "Vertical Bottom Same In" ),
    I18N_NOOP( "Vertical Bottom Same Out" ),
    I18N_NOOP( "Vertical Top Left Opposite In" ),
    I18N_NOOP( "Vertical Top Left Opposite Out" ),
    I18N_NOOP( "Vertical Bottom Left Opposite In" ),
    I18N_NOOP( "Vertical Bottom Left Opposite Out" ),
    I18N_NOOP( "Horizontal Left Same In" ),
    I18N_NOOP( "Horizontal Left Same Out" ),
    I18N_NOOP( "Horizontal Right Same In" ),
    I18N_NOOP( "Horizontal Right Same Out" ),
    I18N_NOOP( "Horizontal Top Left Opposite In" ),
    I18N_NOOP( "Horizontal Top Left Opposite Out" ),
    I18N_NOOP( "Horizontal Top Right Opposite In" ),
    I18N_NOOP( "Horizontal Top Right Opposite Out" ),
    I18N_NOOP( "Diagonal Bottom Left Opposite In" ),
    I18N_NOOP( "Diagonal Bottom Left Opposite Out" ),
    I18N_NOOP( "Diagonal Top Left Opposite In" ),
    I18N_NOOP( "Diagonal Top Left Opposite Out" ),
    I18N_NOOP( "Two Box Top In" ),
    I18N_NOOP( "Two Box Bottom In" ),
    I18N_NOOP( "Two Box Left In" ),
    I18N_NOOP( "Two Box Right In" ),
    I18N_NOOP( "Four Box Vertical In" ),
    I18N_NOOP( "Four Box Horizontal In" ),
    I18N_NOOP( "Two Box Top Out" ),
    I18N_NOOP( "Two Box Bottom Out" ),
    I18N_NOOP( "Two Box Left Out" ),
    I18N_NOOP( "Two Box Right Out" ),
    I18N_NOOP( "Four Box Vertical Out" ),
    I18N_NOOP( "Four Box Horizontal Out" ),
    I18N_NOOP( "Rectangular" ),
    I18N_NOOP( "Diamond" ),
    I18N_NOOP( "Fan Out Top" ),
    I18N_NOOP( "Fan Out Right" ),
    I18N_NOOP( "Fan Out Bottom" ),
    I18N_NOOP( "Fan Out Left" ),
    I18N_NOOP( "Fan In Top" ),
    I18N_NOOP( "Fan In Right" ),
    I18N_NOOP( "Fan In Bottom" ),
    I18N_NOOP( "Fan In Left" ),
    I18N_NOOP( "Fan In Vertical" ),
    I18N_NOOP( "Fan In Horizontal" ),
    I18N_NOOP( "Fan In Vertical Reverse" ),
    I18N_NOOP( "Fan In Horizontal Reverse" ),
    I18N_NOOP( "Parallel Vertical In" ),
    I18N_NOOP( "Parallel Diagonal" ),
    I18N_NOOP( "Opposite Vertical" ),
    I18N_NOOP( "Opposite Horizontal" ),
    I18N_NOOP( "Parallel Diagonal Top Left" ),
    I18N_NOOP( "Parallel Diagonal Bottom Left" ),
    I18N_NOOP( "Parallel Vertical Reverse" ),
    I18N_NOOP( "Parallel Diagonal Reverse" ),
    I18N_NOOP( "OppositeVertical Reverse" ),
    I18N_NOOP( "OppositeHorizontal Reverse" ),
    I18N_NOOP( "Parallel Diagonal Top Left Reverse" ),
    I18N_NOOP( "Parallel Diagonal Bottom Left Reverse" ),
    I18N_NOOP( "Right" ),
    I18N_NOOP( "Up" ),
    I18N_NOOP( "Right Reverse" ),
    I18N_NOOP( "Up Reverse" ),
};

KPrPageEffectDocker::KPrPageEffectDocker( QWidget* parent, Qt::WindowFlags flags )
: QDockWidget( parent, flags )
, m_view( 0 )
{
    setWindowTitle( i18n( "Page effects" ) );

    QWidget* base = new QWidget( this );

    // setup the effect preview
    m_preview = new QLabel( base );
    m_preview->installEventFilter( this );
    m_preview->setFrameShape( QFrame::Box );

    m_effectCombo = new QComboBox( base );
    m_effectCombo->addItem( i18n( "No Effect" ), QString( "" ) );

    QList<KPrPageEffectFactory*> factories = KPrPageEffectRegistry::instance()->values();

    foreach ( KPrPageEffectFactory * factory, factories )
    {
        m_effectCombo->addItem( factory->name(), factory->id() );
    }

    connect( m_effectCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotEffectChanged( int ) ) );

    m_subTypeCombo = new QComboBox( base );

    connect( m_subTypeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotSubTypeChanged( int ) ) );

    // setup widget layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_effectCombo );
    layout->addWidget( m_subTypeCombo );
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

void KPrPageEffectDocker::updateSubTypes( const KPrPageEffectFactory * factory )
{
    m_subTypeCombo->clear();
    if ( factory ) {
        m_subTypeCombo->setEnabled( true );
        foreach( KPrPageEffect::SubType subType, factory->subTypes() ) {
            QString subTypeString = i18n( s_subTypes[subType] );
            m_subTypeCombo->addItem( subTypeString, subType );
        }
    }
    else {
        m_subTypeCombo->setEnabled( false );
    }
}

void KPrPageEffectDocker::slotActivePageChanged()
{
    if ( !m_view )
        return;

    // get the active page
    KPrPage * page = dynamic_cast<KPrPage*>( m_view->activePage() );
    if ( page ) {
        // set the combo box according to the page's effect
        this->setEnabled( true );

        KPrPageApplicationData * pageData = KPrPage::pageData( page );
        KPrPageEffect * pageEffect = pageData->pageEffect();
        QString effectId = pageEffect ? pageEffect->id() : "";

        for ( int i = 0; i < m_effectCombo->count(); ++i )
        {
            if ( m_effectCombo->itemData( i ).toString() == effectId ) {
                m_effectCombo->setCurrentIndex( i );
                break;
            }
        }

        const KPrPageEffectFactory * factory = pageEffect ? KPrPageEffectRegistry::instance()->value( effectId ) : 0;
        updateSubTypes( factory );

        for ( int i = 0; i < m_subTypeCombo->count(); ++i )
        {
            if ( m_subTypeCombo->itemData( i ).toInt() == pageEffect->subType() ) {
                m_subTypeCombo->setCurrentIndex( i );
                break;
            }
        }
/*
    QPainter p( m_activePageBuffer );

    KoViewConverter converter;
    m_view->kopaCanvas()->masterShapeManager()->paint( p, converter, false );
    m_view->kopaCanvas()->shapeManager()->paint( p, converter, false );
*/
    }
    else {
        // disable the page effect docker as effects are only there on a normal page
        this->setEnabled( false );
    }

}

void KPrPageEffectDocker::slotEffectChanged( int index )
{
    // provide a preview of the chosen page effect
    KPrPageEffect * pageEffect = 0;
    QString effectId = m_effectCombo->itemData( index ).toString();
    const KPrPageEffectFactory * factory = effectId != "" ? KPrPageEffectRegistry::instance()->value( effectId ) : 0;
    updateSubTypes( factory );
    if ( factory ) {
        pageEffect = createPageEffect( factory, m_subTypeCombo->itemData( m_subTypeCombo->currentIndex() ).toInt());
    }
    else {
        // this is to avoid the assert that checks if the effect is different then the last one
        // The problem is that a undo is not yet reflected in the UI so it is possible to get the
        // same effect twice.
        // TODO
        KPrPageApplicationData * pageData = KPrPage::pageData( m_view->activePage() );
        if ( pageData->pageEffect() == 0 ) {
            return;
        }
    }

    m_view->kopaCanvas()->addCommand( new KPrPageEffectSetCommand( m_view->activePage(), pageEffect ) );

    // start hard coded
//    KPrSlideWipeEffect effect;
//    effect->setup( m_preview );
}

void KPrPageEffectDocker::slotSubTypeChanged( int index )
{
    QString effectId = m_effectCombo->itemData( m_effectCombo->currentIndex() ).toString();
    const KPrPageEffectFactory * factory = KPrPageEffectRegistry::instance()->value( effectId );
    KPrPageEffect * pageEffect( createPageEffect( factory, m_subTypeCombo->itemData( index ).toInt() ) );

    m_view->kopaCanvas()->addCommand( new KPrPageEffectSetCommand( m_view->activePage(), pageEffect ) );
}

KPrPageEffect * KPrPageEffectDocker::createPageEffect( const KPrPageEffectFactory * factory, int subType )
{
    Q_ASSERT( factory );
    // TODO get data from input
    KPrPageEffectFactory::Properties properties( 5000, KPrPageEffect::SubType( subType ) );
    return factory->createPageEffect( properties );
}

void KPrPageEffectDocker::setView( KPrView* view )
{
    Q_ASSERT( view );
    m_view = view;
    connect( view, SIGNAL( activePageChanged() ),
             this, SLOT( slotActivePageChanged() ) );
}

#include "KPrPageEffectDocker.moc"
