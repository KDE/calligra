/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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

#include "OfficeInterface.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Common.h"
#include "ZoomDialog.h"
#include "HildonMenu.h"
#include "NotifyDialog.h"
#include "AboutDialog.h"
#include "PresentationTool.h"
#include "MainWindowAdaptor.h"
#include "FoCellToolFactory.h"

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
#include <QDesktopServices>
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
#endif

#include <kfileitem.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kparts/event.h>

#include <KoView.h>
#include <KoCanvasBase.h>
#include <KoDocumentInfo.h>
#include <kdemacros.h>
#include <KoCanvasControllerWidget.h>
#include <KoZoomMode.h>
#include <KoZoomController.h>
#include <KoToolProxy.h>
#include <KoToolBase.h>
#include <KoResourceManager.h>
#include <KoToolManager.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoShapeUserData.h>
#include <KoTextShapeData.h>
#include <KoSelection.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoTextEditor.h>
#include <KoPAView.h>
#include <KoStore.h>
#include <KoCanvasBase.h>
#include <KoToolRegistry.h>
#include <styles/KoParagraphStyle.h>
#include <styles/KoListLevelProperties.h>
#include <KoList.h>
#include <kundostack.h>
#include <Map.h>
#include <Doc.h>
#include <View.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

using  KSpread::Doc;
using  KSpread::Map;
using  KSpread::View;
using  KSpread::Sheet;

#define FORMATFRAME_XCORDINATE_VALUE 465
#define FORMATFRAME_YCORDINATE_VALUE 150
#define FORMATFRAME_WIDTH 335
#define FORMATFRAME_HEIGHT 210

#define FONTSTYLEFRAME_XCORDINATE_VALUE 280
#define FONTSTYLEFRAME_YCORDINATE_VALUE 150
#define FONTSTYLEFRAME_WIDTH 520
#define FONTSTYLEFRAME_HEIGHT 210

bool MainWindow::enable_accelerator=false;
MainWindow::MainWindow(Splash *aSplash, QWidget *parent)
        : QMainWindow(parent),
        m_ui(new Ui::MainWindow),
        m_search(NULL),
        m_doc(NULL),
        m_view(NULL),
        m_editor(NULL),
        m_kwview(NULL),
        m_controller(NULL),
        m_undostack(NULL),
        m_focelltool(NULL),
        m_vPage(0),
        m_hPage(0),
        m_pressed(false),
        m_isViewToolBar(true),
        m_fsTimer(NULL),
        m_fsButton(NULL),
        m_fsIcon(FS_BUTTON_PATH),
        m_fsPPTBackButton(NULL),
        m_fsPPTForwardButton(NULL),
        m_currentPage(1),
        m_index(0),
        m_wholeWord(false),
        m_type(Text),
        m_splash(aSplash),
        m_panningCount(0),
        m_isLoading(false),
        m_fontstyleframe(0),
        m_formatframe(0),
        m_confirmationdialog(0),
        m_docdialog(0),
        m_formatframelayout(0),
        m_bold(0),
        m_italic(0),
        m_underline(0),
        m_alignleft(0),
        m_alignright(0),
        m_aligncenter(0),
        m_numberedlist(0),
        m_bulletlist(0),
        m_alignjustify(0),
        m_fontstyleframelayout(0),
        m_fontcombobox(0),
        m_textcolor(0),
        m_superscript(0),
        m_subscript(0),
        m_textbackgroundcolor(0),
        m_docdialoglayout(0),
        m_fontsizebutton(0),
        m_fontSizeDialog(0),
        m_fontSizeDialogLayout(0),
        m_fontSizeLineEdit(0),
        m_fontSizeList(0),
        m_document(0),
        m_presenter(0),
        m_spreadsheet(0),
        m_confirmationdialoglayout(0),
        m_yes(0),
        m_no(0),
        m_cancel(0),
        m_firstChar(true),
        m_message(0),
        m_newDocOpen(false),
        m_isDocModified(false),
        m_xcordinate(0),
        m_ycordinate(0),
        m_pptTool(0),
        m_fsPPTDrawHighlightButton(0),
        m_fsPPTDrawPenButton(0),
        m_fsAccButton(0),
        m_dbus( new MainWindowAdaptor(this)),
        m_collab(0),
        m_collabDialog(0),
        m_collabEditor(0),
        storeButtonPreview(0),
        notesDialog(0),
        m_slidingmotiondialog(0),
        m_slideNotesButton(0),
        m_slideNotesIcon(VIEW_NOTES_PIXMAP),
        m_tempnumber(0),
        m_tempselectiondialog(0),
        m_tempdialoglayout(0),
        m_templateWidget(0),
        m_go(0),
        m_closetemp(0),
        m_templatepreview(0)
{
    init();
}

void MainWindow::init()
{
    m_ui->setupUi(this);

    QDBusConnection::sessionBus().registerObject("/presentation/view", this);

    QMenuBar* menu = menuBar();
    menu->addAction(m_ui->actionOpen);
    menu->addAction(m_ui->actionNew);
    menu->addAction(m_ui->actionSave);
    menu->addAction(m_ui->actionSaveAs);
    menu->addAction(m_ui->actionClose);
    menu->addAction(m_ui->actionAbout);
    menu->addAction(m_ui->actionCollaborate);

    // false here means that they are not plugins
    m_ui->actionOpen->setData(QVariant(false));
    m_ui->actionAbout->setData(QVariant(false));
    m_ui->actionNew->setData(QVariant(false));
    m_ui->actionSave->setData(QVariant(false));
    m_ui->actionSaveAs->setData(QVariant(false));
    m_ui->actionClose->setData(QVariant(false));

    const QDir pluginDir("/usr/lib/freoffice/");
    const QStringList plugins = pluginDir.entryList(QDir::Files);

    for (int i = 0; i < plugins.size(); ++i) {
        QPluginLoader test(pluginDir.absoluteFilePath(plugins.at(i)));
        QObject *plug = test.instance();
        plug->setParent(this);
        if (plug != 0)
        {
            OfficeInterface* inter = qobject_cast<OfficeInterface*>(plug);
            const QString plugName = inter->pluginName();
            loadedPlugins[plugName] = inter;
            connect(plug, SIGNAL(openDocument(bool, const QString&)), this, SLOT(pluginOpen(bool, const QString&)));
            QAction *action = new QAction(plugName, this);

            // True states that this action is a plugin
            action->setData(QVariant(true));
            menu->addAction(action);
        }
    }

    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuClicked(QAction*)));
    m_search = new QLineEdit(this);
    m_search->setInputMethodHints(Qt::ImhNoAutoUppercase);
    m_ui->SearchToolBar->insertWidget(m_ui->actionSearchOption, m_search);
    m_exactMatchCheckBox = new QCheckBox(i18n("Exact Match"), this);
    m_ui->SearchToolBar->insertWidget(m_ui->actionSearchOption, m_exactMatchCheckBox);
    m_ui->SearchToolBar->hide();

    connect(m_ui->actionNew,SIGNAL(triggered()),this,SLOT(chooseDocumentType()));
    connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(m_ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(m_ui->actionClose,SIGNAL(triggered()),this,SLOT(closeDoc()));
    connect(m_ui->actionFormat,SIGNAL(triggered()),SLOT(openFormatFrame()));
    connect(m_ui->actionStyle,SIGNAL(triggered()),SLOT(openFontStyleFrame()));
    connect(m_ui->actionMathOp,SIGNAL(triggered()),SLOT(openMathOpFrame()));

    connect(m_search, SIGNAL(returnPressed()), SLOT(nextWord()));
    connect(m_search, SIGNAL(textEdited(QString)), SLOT(startSearch()));

    connect(m_ui->actionEdit, SIGNAL(toggled(bool)), this, SLOT(editToolBar(bool)));
    connect(m_ui->actionSearch, SIGNAL(toggled(bool)), this, SLOT(toggleToolBar(bool)));

    connect(m_ui->actionUndo,SIGNAL(triggered()),this,SLOT(doUndo()));
    connect(m_ui->actionRedo,SIGNAL(triggered()),this,SLOT(doRedo()));

    connect(m_ui->actionCopy,SIGNAL(triggered()),this,SLOT(copy()));
    connect(m_ui->actionCut,SIGNAL(triggered()),this,SLOT(cut()));
    connect(m_ui->actionPaste,SIGNAL(triggered()),this,SLOT(paste()));

    connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileDialog()));
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

    connect(m_ui->actionNextPage, SIGNAL(triggered()), this, SLOT(nextPage()));
    connect(m_ui->actionPrevPage, SIGNAL(triggered()), this, SLOT(prevPage()));

    connect(m_ui->actionPrevWord, SIGNAL(triggered()), this, SLOT(previousWord()));
    connect(m_ui->actionNextWord, SIGNAL(triggered()), this, SLOT(nextWord()));
    connect(m_exactMatchCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(searchOptionChanged(int)));

    m_fsTimer = new QTimer(this);
    Q_CHECK_PTR(m_fsTimer);
    connect(m_fsTimer, SIGNAL(timeout()), this, SLOT(fsTimer()));

    m_fsButton = new QPushButton(this);
    Q_CHECK_PTR(m_fsButton);
    m_fsButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
    m_fsButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
    m_fsButton->setIcon(m_fsIcon);
    m_fsButton->hide();
    connect(m_fsButton, SIGNAL(clicked()), SLOT(fsButtonClicked()));
    qApp->installEventFilter(this);

    updateActions();

    /* taking care of Zoom buttons : starts */
    unsigned long val = 1;
    Atom atom = XInternAtom(QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", False);
    XChangeProperty(QX11Info::display(), winId(), atom, XA_INTEGER, 32,
                    PropModeReplace,
                    (unsigned char *) &val, 1);
    /* taking care of Zoom buttons : ends */

    m_copyShortcut = new QShortcut(QKeySequence::Copy, this);
    connect(m_copyShortcut, SIGNAL(activated()), this, SLOT(copy()));

    // Toolbar should be shown only when we open a document
    m_ui->viewToolBar->hide();
    m_ui->EditToolBar->hide();
    connect(m_ui->actionPageNumber,SIGNAL(triggered()),this,SLOT(showPreviewDialog()));
    viewNumber=0;
}

MainWindow::~MainWindow()
{
    closeDocument();
    QMapIterator<QString, OfficeInterface*> i(loadedPlugins);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
    delete m_ui;
    m_ui = 0;
    delete m_collab;
    delete m_collabDialog;
    delete m_collabEditor;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    closeDoc();

    if(m_doc) {
        event->ignore();
    } else {
        QMainWindow::closeEvent(event);
    }
}

void MainWindow::showPreviewDialog()
{
    if(m_type == Presentation) {
        storeButtonPreview->showDialog(m_currentPage);
    }
}

void MainWindow::gotoPage(int page)
{
    if(page>=m_currentPage) {
        for(int i=m_currentPage;i<page;i++)
            nextPage();
    } else {
        for(int i=m_currentPage;i>page;i--) {
            prevPage();
        }
    }
}

void MainWindow::openFormatFrame()
{
    if(m_fontstyleframe)
        m_fontstyleframe->hide();

    if(m_formatframe && m_formatframe->isVisible()) {
        m_formatframe->hide();
        return;
    } else if(m_formatframe){
        if(m_type==Text)
            activeFormatOptionCheck();
        m_formatframe->show();
        return;
    }

    m_formatframe=new QFrame(this);
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

    m_formatframelayout->addWidget(m_alignleft,0,0);
    m_formatframelayout->addWidget(m_alignright,0,1);
    m_formatframelayout->addWidget(m_aligncenter,1,0);
    m_formatframelayout->addWidget(m_alignjustify,1,1);
    m_formatframelayout->addWidget(m_numberedlist,2,0);
    m_formatframelayout->addWidget(m_bulletlist,2,1);

    m_formatframe->setGeometry(FORMATFRAME_XCORDINATE_VALUE,
                               FORMATFRAME_YCORDINATE_VALUE,
                               FORMATFRAME_WIDTH,
                               FORMATFRAME_HEIGHT);
    m_formatframe->setLayout(m_formatframelayout);
    if(m_type==Text)
        activeFormatOptionCheck();
    m_formatframe->show();

    connect(m_alignjustify,SIGNAL(clicked()),this,SLOT(doJustify()));
    connect(m_alignleft,SIGNAL(clicked()),this,SLOT(doLeftAlignment()));
    connect(m_alignright,SIGNAL(clicked()),this,SLOT(doRightAlignment()));
    connect(m_aligncenter,SIGNAL(clicked()),this,SLOT(doCenterAlignment()));
    connect(m_numberedlist,SIGNAL(clicked()),this,SLOT(doNumberList()));
    connect(m_bulletlist,SIGNAL(clicked()),this,SLOT(doBulletList()));
}

