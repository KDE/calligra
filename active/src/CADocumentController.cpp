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

#include "CACanvasController.h"
#include "CATextDocumentHandler.h"
#include "CASpreadsheetHandler.h"
#include "CAPresentationHandler.h"

class CADocumentController::Private
{
public:
    Private()
    {
        canvasController = 0;
        currentDocumentHandler = 0;
    }
    QString documentUri;
    CACanvasController* canvasController;
    QList<CAAbstractDocumentHandler*> documentHandlers;
    CAAbstractDocumentHandler* currentDocumentHandler;
};

CADocumentController::CADocumentController (QObject* parent)
    : QObject (parent)
    , d (new Private())
{
    d->documentHandlers.append (new CATextDocumentHandler (this));
    d->documentHandlers.append (new CASpreadsheetHandler (this));
    d->documentHandlers.append (new CAPresentationHandler (this));
}

CADocumentController::~CADocumentController()
{

}

QString CADocumentController::documentUri() const
{
    return d->documentUri;
}

void CADocumentController::setDocumentUri (const QString& uri)
{
    d->documentUri = uri;
    emit documentUriChanged();
}

CACanvasController* CADocumentController::canvasController() const
{
    return d->canvasController;
}

void CADocumentController::setCanvasController (CACanvasController* canvasController)
{
    d->canvasController = canvasController;
    emit canvasControllerChanged();
}

void CADocumentController::loadDocument()
{
    Q_FOREACH (CAAbstractDocumentHandler * handler, d->documentHandlers) {
        if (handler->canOpenDocument (documentUri())) {
            d->canvasController->setDocumentHandler(handler);
            if (handler->openDocument (documentUri())) {
                d->currentDocumentHandler = handler;
                emit documentHandlerChanged();
                emit documentTypeNameChanged();
                emit documentOpened();
                return;
            }
        }
    }

    emit failedToOpenDocument();
}

QString CADocumentController::documentTypeName() const
{
    if (!d->currentDocumentHandler) {
        return QString();
    }
    return d->currentDocumentHandler->documentTypeName();
}

CAAbstractDocumentHandler* CADocumentController::documentHandler()
{
    return d->currentDocumentHandler;
}

#include "CADocumentController.moc"
