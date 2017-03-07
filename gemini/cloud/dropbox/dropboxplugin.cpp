/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dropboxplugin.h"

#include "controller.h"

#include <QtQml/QtQml>
#include <QQmlContext>

void DropboxPlugin::registerTypes(const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT( uri == QLatin1String( "Calligra.Gemini.Dropbox" ) );
    qmlRegisterUncreatableType<Controller>("Calligra.Gemini.Dropbox", 1, 0, "Controller", "Main controller for all interaction with Dropbox");
}

void DropboxPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT( uri == QLatin1String( "Calligra.Gemini.Dropbox" ) );

    Controller* controller = new Controller(qApp);

    QQmlContext *context = engine->rootContext();
    context->setContextProperty("controllerMIT", controller);
    context->setContextProperty("Options", &controller->m_options);
    context->setContextProperty("folderListModel", controller->folder_model);
    context->setContextProperty("filesTransferModel", controller->filestransfer_model);
}

#include "dropboxplugin.moc"
