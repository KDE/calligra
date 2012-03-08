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

#include "ApplicationController.h"

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Common.h"
#include "ZoomDialog.h"
#include "HildonMenu.h"
#include "NotifyDialog.h"
#include "AboutDialog.h"
#include "PresentationTool.h"
#include "MainWindowAdaptor.h"
#include "FoExternalEditor.h"
#include "FoImageSelectionWidget.h"
#ifdef HAVE_OPENGL
#include "GlPresenter.h"
#endif
#include <KoCellTool.h>
#include "VirtualKeyBoard.h"
#include "ConfirmationDialog.h"
#include "PreviewDialog.h"
#include "FileChooserDialog.h"

#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>
#include <QScrollBar>
#include <QTimer>
#include <QIcon>
#include <QPushButton>
#include <QSize>
#include <QTextDocument>
#include <QTextCursor>
#include <QPair>
#include <QMenuBar>
#include <QX11Info>
#include <QShortcut>
#include <QProcess>
#include <QAction>
#include <QLabel>
#include <QTextBlock>
#include <QTextList>
#include <QGridLayout>
#include <QDialog>
#include <QToolButton>
#include <QMessageBox>
#include <QFontComboBox>
#include <QColor>
#include <QColorDialog>
#include <QFrame>
#include <QPalette>
#include <QListWidget>

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#include "Accelerator.h"
#endif

#include <kfileitem.h>

#include <KoView.h>
#include <KoCanvasBase.h>
#include <kdemacros.h>
#include <KoZoomMode.h>
#include <KoZoomController.h>
#include <KoToolProxy.h>
#include <KoToolBase.h>
#include <KoCanvasResourceManager.h>
#include <KoToolManager.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoShapeUserData.h>
#include <KoSelection.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPACanvasBase.h>
#include <KoTextEditor.h>
#include <KoTextDocument.h>
#include <KoTextShapeDataBase.h>
#include <KoPAView.h>
#include <KoStore.h>
#include <KoCanvasBase.h>
#include <KoToolRegistry.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWView.h>
#include <KoShapeLayer.h>
#include <styles/KoParagraphStyle.h>
#include <styles/KoListLevelProperties.h>
#include <KoList.h>
#include <kundo2stack.h>
#include <sheets/Map.h>
#include <sheets/DocBase.h>
#include <sheets/part/View.h>
#include <sheets/Sheet.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
//#define Q_WS_MAEMO_5 1

#define FORMATRAME_XCORDINATE_VALUE 465
#define FORMATFRAME_YCORDINATE_VALUE 150
#define FORMATFRAME_WIDTH 335
#define FORMATFRAME_HEIGHT 210

#define FONTSTYLEFRAME_XCORDINATE_VALUE 280
#define FONTSTYLEFRAME_YCORDINATE_VALUE 150
#define FONTSTYLEFRAME_WIDTH 520
#define FONTSTYLEFRAME_HEIGHT 210

#define SHEETINFOFRAME_XCORDINATE_VALUE 100
#define SHEETINFOFRAME_YCORDINATE_VALUE 290
#define SHEETINFOFRAME_WIDTH 520
#define SHEETINFOFRAME_HEIGHT 70


#ifdef Q_WS_MAEMO_5
bool ApplicationController::m_enableAccelerator = false;
bool ApplicationController::m_enableScrolling = false;
bool ApplicationController::m_stopAcceleratorScrolling = true;
#endif

ApplicationController::ApplicationController(Splash *aSplash, MainWindow *mainWindow)
        : KoAbstractApplicationController(mainWindow),
        m_mainWindow(mainWindow),
        m_slideNotesIcon(VIEW_NOTES_PIXMAP),
        m_fsIcon(FS_BUTTON_PATH)
{
    m_xcoordinate = 0;
    m_ycoordinate = 0;
    m_fontSizeDialog = 0;
    m_fontSizeDialogLayout = 0;
    m_fontSizeLineEdit = 0;
    m_fontSizeList = 0;
    m_formatframe = 0;
    m_formatframelayout = 0;
    m_alignleft = 0;
    m_alignright = 0;
    m_aligncenter = 0;
    m_numberedlist = 0;
    m_bulletlist = 0;
    m_alignjustify = 0;
    m_fontstyleframe = 0;
    m_fontstyleframelayout = 0;
    m_fontcombobox = 0;
    m_fontsizebutton = 0;
    m_textcolor = 0;
    m_textbackgroundcolor = 0;
    m_superscript = 0;
    m_subscript = 0;
    m_bold = 0;
    m_italic = 0;
    m_underline = 0;
    m_docdialog = 0;
    m_docdialoglayout = 0;
    m_templateWidget = 0;
    m_addAction = 0;
    m_subtractAction = 0;
    m_multiplyAction = 0;
    m_divideAction = 0;
    m_percentageAction = 0;
    m_equalsAction = 0;
    m_signalMapper = 0;
    m_sheetInfoFrame = 0;
    m_sheetInfoFrameLayout = 0;
    m_addSheet = 0;
    m_removeSheet = 0;
    m_sheetName = 0;
    m_search = 0;
    m_vPage = 0;
    m_hPage = 0;
    m_viewNumber = 0;
    m_slideNotesButton = 0;
    m_presentationTool = 0;
    m_storeButtonPreview = 0;
    m_isViewToolBar = true;
    m_fsTimer = 0;
    m_fsButton = 0;
    m_fsPPTBackButton = 0;
    m_fsPPTForwardButton = 0;
    m_fsPPTDrawPenButton = 0;
    m_fsPPTDrawHighlightButton = 0;
    m_dbus = new MainWindowAdaptor(this);
    m_panningCount = 0;
    m_pressed = false;
    m_digitalSignatureDialog = 0;
    m_closetemp = 0;
    m_collab = 0;
    m_collabDialog = 0;
    m_collabEditor = 0;
    m_document = 0;
    m_go = 0;
    m_presenter = 0;
    m_slidingmotiondialog = 0;
    m_spreadEditToolBar = 0;
    m_spreadsheet = 0;
    m_tempdialoglayout = 0;
    m_templatepreview = 0;
    m_tempselectiondialog = 0;
    m_ui = new Ui::MainWindow;
    m_notesDialog = 0;
    m_virtualKeyBoard = 0;

    setSplashScreen(aSplash);
    m_ui->setupUi(m_mainWindow);

    QDBusConnection::sessionBus().registerObject("/presentation/view", m_mainWindow);
    m_shortcutForVirtualKeyBoard = new QShortcut(QKeySequence(("Ctrl+K")), m_mainWindow);
    Q_CHECK_PTR(m_shortcutForVirtualKeyBoard);
    m_shortcutForVirtualKeyBoard->setEnabled(true);
    connect(m_shortcutForVirtualKeyBoard, SIGNAL(activated()),
            this, SLOT(toggleVirtualKeyboardVisibility()));

    m_spaceHandlerShortcutForVirtualKeyBoard = new QShortcut(Qt::Key_Space, m_mainWindow);
    Q_CHECK_PTR(m_spaceHandlerShortcutForVirtualKeyBoard);
    m_spaceHandlerShortcutForVirtualKeyBoard->setEnabled(true);
    connect(m_spaceHandlerShortcutForVirtualKeyBoard, SIGNAL(activated()),
            this, SLOT(spaceHandlerForVirtualKeyboard()));

#ifdef Q_WS_MAEMO_5
    m_fsAccButton = 0;
    shortcutForAccelerator = new QShortcut(QKeySequence(("Ctrl+G")), m_mainWindow);
    Q_CHECK_PTR(shortcutForAccelerator);
    shortcutForAccelerator->setEnabled(true);
#endif

    QMenuBar* menu = m_mainWindow->menuBar();
#ifdef Q_WS_MAEMO_5
    menu->addAction(m_ui->actionOpen);
    menu->addAction(m_ui->actionNew);
    menu->addAction(m_ui->actionSave);
    menu->addAction(m_ui->actionSaveAs);
    menu->addAction(m_ui->actionPresentation);
    menu->addAction(m_ui->actionClose);
    menu->addAction(m_ui->actionAbout);
#else
    QMenu *m = m_mainWindow->menuBar()->addMenu("Calligra Mobile");
    m->addAction(m_ui->actionOpen);
    m->addAction(m_ui->actionNew);
    m->addAction(m_ui->actionSave);
    m->addAction(m_ui->actionSaveAs);
    m->addAction(m_ui->actionPresentation);
    m->addAction(m_ui->actionClose);
    m->addAction(m_ui->actionAbout);
#endif
   // menu->addAction(m_ui->actionCollaborate);
    // false here means that they are not plugins
    m_ui->actionOpen->setData(QVariant(false));
    m_ui->actionAbout->setData(QVariant(false));
    m_ui->actionNew->setData(QVariant(false));
    m_ui->actionSave->setData(QVariant(false));
    m_ui->actionSaveAs->setData(QVariant(false));
    m_ui->actionClose->setData(QVariant(false));
    m_ui->actionPresentation->setData(QVariant(false));
    const QDir pluginDir("/usr/lib/calligramobile");
    const QStringList plugins = pluginDir.entryList(QDir::Files);

    for (int i = 0; i < plugins.size(); ++i) {
        QPluginLoader test(pluginDir.absoluteFilePath(plugins.at(i)));
        QObject *plug = test.instance();
        plug->setParent(this);
        if (plug != 0)
        {
            OfficeInterface* inter = qobject_cast<OfficeInterface*>(plug);
            const QString plugName = inter->pluginName();
            m_loadedPlugins.insert(plugName, inter);
            connect(plug, SIGNAL(openDocument(bool, const QString&)),
                    this, SLOT(pluginOpen(bool, const QString&)));
            QAction *action = new QAction(inter->pluginTitle(), this);

            // True states that this action is a plugin
            action->setData(QVariant(true));
            menu->addAction(action);
        }
    }

    connect(menu, SIGNAL(triggered(QAction*)),
            this, SLOT(menuClicked(QAction*)));
    m_search = new QLineEdit(m_mainWindow);
    m_search->setInputMethodHints(Qt::ImhNoAutoUppercase);
    m_ui->SearchToolBar->insertWidget(m_ui->actionSearchOption, m_search);
    m_exactMatchCheckBox = new QCheckBox(i18n("Exact Match"), m_mainWindow);
    m_ui->SearchToolBar->insertWidget(m_ui->actionSearchOption, m_exactMatchCheckBox);
    m_ui->SearchToolBar->hide();

    connect(m_ui->actionNew,SIGNAL(triggered()),this,SLOT(chooseDocumentType()));
    connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(saveDocument()));
    connect(m_ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));
    connect(m_ui->actionClose,SIGNAL(triggered()), this, SLOT(closeDocument()));
    connect(m_ui->actionFormat,SIGNAL(triggered()), this, SLOT(openFormatFrame()));
    connect(m_ui->actionStyle,SIGNAL(triggered()), this, SLOT(openFontStyleFrame()));
    m_ui->actionMathOp->setCheckable(true);
#ifdef Q_WS_MAEMO_5
    connect(m_ui->actionClose,SIGNAL(triggered()),this,SLOT(closeAcceleratorSettings()));
