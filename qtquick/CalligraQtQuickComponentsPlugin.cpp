/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
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
#include "CQSpreadsheetCanvas.h"
#include "CQPresentationCanvas.h"

#include <QDeclarativeEngine>

void CalligraQtQuickComponentsPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("org.calligra.CalligraComponents"));
    qmlRegisterType<CQTextDocumentCanvas> (uri, 0, 1, "TextDocumentCanvas");
    qmlRegisterType<CQSpreadsheetCanvas> (uri, 0, 1, "SpreadsheetCanvas");
    qmlRegisterType<CQPresentationCanvas> (uri, 0, 1, "PresentationCanvas");
}

void CalligraQtQuickComponentsPlugin::initializeEngine(QDeclarativeEngine* engine, const char* uri)
{
    CQImageProvider::s_imageProvider = new CQImageProvider;
    engine->addImageProvider(CQImageProvider::identificationString, CQImageProvider::s_imageProvider);

    QDeclarativeExtensionPlugin::initializeEngine(engine, uri);
}

#include "CalligraQtQuickComponentsPlugin.moc"
