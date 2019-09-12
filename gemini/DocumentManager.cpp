/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "DocumentManager.h"
#include "Settings.h"
#include "RecentFileManager.h"
#include "ProgressProxy.h"

#include <KWPart.h>
#include <KoDocument.h>
#include <KoDocumentEntry.h>

#include <libs/pigment/KoColor.h>
#include <KoColorSpaceRegistry.h>

#include <stage/part/KPrDocument.h>
#include <part/KWDocument.h>

#include <QMimeType>

class DocumentManager::Private
{
public:
    Private()
        : proxy(0)
        , document(0)
        , part(0)
        , settingsManager(0)
        , recentFileManager(0)
        , importingDocument(false)
        , temporaryFile(false)
    { }

    ProgressProxy* proxy;
    QPointer<KoDocument> document;
    QPointer<KoPart> part;
    Settings* settingsManager;
    RecentFileManager* recentFileManager;

    QString saveAsFilename;
    QString openDocumentFilename;
    bool importingDocument;
    QVariantMap newDocOptions;
    bool temporaryFile;
};

DocumentManager *DocumentManager::sm_instance = 0;

KoDocument* DocumentManager::document() const
{
    return d->document;
}

QObject* DocumentManager::doc() const
{
    return d->document;
}

KoPart* DocumentManager::part(const QString& type)
{
    Q_UNUSED(type)
    if (!d->part)
        d->part = new KWPart(this);
    return d->part;
}

ProgressProxy* DocumentManager::progressProxy() const
{
    return d->proxy;
}

Settings* DocumentManager::settingsManager() const
{
    return d->settingsManager;
}

void DocumentManager::setSettingsManager(Settings* newManager)
{
    d->settingsManager = newManager;
}

void DocumentManager::setDocAndPart(KoDocument* document, KoPart* part)
{
    d->document = document;
    d->part = part;
    d->temporaryFile = false;
    emit documentChanged();
    connect(document, SIGNAL(destroyed()), SIGNAL(aboutToDeleteDocument()));
}

RecentFileManager* DocumentManager::recentFileManager() const
{
    return d->recentFileManager;
}

bool DocumentManager::isTemporaryFile() const
{
    if(d->document->url().isEmpty()) {
        return true;
    }
    return d->temporaryFile;
}

void DocumentManager::newDocument(int width, int height, float resolution)
{
    Q_UNUSED(width)
    Q_UNUSED(height)
    Q_UNUSED(resolution)
    closeDocument();

    QTimer::singleShot(300, this, SLOT(delayedNewDocument()));
}

void DocumentManager::newDocument(const QVariantMap& options)
{
    closeDocument();

    d->newDocOptions = options;
    QTimer::singleShot(300, this, SLOT(delayedNewDocument()));
}

void DocumentManager::delayedNewDocument()
{
    QString filetype;
    if(d->newDocOptions.value("type", WORDS_MIME_TYPE).toString() == WORDS_MIME_TYPE) {
        filetype = "odt";
        d->document = new KWDocument(part(WORDS_MIME_TYPE));
    }
    else {
        filetype = "odp";
        d->document = new KPrDocument(part(STAGE_MIME_TYPE));
    }
    d->document->setProgressProxy(d->proxy);
    d->document->setSaveInBatchMode(true);
    part()->setDocument(d->document);
    connect(d->document, SIGNAL(destroyed()), SIGNAL(aboutToDeleteDocument()));

    if(d->newDocOptions.isEmpty())
    {
        //d->document->newImage("Untitled", d->newDocWidth, d->newDocHeight, KoColorSpaceRegistry::instance()->rgb8());
        //d->document->image()->setResolution(d->newDocResolution, d->newDocResolution);
        //d->document->setUrl(QUrl("Untitled.kra"));
    }
    else
    {
        QString name = d->newDocOptions.value("name", "Untitled").toString();
        //int width = d->newDocOptions.value("width").toInt();
        //int height = d->newDocOptions.value("height").toInt();
        // internal resolution is pixels per point, not ppi
        //float res = d->newDocOptions.value("resolution", 72.0f).toFloat() / 72.0f;

        QString colorModelId = d->newDocOptions.value("colorModelId").toString();
        QString colorDepthId = d->newDocOptions.value("colorDepthId").toString();
        QString colorProfileId = d->newDocOptions.value("colorProfileId").toString();

        const KoColorSpace* profile;
        if(colorModelId.isEmpty() || colorDepthId.isEmpty() || colorProfileId.isEmpty())
        {
            profile = KoColorSpaceRegistry::instance()->rgb8();
        }
        else
        {
            profile = KoColorSpaceRegistry::instance()->colorSpace(colorModelId, colorDepthId, colorProfileId);
        }

        QColor background = d->newDocOptions.value("backgroundColor", QColor("white")).value<QColor>();
        background.setAlphaF(d->newDocOptions.value("backgroundOpacity", 1.0f).toFloat());
        KoColor bg(background, profile);

        d->document->setUrl(QUrl(QString("Untitled.").append(filetype)));
    }

    d->temporaryFile = true;

    emit documentChanged();
}

