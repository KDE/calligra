/* This file is part of the KDE project
 * Copyright (C) 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KPRPRESENTERVIEWWIDGET
#define KPRPRESENTERVIEWWIDGET

#include <QWidget>

class QStackedLayout;

class KoPACanvas;
class KoPAPageBase;

class KPrViewModePresentation;

class KPrPresenterViewBaseInterface;
class KPrPresenterViewInterface;
class KPrPresenterViewSlidesInterface;
class KPrPresenterViewToolWidget;

class KPrPresenterViewWidget : public QWidget
{
    Q_OBJECT
public:
    KPrPresenterViewWidget( KPrViewModePresentation *viewMode, const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent=0 );
    ~KPrPresenterViewWidget() override;
    void setActivePage( KoPAPageBase *page );
    void setActivePage( int pageIndex );
    void updateWidget( const QSize &widgetSize, const QSize &canvasSize );

private Q_SLOTS:
    void showSlideThumbnails( bool show );
    void requestPreviousSlide();
    void requestNextSlide();
    void requestChangePage( int index, bool enableMainView );

private:
    KPrViewModePresentation *m_viewMode;
    QList<KoPAPageBase *> m_pages;
    KoPACanvas *m_canvas;

    QStackedLayout *m_stackedLayout;

    KPrPresenterViewInterface *m_mainWidget;
    KPrPresenterViewSlidesInterface *m_slidesWidget;
    KPrPresenterViewBaseInterface *m_activeWidget;
    KPrPresenterViewToolWidget *m_toolWidget;
};

#endif

