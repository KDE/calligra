/* This file is part of the KDE project
 * Copyright (C) 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
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
#include "KPrPresentationBlackStrategy.h"

#include <QKeyEvent>
#include <QApplication>

#include <kiconloader.h>

#include <KoPACanvas.h>

#include "KPrPresentationTool.h"
#include "KPrPresentationBlackWidget.h"

KPrPresentationBlackStrategy::KPrPresentationBlackStrategy( KPrPresentationTool * tool )
: KPrPresentationStrategyBase( tool )
{
    m_widget = new KPrPresentationBlackWidget( canvas() );
    // TODO
    QString str("kpresenter");
    KIconLoader kicon(str);
    str.clear();
    str.append("black.png");
    QPixmap pix(kicon.loadIcon(str, kicon.Small));
    float factor = 1.0;
    pix = pix.scaledToHeight(pix.height()*factor);
    pix = pix.scaledToWidth(pix.width()*factor);
    QCursor cur = QCursor(pix);
    QApplication::setOverrideCursor(cur);

    setToolWidgetParent( m_widget );
    m_widget->show();
    m_widget->installEventFilter( m_tool );
}

KPrPresentationBlackStrategy::~KPrPresentationBlackStrategy()
{
    setToolWidgetParent( canvas()->canvasWidget() );
    QApplication::restoreOverrideCursor();
}

bool KPrPresentationBlackStrategy::keyPressEvent( QKeyEvent * event )
{
    bool handled = true;
    switch ( event->key() )
    {
        case Qt::Key_Escape:
            activateDefaultStrategy();
            break;
        case Qt::Key_H:
            handled = false;
            break;
    }
    return handled;
}
