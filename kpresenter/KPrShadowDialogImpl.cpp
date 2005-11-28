// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>

#include <kcolorbutton.h>
#include <klocale.h>

#include "KPrShadowDialogImpl.h"
#include "KPrTextPreview.h"

ShadowDialogImpl::ShadowDialogImpl( QWidget *parent, const char* name )
    : ShadowDialogBase( parent, name )
{
    _preview = new TextPreview( previewPanel );
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

void ShadowDialogImpl::setShadowDirection( ShadowDirection d )
{
    _preview->setShadowDirection( d );
    directionGroup->setButton( d );
}

void ShadowDialogImpl::setShadowDistance( int d )
{
    _preview->setShadowDistance( d );
    distanceSpinBox->setValue( d );
}

void ShadowDialogImpl::setShadowColor( const QColor &c )
{
    _preview->setShadowColor( c );
    colorButton->setColor( c );
}

void ShadowDialogImpl::colorChanged( const QColor& c )
{
    _preview->setShadowColor( c );
}

void ShadowDialogImpl::directionChanged( int d )
{
    _preview->setShadowDirection( (ShadowDirection)d );
}

void ShadowDialogImpl::distanceChanged( int d )
{
    _preview->setShadowDistance( d );
}

void ShadowDialogImpl::applyClicked()
{
    emit apply();
}

void ShadowDialogImpl::okClicked()
{
    applyClicked();
    accept();
}

ShadowDirection ShadowDialogImpl::shadowDirection()
{
    return (ShadowDirection) ( directionGroup->id( directionGroup->selected() ) );
}

int ShadowDialogImpl::shadowDistance()
{
    return distanceSpinBox->value();
}

QColor ShadowDialogImpl::shadowColor()
{
    return colorButton->color();
}
#include "KPrShadowDialogImpl.moc"