#else
    connect(m_ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
#endif

    connect(m_ui->actionMathOp,SIGNAL(toggled(bool)),this,SLOT(startMathMode(bool)));
#ifdef HAVE_OPENGL
    connect(m_ui->actionPresentation,SIGNAL(triggered()),this,SLOT(glPresenter()));
#endif

    connect(m_search, SIGNAL(returnPressed()), SLOT(goToNextWord()));
    connect(m_search, SIGNAL(textEdited(QString)), SLOT(startSearch()));

    connect(m_ui->actionEdit, SIGNAL(toggled(bool)), this, SLOT(setEditingMode(bool)));
    connect(m_ui->actionSearch, SIGNAL(toggled(bool)), this, SLOT(toggleToolBar(bool)));

    connect(m_ui->actionUndo,SIGNAL(triggered()),this,SLOT(doUndo()));
    connect(m_ui->actionRedo,SIGNAL(triggered()),this,SLOT(doRedo()));

    connect(m_ui->actionCopy,SIGNAL(triggered()),this,SLOT(copy()));
    connect(m_ui->actionCut,SIGNAL(triggered()),this,SLOT(cut()));
    connect(m_ui->actionPaste,SIGNAL(triggered()),this,SLOT(paste()));

    connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(openDocument()));
    connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAboutDialog()));
    connect(m_ui->actionFullScreen, SIGNAL(triggered()), this, SLOT(fullScreen()));
    connect(m_ui->actionSlidingMotion, SIGNAL(triggered()), this, SLOT(slideTransitionDialog()));
    connect(m_ui->actionCollaborate, SIGNAL(triggered()), this, SLOT(collaborateDialog()));

    m_ui->actionZoomIn->setShortcuts(QKeySequence::ZoomIn);
    m_ui->actionZoomIn->setShortcutContext(Qt::ApplicationShortcut);
    m_ui->actionZoomOut->setShortcuts(QKeySequence::ZoomOut);
    m_ui->actionZoomOut->setShortcutContext(Qt::ApplicationShortcut);
    connect(m_ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(m_ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(m_ui->actionZoomLevel, SIGNAL(triggered()), this, SLOT(zoom()));

    connect(m_ui->actionNextPage, SIGNAL(triggered()), this, SLOT(goToNextPage()));
    connect(m_ui->actionPrevPage, SIGNAL(triggered()), this, SLOT(goToPreviousPage()));

    connect(m_ui->actionPrevWord, SIGNAL(triggered()), this, SLOT(goToPreviousWord()));
    connect(m_ui->actionNextWord, SIGNAL(triggered()), this, SLOT(goToNextWord()));
    connect(m_exactMatchCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(searchOptionChanged(int)));

    m_fsTimer = new QTimer(this);
    Q_CHECK_PTR(m_fsTimer);
    connect(m_fsTimer, SIGNAL(timeout()), this, SLOT(fsTimer()));

    m_fsButton = new QPushButton(m_mainWindow);
    Q_CHECK_PTR(m_fsButton);
#ifdef Q_WS_MAEMO_5
    m_fsButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
#endif

    m_fsButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
    m_fsButton->setIcon(m_fsIcon);
    m_fsButton->setIconSize(QSize(48, 48));
    m_fsButton->hide();
    connect(m_fsButton, SIGNAL(clicked()), SLOT(fsButtonClicked()));
    qApp->installEventFilter(m_mainWindow);

    updateActions();

#ifdef Q_WS_X11
    /* taking care of Zoom buttons : starts */
    unsigned long val = 1;
    Atom atom = XInternAtom(QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", False);
    XChangeProperty(QX11Info::display(), m_mainWindow->winId(), atom, XA_INTEGER, 32,
                    PropModeReplace,
                    (unsigned char *) &val, 1);
    /* taking care of Zoom buttons : ends */
#endif

    m_cutShortcut = new QShortcut(QKeySequence::Cut, m_mainWindow);
    connect(m_cutShortcut, SIGNAL(activated()), this, SLOT(cut()));

    m_copyShortcut = new QShortcut(QKeySequence::Copy, m_mainWindow);
    connect(m_copyShortcut, SIGNAL(activated()), this, SLOT(copy()));

    m_pasteShortcut = new QShortcut(QKeySequence::Paste, m_mainWindow);
    connect(m_pasteShortcut, SIGNAL(activated()), this, SLOT(paste()));

    m_undoShortcut = new QShortcut(QKeySequence::Undo, m_mainWindow);
    connect(m_undoShortcut,SIGNAL(activated()),this,SLOT(doUndo()));

    m_redoShortcut = new QShortcut(QKeySequence::Redo, m_mainWindow);
    connect(m_redoShortcut,SIGNAL(activated()),this,SLOT(doRedo()));

    // Toolbar should be shown only when we open a document
    m_ui->viewToolBar->hide();
    m_ui->EditToolBar->hide();
    connect(m_ui->actionPageNumber,SIGNAL(triggered()),this,SLOT(showPreviewDialog()));
    /*
     * Default show time is 7000 milli seconds
     */
    gl_showtime=7000;
    gl_style=0;

    connect(m_ui->actionSelect, SIGNAL(triggered()), this, SLOT(enableSelectTool()));
    connect(m_ui->actionShowCCP, SIGNAL(triggered()), this, SLOT(showCCP()));
    connect(m_ui->actionInsert, SIGNAL(triggered()), this, SLOT(insertButtonClicked()));
    connect(m_ui->actionInsertImage, SIGNAL(triggered()), this, SLOT(insertImage()));
    connect(m_ui->actionInsertTextShape, SIGNAL(triggered()), this, SLOT(insertNewTextShape()));
    connect(m_ui->actionInsertTable, SIGNAL(triggered()), this, SLOT(insertNewTable()));
    m_ui->actionInsertImage->setVisible(false);
    m_ui->actionInsertTextShape->setVisible(false);
    m_ui->actionInsertTable->setVisible(false);
    showCCP();
    insertButtonClicked();

    m_mainWindow->showNormal();
}

ApplicationController::~ApplicationController()
{
    qDeleteAll(m_loadedPlugins.values());
    delete m_ui;
    m_ui = 0;
    delete m_collab;
    delete m_collabDialog;
    delete m_collabEditor;
    delete m_virtualKeyBoard;
}

void ApplicationController::spaceHandlerForVirtualKeyboard()
{
    textEditor()->insertText(" ");
}

void ApplicationController::setVirtualKeyboardVisible(bool set)
{
    if (set && !m_virtualKeyBoard) {
        m_virtualKeyBoard = new VirtualKeyBoard;
    }
    if (m_virtualKeyBoard)
        m_virtualKeyBoard->ShowVirtualKeyBoard(m_mainWindow, textEditor()); // switches
}

bool ApplicationController::isVirtualKeyboardVisible() const
{
    return m_virtualKeyBoard && m_virtualKeyBoard->isVisible();
}

void ApplicationController::showPreviewDialog()
{
    if (documentType() == PresentationDocument) {
        m_storeButtonPreview->showDialog(currentPage());
    }
    //add spreadsheet sheet information frame
    if (documentType() == SpreadsheetDocument) {
        spreadSheetInfo();
    }
}

void ApplicationController::spreadSheetInfo()
{
    if(m_sheetInfoFrame) {
        m_sheetInfoFrame->hide();
        disconnect(m_addSheet,SIGNAL(clicked()),this,SLOT(addSheet()));
        disconnect(m_removeSheet,SIGNAL(clicked()),this,SLOT(removeSheet()));
        delete m_sheetInfoFrame;
        m_sheetInfoFrame=0;
        m_sheetInfoFrameLayout=0;
        m_addSheet=0;
        m_removeSheet=0;
        m_sheetName=0;
        return;
    }
    m_sheetInfoFrame=new QFrame(m_mainWindow);
    m_addSheet=new QPushButton(QString("+"), m_mainWindow);
    m_removeSheet=new QPushButton(QString("-"), m_mainWindow);
    m_sheetName=new QPushButton(m_mainWindow);

    m_sheetInfoFrameLayout=new QGridLayout(m_sheetInfoFrame);
    m_sheetInfoFrameLayout->addWidget(m_removeSheet,0,0);
    m_sheetInfoFrameLayout->addWidget(m_sheetName,0,2,0,2);
    m_sheetInfoFrameLayout->addWidget(m_addSheet,0,5);
    m_sheetInfoFrameLayout->setHorizontalSpacing(0);
    m_sheetInfoFrameLayout->setVerticalSpacing(0);

    m_sheetInfoFrame->setGeometry(SHEETINFOFRAME_XCORDINATE_VALUE,
                               SHEETINFOFRAME_YCORDINATE_VALUE,
                               SHEETINFOFRAME_WIDTH,
                               SHEETINFOFRAME_HEIGHT);

    m_sheetInfoFrame->setLayout(m_sheetInfoFrameLayout);
    m_sheetInfoFrame->show();
    connect(m_addSheet,SIGNAL(clicked()),this,SLOT(addSheet()));
    connect(m_removeSheet,SIGNAL(clicked()),this,SLOT(removeSheet()));
    m_sheetName->setText(currentSheetName());
}

void ApplicationController::openFormatFrame()
{
    if(m_fontstyleframe)
        m_fontstyleframe->hide();

    if(m_formatframe && m_formatframe->isVisible()) {
        m_formatframe->hide();
        return;
    } else if(m_formatframe){
        if (documentType() == TextDocument)
            activeFormatOptionCheck();
        m_formatframe->show();
        return;
    }

    m_formatframe=new QFrame(m_mainWindow);
    Q_CHECK_PTR(m_formatframe);
    m_formatframe->setFrameStyle(QFrame::Sunken);

    m_formatframelayout = new QGridLayout;
    Q_CHECK_PTR(m_formatframelayout);
    m_formatframelayout->setVerticalSpacing(0);
    m_formatframelayout->setHorizontalSpacing(0);

    m_alignleft=addFormatFrameComponent(i18n("AlignLeft"));
    m_alignright=addFormatFrameComponent(i18n("AlignRight"));
    m_aligncenter=addFormatFrameComponent(i18n("AlignCenter"));
    m_bulletlist=addFormatFrameComponent(i18n("Bullets"));
    m_numberedlist=addFormatFrameComponent(i18n("Number"));
    m_alignjustify=addFormatFrameComponent(i18n("AlignJustify"));

    m_bulletlist->setCheckable(false);
    m_numberedlist->setCheckable(false);

    m_formatframelayout->addWidget(m_alignleft,0,0);
    m_formatframelayout->addWidget(m_alignright,0,1);
    m_formatframelayout->addWidget(m_aligncenter,1,0);
    m_formatframelayout->addWidget(m_alignjustify,1,1);
    m_formatframelayout->addWidget(m_numberedlist,2,0);
    m_formatframelayout->addWidget(m_bulletlist,2,1);

    m_formatframe->setGeometry(FORMATRAME_XCORDINATE_VALUE,
                               FORMATFRAME_YCORDINATE_VALUE,
                               FORMATFRAME_WIDTH,
                               FORMATFRAME_HEIGHT);
    m_formatframe->setLayout(m_formatframelayout);
    if (documentType() == TextDocument)
        activeFormatOptionCheck();
    m_formatframe->show();

    connect(m_alignjustify,SIGNAL(clicked()),this,SLOT(doJustify()));
    connect(m_alignleft,SIGNAL(clicked()),this,SLOT(doLeftAlignment()));
    connect(m_alignright,SIGNAL(clicked()),this,SLOT(doRightAlignment()));
    connect(m_aligncenter,SIGNAL(clicked()),this,SLOT(doCenterAlignment()));
    connect(m_numberedlist,SIGNAL(clicked()),this,SLOT(doNumberList()));
    connect(m_bulletlist,SIGNAL(clicked()),this,SLOT(doBulletList()));
}

void ApplicationController::openFontStyleFrame()
{
    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe && m_fontstyleframe->isVisible()) {
        m_fontstyleframe->hide();
        return;
    } else if(m_fontstyleframe){
        if (documentType() == TextDocument || documentType() == SpreadsheetDocument)
            activeFontOptionCheck();
        m_fontstyleframe->show();
        return;
    }
    m_fontstyleframe=new QFrame(m_mainWindow);
    Q_CHECK_PTR(m_fontstyleframe);

    m_fontstyleframelayout=new QGridLayout;
    Q_CHECK_PTR(m_fontstyleframelayout);
    m_fontstyleframelayout->setVerticalSpacing(0);
    m_fontstyleframelayout->setHorizontalSpacing(0);

    m_fontsizebutton=new QPushButton(m_mainWindow);
    Q_CHECK_PTR(m_fontsizebutton);
    m_fontsizebutton->setMinimumSize(100,73);

    m_bold=addFontStyleFrameComponent(i18n("Bold"));
    m_italic=addFontStyleFrameComponent(i18n("Italic"));
    m_underline=addFontStyleFrameComponent(i18n("UnderLine"));
    m_textcolor=addFontStyleFrameComponent(i18n("TextColor"));
    m_textbackgroundcolor=addFontStyleFrameComponent(i18n("TextBackgroundColor"));
    m_subscript=addFontStyleFrameComponent(i18n("SubScript"));
    m_superscript=addFontStyleFrameComponent(i18n("SuperScript"));

    m_textcolor->setCheckable(false);
    m_textbackgroundcolor->setCheckable(false);

    m_fontcombobox=new QFontComboBox(m_mainWindow);
    Q_CHECK_PTR(m_fontcombobox);
    m_fontcombobox->setMinimumSize(230,73);
    m_fontcombobox->setFont(QFont("Nokia Sans",20,QFont::Normal));
    m_fontcombobox->setEditable(false);

    m_fontstyleframelayout->addWidget(m_fontcombobox,0,0,1,2);
    m_fontstyleframelayout->addWidget(m_fontsizebutton,0,2);
    m_fontstyleframelayout->addWidget(m_bold,0,3,1,2);
    m_fontstyleframelayout->addWidget(m_textcolor,1,0);
    m_fontstyleframelayout->addWidget(m_textbackgroundcolor,1,1,1,3);
    m_fontstyleframelayout->addWidget(m_italic,1,4);
    m_fontstyleframelayout->addWidget(m_superscript,2,0);
    m_fontstyleframelayout->addWidget(m_subscript,2,1,1,3);
    m_fontstyleframelayout->addWidget(m_underline,2,4);

    m_fontstyleframe->setLayout(m_fontstyleframelayout);
    m_fontstyleframe->setGeometry(FONTSTYLEFRAME_XCORDINATE_VALUE,
                             FONTSTYLEFRAME_YCORDINATE_VALUE,
                             FONTSTYLEFRAME_WIDTH,
                             FONTSTYLEFRAME_HEIGHT);
    if (documentType() == TextDocument || documentType() == SpreadsheetDocument)
        activeFontOptionCheck();
    m_fontstyleframe->show();

    connect(m_fontsizebutton,SIGNAL(clicked()),SLOT(showFontSizeDialog()));
    connect(m_fontcombobox,SIGNAL(activated(int)),SLOT(selectFontType()));
    connect(m_textcolor,SIGNAL(clicked()),SLOT(selectTextColor()));
    connect(m_textbackgroundcolor,SIGNAL(clicked()),SLOT(selectTextBackGroundColor()));
    connect(m_subscript,SIGNAL(clicked()),SLOT(doSubScript()));
    connect(m_superscript,SIGNAL(clicked()),SLOT(doSuperScript()));
    connect(m_bold,SIGNAL(clicked()),this,SLOT(doBold()));
    connect(m_italic,SIGNAL(clicked()),this,SLOT(doItalic()));
    connect(m_underline,SIGNAL(clicked()),this,SLOT(doUnderLine()));
}

void  ApplicationController::showFontSizeDialog()
{
    if (m_fontstyleframe) {
        m_fontstyleframe->hide();
    }
    m_fontSizeDialog = new QDialog(m_mainWindow);
    Q_ASSERT(m_fontSizeDialog);
    m_fontSizeLineEdit=new QLineEdit(m_fontSizeDialog);
    Q_ASSERT(m_fontSizeLineEdit);
    m_fontSizeList= new QListWidget(m_fontSizeDialog);
    Q_ASSERT(m_fontSizeList);
    m_fontSizeDialogLayout= new QVBoxLayout();
    m_fontSizeDialogLayout->addWidget(m_fontSizeLineEdit);
    m_fontSizeDialogLayout->addWidget(m_fontSizeList);
    m_fontSizeDialog->setWindowTitle(i18n("Font Size"));
    m_fontSizeDialog->setLayout(m_fontSizeDialogLayout);
    m_fontSizeLineEdit->setInputMethodHints(Qt::ImhDigitsOnly);

    int i;
    for(i=4;i<=40;i++)
    {
        QString f_size;
        m_fontSizeList->addItem(f_size.setNum(i));
    }
    int currentFont= m_fontsizebutton->text().toInt();
    if(currentFont>=4 && currentFont<=40) {
        m_fontSizeList->setCurrentRow(currentFont-4);
    }

    m_fontSizeLineEdit->setText(m_fontsizebutton->text());
    m_fontSizeLineEdit->selectAll();
    connect(m_fontSizeList,SIGNAL(itemDoubleClicked (QListWidgetItem *)),SLOT(fontSizeRowSelected( QListWidgetItem * )));
    connect(m_fontSizeLineEdit,SIGNAL(returnPressed()),SLOT(fontSizeEntered()));
    m_fontSizeDialog->exec();
}

void ApplicationController::fontSizeRowSelected(QListWidgetItem *item)
{
     int row=(item->text()).toInt();
     selectFontSize(row+4);
     m_fontSizeDialog->accept();
}

void ApplicationController::fontSizeEntered()
{
    selectFontSize(m_fontSizeLineEdit->text().toInt());
}

void ApplicationController:: openMathOpFrame() {

}

void ApplicationController::addMathematicalOperator(QString mathSymbol)
{
    Q_ASSERT(cellTool()->externalEditor());
    cellTool()->externalEditor()->insertOperator(mathSymbol);
}

///////////////////////
// Styling functions //
///////////////////////

void ApplicationController::doSubScript()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == PresentationDocument) {
           KoPAView *kopaview = qobject_cast<KoPAView *>(view());
           kopaview->kopaCanvas()->toolProxy()->actions()["format_sub"]->trigger();
    }
    if (setSubScript(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatSubScript);

    setDocumentModified(true);
}

bool ApplicationController::setSubScript(KoTextEditor *editor) {
    if (editor){
        if (editor->charFormat().verticalAlignment() == QTextCharFormat::AlignSubScript ){
            editor->setVerticalTextAlignment(Qt::AlignVCenter);
        } else {
            editor->setVerticalTextAlignment(Qt::AlignBottom);
        }
        return true;
    }
    return false;
}

void ApplicationController::doSuperScript()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == PresentationDocument) {
           KoPAView *kopaview = qobject_cast<KoPAView *>(view());
           kopaview->kopaCanvas()->toolProxy()->actions()["format_super"]->trigger();
    }
    if (setSuperScript(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatSuperScript);

    setDocumentModified(true);
}

bool ApplicationController::setSuperScript(KoTextEditor *editor) {
    if (editor) {
        if (editor->charFormat().verticalAlignment() == QTextCharFormat::AlignSuperScript ) {
            editor->setVerticalTextAlignment(Qt::AlignVCenter);
        } else {
            editor->setVerticalTextAlignment(Qt::AlignTop);
        }
        return true;
    }
    return false;
}

void ApplicationController::selectFontSize(int size)
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == SpreadsheetDocument) {
        cellTool()->selectFontSize(size);
    } else {
        if (setFontSize(size, textEditor()) && m_collab)
            m_collab->sendFontSize(textEditor()->selectionStart(), textEditor()->selectionEnd(), size);
    }
    setDocumentModified(true);
    m_fontSizeDialog->hide();
}

bool ApplicationController::setFontSize(int size, KoTextEditor *editor) {
    if (editor) {
        editor->setFontSize(size);
        return true;
    }
    return false;
}

void ApplicationController::selectFontType()
{
    QString selectedfont=m_fontcombobox->currentText();
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == SpreadsheetDocument) {
        cellTool()->selectFontType(selectedfont);
    } else {
        if (setFontType(selectedfont, textEditor()) && m_collab)
            m_collab->sendFontType(textEditor()->selectionStart(), textEditor()->selectionEnd(), selectedfont);
    }
    setDocumentModified(true);
}

bool ApplicationController::setFontType(const QString &font, KoTextEditor *editor) {
    if (editor) {
        editor->setFontFamily(font);
        return true;
    }
    return false;
}

void ApplicationController::selectTextColor()
{
    QColor color = QColorDialog::getColor(Qt::white, m_mainWindow);
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == SpreadsheetDocument) {
        cellTool()->selectTextColor(color);
    } else {
        if (setTextColor(color, textEditor()) && m_collab)
            m_collab->sendTextColor(textEditor()->selectionStart(), textEditor()->selectionEnd(), color.rgb());
    }
    setDocumentModified(true);
}

