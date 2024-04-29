/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    KPrPresenterViewWidget(KPrViewModePresentation *viewMode, const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent = nullptr);
    ~KPrPresenterViewWidget() override;
    void setActivePage(KoPAPageBase *page);
    void setActivePage(int pageIndex);
    void updateWidget(const QSize &widgetSize, const QSize &canvasSize);

private Q_SLOTS:
    void showSlideThumbnails(bool show);
    void requestPreviousSlide();
    void requestNextSlide();
    void requestChangePage(int index, bool enableMainView);

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
