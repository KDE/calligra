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

#ifndef KPRPRESENTERVIEWINTERFACE
#define KPRPRESENTERVIEWINTERFACE

#include <QtGui/QFrame>
#include <QtCore/QTime>

class QLabel;
class QListView;
class QModelIndex;
class QTextEdit;
class QTime;
class QToolButton;

class KoPACanvas;
class KoPADocument;
class KoPAPageBase;
class KoPAPageThumbnailModel;

class KPrPresenterViewBaseInterface : public QWidget
{
    Q_OBJECT
public:
    KPrPresenterViewBaseInterface( const QList<KoPAPageBase *> &pages, QWidget *parent = 0 );

public slots:
    virtual void setActivePage( KoPAPageBase *page );

signals:
    void activeSlideChanged( KoPAPageBase *page );

protected:
    QList<KoPAPageBase *> m_pages;
    KoPAPageBase *m_activePage;
};

class KPrPresenterViewInterface : public KPrPresenterViewBaseInterface
{
    Q_OBJECT
public:
    KPrPresenterViewInterface( const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent = 0 );

    void setPreviewSize( const QSize &size );

public slots:
    virtual void setActivePage( KoPAPageBase *page );

private:
    KoPACanvas *m_canvas;
    QLabel *m_currentSlideLabel;
    QLabel *m_nextSlideLabel;
    QLabel *m_nextSlidePreview;
    QTextEdit *m_notesTextEdit;
    QSize m_previewSize;
};

class KPrPresenterViewSlidesInterface : public KPrPresenterViewBaseInterface
{
    Q_OBJECT
public:
    KPrPresenterViewSlidesInterface( const QList<KoPAPageBase *> &pages, QWidget *parent = 0 );

signals:
    void selectedPageChanged( KoPAPageBase *page, bool doubleClicked );

public slots:
    virtual void setActivePage( KoPAPageBase *page );

private slots:
    void itemClicked( const QModelIndex &index );
    void itemDoubleClicked( const QModelIndex &index );

private:
    KoPAPageThumbnailModel *m_thumbnailModel;
    QListView *m_listView;
};

class KPrPresenterViewToolWidget : public QFrame
{
    Q_OBJECT
public:
    KPrPresenterViewToolWidget( QWidget *parent = 0 );
    void toggleSlideThumbnails( bool toggle );

signals:
    void slideThumbnailsToggled( bool toggle );
    void previousSlideClicked();
    void nextSlideClicked();

private slots:
    void updateClock();

private:
    QToolButton *m_slidesToolButton;
    QLabel *m_clockLabel;
    QLabel *m_timerLabel;

    QTime m_currentTime;
    QTimer *m_clockTimer;
};

#endif

