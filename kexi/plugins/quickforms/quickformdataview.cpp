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

#include "quickformdataview.h"
#include "quickformview.h"

#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <KDebug>
#include <QDomElement>
#include <qgraphicsitem.h>

QuickFormDataView::QuickFormDataView(QWidget* parent): KexiView(parent)
{
    //m_view = new QDeclarativeView(this);
    m_view = new QuickFormView(this);
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(5,5,100,100);
    m_view->setScene(m_scene);
    
    m_scene->setBackgroundBrush(palette().brush(QPalette::Dark));
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    
    m_view->setBackgroundBrush(palette().brush(QPalette::Light));
    setViewWidget(m_view);
    
    m_engine = new QDeclarativeEngine;    
}

QuickFormDataView::~QuickFormDataView()
{

}

void QuickFormDataView::setDefinition(const QString& def)
{
    kDebug() << def;
    
    QDomDocument doc;
    doc.setContent(def);
        
    QDomElement root = doc.documentElement();
    QDomElement qf = root.firstChildElement("quickform");
    kDebug() << root.text();

    QDeclarativeComponent *c = new QDeclarativeComponent(m_engine);
    kDebug() << "Setting definition";
    c->setData(root.text().toLocal8Bit(), QUrl());
    
    kDebug() << "Creating object";

    m_object = qobject_cast<QGraphicsObject*>(c->create());
    
    kDebug() << "Adding object to scene";
    if (m_object) {
        m_scene->addItem(m_object);
    }
}
