/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "CalligraQtQuickComponentsPlugin.h"

#include "CQTextDocumentCanvas.h"
#include "CQImageProvider.h"
//#include "CQSpreadsheetCanvas.h"
#include "CQPresentationCanvas.h"
#include "CQCanvasControllerItem.h"
#include "CQTextToCModel.h"
//#include "CQSpreadsheetListModel.h"
#include "CQPresentationModel.h"
#include "CQThumbnailItem.h"
#include "CQPluginLoaderProxy.h"
#include "CQLinkArea.h"

#include <QDeclarativeEngine>
#include <QDeclarativeContext>

void CalligraQtQuickComponentsPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("org.calligra.CalligraComponents"));
    qmlRegisterType<CQTextDocumentCanvas> (uri, 0, 1, "TextDocumentCanvas");
//    qmlRegisterType<CQSpreadsheetCanvas> (uri, 0, 1, "SpreadsheetCanvas");
    qmlRegisterType<CQPresentationCanvas> (uri, 0, 1, "PresentationCanvas");
    qmlRegisterType<CQCanvasControllerItem> (uri, 0, 1, "CanvasControllerItem");
    qmlRegisterType<CQTextToCModel> (uri, 0, 1, "TextToCModel");
//    qmlRegisterType<CQSpreadsheetListModel> (uri, 0, 1, "SpreadsheetListModel");
    qmlRegisterType<CQPresentationModel>(uri, 0, 1, "PresentationModel");
    qmlRegisterType<CQThumbnailItem>(uri, 0, 1, "Thumbnail");
    qmlRegisterType<CQLinkArea>( uri, 0, 1, "LinkArea" );
}

void CalligraQtQuickComponentsPlugin::initializeEngine(QDeclarativeEngine* engine, const char* uri)
{
    CQImageProvider::s_imageProvider = new CQImageProvider;
    engine->addImageProvider(CQImageProvider::identificationString, CQImageProvider::s_imageProvider);

    engine->rootContext()->setContextProperty( "Calligra", new CQPluginLoaderProxy( engine ) );

    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
}

#include "CalligraQtQuickComponentsPlugin.moc"
