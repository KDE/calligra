/* This file is part of the KDE project
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KOABSTRACTAPPLICATIONCONTROLLER_H
#define KOABSTRACTAPPLICATIONCONTROLLER_H

#include <QMessageBox>
#include <KoPAPageBase.h>
#include <KoShape.h>
#include <KoDocument.h>
#include <KoCanvasControllerWidget.h>

#include "koabstraction_export.h"

//TODO #include "CollaborateInterface.h"

class QTextDocument;
class QSplashScreen;
class QCloseEvent;

class KUndo2Stack;
class KoView;
class KWView;
class KoTextEditor;
class KoCanvasController;
class KoCellToolFactory;
class KoCellTool;
class KoExternalEditorInterface;

//! Arguments for document opening
class KOABSTRACTION_EXPORT KoAbstractApplicationOpenDocumentArguments {
public:
    KoAbstractApplicationOpenDocumentArguments();

    /*!
     * true if document(s) should be opened for editing. False by default.
     */
    bool editing;

    /*!
     * true if document(s) should be opened as templates.
     * Implies editing mode. False by default.
     */
    bool openAsTemplates;

    /*!
     * List of documents to open. If empty, new document should be created.
     */
    QStringList documentsToOpen;
};

//! Abstraction of custom office application's controller implementing fundamental features.
/*! The controller implements standard behaviour for the application, which can be altered
 * by reimplemented given methods or setting properties.
 * Events of application are filtered by the controller, e.g. closeEvent().
*/
//! @todo public CollaborateInterface
class KOABSTRACTION_EXPORT KoAbstractApplicationController : public QObject
{
    Q_OBJECT

public:
    /*!
     * Controller constructor, takes mandatory application object @a application.
     * The controller implements standard behaviour for the application.
     * @a application becomes also parent of the controller.
     */
    KoAbstractApplicationController(QObject *application);

    virtual ~KoAbstractApplicationController();

#if 0
    //! Receiver provides slots that can be connected to signals. The slots are:
    /*! - void documentPageSetupChanged()
        - void goToPage(int page)
        - void goToPreviousPage()
        - void goToNextPage()
    */
    QObject *signalReceiver() const;
#endif

    enum MessageType {
        UnsupportedFileTypeMessage,
        InformationMessage,              //!< Information with default timeout
        InformationWithoutTimeoutMessage //!< Information without timeout
    };

    enum QuestionType {
        SaveDiscardCancelQuestion, //!< "Document is modified, do you want to save it before closing?" question
                                   //!< with possible resultsL QMessageBox::Save, QMessageBox::Discard, QMessageBox::Cancel
        ConfirmSheetDeleteQuestion //!< "Do you want to delete the sheet?" question
                                   //!< with possible results: QMessageBox::Yes, QMessageBox::No
                                   //! Provides localized message.
    };

    QStringList supportedExtensions() const;

    QStringList supportedFilters() const;

    /*!
     * @return true if @a extension is supported by the application.
     * Comparison is case insensitive.
     */
    bool isSupportedExtension(const QString& extension) const;

    /*!
     * @return true if @a extension is natively supported by the application (i.e. is ODT, ODP or ODS).
     * Comparison is case insensitive.
     */
    bool isNativeDocumentExtension(const QString& extension) const;

    /*!
     * @return true if @a extension is supported by the application as text document.
     * Comparison is case insensitive.
     */
    bool isTextDocumentExtension(const QString& extension) const;

    /*!
     * @return true if @a extension is supported by the application as presentation document.
     * Comparison is case insensitive.
     */
    bool isPresentationDocumentExtension(const QString& extension) const;

    /*!
     * @return true if @a extension is supported by the application as spreadsheet document.
     * Comparison is case insensitive.
     */
    bool isSpreadsheetDocumentExtension(const QString& extension) const;
    
    // tool identifiers
    static QString panToolFactoryId();
    static QString textToolFactoryId();
    static QString cellToolFactoryId();

    // -- for implementation --

    /*!
     * Shows message with text @a messageText of type @a type.
     */
    virtual void showMessage(MessageType type, const QString& messageText = QString()) = 0;

