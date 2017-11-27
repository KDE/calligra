/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KOPART_H
#define KOPART_H

#include <QList>
#include <QUrl>

#include "komain_export.h"

#include <KoMainWindow.h>

class KoDocument;
class KoView;
class KoComponentData;
class KoOpenPane;
class QGraphicsItem;


/**
 * Override this class in your application. It's the main entry point that
 * should provide the document, the view and the component data to the calligra
 * system.
 *
 * There is/will be a single KoPart instance for an application that will manage
 * the list of documents, views and mainwindows.
 *
 * It hasn't got much to do with kparts anymore.
 */
class KOMAIN_EXPORT KoPart : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param componentData data about the component
     * @param parent may be another KoDocument, or anything else.
     *        Usually passed by KPluginFactory::create.
     */
    explicit KoPart(const KoComponentData &componentData, QObject *parent);

    /**
     *  Destructor.
     *
     * The destructor does not delete any attached KoView objects and it does not
     * delete the attached widget as returned by widget().
     */
    virtual ~KoPart();

    /**
     * @return The componentData ( KoComponentData ) for this GUI client. You set the componentdata
     * in your subclass: setComponentData(AppFactory::componentData()); in the constructor
     */
    KoComponentData componentData() const;

    /**
     * @param document the document this part manages
     */
    void setDocument(KoDocument *document);

    /**
     * @return the document this part loads and saves to and makes views for
     */
    KoDocument *document() const;

    // ----------------- mainwindow management -----------------

    /**
     * Create a new main window, but does not add it to the current set of managed main windows.
     */
    virtual KoMainWindow *createMainWindow() = 0;

    /**
     * Appends the mainwindow to the list of mainwindows which show this
     * document as their root document.
     *
     * This method is automatically called from KoMainWindow::setRootDocument,
     * so you do not need to call it.
     */
    virtual void addMainWindow(KoMainWindow *mainWindow);

    /**
     * Removes the mainwindow from the list.
     */
    virtual void removeMainWindow(KoMainWindow *mainWindow);

    /**
     * @return the list of main windows.
     */
    const QList<KoMainWindow*>& mainWindows() const;

    /**
     * @return the number of shells for the main window
     */
    int mainwindowCount() const;

    void addRecentURLToAllMainWindows(const QUrl &url);

    KoMainWindow *currentMainwindow() const;

public Q_SLOTS:

    /**
     * This slot loads an existing file and deletes the start up widget.
     * @param url the file to load
     */
    virtual void openExistingFile(const QUrl &url);

protected Q_SLOTS:

    /**
     * This slot loads a template and deletes the start up widget.
     * @param url the template to load
     */
    virtual void openTemplate(const QUrl &url);

public:

    /**
     *  Create a new view for the document.
     */
    KoView *createView(KoDocument *document, QWidget *parent = 0);

    /**
     * Adds a view to the document. If the part doesn't know yet about
     * the document, it is registered.
     *
     * This calls KoView::updateReadWrite to tell the new view
     * whether the document is readonly or not.
     */
    virtual void addView(KoView *view, KoDocument *document);

    /**
     * Removes a view of the document.
     */
    virtual void removeView(KoView *view);

    /**
     * @return a list of views this document is displayed in
     */
    QList<KoView*> views() const;

    /**
     * @return number of views this document is displayed in
     */
    int viewCount() const;

    /**
     * Template resource path used. This is used by the start up widget to show
     * the correct templates.
     */
    QString templatesResourcePath() const;


    /**
     * Creates and shows the start up widget.
     * @param parent the KoMainWindow used as parent for the widget.
     * @param alwaysShow always show the widget even if the user has configured it to not show.
     */
    virtual void showStartUpWidget(KoMainWindow *parent) = 0;

protected:

    /**
     * Set the templates resource path used. This is used by the start up widget to show
     * the correct templates.
     */
    void setTemplatesResourcePath(const QString &templatesResourcePath);

    virtual KoView *createViewInstance(KoDocument *document, QWidget *parent) = 0;

    /**
     * Override this to create a QGraphicsItem that does not rely
     * on proxying a KoCanvasController.
     */
    virtual QGraphicsItem *createCanvasItem(KoDocument *document);

private:

    Q_DISABLE_COPY(KoPart)

    class Private;
    Private *const d;

};

#endif
