/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __ko_main_window_h__
#define __ko_main_window_h__

#include <kparts/mainwindow.h>

class KoDocument;
class KoView;
class KoMainWindowPrivate;
class KURL;
class KRecentFilesAction;

namespace KParts
{
  class PartManager;
}

/**
 * This class is used to represent a main window
 * of a KOffice component. Each main window contains
 * a menubar and some toolbars.
 *
 * If you are going to implement a new KOffice component, then
 * you must implement a subclass of this class.
 */
class KoMainWindow : public KParts::MainWindow
{
    Q_OBJECT
    friend class KoDocument;
public:

    /**
     *  Constructor.
     *
     *  Initializes a KOffice Main-Window (with its basic GUI etc.).
     */
    KoMainWindow( KInstance *instance, const char *_name = 0 );

    /**
     *  Destructor.
     */
    ~KoMainWindow();

    /**
     * Called when a document is assigned to this mainwindow.
     * This creates a view for this document, makes it the active part, etc.
     */
    virtual void setRootDocument( KoDocument *doc );

    /**
     * Update caption from document info - call when document info
     * (title in the about page) changes.
     */
    virtual void updateCaption();

    /**
     *  Retrieves the document that is displayed in the mainwindow.
     */
    virtual KoDocument* rootDocument() const;

    virtual KoView *rootView() const;

    /**
     * The pattern of the native file format, for example "*.ksp".
     */
    QString nativeFormatPattern();

    /**
     * The name of the native file format. Usually the name of
     * your component, for example i18n("KSpread") or i18n("KWord").
     */
    QString nativeFormatName();

    virtual KParts::PartManager *partManager();

    /**
     * Get hold of the label in the statusbar, to write messages to it.
     * You can also insert other items in the status bar by using QStatusBar::addWidget.
     */
    QLabel * statusBarLabel() const;

public slots:

    /**
     *  Slot for opening a new document.
     *
     *  If the current document is empty, the new document replaces the it.
     *  If not a new shell will be opened for showing the document.
     */
    virtual void slotFileNew();

    /**
     *  Slot for opening a saved file.
     *
     *  If the current document is empty, the opened document replaces the it.
     *  If not a new shell will be opened for showing the opened file.
     */
    virtual void slotFileOpen();

    /**
     *  Slot for opening a file among the recently opened files.
     *
     *  If the current document is empty, the opened document replaces the it.
     *  If not a new shell will be opened for showing the opened file.
     */
    virtual void slotFileOpenRecent( const KURL & );

    /**
     *  Saves the current document with the current name.
     */
    virtual void slotFileSave();

    /**
     *  Saves the current document with a new name.
     */
    virtual void slotFileSaveAs();

    /**
     *  Prints the actual document.
     */
    virtual void slotFilePrint();

    /**
     *  Show a print preview
     */
    void slotFilePrintPreview(); // make virtual later

    virtual void slotDocumentInfo();

    /**
     *  Closes the document.
     */
    virtual void slotFileClose();

    /**
     *  Closes the shell.
     */
    virtual void slotFileQuit();

    /**
     *  Configure key bindings
     */
    virtual void slotConfigureKeys();

    /**
     *  Configure toolbars
     */
    virtual void slotConfigureToolbars();

    /**
     *  Post toolbar config.
     * (Plug action lists back in, etc.)
     */
    void slotNewToolbarConfig(); // TODO make virtual (BCI)

    /**
     *  Shows or hides a toolbar
     */
    virtual void slotToolbarToggled( bool toggle );

    /**
     * View splitting stuff
     */
    virtual void slotSplitView();
    virtual void slotRemoveView();
    virtual void slotSetOrientation();
    /**
     * Close all views
     */
    virtual void slotCloseAllViews();

    void slotProgress(int value);

protected:

    /**
     * Special method for KOShell, to allow switching the root
     * document (and its views) among a set of them.
     */
    void setRootDocumentDirect( KoDocument *doc, const QList<KoView> & views );

    /**
     * Create a new empty document.
     */
    virtual KoDocument* createDoc() const;

    /**
     * Load the desired document and show it.
     * @param url the URL to open
     *
     * @return TRUE on success.
     */
    virtual bool openDocument( const KURL & url );

    /**
     * Saves the document, asking for a filename if necessary.
     *
     * @param _saveas if set to TRUE the user is always prompted for a filename
     *
     * @return TRUE on success or on cancel, false on error
     *         (don't display anything in this case, the error dialog box is also implemented here
     *         but restore the original URL in slotFileSaveAs)
     */
    virtual bool saveDocument( bool _saveas = false );

    virtual void closeEvent( QCloseEvent * e );
    virtual void resizeEvent( QResizeEvent * e );
    virtual bool queryClose();

    virtual bool eventFilter(QObject *obj, QEvent *ev);

    KRecentFilesAction *m_recent;

protected slots:
    virtual void slotActivePartChanged( KParts::Part *newPart );

private:
    KoMainWindowPrivate *d;

};

#endif