void MainWindow::openFontStyleFrame()
{
    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe && m_fontstyleframe->isVisible()) {
        m_fontstyleframe->hide();
        return;
    } else if(m_fontstyleframe){
        if(m_type==Text)
            activeFontOptionCheck();
        m_fontstyleframe->show();
        return;
    }
    m_fontstyleframe=new QFrame(this);
    Q_CHECK_PTR(m_fontstyleframe);

    m_fontstyleframelayout=new QGridLayout;
    Q_CHECK_PTR(m_fontstyleframelayout);
    m_fontstyleframelayout->setVerticalSpacing(0);
    m_fontstyleframelayout->setHorizontalSpacing(0);

    m_fontsizebutton=new QPushButton(this);
    Q_CHECK_PTR(m_fontsizebutton);
    m_fontsizebutton->setMinimumSize(100,73);

    m_bold=addFontStyleFrameComponent(i18n("Bold"));
    m_italic=addFontStyleFrameComponent(i18n("Italic"));
    m_underline=addFontStyleFrameComponent(i18n("UnderLine"));
    m_textcolor=addFontStyleFrameComponent(i18n("TextColor"));
    m_textbackgroundcolor=addFontStyleFrameComponent(i18n("TextBackgroundColor"));
    m_subscript=addFontStyleFrameComponent(i18n("SubScript"));
    m_superscript=addFontStyleFrameComponent(i18n("SuperScript"));

    m_fontcombobox=new QFontComboBox(this);
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
    if(m_type==Text)
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

