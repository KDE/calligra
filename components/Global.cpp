/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "Global.h"

#include <QDebug>

#include <QMimeDatabase>
#include <QPluginLoader>
#include <QUrl>
#include <QUrlQuery>

#include <KPluginMetaData>
#include <KoPluginLoader.h>

// For the mimetype names
#include <KPrDocument.h>
#include <KWDocument.h>
#include <sheets/core/DocBase.h>

using namespace Calligra::Components;

static const QStringList staticTextTypes{"application/pdf"};

Calligra::Components::Global::Global(QObject *parent)
    : QObject{parent}
{
}

int Global::documentType(const QUrl &document)
{
    int result = DocumentType::Unknown;

    if (!document.isValid()) {
        return result;
    }

    const QUrlQuery query(document);

    // First, check if the URL gives us specific information on this topic (such as asking for a new file)
    if (query.hasQueryItem("mimetype")) {
        QString mime = query.queryItemValue("mimetype");
        if (mime == WORDS_MIME_TYPE) {
            result = DocumentType::TextDocument;
        } else if (mime == SHEETS_MIME_TYPE) {
            result = DocumentType::Spreadsheet;
        } else if (mime == STAGE_MIME_TYPE) {
            result = DocumentType::Presentation;
        }
    } else {
        QMimeType mime = QMimeDatabase{}.mimeTypeForUrl(document);

        // TODO: see if KoPluginLoader could provide this info via some metadata query instead
        const auto metaDatas = KoPluginLoader::pluginLoaders(QStringLiteral("calligra/parts"), mime.name());

        for (const auto &metaData : metaDatas) {
            if (metaData.fileName().contains("words")) {
                result = DocumentType::TextDocument;
                break;
            } else if (metaData.fileName().contains("sheets")) {
                result = DocumentType::Spreadsheet;
                break;
            } else if (metaData.fileName().contains("stage")) {
                result = DocumentType::Presentation;
                break;
            }
        }

        // Since we don't actually have a Calligra plugin that handles these...
        if ((result == DocumentType::Unknown) && staticTextTypes.contains(mime.name())) {
            result = DocumentType::StaticTextDocument;
        }
    }

    return result;
}
