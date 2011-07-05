/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
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

#ifndef APPLICATION_CONTROLLER_H
#define APPLICATION_CONTROLLER_H

#include <KoAbstractApplicationController.h>

#include <styles/KoListStyle.h>
#include <KoPAView.h>

#include "OfficeInterface.h"
#include "Splash.h"
#include "PreviewDialog.h"
#include "NotesDialog.h"
#include "SlidingMotionDialog.h"

#include "CollabClient.h"
#include "CollabDialog.h"
#include "CollabServer.h"
#include "DigitalSignatureDialog.h"

class QPushButton;
class QIcon;
class QTextDocument;
class QToolButton;
class QFrame;
class QLabel;
class QMessageBox;
class QLineEdit;
class QTextCursor;
class QShortcut;
class QCheckBox;
class QComboBox;
class QFontComboBox;
class QTextListFormat;
class QTextDocument;
class QGridLayout;
class QDialog;
class QListWidget;
class QListWidgetItem;
class SlidingMotionDialog;
class VirtualKeyBoard;
#ifdef HAVE_OPENGL
class GLPresenter;
#endif

class KUndo2Stack;
class KoTextEditor;
class PresentationTool;
class MainWindowAdaptor;
class KoCanvasControllerWidget;
class KoShape;
class KoPAPageBase;

class MainWindow;
namespace Ui
{
    class MainWindow;
}

/*!
 * @brief implementation of the application's controller
 */
class ApplicationController : public KoAbstractApplicationController
{
    Q_OBJECT

public:
    ApplicationController(Splash *aSplash, MainWindow *mainWindow);
    ~ApplicationController();

    /*!
     * Reimplemented for KoAbstractApplicationController.
     */
    virtual bool openDocuments(const KoAbstractApplicationOpenDocumentArguments& args);

    //! Implemented for KoAbstractApplicationController
    virtual void showMessage(MessageType type,
                             const QString& messageText = QString());

    //! Implemented for KoAbstractApplicationController
    virtual QMessageBox::StandardButton askQuestion(QuestionType type,
                                                    const QString& messageText = QString());

    //! Implemented for KoAbstractApplicationController
    virtual bool startNewInstance(const KoAbstractApplicationOpenDocumentArguments& args);

    //! Implemented for KoAbstractApplicationController
    virtual void showUiBeforeDocumentOpening(bool isNewDocument);

    //! Implemented for KoAbstractApplicationController
    virtual QString showGetOpenFileNameDialog(const QString& caption,
                                              const QString& dir, const QString& filter);

    //! Implemented for KoAbstractApplicationController
    virtual QString showGetSaveFileNameDialog(const QString& caption,
                                              const QString& dir, const QString& filter);

    //! Implemented for KoAbstractApplicationController
    virtual void setCentralWidget(QWidget *widget);

    //! Implemented for KoAbstractApplicationController
    virtual QWidget* mainWindow() const;

    //! Implemented for KoAbstractApplicationController
    virtual void updateActions();

    //! Implemented for KoAbstractApplicationController
    virtual void handleDocumentPageSetupChanged();

    //! Implemented for KoAbstractApplicationController
    virtual void handleCurrentPageChanged(int previousPage);
    
    //! Implemented for KoAbstractApplicationController
    virtual QString applicationName() const;

    //! Implemented for KoAbstractApplicationController
    virtual bool isVirtualKeyboardVisible() const;

    //! Implemented for KoAbstractApplicationController
    KoExternalEditorInterface* createExternalCellEditor(KoCellTool* cellTool) const;

    //! Called from main window's eventFilter()
    bool handleMainWindowEventFilter(QObject *watched, QEvent *event);

    //! Called from main window's mousePressEvent()
    void handleMainWindowMousePressEvent(QMouseEvent *event);

    //! Called from main window's mouseMoveEvent()
    void handleMainWindowMouseMoveEvent(QMouseEvent *event);

    //! Called from main window's mouseReleaseEvent()
    void handleMainWindowMouseReleaseEvent(QMouseEvent *event);

