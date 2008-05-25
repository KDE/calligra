/* This file is part of the KDE project
   Copyright (C) 2008 Sven Langkamp <sven.langkamp@gmail.com>

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

#include "KPrPreviewWidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <KoShapePainter.h>
#include <KoShapeContainer.h>
#include <KoPAMasterPage.h>
#include <KoZoomHandler.h>

#include "KPrPage.h"
#include "pageeffects/KPrPageEffect.h"
#include "pageeffects/KPrPageEffectRunner.h"

#include <kdebug.h>

KPrPreviewWidget::KPrPreviewWidget( QWidget* parent )
: QWidget( parent ), m_pageEffect(0), m_pageEffectRunner(0), m_page(0)
{
    connect( &m_timeLine, SIGNAL( valueChanged( qreal ) ), this, SLOT( animate() ) );
}

KPrPreviewWidget::~KPrPreviewWidget()
{
}

void KPrPreviewWidget::paintEvent( QPaintEvent *event )
{
    QPainter p(this);

    if(m_pageEffectRunner && m_timeLine.state() == QTimeLine::Running) {
        m_pageEffectRunner->paint(p);
    }
    else if(m_page && !m_newPage.isNull()) {
        p.drawPixmap(rect().topLeft(), m_newPage);
    }
    else {
        p.drawLine(rect().topLeft(), rect().bottomRight());
        p.drawLine(rect().topRight(), rect().bottomLeft());
    }

    QPen pen(Qt::SolidLine);
    pen.setColor(palette().color(QPalette::Mid));
    p.setPen(pen);
    QRect framerect = rect();
    framerect.setWidth(framerect.width() - 1);
    framerect.setHeight(framerect.height() - 1);
    p.drawRect(framerect);
}

void  KPrPreviewWidget::resizeEvent( QResizeEvent* event )
{
    if(m_page)
        updatePixmaps();

    if(m_pageEffectRunner) {
        m_pageEffectRunner->setOldPage( m_oldPage );
        m_pageEffectRunner->setNewPage( m_newPage );
    }
}

void KPrPreviewWidget::setPageEffect( KPrPageEffect* pageEffect, KPrPage* page )
{
    delete m_pageEffect;
    m_pageEffect = pageEffect;
    delete m_pageEffectRunner;
    m_pageEffectRunner = 0;

    m_page = page;

    if(m_page) {
        updatePixmaps();

        if(m_pageEffect) {
            m_pageEffectRunner = new KPrPageEffectRunner( m_oldPage, m_newPage, this, m_pageEffect );
        }
    }

    update();
}

void KPrPreviewWidget::animate()
{
    if ( m_pageEffectRunner ) {
        m_pageEffectRunner->next( m_timeLine.currentTime() );
    }
}

void KPrPreviewWidget::runPreview()
{
    if(m_pageEffect) {
        m_timeLine.setDuration( m_pageEffect->duration() );
        m_timeLine.setCurrentTime( 0 );
        m_timeLine.start();
    }
}

void KPrPreviewWidget::updatePixmaps()
{
    if(!m_page || !size().isValid() || (size().width() == 0 && size().height() == 0))
        return;

    KoZoomHandler zoomHandler;
    KoPageLayout layout = m_page->pageLayout();
    double hzoom = (double) size().height() / zoomHandler.zoomItY( layout.height );
    double wzoom = (double) size().width() / zoomHandler.zoomItX( layout.width );
    double zoom = qMin( hzoom, wzoom );
    zoomHandler.setZoom( zoom );

    QSize pagesize;
    pagesize.setWidth( zoomHandler.zoomItX( layout.width ) );
    pagesize.setHeight( zoomHandler.zoomItY( layout.height ) );

    if(!pagesize.isValid() || (pagesize.width() == 0 && pagesize.height() == 0))
        return;

    QPixmap pageImage( size() );
    pageImage.fill( QColor( Qt::white ) );

    if(pageImage.isNull())
        return;

    QList<KoShape*> shapes;

    if(m_page->masterPage())
        shapes.append(m_page->masterPage());

    shapes.append( m_page );
    KoShapePainter painter;
    painter.setShapes( shapes );
    QPainter p2( &pageImage );
    p2.translate( ( size().width() - pagesize.width() ) / 2, ( size().height() - pagesize.height() ) / 2 );
    p2.setClipRect( 0, 0, pagesize.width(), pagesize.height() );
    painter.paintShapes( p2, zoomHandler );

    QPixmap oldPage( size() );
    oldPage.fill( QColor(Qt::black) );
    m_oldPage = oldPage;

    m_newPage = pageImage;
}

void KPrPreviewWidget::mousePressEvent( QMouseEvent* event )
{
    event->accept();
    runPreview();
}

#include "KPrPreviewWidget.moc"
