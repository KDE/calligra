/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "kis_mdi_mainwindow.h"

#include <QSettings>
#include <QMdiArea>

#include <KoDocumentManager.h>

KisMdiMainWindow::KisMdiMainWindow(KoDocumentManager *documentManager, QWidget *parent)
    : QMainWindow(parent)
    , m_mdiArea(new QMdiArea(this))
    , m_documentManager(documentManager)
{
    QSettings settings;
    m_mdiArea->setViewMode((QMdiArea::ViewMode)settings.value("qrita/mainwindowstyle", QMdiArea::SubWindowView).toInt());
    setCentralWidget(m_mdiArea);

    // XXX: Move to KoMainWindowBase since it's the same for all Calligra apps
    setDocumentMode(true);
    setDockOptions(QMainWindow::AnimatedDocks |
                   QMainWindow::AllowNestedDocks |
                   QMainWindow::AllowTabbedDocks);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setTabShape(QTabWidget::Rounded);

    restoreGeometry(settings.value("qrita/mainwindow/geometry").toByteArray());
    restoreState(settings.value("qrita/mainwindow/windowState").toByteArray());

    show();
}

KisMdiMainWindow::~KisMdiMainWindow()
{
}

void KisMdiMainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("qrita/mainwindow/geometry", saveState(0));
    settings.setValue("qrita/mainwindow/windowState", saveGeometry());
    QMainWindow::closeEvent(event);
}