    //! Called from main window's paintEvent()
    void handleMainWindowPaintEvent(QPaintEvent *event);

    //! Called from main window's resizeEvent()
    void handleMainWindowResizeEvent(QResizeEvent* event);
public slots:
    //! Implemented for KoAbstractApplicationController
    virtual void setWindowTitle(const QString& title);

    //! Implemented for KoAbstractApplicationController
    virtual void setProgressIndicatorVisible(bool visible);

    //! Reimplemented for KoAbstractApplicationController
    virtual bool setEditingMode(bool set);

    //! Reimplemented for KoAbstractApplicationController
    virtual void closeDocument();

    //! Implemented for KoAbstractApplicationController
    virtual void setVirtualKeyboardVisible(bool set);

    void raiseWindow();

    void setFullScreenPresentationNavigationEnabled(bool set);

private slots:
    /*!
     * Reimplemented for KoAbstractApplicationController
     */
    virtual bool openScheduledDocument();

    //! Reimplemented for KoAbstractApplicationController
    virtual void resourceChanged(int key, const QVariant& value);

    /*!
     * Enable the select tool. This will allow shapes to be selected and moved around
     */
    void enableSelectTool();
    /*!
     * Insert picture into the document
     */
    void insertImage();
    /*!
     * Shows available items which can be inserted into the document
     */
    void insertButtonClicked();
    /*!
     * Show cut,copy, paste actions
     */
    void showCCP();

    void menuClicked(QAction* action);
    void pluginOpen(bool newWindow, const QString& path);
    void showFontSizeDialog();
    void fontSizeEntered();
    void fontSizeRowSelected(QListWidgetItem *item);
    void startMathMode(bool start);
    /**
     * Adds symbol for spreadEditToolBar
     */
    void addMathematicalOperator(QString mathSymbol);
    /*!
     * Slot to perform UndoAction
     */
    void doUndo();
    /*!
     * Slot to perform RedoAction
     */
    void doRedo();
    /*!
     * Slot to actionSearch toggled signal
     */
    void toggleToolBar(bool);
    /*!
     *  Slot to convert character into bold
     */
    void doBold();
    /*!
     *  Slot to convert character into italic
     */
    void doItalic();
    /*!
     *  Slot to convert character into underline
     */
    void doUnderLine();
    /*!
     *  Slot for Left Alignment
     */
    void doLeftAlignment();
    /*!
     *  Slot for right Alignment
     */
    void doRightAlignment();
    /*!
     *  Slot for center Alignment
     */
    void doCenterAlignment();
    /*!
     *  Slot for justify Alignment
     */
    void doJustify();
    /*!
     *  Slot for adding Numbers
     */
    void doNumberList();
    /*!
     *  Slot for adding Bullets
     */
    void doBulletList();
    /*!
     *  Slot to perform sub Script action
     */
    void doSubScript();
    /*!
     *  Slot to perform super Script action
     */
    void doSuperScript();
    /*!
     *  Slot for font size Selection
     */
    void selectFontSize(int size);
    /*!
     *  Slot for font type Selection
     */
    void selectFontType();
    /*!
     *  Slot for text color Selection
     */
    void selectTextColor();
    /*!
     *  Slot for text backgroundcolor Selection
     */
    void selectTextBackGroundColor();
    /*!
     * Slot to display formatframe with all options
     */
    void openFormatFrame();
    /*!
     * Slot to display fontstyleframe with all options
     */
     void openFontStyleFrame();
     /*!
      * Slot to display Math operation options
      */
    void openMathOpFrame();
    /*!
     * Slot to open new documnet
     */
    void openNewDoc();
    /*!
     * Slot to open new presenter
     */
    void openNewPresenter();
    /*!
     * Slot to open new spreadsheet
     */
    void openNewSpreadSheet();
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
     * Slot for actionSearch triggered signal
     */
    void startSearch();
    /*!
     * Slot for moving to previous found text string
     */
    void goToPreviousWord();
    /*!
     * Slot for moving to next found text string
     */
    void goToNextWord();
    /*!
     * Slot for toggleing between whole word search and part of word search
     */
    void searchOptionChanged(int aCheckBoxState);
    /*!
     * Slot to perform copy operation on selected text
     */
    void copy();
      /*!
     * Slot to perform cut operation on selected text
     */
    void cut();
    /*!
     * Slot to perform paste operation on selected text
     */
    void paste();
    /*!
     * Slot that shows a hildonized application menu
     */
    void showApplicationMenu();
    /*!
     * Slot to actionAbout triggered signal
     */
    void openAboutDialog();
    /*!
     * Slot to show  slide transition options
     */
    void slideTransitionDialog();
    /*!
     * Slot to preview selected templates
     */
    void selectedTemplatePreview(int number);
    /*!
     * Open selected template
     */
    void openSelectedTemplate();
    /*!
     * Close Template Selection Dialog
     */
    void closeTempSelectionDialog();
    /*!
     * Slot for progress indicator
     */
    //void slotProgress(int value);
    /*!
     * Slot to show digital signature information
     */
    void showDigitalSignatureInfo();

