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

#include "CAAbstractDocumentHandler.h"
#include "CADocumentController.h"
#include "CACanvasController.h"

#include <KoCanvasController.h>
#include <KoCanvasBase.h>

#include <KMimeType>

class CAAbstractDocumentHandler::Private
{
public:
    Private()
    {
        documentController = 0;
    }
    CADocumentController *documentController;
};

CAAbstractDocumentHandler::CAAbstractDocumentHandler (CADocumentController* documentController)
    : QObject (documentController)
    , d (new Private())
{
    d->documentController = documentController;
}

CADocumentController* CAAbstractDocumentHandler::documentController() const
{
    return d->documentController;
}

CAAbstractDocumentHandler::~CAAbstractDocumentHandler()
{
    delete d;
}

KoCanvasBase* CAAbstractDocumentHandler::canvas() const
{
    return documentController()->canvasController()->canvas();
}

void CAAbstractDocumentHandler::setCanvas (KoCanvasBase* canvas)
{
    documentController()->canvasController()->setCanvas (canvas);
}

bool CAAbstractDocumentHandler::canOpenDocument (const QString& uri)
{
    return supportedMimetypes().contains (KMimeType::findByUrl (uri)->name());
}

QString CAAbstractDocumentHandler::bottomToolbarSource() const
{
    return QString();
}

QString CAAbstractDocumentHandler::leftToolbarSource() const
{
    return QString();
}

QString CAAbstractDocumentHandler::rightToolbarSource() const
{
    return QString();
}

QString CAAbstractDocumentHandler::topToolbarSource() const
{
    return QString();
}

QString CAAbstractDocumentHandler::centerOverlaySource() const
{
    return QString();
}

QString CAAbstractDocumentHandler::nextPageImage() const
{
    return QString();
}

QString CAAbstractDocumentHandler::previousPageImage() const
{
    return QString();
}

void CAAbstractDocumentHandler::gotoNextPage()
{
}

void CAAbstractDocumentHandler::gotoPreviousPage()
{
}

CAAbstractDocumentHandler::FlickModes CAAbstractDocumentHandler::flickMode() const
{
    return FlickAutomatically;
}

#include "CAAbstractDocumentHandler.moc"
