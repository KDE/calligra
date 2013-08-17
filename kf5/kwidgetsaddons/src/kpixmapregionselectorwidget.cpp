/* This file is part of the KDE libraries
    Copyright (C) 2004 Antonio Larrosa <larrosa@kde.org

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

/* NOTE: There are two copies of this .h and the .cpp file, with subtle differences.
 * One copy is in kdelibs/kdeui, and the other copy is in kdepim/libkdepim
 * This is because kdepim has to remain backwards compatible.  Any changes
 * to either file should be made to the other.
 */

#include "kpixmapregionselectorwidget.h"
#include <QAction>
#include <QPainter>
#include <QColor>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QMenu>
#include <QRubberBand>
#include <QCursor>
#include <QApplication>
#include <QMouseEvent>

class KPixmapRegionSelectorWidget::Private
{
public:
    Private(KPixmapRegionSelectorWidget *q): q(q) {}

    KPixmapRegionSelectorWidget *q;

    /**
    * Recalculates the pixmap that is shown based on the current selected area,
    * the original image, etc.
    */
    void updatePixmap();

    QRect calcSelectionRectangle( const QPoint &startPoint, const QPoint & endPoint );

    enum CursorState { None=0, Resizing, Moving };
    CursorState m_state;

    QPixmap m_unzoomedPixmap;
    QPixmap m_originalPixmap;
    QPixmap m_linedPixmap;
    QRect   m_selectedRegion;
    QLabel *m_label;

    QPoint m_tempFirstClick;
    double m_forcedAspectRatio;

    int m_maxWidth, m_maxHeight;
    double m_zoomFactor;

    QRubberBand *m_rubberBand;
};

KPixmapRegionSelectorWidget::KPixmapRegionSelectorWidget( QWidget *parent)
    : QWidget( parent ), d(new Private(this))
{
    QHBoxLayout * hboxLayout=new QHBoxLayout( this );

    hboxLayout->addStretch();
    QVBoxLayout * vboxLayout=new QVBoxLayout();
    hboxLayout->addItem(vboxLayout);

    vboxLayout->addStretch();
    d->m_label = new QLabel(this);
    d->m_label->setAttribute(Qt::WA_NoSystemBackground,true);//setBackgroundMode( Qt::NoBackground );
    d->m_label->installEventFilter( this );

    vboxLayout->addWidget(d->m_label);
    vboxLayout->addStretch();

    hboxLayout->addStretch();

    d->m_forcedAspectRatio=0;

    d->m_zoomFactor=1.0;
    d->m_rubberBand = new QRubberBand(QRubberBand::Rectangle, d->m_label);
    d->m_rubberBand->hide();
}

KPixmapRegionSelectorWidget::~KPixmapRegionSelectorWidget()
{
    delete d;
}

QPixmap KPixmapRegionSelectorWidget::pixmap() const
{
    return d->m_unzoomedPixmap;
}

void KPixmapRegionSelectorWidget::setPixmap( const QPixmap &pixmap )
{
    Q_ASSERT(!pixmap.isNull()); //This class isn't designed to deal with null pixmaps.
    d->m_originalPixmap = pixmap;
    d->m_unzoomedPixmap = pixmap;
    d->m_label->setPixmap( pixmap );
    resetSelection();
}

void KPixmapRegionSelectorWidget::resetSelection()
{
    d->m_selectedRegion = d->m_originalPixmap.rect();
    d->m_rubberBand->hide();
    d->updatePixmap();
}

QRect KPixmapRegionSelectorWidget::selectedRegion() const
{
    return d->m_selectedRegion;
}

void KPixmapRegionSelectorWidget::setSelectedRegion(const QRect &rect)
{
    if (!rect.isValid()) resetSelection();
    else
    {
        d->m_selectedRegion=rect;
        d->updatePixmap();
    }
}

