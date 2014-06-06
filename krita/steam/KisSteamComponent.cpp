/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
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
#include "KisSteamComponent.h"

#include <QTimer>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QClipboard>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QScrollBar>

#include <kdebug.h>
#include <ui/kis_doc2.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kaction.h>
#include <ktemporaryfile.h>

#include "sketch/DocumentManager.h"

#include "steam/kritasteam.h"

class KisSteamComponent::Private
{
public:
    Private(KisSteamComponent* qq)
        : q(qq)
    { }
    ~Private() {
    }

    KisSteamComponent* q;
};

KisSteamComponent::KisSteamComponent(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , d(new Private(this))
{
    // this is just an interaction overlay, the contents are painted on the sceneview background
    setFlag(QGraphicsItem::ItemHasNoContents, true);
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);

    grabGesture(Qt::PanGesture);
}

KisSteamComponent::~KisSteamComponent()
{
    delete d;
}

/**
 * @brief Save a file to the cloud
 * @param fileName Destination path
 * @param mimeType desired mimetype
 */
void KisSteamComponent::saveAs(const QString& fileName, const QString& mimeType)
{
    // Save a temporary file
    KTemporaryFile* tempFile=0;
    tempFile = new KTemporaryFile();
    tempFile->setPrefix("steamExport");
    tempFile->setSuffix(".kra");
    tempFile->setAutoRemove(false);
    tempFile->open();
    QString tempFileName = tempFile->fileName();
    tempFile->close();
    delete tempFile;

    DocumentManager::instance()->document()->exportDocument(tempFileName);

    // Send temporary file to the cloud
    KritaSteamClient::instance()->remoteStorage()->pushFile(tempFileName, fileName);

    // Delete temp file
    QFile file(tempFileName);
    if (file.exists()) {
        QFile::remove(tempFileName);
    }

}

#include "KisSteamComponent.moc"
