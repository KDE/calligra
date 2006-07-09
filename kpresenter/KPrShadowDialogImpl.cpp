// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>

#include <kcolorbutton.h>
#include <klocale.h>

#include "KPrShadowDialogImpl.h"
#include "KPrTextPreview.h"

KPrShadowDialogImpl::KPrShadowDialogImpl( QWidget *parent, const char* name )
    : ShadowDialogBase( parent, name )
{
    _preview = new KPrTextPreview( previewPanel );
    QHBoxLayout *lay = new QHBoxLayout( previewPanel, previewPanel->lineWidth(), 0 );
    lay->addWidget( _preview );
    distanceSpinBox->setSuffix(i18n("pt"));
    ltButton->setPixmap( BarIcon( "shadowLU" ) );
    tButton->setPixmap( BarIcon( "shadowU" ) );
    rtButton->setPixmap( BarIcon( "shadowRU" ) );
    rButton->setPixmap( BarIcon( "shadowR" ) );
    rbButton->setPixmap( BarIcon( "shadowRB" ) );
    bButton->setPixmap( BarIcon( "shadowB" ) );
    lbButton->setPixmap( BarIcon( "shadowLB" ) );
    lButton->setPixmap( BarIcon( "shadowL" ) );

    connect( colorButton, SIGNAL( changed( const QColor& ) ),
             SLOT( colorChanged( const QColor& ) ) );
}

void KPrShadowDialogImpl::setShadowDirection( ShadowDirection d )
{
    _preview->setShadowDirection( d );
    directionGroup->setButton( d );
}

void KPrShadowDialogImpl::setShadowDistance( int d )
{
    _preview->setShadowDistance( d );
    distanceSpinBox->setValue( d );
}

void KPrShadowDialogImpl::setShadowColor( const QColor &c )
{
    _preview->setShadowColor( c );
    colorButton->setColor( c );
}

void KPrShadowDialogImpl::colorChanged( const QColor& c )
{
    _preview->setShadowColor( c );
}

void KPrShadowDialogImpl::directionChanged( int d )
{
    _preview->setShadowDirection( (ShadowDirection)d );
}

void KPrShadowDialogImpl::distanceChanged( int d )
{
    _preview->setShadowDistance( d );
}

void KPrShadowDialogImpl::applyClicked()
{
    emit apply();
}

void KPrShadowDialogImpl::okClicked()
{
    applyClicked();
    accept();
}

ShadowDirection KPrShadowDialogImpl::shadowDirection()
{
    return (ShadowDirection) ( directionGroup->id( directionGroup->selected() ) );
}

int KPrShadowDialogImpl::shadowDistance()
{
    return distanceSpinBox->value();
}

QColor KPrShadowDialogImpl::shadowColor()
{
    return colorButton->color();
}
#include "KPrShadowDialogImpl.moc"
