/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000-2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KOMAINWINDOWBASE_H
#define KOMAINWINDOWBASE_H

#include "komvc_export.h"
#include <QMainWindow>

class KoViewBase;
class KoControllerBase;
class KoDocumentFactory;
class KoViewFactory;

/**
 * @brief Main window for a Calligra application
 *
 * This class is used to represent a main window
 * of a Calligra component. Each main window contains
 * a menubar and some toolbars and one or more views.
 *
 * It is up to the subclass of KoMainWindowBase to determine
 * how to show those multiple views: split, in a tabbar,
 * mdi or whatever.
 */
class KOMVC_EXPORT KoMainWindowBase : public QMainWindow
{
    Q_OBJECT
public:

    /**
     *  Constructor.
     *
     *  Initializes a Calligra main window (with its basic GUI etc.).
     */
    explicit KoMainWindowBase();

    /**
     *  Destructor.
     */
    virtual ~KoMainWindowBase();

    void setDocumentFactory(KoDocumentFactory *factory);
    void setViewFactory(KoViewFactory *factory);

public slots:

    bool fileNew();
    bool fileOpen();
    bool fileSave();
    bool fileSaveAs();
    bool fileSaveAll();
    bool fileExport();
    bool fileClose();
    bool fileCloseAll();

protected:
    void closeEvent(QCloseEvent *event);

    // Add a given view to this window.
    virtual bool addView(KoViewBase *view) = 0;

signals:

    void aboutToClose();

private:

    class Private;
    Private *const d;
};

class KoMainWindowFactory {
public:
    virtual ~KoMainWindowFactory() {}
    virtual KoMainWindowBase *create(KoControllerBase *controller) = 0;
};

#endif