void KPixmapRegionSelectorWidget::Private::updatePixmap()
{
    Q_ASSERT(!m_originalPixmap.isNull());
    if (m_originalPixmap.isNull()) {
        m_label->setPixmap(m_originalPixmap);
        return;
    }
    if (m_selectedRegion.width()>m_originalPixmap.width()) {
        m_selectedRegion.setWidth( m_originalPixmap.width() );
    }
    if (m_selectedRegion.height()>m_originalPixmap.height()) {
        m_selectedRegion.setHeight( m_originalPixmap.height() );
    }

    QPainter painter;
    if (m_linedPixmap.isNull()) {
        m_linedPixmap = m_originalPixmap;
        QPainter p(&m_linedPixmap);
        p.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        p.fillRect(m_linedPixmap.rect(), QColor(0, 0, 0, 100));
    }

    QPixmap pixmap = m_linedPixmap;
    painter.begin(&pixmap);
    painter.drawPixmap( m_selectedRegion.topLeft(),
            m_originalPixmap, m_selectedRegion );


    painter.end();

    m_label->setPixmap(pixmap);

    qApp->sendPostedEvents(0,QEvent::LayoutRequest);

    if (m_selectedRegion == m_originalPixmap.rect()) {//d->m_label->rect()) //### CHECK!
        m_rubberBand->hide();
    } else {
        m_rubberBand->setGeometry(QRect(m_selectedRegion.topLeft(),
                                        m_selectedRegion.size()));

/*        m_rubberBand->setGeometry(QRect(m_label -> mapToGlobal(m_selectedRegion.topLeft()),
                                        m_selectedRegion.size()));
*/
        if (m_state!=None) m_rubberBand->show();
    }
}


QMenu *KPixmapRegionSelectorWidget::createPopupMenu()
{
    QMenu *popup=new QMenu(this );
    popup->setObjectName(QStringLiteral("PixmapRegionSelectorPopup"));
    popup->addSection(tr("Image Operations"));

    popup->addAction(QIcon::fromTheme( QStringLiteral( "object-rotate-right" ) ), tr("&Rotate Clockwise"),
                     this, SLOT(rotateClockwise()));
    popup->addAction(QIcon::fromTheme( QStringLiteral( "object-rotate-left" ) ), tr("Rotate &Counterclockwise"),
                     this, SLOT(rotateCounterclockwise()));

/*
   I wonder if it would be appropriate to have here an "Open with..." option to
   edit the image (antlarr)
*/
    return popup;
}

void KPixmapRegionSelectorWidget::rotate(RotateDirection direction)
{
    int w=d->m_originalPixmap.width();
    int h=d->m_originalPixmap.height();
    QImage img=d->m_unzoomedPixmap.toImage();
    if(direction == Rotate90)
        img = img.transformed(QTransform().rotate(90.0));
    else if(direction == Rotate180)
        img = img.transformed(QTransform().rotate(180.0));
    else
        img = img.transformed(QTransform().rotate(270.0));

    d->m_unzoomedPixmap=QPixmap::fromImage(img);

    img=d->m_originalPixmap.toImage();
    if(direction == Rotate90)
        img = img.transformed(QTransform().rotate(90.0));
    else if(direction == Rotate180)
        img = img.transformed(QTransform().rotate(180.0));
    else
        img = img.transformed(QTransform().rotate(270.0));

    d->m_originalPixmap=QPixmap::fromImage(img);

    d->m_linedPixmap=QPixmap();

    if (d->m_forcedAspectRatio>0 && d->m_forcedAspectRatio!=1)
        resetSelection();
    else
    {
        switch (direction)
        {
            case ( Rotate90 ):
                {
                int x=h-d->m_selectedRegion.y()-d->m_selectedRegion.height();
                int y=d->m_selectedRegion.x();
                d->m_selectedRegion.setRect(x, y, d->m_selectedRegion.height(), d->m_selectedRegion.width() );
                d->updatePixmap();
    //              qApp->sendPostedEvents(0,QEvent::LayoutRequest);
    //              updatePixmap();

                } break;
            case ( Rotate270 ):
                {
                int x=d->m_selectedRegion.y();
                int y=w-d->m_selectedRegion.x()-d->m_selectedRegion.width();
                d->m_selectedRegion.setRect(x, y, d->m_selectedRegion.height(), d->m_selectedRegion.width() );
                d->updatePixmap();
    //              qApp->sendPostedEvents(0,QEvent::LayoutRequest);
    //              updatePixmap();
                } break;
            default: resetSelection();
        }
    }

   emit pixmapRotated();
}

void KPixmapRegionSelectorWidget::rotateClockwise()
{
   rotate(Rotate90);
}

void KPixmapRegionSelectorWidget::rotateCounterclockwise()
{
   rotate(Rotate270);
}

bool KPixmapRegionSelectorWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if ( ev->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent *mev= (QMouseEvent *)(ev);
        //qDebug() << QString("click at  %1,%2").arg( mev->x() ).arg( mev->y() );

        if ( mev->button() == Qt::RightButton )
        {
            QMenu *popup = createPopupMenu( );
            popup->exec( mev->globalPos() );
            delete popup;
            return true;
        }

        QCursor cursor;

        if ( d->m_selectedRegion.contains( mev->pos() )
            && d->m_selectedRegion!=d->m_originalPixmap.rect() )
        {
            d->m_state=Private::Moving;
            cursor.setShape( Qt::SizeAllCursor );
            d->m_rubberBand->show();
        }
        else
        {
            d->m_state=Private::Resizing;
            cursor.setShape( Qt::CrossCursor );
        }
        QApplication::setOverrideCursor(cursor);

        d->m_tempFirstClick=mev->pos();


        return true;
    }

    if ( ev->type() == QEvent::MouseMove )
    {
        QMouseEvent *mev= (QMouseEvent *)(ev);

        //qDebug() << QString("move to  %1,%2").arg( mev->x() ).arg( mev->y() );

        if ( d->m_state == Private::Resizing )
        {
            setSelectedRegion (
                d->calcSelectionRectangle( d->m_tempFirstClick, mev->pos() ) );
        }
        else if (d->m_state == Private::Moving )
        {
            int mevx = mev->x();
            int mevy = mev->y();
            bool mouseOutside=false;
            if ( mevx < 0 )
            {
            d->m_selectedRegion.translate(-d->m_selectedRegion.x(),0);
            mouseOutside=true;
            }
            else if ( mevx > d->m_originalPixmap.width() )
            {
            d->m_selectedRegion.translate(d->m_originalPixmap.width()-d->m_selectedRegion.width()-d->m_selectedRegion.x(),0);
            mouseOutside=true;
            }
            if ( mevy < 0 )
            {
            d->m_selectedRegion.translate(0,-d->m_selectedRegion.y());
            mouseOutside=true;
            }
            else if ( mevy > d->m_originalPixmap.height() )
            {
            d->m_selectedRegion.translate(0,d->m_originalPixmap.height()-d->m_selectedRegion.height()-d->m_selectedRegion.y());
            mouseOutside=true;
            }
            if (mouseOutside) { d->updatePixmap(); return true; };

            d->m_selectedRegion.translate( mev->x()-d->m_tempFirstClick.x(),
                                    mev->y()-d->m_tempFirstClick.y() );

            // Check that the region has not fallen outside the image
            if (d->m_selectedRegion.x() < 0)
                d->m_selectedRegion.translate(-d->m_selectedRegion.x(),0);
            else if (d->m_selectedRegion.right() > d->m_originalPixmap.width())
                d->m_selectedRegion.translate(-(d->m_selectedRegion.right()-d->m_originalPixmap.width()),0);

            if (d->m_selectedRegion.y() < 0)
                d->m_selectedRegion.translate(0,-d->m_selectedRegion.y());
            else if (d->m_selectedRegion.bottom() > d->m_originalPixmap.height())
                d->m_selectedRegion.translate(0,-(d->m_selectedRegion.bottom()-d->m_originalPixmap.height()));

            d->m_tempFirstClick=mev->pos();
            d->updatePixmap();
        }
        return true;
    }

    if ( ev->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent *mev= (QMouseEvent *)(ev);

        if ( d->m_state == Private::Resizing && mev->pos() == d->m_tempFirstClick)
            resetSelection();

        d->m_state=Private::None;
        QApplication::restoreOverrideCursor();
        d->m_rubberBand->hide();
        return true;
    }

    QWidget::eventFilter(obj, ev);
    return false;
}

