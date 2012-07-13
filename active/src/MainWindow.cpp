/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
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
#include "CACanvasController.h"
#include "CADocumentInfo.h"
#include "CADocumentController.h"
#include "calligra_active_global.h"

#include <KDE/KGlobal>
#include <KDE/KStandardDirs>
#include <KDE/KDebug>

#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QSettings>
#include <QTimer>
#include <QFileDialog>
#include <QDesktopServices>

MainWindow::MainWindow (QWidget* parent)
{
    qmlRegisterType<CACanvasController> ("CalligraActive", 1, 0, "CanvasController");
    qmlRegisterType<CADocumentInfo> ("CalligraActive", 1, 0, "CADocumentInfo");
    qmlRegisterType<CADocumentController> ("CalligraActive", 1, 0, "CADocumentController");
    qmlRegisterInterface<KoCanvasController> ("KoCanvasController");

    m_view = new QDeclarativeView (this);

    kdeclarative.setDeclarativeEngine(m_view->engine());
    kdeclarative.initialize();
    kdeclarative.setupBindings();

    QList<QObject*> recentFiles;
    QList<QObject*> recentTextDocs;
    QList<QObject*> recentSpreadsheets;
    QList<QObject*> recentPresentations;
    QSettings settings;
    foreach (QString string, settings.value ("recentFiles").toStringList()) {
        CADocumentInfo* docInfo = CADocumentInfo::fromStringList (string.split (";"));
        recentFiles.append (docInfo);
        switch (docInfo->type()) {
        case CADocumentInfo::TextDocument:
            recentTextDocs.append (docInfo);
            break;
        case CADocumentInfo::Spreadsheet:
            recentSpreadsheets.append (docInfo);
            break;
        case CADocumentInfo::Presentation:
            recentPresentations.append (docInfo);
            break;
        }
    }

    foreach (const QString & importPath, KGlobal::dirs()->findDirs ("module", "imports")) {
        m_view->engine()->addImportPath (importPath);
    }

    m_view->rootContext()->setContextProperty ("mainwindow", this);
    loadMetadataModel();

    m_view->setSource (QUrl::fromLocalFile (CalligraActive::Global::installPrefix()
                                            + "/share/calligraactive/qml/HomeScreen.qml"));
    m_view->setResizeMode (QDeclarativeView::SizeRootObjectToView);

    setCentralWidget (m_view);
    connect (m_view, SIGNAL (sceneResized (QSize)), SLOT (adjustWindowSize (QSize)));
    resize (800, 600);

    if (!documentPath.isEmpty()) {
        QTimer::singleShot(1000, this, SLOT(checkForAndOpenDocument()));
    }
}

void MainWindow::openFile (const QString& path)
{
    documentPath = path;
}

void MainWindow::adjustWindowSize (QSize size)
{
    resize (size);
}

void MainWindow::openFileDialog()
{
    const QString path = QFileDialog::getOpenFileName (this, "Open File", QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation));
    if (!path.isEmpty()) {
        QObject* object = m_view->rootObject();
        Q_ASSERT (object);
        QMetaObject::invokeMethod (object, "openDocument", Q_ARG (QVariant, QVariant (path)));
    }

}

void MainWindow::loadMetadataModel()
{
    if (!m_view) {
        return;
    }
    QDeclarativeComponent component(m_view->engine());
    component.setData("import org.kde.metadatamodels 0.1\nMetadataModel { sortOrder: Qt.AscendingOrder }\n", QUrl());

    if (!component.isError()) {
        m_view->rootContext()->setContextProperty("metadataInternalModel", component.create());
    } else {
        kDebug() << "Plasma Active Metadata Models are not installed, using built in model";
        m_view->rootContext()->setContextProperty("metadataInternalModel", false);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::checkForAndOpenDocument()
{
    QObject* object = m_view->rootObject();
    QMetaObject::invokeMethod (object, "openDocument", Q_ARG (QVariant, QVariant (documentPath)));
}

#include "MainWindow.moc"
