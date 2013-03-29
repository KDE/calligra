/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KODOCUMENTMANAGER_H
#define KODOCUMENTMANAGER_H

#include "komvc_export.h"
#include <QObject>

class KoDocumentFactory;
class KoViewFactory;

class QStringList;

/**
 * @brief The KoDocumentManager class handles documents, views and windows for the application.
 * A single application instance can have multiple open multiple windows, multiple views per window and
 * documents that are visible in more than one view and window.
 */
class KOMVC_EXPORT KoDocumentManager : public QObject
{
    Q_OBJECT
public:
    explicit KoDocumentManager(QObject *parent = 0);
    
    void setDocumentFactory(KoDocumentFactory *documentFactory);
    void setViewFactory(KoViewFactory *viewFactory);

    /**
     * Loads all the documents that were given on the command line
     * and handles autosave documents
     */
    virtual bool initialize(const QStringList &urls);

signals:
    
public slots:
private:

    class Private;
    Private * const d;
};

#endif // KODOCUMENTMANAGER_H
