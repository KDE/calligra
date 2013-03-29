/*
 *  kis_main.cpp - part of Krita
 *
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
 */
#include "kis_application.h"

#include <QPixmap>

#include "KoDocumentManager.h"

#include "kis_view_factory.h"
#include "kis_document_factory.h"
#include "kis_mdi_mainwindow.h"

#include "../data/splash/splash_screen.xpm"

KisApplication::KisApplication(int &argc, char **argv, QObject *parent)
    : QApplication(argc, argv)
    , KoApplicationBase()
    , m_documentManager(new KoDocumentManager(this))
    , m_documentFactory(new KisDocumentFactory(this))
    , m_viewFactory(new KisViewFactory(this))
{
    Q_UNUSED(parent)

    setSplashScreen(QPixmap(splash_screen_xpm));
    // Fix the style, outside KDE only Plastique and Oxygen are good enough
    if (qgetenv("KDE_FULL_SESSION").isEmpty()) {
        setStyle("Plastique");
        setStyle("Oxygen");
    }

    KoApplicationBase::setApplication(this);
    QCoreApplication::setApplicationName("qrita");
    QCoreApplication::setOrganizationName("Krita Foundation");
    QCoreApplication::setOrganizationDomain("krita.org");

    m_documentManager->setViewFactory(m_viewFactory);
    m_documentManager->setDocumentFactory(m_documentFactory);
    m_documentManager->setMainWindowFactory(new KisMdiMainWindowFactory);
}


int KisApplication::start()
{
    return KoApplicationBase::start(m_documentManager);
}

bool KisApplication::notify(QObject *receiver, QEvent *event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception &e) {
        qWarning("Error %s sending event %i to object %s",
                 e.what(), event->type(), qPrintable(receiver->objectName()));
    } catch (...) {
        qWarning("Error <unknown> sending event %i to object %s",
                 event->type(), qPrintable(receiver->objectName()));
    }
    return false;

}
