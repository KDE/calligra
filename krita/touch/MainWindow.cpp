/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "MainWindow.h"

#include <QDebug>
#include <QDeclarativeView>

#include <KGlobal>
#include <KComponentData>
#include <KStandardDirs>

MainWindow::MainWindow(QWidget* parent )
{
    m_view = new QDeclarativeView (this);
    setCentralWidget(m_view);
    
    KGlobal::mainComponent().dirs()->addResourceType("krita_touch_qml", "data", "kritatouch/qml/");
    
    m_view->setSource(QUrl::fromLocalFile(KGlobal::mainComponent().dirs()->findResource("krita_touch_qml", "MainWindow.qml")));
    m_view->setResizeMode (QDeclarativeView::SizeRootObjectToView);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile (const QString& path)
{
    
}


#include "MainWindow.moc"