    /*!
     * Slot to choose new document
     */
    void chooseDocumentType();
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
    /*!
     * Slot to show the preview dialog
     */
    void showPreviewDialog();
    /*!
     * Slot to show the notes dialog when show notes dialog is clicked
     */
    void slideNotesButtonClicked();
    /*!
     * Slot to update current slide in the presentation, when the slide changed in the notes dialog
     */
    void moveSlideFromNotesSlide(bool flag);

    void startCollaborating();
    void collaborationCancelled();
    void collaborateDialog();

    void receivedFontSize(uint start, uint end, uint size);
    void receivedTextColor(uint start, uint end, QRgb color);
    void receivedTextBackgroundColor(uint start, uint end, QRgb color);
    void receivedFontType(uint start, uint end, const QString &font);
    void receivedString(uint start, uint end, QByteArray msg);
    void receivedBackspace(uint start, uint end);
    void receivedFormat(uint start, uint end, Collaborate::FormatFlag format);
    void error(quint16 err);

    void collabSaveFile(const QString &filename);
    void collabOpenFile(const QString &filename);

    void insertNewTextShape();
    void insertNewTable();

    void spaceHandlerForVirtualKeyboard();

#ifdef Q_WS_MAEMO_5
    /*!
    * Slot to toggle between the slide transition
    */
    void switchToSlid();
    /*!
    * Slot to toggle between the scrolling
    */
    void switchToScroll();
    /*!
    * Slot for the scroll action
    */
    void scrollAction();
    /*!
    * Slot for closing all the accelerator settings
    */
    void closeAcceleratorSettings();
    /*!
    * Slot for Enabling and disabling the scroll
    */
    void  enableDisableScrollingOption();

    friend void AcceleratorScrollSlide::ifYesScroll();

    friend void AcceleratorScrollSlide::ifNoScroll();
#endif // Q_WS_MAEMO_5

private:
    // Apply the selected formatting
    bool setFontSize(int size, KoTextEditor* editor);
    bool setFontType(const QString &font, KoTextEditor* editor);
    bool setTextColor(const QColor &color, KoTextEditor* editor);
    bool setTextBackgroundColor(const QColor &color, KoTextEditor* editor);
    bool setBold(KoTextEditor* editor);
    bool setItalic(KoTextEditor* editor);
    bool setUnderline(KoTextEditor* editor);
    bool setLeftAlign(KoTextEditor* editor);
    bool setRightAlign(KoTextEditor* editor);
    bool setCenterAlign(KoTextEditor* editor);
    bool setJustify(KoTextEditor* editor);
    bool setNumberList(KoTextEditor* editor);
    bool setBulletList(KoTextEditor* editor);
    bool setSubScript(KoTextEditor* editor);
    bool setSuperScript(KoTextEditor* editor);