QRect KPixmapRegionSelectorWidget::Private::calcSelectionRectangle( const QPoint & startPoint, const QPoint & _endPoint )
{
    QPoint endPoint = _endPoint;
    if ( endPoint.x() < 0 ) {
        endPoint.setX(0);
    } else if ( endPoint.x() > m_originalPixmap.width() ) {
        endPoint.setX(m_originalPixmap.width());
    }
    if ( endPoint.y() < 0 ) {
        endPoint.setY(0);
    } else if ( endPoint.y() > m_originalPixmap.height() ) {
        endPoint.setY(m_originalPixmap.height());
    }
    int w=abs(startPoint.x()-endPoint.x());
    int h=abs(startPoint.y()-endPoint.y());

    if (m_forcedAspectRatio>0)
    {
        double aspectRatio=w/double(h);

        if (aspectRatio>m_forcedAspectRatio)
            h=(int)(w/m_forcedAspectRatio);
        else
            w=(int)(h*m_forcedAspectRatio);
    }

    int x,y;
    if ( startPoint.x() < endPoint.x() )
        x=startPoint.x();
    else
        x=startPoint.x()-w;

    if ( startPoint.y() < endPoint.y() )
        y=startPoint.y();
    else
        y=startPoint.y()-h;

    if (x<0)
    {
        w+=x;
        x=0;
        h=(int)(w/m_forcedAspectRatio);

        if ( startPoint.y() > endPoint.y() )
            y=startPoint.y()-h;
    }
    else if (x+w>m_originalPixmap.width())
    {
        w=m_originalPixmap.width()-x;
        h=(int)(w/m_forcedAspectRatio);

        if ( startPoint.y() > endPoint.y() )
            y=startPoint.y()-h;
    }

    if (y<0)
    {
        h+=y;
        y=0;
        w=(int)(h*m_forcedAspectRatio);

        if ( startPoint.x() > endPoint.x() )
            x=startPoint.x()-w;
    }
    else if (y+h>m_originalPixmap.height())
    {
        h=m_originalPixmap.height()-y;
        w=(int)(h*m_forcedAspectRatio);

        if ( startPoint.x() > endPoint.x() )
            x=startPoint.x()-w;
    }

   return QRect(x,y,w,h);
}

QRect KPixmapRegionSelectorWidget::unzoomedSelectedRegion() const
{
    return QRect((int)(d->m_selectedRegion.x()/d->m_zoomFactor),
                 (int)(d->m_selectedRegion.y()/d->m_zoomFactor),
                 (int)(d->m_selectedRegion.width()/d->m_zoomFactor),
                 (int)(d->m_selectedRegion.height()/d->m_zoomFactor));
}

QImage KPixmapRegionSelectorWidget::selectedImage() const
{
    QImage origImage=d->m_unzoomedPixmap.toImage();
    return origImage.copy(unzoomedSelectedRegion());
}

void KPixmapRegionSelectorWidget::setSelectionAspectRatio(int width, int height)
{
    d->m_forcedAspectRatio=width/double(height);
}

void KPixmapRegionSelectorWidget::setFreeSelectionAspectRatio()
{
    d->m_forcedAspectRatio=0;
}

void KPixmapRegionSelectorWidget::setMaximumWidgetSize(int width, int height)
{
    d->m_maxWidth=width;
    d->m_maxHeight=height;

    if (d->m_selectedRegion == d->m_originalPixmap.rect()) d->m_selectedRegion=QRect();
    d->m_originalPixmap=d->m_unzoomedPixmap;

    //   qDebug() << QString(" original Pixmap :") << d->m_originalPixmap.rect();
    //   qDebug() << QString(" unzoomed Pixmap : %1 x %2 ").arg(d->m_unzoomedPixmap.width()).arg(d->m_unzoomedPixmap.height());

    if ( !d->m_originalPixmap.isNull() &&
        ( d->m_originalPixmap.width() > d->m_maxWidth ||
            d->m_originalPixmap.height() > d->m_maxHeight ) )
    {
            /* We have to resize the pixmap to get it complete on the screen */
            QImage image=d->m_originalPixmap.toImage();
            d->m_originalPixmap=QPixmap::fromImage( image.scaled( width, height, Qt::KeepAspectRatio,Qt::SmoothTransformation ) );
            double oldZoomFactor = d->m_zoomFactor;
            d->m_zoomFactor=d->m_originalPixmap.width()/(double)d->m_unzoomedPixmap.width();

            if (d->m_selectedRegion.isValid())
            {
                d->m_selectedRegion=
                    QRect((int)(d->m_selectedRegion.x()*d->m_zoomFactor/oldZoomFactor),
                            (int)(d->m_selectedRegion.y()*d->m_zoomFactor/oldZoomFactor),
                            (int)(d->m_selectedRegion.width()*d->m_zoomFactor/oldZoomFactor),
                            (int)(d->m_selectedRegion.height()*d->m_zoomFactor/oldZoomFactor) );
            }
    }

    if (!d->m_selectedRegion.isValid()) d->m_selectedRegion = d->m_originalPixmap.rect();

    d->m_linedPixmap=QPixmap();
    d->updatePixmap();
    resize(d->m_label->width(), d->m_label->height());
}

