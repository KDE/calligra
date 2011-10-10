/*
  Copyright 2008        Brad Hards  <bradh@frogmouth.net>
  Copyright 2009 - 2010 Inge Wallin <inge@lysator.liu.se>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "EmfPainterBackend.h"

#include <math.h>

#include <KDebug>

#include "EmfObjects.h"
#include "EmfDeviceContext.h"


#define DEBUG_EMFPAINT 0
#define DEBUG_PAINTER_TRANSFORM 0

namespace Libemf
{


static QPainter::CompositionMode  rasteropToQtComposition(long rop);
static Qt::FillRule               fillModeToQtFillRule(quint32 polyFillMode);

// ================================================================
//                         Class EmfPainterBackend


EmfPainterBackend::EmfPainterBackend()
    : m_header( 0 )
{
    m_painter         = 0;
    m_painterSaves    = 0;
    m_outputSize      = QSize();
    m_keepAspectRatio = true;
}

EmfPainterBackend::EmfPainterBackend(QPainter &painter, QSize &size,
                                             bool keepAspectRatio)
    : m_header( 0 )
    , m_windowExtIsSet(false)
    , m_viewportExtIsSet(false)
    , m_windowViewportIsSet(false)
{
    m_painter         = &painter;
    m_painterSaves    = 0;
    m_outputSize      = size;
    m_keepAspectRatio = keepAspectRatio;
}

EmfPainterBackend::~EmfPainterBackend()
{
    delete m_header;
}

void EmfPainterBackend::paintBounds(const Header *header)
{
    // The rectangle is in device coordinates.
    QRectF  rect(header->bounds());
    m_painter->save();

    // Draw a simple cross in a rectangle to show the bounds.
    m_painter->setPen(QPen(QColor(172, 196, 206)));
    m_painter->drawRect(rect);
    m_painter->drawLine(rect.topLeft(), rect.bottomRight());
    m_painter->drawLine(rect.bottomLeft(), rect.topRight());

    m_painter->restore();
}

void EmfPainterBackend::init( const Header *header )
{
    // Save the header since we need the frame and bounds inside the drawing.
    m_header = new Header(*header);

    QSize  headerBoundsSize = header->bounds().size();

#if DEBUG_EMFPAINT
    kDebug(31000) << "----------------------------------------------------------------------";
    kDebug(31000) << "Shape size               =" << m_outputSize.width() << m_outputSize.height() << " pt";
    kDebug(31000) << "----------------------------------------------------------------------";
    kDebug(31000) << "Boundary box (dev units) =" << header->bounds().x() << header->bounds().y()
                  << header->bounds().width() << header->bounds().height();
    kDebug(31000) << "Frame (phys size)        =" << header->frame().x() << header->frame().y()
                  << header->frame().width() << header->frame().height() << " *0.01 mm";

    kDebug(31000) << "Device =" << header->device().width() << header->device().height();
    kDebug(31000) << "Millimeters =" << header->millimeters().width()
                  << header->millimeters().height();
#endif

#if DEBUG_PAINTER_TRANSFORM
    printPainterTransform("In init, before save:");
#endif

    // This is restored in cleanup().
    m_painter->save();

    // Calculate how much the painter should be resized to fill the
    // outputSize with output.
    qreal  scaleX = qreal( m_outputSize.width() )  / headerBoundsSize.width();
    qreal  scaleY = qreal( m_outputSize.height() ) / headerBoundsSize.height();
    if ( m_keepAspectRatio ) {
        // Use the smaller value so that we don't get an overflow in
        // any direction.
        if ( scaleX > scaleY )
            scaleX = scaleY;
        else
            scaleY = scaleX;
    }
#if DEBUG_EMFPAINT
    kDebug(31000) << "scale = " << scaleX << ", " << scaleY;
#endif

    // Transform the EMF object so that it fits in the shape.  The
    // topleft of the EMF will be the top left of the shape.
    m_painter->scale( scaleX, scaleY );
    m_painter->translate(-header->bounds().left(), -header->bounds().top());
#if DEBUG_PAINTER_TRANSFORM
    printPainterTransform("after fitting into shape");
#endif

    // Calculate translation if we should center the EMF in the
    // area and keep the aspect ratio.
#if 0 // Should apparently be upper left.  See bug 265868
    if ( m_keepAspectRatio ) {
        m_painter->translate((m_outputSize.width() / scaleX - headerBoundsSize.width()) / 2,
                             (m_outputSize.height() / scaleY - headerBoundsSize.height()) / 2);
#if DEBUG_PAINTER_TRANSFORM
        printPainterTransform("after translation for keeping center in the shape");
#endif
    }
#endif

    m_outputTransform = m_painter->transform();

    // For calculations of window / viewport during the painting
    m_windowOrg = QPoint(0, 0);
    m_viewportOrg = QPoint(0, 0);
    m_windowExtIsSet = false;
    m_viewportExtIsSet = false;
    m_windowViewportIsSet = false;

#if DEBUG_EMFPAINT
    paintBounds(header);
#endif
}

void EmfPainterBackend::cleanup( const Header *header )
{
    Q_UNUSED( header );

#if DEBUG_EMFPAINT
    if (m_painterSaves > 0)
        kDebug(33100) << "WARNING: UNRESTORED DC's:" << m_painterSaves;
#endif

    // Restore all the save()s that were done during the processing.
    for (int i = 0; i < m_painterSaves; ++i)
        m_painter->restore();
    m_painterSaves = 0;

    // Restore the painter to what it was before init() was called.
    m_painter->restore();
}


void EmfPainterBackend::eof()
{
}

void EmfPainterBackend::saveDC(EmfDeviceContext &context)
{
#if DEBUG_EMFPAINT
    kDebug(31000);
#endif

    // A little trick here: Save the worldTransform in the painter.
    // If we didn't do this, we would have to create a separate stack
    // for these.
    //
    // FIXME: We should collect all the parts of the DC that are not
    //        stored in the painter and save them separately.
    QTransform  savedTransform = m_painter->worldTransform();
    m_painter->setWorldTransform(context.m_worldTransform);

    m_painter->save();
    ++m_painterSaves;

    m_painter->setWorldTransform(savedTransform);
}

void EmfPainterBackend::restoreDC(EmfDeviceContext &context, const qint32 savedDC )
{
#if DEBUG_EMFPAINT
    kDebug(31000) << savedDC;
#endif

    // Note that savedDC is always negative
    for (int i = 0; i < -savedDC; ++i) {
        if (m_painterSaves > 0) {
            m_painter->restore();
            --m_painterSaves;
        }
        else {
            kDebug(33100) << "restoreDC(): try to restore painter without save" << savedDC - i;
            break;
        }
    }

    // We used a trick in saveDC() and stored the worldTransform in
    // the painter.  Now restore the full transformation.
    context.m_worldTransform = m_painter->worldTransform();
    QTransform newMatrix = context.m_worldTransform * m_outputTransform;
    m_painter->setWorldTransform( newMatrix );
}

void EmfPainterBackend::setMetaRgn(EmfDeviceContext &context)
{
    kDebug(33100) << "EMR_SETMETARGN not yet implemented";
}


// ----------------------------------------------------------------
//                         Drawing operations


void EmfPainterBackend::setPixelV(EmfDeviceContext &context,
                                      QPoint &point, quint8 red, quint8 green, quint8 blue,
                                      quint8 reserved )
{
    Q_UNUSED( reserved );

    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << point << red << green << blue;
#endif

    m_painter->save();

    QPen pen;
    pen.setColor( QColor( red, green, blue ) );
    m_painter->setPen( pen );
    m_painter->drawPoint( point );

    m_painter->restore();
}


void EmfPainterBackend::arc(EmfDeviceContext &context,
                                const QRect &box, const QPoint &start, const QPoint &end )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << box << start << end;
#endif

    QPoint centrePoint = box.center();

    qreal startAngle = angleFromArc( centrePoint, start );
    qreal endAngle   = angleFromArc( centrePoint, end );
    qreal spanAngle  = angularSpan( startAngle, endAngle );

    m_painter->drawArc( box, startAngle*16, spanAngle*16 );
}

void EmfPainterBackend::chord(EmfDeviceContext &context,
                                  const QRect &box, const QPoint &start, const QPoint &end )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << box << start << end;
#endif

    QPoint centrePoint = box.center();

    qreal startAngle = angleFromArc( centrePoint, start );
    qreal endAngle   = angleFromArc( centrePoint, end );
    qreal spanAngle  = angularSpan( startAngle, endAngle );

    m_painter->drawChord( box, startAngle*16, spanAngle*16 );
}

void EmfPainterBackend::pie(EmfDeviceContext &context,
                                const QRect &box, const QPoint &start, const QPoint &end )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << box << start << end;
#endif

    QPoint centrePoint = box.center();

    qreal startAngle = angleFromArc( centrePoint, start );
    qreal endAngle   = angleFromArc( centrePoint, end );
    qreal spanAngle  = angularSpan( startAngle, endAngle );

    m_painter->drawPie( box, startAngle*16, spanAngle*16 );
}

void EmfPainterBackend::ellipse(EmfDeviceContext &context, const QRect &box )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << box;
#endif

    m_painter->drawEllipse( box );
}

void EmfPainterBackend::rectangle(EmfDeviceContext &context, const QRect &box )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << box;
#endif

    m_painter->drawRect( box );
}


// ----------------------------------------------------------------


#define DEBUG_TEXTOUT 0

void EmfPainterBackend::extTextOut(EmfDeviceContext &context,
                                   const QRect &bounds, const EmrTextObject &textObject )
{
    updateFromDeviceContext(context);

    const QPoint  &referencePoint = textObject.referencePoint();
    const QString &text = textObject.textString();

#if DEBUG_EMFPAINT
    kDebug(31000) << "Ref point: " << referencePoint
                  << "options: " << hex << textObject.options() << dec
                  << "rectangle: " << textObject.rectangle()
                  << "text: " << textObject.textString();
#endif

    int  x = referencePoint.x();
    int  y = referencePoint.y();

    // The TA_UPDATECP flag tells us to use the current position
    if (context.textAlignMode & TA_UPDATECP) {
        // (left, top) position = current logical position
#if DEBUG_EMFPAINT
        kDebug(31000) << "TA_UPDATECP: use current logical position";
#endif
        x = context.currentPoint.x();
        y = context.currentPoint.y();
    }

    QFontMetrics  fm = m_painter->fontMetrics();
    int textWidth  = fm.width(text) + fm.descent(); // fm.width(text) isn't right with Italic text
    int textHeight = fm.height();

    // Make (x, y) be the coordinates of the upper left corner of the
    // rectangle surrounding the text.
    //
    // FIXME: Handle RTL text.

    // Horizontal align.  Default is TA_LEFT.
    if ((context.textAlignMode & TA_HORZMASK) == TA_CENTER)
        x -= (textWidth / 2);
    else if ((context.textAlignMode & TA_HORZMASK) == TA_RIGHT)
        x -= textWidth;

    // Vertical align.  Default is TA_TOP
    if ((context.textAlignMode & TA_VERTMASK) == TA_BASELINE)
        y -= (textHeight - fm.descent());
    else if ((context.textAlignMode & TA_VERTMASK) == TA_BOTTOM) {
        y -= textHeight;
    }

#if DEBUG_EMFPAINT
    kDebug(31000) << "textWidth = " << textWidth << "height = " << textHeight;

    kDebug(31000) << "font = " << m_painter->font()
                  << "pointSize = " << m_painter->font().pointSize()
                  << "ascent = " << fm.ascent() << "descent = " << fm.descent()
                  << "height = " << fm.height()
                  << "leading = " << fm.leading();
    kDebug(31000) << "actual point = " << x << y;
#endif

    // Debug code that paints a rectangle around the output area.
#if DEBUG_TEXTOUT
    m_painter->save();
    m_painter->setWorldTransform(m_outputTransform);
    m_painter->setPen(Qt::black);
    m_painter->drawRect(bounds);
    m_painter->restore();
#endif

    // Actual painting starts here.
    m_painter->save();

    // Find out how much we have to scale the text to make it fit into
    // the output rectangle.  Normally this wouldn't be necessary, but
    // when fonts are switched, the replacement fonts are sometimes
    // wider than the original fonts.
    QRect  worldRect(context.m_worldTransform.mapRect(QRect(x, y, textWidth, textHeight)));
    //kDebug(31000) << "rects:" << QRect(x, y, textWidth, textHeight) << worldRect;
    qreal  scaleX = qreal(1.0);
    qreal  scaleY = qreal(1.0);
    if (bounds.width() < worldRect.width())
        scaleX = qreal(bounds.width()) / qreal(worldRect.width());
    if (bounds.height() < worldRect.height())
        scaleY = qreal(bounds.height()) / qreal(worldRect.height());
    //kDebug(31000) << "scale:" << scaleX << scaleY;

    if (scaleX < qreal(1.0) || scaleY < qreal(1.0)) {
        m_painter->translate(-x, -y);
        m_painter->scale(scaleX, scaleY);
        m_painter->translate(x / scaleX, y / scaleY);
    }

    // Use the special pen defined by mTextPen for text.
    QPen  savePen = m_painter->pen();
    m_painter->setPen(context.foregroundTextColor);
    m_painter->drawText(int(x / scaleX), int(y / scaleY), textWidth, textHeight,
                        Qt::AlignLeft|Qt::AlignTop, text);
    m_painter->setPen(savePen);

    m_painter->restore();
}

void EmfPainterBackend::moveToEx(EmfDeviceContext &context, const qint32 x, const qint32 y )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << x << y;
#endif

    if ( context.isDefiningPath )
        context.path.moveTo( QPoint( x, y ) );
    else
        context.currentPoint = QPoint( x, y );
}

void EmfPainterBackend::lineTo(EmfDeviceContext &context, const QPoint &finishPoint )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << finishPoint;
#endif

    if ( context.isDefiningPath )
        context.path.lineTo( finishPoint );
    else {
        m_painter->drawLine( context.currentPoint, finishPoint );
        context.currentPoint = finishPoint;
    }
}

void EmfPainterBackend::arcTo(EmfDeviceContext &context,
                                  const QRect &box, const QPoint &start, const QPoint &end )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << box << start << end;
#endif

    QPoint centrePoint = box.center();

    qreal startAngle = angleFromArc( centrePoint, start );
    qreal endAngle   = angleFromArc( centrePoint, end );
    qreal spanAngle  = angularSpan( startAngle, endAngle );

    context.path.arcTo( box, startAngle, spanAngle );
}

void EmfPainterBackend::polygon16(EmfDeviceContext &context,
                                      const QRect &bounds, const QList<QPoint> points )
{
    updateFromDeviceContext(context);

    Q_UNUSED( bounds );

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    QVector<QPoint> pointVector = points.toVector();
    m_painter->drawPolygon( pointVector.constData(), pointVector.size(),
                            fillModeToQtFillRule(context.polyFillMode) );
}

void EmfPainterBackend::polyLine(EmfDeviceContext &context,
                                     const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED( bounds );

    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    QVector<QPoint> pointVector = points.toVector();
    m_painter->drawPolyline( pointVector.constData(), pointVector.size() );
}

void EmfPainterBackend::polyLine16(EmfDeviceContext &context,
                                       const QRect &bounds, const QList<QPoint> points )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    polyLine(context, bounds, points );
}

void EmfPainterBackend::polyPolygon16(EmfDeviceContext &context,
                                          const QRect &bounds, const QList< QVector< QPoint > > &points )
{
    Q_UNUSED( bounds );

    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    for ( int i = 0; i < points.size(); ++i ) {
        m_painter->drawPolygon( points[i].constData(), points[i].size(),
                                fillModeToQtFillRule(context.polyFillMode) );
    }
}

void EmfPainterBackend::polyPolyLine16(EmfDeviceContext &context,
                                           const QRect &bounds, const QList< QVector< QPoint > > &points )
{
    Q_UNUSED( bounds );

    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    for ( int i = 0; i < points.size(); ++i ) {
        m_painter->drawPolyline( points[i].constData(), points[i].size() );
    }
}

void EmfPainterBackend::polyLineTo16(EmfDeviceContext &context,
                                         const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED( bounds );

    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    for ( int i = 0; i < points.count(); ++i ) {
	context.path.lineTo( points[i] );
    }
}

void EmfPainterBackend::polyBezier16(EmfDeviceContext &context,
                                         const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED( bounds );

    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    QPainterPath path;
    path.moveTo( points[0] );
    for ( int i = 1; i < points.count(); i+=3 ) {
	path.cubicTo( points[i], points[i+1], points[i+2] );
    }
    m_painter->drawPath( path );
}

void EmfPainterBackend::polyBezierTo16(EmfDeviceContext &context,
                                           const QRect &bounds, const QList<QPoint> points )
{
    Q_UNUSED( bounds );

    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds << points;
#endif

    for ( int i = 0; i < points.count(); i+=3 ) {
	context.path.cubicTo( points[i], points[i+1], points[i+2] );
    }
}

void EmfPainterBackend::fillPath(EmfDeviceContext &context, const QRect &bounds )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds;
#endif

    Q_UNUSED( bounds );
    m_painter->fillPath(context.path, m_painter->brush());
}

void EmfPainterBackend::strokeAndFillPath(EmfDeviceContext &context, const QRect &bounds )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds;
#endif

    Q_UNUSED( bounds );
    m_painter->drawPath(context.path);
}

void EmfPainterBackend::strokePath(EmfDeviceContext &context, const QRect &bounds )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bounds;
#endif

    Q_UNUSED( bounds );
    m_painter->strokePath(context.path, m_painter->pen());
}

void EmfPainterBackend::setClipPath(EmfDeviceContext &context, const quint32 regionMode )
{
#if DEBUG_EMFPAINT
    kDebug(31000) << hex << regionMode << dec;
#endif

    switch (regionMode) {
    case RGN_AND:
        m_painter->setClipPath(context.path, Qt::IntersectClip);
        break;
    case RGN_OR:
        m_painter->setClipPath(context.path, Qt::UniteClip);
        break;
    case RGN_COPY:
        m_painter->setClipPath(context.path, Qt::ReplaceClip);
        break;
    default:
        qWarning() <<  "Unexpected / unsupported clip region mode:" << regionMode;
        Q_ASSERT( 0 );
    }
}

void EmfPainterBackend::bitBlt(EmfDeviceContext &context, BitBltRecord &bitBltRecord )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << bitBltRecord.xDest() << bitBltRecord.yDest()
                  << bitBltRecord.cxDest() << bitBltRecord.cyDest()
                  << hex << bitBltRecord.rasterOperation() << dec
                  << bitBltRecord.bkColorSrc();
#endif

    QRect target( bitBltRecord.xDest(), bitBltRecord.yDest(),
                  bitBltRecord.cxDest(), bitBltRecord.cyDest() );
    // 0x00f00021 is the PatCopy raster operation which just fills a rectangle with a brush.
    // This seems to be the most common one.
    //
    // FIXME: Implement the rest of the raster operations.
    if (bitBltRecord.rasterOperation() == 0x00f00021) {
        // Would have been nice if we didn't have to pull out the
        // brush to use it with fillRect()...
        QBrush brush = m_painter->brush();
        m_painter->fillRect(target, brush);
    }
    else if ( bitBltRecord.hasImage() ) {
        m_painter->drawImage( target, bitBltRecord.image() );
    }
}


void EmfPainterBackend::stretchDiBits(EmfDeviceContext &context, StretchDiBitsRecord &record )
{
    updateFromDeviceContext(context);

#if DEBUG_EMFPAINT
    kDebug(31000) << "Bounds:    " << record.bounds();
    kDebug(31000) << "Dest rect: "
                  << record.xDest() << record.yDest() << record.cxDest() << record.cyDest();
    kDebug(31000) << "Src rect:  "
                  << record.xSrc() << record.ySrc() << record.cxSrc() << record.cySrc();
    kDebug(31000) << "Raster op: " << hex << record.rasterOperation() << dec;
                  //<< record.bkColorSrc();
    kDebug(31000) << "usageSrc: " << record.usageSrc();
#endif

    QPoint targetPosition( record.xDest(), record.yDest() );
    QSize  targetSize( record.cxDest(), record.cyDest() );

    QPoint sourcePosition( record.xSrc(), record.ySrc() );
    QSize  sourceSize( record.cxSrc(), record.cySrc() );

    // special cases, from [MS-EMF] Section 2.3.1.7:
    // "This record specifies a mirror-image copy of the source bitmap to the
    // destination if the signs of the height or width fields differ. That is,
    // if cxSrc and cxDest have different signs, this record specifies a mirror
    // image of the source bitmap along the x-axis. If cySrc and cyDest have
    // different signs, this record specifies a mirror image of the source
    //  bitmap along the y-axis."
    QRect target( targetPosition, targetSize );
    QRect source( sourcePosition, sourceSize );
#if DEBUG_EMFPAINT
    //kDebug(31000) << "image size" << record.image()->size();
    kDebug(31000) << "Before transformation:";
    kDebug(31000) << "    target" << target;
    kDebug(31000) << "    source" << source;
#endif
    if ( source.width() < 0 && target.width() > 0 ) {
        sourceSize.rwidth() *= -1;
        sourcePosition.rx() -= sourceSize.width();
        source = QRect( sourcePosition, sourceSize );
    }
    if  ( source.width() > 0 && target.width() < 0 ) {
        targetSize.rwidth() *= -1;
        targetPosition.rx() -= targetSize.width();
        target = QRect( targetPosition, targetSize );
    }
    if ( source.height() < 0 && target.height() > 0 ) {
        sourceSize.rheight() *= -1;
        sourcePosition.ry() -= sourceSize.height();
        source = QRect( sourcePosition, sourceSize );
    }
    if  ( source.height() > 0 && target.height() < 0 ) {
        targetSize.rheight() *= -1;
        targetPosition.ry() -= targetSize.height();
        target = QRect( targetPosition, targetSize );
    }
#if DEBUG_EMFPAINT
    kDebug(31000) << "After transformation:";
    kDebug(31000) << "    target" << target;
    kDebug(31000) << "    source" << source;
    QImage image = record.image();
    kDebug(31000) << "Image" << image.size();

#endif

    QPainter::RenderHints      oldRenderHints = m_painter->renderHints();
    QPainter::CompositionMode  oldCompMode    = m_painter->compositionMode();
    m_painter->setRenderHints(0); // Antialiasing makes composition modes invalid
    m_painter->setCompositionMode(rasteropToQtComposition(record.rasterOperation()));
    m_painter->drawImage(target, record.image(), source);
    m_painter->setCompositionMode(oldCompMode);
    m_painter->setRenderHints(oldRenderHints);
}


// ----------------------------------------------------------------
//                         Private functions


void EmfPainterBackend::printPainterTransform(EmfDeviceContext &context, const char *leadText)
{
#if 0   // temporarily disabled
    QTransform  transform;

    recalculateWorldTransform();

    kDebug(31000) << leadText << "world transform " << context.m_worldTransform
                  << "incl output transform: " << m_painter->transform();
#endif
}


qreal EmfPainterBackend::angleFromArc( const QPoint &centrePoint, const QPoint &radialPoint )
{
    double dX = radialPoint.x() - centrePoint.x();
    double dY = centrePoint.y() - radialPoint.y();
    // Qt angles are in degrees. atan2 returns radians
    return ( atan2( dY, dX ) * 180 / M_PI );
}

qreal EmfPainterBackend::angularSpan( const qreal startAngle, const qreal endAngle )
{
    qreal spanAngle = endAngle - startAngle;

    if ( spanAngle <= 0 ) {
        spanAngle += 360;
    }

    return spanAngle;
}

static Qt::FillRule fillModeToQtFillRule(quint32 polyFillMode)
{
    if ( polyFillMode == ALTERNATE ) {
	return Qt::OddEvenFill;
    } else if ( polyFillMode == WINDING ) {
	return Qt::WindingFill;
    }

    // Good default?
    return Qt::OddEvenFill;
}

static QPainter::CompositionMode  rasteropToQtComposition(long rop)
{
    // Code copied from filters/libkowmf/qwmf.cc
    // FIXME: Should be cleaned up

    /* TODO: Ternary raster operations
    0x00C000CA  dest = (source AND pattern)
    0x00F00021  dest = pattern
    0x00FB0A09  dest = DPSnoo
    0x005A0049  dest = pattern XOR dest   */
    static const struct OpTab {
        long winRasterOp;
        QPainter::CompositionMode qtRasterOp;
    } opTab[] = {
        // ### untested (conversion from Qt::RasterOp)
        { 0x00CC0020, QPainter::CompositionMode_Source }, // CopyROP
        { 0x00EE0086, QPainter::RasterOp_SourceOrDestination }, // OrROP
        { 0x008800C6, QPainter::RasterOp_SourceAndDestination }, // AndROP
        { 0x00660046, QPainter::RasterOp_SourceXorDestination }, // XorROP
        // ----------------------------------------------------------------
        // FIXME: Checked above this, below is still todo
        // ----------------------------------------------------------------
        { 0x00440328, QPainter::CompositionMode_DestinationOut }, // AndNotROP
        { 0x00330008, QPainter::CompositionMode_DestinationOut }, // NotCopyROP
        { 0x001100A6, QPainter::CompositionMode_SourceOut }, // NandROP
        { 0x00C000CA, QPainter::CompositionMode_Source }, // CopyROP
        { 0x00BB0226, QPainter::CompositionMode_Destination }, // NotOrROP
        { 0x00F00021, QPainter::CompositionMode_Source }, // CopyROP
        { 0x00FB0A09, QPainter::CompositionMode_Source }, // CopyROP
        { 0x005A0049, QPainter::CompositionMode_Source }, // CopyROP
        { 0x00550009, QPainter::CompositionMode_DestinationOut }, // NotROP
        { 0x00000042, QPainter::CompositionMode_Clear }, // ClearROP
        { 0x00FF0062, QPainter::CompositionMode_Source } // SetROP
    };

    int i;
    for (i = 0 ; i < 15 ; i++)
        if (opTab[i].winRasterOp == rop)
            break;

    if (i < 15)
        return opTab[i].qtRasterOp;
    else
        return QPainter::CompositionMode_Source;
}


