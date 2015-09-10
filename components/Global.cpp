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
#include <QMimeDatabase>
#include <QPluginLoader>

#include <KoJsonTrader.h>
#include <KoDocumentEntry.h>

using namespace Calligra::Components;

static const QStringList staticTextTypes{ "application/pdf" };

Calligra::Components::Global::Global(QObject* parent)
    : QObject{parent}
{
}

void Global::loadPlugins()
{

}

int Global::documentType(const QUrl& document)
{
    QMimeType mime = QMimeDatabase{}.mimeTypeForUrl(document);

    QList<QPluginLoader*> plugins = KoJsonTrader::self()->query("Calligra/Part", mime.name());

    for (int i = 0; i < plugins.count(); i++) {
        QPluginLoader* loader = plugins.at(i);

        if(loader->fileName().contains("words")) {
            return DocumentType::TextDocument;
        } else if(loader->fileName().contains("sheets")) {
            return DocumentType::Spreadsheet;
        } else if(loader->fileName().contains("stage")) {
            return DocumentType::Presentation;
        }
    }

    // Since we don't actually have a Calligra plugin that handles these...
    if(staticTextTypes.contains(mime.name())) {
        return DocumentType::StaticTextDocument;
    }

    return DocumentType::Unknown;
}
