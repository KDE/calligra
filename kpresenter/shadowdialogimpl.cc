#include <qpainter.h>
#include <qfont.h>
#include <qspinbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>

#include <kcolorbutton.h>
#include <kdebug.h>
#include <koGlobal.h>

#include "shadowdialogimpl.h"

ShadowPreview::ShadowPreview( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    setFrameStyle( NoFrame );
}

void ShadowPreview::drawContents( QPainter* painter )
{
    QFont font(KoGlobal::defaultFont().family(), 30, QFont::Bold);
    QFontMetrics fm( font );

    QRect br = fm.boundingRect( "KOffice" );
    int x = ( width() - br.width() ) / 2;
    int y = ( height() - br.height() ) / 2 + br.height();
    int sx = 0, sy = 0;

    switch ( shadowDirection )
    {
    case SD_LEFT_UP:
    {
        sx = x - shadowDistance;
        sy = y - shadowDistance;
    } break;
    case SD_UP:
    {
        sx = x;
        sy = y - shadowDistance;
    } break;
    case SD_RIGHT_UP:
    {
        sx = x + shadowDistance;
        sy = y - shadowDistance;
    } break;
    case SD_RIGHT:
    {
        sx = x + shadowDistance;
        sy = y;
    } break;
    case SD_RIGHT_BOTTOM:
    {
        sx = x + shadowDistance;
        sy = y + shadowDistance;
    } break;
    case SD_BOTTOM:
    {
        sx = x;
        sy = y + shadowDistance;
    } break;
    case SD_LEFT_BOTTOM:
    {
        sx = x - shadowDistance;
        sy = y + shadowDistance;
    } break;
    case SD_LEFT:
    {
        sx = x - shadowDistance;
        sy = y;
    } break;
    }

    painter->save();

    painter->setFont( font );
    painter->setPen( shadowColor );
    painter->drawText( sx, sy, "KOffice" );

    painter->setPen( blue );
    painter->drawText( x, y, "KOffice" );

    painter->restore();
}

ShadowDialogImpl::ShadowDialogImpl( QWidget *parent, const char* name )
    : ShadowDialogBase( parent, name )
{
    _preview = new ShadowPreview( previewPanel );
    QHBoxLayout *lay = new QHBoxLayout( previewPanel, 4, 0 );
    lay->addWidget( _preview );

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


ShadowDialogImpl::~ShadowDialogImpl()
{

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
