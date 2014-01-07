/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  Adam Pigg <email>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "quickformview.h"
#include <QResizeEvent>
#include <qgraphicsitem.h>
#include <QDeclarativeItem>
#include <kdebug.h>

QuickFormView::QuickFormView(QWidget* parent) : QGraphicsView(parent)
{
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
}

void QuickFormView::resizeEvent(QResizeEvent* event)
{
    if (scene()) {
        kDebug() << "Resizing Scene to " << 0 << 0 << event->size().width() << event->size().height();
        scene()->setSceneRect(0,0,event->size().width(), event->size().height());
        
        kDebug() << scene()->items().count();
        
        QDeclarativeItem *rootItem = qobject_cast< QDeclarativeItem* >(items().at(0));
        rootItem->setWidth(event->size().width());
        rootItem->setHeight(event->size().height());
    }

}
