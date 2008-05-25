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
#include <QDoubleSpinBox>
#include <QLabel>
#include <QEvent>
#include <QPainter>
#include <QTimer>
#include <klocale.h>

#include <KoPACanvas.h>
#include <KoShapeManager.h>
#include "KPrView.h"
#include "KPrPage.h"
#include "KPrPageApplicationData.h"
#include "KPrPreviewWidget.h"
#include "pageeffects/KPrPageEffectRegistry.h"
#include "pageeffects/KPrPageEffectFactory.h"
#include "commands/KPrPageEffectSetCommand.h"

KPrPageEffectDocker::KPrPageEffectDocker( QWidget* parent, Qt::WindowFlags flags )
: QDockWidget( parent, flags )
, m_view( 0 )
{
    setWindowTitle( i18n( "Page effects" ) );

    QWidget* base = new QWidget( this );

    // setup the effect preview
    m_preview = new KPrPreviewWidget( base );
    m_preview->setToolTip( i18n( "Click to preview the page effect." ) );
   // m_preview->installEventFilter( this );
   // m_preview->setFrameShape( QFrame::Box );

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

    QGridLayout* optionLayout = new QGridLayout();
    QLabel* durationLabel = new QLabel( i18n("Duration: "), this);
    m_durationSpinBox = new QDoubleSpinBox( this );
    m_durationSpinBox->setRange( 0.1, 600);
    m_durationSpinBox->setDecimals( 1 );
    m_durationSpinBox->setSuffix( i18n(" seconds") );
    m_durationSpinBox->setAlignment( Qt::AlignRight );
    m_durationSpinBox->setSingleStep( 0.1 );
    m_durationSpinBox->setValue( 2.0 );
    optionLayout->addWidget(durationLabel, 0, 0);
    optionLayout->addWidget(m_durationSpinBox, 0, 1);

    connect( m_durationSpinBox, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotDurationChanged( double ) ) );

    // setup widget layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( m_effectCombo );
    layout->addWidget( m_subTypeCombo );
    layout->addLayout( optionLayout);
    layout->addWidget( m_preview);
    base->setLayout( layout );
    setWidget( base );

    m_updateTimer = new QTimer( this );
    m_updateTimer->setInterval( 100 );
    m_updateTimer->setSingleShot( true );
    connect( m_updateTimer, SIGNAL( timeout() ),
             this, SLOT( setEffectPreview() ) );
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
        foreach( int subType, factory->subTypes() ) {
            QString subTypeString = factory->subTypeName( subType );
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

        double duration = pageEffect ? static_cast<double>(pageEffect->duration())/1000 : 2.0;
        // block the signal so we don't get a new page effect when the page is changed
        m_durationSpinBox->blockSignals( true );
        m_durationSpinBox->setValue( duration );
        m_durationSpinBox->blockSignals( false );

        setEffectPreview();
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
        pageEffect = createPageEffect( factory, m_subTypeCombo->itemData( m_subTypeCombo->currentIndex() ).toInt(), m_durationSpinBox->value());
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

    setEffectPreview();
    m_preview->runPreview();
}

void KPrPageEffectDocker::slotSubTypeChanged( int index )
{
    QString effectId = m_effectCombo->itemData( m_effectCombo->currentIndex() ).toString();
    const KPrPageEffectFactory * factory = KPrPageEffectRegistry::instance()->value( effectId );
    KPrPageEffect * pageEffect( createPageEffect( factory, m_subTypeCombo->itemData( index ).toInt(), m_durationSpinBox->value() ) );

    m_view->kopaCanvas()->addCommand( new KPrPageEffectSetCommand( m_view->activePage(), pageEffect ) );

    setEffectPreview();
    m_preview->runPreview();
}

void KPrPageEffectDocker::slotDurationChanged( double duration )
{
    QString effectId = m_effectCombo->itemData( m_effectCombo->currentIndex() ).toString();
    const KPrPageEffectFactory * factory = KPrPageEffectRegistry::instance()->value( effectId );

    if(factory) {
        KPrPageEffect * pageEffect( createPageEffect( factory, m_subTypeCombo->itemData( m_subTypeCombo->currentIndex() ).toInt(), duration ) );

        m_view->kopaCanvas()->addCommand( new KPrPageEffectSetCommand( m_view->activePage(), pageEffect ) );
    }
}

KPrPageEffect * KPrPageEffectDocker::createPageEffect( const KPrPageEffectFactory * factory, int subType, double duration )
{
    Q_ASSERT( factory );
    // TODO get data from input
    KPrPageEffectFactory::Properties properties( qRound(duration*1000), subType );
    return factory->createPageEffect( properties );
}

void KPrPageEffectDocker::setView( KPrView* view )
{
    Q_ASSERT( view );
    m_view = view;
    connect( view, SIGNAL( activePageChanged() ),
             this, SLOT( slotActivePageChanged() ) );
    connect( view, SIGNAL( destroyed( QObject* ) ),
             this, SLOT( cleanup ( QObject* ) ) );
    connect( view->kopaCanvas(), SIGNAL( canvasUpdated() ),
             m_updateTimer, SLOT( start() ) );

    if( m_view->activePage() )
        slotActivePageChanged();
}

void KPrPageEffectDocker::setEffectPreview()
{
    QString effectId = m_effectCombo->itemData( m_effectCombo->currentIndex() ).toString();
    const KPrPageEffectFactory * factory = KPrPageEffectRegistry::instance()->value( effectId );

    if(factory){
        KPrPageEffect * pageEffect( createPageEffect( factory, m_subTypeCombo->itemData( m_subTypeCombo->currentIndex() ).toInt(), m_durationSpinBox->value() ) );

        m_preview->setPageEffect(pageEffect,  static_cast<KPrPage*>(m_view->activePage()));
    }
    else
        m_preview->setPageEffect(0,  static_cast<KPrPage*>(m_view->activePage()));
}

void KPrPageEffectDocker::cleanup( QObject* object )
{
    if(object != m_view)
        return;

    m_view = 0;
    m_preview->setPageEffect( 0, 0 );
}

#include "KPrPageEffectDocker.moc"
