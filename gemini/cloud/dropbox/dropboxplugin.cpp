/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "dropboxplugin.h"

#include "controller.h"

#include <QQmlContext>
#include <QtQml/QtQml>

void DropboxPlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri)
    Q_ASSERT(uri == QLatin1String("Calligra.Gemini.Dropbox"));
    qmlRegisterUncreatableType<Controller>("Calligra.Gemini.Dropbox", 1, 0, "Controller", "Main controller for all interaction with Dropbox");
}

void DropboxPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri)
    Q_ASSERT(uri == QLatin1String("Calligra.Gemini.Dropbox"));

    Controller *controller = new Controller(qApp);

    QQmlContext *context = engine->rootContext();
    context->setContextProperty("controllerMIT", controller);
    context->setContextProperty("Options", &controller->m_options);
    context->setContextProperty("folderListModel", controller->folder_model);
    context->setContextProperty("filesTransferModel", controller->filestransfer_model);
}

#include "dropboxplugin.moc"
