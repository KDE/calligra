/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "MainWindow.h"
#include "CanvasController.h"
#include "calligra_mobile_global.h"

#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
{
    qmlRegisterType<CanvasController>("CalligraMobile", 1, 0, "CanvasController");
    m_view = new QDeclarativeView(this);

    QSettings settings;
    QStringList recentFilesList = settings.value("recentFiles").toStringList();
    qDebug() << recentFilesList;
    m_view->rootContext()->setContextProperty("recentFilesModel", QVariant::fromValue(recentFilesList));
    m_view->setSource(QUrl::fromLocalFile(CalligraMobile::Global::installPrefix()
                        + "/share/calligra-mobile/qml/HomeScreen.qml"));
    setCentralWidget(m_view);
}

void MainWindow::openFile(const QString &path)
{
    if (path.isEmpty())
        return;

    QObject *object = m_view->rootObject();
    QMetaObject::invokeMethod(object, "openDocument", Q_ARG(QVariant, QVariant(path)));
}

MainWindow::~MainWindow()
{
}

#include "MainWindow.moc"
