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
#include <KoPADocument.h>
#include <KoShapeManager.h>
#include "KoPAView.h"
#include "KPrPage.h"
#include "KPrPageApplicationData.h"
#include "KPrViewModePreviewPageEffect.h"
#include "pageeffects/KPrPageEffectRegistry.h"
#include "pageeffects/KPrPageEffectFactory.h"
#include "commands/KPrPageEffectSetCommand.h"

bool orderFactoryByName( const KPrPageEffectFactory * factory1, const KPrPageEffectFactory * factory2 )
{
    return factory1->name() < factory2->name();
}

KPrPageEffectDocker::KPrPageEffectDocker( QWidget* parent, Qt::WindowFlags flags )
: QWidget( parent, flags )
, m_view( 0 )
, m_previewMode(0)
{
    setObjectName( "KPrPageEffectDocker" );
    QGridLayout* optionLayout = new QGridLayout();
    m_effectCombo = new QComboBox( this );
    m_effectCombo->addItem( i18n( "No Effect" ), QString( "" ) );

    QList<KPrPageEffectFactory*> factories = KPrPageEffectRegistry::instance()->values();

    qSort( factories.begin(), factories.end(), orderFactoryByName );

    foreach ( KPrPageEffectFactory * factory, factories )
    {
        m_effectCombo->addItem( factory->name(), factory->id() );
    }
    optionLayout->addWidget(m_effectCombo, 0, 0);

    connect( m_effectCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotEffectChanged( int ) ) );

    m_subTypeCombo = new QComboBox( this );

    connect( m_subTypeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotSubTypeChanged( int ) ) );

    m_durationSpinBox = new QDoubleSpinBox( this );
    m_durationSpinBox->setRange( 0.1, 60);
    m_durationSpinBox->setDecimals( 1 );
    m_durationSpinBox->setSuffix( i18n(" sec") );
    m_durationSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_durationSpinBox->setAlignment( Qt::AlignRight );
    m_durationSpinBox->setSingleStep( 0.1 );
    m_durationSpinBox->setValue( 2.0 );
    optionLayout->addWidget(m_durationSpinBox, 0, 1);

    connect( m_durationSpinBox, SIGNAL( valueChanged( double ) ),
             this, SLOT( slotDurationChanged( double ) ) );

    // setup widget layout
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addLayout( optionLayout);
    layout->addWidget( m_subTypeCombo );
    setLayout( layout );
}

void KPrPageEffectDocker::updateSubTypes( const KPrPageEffectFactory * factory )
{
    m_subTypeCombo->clear();
    if ( factory ) {
        m_subTypeCombo->setEnabled( true );

        const QMap<QString, int> subTypesByName( factory->subTypesByName() );
        QMap<QString, int>::ConstIterator it( subTypesByName.constBegin() );
        for ( ;it != subTypesByName.constEnd(); ++it ) {
            m_subTypeCombo->addItem( it.key(), it.value() );
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
}

void KPrPageEffectDocker::slotSubTypeChanged( int index )
{
    QString effectId = m_effectCombo->itemData( m_effectCombo->currentIndex() ).toString();
    const KPrPageEffectFactory * factory = KPrPageEffectRegistry::instance()->value( effectId );
    KPrPageEffect * pageEffect( createPageEffect( factory, m_subTypeCombo->itemData( index ).toInt(), m_durationSpinBox->value() ) );

    m_view->kopaCanvas()->addCommand( new KPrPageEffectSetCommand( m_view->activePage(), pageEffect ) );

    setEffectPreview();
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

void KPrPageEffectDocker::setView( KoPAViewBase* view )
{
    Q_ASSERT( view );
    m_view = view;
    connect( view->proxyObject, SIGNAL( activePageChanged() ),
             this, SLOT( slotActivePageChanged() ) );
    connect( view->proxyObject, SIGNAL( destroyed( QObject* ) ),
             this, SLOT( cleanup ( QObject* ) ) );


    if( m_view->activePage() )
        slotActivePageChanged();
}

void KPrPageEffectDocker::setEffectPreview()
{
    QString effectId = m_effectCombo->itemData( m_effectCombo->currentIndex() ).toString();
    const KPrPageEffectFactory * factory = KPrPageEffectRegistry::instance()->value( effectId );
    if(factory){
        KPrPageEffect * pageEffect( createPageEffect( factory, m_subTypeCombo->itemData( m_subTypeCombo->currentIndex() ).toInt(), m_durationSpinBox->value() ) );

        KPrPage* page = static_cast<KPrPage*>(m_view->activePage());
        KPrPage* oldpage = static_cast<KPrPage*>(m_view->kopaDocument()->pageByNavigation(page, KoPageApp::PagePrevious));

        if(!m_previewMode)
            m_previewMode = new KPrViewModePreviewPageEffect(m_view, m_view->kopaCanvas());

        m_previewMode->setPageEffect(pageEffect, page, oldpage); // also stops old if not already stopped
        m_view->setViewMode(m_previewMode); // play the effect (it reverts to normal  when done)
    }
}

void KPrPageEffectDocker::cleanup( QObject* object )
{
    if(object != m_view->proxyObject)
        return;

    m_view = 0;
}

#include "KPrPageEffectDocker.moc"
