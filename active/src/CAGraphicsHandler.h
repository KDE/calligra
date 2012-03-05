/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 * Copyright (C) 2012  Yue Liu <yue.liu@mail.com>
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

#ifndef CAGRAPHICSHANDLER_H
#define CAGRAPHICSHANDLER_H

#include "CAAbstractDocumentHandler.h"

class QSize;
class QSizeF;

class CAGraphicsHandler : public CAAbstractDocumentHandler
{
    //Q_OBJECT
    Q_PROPERTY(int currentPageNumber READ currentPageNumber NOTIFY currentPageChanged)
    Q_PROPERTY(int totalNumberOfPages READ totalNumberOfPages)

public:
    explicit CAGraphicsHandler (CADocumentController* documentController);
    virtual ~CAGraphicsHandler();
    virtual bool openDocument (const QString& uri);
    virtual QStringList supportedMimetypes();
    virtual QString documentTypeName();

    virtual QString topToolbarSource() const;
    virtual QString rightToolbarSource() const;
    virtual QString leftToolbarSource() const;

    int currentPageNumber() const;
    int totalNumberOfPages() const;

public slots:
    void tellZoomControllerToSetDocumentSize(const QSize &size);
    void nextPage();
    void previousPage();
    void zoomToFit();
    void updateCanvas();
    void resizeCanvas(const QSizeF &canvasSize);

//signals:
    //void currentPageNumChanged();

protected:
    virtual KoDocument* document();

private:
    class Private;
    Private * const d;
};

#endif // CAGRAPHICSHANDLER_H