    /*!
     * Shows question message of type @a type with optional text @a messageText.
     */
    virtual QMessageBox::StandardButton askQuestion(QuestionType type, const QString& messageText = QString()) = 0;

    /*!
     * Starts new instance of the application and opens documents as specified by @a args.
     * @return true on success.
     */
    virtual bool startNewInstance(const KoAbstractApplicationOpenDocumentArguments& args) = 0;

    /*!
     * Performs UI initialization needed before document opening.
     */
    virtual void showUiBeforeDocumentOpening(bool isNewDocument) = 0;

    /*!
     * Asks user for a file path name for opening and returns it.
     * @return selected filename or empty string if selection was cancelled.
     * See QFileDialog::getOpenFileName() for explanation of arguments.
     */
    virtual QString showGetOpenFileNameDialog(const QString& caption, const QString& dir, const QString& filter) = 0;

    /*!
     * Asks user for a file path name for saving and returns it.
     * @return selected filename or empty string if selection was cancelled.
     * See QFileDialog::getSaveFileName() for explanation of arguments.
     */
    virtual QString showGetSaveFileNameDialog(const QString& caption, const QString& dir, const QString& filter) = 0;

    /*!
     * Sets central widget for application view.
     */
    virtual void setCentralWidget(QWidget *widget) = 0;

    /*!
     * @return pointer to the main window.
     */
    virtual QWidget* mainWindow() const = 0;

    /*!
     * Update the enabled/disabled state of actions depending on if a document 
     * is currently loaded.
     */
    virtual void updateActions() = 0;

    /*!
     * @return translated application name.
     */
    virtual QString applicationName() const = 0;

    /*!
     * @return true if virtual keyboard is visible.
     */
    virtual bool isVirtualKeyboardVisible() const = 0;

    /*!
     * @return external cell editor for spreadsheets.
     * 0 can be returned, in this case edited text will not be transferred to the editor 
     * but application will still work properly.
     */
    virtual KoExternalEditorInterface* createExternalCellEditor(KoCellTool* cellTool) const = 0;

    /*!
     * @return true if editing mode is on.
     */
    bool editingMode() const;

    // -- utilities --

    /*!
     * @return true if document is modified.
     */
    bool isDocumentModified() const;

    /*!
     * @return loaded document object.
     */
    inline KoDocument* document() const { return m_doc; }

    /*!
     * @return view.
     */
    inline KoView* view() const { return m_view; }

    /*!
     * @return canvas controller.
     */
    inline KoCanvasController* canvasController() const { return m_canvasController; }

    /*!
     * @return canvas controller widget or 0 if the controller is not based on QWidget.
     */
    KoCanvasControllerWidget* canvasControllerWidget() const {
        return dynamic_cast<KoCanvasControllerWidget*>(m_canvasController);
    }

    /*!
     * @return text editor object.
     */
    inline KoTextEditor* textEditor() const { return m_editor; }

    /*!
     * Pointer to KWView
     */
    KWView* wordsView() const;

    /*!
     * Sets splash screen to @a splash.
     */
    void setSplashScreen(QSplashScreen* splash);

    /*!
     * @return pointer to document's undo stack or 0 if there is no document.
     */
    inline KUndo2Stack* undoStack() const { return m_doc ? m_doc->undoStack() : 0; }

    /*!
     * Trigger an action from the action collection of the current KoView.
     * @param name The name of the action to trigger
     * @return bool Returns false if there was no action with the given name found
     */
    bool triggerAction(const char* name);

    /*!
     * Handles application's close event. @return true if close event can be accepted.
     */
    bool handleCloseEvent(QCloseEvent *event);

    /*!
     * Slot for reacting on resource changes in manager
     * Can be reimplemented, do not forget about calling this implementation though.
     */
    virtual void resourceChanged(int key, const QVariant& value);

    /*!
     * @return name of the current sheet.
     */
    QString currentSheetName() const;

    /*!
     * @return pointer to the cell tool.
     */
    KoCellTool* cellTool() const;

   
    /*!
     * @return pointer to the splash widget or 0.
     */
    QSplashScreen* splash() const { return m_splash; }

    /*!
     * Updates window title for @a fileName name.
     */
    void updateWindowTitle(const QString& fileName);

