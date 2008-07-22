
/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/
#include <QWidget>
#include <QDebug>
#include <QLabel>

#include <KDChartPosition.h>
#include <KDChartHeaderFooter.h>

#include <KDChartHeaderFooterPropertiesWidget.h>
#include <KDChartHeaderFooterPropertiesWidget_p.h>


#define d d_func()

using namespace KDChart;

HeaderFooterPropertiesWidget::Private::Private()
    :headerfooter(0), instantApply(true)
{

}

HeaderFooterPropertiesWidget::Private::~Private()
{
}

HeaderFooterPropertiesWidget::HeaderFooterPropertiesWidget( QWidget *parent )
    :QWidget( parent ), _d( new Private )
{
    setupUi( this );

    const QStringList labels = KDChart::Position::printableNames();
    const QList<QByteArray> names = KDChart::Position::names();

    for ( int i = 0, end = qMin( labels.size(), names.size() ) ; i != end ; ++i )
        mPositionCombo->addItem( labels[i], names[i] );

    connect( mPositionCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotPositionChanged( int ) ) );
    connect( headerRB,  SIGNAL( toggled( bool ) ),
             this,  SLOT( slotTypeChanged( bool ) ) );
    connect( textED,  SIGNAL( textChanged( const QString ) ),
             this,  SLOT( slotTextChanged( const QString  ) ) );

    setEnabled( false );

}

HeaderFooterPropertiesWidget::~HeaderFooterPropertiesWidget()
{

}

void HeaderFooterPropertiesWidget::setHeaderFooter( HeaderFooter * hf )
{
     d->headerfooter = hf;
    if ( hf ) {
        readFromHeaderFooter( hf );
        setEnabled( true );
    } else {
        setEnabled( false);
    }
}

void HeaderFooterPropertiesWidget::setInstantApply( bool value )
{
    d->instantApply = value;
}


void HeaderFooterPropertiesWidget::readFromHeaderFooter( const HeaderFooter * hf  )
{
    mPositionCombo->setCurrentIndex( mPositionCombo->findData( QByteArray( hf->position().name() ) ) );
    textED->setText( hf->text() );
    if (  hf->type() == HeaderFooter::Header )
        headerRB->setChecked( true );
    else
        footerRB->setChecked( true );
}


void HeaderFooterPropertiesWidget::writeToHeaderFooter( HeaderFooter * hf )
{
    if ( !hf ) return;
    hf->setPosition( Position::fromName( mPositionCombo->itemData( mPositionCombo->currentIndex() ).toByteArray() ) );
    hf->setText( textED->text() );
    if (  headerRB->isChecked() )
        hf->setType( HeaderFooter::Header );
    else
        hf->setType( HeaderFooter::Footer );
}

void HeaderFooterPropertiesWidget::slotPositionChanged( int idx )
{
    if ( d->headerfooter && d->instantApply ) {
        d->headerfooter->setPosition( Position::fromName( mPositionCombo->itemData( idx ).toByteArray() ) );
    } else {
        emit changed();
    }
}


void HeaderFooterPropertiesWidget::slotTextChanged( const QString& text )
{
    Q_UNUSED( text );

    if ( d->headerfooter && d->instantApply ) {
        d->headerfooter->setText( textED->text() );
    } else {
        emit changed();
    }
}

void HeaderFooterPropertiesWidget::slotTypeChanged( bool toggled )
{
    Q_UNUSED( toggled );

    if ( d->headerfooter && d->instantApply ) {
        if (  headerRB->isChecked() )
            d->headerfooter->setType( HeaderFooter::Header );
        else
            d->headerfooter->setType( HeaderFooter::Footer );
    } else {
        emit changed();
    }
}