bool ApplicationController::setTextColor(const QColor &color, KoTextEditor *editor) {
    if (editor) {
        editor->setTextColor(color);
        return true;
    }
    return false;
}

void ApplicationController::selectTextBackGroundColor()
{
    QColor color = QColorDialog::getColor(Qt::white, m_mainWindow);
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == SpreadsheetDocument) {
        cellTool()->selectTextBackgroundColor(color);
    } else {
        if (setTextBackgroundColor(color, textEditor()) && m_collab)
            m_collab->sendTextColor(textEditor()->selectionStart(), textEditor()->selectionEnd(), color.rgb());
    }
    setDocumentModified(true);
}

bool ApplicationController::setTextBackgroundColor(const QColor &color, KoTextEditor* editor) {
    if (editor) {
        editor->setTextBackgroundColor(color);
        return true;
    }
    return false;
}

void ApplicationController::doBold()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == PresentationDocument) {
           KoPAView *kopaview = qobject_cast<KoPAView *>(view());
           kopaview->kopaCanvas()->toolProxy()->actions()["format_bold"]->trigger();
    }
    if (documentType() == SpreadsheetDocument) {
        canvasControllerWidget()->canvas()->toolProxy()->actions()["bold"]->trigger();
    } else {
        if (setBold(textEditor()) && m_collab)
            m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatBold);
    }
    setDocumentModified(true);
}

bool ApplicationController::setBold(KoTextEditor *editor) {
    if(editor) {
        QTextCharFormat textchar = editor->charFormat();
        if (textchar.fontWeight()==QFont::Bold) {
            editor->bold(false);
        } else {
            editor->bold(true);
        }
        return true;
    }
    return false;
}

void ApplicationController::doItalic()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == PresentationDocument) {
           KoPAView *kopaview = qobject_cast<KoPAView *>(view());
           kopaview->kopaCanvas()->toolProxy()->actions()["format_italic"]->trigger();
    }
    if (documentType() == SpreadsheetDocument) {
        canvasController()->canvas()->toolProxy()->actions()["italic"]->trigger();
    } else {
        if (setItalic(textEditor()) && m_collab)
            m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatItalic);
    }
    setDocumentModified(true);
}

bool ApplicationController::setItalic(KoTextEditor *editor) {
    if(editor) {
        QTextCharFormat textchar = editor->charFormat();
        if (textchar.fontItalic()) {
            editor->italic(false);
        } else {
            editor->italic(true);
        }
        return true;
    }
    return false;
}

void ApplicationController::doUnderLine()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if (documentType() == PresentationDocument) {
           KoPAView *kopaview = qobject_cast<KoPAView *>(view());
           kopaview->kopaCanvas()->toolProxy()->actions()["format_underline"]->trigger();
    }
    if (documentType() == SpreadsheetDocument) {
        canvasController()->canvas()->toolProxy()->actions()["underline"]->trigger();
    } else {
        if (setUnderline(textEditor()) && m_collab)
            m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatUnderline);
    }
    setDocumentModified(true);
}

bool ApplicationController::setUnderline(KoTextEditor *editor) {
    if(editor) {
        QTextCharFormat textchar = editor->charFormat();
        if(textchar.property(KoCharacterStyle::UnderlineType).toBool()) {
            editor->underline(false);
        } else {
            editor->underline(true);
        }
        return true;
    }
    return false;
}