    /*!
     * Handle the Spreadsheet sheet information
     */
    void spreadSheetInfo();

    /*!
     * style formatting function
     */

    void doStyle(KoListStyle::Style, KoTextEditor* editor);
    /*!
     *Function to add formatframe components
     */

    QPushButton *addFormatFrameComponent(const QString &imagepath);
    /*!
     *Function to add fontstyleframe components
     */

    QPushButton *addFontStyleFrameComponent(const QString &imagepath);
    /*!
     *Function to create new document
     */

    QToolButton *addNewDocument(const QString &docname);
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
     * shows back and forward buttons in fullscreen presentation mode
     */
    void showFullScreenPresentationIcons();

    /*!
     * Function to check activeFormatFrame Options
     */
    void activeFormatOptionCheck();
    /*!
     * Function to check activeFontStyleFrame Options
     */
    void activeFontOptionCheck();
    /*!
     * Format Frame Destructor
     */
    void formatFrameDestructor();
    /*!
     * FontStyle Frame Destructor
     */
    void fontStyleFrameDestructor();
    /*!
     * Template Chooser
     */
    void templateSelectionDialog();
    /*!
     * Set up tool bar.
     */
    void setUpSpreadEditorToolBar();
    /*!
     * Reset the SpreadEditor toolbar.
     */
    void resetSpreadEditorToolBar();

    void closeCollabDialog();

    ///////////////////////////
    // Collaborative editing //
    ///////////////////////////

    CollabDialog* m_collabDialog;
    Collaborate* m_collab;
    KoTextEditor* m_collabEditor;

    //////////////////////////////////
    
    MainWindow* m_mainWindow;

    Ui::MainWindow *m_ui;

    QPointer<KoTextEditor> m_pEditor;
    
    QShortcut *m_shortcutForVirtualKeyBoard;
    
    QShortcut *m_spaceHandlerShortcutForVirtualKeyBoard;
    
    VirtualKeyBoard* m_virtualKeyBoard;

    QMap<QString, OfficeInterface*> m_loadedPlugins;

    ///*!
    // * flag for new file to existing file conversion
    // */
    //bool m_existingFile;
    /*!
     * QShortcut to cut text with Ctrl-X
     */
    QShortcut *m_cutShortcut;
    /*!
     * QShortcut for copying text with Ctrl-C
     */
    QShortcut *m_copyShortcut;
    /*!
     * QShortcut for copying text with Ctrl-V
     */
    QShortcut *m_pasteShortcut;
    /*!
     * QShortcut for copying text with Ctrl-Z
     */
    QShortcut *m_undoShortcut;
    /*!
     * QShortcut for copying text with Ctrl-P
     */
    QShortcut *m_redoShortcut;
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
     * view number used while dbus session creation
     */
    int m_viewNumber;
    /*!
     * Pointer to show notes button
     */
    QPushButton *m_slideNotesButton;
    /*!
     * Icon for show notes button
     */
    const QIcon m_slideNotesIcon;
    /*!
     * Pointer to presentation drawing tools
     */
    PresentationTool *m_presentationTool;
    /*!
     * pointer to preview button store
     */
    StoreButtonPreview *m_storeButtonPreview;
    /*!
     * Pointer to notes dialog
     */
    NotesDialog *m_notesDialog;
    /*!
     * Pointer to sliding motion dialog
     */
    SlidingMotionDialog *m_slidingmotiondialog;

    /*!
     * Tool bar for spread sheet.
     */
    QToolBar *m_spreadEditToolBar;

    /*!
     * Dialog for the digital signature.
     */
    DigitalSignatureDialog *m_digitalSignatureDialog;

    /*!
     * Open GL Class , to handle the slide show.
     */
#ifdef HAVE_OPENGL
    GLPresenter *presenter;
#endif