// ----------------------------------------------------------------
//                         Private functions


// If anything has changed in the device context that is relevant to
// the QPainter, then update the painter with the corresponding data.
//
void EmfPainterBackend::updateFromDeviceContext(EmfDeviceContext &context)
{
    // Graphic objects
    if (context.changedItems & DCBrush) {
        m_painter->setBrush(context.brush);
#if DEBUG_EMFPAINT
        kDebug(31000) << "*** Setting fill brush to" << context.brush;
#endif
    }
    // FIXME: context.image
    if (context.changedItems & DCFont) {
        m_painter->setFont(context.font);
#if DEBUG_EMFPAINT
        kDebug(31000) << "*** Setting font to" << context.font;
#endif
    }
    if (context.changedItems & DCPalette) {
        // NYI
#if DEBUG_EMFPAINT
        kDebug(31000) << "*** Setting palette (NYI)";
#endif
    }
    if (context.changedItems & DCPen) {
        QPen p = context.pen;
        int width = p.width();

#if 0 // Check if this code from WMF can also be used for EMF
        if (dynamic_cast<QPrinter *>(mTarget)) {
            width = 0;
        }
        else 
#endif
        if (width == 1)
            // I'm unsure of this, but it seems that EMF uses line
            // width == 1 as cosmetic pen.  Or it could just be that
            // any line width < 1 should be drawn as width == 1.  The
            // EMF spec doesn't mention the term "cosmetic pen"
            // anywhere so we don't get any clue there.
            //
            // For an example where this is shown clearly, see
            // wmf_tests.doc, in the colored rectangles and the polypolygon.
            width = 0;
#if 0
        else {
            // EMF spec: width of pen in logical coordinate
            // => width of pen proportional with device context width
            QRect rec = m_painter->window();
            // QPainter documentation says this is equivalent of xFormDev, but it doesn't compile. Bug reported.

            QRect devRec = rec * m_painter->matrix();
            if (rec.width() != 0)
                width = (width * devRec.width()) / rec.width() ;
            else
                width = 0;
        }
#endif

        p.setWidth(width);
        m_painter->setPen(p);
#if DEBUG_EMFPAINT
        kDebug(31000) << "*** Setting pen to" << p;
#endif
    }
    if (context.changedItems & DCClipRegion) {
        // Not used until SETCLIPREGION is used
#if DEBUG_EMFPAINT
        //kDebug(31000) << "*** region changed to" << context.region;
#endif
    }

    // Structure objects
    if (context.changedItems & DCBgColor) {
        m_painter->setBackground(QBrush(context.backgroundColor));
#if DEBUG_EMFPAINT
        kDebug(31000) << "*** Setting background text color to" << context.backgroundColor;
#endif
    }
    //----------------------------------------------------------------
    // Output surface not supported
    //DCViewportExt
    //DCViewportorg
    //DCWindowExt  
    //DCWindoworg  
    if (context.changedItems & DCWorldTransform) {
        // Apply the output transform.
        QTransform newMatrix = context.m_worldTransform * m_outputTransform;
        m_painter->setWorldTransform(newMatrix);
    }

    //----------------------------------------------------------------
    // Graphic Properties

    if (context.changedItems & DCBkMode) {
        // FIXME: Check the default value for this.
        m_painter->setBackgroundMode(context.bkMode == TRANSPARENT ? Qt::TransparentMode
                                                                   : Qt::OpaqueMode);
#if DEBUG_EMFPAINT
        kDebug(31000) << "*** Setting background mode to" << context.bkMode;
#endif
    }

    //Break extra space NYI
    //Font mapping mode NYI
#if 0 // Fix when we do the MixMode in the device context
    if (context.changedItems & DCFgMixMode) {
        // FIXME: Check the default value for this.
        QPainter::CompositionMode  compMode = QPainter::CompositionMode_Source;
        if (context.rop < 17)
            compMode = koWmfOpTab16[context.rop];
        m_painter->setCompositionMode(compMode);

#if DEBUG_EMFPAINT
        kDebug(31000) << "*** Setting composition mode to" << context.rop;
#endif
    }
#endif
    // rop2mode: not necessary to handle here
    // layoutMode
    if (context.changedItems & DCLayoutMode) {
        if (context.layoutMode == LAYOUT_LTR) {
            m_painter->setLayoutDirection(Qt::LeftToRight);
        } else if (context.layoutMode == LAYOUT_RTL) {
            m_painter->setLayoutDirection(Qt::RightToLeft);
        } else {
            kDebug(33100) << "EMR_SETLAYOUT: Unexpected value -" << context.layoutMode;
            m_painter->setLayoutDirection(Qt::LeftToRight);
        }
    }
    //Mapping mode NYI
    //PolyFillMode  not necessary to handle here
    //Stretchblt    not necesseray to handle here
    //textAlignMode not necessary to handle here
    //Text extra space NYI

    // Reset all changes until next time.
    context.changedItems = 0;
}

} // xnamespace...