void ApplicationController::doLeftAlignment()
{
    if (m_formatframe) {
        m_formatframe->hide();
    }
    if (documentType() == PresentationDocument) {
            KoPAView *kopaview = qobject_cast<KoPAView *>(view());
            kopaview->kopaCanvas()->toolProxy()->actions()["format_alignleft"]->trigger();
    }
    if (setLeftAlign(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatAlignLeft);
}

bool ApplicationController::setLeftAlign(KoTextEditor *editor)
{
    if (editor) {
        editor->setHorizontalTextAlignment(Qt::AlignLeft);
        setDocumentModified(true);
        return true;
    }
    return false;
}

void ApplicationController::doJustify()
{
    if (m_formatframe)
        m_formatframe->hide();
    if (documentType() == PresentationDocument) {
            KoPAView *kopaview = qobject_cast<KoPAView *>(view());
            kopaview->kopaCanvas()->toolProxy()->actions()["format_alignblock"]->trigger();
    }
    if (setJustify(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatAlignJustify);
}

bool ApplicationController::setJustify(KoTextEditor *editor) {
    if (editor) {
        editor->setHorizontalTextAlignment(Qt::AlignJustify);
        setDocumentModified(true);
        return true;
    }
    return false;
}

void ApplicationController::doRightAlignment()
{
    if (m_formatframe)
        m_formatframe->hide();
    if (documentType() == PresentationDocument) {
            KoPAView *kopaview = qobject_cast<KoPAView *>(view());
            kopaview->kopaCanvas()->toolProxy()->actions()["format_alignright"]->trigger();
    }
    if (setRightAlign(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatAlignRight);
}

bool ApplicationController::setRightAlign(KoTextEditor *editor) {
    if (editor) {
        editor->setHorizontalTextAlignment(Qt::AlignRight);
        setDocumentModified(true);
        return true;
    }
    return false;
}

void ApplicationController::doCenterAlignment()
{
    if (m_formatframe)
        m_formatframe->hide();
    if (documentType() == PresentationDocument) {
            KoPAView *kopaview = qobject_cast<KoPAView *>(view());
            kopaview->kopaCanvas()->toolProxy()->actions()["format_aligncenter"]->trigger();
    }
    if (setCenterAlign(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatAlignCenter);
}

bool ApplicationController::setCenterAlign(KoTextEditor *editor) {
    if (editor) {
        editor->setHorizontalTextAlignment(Qt::AlignHCenter);
        setDocumentModified(true);
        return true;
    }
    return true;
}

void ApplicationController::activeFormatOptionCheck()
{
    if (documentType() == PresentationDocument) {
        KoCanvasBase *canvasForChecking = canvasController()->canvas();
        Q_CHECK_PTR(canvasForChecking);
        if (canvasForChecking->toolProxy()->hasSelection()) {
            KoPAView *kopaview = qobject_cast<KoPAView *>(view());
            KoShape *currentShapeSelected = kopaview->shapeManager()->selection()->firstSelectedShape(KoFlake::StrippedSelection);
            KoTextShapeDataBase *currentSelectedTextShapeData = qobject_cast<KoTextShapeDataBase*>(currentShapeSelected->userData());
            QTextDocument *documentForCurrentShape = currentSelectedTextShapeData->document();
            m_pEditor = new KoTextEditor(documentForCurrentShape);
            KoTextDocument(documentForCurrentShape).setUndoStack(undoStack());
            KoTextDocument(documentForCurrentShape).setTextEditor(m_pEditor.data());
            QTextBlockFormat blk = m_pEditor->blockFormat();
            Qt::Alignment textblock_align = blk.alignment();
            switch(textblock_align) {
                case Qt::AlignLeft :
                                m_alignleft->setChecked(true);
                                m_alignjustify->setChecked(false);
                                m_alignright->setChecked(false);
                                m_aligncenter->setChecked(false);
                                break;
                case Qt::AlignRight :
                                m_alignright->setChecked(true);
                                m_alignjustify->setChecked(false);
                                m_alignleft->setChecked(false);
                                m_aligncenter->setChecked(false);
                                break;
                case Qt::AlignCenter :
                                m_aligncenter->setChecked(true);
                                m_alignjustify->setChecked(false);
                                m_alignright->setChecked(false);
                                m_alignleft->setChecked(false);
                                break;
                case Qt::AlignJustify :
                                m_alignjustify->setChecked(true);
                                m_alignleft->setChecked(false);
                                m_alignright->setChecked(false);
                                m_aligncenter->setChecked(false);
                                break;
            }
        }
    }
    else {
        QTextBlockFormat blk = textEditor()->blockFormat();
        Qt::Alignment textblock_align = blk.alignment();
        switch(textblock_align) {
            case Qt::AlignLeft :
                            m_alignleft->setChecked(true);
                            m_alignjustify->setChecked(false);
                            m_alignright->setChecked(false);
                            m_aligncenter->setChecked(false);
                            break;
            case Qt::AlignRight :
                            m_alignright->setChecked(true);
                            m_alignjustify->setChecked(false);
                            m_alignleft->setChecked(false);
                            m_aligncenter->setChecked(false);
                            break;
            case Qt::AlignCenter :
                            m_aligncenter->setChecked(true);
                            m_alignjustify->setChecked(false);
                            m_alignright->setChecked(false);
                            m_alignleft->setChecked(false);
                            break;
            case Qt::AlignJustify :
                            m_alignjustify->setChecked(true);
                            m_alignleft->setChecked(false);
                            m_alignright->setChecked(false);
                            m_aligncenter->setChecked(false);
                            break;
        }
    }
}

void ApplicationController::activeFontOptionCheck()
{
    if (documentType() == PresentationDocument) {
        KoCanvasBase *canvasForChecking = canvasController()->canvas();
        Q_CHECK_PTR(canvasForChecking);
        if(canvasForChecking->toolProxy()->hasSelection())
        {
          KoPAView *kopaview = qobject_cast<KoPAView *>(view());
          KoShape *currentShapeSelected = kopaview->shapeManager()->selection()->firstSelectedShape(KoFlake::StrippedSelection);
          KoTextShapeDataBase *currentSelectedTextShapeData = qobject_cast<KoTextShapeDataBase*>(currentShapeSelected->userData());
          QTextDocument *documentForCurrentShape = currentSelectedTextShapeData->document();
          m_pEditor = new KoTextEditor(documentForCurrentShape);
          KoTextDocument(documentForCurrentShape).setUndoStack(undoStack());
          KoTextDocument(documentForCurrentShape).setTextEditor(m_pEditor.data());
          if(m_superscript) {

            QTextCharFormat textchar = m_pEditor.data()->charFormat();
            if(textchar.verticalAlignment() == QTextCharFormat::AlignSuperScript) {
                m_superscript->setChecked(true);
            } else {
                m_superscript->setChecked(false);
            }
        }

        if(m_subscript) {
            QTextCharFormat textchar = m_pEditor.data()->charFormat();
            if(textchar.verticalAlignment() == QTextCharFormat::AlignSubScript) {
                m_subscript->setChecked(true);
            } else {
                m_subscript->setChecked(false);
            }
        }

        if(m_fontsizebutton) {
            QTextCharFormat textchar = m_pEditor.data()->charFormat();
            QFont font=textchar.font();
            m_fontsizebutton->setText(QString().setNum(font.pointSize()));
        }

        if(m_fontcombobox) {
            QTextCharFormat textchar = m_pEditor.data()->charFormat();
            QString fonttype = textchar.fontFamily();
            m_fontcombobox->setCurrentFont(QFont(fonttype));
        }

        if(m_bold) {
            QTextCharFormat textchar = m_pEditor.data()->charFormat();
            if (textchar.fontWeight()==QFont::Bold) {
                m_bold->setChecked(true);
            } else {
                m_bold->setChecked(false);
            }
        }

        if(m_italic) {
            QTextCharFormat textchar = m_pEditor.data()->charFormat();
            if (textchar.fontItalic()) {
                m_italic->setChecked(true);
            } else {
                m_italic->setChecked(false);
            }
        }

        if(m_underline) {
            QTextCharFormat textchar = m_pEditor.data()->charFormat();
            if (textchar.property(KoCharacterStyle::UnderlineType).toBool()) {
                m_underline->setChecked(true);
            } else {
                m_underline->setChecked(false);
            }
       }
    }
    }

    if (documentType() == TextDocument) {
        if(m_superscript) {
            if (textEditor()->charFormat().verticalAlignment() == QTextCharFormat::AlignSuperScript) {
                m_superscript->setChecked(true);
            } else {
                m_superscript->setChecked(false);
            }
        }

        if(m_subscript) {
            if (textEditor()->charFormat().verticalAlignment() == QTextCharFormat::AlignSubScript) {
                m_subscript->setChecked(true);
            } else {
                m_subscript->setChecked(false);
            }
        }

        if(m_fontsizebutton) {
            QTextCharFormat textchar = textEditor()->charFormat();
            QFont font=textchar.font();
            m_fontsizebutton->setText(QString().setNum(font.pointSize()));
        }

        if(m_fontcombobox) {
            QTextCharFormat textchar = textEditor()->charFormat();
            QString fonttype = textchar.fontFamily();
            m_fontcombobox->setCurrentFont(QFont(fonttype));
        }

        if(m_bold) {
            QTextCharFormat textchar = textEditor()->charFormat();
            if (textchar.fontWeight()==QFont::Bold) {
                m_bold->setChecked(true);
            } else {
                m_bold->setChecked(false);
            }
        }

        if(m_italic) {
            QTextCharFormat textchar = textEditor()->charFormat();
            if (textchar.fontItalic()) {
                m_italic->setChecked(true);
            } else {
                m_italic->setChecked(false);
            }
        }

        if(m_underline) {
            QTextCharFormat textchar = textEditor()->charFormat();
            if(textchar.property(KoCharacterStyle::UnderlineType).toBool()) {
                m_underline->setChecked(true);
            } else {
                m_underline->setChecked(false);
            }
        }
    }

    if (documentType() == SpreadsheetDocument) {
        if(m_fontsizebutton) {
           QString fontsize;
           fontsize.setNum(cellTool()->getFontSize());
           m_fontsizebutton->setText(fontsize);
        }

        if(m_fontcombobox) {
            m_fontcombobox->setCurrentFont(QFont(cellTool()->getFontType()));
        }

        if(m_bold) {
            if (cellTool()->isFontBold()) {
                m_bold->setChecked(true);
            } else {
                m_bold->setChecked(false);
            }
        }

        if(m_italic) {
            if (cellTool()->isFontItalic()) {
                m_italic->setChecked(true);
            } else {
                m_italic->setChecked(false);
            }
        }

        if(m_underline) {
            if(cellTool()->isFontUnderline()) {
                m_underline->setChecked(true);
            } else {
                m_underline->setChecked(false);
            }
        }
    }
}

void ApplicationController::doNumberList()
{
    if(m_formatframe->isVisible())
        m_formatframe->hide();
    if (documentType() == PresentationDocument) {
        KoPAView *kopaview = qobject_cast<KoPAView *>(view());
        KoShape *currentShapeSelected = kopaview->shapeManager()->selection()->firstSelectedShape();
        KoTextShapeDataBase *currentSelectedTextShapeData = qobject_cast<KoTextShapeDataBase*>(currentShapeSelected->userData());
        QTextDocument *documentForCurrentShape = currentSelectedTextShapeData->document();
        m_pEditor = new KoTextEditor(documentForCurrentShape);
        KoTextDocument(documentForCurrentShape).setTextEditor(m_pEditor.data());
        setNumberList(m_pEditor);
    }
    if (setNumberList(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatListNumber);
}

bool ApplicationController::setNumberList(KoTextEditor *editor) {
    if (editor) {
        doStyle(KoListStyle::DecimalItem, editor);
        return true;
    }
    return false;
}

void ApplicationController::doBulletList()
{
    if (m_formatframe->isVisible())
        m_formatframe->hide();
    if (setBulletList(textEditor()) && m_collab)
        m_collab->sendFormat(textEditor()->selectionStart(), textEditor()->selectionEnd(), Collaborate::FormatListBullet);
    if (documentType() == PresentationDocument) {
        KoPAView *kopaview = qobject_cast<KoPAView *>(view());
        KoShape *currentShapeSelected = kopaview->shapeManager()->selection()->firstSelectedShape();
        KoTextShapeDataBase *currentSelectedTextShapeData = qobject_cast<KoTextShapeDataBase*>(currentShapeSelected->userData());
        QTextDocument *documentForCurrentShape = currentSelectedTextShapeData->document();
        m_pEditor = new KoTextEditor(documentForCurrentShape);
        KoTextDocument(documentForCurrentShape).setTextEditor(m_pEditor.data());
        setBulletList(m_pEditor);
    }
}

bool ApplicationController::setBulletList(KoTextEditor *editor) {
    if (editor) {
        doStyle(KoListStyle::Bullet, editor);
        return true;
    }
    return false;
}

void ApplicationController::doStyle(KoListStyle::Style style, KoTextEditor *editor)
{
    KoParagraphStyle *parag = new KoParagraphStyle();
    KoListStyle *list = new KoListStyle(parag);
    KoListLevelProperties llp = list->levelProperties(0);
    llp.setStyle(style);
    list->setLevelProperties(llp);
    parag->setListStyle(list);
    editor->setStyle(parag);
    setDocumentModified(true);
}

QString ApplicationController::showGetOpenFileNameDialog(const QString& caption,
                                                         const QString& dir,
                                                         const QString& filter)
{
    Q_UNUSED(caption);
    Q_UNUSED(dir);
    Q_UNUSED(filter);
    FileChooserDialog fileDialog(m_mainWindow);
    fileDialog.exec();
    return fileDialog.getFilePath();
}

QString ApplicationController::showGetSaveFileNameDialog(const QString& caption,
                                                         const QString& dir,
                                                         const QString& filter)
{
    QList<HildonMenu *> all_dlg = findChildren<HildonMenu *>();
    foreach(HildonMenu *menu, all_dlg)
        menu->close();

    return QFileDialog::getSaveFileName(m_mainWindow, caption, dir, filter,
                                        0, QFileDialog::DontUseNativeDialog);
}

void ApplicationController::chooseDocumentType()
{
    m_docdialog = new QDialog(m_mainWindow);
    Q_CHECK_PTR(m_docdialog);

    m_docdialoglayout = new QGridLayout;
    Q_CHECK_PTR(m_docdialoglayout);

    m_document = addNewDocument("Document");
    m_presenter = addNewDocument("Presenter");
    m_spreadsheet = addNewDocument("SpreadSheet");

    m_docdialoglayout->addWidget(m_document,0,0);
    m_docdialoglayout->addWidget(m_presenter,0,1);
    m_docdialoglayout->addWidget(m_spreadsheet,0,2);

    m_docdialog->setLayout(m_docdialoglayout);
    m_docdialog->show();

    connect(m_document,SIGNAL(clicked()),this,SLOT(openNewDoc()));
    connect(m_presenter,SIGNAL(clicked()),this,SLOT(openNewPresenter()));
    connect(m_spreadsheet,SIGNAL(clicked()),this,SLOT(openNewSpreadSheet()));
}

void ApplicationController::openNewDoc()
{
    m_docdialog->close();
    openDocumentAsTemplate(NEW_WORDDOC);
}

void ApplicationController::openNewPresenter()
{
    m_docdialog->close();
    templateSelectionDialog();
}

void ApplicationController::openNewSpreadSheet()
{
   m_docdialog->close();
   openDocumentAsTemplate(NEW_SPREADSHEET);
}

void ApplicationController::templateSelectionDialog()
{
    if(m_tempselectiondialog){
        m_tempselectiondialog->show();
        return;
    }
    const QDir temppath(NEW_PRESENTER);
    m_temptitle <<temppath.entryList(QDir::Files);

    m_tempselectiondialog = new QDialog(m_mainWindow);
    Q_CHECK_PTR(m_tempselectiondialog);
    m_tempselectiondialog->setWindowTitle(i18n("Select Presentation Template Preview"));

    m_tempdialoglayout = new QGridLayout;
    Q_CHECK_PTR(m_tempdialoglayout);
    m_tempdialoglayout->setVerticalSpacing(0);
    m_tempdialoglayout->setHorizontalSpacing(0);

    m_templateWidget = new QListWidget(m_mainWindow);
    Q_CHECK_PTR(m_templateWidget);
    QStringList tempNames;
    tempNames<<m_temptitle;
    tempNames.replaceInStrings("_"," ");
    tempNames.replaceInStrings(".odp","");
    m_templateWidget->addItems(tempNames);
    m_templateWidget->setMinimumSize(500,200);

    m_go = new QPushButton("Load", m_mainWindow);
    Q_CHECK_PTR(m_go);

    m_closetemp = new QPushButton(i18n("Cancel"), m_mainWindow);
    Q_CHECK_PTR(m_closetemp);

    m_templatepreview = new QLabel(m_mainWindow);
    Q_CHECK_PTR(m_templatepreview);

    m_tempdialoglayout->addWidget(m_templateWidget,0,0,1,2);
    m_tempdialoglayout->addWidget(m_templatepreview,0,2);
    m_tempdialoglayout->addWidget(m_go,1,0);
    m_tempdialoglayout->addWidget(m_closetemp,1,1);

    m_tempselectiondialog->setLayout(m_tempdialoglayout);
    m_tempselectiondialog->show();

    connect(m_templateWidget,SIGNAL(currentRowChanged(int)),SLOT(selectedTemplatePreview(int)));
    connect(m_go,SIGNAL(clicked()),SLOT(openSelectedTemplate()));
    connect(m_closetemp,SIGNAL(clicked()),SLOT(closeTempSelectionDialog()));
}

void ApplicationController::selectedTemplatePreview(int number)
{
    QString path = NEW_PRESENTER+m_temptitle.at(number);
    KoStore::Backend backend = KoStore::Auto;
    KoStore * store = KoStore::createStore(path, KoStore::Read, "", backend);
    if(store->open("/Thumbnails/thumbnail.png")) {
        QByteArray content=store->device()->readAll();
        QPixmap thumbnail;
        thumbnail.loadFromData(content);
        store->close();
        m_templatepreview->setPixmap(thumbnail);
    }
    delete store;
}

void ApplicationController::openSelectedTemplate()
{
    newpresenter = NEW_PRESENTER+m_temptitle.at(m_templateWidget->currentRow());
    m_tempselectiondialog->accept();
    openDocumentAsTemplate(newpresenter);
}

void ApplicationController::closeTempSelectionDialog()
{
    if(m_tempselectiondialog)
        m_tempselectiondialog->hide();
}

QPushButton * ApplicationController::addFormatFrameComponent(const QString &imagepath)
{
    QPushButton * btn = new QPushButton(m_mainWindow);
    Q_CHECK_PTR(btn);
    btn->setCheckable(true);
    btn->setIcon(QIcon(":/images/48x48/Edittoolbaricons/"+imagepath+".png"));
    btn->setMaximumSize(165,75);
    return btn;
}

QPushButton * ApplicationController::addFontStyleFrameComponent(const QString &imagepath)
{
    QPushButton * btn = new QPushButton(m_mainWindow);
    Q_CHECK_PTR(btn);
    btn->setCheckable(true);
    btn->setIcon(QIcon(":/images/48x48/Edittoolbaricons/"+imagepath+".png"));
    btn->setMinimumSize(165,70);
    btn->setMaximumSize(165,70);
    return btn;
}

QToolButton * ApplicationController ::addNewDocument(const QString &docname)
{
    QToolButton * toolbutton = new QToolButton(m_mainWindow);
    Q_CHECK_PTR(toolbutton);
    toolbutton->setIcon(QIcon(":/images/48x48/"+docname+".png"));
    toolbutton->setToolTip(docname);
    return toolbutton;
}

void ApplicationController::openAboutDialog(void)
{
    QList<HildonMenu *> all_dlg = findChildren<HildonMenu *>();
    foreach(HildonMenu *menu, all_dlg)
    menu->close();
    AboutDialog dialog(m_mainWindow);
    dialog.exec();
}

void ApplicationController::toggleToolBar(bool show)
{
    KoToolManager::instance()->switchToolRequested(panToolFactoryId());
    if (documentType() == SpreadsheetDocument) {
        KoToolManager::instance()->switchToolRequested(cellToolFactoryId());
        Calligra::Tables::DocBase *kspreadDoc = qobject_cast<Calligra::Tables::DocBase*>(document());
        kspreadDoc->map()->setReadWrite(!show);
    }
    document()->setReadWrite(!show);
    m_ui->viewToolBar->setVisible(!show);
    m_ui->SearchToolBar->setVisible(show);
    m_isViewToolBar = !show;
    if (show) {
        m_search->setFocus();
        m_search->selectAll();
    }
    else {
        m_search->clearFocus();
    }
}

bool ApplicationController::setEditingMode(bool set)
{
    if (!KoAbstractApplicationController::setEditingMode(set))
        return false;

    if (set) {
        //set the icon to be checked
        m_ui->actionEdit->blockSignals(true);
        if(! m_ui->actionEdit->isChecked()) {
            m_ui->actionEdit->setChecked(true);
        }
        m_ui->actionEdit->blockSignals(false);

        switch (documentType()) {
        case SpreadsheetDocument:
            m_ui->viewToolBar->hide();
            if(m_spreadEditToolBar) {
                m_spreadEditToolBar->show();
            }
            m_ui->EditToolBar->hide();
            break;
        default:
            m_ui->viewToolBar->hide();
            m_ui->EditToolBar->show();
            if(m_spreadEditToolBar) {
                m_spreadEditToolBar->hide();
            }
            m_spaceHandlerShortcutForVirtualKeyBoard->setEnabled(true);
            m_shortcutForVirtualKeyBoard->setEnabled(true);
        }
    }
    else {
        m_ui->EditToolBar->hide();
        m_ui->viewToolBar->show();
        if(m_spreadEditToolBar) {
            m_spreadEditToolBar->hide();
        }
        m_isViewToolBar = true;

        m_spaceHandlerShortcutForVirtualKeyBoard->setEnabled(false);
        m_shortcutForVirtualKeyBoard->setEnabled(false);

        m_ui->actionEdit->blockSignals(true);
        if(m_ui->actionEdit->isChecked()) {
            m_ui->actionEdit->setChecked(false);
        }
        m_ui->actionEdit->blockSignals(false);
    }

    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
    return true;
}

void ApplicationController::doUndo()
{
    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();

    if (undoStack() && undoStack()->canUndo()) {
        undoStack()->undo();
    }
}

void ApplicationController::doRedo()
{
    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();

    if (undoStack() && undoStack()->canRedo()) {
        undoStack()->redo();
    }
}

void ApplicationController::copy()
{
    if(   documentType() != PresentationDocument && documentType() != SpreadsheetDocument
       && !canvasController()->canvas()->toolProxy()->hasSelection())
    {
        return;
    }
    canvasController()->canvas()->toolProxy()->copy();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_formatframe)
        m_formatframe->hide();
}

void ApplicationController::cut()
{
    if(   documentType() != PresentationDocument && documentType() != SpreadsheetDocument
       && !canvasController()->canvas()->toolProxy()->hasSelection())
    {
        return;
    }
    canvasController()->canvas()->toolProxy()->copy();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_formatframe)
        m_formatframe->hide();
}

void ApplicationController::paste()
{
    canvasController()->canvas()->toolProxy()->paste();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_formatframe)
        m_formatframe->hide();
}

void ApplicationController::showFullScreenPresentationIcons()
{
    if (!canvasController())
        return;
    int vScrlbarWidth = 0;
    int hScrlbarHeight = 0;
    QSize size(m_mainWindow->frameSize());

    if (canvasControllerWidget()->verticalScrollBar()->isVisible()) {
        QSize vScrlbar = canvasControllerWidget()->verticalScrollBar()->size();
        vScrlbarWidth = vScrlbar.width();
    }

    if (canvasControllerWidget()->horizontalScrollBar()->isVisible()) {
        QSize hScrlbar = canvasControllerWidget()->horizontalScrollBar()->size();
        hScrlbarHeight = hScrlbar.height();
    }

    if (!m_fsPPTBackButton && m_presentationTool && !m_presentationTool->toolsActivated()) {
        m_fsPPTBackButton = new QPushButton(m_mainWindow);
#ifdef Q_WS_MAEMO_5
        m_fsPPTBackButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
#endif
        m_fsPPTBackButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTBackButton->setIcon(QIcon(FS_PPT_BACK_BUTTON_PATH));
        m_fsPPTBackButton->setIconSize(QSize(48, 48));
        connect(m_fsPPTBackButton, SIGNAL(clicked()), this, SLOT(goToPreviousPage()));
        m_fsPPTBackButton->move(size.width() - FS_BUTTON_SIZE*3 - vScrlbarWidth,
                                size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if (!m_fsPPTForwardButton && m_presentationTool && !m_presentationTool->toolsActivated()) {
        m_fsPPTForwardButton = new QPushButton(m_mainWindow);
#ifdef Q_WS_MAEMO_5
        m_fsPPTForwardButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
#endif
        m_fsPPTForwardButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTForwardButton->setIcon(QIcon(FS_PPT_FORWARD_BUTTON_PATH));
        m_fsPPTForwardButton->setIconSize(QSize(48, 48));
        connect(m_fsPPTForwardButton, SIGNAL(clicked()), this, SLOT(goToNextPage()));
        m_fsPPTForwardButton->move(size.width() - FS_BUTTON_SIZE*2 - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if (currentPage() <= document()->pageCount() && currentPage() != 1 && m_presentationTool && !m_presentationTool->toolsActivated())
    {
        m_fsPPTBackButton->show();
        m_fsPPTBackButton->raise();
    }

    if (!m_fsPPTDrawPenButton && m_presentationTool) {
        m_fsPPTDrawPenButton = new QPushButton(m_mainWindow);
#ifdef Q_WS_MAEMO_5
        m_fsPPTDrawPenButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
#endif
        m_fsPPTDrawPenButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTDrawPenButton->setIcon(QIcon(":/images/64x64/PresentationDrawTool/pen.png"));
        m_fsPPTDrawPenButton->setIconSize(QSize(48, 48));
        m_fsPPTDrawPenButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE*3 - hScrlbarHeight);
        connect(m_fsPPTDrawPenButton,SIGNAL(clicked()),m_presentationTool,SLOT(togglePenTool()));
    }

    m_fsPPTDrawPenButton->show();
    m_fsPPTDrawPenButton->raise();

    if (!m_fsPPTDrawHighlightButton && m_presentationTool) {
        m_fsPPTDrawHighlightButton = new QPushButton(m_mainWindow);
#ifdef Q_WS_MAEMO_5
        m_fsPPTDrawHighlightButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
#endif
        m_fsPPTDrawHighlightButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTDrawHighlightButton->setIcon(QIcon(":/images/64x64/PresentationDrawTool/highlight.png"));
        m_fsPPTDrawHighlightButton->setIconSize(QSize(48, 48));
        m_fsPPTDrawHighlightButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                                         size.height() - FS_BUTTON_SIZE*2 - hScrlbarHeight);
        connect(m_fsPPTDrawHighlightButton,SIGNAL(clicked()),m_presentationTool,SLOT(toggleHighlightTool()));
    }

    m_fsPPTDrawHighlightButton->show();
    m_fsPPTDrawHighlightButton->raise();

    if (currentPage() < document()->pageCount() && m_presentationTool && !m_presentationTool->toolsActivated()) {
        m_fsPPTForwardButton->move(size.width() - FS_BUTTON_SIZE*2 - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
        m_fsPPTForwardButton->show();
        m_fsPPTForwardButton->raise();
    }

    if(documentType() == PresentationDocument) {
        if (!m_slideNotesButton) {
            m_slideNotesButton = new QPushButton(m_mainWindow);
            Q_CHECK_PTR(m_slideNotesButton);
#ifdef Q_WS_MAEMO_5
            m_slideNotesButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
#endif
            m_slideNotesButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
            m_slideNotesButton->setIcon(m_slideNotesIcon);
            m_slideNotesButton->setIconSize(QSize(48, 48));
            connect(m_slideNotesButton, SIGNAL(clicked()), SLOT(slideNotesButtonClicked()));
            m_slideNotesButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                                     size.height() - FS_BUTTON_SIZE*4 - hScrlbarHeight);
        }
        m_slideNotesButton->show();
        m_slideNotesButton->raise();

#ifdef Q_WS_MAEMO_5
        if (!m_fsAccButton && m_presentationTool) {
             m_fsAccButton = new QPushButton(m_mainWindow);
             m_fsAccButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
             m_fsAccButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
             m_fsAccButton->setIcon(QIcon(":/images/64x64/Acceleration/swingoff.png"));
             m_fsAccButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                                      size.height() - FS_BUTTON_SIZE*5 - hScrlbarHeight);
             connect(m_fsAccButton, SIGNAL(clicked(bool)), SLOT(switchToSlid()));
        }
        m_fsAccButton->show();
        m_fsAccButton->raise();
#endif
    }
}
////////////////////////////////////////////////acc/////////////////////////////
#ifdef Q_WS_MAEMO_5
void ApplicationController::switchToSlid()
{

     m_enableAccelerator = !m_enableAccelerator;
     if (!m_enableAccelerator) {
           m_fsAccButton->setIcon(QIcon(":/images/64x64/Acceleration/swingoff.png"));
           m_acceleratorForScrolAndSlide.stopRecognition();
           disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(next()),m_fsPPTForwardButton,SLOT(click()));
           disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(previous()),m_fsPPTBackButton,SLOT(click()));

     }
     else {

           m_fsAccButton->setIcon(QIcon(":/images/64x64/Acceleration/swingon.png"));
           m_acceleratorForScrolAndSlide.startRecognitionSlide();
           disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(next()),m_fsPPTForwardButton,SLOT(click()));
           disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(previous()),m_fsPPTBackButton,SLOT(click()));
           connect(&m_acceleratorForScrolAndSlide,SIGNAL(next()),m_fsPPTForwardButton,SLOT(click()));
           connect(&m_acceleratorForScrolAndSlide,SIGNAL(previous()),m_fsPPTBackButton,SLOT(click()));
      }

 }