   /*!
      * Holds the properties of presentation style and time
      */
    int gl_showtime;
    int gl_style;
    /*!
     * X-cordinate value at mouse click position
     */
    int m_xcoordinate;
    /*!
     * Y-cordinate value at mouse click position
     */
    int m_ycoordinate;
    /*!
     * Font related information for each Character in NewDocuments
     */
    int m_fontsize;
    /*!
     * Dialog for fontsize
     */
    QDialog *m_fontSizeDialog;
    QVBoxLayout *m_fontSizeDialogLayout;
    QLineEdit *m_fontSizeLineEdit;
    QListWidget *m_fontSizeList;
    /*!
     * Format frame declaration
     */
    QFrame * m_formatframe;
    QGridLayout * m_formatframelayout;
    QPushButton * m_alignleft;
    QPushButton * m_alignright;
    QPushButton * m_aligncenter;
    QPushButton * m_numberedlist;
    QPushButton * m_bulletlist;
    QPushButton * m_alignjustify;
    /*!
     * Font style frame declaration
     */
    QFrame * m_fontstyleframe;
    QGridLayout *m_fontstyleframelayout;
    QFontComboBox *m_fontcombobox;
    QPushButton *m_fontsizebutton;
    QPushButton *m_textcolor;
    QPushButton *m_textbackgroundcolor;
    QPushButton *m_superscript;
    QPushButton *m_subscript;
    QPushButton * m_bold;
    QPushButton * m_italic;
    QPushButton * m_underline;
    /*!
     * New document chooser dialog
     */
    QDialog * m_docdialog;
    QGridLayout * m_docdialoglayout;
    QToolButton * m_document;
    QToolButton * m_presenter;
    QToolButton * m_spreadsheet;
    /*!
     * Presentation Template chooser dialog
     */
    QDialog * m_tempselectiondialog;
    QGridLayout * m_tempdialoglayout;
    QListWidget * m_templateWidget;
    QPushButton * m_go;
    QPushButton * m_closetemp;
    QLabel * m_templatepreview;
    QStringList m_temptitle;
    QStringList m_templatepath;
    QString newpresenter;
    /*!
     * Actions for spreadEditToolBar.
     */
    QAction *m_addAction;
    QAction *m_subtractAction;
    QAction *m_multiplyAction;
    QAction *m_divideAction;
    QAction *m_percentageAction;
    QAction *m_equalsAction;
    QSignalMapper *m_signalMapper;
    /*!
     * Frame for Spreadsheet sheet information.
     */
    QFrame *m_sheetInfoFrame;
    QGridLayout *m_sheetInfoFrameLayout;
    QPushButton *m_addSheet;
    QPushButton *m_removeSheet;
    QPushButton *m_sheetName;

    /*!
     * line edit for search
     */
    QLineEdit *m_search;
    /*!
     * Checkbox to change between normal and exact match searches
     */
    QCheckBox *m_exactMatchCheckBox;
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

    /*!
     * Pointer to pen draw button
     */
    QPushButton *m_fsPPTDrawPenButton;
    /*!
     * Pointer to highlight draw button
     */
    QPushButton *m_fsPPTDrawHighlightButton;
    /*!
     *Pointer to the MainWindowAdaptor object
     */
    MainWindowAdaptor *m_dbus;

    ////////////////////////////////////
    // Accelerator scrolling ,sliding //
    ////////////////////////////////////
#ifdef Q_WS_MAEMO_5
    /*!
    * true if scrolling start
    */
    static bool stopAcceleratorScrolling;
    /*!
    * true if slide transition starts
    */
    static bool enableAccelerator;
    /*!
    * true if scrolling starts
    */
    static bool enableScrolling;

    /*!
    *  For accessing the accelerator
    */
    AcceleratorScrollSlide m_acceleratorForScrolAndSlide;
    /*!
    * Pointer to fullscreen Accelerometer Button
    */
    QPushButton *m_fsAccButton;

    /*!
    * shortcut for enabling the gestures
    */
    QShortcut *m_shortcutForAccelerator;
#endif // Q_WS_MAEMO_5
};

#endif // APPLICATION_CONTROLLER_H
