/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "CADocumentController.h"

#include "CATextDocumentHandler.h"
#include "CanvasController.h"

#include <QDebug>

class CADocumentController::Private
{
public:
    QString documentUri;
    CanvasController *canvasController;
    QList<CAAbstractDocumentHandler*> documentHandlers;
};

CADocumentController::CADocumentController(QObject* parent)
    : QObject(parent)
    , d(new Private())
{
    d->documentHandlers.append(new CATextDocumentHandler(this));
}

CADocumentController::~CADocumentController()
{

}

QString CADocumentController::documentUri() const
{
    return d->documentUri;
}

void CADocumentController::setDocumentUri(const QString& uri)
{
    d->documentUri = uri;
    emit documentUriChanged();
    loadDocument();
    emit documentOpened();
}

CanvasController* CADocumentController::canvasController() const
{
    return d->canvasController;
}

void CADocumentController::setCanvasController(CanvasController* canvasController)
{
    d->canvasController = canvasController;
    emit canvasControllerChanged();
}

bool CADocumentController::loadDocument()
{
    Q_FOREACH (CAAbstractDocumentHandler *handler, d->documentHandlers) {
        if (handler->canOpenDocument(documentUri())) {
            if (handler->openDocument(documentUri())) {
                return true;
            }
        }
    }

    return false;
}

#include "CADocumentController.moc"