void ApplicationController::enableDisableScrollingOption()
{

    if (m_enableScrolling) {
        switchToScroll();
    m_stopAcceleratorScrolling = !m_stopAcceleratorScrolling;
}


void ApplicationController::switchToScroll()
{
   m_enableScrolling = !m_enableScrolling;

   if (m_enableScrolling) {
       m_acceleratorForScrolAndSlide.startRecognitionScroll();
       connect(&m_acceleratorForScrolAndSlide,SIGNAL(change()),this,SLOT(scrollAction()));

   }
   else {
       disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(change()),this,SLOT(scrollAction()));
       m_acceleratorForScrolAndSlide.stopRecognitionScroll();
   }
}

void ApplicationController::scrollAction()
{
    canvasControllerWidget()->verticalScrollBar()->
                  setSliderPosition (
                    (canvasControllerWidget()->verticalScrollBar()->sliderPosition()
                    + m_acceleratorForScrolAndSlide.getVerticalScrollValue())
                  );
    canvasControllerWidget()->horizontalScrollBar()->
                  setSliderPosition (
                    (canvasControllerWidget()->horizontalScrollBar()->sliderPosition()
                    + m_acceleratorForScrolAndSlide.getHorizontalScrollValue())
                  );
    m_acceleratorForScrolAndSlide.resetScrollValues();
}

void ApplicationController::closeAcceleratorSettings()
{
    if (m_enableAccelerator) {
        m_fsAccButton->setIcon(QIcon(":/images/64x64/Acceleration/swingoff.png"));
        disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(next()),m_fsPPTForwardButton,SLOT(click()));
        disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(previous()),m_fsPPTBackButton,SLOT(click()));

        m_acceleratorForScrolAndSlide.stopRecognition();
        m_enableAccelerator = !m_enableAccelerator;
    }
    if (m_enableScrolling) {
        disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(change()),this,SLOT(scrollAction()));
        m_acceleratorForScrolAndSlide.stopRecognitionScroll();
        m_enableScrolling = false;
    }
    disconnect(m_shortcutForAccelerator,SIGNAL(activated()),this,SLOT(enableDisableScrollingOption()));
    disconnect(&m_acceleratorForScrolAndSlide,SIGNAL(StopTheAccelerator()),this,SLOT(switchToScroll()));
}

#endif
//////////////////////////////////////////////////////////////////////////////

void ApplicationController::slideTransitionDialog(){
    m_slidingmotiondialog = new SlidingMotionDialog;
    m_slidingmotiondialog->m_select = gl_style ;
    m_slidingmotiondialog->m_time = gl_showtime / 1000;
    m_slidingmotiondialog->showDialog(m_mainWindow);
#ifdef HAVE_OPENGL
    connect(m_slidingmotiondialog, SIGNAL(startglshow(int,int)), SLOT(glPresenterSet(int,int)));
#endif
#ifdef Q_WS_MAEMO_5
   connect(m_slidingmotiondialog->m_opengl, SIGNAL(clicked()),&m_acceleratorForScrolAndSlide,SLOT(startSlideSettings()));
   connect(m_slidingmotiondialog->m_acceleration,SIGNAL(clicked()),&m_acceleratorForScrolAndSlide,SLOT(startScrollSettings()));
#endif

}

void ApplicationController::slideNotesButtonClicked()
{
    if(m_notesDialog) {
        disconnect(m_notesDialog,SIGNAL(gotoPage(int)),this,SLOT(gotoPage(int)));
        disconnect(m_notesDialog,SIGNAL(moveSlide(bool)),this,SLOT(moveSlideFromNotesSlide(bool)));
        delete m_notesDialog;
    }
    m_notesDialog = new NotesDialog(document(), m_viewNumber, m_storeButtonPreview->thumbnailList());
    m_notesDialog->show();
    connect(m_notesDialog,SIGNAL(moveSlide(bool)),this,SLOT(moveSlideFromNotesSlide(bool)));
    connect(m_notesDialog,SIGNAL(gotoPage(int)),this,SLOT(gotoPage(int)));

    m_notesDialog->show();
    m_notesDialog->showNotesDialog(currentPage());
}

void ApplicationController::moveSlideFromNotesSlide(bool flag)
{
    if (flag) {
        goToNextPage();
    } else {
        goToPreviousPage();
    }
}

bool ApplicationController::openDocuments(const KoAbstractApplicationOpenDocumentArguments& args)
{
    const bool result = KoAbstractApplicationController::openDocuments(args);

    if (documentType() == PresentationDocument) {
        //code related to the button previews
        delete m_storeButtonPreview;
        m_storeButtonPreview = new StoreButtonPreview(document(), view());
        connect(m_storeButtonPreview, SIGNAL(goToPage(int)), this, SLOT(goToPage(int)));
    }
    return result;
}

// Reimplemented
void ApplicationController::closeDocument()
{
    KoAbstractApplicationController::closeDocument();

    if (m_digitalSignatureDialog && documentType() == TextDocument) {
        disconnect(m_ui->actionDigitalSignature,SIGNAL(triggered()),this,SLOT(showDigitalSignatureInfo()));
        QMenuBar* menu = m_mainWindow->menuBar();
        menu->removeAction(m_ui->actionDigitalSignature);
        delete m_digitalSignatureDialog;
        m_digitalSignatureDialog = 0;
    }
    switch (documentType()) {
    case PresentationDocument:
        if (m_presentationTool) {
            disconnect(m_fsButton, SIGNAL(clicked()), m_presentationTool, SLOT(deactivateTool()));
        }
        break;
    case SpreadsheetDocument:
        if (m_spreadEditToolBar)
            m_spreadEditToolBar->hide();
        resetSpreadEditorToolBar();
        if (m_sheetInfoFrame && m_sheetInfoFrame->isVisible()) {
            spreadSheetInfo();
        }
        break;
    default:
        break;
    }

    if (m_ui->EditToolBar)
        m_ui->EditToolBar->hide();

    if(m_ui->viewToolBar)
        m_ui->viewToolBar->hide();

    if(m_ui->SearchToolBar)
        m_ui->SearchToolBar->hide();

    m_ui->actionZoomLevel->setText(i18n("%1 %", 100));
    m_ui->actionPageNumber->setText(i18n("%1 of %2", 0, 0));

    fontStyleFrameDestructor();
    formatFrameDestructor();

    delete m_slidingmotiondialog;
    m_slidingmotiondialog = 0;

    m_viewNumber++;
}

void ApplicationController::formatFrameDestructor() {
    if(m_formatframe) {
        m_formatframe->hide();

        delete m_alignright;
        m_alignright = 0;
        delete m_alignleft;
        m_alignleft = 0;
        delete m_alignjustify;
        m_alignjustify = 0;
        delete m_aligncenter;
        m_aligncenter = 0;
        delete m_numberedlist;
        m_numberedlist = 0;
        delete m_bulletlist;
        m_bulletlist = 0;
        delete m_formatframe ;
        m_formatframe = 0;
     }
}

void ApplicationController::fontStyleFrameDestructor() {
    if(m_fontstyleframe) {
        m_fontstyleframe->close();
        delete m_bold;
        m_bold = 0;
        delete m_italic;
        m_italic = 0;
        delete m_underline;
        m_underline = 0;
        delete m_subscript;
        m_subscript = 0;
        delete m_superscript;
        m_superscript = 0;
        delete m_fontcombobox;
        m_fontcombobox = 0;
        delete m_fontsizebutton;
        m_fontsizebutton = 0;
        delete m_textcolor;
        m_textcolor = 0;
        delete m_textbackgroundcolor;
        m_textbackgroundcolor = 0;
        delete m_fontstyleframe;
        m_fontstyleframe = 0;
    }
}

void ApplicationController::raiseWindow()
{
#ifdef Q_WS_X11
    Display *display = QX11Info::display();
    XEvent e;
    Window root = RootWindow(display, DefaultScreen(display));
    memset(&e, 0, sizeof(e));
    e.xclient.type         = ClientMessage;
    e.xclient.window       = m_mainWindow->effectiveWinId();
    e.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    e.xclient.format       = 32;
    XSendEvent(display, root, False, SubstructureRedirectMask, &e);
    XFlush(display);
#endif
}

// reimplemented
bool ApplicationController::openScheduledDocument()
{
    if (!KoAbstractApplicationController::openScheduledDocument())
        return false;

    m_search->setText(QString());

#ifdef Q_WS_MAEMO_5
    connect(m_shortcutForAccelerator,SIGNAL(activated()),this,SLOT(enableDisableScrollingOption()));
    connect(&m_acceleratorForScrolAndSlide,SIGNAL(StopTheAccelerator()),this,SLOT(switchToScroll()));
#endif
    return true;
}

// reimplemented
void ApplicationController::resourceChanged(int key, const QVariant& value)
{
    if (m_presentationTool && m_presentationTool->toolsActivated() && documentType() == PresentationDocument) {
        return;
    }
    KoAbstractApplicationController::resourceChanged(key, value);
}

void ApplicationController::handleDocumentPageSetupChanged()
{
    KoAbstractApplicationController::handleDocumentPageSetupChanged();

    if (!view() || !m_ui)
        return;

    QString pageNo = i18n("pg%1 - pg%2", 0, 0);
    int factor = 100;

    if (document()->pageCount() > 0) {
        factor = view()->zoomController()->zoomAction()->effectiveZoom() * 100;
        pageNo = i18n("pg%1 - pg%2", 1, QString::number(document()->pageCount()));
    }

    if (documentType() == SpreadsheetDocument) {
        Calligra::Tables::DocBase *kspreadDoc = qobject_cast<Calligra::Tables::DocBase*>(document());
        if (kspreadDoc->map()->count() > 0)
            pageNo = i18n("pg%1 - pg%2", 1, QString::number(kspreadDoc->map()->count()));
    }

    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
    m_ui->actionPageNumber->setText(pageNo);

    m_vPage = canvasControllerWidget()->verticalScrollBar()->pageStep();
    m_hPage = canvasControllerWidget()->horizontalScrollBar()->pageStep();
}

void ApplicationController::fullScreen()
{
    if (!m_ui)
        return;
    m_ui->viewToolBar->hide();
    m_ui->SearchToolBar->hide();
    m_ui->EditToolBar->hide();
    if (documentType() == PresentationDocument) {
        emit presentationStarted();
    }
    m_mainWindow->showFullScreen();
    QSize size(m_mainWindow->frameSize());

    int vScrlbarWidth = 0;
    int hScrlbarHeight = 0;
    if (canvasControllerWidget()) {
        if (canvasControllerWidget()->verticalScrollBar()->isVisible()) {
            QSize vScrlbar = canvasControllerWidget()->verticalScrollBar()->size();
            vScrlbarWidth = vScrlbar.width();
        }
        if (canvasControllerWidget()->horizontalScrollBar()->isVisible()) {
            QSize hScrlbar = canvasControllerWidget()->horizontalScrollBar()->size();
            hScrlbarHeight = hScrlbar.height();
        }
    }

    m_fsButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                     size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    m_fsButton->show();
    m_fsButton->raise();
    m_fsTimer->start(3000);

    if (documentType() == PresentationDocument) {
        if (!m_presentationTool) {
            m_presentationTool = new PresentationTool(m_mainWindow, canvasControllerWidget());
            connect(m_fsButton, SIGNAL(clicked()), m_presentationTool, SLOT(deactivateTool()));
        }
        showFullScreenPresentationIcons();
    }
}

void ApplicationController::handleMainWindowResizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QSize size(m_mainWindow->frameSize());

    int vScrlbarWidth = 0;
    int hScrlbarHeight = 0;
    if (canvasControllerWidget()) {
        if (canvasControllerWidget()->verticalScrollBar()->isVisible()) {
            QSize vScrlbar = canvasControllerWidget()->verticalScrollBar()->size();
            vScrlbarWidth = vScrlbar.width();
        }
        if (canvasControllerWidget()->horizontalScrollBar()->isVisible()) {
            QSize hScrlbar = canvasControllerWidget()->horizontalScrollBar()->size();
            hScrlbarHeight = hScrlbar.height();
        }
    }

    m_fsButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                     size.height() - FS_BUTTON_SIZE - hScrlbarHeight);

    if (m_fsPPTBackButton) {
        m_fsPPTBackButton->move(size.width() - FS_BUTTON_SIZE*3 - vScrlbarWidth,
                                size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if (m_fsPPTForwardButton) {
        m_fsPPTForwardButton->move(size.width() - FS_BUTTON_SIZE*2 - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if (m_fsPPTDrawPenButton) {
        m_fsPPTDrawPenButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE*3 - hScrlbarHeight);
    }

    if (m_fsPPTDrawHighlightButton) {
        m_fsPPTDrawHighlightButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                                         size.height() - FS_BUTTON_SIZE*2 - hScrlbarHeight);
    }

    if (m_slideNotesButton) {
        m_slideNotesButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                                 size.height() - FS_BUTTON_SIZE*4 - hScrlbarHeight);
    }
}

void ApplicationController::zoomIn()
{
    if (!view() || !m_ui)
        return;
    KoZoomAction *zAction =  view()->zoomController()->zoomAction();
    int factor = zAction->effectiveZoom() * 100;
    if(factor<199)
    {
        zAction->zoomIn();
    }
    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
}

void ApplicationController::zoomOut()
{
    if (!view() || !m_ui)
        return;
    KoZoomAction *zAction = view()->zoomController()->zoomAction();
    int factor = zAction->effectiveZoom() * 100;
    if(factor>70)
    {
        zAction->zoomOut();
    }
    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
}

void ApplicationController::zoom()
{
    if (documentType() == SpreadsheetDocument) {
        ZoomDialog dlg(m_mainWindow);
        connect(&dlg, SIGNAL(fitPage()), SLOT(zoomToPage()));
        connect(&dlg, SIGNAL(fitPageWidth()), SLOT(zoomToPageWidth()));
        dlg.exec();
    }
}

void ApplicationController::zoomToPage()
{
    if (!view() || !m_ui)
        return;
    view()->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);
    if (documentType() == PresentationDocument)
        m_ui->actionZoomLevel->setText("48%");
    else
        m_ui->actionZoomLevel->setText("29%");
}

