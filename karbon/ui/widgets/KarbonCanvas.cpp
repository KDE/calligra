/* This file is part of the KDE project
   Copyright (C) 2001-2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2001-2002 Rob Buis <buis@kde.org>
   Copyright (C) 2002-2004, 2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2002 Benoit Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2004 Waldo Bastian <bastian@kde.org>
   Copyright (C) 2004-2005 David Faure <faure@kde.org>
   Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2005-2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2006 Peter Simonsson <psn@linux.se>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Thorsten Zachmann <t.zachmann@zagge.de>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KarbonCanvas.h"
#include "KarbonDocument.h"
#include "KarbonPart.h"
#include <KarbonOutlinePaintingStrategy.h>

#include <KoZoomHandler.h>
#include <KoShapeManager.h>
#include <KoToolProxy.h>
#include <KoShapeManagerPaintingStrategy.h>

#include <kdebug.h>
#include <klocale.h>

#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QFocusEvent>
#include <QtGui/QMouseEvent>
#include <QtCore/QEvent>
#include <QtCore/QSizeF>

class KarbonCanvas::KarbonCanvasPrivate
{
public:
    KarbonCanvasPrivate()
    : zoomHandler()
    , document( 0 )
    , part( 0 )
    , showMargins( false )
    , documentOffset( 0, 0 )
    , viewMargin( 100 )
    {}

    ~KarbonCanvasPrivate()
    {
        delete toolProxy;
        toolProxy = 0;
        delete shapeManager;
    }

    KoShapeManager* shapeManager;
    KoZoomHandler zoomHandler;

    KoToolProxy *toolProxy;

    KarbonDocument *document;
    KarbonPart *part;
    QPoint origin;         ///< the origin of the document page rect
    bool showMargins;      ///< should page margins be shown
    QPoint documentOffset; ///< the offset of the virtual canvas from the viewport
    int viewMargin;        ///< the view margin around the document in pixels
    QRectF contentRect;    ///< the last calculated content rect
};

KarbonCanvas::KarbonCanvas( KarbonPart *p )
    : QWidget() , KoCanvasBase( p ), d( new KarbonCanvasPrivate() )
{
    d->part = p;
    d->document = &p->document();
    d->toolProxy = new KoToolProxy(this);
    d->shapeManager = new KoShapeManager(this, d->document->shapes() );
    connect( d->shapeManager, SIGNAL(selectionChanged()), this, SLOT(adjustOrigin()) );

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus); // allow to receive keyboard input
    adjustOrigin();
    setAttribute(Qt::WA_InputMethodEnabled, true);
}

KarbonCanvas::~KarbonCanvas()
{
    delete d;
}

KoShapeManager * KarbonCanvas::shapeManager() const
{
    return d->shapeManager;
}

const KoViewConverter * KarbonCanvas::viewConverter() const
{
    return &d->zoomHandler;
}

KoToolProxy * KarbonCanvas::toolProxy() const
{
    return d->toolProxy;
}

QWidget * KarbonCanvas::canvasWidget()
{
    return this;
}

void KarbonCanvas::paintEvent(QPaintEvent * ev)
{
    QPainter gc( this );
    gc.translate(-d->documentOffset);
    gc.setRenderHint(QPainter::Antialiasing);

    QRect clipRect = ev->rect().translated(d->documentOffset);
    gc.setClipRect( clipRect );

    gc.translate( d->origin.x(), d->origin.y() );
    gc.setPen( Qt::black );
    //gc.setBrush( Qt::white );
    gc.drawRect( d->zoomHandler.documentToView( QRectF( QPointF(0.0, 0.0), d->document->pageSize() ) ) );

    paintMargins( gc, d->zoomHandler );
    gc.setRenderHint(QPainter::Antialiasing, false);
    d->part->gridData().paintGrid( gc, d->zoomHandler, d->zoomHandler.viewToDocument( widgetToView( clipRect ) ) );
    gc.setRenderHint(QPainter::Antialiasing);

    d->shapeManager->paint( gc, d->zoomHandler, false );
    d->toolProxy->paint( gc, d->zoomHandler );

    gc.end();
}

void KarbonCanvas::paintMargins( QPainter &painter, const KoViewConverter &converter )
{
    if( ! d->showMargins )
        return;

    KoPageLayout pl = d->part->pageLayout();

    QSizeF pageSize = d->document->pageSize();
    QRectF marginRect( pl.left, pl.top, pageSize.width()-pl.left-pl.right, pageSize.height()-pl.top-pl.bottom );

    QPen pen( Qt::blue );
    QVector<qreal> pattern;
    pattern << 5 << 5;
    pen.setDashPattern( pattern );
    painter.setPen( pen );
    painter.drawRect( converter.documentToView( marginRect ) );
}

void KarbonCanvas::mouseMoveEvent(QMouseEvent *e)
{
    d->toolProxy->mouseMoveEvent( e, d->zoomHandler.viewToDocument( widgetToView( e->pos() + d->documentOffset ) ) );
}

void KarbonCanvas::mousePressEvent(QMouseEvent *e)
{
    d->toolProxy->mousePressEvent( e, d->zoomHandler.viewToDocument( widgetToView( e->pos() + d->documentOffset ) ) );
}

void KarbonCanvas::mouseDoubleClickEvent(QMouseEvent *e)
{
    d->toolProxy->mouseDoubleClickEvent( e, d->zoomHandler.viewToDocument( widgetToView( e->pos() + d->documentOffset ) ) );
}

void KarbonCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    d->toolProxy->mouseReleaseEvent( e, d->zoomHandler.viewToDocument( widgetToView( e->pos() + d->documentOffset ) ) );
}

void KarbonCanvas::keyReleaseEvent (QKeyEvent *e) {
    d->toolProxy->keyReleaseEvent(e);
}

void KarbonCanvas::keyPressEvent (QKeyEvent *e) {
    d->toolProxy->keyPressEvent(e);
}

void KarbonCanvas::tabletEvent( QTabletEvent *e )
{
    d->toolProxy->tabletEvent( e, d->zoomHandler.viewToDocument( widgetToView( e->pos() + d->documentOffset ) ) );
}

void KarbonCanvas::wheelEvent( QWheelEvent *e )
{
    d->toolProxy->wheelEvent( e, d->zoomHandler.viewToDocument( widgetToView( e->pos() + d->documentOffset ) ) );
}

QVariant KarbonCanvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    return d->toolProxy->inputMethodQuery(query, *(viewConverter()) );
}

void KarbonCanvas::inputMethodEvent(QInputMethodEvent *event)
{
    d->toolProxy->inputMethodEvent(event);
}

void KarbonCanvas::resizeEvent( QResizeEvent * )
{
    adjustOrigin();
}

void KarbonCanvas::gridSize(double *horizontal, double *vertical) const {
    if( horizontal )
        *horizontal = d->part->gridData().gridX();
    if( vertical )
        *vertical = d->part->gridData().gridY();
}

bool KarbonCanvas::snapToGrid() const {
    return d->part->gridData().snapToGrid();
}

void KarbonCanvas::addCommand(QUndoCommand *command) {

    d->part->addCommand(command);
    adjustOrigin();
}

void KarbonCanvas::updateCanvas(const QRectF& rc) {
    QRect clipRect( viewToWidget( d->zoomHandler.documentToView(rc).toRect() ) );
    clipRect.adjust(-2, -2, 2, 2); // grow for anti-aliasing
    clipRect.moveTopLeft( clipRect.topLeft() - d->documentOffset);
    update(clipRect);
}

void KarbonCanvas::adjustOrigin()
{
    //kDebug(38000) <<"KarbonCanvas::adjustOrigin";

    // calculate the zoomed document bounding rect
    QRect documentRect = d->zoomHandler.documentToView( documentViewRect() ).toRect();

    // save the old origin to see if it has changed
    QPoint oldOrigin = d->origin;

    // set the origin to the zoom document rect origin
    d->origin = -documentRect.topLeft();

    // the document bounding rect is always centered on the virtual canvas
    // if there are margins left around the zoomed document rect then
    // distribute them evenly on both sides
    int widthDiff = size().width() - documentRect.width();
    if( widthDiff > 0 )
        d->origin.rx() += widthDiff;
    int heightDiff = size().height() - documentRect.height();
    if( heightDiff > 0 )
        d->origin.ry() += heightDiff;

    // check if the origin has changed and emit signal if it has
    if( d->origin != oldOrigin )
        emit documentOriginChanged( d->origin );
}

void KarbonCanvas::setDocumentOffset(const QPoint &offset) {
    d->documentOffset = offset;
}

void KarbonCanvas::enableOutlineMode( bool on )
{
    if( on )
        new KarbonOutlinePaintingStrategy( d->shapeManager );
    else
    {
        d->shapeManager->setPaintingStrategy( new KoShapeManagerPaintingStrategy( d->shapeManager ) );
    }
}

QPoint KarbonCanvas::widgetToView( const QPoint& p ) const {
    return p - d->origin;
}

QRect KarbonCanvas::widgetToView( const QRect& r ) const {
    return r.translated( - d->origin );
}

QPoint KarbonCanvas::viewToWidget( const QPoint& p ) const {
    return p + d->origin;
}

QRect KarbonCanvas::viewToWidget( const QRect& r ) const {
    return r.translated( d->origin );
}

KoUnit KarbonCanvas::unit() const {
    return d->document->unit();
}

QPoint KarbonCanvas::documentOrigin() const
{
    return d->origin;
}

void KarbonCanvas::setShowPageMargins( bool on )
{
    d->showMargins = on;
}

void KarbonCanvas::setDocumentViewMargin( int margin )
{
    d->viewMargin = margin;
}

int KarbonCanvas::documentViewMargin() const
{
    return d->viewMargin;
}

QRectF KarbonCanvas::documentViewRect()
{
    //kDebug(38000) <<"KarbonCanvas::documentViewRect";

    // save the old content rect for comparing
    QRectF oldContentRect = d->contentRect;
    // calculate the actual content rect
    d->contentRect = d->document->boundingRect();

    QRectF viewRect = d->contentRect.adjusted( -d->viewMargin, -d->viewMargin, d->viewMargin, d->viewMargin );

    // check if the content rect has changed and emit signal if it has
    if( oldContentRect != d->contentRect )
        emit documentViewRectChanged( viewRect );

    return viewRect;
}

void KarbonCanvas::updateInputMethodInfo() {
    updateMicroFocus();
}

#include "KarbonCanvas.moc"

