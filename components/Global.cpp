/*
 * This file is part of the KDE project
 *
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
 *
 */

#include "Global.h"

#include <QDebug>

#include <QUrl>
#include <QUrlQuery>
#include <QMimeDatabase>
#include <QPluginLoader>

#include <KoPluginLoader.h>

// For the mimetype names
#include <KWDocument.h>
#include <DocBase.h>
#include <KPrDocument.h>

using namespace Calligra::Components;

static const QStringList staticTextTypes{ "application/pdf" };

Calligra::Components::Global::Global(QObject* parent)
    : QObject{parent}
{
}

int Global::documentType(const QUrl& document)
{
    int result = DocumentType::Unknown;

    if (!document.isValid()) {
        return result;
    }

    const QUrlQuery query(document);

    // First, check if the URL gives us specific information on this topic (such as asking for a new file)
    if(query.hasQueryItem("mimetype")) {
        QString mime = query.queryItemValue("mimetype");
        if(mime == WORDS_MIME_TYPE) {
            result = DocumentType::TextDocument;
        }
        else if(mime == SHEETS_MIME_TYPE) {
            result = DocumentType::Spreadsheet;
        }
        else if(mime == STAGE_MIME_TYPE) {
            result = DocumentType::Presentation;
        }
    }
    else {
        QMimeType mime = QMimeDatabase{}.mimeTypeForUrl(document);

        // TODO: see if KoPluginLoader could provide this info via some metadata query instead
        QList<QPluginLoader*> plugins = KoPluginLoader::pluginLoaders(QStringLiteral("calligra/parts"), mime.name());

        for (int i = 0; i < plugins.count(); i++) {
            QPluginLoader* loader = plugins.at(i);

            if(loader->fileName().contains("words")) {
                result = DocumentType::TextDocument;
                break;
            } else if(loader->fileName().contains("sheets")) {
                result = DocumentType::Spreadsheet;
                break;
            } else if(loader->fileName().contains("stage")) {
                result = DocumentType::Presentation;
                break;
            }
        }

        // cleanup
        qDeleteAll(plugins);

        // Since we don't actually have a Calligra plugin that handles these...
        if ((result == DocumentType::Unknown) && staticTextTypes.contains(mime.name())) {
            result = DocumentType::StaticTextDocument;
        }
    }

    return result;
}
