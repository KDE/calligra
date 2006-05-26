#include "KPrTextPreview.h"

#include <KoGlobal.h>

#include <qpainter.h>
#include <QFont>
//Added by qt3to4:
#include <Q3Frame>

KPrTextPreview::KPrTextPreview( QWidget* parent, const char* name )
    : Q3Frame( parent, name ),
      shadowDirection( SD_LEFT_BOTTOM ),
      shadowDistance( 0 ),
      angle( 0 )
{
    setBackgroundColor( Qt::white );
    setFrameStyle( NoFrame );
}

void KPrTextPreview::drawContents( QPainter* painter )
{
    QFont font(KoGlobal::defaultFont().family(), 30, QFont::Bold);
    QFontMetrics fm( font );

    QRect br = fm.boundingRect( "KOffice" );
    int pw = br.width();
    int ph = br.height();
    QRect r = br;
    int textYPos = -r.y();
    int textXPos = -r.x();
    br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
    r.moveTopLeft( QPoint( -r.width() / 2, -r.height() / 2 ) );

    int x = r.left() + textXPos;
    int y = r.top() + textYPos;
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

    painter->setViewport( ( width() - pw ) / 2, ( height() - ph ) / 2, width(), height() );

    QMatrix m, mtx;
    mtx.rotate( angle );
    m.translate( pw / 2, ph / 2 );
    m = mtx * m;

    painter->setMatrix( m );
    painter->setFont( font );

    if ( shadowDistance > 0 ) {
	painter->setPen( shadowColor );
	painter->drawText( sx, sy, "KOffice" );
    }
    painter->setPen( Qt::blue );
    painter->drawText( x, y, "KOffice" );

    painter->restore();
}
#include "KPrTextPreview.moc"
