#include "kwimage.h"
#include "kwtextparag.h"
#include "kwtextframeset.h"
#include "kwdoc.h"
#include "defs.h"
#include <kdebug.h>

KWImage KWImageCollection::image( const QString & fileName )
{
    KWImage i = findImage( fileName );
    if ( i.isNull() )
    {
        kdDebug() << " building image " << endl;
        QImage img( fileName );
        if ( !img.isNull() )
            i = insertImage( fileName, img );
        else
            kdWarning() << "Couldn't build QImage from " << fileName << endl;
    }
    return i;
}

KWTextImage::KWTextImage( KWTextDocument *textdoc, const QString & filename )
    : QTextCustomItem( textdoc ), place( PlaceInline )
{
    KWDocument * doc = textdoc->textFrameSet()->kWordDocument();
    if ( !filename.isEmpty() )
    {
        m_image = doc->imageCollection()->image( filename );
        ASSERT( !m_image.isNull() );
        adjustToPainter( 0L ); // Zoom if necessary
    }
}

void KWTextImage::setImage( const KWImage &image )
{
    m_image = image;
    adjustToPainter( 0L );
}

void KWTextImage::adjustToPainter( QPainter* )
{
    if ( !m_image.isNull() ) {
        KWDocument * doc = static_cast<KWTextDocument *>(parent)->textFrameSet()->kWordDocument();
        width = m_image.originalSize().width();
        width = (int)( doc->zoomItX( (double)width ) / POINT_TO_INCH( QPaintDevice::x11AppDpiX() ) );
        height = m_image.originalSize().height();
        height = (int)( doc->zoomItY( (double)height ) / POINT_TO_INCH( QPaintDevice::x11AppDpiY() ) );
        // This ensures 1-1 at 100% on screen, but allows zooming and printing with correct DPI values
        kdDebug() << "KWTextImage::adjustToPainter scaling to " << width << ", " << height << endl;
        m_image = m_image.scale( QSize( width, height ) );
    }
}

void KWTextImage::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg )
{
    if ( placement() != PlaceInline ) {
        x = xpos;
        y = ypos;
    }

    if ( m_image.isNull() ) {
        kdDebug() << "KWTextImage::draw null image!" << endl;
        p->fillRect( x , y, 50, 50, cg.dark() );
        return;
    }

    if ( placement() != PlaceInline && !QRect( xpos, ypos, m_image.size().width(), m_image.size().height() ).intersects( QRect( cx, cy, cw, ch ) ) )
        return;
    if ( placement() == PlaceInline )
        p->drawPixmap( x, y, m_image.pixmap() );
    else
        p->drawPixmap( cx, cy, m_image.pixmap(), cx - x, cy - y, cw, ch );
}
