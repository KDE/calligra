#include "textpreview.h"

#include <koGlobal.h>

#include <qpainter.h>
#include <qfont.h>

TextPreview::TextPreview( QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    setBackgroundColor( white );
    setFrameStyle( NoFrame );
}

void TextPreview::drawContents( QPainter* painter )
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
