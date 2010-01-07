/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
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
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <KoDocument.h>

#include "Splash.h"

class QPushButton;
class QIcon;
class QTextDocument;

namespace Ui
{
class MainWindow;
}

class QLineEdit;
class KoCanvasController;
class QTextCursor;
class KoShape;
class KoPAPageBase;
class QShortcut;
class QCheckBox;

/*!
 * \brief Main window of the application. KoCanvasController is set as
 * the central widget. It displays the loaded documents.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Splash *aSplash, QWidget *parent = 0);
    ~MainWindow();
    //void tabletEvent ( QTabletEvent * event );
    //void mousePressEvent ( QMouseEvent * event );
    //void mouseMoveEvent ( QMouseEvent * event );
    //void mouseReleaseEvent ( QMouseEvent * event );

private:
    Ui::MainWindow *m_ui;
    /*!
     * line edit for search
     */
    QLineEdit *m_search;
    /*!
     * Checkbox to change between normal and exact match searches
     */
    QCheckBox *m_exactMatchCheckBox;
    /*!
     * Pointer to KoDocument
     */
    KoDocument *m_doc;
    /*!
     * Pointer to KoView
     */
    KoView *m_view;
    /*!
     * Pointer to KoCanvasController
     */
    KoCanvasController *m_controller;
    /*!
     * Integers about current page
     */
    int m_vPage, m_hPage;
    /*!
     * Flag for pressed state
     */
    bool m_pressed;
    /*!
     * Flag for seeing which toolbar was active
     */
    bool m_isViewToolBar;
    /*!
     * Timer for hiding full screen button
     */
    QTimer *m_fsTimer;
    /*!
     * Full screen push button
     */
    QPushButton *m_fsButton;
    /*!
     * Icon for full screen button
     */
    const QIcon m_fsIcon;

    /*!
     * Full screen back button for presentations
     */
    QPushButton *m_fsPPTBackButton;

    /*!
     * Full screen forward button for presentations
     */
    QPushButton *m_fsPPTForwardButton;

    /*
     * Current page number. Saved in MainWindow::resourceChanged() slot.
     */
    int m_currentPage;

    /*!
     * Index for moving between searched strings
     */
    int m_index;
    /*!
     * Positions for found text strings
     */
    QList<QPair<QPair<KoPAPageBase*, KoShape*>, QPair<int, int> > > m_positions;
    /*!
     * Flag for seeing if search is for whole words. false by default
     */
    bool m_wholeWord;
    /*!
     * Flag for seeing if open document is presentation
     */
    enum DocumentType { Text, Presentation, Spreadsheet };
    DocumentType m_type;
    /*!
     * Pointer to splash class
     */
    Splash *m_splash;

    void init();
    /*!
     * close the document if it is open and reinit to NULL
     */
    void closeDocument();
    /*!
     * Find string from document
     * /param pointer to QTextDocument
     * /param reference to text to be searched
     */
    void findText(QList<QTextDocument*> docs, QList<QPair<KoPAPageBase*, KoShape*> > shapes, const QString &aText);
    /*!
     * Find string from document
     * /param current index
     */
    void highlightText(int aIndex);

    /*!
     * Trigger an action from the action collection of the current KoView.
     * /param name The name of the action to trigger
     * /return bool Returns false if there was no action with the given name found
     */
    bool triggerAction(const char* name);

    /*!
     * Update the enabled/disabled state of actions depending on if a document is currently
     * loaded.
     */
    void updateActions();

    /*!
     * Event filter to catch all mouse events to be able to properly show and hide the fullscreen
     * button when in fullscreen mode.
     */
    bool eventFilter(QObject *watched, QEvent *event);

    /*!
     * Check filetype
     * /param filename
     * /return true if supported
     */
    bool checkFiletype(const QString &fileName);

    /*!
     * shows back and forward buttons in fullscreen presentation mode
     */
    void showFullScreenPresentationIcons();

    /*!
     * Raise window. For some reason Qt programs are not not raised
     * automatically after D-Bus messages. This low level code sends
     * _NET_ACTIVE_WINDOW message to Matchbox and window is raised.
     */
    void raiseWindow(void);

private slots:
    void updateUI();
    void resourceChanged(int key, const QVariant &value);
    /*!
     * Slot to actionSearch toggled signal
     */
    void toggleToolBar(bool);
    /*!
     * Slot to actionZoomIn triggered signal
     */
    void zoomIn();
    /*!
     * Slot to actionZoomOut triggered signal
     */
    void zoomOut();
    /*!
     * Slot to actionZoomLevel triggered signal
     */
    void zoom();
    void zoomToPage();
    void zoomToPageWidth();
    /*!
     * Slot to actionNextPage triggered signal
     */
    void nextPage();
    /*!
     * Slot to actionPrevPage triggered signal
     */
    void prevPage();
    /*!
     * Slot to fullscreen toolbutton triggered signal
     * Logic for switching from  normal mode to full screen mode
     */
    void fullScreen();
    /*!
     * Slot to mFSTimer timeout signal.
     * Hides full screen button
     */
    void fsTimer();
    /*!
     * Slot to mFSButton clicked signal
     * Deactivates fullscreen mode
     */
    void fsButtonClicked();
    /*!
     * Slot for toggleing between selection and panning
     */
    void toggleSelection();
    /*!
     * Slot for actionSearch triggered signal
     */
    void startSearch();
    /*!
     * Slot for moving to previous found text string
     */
    void previousWord();
    /*!
     * Slot for moving to next found text string
     */
    void nextWord();
    /*!
     * Slot for toggleing between whole word search and part of word search
     */
    void searchOptionChanged(int aCheckBoxState);
    /*!
     * Slot for copying the selected text
     */
    void copy();
    /*!
     * Slot that shows a hildonized application menu
     */
    void showApplicationMenu();
    /*!
     * Slot that is invoked when the currently active tool changes.
     */
    void activeToolChanged(KoCanvasController *canvas, int uniqueToolId);
    void doOpenDocument();
    /*!
     * Slot to actionAbout triggered signal
     */
    void openAboutDialog();
    /*!
     * Slot for progress indicator
     */
    //void slotProgress(int value);
public slots:
    /*!
     * Slot to  dialog fileSelected signal
     * /param filename
     */
    void openDocument(const QString &fileName);
    /*!
     * Slot to actionOpen triggered signal
     */
    void openFileDialog();
    /*!
     * Slot to check DBus activation, If document is not opened
     * then open filedialog
     */
    void checkDBusActivation();
    /*!
     * Slot to test scrolling of a loaded document and quitting afterwards.
     * This slot checks if a document has been loaded. If so, it calls pagedown and waits
     * until the end of the document has been reached.
     * Then it quits.
     */
    void loadScrollAndQuit();

private:
    void setShowProgressIndicator(bool visible);
    /*!
     * Opened file
     */
    QString m_fileName;
    /*!
     * QShortcut for copying text with Ctrl-c
     */
    QShortcut *m_copyShortcut;
    /*!
     * Count of mouseMove or tabletMove events after MousePress or tabletPress event
     */
    int m_panningCount;
    /*!
     * Position of last mousePress or tabletPress event
     */
    QPoint m_pressPos;
    /*!
     * True if slide can be changed by panning document
     */
    bool m_slideChangePossible;
    /*!
     * Is document is currently being loaded
     */
    bool m_isLoading;
};

#endif // MAINWINDOW_H
