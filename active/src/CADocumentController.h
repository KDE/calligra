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

#ifndef CADOCUMENTCONTROLLER_H
#define CADOCUMENTCONTROLLER_H

#include <QObject>

class CAAbstractDocumentHandler;
class CACanvasController;

class CADocumentController : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString documentUri READ documentUri WRITE setDocumentUri NOTIFY documentUriChanged)
    Q_PROPERTY (CACanvasController* canvasController READ canvasController WRITE setCanvasController NOTIFY canvasControllerChanged)
    Q_PROPERTY (QString documentTypeName READ documentTypeName NOTIFY documentTypeNameChanged)
    Q_PROPERTY (CAAbstractDocumentHandler* documentHandler READ documentHandler NOTIFY documentHandlerChanged)

public:
    explicit CADocumentController (QObject* parent = 0);
    virtual ~CADocumentController();

    void setDocumentUri (const QString& uri);
    QString documentUri() const;

    CACanvasController* canvasController() const;
    void setCanvasController (CACanvasController* canvasController);

    QString documentTypeName() const;
    CAAbstractDocumentHandler* documentHandler();

signals:
    void documentUriChanged();
    void canvasControllerChanged();
    void documentOpened();
    void documentTypeNameChanged();
    void failedToOpenDocument();
    void documentHandlerChanged();

public slots:
    void loadDocument();

private:
    class Private;
    Private* const d;
};

#endif // CADOCUMENTCONTROLLER_H