    /*!
     * Shows document save dialog.
     */
    QString getSaveFileName();

    /*!
     * Name of the opened document file.
     */
    inline QString documentFileName() const { return m_fileName; }

    /*!
     * Document type.
     */
    enum DocumentType { NoDocument, TextDocument, PresentationDocument, SpreadsheetDocument };

    /*!
     * @return type of loaded document or NoDocument if no document is loaded.
     */
    inline DocumentType documentType() const { return m_type; }

    /*!
     * @return number of pages in loaded document or 0 if document is not loaded.
     */
    int pageCount() const;

    /*!
     * @return current page number.
     */
    int currentPage() const { return m_currentPage; }

public slots:
    // -- for possible reimplementation --

    /*!
     * Called whenever page setup for document changes. Can be reimplemented.
     * Default implementation just calls updateActions().
     * @see document(), KWDocument::pageSetupChanged()
     */
    virtual void handleDocumentPageSetupChanged();

    /*!
     * Current page number has changed. Reaction on this should be implemented here.
     * @a previousPage points to previously selected page. Use currentPage() to get
     * number of the current page.
     */
    virtual void handleCurrentPageChanged(int previousPage) = 0;

    /*!
     * Invoked when the currently active tool changes.
     */
    virtual void activeToolChanged(KoCanvasController* canvas, int uniqueToolId);

    /*!
     * Convenience method: opens one document pointed by @a fileName for viewing.
     */
    virtual bool openDocument(const QString &fileName);

    /*!
     * Convenience method: opens one document pointed by @a fileName for editing as template.
     */
    bool openDocumentAsTemplate(const QString &fileName);

    /*!
     * Opens documents as specified by @a args.
     * See documentation of @a KoAbstractApplicationOpenDocumentArguments for explanation
     * of arguments @a args.
     */
    virtual bool openDocuments(const KoAbstractApplicationOpenDocumentArguments& args);

    /*!
     * Closes document without asking for confirmation. Can be reimplemented.
     */
    virtual void closeDocument();

    /*!
     * Opens document.
     * @return true if document has been opened.
     */
    virtual bool openDocument();

    /*!
     * Saves document. Calls saveDocumentAs() if needed.
     * @return true if document has been saved.
     */
    virtual bool saveDocument();

    /*!
     * Saves document under name selected by user.
     * @return true if document has been saved.
     */
    virtual bool saveDocumentAs();

    /*!
     * Goes to particular page @a page.
     */
    void goToPage(int page);

    /*!
     * Goes to previous page.
     */
    void goToPreviousPage();

    /*!
     * Goes to next page.
     */
    void goToNextPage();

    /*!
     * Shows or hides virtual keyboard.
     */
    void toggleVirtualKeyboardVisibility();

    /*!
     * Sets editing mode on or off. @return true on success.
     * It is possible to reimplement this method to add more functionality;
     * in this case implementation from the superclass should be called.
     */
    virtual bool setEditingMode(bool set);

    /*!
     * Convenience slot, equivalent to setEditingMode(true).
     * @return true on success.
     */
    bool setEditingMode();

    /*!
     * Convenience slot, equivalent to setEditingMode(false).
     * @return true on success.
     */
    bool setViewingMode();

    /*!
     * Sets main window's title to @a title.
     */
    virtual void setWindowTitle(const QString& title) = 0;

    /*!
     * Shows or hides virtual keyboard.
     */
    virtual void setVirtualKeyboardVisible(bool set) = 0;

    /*!
     * Shows or hides progress bar indicator.
     */
    virtual void setProgressIndicatorVisible(bool visible) = 0;

    /*!
     * Sets document modification flag.
     */
    void setDocumentModified(bool set);

    /*!
     * Controls visibility of horizontal scroll bar of the document view.
     */
    void setHorizontalScrollBarVisible(bool set);

    /*!
     * Controls visibility of vertical scroll bar of the document view.
     */
    void setVerticalScrollBarVisible(bool set);
    
    /*!
     * Adds a new sheet.
     * For spreadsheets documents only.
     */
    void addSheet();

    /*!
     * Removes current sheet.
     * For spreadsheets documents only.
     * @return true on successful removal.
     */
    bool removeCurrentSheet();

