/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>

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

#include "KFormulaCanvas.h"
#include "KFormulaPartView.h"
#include "KFormulaPartDocument.h"
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <QPaintEvent>
#include <QPainter>

KFormulaCanvas::KFormulaCanvas( KFormulaPartView* view, KFormulaPartDocument* document, QWidget* parent )
    : KoCanvasBase( document ),
      QWidget( parent ),
      m_view( view )
{
    m_toolProxy = new KoToolProxy(this, this);
    m_shapeManager = new KoShapeManager( this );

    setFocusPolicy( Qt::StrongFocus );
    setBackgroundRole( QPalette::NoRole );
    m_dirtyBuffer = true;
}

KFormulaCanvas::~KFormulaCanvas()
{
    delete m_shapeManager;
}

void KFormulaCanvas::paintEvent( QPaintEvent* e )
{
    Q_UNUSED( e );

/*    QPainter p;

    if( m_dirtyBuffer )
    {
         m_paintBuffer = QPixmap(  );
         p.begin( &m_paintBuffer );
         p.end();
    }

    p.begin( this );
    QRect tmp = e->rect();
    // if the shown area is smaller than the canvas centralize it
    if( width() > m_paintBuffer.width() || height() > m_paintBuffer.height() )
        tmp.translate( width()/2 - m_paintBuffer.width()/2,
                       height()/2 - m_paintBuffer.height()/2 )

    p.drawPixmap( tmp, m_paintBuffer, viewConverter()->viewToDocument( e->rect() ) );
    p.end();*/
}

void KFormulaCanvas::keyPressEvent( QKeyEvent* e )
{
    m_toolProxy->keyPressEvent( e );
    if (! e->isAccepted()) {
        if (e->key() == Qt::Key_Backtab
                || (e->key() == Qt::Key_Tab && (e->modifiers() & Qt::ShiftModifier)))
            focusNextPrevChild(false);
        else if (e->key() == Qt::Key_Tab)
            focusNextPrevChild(true);
    }
}

void KFormulaCanvas::mousePressEvent( QMouseEvent* e )
{
    m_toolProxy->mousePressEvent( e, viewConverter()->viewToDocument( e->pos() ) );
}

void KFormulaCanvas::mouseReleaseEvent( QMouseEvent* e )
{
    m_toolProxy->mouseReleaseEvent( e, viewConverter()->viewToDocument( e->pos() ) );
}

void KFormulaCanvas::mouseDoubleClickEvent( QMouseEvent* e )
{
    m_toolProxy->mouseDoubleClickEvent( e, viewConverter()->viewToDocument( e->pos() ) );
}

void KFormulaCanvas::mouseMoveEvent( QMouseEvent* e )
{
    m_toolProxy->mouseMoveEvent( e, viewConverter()->viewToDocument( e->pos() ) );
}

void KFormulaCanvas::gridSize( qreal* horizontal, qreal* vertical ) const
{
    *horizontal = 10.0;        // set values to a default as KFormula doesn't
    *vertical = 10.0;          // use any grid
}

bool KFormulaCanvas::snapToGrid() const
{
    return false;             // KFormula doesn't use a grid
}

void KFormulaCanvas::addCommand( QUndoCommand *command )
{
    Q_UNUSED( command );
}

KoShapeManager* KFormulaCanvas::shapeManager() const
{
    return m_shapeManager;
}

void KFormulaCanvas::updateCanvas( const QRectF& rc )
{
    update( viewConverter()->documentToView( rc ).toRect() );
}

const KoViewConverter* KFormulaCanvas::viewConverter() const
{
    return m_view->viewConverter();
}

QWidget* KFormulaCanvas::canvasWidget() {
    return this;
}

const QWidget* KFormulaCanvas::canvasWidget() const
{
    return this;
}

KoUnit KFormulaCanvas::unit() const
{
    return KoUnit( KoUnit::Centimeter );  // return this as default
}

KoToolProxy* KFormulaCanvas::toolProxy() const
{
    return m_toolProxy;
}

void KFormulaCanvas::updateInputMethodInfo()
{
    updateMicroFocus();
}

void KFormulaCanvas::setCursor(const QCursor &cursor)
{
    QWidget::setCursor(cursor);
}
