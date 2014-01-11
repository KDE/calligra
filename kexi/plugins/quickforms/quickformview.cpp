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
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDeclarativeContext>

QuickFormView::QuickFormView(QWidget* parent) : m_object(0), m_component(0), QGraphicsView(parent)
{
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0,0,100,100);
    setScene(m_scene);
    
    m_scene->setBackgroundBrush(palette().brush(QPalette::Dark));
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    
    m_engine = new QDeclarativeEngine; 
    
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
}

void QuickFormView::resizeEvent(QResizeEvent* event)
{
    if (scene()) {
        kDebug() << "Resizing Scene to " << 0 << 0 << event->size().width() << event->size().height();
        scene()->setSceneRect(0,0,event->size().width(), event->size().height());
        
        kDebug() << scene()->items().count();
        
        if (m_object) {
            m_object->setWidth(event->size().width());
            m_object->setHeight(event->size().height());
        }
    }
}

void QuickFormView::setDeclarativeComponent(const QByteArray& def)
{
    m_component = new QDeclarativeComponent(m_engine);
    kDebug() << "Setting definition";
    m_component->setData(def, QUrl());
    
    kDebug() << "Creating object";
    
    m_object = qobject_cast<QDeclarativeItem*>(m_component->create());
    
    kDebug() << "Adding object to scene";
    if (m_object) {
        m_scene->addItem(m_object);
    }
}

void QuickFormView::addContextProperty(const QString &n, QObject* obj)
{
    kDebug() << n << obj;
    if (obj) {
        m_engine->rootContext()->setContextProperty(n, obj);
    }
}

