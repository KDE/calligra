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
#include "CACanvasItem.h"
#include "CAPADocumentModel.h"
#include "CATextDocumentModel.h"
#include "CAImageProvider.h"
#include "CAAbstractDocumentHandler.h"
#include "calligra_active_global.h"

#include <libs/main/calligraversion.h>

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
    qmlRegisterType<CACanvasItem> ("CalligraActive", 1, 0, "CACanvasItem");
    qmlRegisterUncreatableType<CAPADocumentModel> ("CalligraActive", 1, 0, "CAPADocumentModel", "Not allowed");
    qmlRegisterUncreatableType<CATextDocumentModel> ("CalligraActive", 1, 0, "CATextDocumentModel", "Not allowed");
    qmlRegisterInterface<KoCanvasController> ("KoCanvasController");
    qmlRegisterUncreatableType<CAAbstractDocumentHandler>("CalligraActive", 1, 0, "CAAbstractDocumentHandler", "Not allowed");

    m_view = new QDeclarativeView (this);

    kdeclarative.setDeclarativeEngine(m_view->engine());
    kdeclarative.initialize();
    kdeclarative.setupBindings();

    QList<QObject*> recentFiles;
    QList<QObject*> recentTextDocs;
    QList<QObject*> recentSpreadsheets;
    QList<QObject*> recentPresentations;
    QSettings settings;
    foreach (const QString &string, settings.value ("recentFiles").toStringList()) {
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

    m_view->rootContext()->setContextProperty("mainwindow", this);
    m_view->rootContext()->setContextProperty("_calligra_version_string", CALLIGRA_VERSION_STRING);
    m_view->engine()->addImageProvider(CAImageProvider::identificationString, CAImageProvider::instance());

    m_view->setSource (QUrl::fromLocalFile (CalligraActive::Global::installPrefix()
                                            + "/share/calligraactive/qml/Doc.qml"));
    m_view->setResizeMode (QDeclarativeView::SizeRootObjectToView);

    connect (m_view, SIGNAL (sceneResized (QSize)), SLOT (adjustWindowSize (QSize)));
    resize (1024, 768);
    setCentralWidget (m_view);

    QTimer::singleShot(0, this, SLOT(checkForAndOpenDocument()));
}

void MainWindow::openFile (const QString& path)
{
    documentPath = path;
    QObject* object = m_view->rootObject();
    if (object) {
        QMetaObject::invokeMethod (object, "hideOpenButton");
    }
}

void MainWindow::adjustWindowSize (QSize size)
{
    resize (size);
}

void MainWindow::openFileDialog()
{
    const QString path = QFileDialog::getOpenFileName (this, i18n("Open File"), QDesktopServices::storageLocation (QDesktopServices::DocumentsLocation));
    if (!path.isEmpty()) {
        QObject* object = m_view->rootObject();
        Q_ASSERT (object);
        QMetaObject::invokeMethod (object, "openDocument", Q_ARG (QVariant, QVariant (path)));
    }

}

MainWindow::~MainWindow()
{
}

void MainWindow::checkForAndOpenDocument()
{
    if (!documentPath.isEmpty()) {
        QObject* object = m_view->rootObject();
        QMetaObject::invokeMethod (object, "openDocument", Q_ARG (QVariant, QVariant (documentPath)));
    }
}

#include "MainWindow.moc"
