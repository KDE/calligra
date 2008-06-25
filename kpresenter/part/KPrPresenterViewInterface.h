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

class QLabel;
class QTextEdit;
class QTimer;
class QTime;

class KoPACanvas;
class KoPADocument;
class KoPAPageBase;
class KoPAPageThumbnailModel;

class KPrPresenterViewBaseInterface : public QWidget
{
    Q_OBJECT
public:
    KPrPresenterViewBaseInterface( KoPADocument *document, QWidget *parent = 0 );

public slots:
    virtual void setActivePage( KoPAPageBase *page );

signals:
    void activeSlideChanged( KoPAPageBase *page );

protected:
    KoPADocument *m_document;
    KoPAPageBase *m_activePage;
};

class KPrPresenterViewInterface : public KPrPresenterViewBaseInterface
{
    Q_OBJECT
public:
    KPrPresenterViewInterface( KoPADocument *document, KoPACanvas *canvas, QWidget *parent = 0 );

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
    KPrPresenterViewSlidesInterface( KoPADocument *document, QWidget *parent = 0 );

public slots:
    virtual void setActivePage( KoPAPageBase *page );

private:
    KoPAPageThumbnailModel *m_thumbnailModel;
};

class KPrPresenterViewToolWidget : public QFrame
{
    Q_OBJECT
public:
    KPrPresenterViewToolWidget( QWidget *parent = 0 );

signals:
    void slideThumbnailsToggled( bool toggle );
    void previousSlideClicked();
    void nextSlideClicked();

private slots:
    void updateClock();

private:
    QLabel *m_clockLabel;
    QLabel *m_timerLabel;

    QTimer *m_clockTimer;

    int hour;
    int min;
    int sec;
};

#endif

