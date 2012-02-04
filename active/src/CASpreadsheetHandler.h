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

#ifndef CASPREADSHEETHANDLER_H
#define CASPREADSHEETHANDLER_H

#include "CAAbstractDocumentHandler.h"

#include <QtCore/QSize>

class CASpreadsheetHandler : public CAAbstractDocumentHandler
{
    Q_OBJECT
    Q_PROPERTY(int currentSheetNumber READ currentSheetNumber NOTIFY currentSheetNumChanged)
    Q_PROPERTY(int sheetCount READ sheetCount NOTIFY sheetCountChanged)

protected:
    virtual KoDocument* document();

public:
    explicit CASpreadsheetHandler (CADocumentController* documentController);
    virtual ~CASpreadsheetHandler();

    virtual bool openDocument (const QString& uri);
    virtual QStringList supportedMimetypes();
    virtual QString documentTypeName();
    Q_INVOKABLE int sheetCount() const;

    virtual QString rightToolbarSource() const;
    virtual QString leftToolbarSource() const;

    int currentSheetNumber() const;

public slots:
    void tellZoomControllerToSetDocumentSize(QSize size);
    void updateCanvas();
    void resizeCanvas(const QSizeF &canvasSize);

    void nextSheet();
    void previousSheet();

signals:
    void currentSheetNumChanged();
    void sheetCountChanged();

private:
    class Private;
    Private * const d;

    void updateDocumentSizeForActiveSheet();
};

#endif // CASPREADSHEETHANDLER_H