void  MainWindow::showFontSizeDialog()
{
    if (m_fontstyleframe) {
        m_fontstyleframe->hide();
    }
    m_fontSizeDialog = new QDialog(this);
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

void MainWindow::fontSizeRowSelected(QListWidgetItem *item)
{
     int row=(item->text()).toInt();
     selectFontSize(row+4);
     m_fontSizeDialog->accept();
}

void MainWindow::fontSizeEntered()
{
    selectFontSize(m_fontSizeLineEdit->text().toInt());
}

void MainWindow:: openMathOpFrame() {

}

///////////////////////
// Styling functions //
///////////////////////

void MainWindow::doSubScript()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(setSubScript(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatSubScript);

}

bool MainWindow::setSubScript(KoTextEditor *editor) {
    if (editor){
        if (editor->charFormat().verticalAlignment() == QTextCharFormat::AlignSubScript ){
            editor->setVerticalTextAlignment(Qt::AlignVCenter);
        } else {
            editor->setVerticalTextAlignment(Qt::AlignBottom);
        }
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::doSuperScript()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(setSuperScript(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatSuperScript);
}

bool MainWindow::setSuperScript(KoTextEditor *editor) {
    if (editor) {
        if (editor->charFormat().verticalAlignment() == QTextCharFormat::AlignSuperScript ) {
            editor->setVerticalTextAlignment(Qt::AlignVCenter);
        } else {
            editor->setVerticalTextAlignment(Qt::AlignTop);
        }
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::selectFontSize(int size)
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_type == Spreadsheet) {
        m_focelltool->selectFontSize(size);
    } else {
        if(setFontSize(size, m_editor) && m_collab)
            m_collab->sendFontSize(m_editor->selectionStart(), m_editor->selectionEnd(), size);
    }
    m_fontSizeDialog->hide();
}

bool MainWindow::setFontSize(int size, KoTextEditor *editor) {
    if (editor) {
        editor->setFontSize(size);
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::selectFontType()
{
    QString selectedfont=m_fontcombobox->currentText();
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_type == Spreadsheet) {
        m_focelltool->selectFontType(selectedfont);
    } else {
        if(setFontType(selectedfont, m_editor) && m_collab)
            m_collab->sendFontType(m_editor->selectionStart(), m_editor->selectionEnd(), selectedfont);
    }
}

bool MainWindow::setFontType(const QString &font, KoTextEditor *editor) {
    if (editor) {
        editor->setFontFamily(font);
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::selectTextColor()
{
    QColor color = QColorDialog::getColor(Qt::white,this);
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_type == Spreadsheet) {
        m_focelltool->selectTextColor(color);
    } else {
        if(setTextColor(color, m_editor) && m_collab)
            m_collab->sendTextColor(m_editor->selectionStart(), m_editor->selectionEnd(), color.rgb());
    }
}

bool MainWindow::setTextColor(const QColor &color, KoTextEditor *editor) {
    if (editor) {
        editor->setTextColor(color);
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::selectTextBackGroundColor()
{
    QColor color = QColorDialog::getColor(Qt::white,this);
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_type == Spreadsheet) {
        m_focelltool->selectTextBackgroundColor(color);
    } else {
        if(setTextBackgroundColor(color, m_editor) && m_collab)
            m_collab->sendTextColor(m_editor->selectionStart(), m_editor->selectionEnd(), color.rgb());
    }
}

bool MainWindow::setTextBackgroundColor(const QColor &color, KoTextEditor* editor) {
    if (editor) {
        editor->setTextBackgroundColor(color);
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::doBold()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_type == Spreadsheet) {
        m_controller->canvas()->toolProxy()->actions()["bold"]->trigger();
    } else {
        if(setBold(m_editor) && m_collab)
            m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatBold);
    }
}

bool MainWindow::setBold(KoTextEditor *editor) {
    if(editor) {
        QTextCharFormat textchar = editor->charFormat();
        if (textchar.fontWeight()==QFont::Bold) {
            editor->bold(false);
        } else {
            editor->bold(true);
        }
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::doItalic()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_type == Spreadsheet) {
        m_controller->canvas()->toolProxy()->actions()["italic"]->trigger();
    } else {
        if (setItalic(m_editor) && m_collab)
            m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatItalic);
    }
}

bool MainWindow::setItalic(KoTextEditor *editor) {
    if(editor) {
        QTextCharFormat textchar = editor->charFormat();
        if (textchar.fontItalic()) {
            editor->italic(false);
        } else {
            editor->italic(true);
        }
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::doUnderLine()
{
    if (m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_type == Spreadsheet) {
        m_controller->canvas()->toolProxy()->actions()["underline"]->trigger();
    } else {
        if(setUnderline(m_editor) && m_collab)
            m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatUnderline);
    }
}

bool MainWindow::setUnderline(KoTextEditor *editor) {
    if(editor) {
        QTextCharFormat textchar = editor->charFormat();
        if(textchar.property(KoCharacterStyle::UnderlineType).toBool()) {
            editor->underline(false);
        } else {
            editor->underline(true);
        }
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::doLeftAlignment()
{
    if (m_formatframe) {
        m_formatframe->hide();
    }
    if(setLeftAlign(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatAlignLeft);
}

bool MainWindow::setLeftAlign(KoTextEditor *editor) {
    if (editor) {
          editor->setHorizontalTextAlignment(Qt::AlignLeft);
          m_isDocModified = true;
          return true;
    }
    return false;
}

void MainWindow::doJustify()
{
    if (m_formatframe)
        m_formatframe->hide();
    if(setJustify(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatAlignJustify);
}

bool MainWindow::setJustify(KoTextEditor *editor) {
    if (editor) {
        editor->setHorizontalTextAlignment(Qt::AlignJustify);
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::doRightAlignment()
{
    if (m_formatframe)
        m_formatframe->hide();
    if(setRightAlign(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatAlignRight);
}

bool MainWindow::setRightAlign(KoTextEditor *editor) {
    if (editor) {
        editor->setHorizontalTextAlignment(Qt::AlignRight);
        m_isDocModified = true;
        return true;
    }
    return false;
}

void MainWindow::doCenterAlignment()
{
    if (m_formatframe)
        m_formatframe->hide();
    if(setCenterAlign(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatAlignCenter);
}

bool MainWindow::setCenterAlign(KoTextEditor *editor) {
    if (editor) {
        editor->setHorizontalTextAlignment(Qt::AlignCenter);
        m_isDocModified = true;
        return true;
    }
    return true;
}

void MainWindow::activeFormatOptionCheck() {
    QTextBlockFormat blk = m_editor->blockFormat();
    Qt::Alignment textblock_align = blk.alignment();
    switch(textblock_align) {
        case Qt::AlignLeft :
                        m_alignleft->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
                        m_alignjustify->setStyleSheet("");
                        m_alignright->setStyleSheet("");
                        m_aligncenter->setStyleSheet("");
                        break;
        case Qt::AlignRight :
                        m_alignright->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
                        m_alignjustify->setStyleSheet("");
                        m_alignleft->setStyleSheet("");
                        m_aligncenter->setStyleSheet("");
                        break;
        case Qt::AlignCenter :
                        m_aligncenter->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
                        m_alignjustify->setStyleSheet("");
                        m_alignright->setStyleSheet("");
                        m_alignleft->setStyleSheet("");
                        break;
        case Qt::AlignJustify :
                        m_alignjustify->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
                        m_alignleft->setStyleSheet("");
                        m_alignright->setStyleSheet("");
                        m_aligncenter->setStyleSheet("");
                        break;
    }
}

void MainWindow::activeFontOptionCheck() {
    if(m_superscript) {
        QTextCharFormat textchar = m_editor->charFormat();
        if(m_editor->charFormat().verticalAlignment() == QTextCharFormat::AlignSuperScript) {
            m_superscript->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
        } else {
            m_superscript->setStyleSheet("");
        }
    }

    if(m_subscript) {
        QTextCharFormat textchar = m_editor->charFormat();
        if(m_editor->charFormat().verticalAlignment() == QTextCharFormat::AlignSubScript) {
            m_subscript->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
        } else {
            m_subscript->setStyleSheet("");
        }
    }

    if(m_fontsizebutton) {
        QTextCharFormat textchar = m_editor->charFormat();
        QFont font=textchar.font();
        m_fontsizebutton->setText(QString().setNum(font.pointSize()));
    }

    if(m_fontcombobox) {
        QTextCharFormat textchar = m_editor->charFormat();
        QString fonttype = textchar.fontFamily();
        m_fontcombobox->setCurrentFont(QFont(fonttype));
    }

    if(m_bold) {
        QTextCharFormat textchar = m_editor->charFormat();
        if (textchar.fontWeight()==QFont::Bold) {
            m_bold->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
        } else {
            m_bold->setStyleSheet("");
        }
    }

    if(m_italic) {
        QTextCharFormat textchar = m_editor->charFormat();
        if (textchar.fontItalic()) {
            m_italic->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
        } else {
            m_italic->setStyleSheet("");
        }
    }

    if(m_underline) {
        QTextCharFormat textchar = m_editor->charFormat();
        if(textchar.property(KoCharacterStyle::UnderlineType).toBool()) {
            m_underline->setStyleSheet("QPushButton { background-color:lightskyblue; } ");
        } else {
            m_underline->setStyleSheet("");
        }
    }
}

void MainWindow::doNumberList()
{
    if(m_formatframe->isVisible())
        m_formatframe->hide();
    if(setNumberList(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatListNumber);
}

bool MainWindow::setNumberList(KoTextEditor *editor) {
    if (editor) {
        doStyle(KoListStyle::DecimalItem, editor);
        return true;
    }
    return false;
}

void MainWindow::doBulletList()
{
    if(m_formatframe->isVisible())
        m_formatframe->hide();
    if(setBulletList(m_editor) && m_collab)
        m_collab->sendFormat(m_editor->selectionStart(), m_editor->selectionEnd(), Collaborate::FormatListBullet);
}

bool MainWindow::setBulletList(KoTextEditor *editor) {
    if (editor) {
        doStyle(KoListStyle::DiscItem, editor);
        return true;
    }
    return false;
}

void MainWindow::doStyle(KoListStyle::Style style, KoTextEditor *editor)
{
    KoParagraphStyle *parag = new KoParagraphStyle();
    KoListStyle *list = new KoListStyle(parag);
    KoListLevelProperties llp = list->levelProperties(0);
    llp.setStyle(style);
    list->setLevelProperties(llp);
    parag->setListStyle(list);
    editor->setStyle(parag);
    m_isDocModified = true;
}

void MainWindow::saveFile()
{
    QMessageBox msgBox;
    if(m_doc) {
        if(m_fileName.isEmpty()) {
            saveFileAs();
        } else {
            QString ext = KMimeType::extractKnownExtension(m_fileName);
            if (!QString::compare(ext,EXT_ODT,Qt::CaseInsensitive) ||
                !QString::compare(ext, EXT_ODP, Qt::CaseInsensitive)||
                !QString::compare(ext, EXT_ODS, Qt::CaseInsensitive)) {

                if(m_doc->saveNativeFormat(m_fileName)) {
#ifdef Q_WS_MAEMO_5
                    QMaemo5InformationBox::information(0, i18n("The document has been saved successfully"),
                                                       QMaemo5InformationBox::DefaultTimeout);
#else
                    msgBox.setText(i18n("The document has been saved successfully"));
                    msgBox.exec();
#endif
                    m_isDocModified = false;

                    if(m_confirmationdialog) {
                        closeDocument();
                    }
                } else {
#ifdef Q_WS_MAEMO_5
                    QMaemo5InformationBox::information(0, i18n("The document could not be saved"),
                                                       QMaemo5InformationBox::DefaultTimeout);
#else
                    msgBox.setText(i18n("The document could not be saved"));
                    msgBox.exec();
#endif
                }
            } else if (!QString::compare(ext, EXT_DOC, Qt::CaseInsensitive)||
                       !QString::compare(ext, EXT_XLS, Qt::CaseInsensitive)||
                       !QString::compare(ext, EXT_PPT, Qt::CaseInsensitive))  {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("File will be saved in ODF"),
                                                   QMaemo5InformationBox::NoTimeout);
#else
                msgBox.setText(i18n("File will be saved in ODF"));
                msgBox.exec();
#endif
                QString fileName;
                fileName = getFileExtension(m_type);

                if (fileName.isEmpty()) {
#ifdef Q_WS_MAEMO_5
                    QMaemo5InformationBox::information(0, i18n("File name not specified"),
                                                       QMaemo5InformationBox::NoTimeout);
#else
                    msgBox.setText(i18n("File name not specified"));
                    msgBox.exec();
#endif
                    return;
                }
                KUrl newURL;
                newURL.setPath(fileName);
                KMimeType::Ptr mime = KMimeType::findByUrl(newURL);
                QString outputFormatString = mime->name();
                m_doc->setOutputMimeType(outputFormatString.toLatin1(), 0);

                if(m_doc->saveAs(newURL)) {
#ifdef Q_WS_MAEMO_5
                    QMaemo5InformationBox::information(0, i18n("The document has been saved successfully"),
                                                       QMaemo5InformationBox::DefaultTimeout);
#else
                    msgBox.setText(i18n("The document has been saved successfully"));
                    msgBox.exec();
#endif
                    m_isDocModified = false;

                    if(m_confirmationdialog) {
                        closeDocument();
                    }
                } else {
#ifdef Q_WS_MAEMO_5
                    QMaemo5InformationBox::information(0, i18n("The document could not be saved"),
                                                       QMaemo5InformationBox::DefaultTimeout);
#else
                    msgBox.setText(i18n("The document could not be saved"));
                    msgBox.exec();
#endif
                }
            } else {

#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("Saving operation supports only open document formats currently,Sorry"),
                                                   QMaemo5InformationBox::DefaultTimeout);
#else
                msgBox.setText(i18n("Saving operation supports only open document formats currently,Sorry"));
                msgBox.exec();
#endif
            }
        }
    } else {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, i18n("No document is open to perform save operation , invalid try"),
                                           QMaemo5InformationBox::DefaultTimeout);
#else
        msgBox.setText(i18n("No document is open to perform save operation , invalid try"));
        msgBox.exec();
#endif
    }
}

void MainWindow::saveFileAs()
{
    QMessageBox msgBox;
    if(m_doc) {
        QString ext;
        if(m_fileName.isEmpty()) {
            m_fileName = NEW_WORDDOC;
            ext = KMimeType::extractKnownExtension(m_fileName);
        } else {
            ext = KMimeType::extractKnownExtension(m_fileName);
        }
        if (!QString::compare(ext,EXT_ODT,Qt::CaseInsensitive)||
            !QString::compare(ext,EXT_ODP,Qt::CaseInsensitive)||
            !QString::compare(ext,EXT_ODS,Qt::CaseInsensitive)) {

            QString fileName;
            fileName = getFileExtension(m_type);
            m_fileName = fileName;

            if (fileName.isEmpty()) {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("File name not specified"),
                                                   QMaemo5InformationBox::NoTimeout);
#else
                msgBox.setText(i18n("File name not specified"));
                msgBox.exec();
#endif
                return;
            }
            if(m_doc->saveNativeFormat(m_fileName)) {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("The document has been saved successfully"),
                                                   QMaemo5InformationBox::DefaultTimeout);
#else
                msgBox.setText(i18n("The document has been saved successfully"));
                msgBox.exec();
#endif
                m_fileName = fileName;
                m_isDocModified = false;

                if(m_confirmationdialog) {
                    closeDocument();
                }
            } else {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("The document could not be saved"),
                                                   QMaemo5InformationBox::DefaultTimeout);
#else
                msgBox.setText(i18n("The document could not be saved"));
                msgBox.exec();
#endif
            }

        } else if (!QString::compare(ext,EXT_DOC,Qt::CaseInsensitive)||
                   !QString::compare(ext,EXT_XLS,Qt::CaseInsensitive)||
                   !QString::compare(ext,EXT_PPT,Qt::CaseInsensitive))  {
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, i18n("File will be saved in ODF"),
                                               QMaemo5InformationBox::NoTimeout);
#else
            msgBox.setText(i18n("File will be saved in ODF"));
            msgBox.exec();
#endif
            QString fileName;
            fileName = getFileExtension(m_type);

            if (fileName.isEmpty()) {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("File name not specified"),
                                                   QMaemo5InformationBox::NoTimeout);
#else
                msgBox.setText(i18n("File name not specified"));
                msgBox.exec();
#endif
                return;
            }
            KUrl newURL;
            newURL.setPath(fileName);
            KMimeType::Ptr mime = KMimeType::findByUrl(newURL);
            QString outputFormatString = mime->name();
            m_doc->setOutputMimeType(outputFormatString.toLatin1(), 0);

            if(m_doc->saveAs(newURL)) {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("The document has been saved successfully"),
                                                   QMaemo5InformationBox::DefaultTimeout);
#else
                msgBox.setText(i18n("The document has been saved successfully"));
                msgBox.exec();
#endif
                m_fileName = fileName;
                m_isDocModified = false;

                if(m_confirmationdialog) {
                    closeDocument();
                }
            } else {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("The document could not be saved"),
                                                   QMaemo5InformationBox::DefaultTimeout);
#else
                msgBox.setText(i18n("The document could not be saved"));
                msgBox.exec();
#endif
            }
        }else if (!QString::compare(ext, EXT_DOC, Qt::CaseInsensitive)||
                  !QString::compare(ext, EXT_XLS, Qt::CaseInsensitive)||
                  !QString::compare(ext, EXT_PPT, Qt::CaseInsensitive))  {
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, i18n("File will be saved in ODF"),
                                               QMaemo5InformationBox::NoTimeout);
#else
            msgBox.setText(i18n("File will be saved in ODF"));
            msgBox.exec();
#endif
            QString fileName;
            fileName = getFileExtension(m_type);
            m_fileName = fileName;

            if (fileName.isEmpty()) {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("File name not specified"),
                                                   QMaemo5InformationBox::NoTimeout);
#else
                msgBox.setText(i18n("File name not specified"));
                msgBox.exec();
#endif
                return;
            }
            KUrl newURL;
            newURL.setPath(m_fileName);
            KMimeType::Ptr mime = KMimeType::findByUrl(newURL);
            QString outputFormatString = mime->name();
            m_doc->setOutputMimeType(outputFormatString.toLatin1(), 0);

            if(m_doc->saveAs(newURL)) {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("The document has been saved successfully"),
                                                   QMaemo5InformationBox::DefaultTimeout);
#else
                msgBox.setText(i18n("The document has been saved successfully"));
                msgBox.exec();
#endif
                m_isDocModified = false;

                if(m_confirmationdialog) {
                    closeDocument();
                }
            } else {
#ifdef Q_WS_MAEMO_5
                QMaemo5InformationBox::information(0, i18n("The document could not be saved"),
                                                   QMaemo5InformationBox::DefaultTimeout);
#else
                msgBox.setText(i18n("The document could not be saved"));
                msgBox.exec();
#endif
            }
        }else {
#ifdef Q_WS_MAEMO_5
            QMaemo5InformationBox::information(0, i18n("Saving operation supports only open document formats currently,Sorry"),
                                               QMaemo5InformationBox::DefaultTimeout);
#else
            msgBox.setText(i18n("Saving operation supports only open document formats currently,Sorry"));
            msgBox.exec();
#endif
        }
    }  else {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(0, i18n("No document is open to perform save operation , invalid try"),
                                           QMaemo5InformationBox::DefaultTimeout);
#else
        msgBox.setText(i18n("No document is open to perform save operation , invalid try"));
        msgBox.exec();
#endif
    }
}

QString MainWindow::getFileExtension(DocumentType type){
    QString fileName;
    switch(type) {
        case Text:
            fileName = QFileDialog::getSaveFileName(this,i18n("Save File"),"/home/user/MyDocs/.odt");
            break;
        case Presentation:
            fileName = QFileDialog::getSaveFileName(this,i18n("Save File"),"/home/user/MyDocs/.odp");
            break;
        case Spreadsheet:
            fileName = QFileDialog::getSaveFileName(this,i18n("Save File"),"/home/user/MyDocs/.ods");
            break;
    }
   return fileName;
}



void MainWindow::chooseDocumentType()
{
    m_docdialog = new QDialog(this);
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

void MainWindow::openNewDoc()
{
    DocumentType type = Text;
    m_docdialog->close();
    openNewDocument(type);
}

void MainWindow::openNewPresenter()
{
    m_docdialog->close();
    templateSelectionDialog();
}

void MainWindow::openNewSpreadSheet()
{
   DocumentType type = Spreadsheet;
   m_docdialog->close();
   openNewDocument(type);
}

void MainWindow::templateSelectionDialog()
{
    if(m_tempselectiondialog){
        m_tempselectiondialog->show();
        return;
    }
    const QDir temppath(NEW_PRESENTER);
    m_temptitle <<temppath.entryList(QDir::Files);

    m_tempselectiondialog = new QDialog(this);
    Q_CHECK_PTR(m_tempselectiondialog);
    m_tempselectiondialog->setWindowTitle(i18n("Select Presentation Template                                   Preview"));

    m_tempdialoglayout = new QGridLayout;
    Q_CHECK_PTR(m_tempdialoglayout);
    m_tempdialoglayout->setVerticalSpacing(0);
    m_tempdialoglayout->setHorizontalSpacing(0);

    m_templateWidget = new QListWidget(this);
    Q_CHECK_PTR(m_templateWidget);
    QStringList tempNames;
    tempNames<<m_temptitle;
    tempNames.replaceInStrings("_"," ");
    tempNames.replaceInStrings(".odp","");
    m_templateWidget->addItems(tempNames);
    m_templateWidget->setMinimumSize(500,200);

    m_go = new QPushButton("Load",this);
    Q_CHECK_PTR(m_go);

    m_closetemp = new QPushButton("Cancel",this);
    Q_CHECK_PTR(m_closetemp);

    m_templatepreview = new QLabel(this);
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

void MainWindow::selectedTemplatePreview(int number) {
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
    m_tempnumber=number;
}

void MainWindow::openSelectedTemplate(){
    newpresenter = NEW_PRESENTER+m_temptitle.at(m_tempnumber);
    m_tempselectiondialog->accept();
    DocumentType type = Presentation;
    openNewDocument(type);
    if(storeButtonPreview!=0) {
        disconnect(storeButtonPreview,SIGNAL(gotoPage(int)),this,SLOT(gotoPage(int)));
        delete storeButtonPreview;
    }
    storeButtonPreview=new StoreButtonPreview(m_doc,m_view);
    connect(storeButtonPreview,SIGNAL(gotoPage(int)),this,SLOT(gotoPage(int)));
}

void MainWindow::closeTempSelectionDialog() {
    if(m_tempselectiondialog)
        m_tempselectiondialog->hide();
}

QPushButton * MainWindow::addFormatFrameComponent(const QString &imagepath)
{
    QPushButton * btn = new QPushButton(this);
    btn->setIcon(QIcon(":/images/48x48/Edittoolbaricons/"+imagepath+".png"));
    btn->setMaximumSize(165,75);
    Q_CHECK_PTR(btn);
    return btn;
}
QPushButton * MainWindow::addFontStyleFrameComponent(const QString &imagepath)
{
    QPushButton * btn = new QPushButton(this);
    btn->setIcon(QIcon(":/images/48x48/Edittoolbaricons/"+imagepath+".png"));
    btn->setMinimumSize(165,70);
    btn->setMaximumSize(165,70);
    Q_CHECK_PTR(btn);
    return btn;
}

QToolButton * MainWindow ::addNewDocument(const QString &docname)
{
    QToolButton * toolbutton = new QToolButton(this);
    Q_CHECK_PTR(toolbutton);
    toolbutton->setIcon(QIcon(":/images/48x48/"+docname+".png"));
    toolbutton->setToolTip(docname);
    return toolbutton;
}

void MainWindow::openNewDocumentType(QString type)
{
    if(type.compare("Text") == 0) {
        openNewDocument(Text);
    }
    if(type.compare("Spreadsheet") == 0) {
        openNewDocument(Spreadsheet);
    }
    if(type.compare("Presentation") == 0) {
        templateSelectionDialog();
    }
}

void MainWindow::openNewDocument(DocumentType type)
{
    KUrl newurl;
    QString mimetype;
    switch(type) {
    case Text:
        newurl.setPath(NEW_WORDDOC);
        mimetype = "application/vnd.oasis.opendocument.text";
        break;
    case Presentation:
        newurl.setPath(newpresenter);
        mimetype = "application/vnd.oasis.opendocument.presentation-template";
        break;
    case Spreadsheet:
        newurl.setPath(NEW_SPREADSHEET);
        mimetype = "application/vnd.oasis.opendocument.spreadsheet";
        break;
    }
    if (m_doc) {
        QStringList args;
        if(type == Text)
            args <<""<<"Text";
        if(type == Spreadsheet)
            args <<""<<"Spreadsheet";
        if(type == Presentation)
            args <<""<<"Presentation";

        QProcess::startDetached(FREOFFICE_APPLICATION_PATH,args);
        return;
    }
    raiseWindow();

    setShowProgressIndicator(true);
    closeDocument();
    int errorCode = 0;
    m_isLoading = true;
    m_doc = KParts::ComponentFactory::createPartInstanceFromQuery<KoDocument>(
                mimetype, QString(),
                0, 0, QStringList(),
                &errorCode);
    if (!m_doc) {
        setShowProgressIndicator(false);
        return;
    }

    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(true);
    if (!m_doc->openUrl(newurl)) {
        setShowProgressIndicator(false);
        return;
    }

    m_doc->setReadWrite(true);
    m_doc->setAutoSave(0);

    // registering tools
    KoToolRegistry::instance()->add(new FoCellToolFactory(KoToolRegistry::instance()));

    m_view = m_doc->createView();
    if(type == Text) {
        m_kwview = qobject_cast<KWView *>(m_view);
        m_editor = qobject_cast<KoTextEditor *>(m_kwview->canvasBase()->toolProxy()->selection());
        m_ui->actionFormat->setVisible(true);
    }

    QList<KoCanvasControllerWidget*> controllers = m_view->findChildren<KoCanvasControllerWidget*>();
    if (controllers.isEmpty()) {
        setShowProgressIndicator(false);
        return;// Panic
    }
    m_controller = controllers.first();
    if (!m_controller) {
        setShowProgressIndicator(false);
        return;
    }
    setWindowTitle(QString("%1 - %2").arg(i18n("FreOffice"),"NewDocument"));
    m_controller->setProperty("FingerScrollable", true);

    if (type == Spreadsheet) {
        KoToolManager::instance()->addController(m_controller);
        QApplication::sendEvent(m_view, new KParts::GUIActivateEvent(true));
        m_focelltool = new FoCellTool(((View*)m_view)->selection()->canvas());

        ((View *)m_view)->showTabBar(false);
        ((View *)m_view)->setStyleSheet("* { color:white; } ");
        ((View *)m_view)->setStyleSheet("* { color:white; } ");

        setCentralWidget(((View *)m_view));
    }

    if(type!=Spreadsheet)
        setCentralWidget(m_controller);

    QTimer::singleShot(250, this, SLOT(updateUI()));

    KoCanvasBase *new_canvas = m_controller->canvas();
    connect(new_canvas->resourceManager(),
            SIGNAL(resourceChanged(int, const QVariant &)),
            this,SLOT(resourceChanged(int, const QVariant &)));
    connect(KoToolManager::instance(),
            SIGNAL(changedTool(KoCanvasController*, int)),
            SLOT(activeToolChanged(KoCanvasController*, int)));

    if(type!=Spreadsheet) {
        KoToolManager::instance()->switchToolRequested(TextTool_ID);
    } else {
        KoToolManager::instance()->switchToolRequested(CellTool_ID);
    }
    setShowProgressIndicator(false);

    m_isLoading = false;
    if (m_splash && !this->isActiveWindow()) {
        this->show();
        m_splash->finish(m_controller);
        m_splash = 0;
    }
    m_ui->actionEdit->setVisible(true);
    if(type == Spreadsheet) {
        m_ui->actionMathOp->setVisible(true);
        m_ui->actionFormat->setVisible(false);
        m_ui->actionCollaborate->setVisible(false);
        m_ui->actionSlidingMotion->setVisible(false);
        m_ui->EditToolBar->show();
    }
    if(type == Text) {
        m_ui->actionFormat->setVisible(true);
        m_ui->EditToolBar->show();
    }

    if(type == Presentation){
        m_ui->viewToolBar->show();
    }

    if(type==Text||type==Spreadsheet)
        m_ui->actionEdit->setChecked(true);

    m_fileName = "";
    m_newDocOpen = true;
    m_type = type;
    m_undostack = m_doc->undoStack();
    m_firstChar=true;
}
void MainWindow::closeDoc()
{
    if(m_doc == NULL)
        return;

    if(!m_fileName.isEmpty()) {
        QString ext = KMimeType::extractKnownExtension(m_fileName);
        if (!QString::compare(ext,EXT_ODT,Qt::CaseInsensitive) ||
                !QString::compare(ext, EXT_ODP, Qt::CaseInsensitive)) {
        } else {
            closeDocument();
            return;
        }
    }

    m_doc->setModified(m_isDocModified);
    if (m_doc->isModified()) {
        m_confirmationdialog = new QDialog(this);
        Q_CHECK_PTR(m_confirmationdialog);
        m_confirmationdialoglayout = new QGridLayout;
        Q_CHECK_PTR(m_confirmationdialoglayout);
        m_yes = new QPushButton(i18n("Save"),this);
        Q_CHECK_PTR(m_yes);
        m_no = new QPushButton(i18n("Discard"),this);
        Q_CHECK_PTR(m_no);
        m_cancel = new QPushButton(i18n("Cancel"),this);
        Q_CHECK_PTR(m_cancel);
        m_message = new QLabel(i18n("Document is modified do you want to save before closing ?"),this);
        Q_CHECK_PTR(m_message);

        m_confirmationdialoglayout->addWidget(m_message,0,0,1,3,Qt::AlignCenter);
        m_confirmationdialoglayout->addWidget(m_yes,1,0);
        m_confirmationdialoglayout->addWidget(m_no,1,1);
        m_confirmationdialoglayout->addWidget(m_cancel,1,2);
        m_confirmationdialog->setLayout(m_confirmationdialoglayout);

        connect(m_no,SIGNAL(clicked()),this,SLOT(discardNewDocument()));
        connect(m_yes,SIGNAL(clicked()),this,SLOT(saveFile()));
        connect(m_cancel,SIGNAL(clicked()),this,SLOT(returnToDoc()));

        m_confirmationdialog->exec();
    } else {
        closeDocument();
    }
}

void MainWindow::openAboutDialog(void)
{
    QList<HildonMenu *> all_dlg = this->findChildren<HildonMenu *>();
    foreach(HildonMenu *menu, all_dlg)
    menu->close();
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::toggleToolBar(bool show)
{
    KoToolManager::instance()->switchToolRequested(PanTool_ID);
    if (show) {
        m_doc->setReadWrite(false);
        m_ui->viewToolBar->hide();
        m_ui->SearchToolBar->show();
        m_isViewToolBar = false;
        m_search->setFocus();
        m_search->selectAll();
    } else {
        m_doc->setReadWrite(true);
        m_search->clearFocus();
        m_ui->SearchToolBar->hide();
        m_ui->viewToolBar->show();
        m_isViewToolBar = true;
    }
}

//Toogling between edit toolbar and view toolbar with various options
void MainWindow::editToolBar(bool edit)
{
    if (m_newDocOpen) {
         if (!edit) {
             KoToolManager::instance()->switchToolRequested(PanTool_ID);
             m_ui->EditToolBar->hide();
             m_ui->viewToolBar->show();
             m_isViewToolBar = true;
         } else {
             KoToolManager::instance()->switchToolRequested(TextTool_ID);
             m_ui->viewToolBar->hide();
             m_ui->EditToolBar->show();
         }
    } else {
        if (edit) {
            if(m_type==Spreadsheet) {
                KoToolManager::instance()->switchToolRequested(CellTool_ID);
                m_ui->viewToolBar->hide();
                m_ui->EditToolBar->show();
            } else {
                KoToolManager::instance()->switchToolRequested(TextTool_ID);
                m_ui->viewToolBar->hide();
                m_ui->EditToolBar->show();
            }
        } else {
            KoToolManager::instance()->switchToolRequested(PanTool_ID);
            m_ui->EditToolBar->hide();
            m_ui->viewToolBar->show();
            m_isViewToolBar = true;
        }
    }

    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
}

void MainWindow::doUndo()
{
    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();

    if(m_doc && m_undostack->canUndo()) {
        m_undostack->undo();
    }
}

void MainWindow::doRedo()
{
    if(m_formatframe)
        m_formatframe->hide();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();

    if(m_doc && m_undostack->canRedo()) {
        m_undostack->redo();
    }
}

void MainWindow::copy()
{
    if((m_type == Text) && m_editor->hasSelection()) {
       m_controller->canvas()->toolProxy()->copy();
    }
    if(m_type == Spreadsheet)
          m_controller->canvas()->toolProxy()->copy();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_formatframe)
        m_formatframe->hide();
}

void MainWindow::cut()
{
    if((m_type == Text)&& m_editor->hasSelection())
        m_controller->canvas()->toolProxy()->cut();

    if(m_type == Spreadsheet)
        m_controller->canvas()->toolProxy()->cut();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_formatframe)
        m_formatframe->hide();
}

void MainWindow::paste()
{
    if ((m_type == Text) && QApplication::clipboard()->text().isEmpty()) {
        return ;
    }
    m_controller->canvas()->toolProxy()->paste();

    if(m_fontstyleframe)
        m_fontstyleframe->hide();
    if(m_formatframe)
        m_formatframe->hide();
}

void MainWindow::showFullScreenPresentationIcons(void)
{
    if (!m_controller)
        return;
    int vScrlbarWidth = 0;
    int hScrlbarHeight = 0;
    QSize size(this->frameSize());

    if (m_controller->verticalScrollBar()->isVisible()) {
        QSize vScrlbar = m_controller->verticalScrollBar()->size();
        vScrlbarWidth = vScrlbar.width();
    }

    if (m_controller->horizontalScrollBar()->isVisible()) {
        QSize hScrlbar = m_controller->horizontalScrollBar()->size();
        hScrlbarHeight = hScrlbar.height();
    }

    if ( ! m_fsPPTBackButton && ( m_pptTool ) && ( ! m_pptTool->toolsActivated() ) ) {
        m_fsPPTBackButton = new QPushButton(this);
        m_fsPPTBackButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
        m_fsPPTBackButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTBackButton->setIcon(QIcon(FS_PPT_BACK_BUTTON_PATH));
        connect(m_fsPPTBackButton, SIGNAL(clicked()), this, SLOT(prevPage()));
        m_fsPPTBackButton->move(size.width() - FS_BUTTON_SIZE*3 - vScrlbarWidth,
                                size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if (! m_fsPPTForwardButton && ( m_pptTool ) && ( ! m_pptTool->toolsActivated() ) ) {
        m_fsPPTForwardButton = new QPushButton(this);
        m_fsPPTForwardButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
        m_fsPPTForwardButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTForwardButton->setIcon(QIcon(FS_PPT_FORWARD_BUTTON_PATH));
        connect(m_fsPPTForwardButton, SIGNAL(clicked()), this, SLOT(nextPage()));
        m_fsPPTForwardButton->move(size.width() - FS_BUTTON_SIZE*2 - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if ( ( ! m_fsPPTDrawPenButton ) && ( m_pptTool ) ) {
        m_fsPPTDrawPenButton = new QPushButton(this);
        m_fsPPTDrawPenButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
        m_fsPPTDrawPenButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTDrawPenButton->setIcon(QIcon(":/images/64x64/PresentationDrawTool/pen.png"));
        m_fsPPTDrawPenButton->move(736 ,284);
        connect(m_fsPPTDrawPenButton,SIGNAL(clicked()),m_pptTool,SLOT(togglePenTool()));
    }

    m_fsPPTDrawPenButton->show();
    m_fsPPTDrawPenButton->raise();

    if ( ( ! m_fsPPTDrawHighlightButton ) && ( m_pptTool ) ) {
        m_fsPPTDrawHighlightButton = new QPushButton(this);
        m_fsPPTDrawHighlightButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
        m_fsPPTDrawHighlightButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTDrawHighlightButton->setIcon(QIcon(":/images/64x64/PresentationDrawTool/highlight.png"));
        m_fsPPTDrawHighlightButton->move(736,350);
        connect(this->m_fsPPTDrawHighlightButton,SIGNAL(clicked()),m_pptTool,SLOT(toggleHighlightTool()));
    }

    m_fsPPTDrawHighlightButton->show();
    m_fsPPTDrawHighlightButton->raise();

    if (m_currentPage < m_doc->pageCount() && ( m_pptTool ) && ( ! m_pptTool->toolsActivated() ) ) {
        m_fsPPTForwardButton->move(size.width() - FS_BUTTON_SIZE*2 - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
        m_fsPPTForwardButton->show();
        m_fsPPTForwardButton->raise();
    }

    if ( m_currentPage <= m_doc->pageCount() && m_currentPage != 1 && ( m_pptTool ) && (! m_pptTool->toolsActivated() ) ) {
        m_fsPPTBackButton->move(size.width() - FS_BUTTON_SIZE*3 - vScrlbarWidth,
                                size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
        m_fsPPTBackButton->show();
        m_fsPPTBackButton->raise();
    }
    if(m_type == Presentation)
    {
        if (!m_slideNotesButton) {
            m_slideNotesButton = new QPushButton(this);
            Q_CHECK_PTR(m_slideNotesButton);
            m_slideNotesButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
            m_slideNotesButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
            m_slideNotesButton->setIcon(m_slideNotesIcon);
            connect(m_slideNotesButton, SIGNAL(clicked()), SLOT(slideNotesButtonClicked()));
            m_slideNotesButton->move(736,222);
        }
        m_slideNotesButton->show();
        m_slideNotesButton->raise();

        if ((!m_fsAccButton)&& (m_pptTool)) {
             m_fsAccButton = new QPushButton(this);
             m_fsAccButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
             m_fsAccButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
             m_fsAccButton->setIcon(QIcon(":/images/64x64/Acceleration/swingoff.png"));
             m_fsAccButton->move(736,156);
             connect(m_fsAccButton, SIGNAL(clicked(bool)), SLOT(toggle_accelerator()));
        }
        m_fsAccButton->show();
        m_fsAccButton->raise();
    }
}

void MainWindow::slideTransitionDialog(){
    m_slidingmotiondialog = new SlidingMotionDialog;
    m_slidingmotiondialog->showDialog(this);
    connect(m_slidingmotiondialog->m_opengl, SIGNAL(clicked()), SLOT(toggle_accelerator()));
}

void MainWindow::toggle_accelerator()
{
    if(m_slidingmotiondialog){
        m_slidingmotiondialog->m_slidingmotionframe->hide();
    }
     enable_accelerator=!enable_accelerator;
     if(enable_accelerator==false)
     {qDebug()<<"closed the acc";
         //m_fsAccButton->setIcon(QIcon(":/images/64x64/Acceleration/swingoff.png"));
//         Accelerator_Thread.StopRecognition();
//         disconnect(&Accelerator_Thread,SIGNAL(next()),m_fsPPTForwardButton,SLOT(click()));
//         disconnect(&Accelerator_Thread,SIGNAL(previous()),m_fsPPTBackButton,SLOT(click()));
     }
     else
     {   qDebug()<<"toggled to start";
         //m_fsAccButton->setIcon(QIcon(":/images/64x64/Acceleration/swingon.png"));
//         Accelerator_Thread.StartRecognition();
//         connect(&Accelerator_Thread,SIGNAL(next()),m_fsPPTForwardButton,SLOT(click()));
//         connect(&Accelerator_Thread,SIGNAL(previous()),m_fsPPTBackButton,SLOT(click()));
     }
 }

void MainWindow::slideNotesButtonClicked()
{
    if(notesDialog) {
        disconnect(notesDialog,SIGNAL(moveSlide(bool)),this,SLOT(moveSLideFromNotesSLide(bool)));
        delete notesDialog;
    }
    notesDialog=new NotesDialog(m_doc,viewNumber,storeButtonPreview->thumbnailList);
    notesDialog->show();
    connect(notesDialog,SIGNAL(moveSlide(bool)),this,SLOT(moveSLideFromNotesSLide(bool)));

    notesDialog->show();
    notesDialog->showNotesDialog(m_currentPage);
}
void MainWindow::moveSLideFromNotesSLide(bool flag)
{
    if(flag==true) {
        nextPage();
    } else {
        prevPage();
    }
}
void MainWindow::openFileDialog()
{
    if (m_splash && !this->isActiveWindow()) {
        this->show();
        m_splash->finish(this);
        m_splash = 0;
    }

    QList<HildonMenu *> all_dlg = this->findChildren<HildonMenu *>();
    foreach(HildonMenu *menu, all_dlg)
    menu->close();

    QString file = QFileDialog::getOpenFileName(this, i18n("Open Document"),
                   QDesktopServices::storageLocation(
                       QDesktopServices::DocumentsLocation),
                   FILE_CHOOSER_FILTER);

    if (!file.isNull() && !checkFiletype(file)) {
        return;
    }
    if (!file.isNull() && !m_isLoading) {
        m_fileName = file;
        QTimer::singleShot(100, this, SLOT(doOpenDocument()));
    }
}

void MainWindow::closeDocument()
{
    if (m_doc == NULL)
        return;

    setWindowTitle(i18n("FreOffice"));
    setCentralWidget(0);
    m_positions.clear();
    // the presentation and text document instances seem to require different ways to do cleanup
    if (m_type == Presentation) {

        if(m_pptTool) {
        disconnect(m_fsButton, SIGNAL(clicked()), m_pptTool, SLOT(deactivateTool()));
        }
        KoToolManager::instance()->removeCanvasController(m_controller);
        delete m_doc;
        m_doc = 0;
        delete m_view;
        m_view = 0;
        if(m_controller) {
            delete m_controller;
        }
        m_controller = 0;
    } else if (m_type == Spreadsheet) {
        KoToolManager::instance()->removeCanvasController(m_controller);
        delete m_undostack;
        m_undostack = 0;
        delete m_view;
        m_view=0;
        delete m_doc;
        m_doc = 0;
    } else {
        KoToolManager::instance()->removeCanvasController(m_controller);
        if(m_kwview)
            m_kwview->canvasBase()->toolProxy()->deleteSelection();
        delete m_undostack;
        m_undostack = 0;
        delete m_doc;
        m_doc = 0;
        delete m_view;
        m_view = 0;
        m_kwview=0;
        m_editor=0;
        if(m_controller) {
            delete m_controller;
        }
        m_controller = 0;
    }

    setCentralWidget(0);
    m_currentPage = 1;

    if (m_ui->EditToolBar)
        m_ui->EditToolBar->hide();

    if(m_ui->viewToolBar)
        m_ui->viewToolBar->hide();

    if(m_ui->SearchToolBar)
        m_ui->SearchToolBar->hide();

    updateActions();

    m_ui->actionZoomLevel->setText(i18n("%1 %", 100));
    m_ui->actionPageNumber->setText(i18n("%1 of %2", 0, 0));

    fontStyleFrameDestructor();
    formatFrameDestructor();
    confirmationDialogDestructor();

    if(m_focelltool){
        delete m_focelltool;
        m_focelltool = 0;
    }
    if(m_slidingmotiondialog) {
        delete m_slidingmotiondialog;
        m_slidingmotiondialog = 0;
    }
    m_isDocModified=false;
    m_newDocOpen=false;
    viewNumber++;

    m_ui->actionMathOp->setVisible(false);
    m_ui->actionFormat->setVisible(false);
}

void MainWindow::returnToDoc()
{
    confirmationDialogDestructor();
    return;
}

void MainWindow::discardNewDocument()
{
    closeDocument();
}

void MainWindow::confirmationDialogDestructor()
{
    if(m_confirmationdialog) {
        m_confirmationdialog->accept();
        delete m_yes;
        m_yes = 0;
        delete m_no;
        m_no = 0;
        delete m_cancel;
        m_cancel = 0;
        delete m_confirmationdialog;
        m_confirmationdialog = 0;

    }
    return;
}

void MainWindow::formatFrameDestructor() {
    if(m_formatframe) {
        m_formatframe->close();
        delete m_bold;
        m_bold = 0;
        delete m_italic;
        m_italic = 0;
        delete m_underline;
        m_underline = 0;
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

void MainWindow::fontStyleFrameDestructor() {
    if(m_fontstyleframe) {
        m_fontstyleframe->close();
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

void MainWindow::raiseWindow(void)
{
    Display *display = QX11Info::display();
    XEvent e;
    Window root = RootWindow(display, DefaultScreen(display));
    memset(&e, 0, sizeof(e));
    e.xclient.type         = ClientMessage;
    e.xclient.window       = effectiveWinId();
    e.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    e.xclient.format       = 32;
    XSendEvent(display, root, False, SubstructureRedirectMask, &e);
    XFlush(display);
}

void MainWindow::doOpenDocument()
{
    openDocument(m_fileName);
    m_ui->actionEdit->setChecked(false);
    m_ui->actionSearch->setChecked(false);
    m_search->setText("");
}

void MainWindow::openDocument(const QString &fileName)
{
    if (!checkFiletype(fileName))
        return;

    if (m_doc) {
        QStringList args;
        args << fileName;
        QProcess::startDetached(FREOFFICE_APPLICATION_PATH, args);
        return;
    }
    m_ui->viewToolBar->show();
    m_ui->actionSlidingMotion->setVisible(false);
    raiseWindow();

    setShowProgressIndicator(true);
    QString mimetype = KMimeType::findByPath(fileName)->name();
    closeDocument();
    int errorCode = 0;
    m_isLoading = true;
    m_doc = KParts::ComponentFactory::createPartInstanceFromQuery<KoDocument>(
                mimetype, QString(),
                0, 0, QStringList(),
                &errorCode);
    if (!m_doc) {
        setShowProgressIndicator(false);
        return;
    }

    m_url;
    m_url.setPath(fileName);
    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(true);
    if (!m_doc->openUrl(m_url)) {
        setShowProgressIndicator(false);
        return;
    }
    m_doc->setReadWrite(true);
    m_doc->setAutoSave(0);

    // registering tools
    KoToolRegistry::instance()->add(new FoCellToolFactory(KoToolRegistry::instance()));

    m_view = m_doc->createView();
    QList<KoCanvasControllerWidget*> controllers = m_view->findChildren<KoCanvasControllerWidget*>();
    if (controllers.isEmpty()) {
        setShowProgressIndicator(false);
        return;// Panic
    }
    m_controller = controllers.first();
    if (!m_controller) {
        setShowProgressIndicator(false);
        return;
    }

    QString fname = m_url.fileName();
    QString ext = KMimeType::extractKnownExtension(fname);
    if (ext.length()) {
        fname.chop(ext.length() + 1);
    }

    if (!QString::compare(ext, EXT_ODP, Qt::CaseInsensitive) ||
            !QString::compare(ext, EXT_PPS, Qt::CaseInsensitive) ||
            !QString::compare(ext, EXT_PPSX, Qt::CaseInsensitive) ||
            !QString::compare(ext, EXT_PPTX, Qt::CaseInsensitive) ||
            !QString::compare(ext, EXT_PPT, Qt::CaseInsensitive)) {
        m_type = Presentation;
    } else if (!QString::compare(ext, EXT_ODS, Qt::CaseInsensitive) ||
               !QString::compare(ext, EXT_XLSX, Qt::CaseInsensitive) ||
               !QString::compare(ext, EXT_XLS, Qt::CaseInsensitive)) {
        m_type = Spreadsheet;
    } else {
        m_type = Text;
        // We need to get the page count again after layout rounds.
        connect(m_doc, SIGNAL(pageSetupChanged()), this, SLOT(updateUI()));
    }
    m_ui->actionMathOp->setVisible(false);
    m_ui->actionFormat->setVisible(false);
    if (m_type == Spreadsheet) {
        KoToolManager::instance()->addController(m_controller);
        QApplication::sendEvent(m_view, new KParts::GUIActivateEvent(true));
        m_focelltool = new FoCellTool(((View*)m_view)->selection()->canvas());
        ((View *)m_view)->showTabBar(false);
        ((View *)m_view)->setStyleSheet("* { color:white; } ");
        ((View *)m_view)->setStyleSheet("* { color:white; } ");
        setCentralWidget(((View *)m_view));
        m_ui->actionMathOp->setVisible(true);
    }

    if((m_type==Text) && ((!QString::compare(ext,EXT_ODT,Qt::CaseInsensitive)) ||
                          (!QString::compare(ext,EXT_DOC,Qt::CaseInsensitive)))) {
        m_ui->actionEdit->setVisible(true);
        m_kwview = qobject_cast<KWView *>(m_view);
        m_editor = qobject_cast<KoTextEditor *>(m_kwview->canvasBase()->toolProxy()->selection());
        m_ui->actionFormat->setVisible(true);
        m_ui->actionEdit->setVisible(true);
    } else {
        if(m_type == Presentation) {
            m_ui->actionEdit->setVisible(false);
            m_ui->actionSlidingMotion->setVisible(true);
        }
    }
    setWindowTitle(QString("%1 - %2").arg(i18n("FreOffice"), fname));

    m_controller->setProperty("FingerScrollable", true);

    if(m_type!=Spreadsheet) {
        setCentralWidget(m_controller);
    }
    QTimer::singleShot(250, this, SLOT(updateUI()));

    KoCanvasBase *canvas = m_controller->canvas();
    connect(canvas->resourceManager(),
            SIGNAL(resourceChanged(int, const QVariant &)),
            this,
            SLOT(resourceChanged(int, const QVariant &)));
    connect(KoToolManager::instance(),
            SIGNAL(changedTool(KoCanvasController*, int)),
            SLOT(activeToolChanged(KoCanvasController*, int)));

    KoToolManager::instance()->switchToolRequested(PanTool_ID);
    setShowProgressIndicator(false);
    m_isLoading = false;
    if (m_splash && !this->isActiveWindow()) {
        this->show();
        m_splash->finish(m_controller);
        m_splash = 0;
   }

   if(m_type==Presentation)
   {
       if(storeButtonPreview!=0)
       {
           disconnect(storeButtonPreview,SIGNAL(gotoPage(int)),this,SLOT(gotoPage(int)));
           delete storeButtonPreview;
       }
           storeButtonPreview=new StoreButtonPreview(m_doc,m_view);
           connect(storeButtonPreview,SIGNAL(gotoPage(int)),this,SLOT(gotoPage(int)));
   }
   m_undostack = m_doc->undoStack();
}

bool MainWindow::checkFiletype(const QString &fileName)
{
    bool ret = false;
    QList<QString> extensions;

    //Add Txt extension after adding ascii filter to koffice package
    /*extensions << EXT_DOC << EXT_DOCX << EXT_ODT << EXT_TXT \*/
    extensions << EXT_DOC << EXT_DOCX << EXT_ODT << EXT_TXT \
    << EXT_PPT << EXT_PPTX << EXT_ODP << EXT_PPS << EXT_PPSX \
    << EXT_ODS << EXT_XLS << EXT_XLSX;

    QString ext = KMimeType::extractKnownExtension(fileName);

    for (int i = 0; i < extensions.size(); i++) {
        if (extensions.at(i) == ext)
            ret = true;
    }
    if (!ret) {
        NotifyDialog dialog(this);
        dialog.exec();
    }
    return ret;
}

void MainWindow::updateUI()
{
    updateActions();
    if (!m_view || !m_ui)
        return;

    QString pageNo = i18n("pg%1 - pg%2", 0, 0);
    int factor = 100;

    if (m_doc->pageCount() > 0) {
        factor = m_view->zoomController()->zoomAction()->effectiveZoom() * 100;
        pageNo = i18n("pg%1 - pg%2", 1, QString::number(m_doc->pageCount()));
    }

    if(m_type==Spreadsheet) {
        if ((((Doc*)m_doc)->map())->count()>0)
            pageNo = i18n("pg%1 - pg%2", 1, QString::number((((Doc*)m_doc)->map())->count()));
    }

    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
    m_ui->actionPageNumber->setText(pageNo);

    m_vPage = m_controller->verticalScrollBar()->pageStep();
    m_hPage = m_controller->horizontalScrollBar()->pageStep();
}

void MainWindow::resourceChanged(int key, const QVariant &value)
{
    if( ( m_pptTool ) && m_pptTool->toolsActivated() && m_type == Presentation ) {
        return;
    }
    if (KoCanvasResource::CurrentPage == key) {
        m_currentPage = value.toInt();
        if (m_type == Presentation && isFullScreen()) {
            if (m_currentPage == 1)
                m_fsPPTBackButton->hide();
            else if (m_currentPage > 1)
                m_fsPPTBackButton->show();
            if (m_currentPage == m_doc->pageCount())
                m_fsPPTForwardButton->hide();
            else if (m_currentPage < m_doc->pageCount())
                m_fsPPTForwardButton->show();
        }
        QString pageNo;
        if(m_type == Spreadsheet){
            pageNo = i18n("pg%1 - pg%2", QString::number(value.toInt()), QString::number((((Doc*)m_doc)->map())->count()));
        }
        else
            pageNo = i18n("pg%1 - pg%2", QString::number(value.toInt()), QString::number(m_doc->pageCount()));
        m_ui->actionPageNumber->setText(pageNo);
    }
}

void MainWindow::fullScreen()
{
    if (!m_ui)
        return;
    int vScrlbarWidth = 0;
    int hScrlbarHeight = 0;
    m_ui->viewToolBar->hide();
    m_ui->SearchToolBar->hide();
    m_ui->EditToolBar->hide();
    if(m_type == Presentation) {
        emit presentationStarted();
    }
    showFullScreen();
    QSize size(this->frameSize());

    if (m_controller) {
        if (m_controller->verticalScrollBar()->isVisible()) {
            QSize vScrlbar = m_controller->verticalScrollBar()->size();
            vScrlbarWidth = vScrlbar.width();
        }
        if (m_controller->horizontalScrollBar()->isVisible()) {
            QSize hScrlbar = m_controller->horizontalScrollBar()->size();
            hScrlbarHeight = hScrlbar.height();
        }
    }

    m_fsButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                     size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    m_fsButton->show();
    m_fsButton->raise();
    m_fsTimer->start(3000);

    if (m_type == Presentation) {
        if((!m_pptTool))
        {
            m_pptTool=new PresentationTool(this, m_controller);
            connect(m_fsButton, SIGNAL(clicked()), m_pptTool, SLOT(deactivateTool()));
        }
        showFullScreenPresentationIcons();
    }
}

void MainWindow::zoomIn()
{
    if (!m_view || !m_ui)
        return;
    KoZoomAction *zAction =  m_view->zoomController()->zoomAction();
    int factor = zAction->effectiveZoom() * 100;
    if(factor<199)
    {
        zAction->zoomIn();
    }
    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
}

void MainWindow::zoomOut()
{
    if (!m_view || !m_ui)
        return;
    KoZoomAction *zAction = m_view->zoomController()->zoomAction();
    int factor = zAction->effectiveZoom() * 100;
    if(factor>70)
    {
        zAction->zoomOut();
    }
    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
}

void MainWindow::zoom()
{
    if(m_type!=Spreadsheet)
    {
        ZoomDialog dlg(this);
        connect(&dlg, SIGNAL(fitPage()), SLOT(zoomToPage()));
        connect(&dlg, SIGNAL(fitPageWidth()), SLOT(zoomToPageWidth()));
        dlg.exec();
    }
}

void MainWindow::zoomToPage()
{
    if (!m_view || !m_ui)
        return;
    m_view->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);
    if(m_type == Presentation)
        m_ui->actionZoomLevel->setText("48%");
    else
        m_ui->actionZoomLevel->setText("29%");
}

void MainWindow::zoomToPageWidth()
{
    if (!m_view || !m_ui)
        return;
    m_view->zoomController()->setZoomMode(KoZoomMode::ZOOM_WIDTH);
    if(m_type == Presentation)
        m_ui->actionZoomLevel->setText("84%");
    else
        m_ui->actionZoomLevel->setText("100%");
}

/*void MainWindow::prevPage()
{
    if (!m_controller)
        return;
    if ((m_doc->pageCount() > 0) && triggerAction("page_previous"))
        return;
    m_vPage = m_controller->verticalScrollBar()->pageStep();
    m_controller->pan(QPoint(0, -m_vPage));
}

void MainWindow::nextPage()
{
    if (!m_controller)
        return;
    if ((m_doc->pageCount() > 0) && triggerAction("page_next"))
        return;
    m_vPage = m_controller->verticalScrollBar()->pageStep();
    m_controller->pan(QPoint(0, m_vPage));
}*/
void MainWindow::nextSheet()
{
    Sheet *sheet = ((View*)m_view)->activeSheet();
    sheet = ((Doc *)m_doc)->map()->nextSheet(sheet);
    if(sheet)
      ((View*)m_view)->setActiveSheet(sheet);
}

void MainWindow::prevSheet()
{
    Sheet *sheet = ((View*)m_view)->activeSheet();
    sheet = ((Doc *)m_doc)->map()->previousSheet(sheet);
    if(sheet)
      ((View*)m_view)->setActiveSheet(sheet);
}

//this new functions  solve problem in action next page which stops  moving page from 13th page number
void MainWindow::prevPage()
{
    if (!m_controller)
        return;
    if(m_type == Spreadsheet) {
       prevSheet();
       return;
    }
    if(m_type == Presentation) {
        emit previousSlide();
    }

    if(m_currentPage == 1)
        return;

    int cur_page = m_currentPage;
    bool check = triggerAction("page_previous");
    if(check) {
        while(cur_page - m_currentPage != 1) {
            nextPage();
        }
    }
}

void MainWindow::nextPage()
{
   if (!m_controller)
       return ;
   if(m_type == Spreadsheet) {
      nextSheet();
      return;
   }
   if(m_type == Presentation) {
       emit nextSlide();
   }
   if(m_currentPage == m_doc->pageCount()) {
      return;
   }
   if(m_currentPage == m_doc->pageCount()) {
      return;
   }
   static int prev_curpage=0;
   if(prev_curpage != m_currentPage) {
        int cur_page = m_currentPage;
        prev_curpage = m_currentPage;
        triggerAction("page_next");
        if(cur_page == m_currentPage)
           nextPage();
        return;
   }
   else {
        int cur_page = m_currentPage;
        int next_page = cur_page+1;
        while(next_page != cur_page && m_type == Text) {
            m_controller->pan(QPoint(0, 1));
            cur_page = m_currentPage;
        }
        prev_curpage = m_currentPage;
        triggerAction("page_next");
        return;
    }
}

void MainWindow::fsTimer()
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
        if (m_fsAccButton && m_fsAccButton->isVisible())
            m_fsAccButton->hide();
    }
}

void MainWindow::fsButtonClicked()
{
    if (!m_ui)
        return;

    if(m_controller) {
        m_controller->show();
    }

    if(m_type == Presentation) {
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

    if (m_fsAccButton && m_fsAccButton->isVisible())
        m_fsAccButton->hide();

    if (m_isViewToolBar)
        m_ui->viewToolBar->show();
    else
        m_ui->SearchToolBar->show();

    showNormal();
}

static void findTextShapesRecursive(KoShapeContainer* con, KoPAPageBase* page,
                                    QList<QPair<KoPAPageBase*, KoShape*> >& shapes,
                                    QList<QTextDocument*>& docs)
{
    foreach(KoShape* shape, con->shapes()) {
        KoTextShapeData* tsd = qobject_cast<KoTextShapeData*> (shape->userData());
        if (tsd) {
            shapes.append(qMakePair(page, shape));
            docs.append(tsd->document());
        }
        KoShapeContainer* child = dynamic_cast<KoShapeContainer*>(shape);
        if (child) findTextShapesRecursive(child, page, shapes, docs);
    }
}

void MainWindow::startSearch()
{
    if (!m_search || !m_controller)
        return;

    QString searchString = m_search->text();
    if(m_search->text()==""){
        KoCanvasBase *canvas = m_controller->canvas();
        KoSelection *selection = canvas->shapeManager()->selection();
        selection->deselectAll();
        m_ui->actionSearchResult->setText(i18n("%1 of %2", 0, 0));
        return;
    }

    KoCanvasBase *canvas = m_controller->canvas();
    Q_CHECK_PTR(canvas);

    KoPADocument* padoc = qobject_cast<KoPADocument*>(m_doc);
    if (padoc) {
        // loop over all pages starting from current page to get
        // search results in the right order
        int curPage = canvas->resourceManager()->resource(\
                      KoCanvasResource::CurrentPage).toInt() - 1;
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
            for (int i = 0; i < m_positions.size(); i++) {
                if (m_positions[i].first.first == padoc->pageByIndex(page, false)) {
                    foundIt = true;
                    m_index = i;
                    highlightText(m_index);
                    break;
                }
            }
            if (foundIt) break;
        }
        if (!foundIt) {
            for (int page = 0; page < curPage; page++) {
                for (int i = 0; i < m_positions.size(); i++) {
                    if (m_positions[i].first.first ==
                            padoc->pageByIndex(page, false)) {
                        foundIt = true;
                        m_index = i;
                        highlightText(m_index);
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
            QList<KoTextShapeData*> shapeDatas;
            QList<QTextDocument*> textDocs;
            QList<QPair<KoPAPageBase*, KoShape*> > textShapes;
            QSet<QTextDocument*> textDocSet;

            for (int i = 0; i < size; i++) {
                shapeDatas.append(qobject_cast<KoTextShapeData*> \
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

void MainWindow::findText(QList<QTextDocument*> aDocs,
                          QList<QPair<KoPAPageBase*, KoShape*> > shapes,
                          const QString &aText)
{
    if (aDocs.isEmpty())
        return;

    m_positions.clear();

    for (int i = 0; i < aDocs.size(); i++) {
        QTextDocument* doc = aDocs.at(i);
        KoShape* shape = shapes.at(i).second;

        QTextCursor result(doc);
        do {
            if (!m_wholeWord)
                result = doc->find(aText, result);
            else
                result = doc->find(aText, result, QTextDocument::FindCaseSensitively);
            if (result.hasSelection()) {
                m_positions.append(qMakePair(qMakePair(shapes.at(i).first, shape),
                                             qMakePair(result.selectionStart(),
                                                       result.selectionEnd())));
            }
        } while (!result.isNull());
    }

    m_index = 0;
    if (!m_positions.isEmpty())
        highlightText(m_index);
    else
        m_ui->actionSearchResult->setText(i18n("%1 of %2", 0, 0));
}

void MainWindow::highlightText(int aIndex)
{
    if (!m_controller || !m_ui)
        return;

    KoCanvasBase *canvas = m_controller->canvas();
    Q_CHECK_PTR(canvas);

    // first potentially go to the correct page
    KoPAPageBase* page = m_positions.at(aIndex).first.first;
    if (page) {
        KoPAView* paview = static_cast<KoPAView*>(m_view);
        if (paview->activePage() != page) {
            paview->doUpdateActivePage(page);
        }
    }

    // secondly set the currently selected text shape to the one containing this search result
    KoSelection *selection = canvas->shapeManager()->selection();
    KoShape* shape = m_positions.at(aIndex).first.second;
    if (selection->count() != 1 || selection->firstSelectedShape() != shape) {
        selection->deselectAll();
        selection->select(shape);
    }
    // ugly hack, but if we don't first disable and than re-enable the text tool
    // it will still keep the wrong textshape selected
    KoToolManager::instance()->switchToolRequested(PanTool_ID);
    KoToolManager::instance()->switchToolRequested(TextTool_ID);

    KoResourceManager *provider = canvas->resourceManager();
    Q_CHECK_PTR(provider);

    QString sizeStr = QString::number(m_positions.size());
    QString indexStr = QString::number(aIndex + 1);

    m_ui->actionSearchResult->setText(i18n("%1 of %2",aIndex + 1,m_positions.size() ));

    provider->setResource(KoText::CurrentTextPosition,
                          m_positions.at(aIndex).second.first);
    provider->setResource(KoText::CurrentTextAnchor,
                          m_positions.at(aIndex).second.second);
}

void MainWindow::previousWord()
{
    if (m_positions.isEmpty())
        return;
    if (m_index == 0) {
        m_index = m_positions.size() - 1;
    } else {
        m_index--;
    }
    highlightText(m_index);
}

void MainWindow::nextWord()
{
    if (m_positions.isEmpty())
        return;
    if (m_index == m_positions.size() - 1) {
        m_index = 0;
    } else {
        m_index++;
    }
    highlightText(m_index);
}

void MainWindow::searchOptionChanged(int aCheckBoxState)
{
    if (aCheckBoxState == Qt::Unchecked)
        m_wholeWord = false;
    if (aCheckBoxState == Qt::Checked)
        m_wholeWord = true;
    startSearch();
}

bool MainWindow::triggerAction(const char* name)
{
    if (m_view) {
        // the cast in the next line is no longer needed for
        // koffice revision 1004085 and newer
        QAction* action = ((KXMLGUIClient*)m_view)->action(name);
        if (action) {
            action->activate(QAction::Trigger);
            return true;
        }
    }
    return false;
}

void MainWindow::updateActions()
{
    bool docLoaded = m_doc;
    m_ui->actionSearch->setEnabled(docLoaded);
    m_ui->actionEdit->setEnabled(docLoaded);
    m_ui->actionFullScreen->setEnabled(docLoaded);
    m_ui->actionZoomIn->setEnabled(docLoaded);
    m_ui->actionZoomOut->setEnabled(docLoaded);
    m_ui->actionZoomLevel->setEnabled(docLoaded);
    m_ui->actionNextPage->setEnabled(docLoaded);
    m_ui->actionPrevPage->setEnabled(docLoaded);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu)
        return true;

    // TODO: refine the collaborative-editing section of eventFilter
    if(m_editor && m_collab)
    {
        if(event->type() == 6 && QString::compare("KWCanvas", watched->metaObject()->className())== 0 )
        {
            //qDebug()<<"111111111111111"<<watched->metaObject()->className();
            QKeyEvent *ke=static_cast<QKeyEvent *>(event);
            if( ke->key() == Qt::Key_Backspace )
                m_collab->sendBackspace(m_editor->selectionStart(), m_editor->selectionEnd());
            else if (((ke->key()>=Qt::Key_Space) && (ke->key()<=Qt::Key_AsciiTilde)) || ke->key() == Qt::Key_Return )
                m_collab->sendString(m_editor->selectionStart(), m_editor->selectionEnd(), ke->text().toUtf8());
            else
                qDebug() << "Collaborate: Unsupported key: " << ke->key() << " (" << ke->text() << ")";
        }
    }

    // show buttons in full screen mode if user taps anywhere in the screen
    if (event && this->isFullScreen()) {

        if (event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::TabletPress) {
            m_pressed = true;
            m_panningCount = 0;
            m_slideChangePossible = true;
            m_fsTimer->start(3000);
            m_pressPos = (reinterpret_cast<QMouseEvent*>(event))->pos();
        } else if (event->type() == QEvent::MouseButtonRelease ||
                   event->type() == QEvent::TabletRelease) {
            m_pressed = false;
            //show buttons only if user just tap the screen without
            //panning document
            if (m_panningCount <= 5) {
                m_fsButton->show();
                m_fsButton->raise();
                if (m_type == Presentation)
                    showFullScreenPresentationIcons();
                m_fsTimer->start(3000);
                m_slideChangePossible = false;
            }
            m_panningCount = 0;
        } else if ((event->type() == QEvent::TabletMove ||
                    event->type() == QEvent::MouseMove) && m_pressed) {
            int sliderMin = m_controller->verticalScrollBar()->minimum();
            int sliderVal = m_controller->verticalScrollBar()->value();
            int sliderMax = m_controller->verticalScrollBar()->maximum();
            if (sliderVal == sliderMin || sliderVal == sliderMax)
                m_panningCount++;
        }

        if (m_type == Presentation && m_slideChangePossible
                && m_panningCount > 5 && (event->type() == QEvent::MouseMove
                                          || event->type() == QEvent::TabletMove)) {
            int sliderMin = m_controller->verticalScrollBar()->minimum();
            int sliderVal = m_controller->verticalScrollBar()->value();
            int sliderMax = m_controller->verticalScrollBar()->maximum();
            QPoint movePos = (reinterpret_cast<QMouseEvent*>(event))->pos();
            if (movePos.y() - m_pressPos.y() > 50 && sliderVal == sliderMin && m_pptTool && ( ! m_pptTool->toolsActivated() ) ) {
                m_slideChangePossible = false;
                triggerAction("page_previous");
                emit previousSlide();
            }
            if (m_pressPos.y() - movePos.y() > 50 && sliderVal == sliderMax && m_pptTool && ( ! m_pptTool->toolsActivated() ) ) {
                m_slideChangePossible = false;
                triggerAction("page_next");
                emit nextSlide();
            }
        }
    }
    // Maemo Qt hardcodes handling of F6 to toggling full screen directly, so
    // override that shortcut to do what we want it to do instead.
    if (event && event->type() == QEvent::Shortcut) {
        QShortcutEvent *qse = reinterpret_cast<QShortcutEvent*>(event);
        if (qse->key() == QKeySequence(Qt::Key_F6)) {
            if (m_ui->actionFullScreen->isEnabled())
                fullScreen();
            return true;
        } else if (qse->key() == QKeySequence(Qt::Key_F4)) {
            showApplicationMenu();
            return true;
        }
    } else if (event->type() == QEvent::ShortcutOverride && isFullScreen()) {
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
    if(m_doc) {
        if ( ( m_pptTool && ( ! m_pptTool->toolsActivated() ) ) && watched && event && m_type == Presentation  &&  m_doc->pageCount() > 0
                && this->isFullScreen()
                && event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = reinterpret_cast<QMouseEvent*>(event);
            // check that event wasn't from full screen push button
            if (QString::compare("QPushButton", watched->metaObject()->className())) {
                QSize size(this->frameSize());
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

    if(event && event->type()==QEvent::MouseButtonPress && m_doc) {
        QMouseEvent *mouseEvent= reinterpret_cast<QMouseEvent*>(event);
        m_xcordinate = mouseEvent->globalX ();
        m_ycordinate = mouseEvent->globalY();
        if((m_formatframe) && (m_formatframe->isVisible())) {
            if ((m_xcordinate<480) || (m_ycordinate<205))
                m_formatframe->hide();
        }
        if((m_fontstyleframe) && (m_fontstyleframe->isVisible())) {
            if(!this->isActiveWindow()){
            } else {
                if((m_xcordinate<325) || (m_ycordinate<205))
                    m_fontstyleframe->hide();
            }
        }
    }

    if(m_doc && m_editor && event && event->type() == 7 ) {
         QKeyEvent *ke=static_cast<QKeyEvent *>(event);
         if(ke->key()!=Qt::Key_Up && ke->key()!=Qt::Key_Down &&
            ke->key()!=Qt::Key_Right && ke->key()!=Qt::Key_Left &&
            ke->key()!=Qt::Key_Shift && ke->key()!=Qt::Key_Alt &&
            ke->key()!=Qt::Key_Control && ke->key()!=Qt::Key_CapsLock ){
             m_isDocModified = true;
             if(m_firstChar==true){
                m_ui->EditToolBar->setFocus();
                m_controller->setFocus();
                m_firstChar=false;
            }
         }
    }

    if(event->type() == QEvent::MouseButtonDblClick && m_type == Text) {
        if(m_ui->actionEdit->isChecked())
            m_editor->setPosition(0,QTextCursor::MoveAnchor);
        return true;
    }
    return false;
    //return QMainWindow::eventFilter(watched, event);
}

void MainWindow::showApplicationMenu()
{
    HildonMenu menu(this);
    menu.exec();
}

void MainWindow::activeToolChanged(KoCanvasControllerWidget* canvas, int)
{
   QString newTool= KoToolManager::instance()->activeToolId();
   // only Pan tool or Text tool should ever be the active tool, so if
   // another tool got activated, switch back to pan tool
    if (newTool != PanTool_ID && newTool != TextTool_ID && newTool != CellTool_ID) {
        KoToolManager::instance()->switchToolRequested(PanTool_ID);
    }
    canvas->setProperty("FingerScrollable", true);
}

void MainWindow::setShowProgressIndicator(bool visible)
{
    unsigned long val = visible ? 1 : 0;
    Atom atom = XInternAtom(QX11Info::display(),
                            "_HILDON_WM_WINDOW_PROGRESS_INDICATOR", False);
    XChangeProperty(QX11Info::display(), winId(), atom, XA_INTEGER,
                    32, PropModeReplace,
                    (unsigned char *) &val, 1);
}

//Function to check if application has been started by DBus
void MainWindow::checkDBusActivation()
{
    if (m_splash && !this->isActiveWindow())
        openFileDialog();
}

void MainWindow::pluginOpen(bool newWindow, const QString& path)
{
    openDocument(path);
}

void MainWindow::menuClicked(QAction* action)
{
    if (!action->data().toBool())
    {
        return; // We return if it was not a plugin action
    }

    const QString activeText = action->text();
    OfficeInterface *nextPlugin = loadedPlugins[activeText];
    nextPlugin->setDocument(m_doc);
    QWidget *pluginView = nextPlugin->view();
    pluginView->setParent(this);
    pluginView->setWindowFlags(Qt::Dialog);
    pluginView->show();
}

void MainWindow::loadScrollAndQuit()
{
    // if no document is loaded, simply quit
    if (m_doc == 0 || m_controller == 0) {
        QTimer::singleShot(1, qApp, SLOT(quit()));
        return;
    }
    // if still loading, wait some more
    if (m_doc->isLoading()) {
        QTimer::singleShot(100, this, SLOT(loadScrollAndQuit()));
        return;
    }
    // when done loading, start proceeding to the next page until the end of the document has been
    // reached
    bool done;
    if (m_type == Presentation) {
        done = m_currentPage == m_doc->pageCount();
    } else {
        QScrollBar *v = m_controller->verticalScrollBar();
        done = v->value() >= v->maximum();
    }
    if (done) {
        QTimer::singleShot(1, qApp, SLOT(quit()));
    } else {
        nextPage();
        QTimer::singleShot(20, this, SLOT(loadScrollAndQuit()));
    }
}

void MainWindow::mousePressEvent( QMouseEvent *event )
{
    if( m_pptTool && m_pptTool->toolsActivated() ) {
        m_pptTool->handleMainWindowMousePressEvent( event );
    }
}

void MainWindow::mouseMoveEvent( QMouseEvent *event )
{
    if( m_pptTool && m_pptTool->toolsActivated() ) {
        m_pptTool->handleMainWindowMouseMoveEvent( event );
    }
}

void MainWindow::mouseReleaseEvent( QMouseEvent *event )
{
    if( m_pptTool && m_pptTool->toolsActivated() ) {
        m_pptTool->handleMainWindowMouseReleaseEvent( event );
    }
}

void MainWindow::paintEvent( QPaintEvent *event )
{
    if( !m_pptTool ) {
        return;
    }

    if( m_pptTool->toolsActivated() ) {
        QPainter painter(this);
        QRectF target(0,0,800,480);
        QRectF source(0,0,800,480);
        painter.drawImage( target,*( m_pptTool->getImage() ), source );
    }

    if( ( !m_pptTool->getPenToolStatus() ) && ( !m_pptTool->getHighlightToolStatus() ) && m_controller) {
        m_controller->show();
    }
}

void MainWindow::disableFullScreenPresentationNavigation()
{
    disconnect( m_fsPPTBackButton, SIGNAL( clicked() ), this, SLOT( prevPage() ) );
    disconnect( m_fsPPTForwardButton, SIGNAL( clicked() ), this, SLOT( nextPage() ) );
    m_fsPPTBackButton->hide();
    m_fsPPTForwardButton->hide();
}

void MainWindow::enableFullScreenPresentationNavigation()
{
    connect( m_fsPPTBackButton, SIGNAL( clicked() ), this, SLOT( prevPage() ) );
    connect( m_fsPPTForwardButton, SIGNAL( clicked() ), this, SLOT( nextPage() ) );
    m_fsPPTBackButton->show();
    m_fsPPTBackButton->raise();
    m_fsPPTForwardButton->show();
    m_fsPPTForwardButton->raise();
}

///////////////////////////
// Collaborative editing //
///////////////////////////

void MainWindow::collaborateDialog()
{
    if(m_collab)
    {
        QMessageBox::warning(this,"Collaborative Editing",QString("An active session already exists"),QMessageBox::Ok);
        return ;
    }
    else
    {
        if (m_collabDialog) {
            delete m_collabDialog;
            m_collabDialog = 0;
        }
        m_collabDialog = new CollabDialog(this);

        connect(m_collabDialog, SIGNAL(accepted()), this, SLOT(startCollaborating()));
        //connect(m_collabDialog, SIGNAL(finished(int)), this, SLOT(startCollaborating(int)));
        connect(m_collabDialog, SIGNAL(rejected()), this, SLOT(collaborationCancelled()));

        m_collabDialog->show();
    }

}

void MainWindow::closeCollabDialog() {
    qDebug() << "Collaborate-Dialog: closeDialog() called";
    //delete m_collabDialog;
    //m_collabDialog = 0;
    m_collabDialog->hide();
    qDebug() << "Collaborate-Dialog: closeDialog() done";
}

void MainWindow::startCollaborating() {
    qDebug() << "Collaborate-Dialog: start ";
    if (m_collabDialog->isClient()) {
        if (m_doc) {
            qDebug() << "A document is already open. Cannot start client.";
            return closeCollabDialog();
        }
        m_collab = new CollabClient(m_collabDialog->getNick(),
                                    m_collabDialog->getAddress(),
                                    m_collabDialog->getPort(),
                                    this);
        connect(m_collab, SIGNAL(openFile(const QString&)), this, SLOT(collabOpenFile(QString)));

    } else if (m_collabDialog->isServer()) {
        if (!m_doc) {
            qDebug() << "No document is open. Cannot start server.";
            return closeCollabDialog();
        }
        m_collab = new CollabServer(m_collabDialog->getNick(),
                                    m_url.fileName(),
                                    m_collabDialog->getPort(),
                                    this);
        connect(m_collab, SIGNAL(saveFile(const QString&)), this, SLOT(collabSaveFile(const QString&)));
        m_collabEditor = new KoTextEditor(m_editor->document()); // qobject_cast<KoTextEditor*>(qobject_cast<KWView*>(m_doc->createView(this))->kwcanvas()->toolProxy()->selection());

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

void MainWindow::collaborationCancelled() {
    closeCollabDialog();
}

void MainWindow::receivedString(uint start, uint end, QByteArray msg) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    m_collabEditor->insertText(msg);
}

void MainWindow::receivedBackspace(uint start, uint end) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    m_collabEditor->deletePreviousChar();
}

void MainWindow::receivedFormat(uint start, uint end, Collaborate::FormatFlag format) {
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

void MainWindow::receivedFontSize(uint start, uint end, uint size) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setFontSize(size, m_collabEditor);
}

void MainWindow::receivedFontType(uint start, uint end, const QString &font) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setFontType(font, m_collabEditor);
}

void MainWindow::receivedTextColor(uint start, uint end, QRgb color) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setTextColor(QColor(color), m_collabEditor);
}

void MainWindow::receivedTextBackgroundColor(uint start, uint end, QRgb color) {
    m_collabEditor->setPosition(start);
    m_collabEditor->setPosition(end, QTextCursor::KeepAnchor);
    setTextBackgroundColor(QColor(color), m_collabEditor);
}

void MainWindow::error(quint16 err) {
    qDebug() << "Collaborate: Error: " << err;
}

void MainWindow::collabSaveFile(const QString &filename) {
    m_doc->saveNativeFormat(filename);
}

void MainWindow::collabOpenFile(const QString &filename) {
    m_fileName = filename;
    openDocument(filename);
    qDebug() << "============================================";
    m_collabEditor = new KoTextEditor(m_editor->document()); // qobject_cast<KoTextEditor*>(qobject_cast<KWView*>(m_doc->createView(this))->kwcanvas()->toolProxy()->selection());
    qDebug() << "::::::::::::::::::::::::::::::::::::::::::::";
}

///////////////////////////
///////////////////////////
