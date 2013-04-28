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

#ifndef CATEXTDOCUMENTHANDLER_H
#define CATEXTDOCUMENTHANDLER_H

#include "CAAbstractDocumentHandler.h"

class CATextDocumentModel;
class QSizeF;
class KoFindMatch;

class CATextDocumentHandler : public CAAbstractDocumentHandler
{
    Q_OBJECT
    Q_PROPERTY (QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)
    Q_PROPERTY (int totalPages READ totalPages)

public:
    explicit CATextDocumentHandler (CADocumentController* documentController);
    virtual ~CATextDocumentHandler();

    virtual QStringList supportedMimetypes();
    virtual bool openDocument (const QString& uri);
    virtual KoDocument* document();
    virtual QString documentTypeName();
    virtual KoZoomMode::Mode preferredZoomMode() const;

    QString searchString() const;
    void setSearchString (const QString& searchString);

    virtual QString bottomToolbarSource() const;
    virtual QString topToolbarSource() const;
    virtual QString centerOverlaySource() const;
    virtual FlickModes flickMode() const;

    int totalPages() const;

    Q_INVOKABLE CATextDocumentModel *paTextDocumentModel() const;

public slots:
    void updateCanvas();
    void resizeCanvas (const QSizeF& canvasSize);

    void findNext();
    void findPrevious();
    void copy();
    void gotoPage(int pageNumber);

signals:
    void searchStringChanged();

private slots:
    void findMatchFound(const KoFindMatch& match);
    void findNoMatchFound();

private:
    class Private;
    Private* const d;
};

#endif // CATEXTDOCUMENTHANDLER_H
