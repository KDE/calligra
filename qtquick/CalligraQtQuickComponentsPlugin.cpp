/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "CalligraQtQuickComponentsPlugin.h"

#include "CQImageProvider.h"
#include "CQTextDocumentCanvas.h"
// #include "CQSpreadsheetCanvas.h"
#include "CQCanvasControllerItem.h"
#include "CQPresentationCanvas.h"
#include "CQTextToCModel.h"
// #include "CQSpreadsheetListModel.h"
#include "CQLinkArea.h"
#include "CQPluginLoaderProxy.h"
#include "CQPresentationModel.h"
#include "CQThumbnailItem.h"

#include <QQmlContext>
#include <QQmlEngine>

void CalligraQtQuickComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.calligra.CalligraComponents"));
    qmlRegisterType<CQTextDocumentCanvas>(uri, 0, 1, "TextDocumentCanvas");
    //    qmlRegisterType<CQSpreadsheetCanvas> (uri, 0, 1, "SpreadsheetCanvas");
    qmlRegisterType<CQPresentationCanvas>(uri, 0, 1, "PresentationCanvas");
    qmlRegisterType<CQCanvasControllerItem>(uri, 0, 1, "CanvasControllerItem");
    qmlRegisterType<CQTextToCModel>(uri, 0, 1, "TextToCModel");
    //    qmlRegisterType<CQSpreadsheetListModel> (uri, 0, 1, "SpreadsheetListModel");
    qmlRegisterType<CQPresentationModel>(uri, 0, 1, "PresentationModel");
    qmlRegisterType<CQThumbnailItem>(uri, 0, 1, "Thumbnail");
    qmlRegisterType<CQLinkArea>(uri, 0, 1, "LinkArea");
}

void CalligraQtQuickComponentsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    CQImageProvider::s_imageProvider = new CQImageProvider;
    engine->addImageProvider(CQImageProvider::identificationString, CQImageProvider::s_imageProvider);

    engine->rootContext()->setContextProperty("Calligra", new CQPluginLoaderProxy(engine));

    QQmlEngineExtensionPlugin::initializeEngine(engine, uri);
}

#include "CalligraQtQuickComponentsPlugin.moc"