void ApplicationController::zoomToPageWidth()
{
    if (!view() || !m_ui)
        return;
    view()->zoomController()->setZoomMode(KoZoomMode::ZOOM_WIDTH);
    if (documentType() == PresentationDocument)
        m_ui->actionZoomLevel->setText("84%");
    else
        m_ui->actionZoomLevel->setText("100%");
}

/*void ApplicationController::prevPage()
{
    if (!canvasController())
        return;
    if ((m_doc->pageCount() > 0) && triggerAction("page_previous"))
        return;
    m_vPage = canvasControllerWidget()->verticalScrollBar()->pageStep();
    canvasController()->pan(QPoint(0, -m_vPage));
}

void ApplicationController::nextPage()
{
    if (!canvasController())
        return;
    if ((m_doc->pageCount() > 0) && triggerAction("page_next"))
        return;
    m_vPage = canvasControllerWidget()->verticalScrollBar()->pageStep();
    canvasController()->pan(QPoint(0, m_vPage));
}*/

void ApplicationController::fsTimer()
{
    if (!m_pressed) {
        m_fsTimer->stop();
        m_fsButton->hide();
        if (m_fsPPTBackButton && m_fsPPTBackButton->isVisible())
            m_fsPPTBackButton->hide();
        if (m_fsPPTForwardButton && m_fsPPTForwardButton->isVisible())
            m_fsPPTForwardButton->hide();
        if (m_fsPPTDrawHighlightButton && m_fsPPTDrawHighlightButton->isVisible() )
            m_fsPPTDrawHighlightButton->hide();
        if (m_fsPPTDrawPenButton && m_fsPPTDrawPenButton->isVisible() )
            m_fsPPTDrawPenButton->hide();
        if (m_slideNotesButton && m_slideNotesButton->isVisible())
            m_slideNotesButton->hide();
#ifdef Q_WS_MAEMO_5
        if (m_fsAccButton && m_fsAccButton->isVisible())
            m_fsAccButton->hide();
#endif
    }
}

void ApplicationController::fsButtonClicked()
{
    if (!m_ui)
        return;

    if (canvasController()) {
        canvasControllerWidget()->show();
    }

    if (documentType() == PresentationDocument) {
        emit presentationStopped();
    }

    m_fsButton->hide();

    if (m_fsPPTBackButton && m_fsPPTBackButton->isVisible())
        m_fsPPTBackButton->hide();

    if (m_fsPPTForwardButton && m_fsPPTForwardButton->isVisible())
        m_fsPPTForwardButton->hide();

    if (m_fsPPTDrawHighlightButton && m_fsPPTDrawHighlightButton->isVisible() )
        m_fsPPTDrawHighlightButton->hide();

    if (m_fsPPTDrawPenButton && m_fsPPTDrawPenButton->isVisible() )
        m_fsPPTDrawPenButton->hide();

    if (m_slideNotesButton && m_slideNotesButton->isVisible())
        m_slideNotesButton->hide();

#ifdef Q_WS_MAEMO_5
    if (m_fsAccButton && m_fsAccButton->isVisible())
        m_fsAccButton->hide();
#endif

    if (m_isViewToolBar)
        m_ui->viewToolBar->show();
    else
        m_ui->SearchToolBar->show();

    m_mainWindow->showNormal();
}

static void findTextShapesRecursive(KoShapeContainer* con, KoPAPageBase* page,
                                    QList<QPair<KoPAPageBase*, KoShape*> >& shapes,
                                    QList<QTextDocument*>& docs)
{
    foreach(KoShape* shape, con->shapes()) {
        KoTextShapeDataBase* tsd = qobject_cast<KoTextShapeDataBase*> (shape->userData());
        if (tsd) {
            shapes.append(qMakePair(page, shape));
            docs.append(tsd->document());
        }
        KoShapeContainer* child = dynamic_cast<KoShapeContainer*>(shape);
        if (child)
            findTextShapesRecursive(child, page, shapes, docs);
    }
}

void ApplicationController::startSearch()
{
    if (!m_search || !canvasController())
        return;

    QString searchString = m_search->text();
    if (documentType() == SpreadsheetDocument && !searchString.isEmpty()) {
        //for spreadsheet use the find method in FoCellTool.
        cellTool()->setCaseSensitive(m_searchCaseSensitive);
        cellTool()->slotSearchTextChanged(searchString);
        m_ui->actionSearchResult->setText(i18n("%1 of %2", cellTool()->currentSearchStatistics().first,
                                               cellTool()->currentSearchStatistics().second));
        goToNextWord();
        return;
    }

    if(m_search->text()==""){
        KoCanvasBase *canvas = canvasController()->canvas();
        KoSelection *selection = canvas->shapeManager()->selection();
        selection->deselectAll();
        m_ui->actionSearchResult->setText(i18n("%1 of %2", 0, 0));
        return;
    }

    KoCanvasBase *canvas = canvasController()->canvas();
    Q_CHECK_PTR(canvas);

    KoPADocument* padoc = qobject_cast<KoPADocument*>(document());
    if (padoc) {
        // loop over all pages starting from current page to get
        // search results in the right order
        int curPage = canvas->resourceManager()->resource(\
                      KoCanvasResourceManager::CurrentPage).toInt() - 1;
        QList<QPair<KoPAPageBase*, KoShape*> > textShapes;
        QList<QTextDocument*> textDocs;
        for (int page = 0; page < padoc->pageCount(); page++) {
            KoPAPageBase* papage = padoc->pageByIndex(page, false);
            findTextShapesRecursive(papage, papage, textShapes, textDocs);
        };

        findText(textDocs, textShapes, searchString);

        // now find the first search result in the list of positions
        // counting from the current page
        // this is not very efficient...
        bool foundIt = false;
        for (int page = curPage; page < padoc->pageCount(); page++) {
            for (int i = 0; i < m_searchTextPositions.size(); i++) {
                if (m_searchTextPositions[i].first.first == padoc->pageByIndex(page, false)) {
                    foundIt = true;
                    m_searchTextIndex = i;
                    highlightText(m_searchTextIndex);
                    break;
                }
            }
            if (foundIt) break;
        }
        if (!foundIt) {
            for (int page = 0; page < curPage; page++) {
                for (int i = 0; i < m_searchTextPositions.size(); i++) {
                    if (m_searchTextPositions[i].first.first ==
                            padoc->pageByIndex(page, false)) {
                        foundIt = true;
                        m_searchTextIndex = i;
                        highlightText(m_searchTextIndex);
                        break;
                    }
                }
                if (foundIt) break;
            }
        }
    } else {
        KoShapeManager *manager = canvas->shapeManager();
        Q_CHECK_PTR(manager);
        QList<KoShape*> shapes = manager->shapes();

        int size = shapes.size();
        if (size != 0) {
            QList<KoTextShapeDataBase*> shapeDatas;
            QList<QTextDocument*> textDocs;
            QList<QPair<KoPAPageBase*, KoShape*> > textShapes;
            QSet<QTextDocument*> textDocSet;

            for (int i = 0; i < size; i++) {
                shapeDatas.append(qobject_cast<KoTextShapeDataBase*> \
                                  (shapes.at(i)->userData()));
                if (shapeDatas.at(i) && !textDocSet.contains(\
                        shapeDatas.at(i)->document())) {
                    textDocSet.insert(shapeDatas.at(i)->document());
                    textDocs.append(shapeDatas.at(i)->document());
                    textShapes.append(qMakePair((KoPAPageBase*)(0),
                                                shapes.at(i)));
                }
            }
            findText(textDocs, textShapes, searchString);
        }
    }
}

void ApplicationController::findText(QList<QTextDocument*> aDocs,
                          QList<QPair<KoPAPageBase*, KoShape*> > shapes,
                          const QString &aText)
{
    if (aDocs.isEmpty())
        return;

    m_searchTextPositions.clear();

    for (int i = 0; i < aDocs.size(); i++) {
        QTextDocument* doc = aDocs.at(i);
        KoShape* shape = shapes.at(i).second;

        QTextCursor result(doc);
        do {
            if (!m_searchCaseSensitive)
                result = doc->find(aText, result);
            else
                result = doc->find(aText, result, QTextDocument::FindCaseSensitively);
            if (result.hasSelection()) {
                m_searchTextPositions.append(qMakePair(qMakePair(shapes.at(i).first, shape),
                                             qMakePair(result.selectionStart(),
                                                       result.selectionEnd())));
            }
        } while (!result.isNull());
    }

    m_searchTextIndex = 0;
    if (!m_searchTextPositions.isEmpty())
        highlightText(m_searchTextIndex);
    else
        m_ui->actionSearchResult->setText(i18n("%1 of %2", 0, 0));
}

void ApplicationController::highlightText(int aIndex)
{
    if (!canvasController() || !m_ui)
        return;

    KoCanvasBase *canvas = canvasController()->canvas();
    Q_CHECK_PTR(canvas);

    // first potentially go to the correct page
    KoPAPageBase* page = m_searchTextPositions.at(aIndex).first.first;
    if (page) {
        KoPAView* paview = static_cast<KoPAView*>(view());
        if (paview->activePage() != page) {
            paview->doUpdateActivePage(page);
        }
    }

    // secondly set the currently selected text shape to the one containing this search result
    KoSelection *selection = canvas->shapeManager()->selection();
    KoShape* shape = m_searchTextPositions.at(aIndex).first.second;
    if (selection->count() != 1 || selection->firstSelectedShape() != shape) {
        selection->deselectAll();
        selection->select(shape);
    }
    // ugly hack, but if we don't first disable and than re-enable the text tool
    // it will still keep the wrong textshape selected
    KoToolManager::instance()->switchToolRequested(panToolFactoryId());
    KoToolManager::instance()->switchToolRequested(textToolFactoryId());

    KoCanvasResourceManager *provider = canvas->resourceManager();
    Q_CHECK_PTR(provider);

    QString sizeStr = QString::number(m_searchTextPositions.size());
    QString indexStr = QString::number(aIndex + 1);

    m_ui->actionSearchResult->setText(i18n("%1 of %2",aIndex + 1,m_searchTextPositions.size() ));

    provider->setResource(KoText::CurrentTextPosition,
                          m_searchTextPositions.at(aIndex).second.first);
    provider->setResource(KoText::CurrentTextAnchor,
                          m_searchTextPositions.at(aIndex).second.second);
}

void ApplicationController::goToPreviousWord()
{
    if (documentType() == SpreadsheetDocument) {
        cellTool()->findPrevious();
        m_ui->actionSearchResult->setText(i18n("%1 of %2", cellTool()->currentSearchStatistics().first,
                                               cellTool()->currentSearchStatistics().second));
        return;
    }

    if (m_searchTextPositions.isEmpty())
        return;
    if (m_searchTextIndex == 0) {
        m_searchTextIndex = m_searchTextPositions.size() - 1;
    } else {
        m_searchTextIndex--;
    }
    highlightText(m_searchTextIndex);
}

void ApplicationController::goToNextWord()
{
    if (documentType() == SpreadsheetDocument) {
        cellTool()->findNext();
        m_ui->actionSearchResult->setText(i18n("%1 of %2", cellTool()->currentSearchStatistics().first,
                                               cellTool()->currentSearchStatistics().second));
        return;
    }

    if (m_searchTextPositions.isEmpty())
        return;
    if (m_searchTextIndex == m_searchTextPositions.size() - 1) {
        m_searchTextIndex = 0;
    } else {
        m_searchTextIndex++;
    }

    highlightText(m_searchTextIndex);
}

void ApplicationController::searchOptionChanged(int aCheckBoxState)
{
    m_searchCaseSensitive = aCheckBoxState == Qt::Checked;
    startSearch();
}

void ApplicationController::updateActions()
{
    bool docLoaded = document();
    m_ui->actionSearch->setEnabled(docLoaded);
    m_ui->actionEdit->setEnabled(docLoaded);
    m_ui->actionFullScreen->setEnabled(docLoaded);
    m_ui->actionZoomIn->setEnabled(docLoaded);
    m_ui->actionZoomOut->setEnabled(docLoaded);
    m_ui->actionZoomLevel->setEnabled(docLoaded);
    m_ui->actionNextPage->setEnabled(docLoaded);
    m_ui->actionPrevPage->setEnabled(docLoaded);
    m_ui->actionFormat->setVisible(docLoaded && (documentType() == TextDocument || documentType() == PresentationDocument));
    m_ui->actionSlidingMotion->setVisible(documentType() == PresentationDocument);
}

void ApplicationController::setCentralWidget(QWidget *widget)
{
    m_mainWindow->setCentralWidget(widget);
    if (!widget)
        return;

    switch (documentType()) {
    case TextDocument:
        break;
    case PresentationDocument:
        break;
    case SpreadsheetDocument:
        setUpSpreadEditorToolBar();
        break;
    default:
        break;
    };
}

QWidget* ApplicationController::mainWindow() const
{
    return m_mainWindow;
}

