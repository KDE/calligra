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
#include <kfiledialog.h>

class QLabel;
class KoDocument;
class KoView;
class KoMainWindowPrivate;
class KURL;
class KRecentFilesAction;
class KoFilterManager;
class DCOPObject;

namespace KParts
{
  class PartManager;
}

/**
 *
 * This class is used to represent a main window
 * of a KOffice component. Each main window contains
 * a menubar and some toolbars.
 *
 * This class does NOT need to be subclassed in your application.
 *
 * @short Main window for a KOffice application
 */
class KoMainWindow : public KParts::MainWindow
{
    Q_OBJECT
public:

    /**
     *  Constructor.
     *
     *  Initializes a KOffice main window (with its basic GUI etc.).
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

    virtual KParts::PartManager *partManager();

    /**
     * Prints the document
     * @param quick whether the print setup dialog is to be displayed
     **/
    void print(bool quick);

    /**
     * The application should call this to show or hide a toolbar.
     * It also takes care of the corresponding action in the settings menu.
     */
    void showToolbar( const char * tbName, bool shown );

    /**
     * @return TRUE if the toolbar @p tbName is visible
     */
    bool toolbarIsVisible(const char *tbName);

    /**
     * Get hold of the label in the statusbar, to write messages to it.
     * You can also insert other items in the status bar by using QStatusBar::addWidget.
     */
    QLabel * statusBarLabel();

    /**
     * Sets the maximum number of recent documents entries.
     */
    void setMaxRecentItems(uint _number);

    /**
     * The document opened a URL -> store into recent documents list.
     */
    void addRecentURL( const KURL& url );

    /**
     * Load the desired document and show it.
     * @param url the URL to open
     *
     * @return TRUE on success.
     */
    virtual bool openDocument( const KURL & url );

    /**
     * Load the URL into this document (and make it root doc after loading)
     *
     * Special method for KoApplication::start, don't use.
     */
    bool openDocument( KoDocument *newdoc, const KURL & url );

    virtual DCOPObject * dcopObject();

    /**
     * Reloads the recent documents list.
     */
    void reloadRecentFileList();

    /**
     * Updates the window caption based on the document info and path.
     */
    void updateCaption( const QString caption, bool mod ); // BCI: make virtual
    void updateReloadFileAction(KoDocument *doc);

signals:
    /**
     * This signal is emitted if the document has been saved successfully.
     */
    void documentSaved();

public slots:

    /**
     * Slot for eMailing the document using KMail
     *
     * This is a very simple extension that will allow any document
     * that is currently being edited to be emailed using KMail.
     */
    void slotEmailFile();

    /**
     *  Slot for opening a new document.
     *
     *  If the current document is empty, the new document replaces it.
     *  If not, a new shell will be opened for showing the document.
     */
    virtual void slotFileNew();

    /**
     *  Slot for opening a saved file.
     *
     *  If the current document is empty, the opened document replaces it.
     *  If not a new shell will be opened for showing the opened file.
     */
    virtual void slotFileOpen();

    /**
     *  Slot for opening a file among the recently opened files.
     *
     *  If the current document is empty, the opened document replaces it.
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

    /**
     * Show a dialog with author and document information.
     */
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
     *  Configure key bindings.
     */
    virtual void slotConfigureKeys();

    /**
     *  Configure toolbars.
     */
    virtual void slotConfigureToolbars();

    /**
     *  Post toolbar config.
     * (Plug action lists back in, etc.)
     */
    virtual void slotNewToolbarConfig();

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

    /**
     * Reload file
     */
    void slotReloadFile();

    /**
     * File --> Import
     *
     * This will call slotFileOpen().  To differentiate this from an ordinary
     * call to slotFileOpen() call @ref isImporting().
     */
    void slotImportFile();

    /**
     * File --> Export
     *
     * This will call slotFileSaveAs().  To differentiate this from an ordinary
     * call to slotFileSaveAs() call @ref isExporting().
     */
    void slotExportFile();

protected:

    /// Helper method for slotFileNew and slotFileClose
    void chooseNewDocument( int /*KoDocument::InitDocFlags*/ initDocFlags );
    /**
     * Special method for KOShell, to allow switching the root
     * document (and its views) among a set of them.
     */
    void setRootDocumentDirect( KoDocument *doc, const QPtrList<KoView> & views );

    /**
     * Create a new empty document.
     */
    virtual KoDocument* createDoc() const;

    /**
     * Saves the document, asking for a filename if necessary.
     *
     * @param saveas if set to TRUE the user is always prompted for a filename
     *
     * @return TRUE on success, false on error or cancel
     *         (don't display anything in this case, the error dialog box is also implemented here
     *         but restore the original URL in slotFileSaveAs)
     */
    virtual bool saveDocument( bool saveas = false );

    /**
     * Asks the user if they really want to save the document if
     * outputFormat != nativeFormat.  If outputFormat == nativeFormat, no dialog
     * is shown and it is assumed that the user wishes to save.
     *
     * @return true if the document should be saved
     */
    bool exportConfirmation( const QCString &outputFormat, const QCString &nativeFormat );

    virtual void closeEvent( QCloseEvent * e );
    virtual void resizeEvent( QResizeEvent * e );

    /**
     * Ask user about saving changes to the document upon exit.
     */
    virtual bool queryClose();

    virtual bool openDocumentInternal( const KURL & url, KoDocument * newdoc = 0L );

    /**
     * Save the list of recent files.
     */
    void saveRecentFiles();

    /**
     * Returns whether or not the current slotFileSave[As]() or saveDocument()
     * call is actually an export operation (like File --> Export).
     *
     * If this is true, you must call KoDocument::export() instead of
     * KoDocument::save() or KoDocument::saveAs(), in any reimplementation of
     * saveDocument().
     */
    bool isExporting() const;

    /**
     * Returns whether or not the current slotFileOpen() or openDocument()
     * call is actually an import operation (like File --> Import).
     *
     * If this is true, you must call KoDocument::import() instead of
     * KoDocument::openURL(), in any reimplementation of openDocument() or
     * openDocumentInternal().
     */
    bool isImporting() const;

    KRecentFilesAction *recentAction() const { return m_recent; }
private:

    void saveWindowSettings();

    KRecentFilesAction *m_recent;

protected slots:
    virtual void slotActivePartChanged( KParts::Part *newPart );

private slots:
    void slotProgress(int value);
    void slotLoadCompleted();
    void slotLoadCanceled (const QString &);
    void slotSaveCompleted();
    void slotSaveCanceled(const QString &);

private:
    KoMainWindowPrivate *d;

};

// Extension to KFileDialog in order to add "save as koffice-1.1" and "save as dir"
// Used only when saving!
class KoFileDialog : public KFileDialog
{
    Q_OBJECT
public:
    KoFileDialog(const QString& startDir, const QString& filter,
                 QWidget *parent, const char *name,
                 bool modal);
    void setSpecialMimeFilter( QStringList& mimeFilter,
                               const QString& currentFormat, const int specialOutputFlag,
                               const QString& nativeFormat );
    int specialEntrySelected();
private slots:
void slotChangedfilter( int index );
};

#endif
