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

#ifndef CAABSTRACTDOCUMENTHANDLER_H
#define CAABSTRACTDOCUMENTHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>

class KoDocument;
class QGraphicsItem;
class KoCanvasController;
class KoCanvasBase;
class CADocumentController;

class CAAbstractDocumentHandler : public QObject
{
    Q_OBJECT

public:
    explicit CAAbstractDocumentHandler (CADocumentController* documentController);
    virtual ~CAAbstractDocumentHandler();

    virtual QStringList supportedMimetypes() = 0;
    virtual bool openDocument (const QString& uri) = 0;
    virtual QString documentTypeName() = 0;

    bool canOpenDocument (const QString& uri);
    KoCanvasBase* canvas() const;

protected:
    class Private;
    Private* const d;

    virtual KoDocument* document() = 0;
    void setCanvas (KoCanvasBase* canvas);
    CADocumentController* documentController() const;
};

#endif // CAABSTRACTDOCUMENTHANDLER_H