    /*!
     * If @a set is true only document name is displayed and not application name.
     * This is to conserve space on small displays.
     * The default is false.
     */
    void setOnlyDisplayDocumentNameInTitle(bool set);

signals:
    /*!
     * Presentation has entered full screen mode.
     */
    void presentationStarted();
    /*!
     * Presentation has exited from full screen mode.
     */
    void presentationStopped();
    /*!
     * Presentation has moved to the next slide.
     */
    void nextSlide();
    /*!
     * Presentation has moved to the previous slide.
     */
    void previousSlide();

protected slots:
    /*!
     * Moves to next slide.
     * Default implementation just emits nextSlide() signal.
     */
    virtual void goToNextSlide();

    /*!
     * Moves to previous slide.
     * Default implementation just emits previousSlide() signal.
     */
    virtual void goToPreviousSlide();

    /*!
     * Moves to next sheet.
     */
    virtual void goToNextSheet();

    /*!
     * Moves to previous sheet.
     */
    virtual void goToPreviousSheet();

    /*!
     * Opens document scheduled for opening (in m_fileNameToOpen).
     * Used for delayed execution. Can be reimplemented.
     * @return true on success.
     */
    virtual bool openScheduledDocument();

protected:
    /*!
     * Filtering of the application's events.
     */
    virtual bool eventFilter(QObject *watched, QEvent *event);

    /*!
     * Application object.
     */
    QObject *m_application;

//! @todo make private
    bool m_firstChar;
//! @todo make private
    /*!
     * Index for moving between searched strings
     */
    int m_searchTextIndex;
//! @todo make private
    /*!
     * Positions for found text strings
     */
    QList<QPair<QPair<KoPAPageBase*, KoShape*>, QPair<int, int> > > m_searchTextPositions;
//! @todo make private
    /*!
     * Flag for seeing if search is case sensitive. False by default.
     * @todo implement this
     */
    bool m_searchWholeWords;
//! @todo make private
    /*!
     * Flag for seeing if search is case sensitive. False by default.
     */
    bool m_searchCaseSensitive;

private:
    bool saveDocumentInternal(QString fileName);

#if 0
    /*!
     * Internal object for receiving signals.
     */
    KoAbstractApplicationSignalReceiver *m_signalReceiver;
#endif
    
    /*!
     * Central widget for application view.
     */
    QWidget* m_centralWidget;

    /*!
     * Name of the opened file.
     */
    QString m_fileName;

    /*!
     * File name that will be opened by openScheduledDocument().
     */
    QString m_fileNameToOpen;

    mutable QStringList m_supportedExtensions;

    mutable QStringList m_supportedFilters;

    /*!
     * Pointer to KoDocument
     */
    KoDocument *m_doc;

    /*!
     * true if document is currently being loaded
     */
    bool m_isLoading;

    /*!
     * true if editing mode is on
     */
    bool m_editingMode;

    /*!
     * flag for checking open document type
     */
    DocumentType m_type;

    /*!
     * Current page number. Saved in resourceChanged().
     */
    int m_currentPage;

    /*!
     * Previous page number. Used in goToNextPage().
     */
    int m_prevCurrentPage;

    /*!
     * Pointer to KoView
     */
    KoView *m_view;

    /*!
     * Pointer to QTextDocument
     */
    QTextDocument *m_textDocument;

    /*!
     * Pointer to KoTextEditor
     */
    KoTextEditor *m_editor;

    /*!
     * Pointer to KoCanvasController
     */
    KoCanvasController *m_canvasController;

    /*!
     * Pointer to dedicated spreadsheet's CellTool
     */
    KoCellTool *m_cellTool;

    /*!
     * Pointer to factory providing spreadsheet's CellTool
     */
    KoCellToolFactory *m_cellToolFactory;

    /*!
     * Pointer to splash class
     */
    QSplashScreen *m_splash;

    /*!
     * If @a set is true only document name is displayed and not application name.
     * @see setOnlyDisplayDocumentNameInTitle()
     */
    bool m_onlyDisplayDocumentNameInTitle;

    friend class KoCellTool;
    friend class KoCellToolFactory;
};

#endif