void DocumentManager::openDocument(const QString& document, bool import)
{
    closeDocument();
    d->openDocumentFilename = document;
    d->importingDocument = import;
    QTimer::singleShot(0, this, SLOT(delayedOpenDocument()));
}

void DocumentManager::delayedOpenDocument()
{
    d->document = 0;
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForUrl(QUrl::fromLocalFile(d->openDocumentFilename));
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimeType.name());
    d->part = documentEntry.createKoPart();
    if (d->part) {
        d->document = d->part->document();
        d->document->setProgressProxy(d->proxy);
        d->document->setSaveInBatchMode(true);

        d->document->setModified(false);
        if (d->importingDocument)
            d->document->importDocument(QUrl::fromLocalFile(d->openDocumentFilename));
        else
            d->document->openUrl(QUrl::fromLocalFile(d->openDocumentFilename));
        d->recentFileManager->addRecent(d->openDocumentFilename);

        d->temporaryFile = false;
    }

    emit documentChanged();
}

void DocumentManager::closeDocument()
{
    if (d->document) {
        d->document->closeUrl(false);
        d->document->deleteLater();
        d->document.clear();
    }
}

bool DocumentManager::save()
{
    if (d->document->save())
    {
        d->recentFileManager->addRecent(d->document->url().toLocalFile());
        d->settingsManager->setCurrentFile(d->document->url().toLocalFile());
        emit documentSaved();
        return true;
    }
    return false;
}

void DocumentManager::saveAs(const QString &filename, const QString &mimetype)
{
    d->document->setOutputMimeType(mimetype.toAscii());
    d->saveAsFilename = filename;
    // Yes. This is a massive hack. Basically, we need to wait a little while, to ensure
    // the save call happens late enough for a variety of UI things to happen first.
    // A second seems like a long time, but well, we do have file system interaction here,
    // so for now, we can get away with it.
    QTimer::singleShot(0, this, SLOT(delayedSaveAs()));
}

void DocumentManager::delayedSaveAs()
{
    d->document->saveAs(QUrl::fromLocalFile(d->saveAsFilename));
    d->settingsManager->setCurrentFile(d->saveAsFilename);
    d->recentFileManager->addRecent(d->saveAsFilename);
    emit documentSaved();
}

void DocumentManager::reload()
{
    QUrl url = d->document->url();
    closeDocument();
    d->openDocumentFilename = url.toLocalFile();
    QTimer::singleShot(0, this, SLOT(delayedOpenDocument()));
}

void DocumentManager::setTemporaryFile(bool temp)
{
    d->temporaryFile = temp;
    emit documentSaved();
}

DocumentManager* DocumentManager::instance()
{
    if (!sm_instance) {
        sm_instance = new DocumentManager(QCoreApplication::instance());
    }

    return sm_instance;
}

DocumentManager::DocumentManager(QObject* parent)
    : QObject(parent), d(new Private)
{
    d->proxy = new ProgressProxy(this);
    d->recentFileManager = new RecentFileManager(this);
}

DocumentManager::~DocumentManager()
{
    delete d;
}