bool ApplicationController::handleMainWindowEventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu)
        return true;

    // TODO: refine the collaborative-editing section of eventFilter
    if (textEditor() && m_collab) {
        if(event->type() == 6 && QString::compare("KWCanvas", watched->metaObject()->className())== 0 )
        {
            //qDebug()<<"111111111111111"<<watched->metaObject()->className();
            QKeyEvent *ke=static_cast<QKeyEvent *>(event);
            if( ke->key() == Qt::Key_Backspace )
                m_collab->sendBackspace(textEditor()->selectionStart(), textEditor()->selectionEnd());
            else if (((ke->key()>=Qt::Key_Space) && (ke->key()<=Qt::Key_AsciiTilde)) || ke->key() == Qt::Key_Return )
                m_collab->sendString(textEditor()->selectionStart(), textEditor()->selectionEnd(), ke->text().toUtf8());
            else
                qDebug() << "Collaborate: Unsupported key: " << ke->key() << " (" << ke->text() << ")";
        }
    }

    // show buttons in full screen mode if user taps anywhere in the screen
    if (event && m_mainWindow->isFullScreen()) {

        if (event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::TabletPress) {
            m_pressed = true;
            m_panningCount = 0;
            m_slideChangePossible = true;
            m_fsTimer->start(3000);
            m_pressPos = (reinterpret_cast<QMouseEvent*>(event))->pos();
        }
        else if (   event->type() == QEvent::MouseButtonRelease
                   || event->type() == QEvent::TabletRelease)
        {
            m_pressed = false;
            //show buttons only if user just tap the screen without
            //panning document
            if (m_panningCount <= 5) {
                m_fsButton->show();
                m_fsButton->raise();
#ifdef Q_WS_MAEMO_5

                //tap on the fullscreen starts and stops the scrolling functionality

                if (!m_stopAcceleratorScrolling)
                {
                    qDebug() << "this is the scroll stoppping and starting option"
                        << m_stopAcceleratorScrolling;
                    switchToScroll();
                }
#endif
                if (documentType() == PresentationDocument) {
                    showFullScreenPresentationIcons();
                }
                m_fsTimer->start(3000);
                m_slideChangePossible = false;
            }
            m_panningCount = 0;
        }
        else if ((event->type() == QEvent::TabletMove ||
                    event->type() == QEvent::MouseMove) && m_pressed) {
            int sliderMin = canvasControllerWidget()->verticalScrollBar()->minimum();
            int sliderVal = canvasControllerWidget()->verticalScrollBar()->value();
            int sliderMax = canvasControllerWidget()->verticalScrollBar()->maximum();
            if (sliderVal == sliderMin || sliderVal == sliderMax)
                m_panningCount++;
        }

        if (documentType() == PresentationDocument && m_slideChangePossible
                && m_panningCount > 5 && (event->type() == QEvent::MouseMove
                                          || event->type() == QEvent::TabletMove))
        {
            int sliderMin = canvasControllerWidget()->verticalScrollBar()->minimum();
            int sliderVal = canvasControllerWidget()->verticalScrollBar()->value();
            int sliderMax = canvasControllerWidget()->verticalScrollBar()->maximum();
            QPoint movePos = (reinterpret_cast<QMouseEvent*>(event))->pos();
            if (movePos.y() - m_pressPos.y() > 50 && sliderVal == sliderMin && m_presentationTool && !m_presentationTool->toolsActivated())
            {
                m_slideChangePossible = false;
                triggerAction("page_previous");
                emit previousSlide();
            }
            if (   m_pressPos.y() - movePos.y() > 50
                && sliderVal == sliderMax
                && m_presentationTool
                && !m_presentationTool->toolsActivated())
            {
                m_slideChangePossible = false;
                triggerAction("page_next");
                emit nextSlide();
            }
        }
    }
    // Maemo Qt hardcodes handling of F6 to toggling full screen directly, so
    // override that shortcut to do what we want it to do instead.
    if (event && event->type() == QEvent::Shortcut) {
        QShortcutEvent *qse = static_cast<QShortcutEvent*>(event);
        if (qse->key() == QKeySequence(Qt::Key_F6)) {
            if (m_ui->actionFullScreen->isEnabled())
                fullScreen();
            return true;
        } else if (qse->key() == QKeySequence(Qt::Key_F4)) {
            showApplicationMenu();
            return true;
        }
    } else if (event->type() == QEvent::ShortcutOverride && m_mainWindow->isFullScreen()) {
        // somehow shortcuts aren't working properly in full-screen mode...
        QKeyEvent* qke = reinterpret_cast<QKeyEvent*>(event);
        if (qke->key() == Qt::Key_F7) {
            m_ui->actionZoomIn->trigger();
            return true;
        } else if (qke->key() == Qt::Key_F8) {
            m_ui->actionZoomOut->trigger();
            return true;
        }
    }

    // change presentation slide in fullscreen mode if user taps on
    // left or right side of the screen
    if (document()) {
        if (   m_presentationTool && !m_presentationTool->toolsActivated()
            && watched && event && documentType() == PresentationDocument && pageCount() > 0
            && m_mainWindow->isFullScreen()
            && event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = reinterpret_cast<QMouseEvent*>(event);
            // check that event wasn't from full screen push button
            if (QString::compare("QPushButton", watched->metaObject()->className())) {
                QSize size(m_mainWindow->frameSize());
                if (mouseEvent->x() <= FS_BUTTON_SIZE) {
                    triggerAction("page_previous");
                    emit previousSlide();
                } else if (mouseEvent->x() >= (size.width() - FS_BUTTON_SIZE)) {
                    triggerAction("page_next");
                    emit nextSlide();
                }
            }
        }
    }

    if(event && event->type()==QEvent::MouseButtonPress && document()) {
        QMouseEvent *mouseEvent= reinterpret_cast<QMouseEvent*>(event);
        m_xcoordinate = mouseEvent->globalX ();
        m_ycoordinate = mouseEvent->globalY();
        if((m_formatframe) && (m_formatframe->isVisible())) {
            if ((m_xcoordinate<480) || (m_ycoordinate<205))
                m_formatframe->hide();
        }
        if((m_fontstyleframe) && (m_fontstyleframe->isVisible())) {
            if (!m_mainWindow->isActiveWindow()){
            } else {
                if((m_xcoordinate<325) || (m_ycoordinate<205))
                    m_fontstyleframe->hide();
            }
        }
    }

    if (document() && event && event->type() == 7 && documentType() == TextDocument) {
         QKeyEvent *ke=static_cast<QKeyEvent *>(event);
         if(ke->key()!=Qt::Key_Up && ke->key()!=Qt::Key_Down &&
            ke->key()!=Qt::Key_Right && ke->key()!=Qt::Key_Left &&
            ke->key()!=Qt::Key_Shift && ke->key()!=Qt::Key_Alt &&
            ke->key()!=Qt::Key_Control && ke->key()!=Qt::Key_CapsLock &&
            ke->key()!=Qt::Key_Backspace ){
            setDocumentModified(true);
            if(m_firstChar && m_mainWindow->centralWidget()->hasFocus()){
                 m_mainWindow->centralWidget()->clearFocus();
                 canvasControllerWidget()->setFocus();
                 m_firstChar=false;
            }
         }
    }

    if (textEditor() && event->type() == QEvent::MouseButtonDblClick && m_mainWindow->isActiveWindow()) {
        if(m_ui->actionEdit->isChecked())
            textEditor()->setPosition(textEditor()->position(),QTextCursor::MoveAnchor);
        return true;
    }
    if (event->type() == QEvent::MouseButtonDblClick && documentType() == SpreadsheetDocument) {
        setDocumentModified(true);
        return false;
    }
    if (event->type() == QEvent::MouseButtonDblClick && documentType() == PresentationDocument) {
        setEditingMode(!editingMode());
    }
    return false;
}

void ApplicationController::showApplicationMenu()
{
    HildonMenu menu(m_mainWindow);
    menu.exec();
}

//Function to check if application has been started by DBus
void ApplicationController::checkDBusActivation()
{
    if (splash() && !m_mainWindow->isActiveWindow())
        openDocument();
}

void ApplicationController::pluginOpen(bool /*newWindow*/, const QString& path)
{
    openDocument(path);
}

void ApplicationController::menuClicked(QAction* action)
{
    if (!action->data().toBool())
    {
        return; // We return if it was not a plugin action
    }

    const QString activeText = action->text();
    OfficeInterface *nextPlugin = m_loadedPlugins.value(activeText);
    nextPlugin->setDocument(document());
    QWidget *pluginView = nextPlugin->view();
    pluginView->setParent(m_mainWindow);
    pluginView->setWindowFlags(Qt::Dialog);
    pluginView->show();
}

void ApplicationController::loadScrollAndQuit()
{
    // if no document is loaded, simply quit
    if (!document() || !canvasController()) {
        QTimer::singleShot(1, qApp, SLOT(quit()));
        return;
    }
    // if still loading, wait some more
    if (document()->isLoading()) {
        QTimer::singleShot(100, this, SLOT(loadScrollAndQuit()));
        return;
    }
    // when done loading, start proceeding to the next page until the end of the document has been
    // reached
    bool done;
    if (documentType() == PresentationDocument) {
        done = currentPage() == document()->pageCount();
    } else {
        QScrollBar *v = canvasControllerWidget()->verticalScrollBar();
        done = v->value() >= v->maximum();
    }
    if (done) {
        QTimer::singleShot(1, qApp, SLOT(quit()));
    } else {
        goToNextPage();
        QTimer::singleShot(20, this, SLOT(loadScrollAndQuit()));
    }
}

void ApplicationController::handleMainWindowMousePressEvent( QMouseEvent *event )
{
    if (m_presentationTool && m_presentationTool->toolsActivated()) {
        m_presentationTool->handleMainWindowMousePressEvent( event );
    }
}

void ApplicationController::handleMainWindowMouseMoveEvent( QMouseEvent *event )
{
    if (m_presentationTool && m_presentationTool->toolsActivated()) {
        m_presentationTool->handleMainWindowMouseMoveEvent( event );
    }
}

void ApplicationController::handleMainWindowMouseReleaseEvent( QMouseEvent *event )
{
    if (m_presentationTool && m_presentationTool->toolsActivated()) {
        m_presentationTool->handleMainWindowMouseReleaseEvent( event );
    }
}

void ApplicationController::handleMainWindowPaintEvent( QPaintEvent */*event*/ )
{
    if (!m_presentationTool) {
        return;
    }

    if (m_presentationTool->toolsActivated()) {
        QPainter painter(m_mainWindow);
        painter.drawImage( QRectF(QPointF(0,0), m_mainWindow->size()),
                          *m_presentationTool->getImage(),
                          QRectF(QPointF(0,0), m_presentationTool->getImage()->size()));
    }

    if (!m_presentationTool->getPenToolStatus() && !m_presentationTool->getHighlightToolStatus() && canvasControllerWidget()) {
        canvasControllerWidget()->show();
    }
}

void ApplicationController::setFullScreenPresentationNavigationEnabled(bool set)
{
    m_fsPPTBackButton->setEnabled(set);
    m_fsPPTForwardButton->setEnabled(set);
    m_fsPPTBackButton->setVisible(set);
    m_fsPPTForwardButton->setVisible(set);
    if (set) {
        m_fsPPTBackButton->raise();
        m_fsPPTForwardButton->raise();
    }
}

void ApplicationController::setUpSpreadEditorToolBar()
{
    if (documentType() != SpreadsheetDocument)
        return;

    if (!m_ui)
        return;

    if (!cellTool() || !cellTool()->externalEditor() || cellTool()->externalEditor()->thisWidget())
        return;

    delete m_spreadEditToolBar;
    m_spreadEditToolBar=0;
    m_spreadEditToolBar = new QToolBar();
    m_spreadEditToolBar->setIconSize(QSize(48,48));
    m_mainWindow->addToolBar(Qt::BottomToolBarArea, m_spreadEditToolBar);
    m_spreadEditToolBar->addAction(m_ui->actionEdit);
    m_spreadEditToolBar->addSeparator();
    m_spreadEditToolBar->addAction(m_ui->actionCut);
    m_spreadEditToolBar->addAction(m_ui->actionCopy);
    m_spreadEditToolBar->addAction(m_ui->actionPaste);
    m_spreadEditToolBar->addAction(m_ui->actionUndo);
    m_spreadEditToolBar->addAction(m_ui->actionRedo);
    m_spreadEditToolBar->addAction(m_ui->actionStyle);
    m_spreadEditToolBar->addAction(m_ui->actionFormat);

    m_spreadEditToolBar->insertWidget(m_ui->actionCut, cellTool()->externalEditor()->thisWidget());

    delete m_addAction;
    m_addAction = new QAction(this);
    m_addAction->setIcon(QIcon(":/images/48x48/Edittoolbaricons/Plus.png"));
    delete m_subtractAction;
    m_subtractAction=0;
    m_subtractAction= new QAction(this);
    m_subtractAction->setIcon(QIcon(":/images/48x48/Edittoolbaricons/Subtract.png"));
    delete m_multiplyAction;
    m_multiplyAction=0;
    m_multiplyAction= new QAction(this);
    m_multiplyAction->setIcon(QIcon(":/images/48x48/Edittoolbaricons/Multiply.png"));
    delete m_divideAction;
    m_divideAction=0;
    m_divideAction= new QAction(this);
    m_divideAction->setIcon(QIcon(":/images/48x48/Edittoolbaricons/Divide.png"));
    delete m_percentageAction;
    m_percentageAction=0;
    m_percentageAction= new QAction(this);
    m_percentageAction->setIcon(QIcon(":/images/48x48/Edittoolbaricons/Percentage.png"));
    delete m_equalsAction;
    m_equalsAction=0;
    m_equalsAction= new QAction(this);
    m_equalsAction->setIcon(QIcon(":/images/48x48/Edittoolbaricons/Equal.png"));

    m_spreadEditToolBar->addAction(m_addAction);
    m_spreadEditToolBar->addAction(m_subtractAction);
    m_spreadEditToolBar->addAction(m_multiplyAction);
    m_spreadEditToolBar->addAction(m_divideAction);
    m_spreadEditToolBar->addAction(m_percentageAction);
    m_spreadEditToolBar->addAction(m_equalsAction);

    delete m_signalMapper;
    m_signalMapper=0;
    m_signalMapper= new QSignalMapper(this);
    m_signalMapper->setMapping(m_addAction,QString("+"));
    m_signalMapper->setMapping(m_subtractAction,QString("-"));
    m_signalMapper->setMapping(m_multiplyAction,QString("*"));
    m_signalMapper->setMapping(m_divideAction,QString("/"));
    m_signalMapper->setMapping(m_percentageAction,QString("%"));
    m_signalMapper->setMapping(m_equalsAction,QString("="));

    connect(m_addAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    connect(m_subtractAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    connect(m_multiplyAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    connect(m_divideAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    connect(m_percentageAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    connect(m_equalsAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));

    connect(m_signalMapper,SIGNAL(mapped(QString)),this,SLOT(addMathematicalOperator(QString)));

    m_addAction->setVisible(false);
    m_subtractAction->setVisible(false);
    m_multiplyAction->setVisible(false);
    m_divideAction->setVisible(false);
    m_percentageAction->setVisible(false);
    m_equalsAction->setVisible(false);

    m_spreadEditToolBar->addAction(m_ui->actionMathOp);
    m_ui->actionMathOp->setVisible(true);
    m_spreadEditToolBar->hide();
}

void ApplicationController::resetSpreadEditorToolBar()
{
    if (documentType() != SpreadsheetDocument)
        return;
    m_ui->actionMathOp->setChecked(false);
    m_ui->actionCut->setVisible(true);
    m_ui->actionCopy->setVisible(true);
    m_ui->actionPaste->setVisible(true);
    m_ui->actionUndo->setVisible(true);
    m_ui->actionRedo->setVisible(true);
    m_ui->actionStyle->setVisible(true);
    m_ui->actionFormat->setVisible(true);
    cellTool()->externalEditor()->thisWidget()->hide();
    cellTool()->externalEditor()->thisWidget()->releaseKeyboard();

    disconnect(m_signalMapper,SIGNAL(mapped(QString)),this,SLOT(addMathematicalOperator(QString)));
    disconnect(m_addAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    disconnect(m_subtractAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    disconnect(m_multiplyAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    disconnect(m_divideAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    disconnect(m_percentageAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));
    disconnect(m_equalsAction,SIGNAL(triggered()),m_signalMapper,SLOT(map()));

    if (m_spreadEditToolBar)
        m_mainWindow->removeToolBar(m_spreadEditToolBar);

    delete m_addAction;
    m_addAction=0;
    delete m_subtractAction;
    m_subtractAction=0;
    delete m_multiplyAction;
    m_multiplyAction=0;
    delete m_divideAction;
    m_divideAction=0;
    delete m_percentageAction;
    m_percentageAction=0;
    delete m_equalsAction;
    m_equalsAction=0;
    delete m_spreadEditToolBar;
    m_spreadEditToolBar=0;
}

void ApplicationController::startMathMode(bool start)
{
    if (documentType() != SpreadsheetDocument)
        return;
    m_addAction->setVisible(start);
    m_subtractAction->setVisible(start);
    m_multiplyAction->setVisible(start);
    m_divideAction->setVisible(start);
    m_percentageAction->setVisible(start);
    m_equalsAction->setVisible(start);
    m_ui->actionCut->setVisible(!start);
    m_ui->actionCopy->setVisible(!start);
    m_ui->actionPaste->setVisible(!start);
    m_ui->actionUndo->setVisible(!start);
    m_ui->actionRedo->setVisible(!start);
    m_ui->actionStyle->setVisible(!start);
    m_ui->actionFormat->setVisible(!start);
}

///////////////////////////
// Collaborative editing //
///////////////////////////

void ApplicationController::collaborateDialog()
{
    if(m_collab)
    {
        QMessageBox::warning(m_mainWindow, i18n("Collaborative Editing"),
                             i18n("An active session already exists"), QMessageBox::Ok);
        return ;
    }
    else
    {
        delete m_collabDialog;
        m_collabDialog = 0;
        m_collabDialog = new CollabDialog(m_mainWindow);

        connect(m_collabDialog, SIGNAL(accepted()), this, SLOT(startCollaborating()));
        //connect(m_collabDialog, SIGNAL(finished(int)), this, SLOT(startCollaborating(int)));
        connect(m_collabDialog, SIGNAL(rejected()), this, SLOT(collaborationCancelled()));

        m_collabDialog->show();
    }

}

void ApplicationController::closeCollabDialog() {
    qDebug() << "Collaborate-Dialog: closeDialog() called";
    //delete m_collabDialog;
    //m_collabDialog = 0;
    m_collabDialog->hide();
    qDebug() << "Collaborate-Dialog: closeDialog() done";
}

void ApplicationController::startCollaborating() {
    qDebug() << "Collaborate-Dialog: start ";
    if (m_collabDialog->isClient()) {
        if (document()) {
            qDebug() << "A document is already open. Cannot start client.";
            return closeCollabDialog();
        }
        m_collab = new CollabClient(m_collabDialog->getNick(),
                                    m_collabDialog->getAddress(),
                                    m_collabDialog->getPort(),
                                    this);
        connect(m_collab, SIGNAL(openFile(const QString&)), this, SLOT(collabOpenFile(QString)));

    } else if (m_collabDialog->isServer()) {
        if (!document()) {
            qDebug() << "No document is open. Cannot start server.";
            return closeCollabDialog();
        }
        m_collab = new CollabServer(m_collabDialog->getNick(),
                                    documentFileName(),
                                    m_collabDialog->getPort(),
                                    this);
        connect(m_collab, SIGNAL(saveFile(const QString&)), this, SLOT(collabSaveFile(const QString&)));
        m_collabEditor = new KoTextEditor(const_cast<QTextDocument*>(textEditor()->document()));

    } else {
        return closeCollabDialog();
    }

    //qDebug() << "888888888888888888888888888888888888888";
    //m_collabEditor =
    //qDebug() << "999999999999999999999999999999999999999";

    connect(m_collab, SIGNAL(receivedBackspace(uint,uint)), this, SLOT(receivedBackspace(uint,uint)));
    connect(m_collab, SIGNAL(receivedFormat(uint,uint,Collaborate::FormatFlag)), this, SLOT(receivedFormat(uint,uint,Collaborate::FormatFlag)));
    connect(m_collab, SIGNAL(receivedString(uint,uint,QByteArray)), this, SLOT(receivedString(uint,uint,QByteArray)));
    connect(m_collab, SIGNAL(receivedFontSize(uint,uint,uint)), this, SLOT(receivedFontSize(uint,uint,uint)));
    connect(m_collab, SIGNAL(receivedTextColor(uint,uint,QRgb)), this, SLOT(receivedTextColor(uint,uint,QRgb)));
    connect(m_collab, SIGNAL(receivedTextBackgroundColor(uint,uint,QRgb)), this, SLOT(receivedTextBackgroundColor(uint,uint,QRgb)));
    connect(m_collab, SIGNAL(receivedFontType(uint,uint,QString)), this, SLOT(receivedFontType(uint,uint,QString)));
    connect(m_collab, SIGNAL(error(quint16)), this, SLOT(error(quint16)));

    closeCollabDialog();
    qDebug() << "Collaborate-Dialog: end ----------------------------------------- ";
}

void ApplicationController::collaborationCancelled() {
    closeCollabDialog();
}

void ApplicationController::receivedString(uint start, uint end, QByteArray msg) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    m_collabEditor->insertText(msg);
}

void ApplicationController::receivedBackspace(uint start, uint end) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    m_collabEditor->deletePreviousChar();
}

void ApplicationController::receivedFormat(uint start, uint end, Collaborate::FormatFlag format) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);

    switch(format) {
    case Collaborate::FormatAlignCenter:
        setCenterAlign(m_collabEditor);
        break;
    case Collaborate::FormatAlignJustify:
        setJustify(m_collabEditor);
        break;
    case Collaborate::FormatAlignLeft:
        setLeftAlign(m_collabEditor);
        break;
    case Collaborate::FormatAlignRight:
        setRightAlign(m_collabEditor);
        break;
    case Collaborate::FormatBold:
        setBold(m_collabEditor);
        break;
    case Collaborate::FormatItalic:
        setItalic(m_collabEditor);
        break;
    case Collaborate::FormatListBullet:
        setBulletList(m_collabEditor);
        break;
    case Collaborate::FormatListNumber:
        setNumberList(m_collabEditor);
        break;
    case Collaborate::FormatSubScript:
        setSubScript(m_collabEditor);
        break;
    case Collaborate::FormatSuperScript:
        setSuperScript(m_collabEditor);
        break;
    case Collaborate::FormatUnderline:
        setUnderline(m_collabEditor);
        break;
    default:
        qDebug() << "Collaborate: Unknown format flag";
    }
}

void ApplicationController::receivedFontSize(uint start, uint end, uint size) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setFontSize(size, m_collabEditor);
}

void ApplicationController::receivedFontType(uint start, uint end, const QString &font) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setFontType(font, m_collabEditor);
}

void ApplicationController::receivedTextColor(uint start, uint end, QRgb color) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setTextColor(QColor(color), m_collabEditor);
}

