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

#include "KPrPresenterViewBaseInterface.h"

#include <QFrame>

class QLabel;
class QTextEdit;

class KoPACanvas;
class KoPAPageBase;

/**
 * KPrPresenterViewInterface
 * This widget is the main interface, this widget shows current slide, next slide
 * and the presenter's notes
 */
class KPrPresenterViewInterface : public KPrPresenterViewBaseInterface
{
    Q_OBJECT
public:
    KPrPresenterViewInterface( const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent = 0 );

    void setPreviewSize( const QSize &size );

public Q_SLOTS:
    /// reimplemented
    void setActivePage( int pageIndex ) override;

private:
    KoPACanvas *m_canvas;
    QLabel *m_currentSlideLabel;
    QLabel *m_nextSlideLabel;
    QLabel *m_nextSlidePreview;
    QTextEdit *m_notesTextEdit;
    QSize m_previewSize;
};

#endif

