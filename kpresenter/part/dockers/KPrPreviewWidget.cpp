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
#include <KoShapePainter.h>
#include <KoShapeContainer.h>
#include <KoPAMasterPage.h>

#include "KPrPage.h"
#include "pageeffects/KPrPageEffect.h"
#include "pageeffects/KPrPageEffectRunner.h"

#include <kdebug.h>

KPrPreviewWidget::KPrPreviewWidget( QWidget* parent )
: QWidget( parent ), m_pageEffect(0), m_pageEffectRunner(0)
{
    connect( &m_timeLine, SIGNAL( valueChanged( qreal ) ), this, SLOT( animate() ) );
}

KPrPreviewWidget::~KPrPreviewWidget()
{
}

void KPrPreviewWidget::paintEvent( QPaintEvent *event )
{
    QPainter p(this);
    if(m_pageEffectRunner) {
        m_pageEffectRunner->paint(p);
    }
    else {
        p.drawLine(rect().topLeft(), rect().bottomRight());
        p.drawLine(rect().topRight(), rect().bottomLeft());
    }
}

void KPrPreviewWidget::setPageEffect( KPrPageEffect* pageEffect, KPrPage* page )
{
    if(m_pageEffect)
        delete m_pageEffect;
    if(m_pageEffectRunner)
        delete m_pageEffectRunner;

    m_pageEffect = pageEffect;

    if(m_pageEffect && page) {
        QPixmap oldPage( size() );
        QPainter p(&oldPage);
        p.fillRect( rect(), QBrush(Qt::black) );

        QImage pageImage( size(), QImage::Format_RGB32 );
        pageImage.fill( QColor( Qt::white ).rgb() );

        QList<KoShape*> shapes;
        if(page->masterPage())
            shapes.append(page->masterPage());

        shapes.append(page);
        KoShapePainter painter;
        painter.setShapes( shapes );
        painter.paintShapes( pageImage );

        QPixmap newPage( size() );
        QPainter p2(&newPage);
        p2.drawImage(rect(), pageImage);

        m_pageEffectRunner = new KPrPageEffectRunner( oldPage, newPage, this, m_pageEffect );
    }
    else {
        m_pageEffect = 0;
        m_pageEffectRunner = 0;
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

#include "KPrPreviewWidget.moc"