void ApplicationController::receivedTextBackgroundColor(uint start, uint end, QRgb color) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setTextBackgroundColor(QColor(color), m_collabEditor);
}

void ApplicationController::error(quint16 err) {
    qDebug() << "Collaborate: Error: " << err;
}

void ApplicationController::collabSaveFile(const QString &filename) {
    document()->saveNativeFormat(filename);
}

void ApplicationController::collabOpenFile(const QString &filename) {
    openDocument(filename);
    qDebug() << "============================================";
    m_collabEditor = new KoTextEditor(const_cast<QTextDocument*>(textEditor()->document()));
    qDebug() << "::::::::::::::::::::::::::::::::::::::::::::";
}

///////////////////////////
///////////////////////////
#ifdef HAVE_OPENGL
void ApplicationController::glPresenter()
{
    if (documentType() == PresentationDocument) {
    /*
    int i=0;
    QList <QPixmap> pixm;
    while(i < document()->pageCount())
    {
        KoPADocument* slide = qobject_cast<KoPADocument*>(m_doc);
        KoPAPageBase* cur_slide = slide->pageByIndex(i, false);
        pixm << cur_slide->thumbnail();
        i++;
    }
    */
        presenter = new GLPresenter(m_mainWindow, gl_style, gl_showtime, m_storeButtonPreview->thumbnailList());
    }
}

void ApplicationController::glPresenterSet(int a , int b)
{
    gl_showtime = b * 1000;
    gl_style = a;
}
#endif

void ApplicationController::showDigitalSignatureInfo()
{
    if (m_digitalSignatureDialog) {
        m_digitalSignatureDialog->initializeDialog();
        m_digitalSignatureDialog->exec();
    }
}

void ApplicationController::enableSelectTool()
{
    KoToolManager::instance()->switchToolRequested("InteractionTool");
}

void ApplicationController::insertImage()
{
    KoShape *shape;
    KoShapeCreateCommand *cmd;
    KoSelection *selection;
    if (documentType() == TextDocument) {
        KWDocument* kwDoc = qobject_cast<KWDocument*>(document());
        shape = FoImageSelectionWidget::selectImageShape(kwDoc->resourceManager(), m_mainWindow);
        if (shape) {
            KWView* kwView = qobject_cast<KWView*>(document()->createView(m_mainWindow));
            if (currentPage()) {
                KWPage kwpage = wordsView()->currentPage();
                QRectF page = kwpage.rect();
                // make the shape be on the current page, and fit inside the current page.
                if (page.width() < shape->size().width() || page.height() < shape->size().height()) {
                    QSizeF newSize(page.width() * 0.7, page.height() * 0.7);
                    const qreal xRatio = newSize.width() / shape->size().width();
                    const qreal yRatio = newSize.height() / shape->size().height();
                    if (xRatio > yRatio) // then lets make the vertical set the size.
                        newSize.setWidth(shape->size().width() * yRatio);
                    else
                        newSize.setHeight(shape->size().height() * xRatio);
                    shape->setSize(newSize);
                }
                shape->setPosition(page.topLeft());
                int zIndex = 0;
                foreach (KoShape *s, kwView->canvasBase()->shapeManager()->shapesAt(page))
                    zIndex = qMax(s->zIndex(), zIndex);
                shape->setZIndex(zIndex+1);
            }
            cmd = new KoShapeCreateCommand(kwDoc, shape);
            selection = kwView->canvasBase()->shapeManager()->selection();
            selection->deselectAll();
            selection->select(shape);
            document()->addCommand(cmd);
        }
    }
    if (documentType() == PresentationDocument) {
        KoPADocument *prDoc = qobject_cast<KoPADocument *>(document());
        int pageIndex = canvasController()->canvas()->resourceManager()->resource(KoCanvasResourceManager::CurrentPage).toInt() - 1;
        KoPAPageBase *kprpage = prDoc->pageByIndex( pageIndex, false);
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(kprpage->shapes().first());
        shape = FoImageSelectionWidget::selectImageShape(prDoc->resourceManager(), m_mainWindow);
        if(!shape)
            return;
        shape->setParent(&(*layer));
        int zIndex = 0;
        QRectF page = kprpage->contentRect();
        if (page.width() < shape->size().width() || page.height() < shape->size().height()) {
            QSizeF newSize(page.width() * 0.7, page.height() * 0.7);
            const qreal xRatio = newSize.width() / shape->size().width();
            const qreal yRatio = newSize.height() / shape->size().height();
            if (xRatio > yRatio) // then lets make the vertical set the size.
                newSize.setWidth(shape->size().width() * yRatio);
            else
                newSize.setHeight(shape->size().height() * xRatio);
            shape->setSize(newSize);
        }
        foreach (KoShape *s, canvasController()->canvas()->shapeManager()->shapesAt(page))
            zIndex = qMax(s->zIndex(), zIndex);
        shape->setZIndex(zIndex);
        //        shape->setPosition(page.center());
        cmd = new KoShapeCreateCommand(prDoc, shape);
        prDoc->addCommand(cmd);
    }
}

void ApplicationController::insertButtonClicked()
{
    m_ui->actionInsert->setVisible(false);
    m_ui->actionInsertImage->setVisible(true);
}

void ApplicationController::showCCP()
{
    if(!m_ui->actionShowCCP->isChecked()) {
        m_ui->actionShowCCP->setChecked(false);
        m_ui->actionCopy->setVisible(false);
        m_ui->actionCut->setVisible(false);
        m_ui->actionPaste->setVisible(false);
        return;
    }
    m_ui->actionShowCCP->setChecked(true);
    m_ui->actionCopy->setVisible(true);
    m_ui->actionCut->setVisible(true);
    m_ui->actionPaste->setVisible(true);
}

void ApplicationController::insertNewTextShape()
{
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value( "TextShapeID" );
    Q_ASSERT( factory );
    KoShape *textShape = factory->createDefaultShape();
    KoCanvasBase *canvasForInserting = canvasController()->canvas();
    Q_CHECK_PTR(canvasForInserting);
    int curPage = canvasForInserting->resourceManager()->resource(KoCanvasResourceManager::CurrentPage).toInt() - 1;
    KoPADocument* paDocForInserting = qobject_cast<KoPADocument*>(document());
    KoPAPageBase* paPageForInserting = paDocForInserting->pageByIndex(curPage, false);
    KoShapeContainer *container =paPageForInserting;
    textShape->setParent(container);
   // textShape->setPosition (QPointF(m_xcoordinate,m_ycoordinate));
    paDocForInserting->addShape(textShape);
    textShape->setVisible(true);
}

void ApplicationController::insertNewTable()
{
    KoCanvasBase *canvasForChecking = canvasController()->canvas();
    Q_CHECK_PTR(canvasForChecking);
    qDebug()<<"selection:"<<canvasForChecking->toolProxy()->hasSelection();
    if(canvasForChecking->toolProxy()->hasSelection())
    {
        KoPAView *kopaview = qobject_cast<KoPAView *>(view());
        kopaview->kopaCanvas()->toolProxy()->actions()["insert_table"]->trigger();
    }
}

// KoAbstractApplication implementation

void ApplicationController::setWindowTitle(const QString& title)
{
    m_mainWindow->setWindowTitle(title);
}

void ApplicationController::showMessage(MessageType type,
                                        const QString& messageText)
{
    switch (type) {
    case UnsupportedFileTypeMessage: {
        NotifyDialog dialog(messageText, m_mainWindow);
        dialog.exec();
        break;
    }
    case InformationMessage:
    case InformationWithoutTimeoutMessage: {
#ifdef Q_WS_MAEMO_5
        const int timeout = (type == InformationMessage) ? QMaemo5InformationBox::DefaultTimeout
            : QMaemo5InformationBox::NoTimeout;
        QMaemo5InformationBox::information(0, messageText, timeout);
#else
        QMessageBox msgBox(m_mainWindow);
        msgBox.setText(messageText);
        msgBox.exec();
#endif
        break;
    }
    default:
        break;
    }
}

bool ApplicationController::startNewInstance(const KoAbstractApplicationOpenDocumentArguments& args)
{
    QStringList cmdLineArgs;
    const QString fileName = args.documentsToOpen.isEmpty()
                             ? QString() : args.documentsToOpen.first();
    cmdLineArgs << fileName;
    cmdLineArgs << (args.openAsTemplates ? "true" : "false");
    return QProcess::startDetached(qApp->applicationFilePath(), cmdLineArgs);
}

void ApplicationController::setProgressIndicatorVisible(bool visible)
{
#ifdef Q_WS_X11
    unsigned long val = visible ? 1 : 0;
    Atom atom = XInternAtom(QX11Info::display(),
                            "_HILDON_WM_WINDOW_PROGRESS_INDICATOR", False);
    XChangeProperty(QX11Info::display(), m_mainWindow->winId(), atom, XA_INTEGER,
                    32, PropModeReplace,
                    (unsigned char *) &val, 1);
#endif
}

void ApplicationController::showUiBeforeDocumentOpening(bool isNewDocument)
{
    m_ui->viewToolBar->show();
    m_ui->actionSlidingMotion->setVisible(false);
    m_ui->actionMathOp->setVisible(false);
    m_ui->actionFormat->setVisible(false);
    m_ui->actionSlidingMotion->setVisible(false);
    m_ui->actionSearch->setChecked(false);

    switch (documentType()) {
    case PresentationDocument:
        m_ui->actionInsertTextShape->setVisible(true);
        m_ui->actionInsertTable->setVisible(true);
        break;
    case SpreadsheetDocument:
        m_ui->actionEdit->setVisible(true);
        break;
    default:;
    }

   if (documentType() == TextDocument && !isNewDocument) {
       m_digitalSignatureDialog = new DigitalSignatureDialog(documentFileName());
        if (m_digitalSignatureDialog->verifySignature()){
            QMenuBar* menu = m_mainWindow->menuBar();
            menu->insertAction(m_ui->actionClose,m_ui->actionDigitalSignature);
            connect(m_ui->actionDigitalSignature, SIGNAL(triggered()),
                    this, SLOT(showDigitalSignatureInfo()));
        }
   }

    raiseWindow();
}

QString ApplicationController::applicationName() const
{
    return i18n("Calligra Mobile");
}

QMessageBox::StandardButton ApplicationController::askQuestion(QuestionType type,
                                                               const QString& messageText)
{
    switch (type) {
    case SaveDiscardCancelQuestion: {
        ConfirmationDialog dlg(m_mainWindow);
        return static_cast<QMessageBox::StandardButton>(dlg.exec());
    }
    case ConfirmSheetDeleteQuestion:
        return QMessageBox::question(m_mainWindow, i18n("Confirm Delete"),
                                     messageText,
                                     QMessageBox::Yes | QMessageBox::No);
    default:
        kFatal() << "Unsupported question type";
    }
    return QMessageBox::Cancel;
}

void ApplicationController::handleCurrentPageChanged(int previousPage)
{
    Q_UNUSED(previousPage);
    if (documentType() == PresentationDocument && m_mainWindow->isFullScreen()) {
        if (currentPage() == 1)
            m_fsPPTBackButton->hide();
        else if (currentPage() > 1)
            m_fsPPTBackButton->show();
        if (currentPage() == document()->pageCount())
            m_fsPPTForwardButton->hide();
        else if (currentPage() < document()->pageCount())
            m_fsPPTForwardButton->show();
    }
    const QString pageNo = i18n("pg%1 - pg%2", QString::number(currentPage()), QString::number(pageCount()));
    m_ui->actionPageNumber->setText(pageNo);
}

KoExternalEditorInterface* ApplicationController::createExternalCellEditor(KoCellTool* cellTool) const
{
    return new FoExternalEditor(cellTool);
}
