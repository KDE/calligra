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

#include <QObject>
#include <QStringList>
#include <KoZoomMode.h>

class KoDocument;
class QGraphicsItem;
class KoCanvasController;
class KoCanvasBase;
class CADocumentController;

class CAAbstractDocumentHandler : public QObject
{
    Q_OBJECT
    Q_ENUMS (FlickModes)
    Q_PROPERTY(QString topToolbarSource READ topToolbarSource CONSTANT)
    Q_PROPERTY(QString rightToolbarSource READ rightToolbarSource CONSTANT)
    Q_PROPERTY(QString bottomToolbarSource READ bottomToolbarSource CONSTANT)
    Q_PROPERTY(QString leftToolbarSource READ leftToolbarSource CONSTANT)
    Q_PROPERTY(QString centerOverlaySource READ centerOverlaySource CONSTANT)
    Q_PROPERTY(QString previousPageImage READ previousPageImage NOTIFY previousPageImageChanged)
    Q_PROPERTY(QString nextPageImage READ nextPageImage NOTIFY nextPageImageChanged)
    Q_PROPERTY(FlickModes flickMode READ flickMode CONSTANT)

public:
    enum FlickModes {
        FlickAutomatically,
        FlickHorizontally,
        FlickVertically,
        FlickBoth
    };
    explicit CAAbstractDocumentHandler (CADocumentController* documentController);
    virtual ~CAAbstractDocumentHandler();

    virtual QStringList supportedMimetypes() = 0;
    virtual bool openDocument (const QString& uri) = 0;
    virtual QString documentTypeName() = 0;
    virtual KoZoomMode::Mode preferredZoomMode() const = 0;
    virtual KoDocument* document() = 0;

    bool canOpenDocument (const QString& uri);
    KoCanvasBase* canvas() const;

    virtual QString topToolbarSource() const;
    virtual QString rightToolbarSource() const;
    virtual QString bottomToolbarSource() const;
    virtual QString leftToolbarSource() const;
    virtual QString centerOverlaySource() const;
    virtual QString previousPageImage() const;
    virtual QString nextPageImage() const;
    virtual FlickModes flickMode() const;

public slots:
    virtual void gotoPreviousPage();
    virtual void gotoNextPage();

signals:
    void previousPageImageChanged();
    void nextPageImageChanged();

protected:
    class Private;
    Private* const d;

    void setCanvas (KoCanvasBase* canvas);
    CADocumentController* documentController() const;
};

#endif // CAABSTRACTDOCUMENTHANDLER_H
