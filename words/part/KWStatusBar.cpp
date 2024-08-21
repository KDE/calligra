/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2008-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWStatusBar.h"
#include "KWDocument.h"
#include "KWView.h"
#include "frames/KWTextFrameSet.h"

#include <KoCanvasBase.h>
#include <KoCanvasControllerWidget.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextEditor.h>
#include <KoTextLayoutRootArea.h>
#include <KoToolManager.h>
#include <KoZoomController.h>

#include <QAction>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTimer>
#include <QToolButton>

#include <KLocalizedString>
#include <ksqueezedtextlabel.h>

const KLocalizedString i18nModified = ki18n("Modified");
const KLocalizedString i18nSaved = ki18n("Saved");
const KLocalizedString i18nPage = ki18n("Page %1 of %2");
const KLocalizedString i18nPageRange = ki18n("Page %1-%2 of %3");
const KLocalizedString i18nLine = ki18n("Line %1");

#define KWSTATUSBAR "KWStatusBarPointer"

class KWStatusBarBaseItem : public QStackedWidget
{
public:
    QLabel *m_label;
    QWidget *m_widget;
    KWStatusBarBaseItem(QWidget *parent = nullptr)
        : QStackedWidget(parent)
        , m_widget(nullptr)
    {
#ifdef Q_WS_MAC
        setAttribute(Qt::WA_MacMiniSize, true);
#endif
        m_label = new QLabel(this);
        addWidget(m_label);
    }

protected:
    void enterEvent(QEnterEvent *) override
    {
        setCurrentIndex(1);
    }
    void leaveEvent(QEvent *) override
    {
        if (m_widget) {
            if (m_widget->hasFocus()) {
                m_widget->installEventFilter(this);
            } else {
                setCurrentIndex(0);
                m_widget->removeEventFilter(this);
            }
        }
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (watched == m_widget && event->type() == QEvent::FocusOut && !m_widget->hasFocus()) {
            setCurrentIndex(0);
            m_widget->removeEventFilter(this);
        }
        return false;
    }
};

class KWStatusBarEditItem : public KWStatusBarBaseItem
{
public:
    QLineEdit *m_edit;
    KWStatusBarEditItem(QWidget *parent = nullptr)
        : KWStatusBarBaseItem(parent)
    {
        m_edit = new QLineEdit(this);
        m_edit->setValidator(new QIntValidator(m_edit));
        m_edit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        m_widget = m_edit;
        addWidget(m_widget);
    }
};

class KWStatusBarButtonItem : public KWStatusBarBaseItem
{
public:
    QToolButton *m_button;
    KWStatusBarButtonItem(QWidget *parent = nullptr)
        : KWStatusBarBaseItem(parent)
    {
        m_button = new QToolButton(this);
        m_widget = m_button;
        addWidget(m_widget);
    }
};

KWStatusBar::KWStatusBar(QStatusBar *statusBar, KWView *view)
    : QObject(statusBar)
    , m_statusbar(statusBar)
    , m_controller(nullptr)
    , m_currentPageNumber(0)
{
    KWDocument *document = view->kwdocument();
    m_statusbar->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_pageLabel = new KWStatusBarEditItem();
    m_pageLabel->setFixedWidth(
        QFontMetrics(m_pageLabel->m_label->font()).boundingRect(i18nPageRange.subs("9999").subs("9999").subs("9999").toString()).width());
    m_statusbar->addWidget(m_pageLabel);
    m_pageLabel->setVisible(document->config().statusBarShowPage());
    connect(m_pageLabel->m_edit, &QLineEdit::returnPressed, this, [this]() {
        gotoPage();
    });
    connect(document, &KWDocument::pageSetupChanged, this, &KWStatusBar::updatePageCount);

    QAction *action = new QAction(i18n("Page Number"), this);
    action->setObjectName("pages_current_total");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowPage());
    m_statusbar->addAction(action);
    connect(action, &QAction::toggled, this, &KWStatusBar::showPage);

    m_lineLabel = new KWStatusBarEditItem();
    m_lineLabel->setFixedWidth(QFontMetrics(m_lineLabel->m_label->font()).boundingRect(i18nLine.subs("999999").toString()).width());
    m_statusbar->addWidget(m_lineLabel);
    connect(m_lineLabel->m_edit, &QLineEdit::returnPressed, this, &KWStatusBar::gotoLine);
    m_lineLabel->setVisible(document->config().statusBarShowLineNumber());

    action = new QAction(i18n("Line Number"), this);
    action->setObjectName("textcursor_position");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowLineNumber());
    m_statusbar->addAction(action);
    connect(action, &QAction::toggled, this, &KWStatusBar::showLineColumn);

    m_pageStyleLabel = new KWStatusBarButtonItem();
    QFontMetrics psfm(m_pageStyleLabel->m_label->font());
    m_pageStyleLabel->setFixedWidth(psfm.boundingRect(i18n("Standard")).width() * 2.5);
    m_pageStyleLabel->m_button->setMinimumHeight(psfm.height());
    m_pageStyleLabel->m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pageStyleLabel->setToolTip(i18n("Change the page style"));
    m_statusbar->addWidget(m_pageStyleLabel);
    connect(m_pageStyleLabel->m_button, &QToolButton::clicked, this, [this]() {
        showPageStyle();
    });
    connect(document, &KWDocument::pageSetupChanged, this, &KWStatusBar::updatePageStyle);
    m_pageStyleLabel->setVisible(document->config().statusBarShowPageStyle());

    action = new QAction(i18n("Page Style"), this);
    action->setObjectName("pagestyle_current_name");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowPageStyle());
    m_statusbar->addAction(action);
    connect(action, &QAction::toggled, this, QOverload<bool>::of(&KWStatusBar::showPageStyle));

    m_pageSizeLabel = new QLabel();
    m_pageSizeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pageSizeLabel->setMinimumWidth(QFontMetrics(m_pageSizeLabel->font()).boundingRect("99999x99999").width());
    m_statusbar->addWidget(m_pageSizeLabel);
    m_pageSizeLabel->setVisible(document->config().statusBarShowPageSize());
    connect(document, &KWDocument::pageSetupChanged, this, &KWStatusBar::updatePageSize);

    action = new QAction(i18n("Page Size"), this);
    action->setObjectName("pagestyle_current_size");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowPageSize());
    m_statusbar->addAction(action);
    connect(action, &QAction::toggled, this, &KWStatusBar::showPageSize);

    m_modifiedLabel = new QLabel(m_statusbar);
    m_modifiedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFontMetrics modfm(m_modifiedLabel->font());
    m_modifiedLabel->setMinimumWidth(qMax(modfm.boundingRect(i18nModified.toString()).width(), modfm.boundingRect(i18nSaved.toString()).width()));
    m_statusbar->addWidget(m_modifiedLabel);
    m_modifiedLabel->setVisible(document->config().statusBarShowModified());
    connect(document, &KoDocument::modified, this, &KWStatusBar::setModified);

    action = new QAction(i18n("Saved/Modified"), this);
    action->setObjectName("doc_save_state");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowModified());
    m_statusbar->addAction(action);
    connect(action, &QAction::toggled, this, &KWStatusBar::showModified);

    m_mousePosLabel = new QLabel(m_statusbar);
    m_mousePosLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_mousePosLabel->setMinimumWidth(QFontMetrics(m_mousePosLabel->font()).boundingRect("99999:99999").width());
    m_statusbar->addWidget(m_mousePosLabel);
    m_mousePosLabel->setVisible(document->config().statusBarShowMouse());

    action = new QAction(i18n("Mouse Cursor X:Y"), this);
    action->setObjectName("mousecursor_position");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowMouse());
    m_statusbar->addAction(action);
    connect(action, &QAction::toggled, this, &KWStatusBar::showMouse);

    m_statusLabel = new KSqueezedTextLabel(m_statusbar);
    m_statusLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_statusbar->addWidget(m_statusLabel, 1);
    connect(m_statusbar, &QStatusBar::messageChanged, this, &KWStatusBar::setText);
    connect(KoToolManager::instance(), &KoToolManager::changedStatusText, this, &KWStatusBar::setText);

    m_zoomAction = new QAction(i18n("Zoom Controller"), this);
    m_zoomAction->setObjectName("zoom_controller");
    m_zoomAction->setCheckable(true);
    m_zoomAction->setChecked(document->config().statusBarShowZoom());
    m_statusbar->addAction(m_zoomAction);

    updateCurrentTool(nullptr);
    setCurrentView(view);
    connect(KoToolManager::instance(), &KoToolManager::changedTool, this, &KWStatusBar::updateCurrentTool);
}

KWStatusBar::~KWStatusBar()
{
    // delete these as they are children of the statusBar but we want to delete them when the view disappears
    delete m_modifiedLabel;
    delete m_pageLabel;
    delete m_mousePosLabel;
    delete m_statusLabel;
    foreach (QWidget *widget, m_zoomWidgets)
        widget->deleteLater();

    m_statusbar->setProperty(KWSTATUSBAR, QVariant());
}

void KWStatusBar::setText(const QString &text)
{
    m_statusLabel->setText(text);
}

void KWStatusBar::setModified(bool modified)
{
    m_modifiedLabel->setText(modified ? i18nModified.toString() : i18nSaved.toString());
}

void KWStatusBar::updatePageCount()
{
    if (m_currentView) {
        if (m_currentView->minPageNumber() == m_currentView->maxPageNumber()) {
            m_pageLabel->m_label->setText(i18nPage.subs(m_currentView->minPageNumber()).subs(m_currentView->kwdocument()->pageCount()).toString());
        } else {
            m_pageLabel->m_label->setText(i18nPageRange.subs(m_currentView->minPageNumber())
                                              .subs(m_currentView->maxPageNumber())
                                              .subs(m_currentView->kwdocument()->pageCount())
                                              .toString());
        }
        m_pageLabel->m_edit->setText(QString::number(m_currentView->currentPage().pageNumber()));
        if (m_modifiedLabel->text().isEmpty())
            setModified(m_currentView->kwdocument()->isModified());
    } else {
        m_pageLabel->m_label->setText(i18nPage.toString());
        m_pageLabel->m_edit->setText(QString());
        m_modifiedLabel->setText(QString());
    }
}

void KWStatusBar::gotoPage(int pagenumber)
{
    if (!m_currentView)
        return;
    if (pagenumber < 0)
        pagenumber = m_pageLabel->m_edit->text().toInt();
    KWPage page = m_currentView->kwdocument()->pageManager()->page(pagenumber);
    if (!page.isValid())
        return;
    m_currentView->canvasBase()->ensureVisible(page.rect());
}

void KWStatusBar::updatePageStyle()
{
    KWPage page = m_currentView ? m_currentView->currentPage() : KWPage();
    QString name;
    if (page.isValid() && page.pageStyle().isValid()) {
        if (!page.pageStyle().displayName().isEmpty())
            name = page.pageStyle().displayName();
        else
            name = page.pageStyle().name();
    }
    m_pageStyleLabel->m_label->setText(name);
    m_pageStyleLabel->m_button->setText(name);
}

void KWStatusBar::showPageStyle()
{
    if (m_currentView)
        m_currentView->formatPage();
}

void KWStatusBar::updatePageSize()
{
    KWPage page = m_currentView ? m_currentView->currentPage() : KWPage();
    QString text;
    if (page.isValid() && page.pageStyle().isValid()) {
        KoPageLayout l = page.pageStyle().pageLayout();
        QLocale locale;
        text = QString::fromLatin1("%1x%2").arg(locale.toString(l.width, 'f', 0), locale.toString(l.height, 'f', 0));
    }
    m_pageSizeLabel->setText(text);
}

void KWStatusBar::updateCursorPosition()
{
    int line = 1;
    KWTextFrameSet *fs = m_currentView ? m_currentView->kwdocument()->mainFrameSet() : nullptr;
    KoTextEditor *editor = fs ? KoTextDocument(fs->document()).textEditor() : nullptr;
    if (editor) {
        line = editor->block().firstLineNumber();
        int posInDoc = editor->position() - editor->block().position();
        line += editor->block().layout()->lineForTextPosition(posInDoc).lineNumber() + 1;
    }
    m_lineLabel->m_label->setText(i18nLine.subs(line).toString());
    m_lineLabel->m_edit->setText(QString::number(line));
}

void KWStatusBar::gotoLine()
{
    if (!m_currentView)
        return;
    int linenumber = m_lineLabel->m_edit->text().toInt();
    KWTextFrameSet *fs = m_currentView->kwdocument()->mainFrameSet();
    QTextBlock block = fs ? fs->document()->findBlockByLineNumber(linenumber) : QTextBlock();
    if (!block.isValid())
        return;
    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout *>(fs->document()->documentLayout());
    Q_ASSERT(lay);
    KoTextLayoutRootArea *area = lay->rootAreaForPosition(block.position());
    if (!area)
        return;
    gotoPage(area->page()->pageNumber());
    // m_currentView->canvasBase()->ensureVisible(block.layout()->boundingRect());
}

void KWStatusBar::updateMousePosition(const QPoint &pos)
{
    if (m_mousePosLabel->isVisible())
        m_mousePosLabel->setText(QString("%1:%2").arg(pos.x()).arg(pos.y()));
}

void KWStatusBar::canvasResourceChanged(int key, const QVariant &value)
{
    Q_UNUSED(value);
    if (key == KoCanvasResourceManager::CurrentPage) {
        updateCursorPosition();
        updatePageStyle();
        updatePageSize();
    }
}

void KWStatusBar::updateCurrentTool(KoCanvasController *canvasController)
{
    KoCanvasControllerWidget *widget = dynamic_cast<KoCanvasControllerWidget *>(canvasController);
    if (!widget) {
        return;
    }
    QWidget *root = m_statusbar->window();
    if (root && !root->isAncestorOf(widget))
        return; // ignore tool changes in other mainWindows

    if (m_controller) {
        disconnect(m_controller.data(), &KoCanvasControllerProxyObject::canvasMousePositionChanged, this, &KWStatusBar::updateMousePosition);
    }
    m_controller = canvasController->proxyObject;
    if (canvasController) {
        // find KWView parent of the canvas controller widget
        KWView *view = nullptr;
        QWidget *parent = widget->parentWidget();
        while (view == nullptr && parent != nullptr) {
            view = dynamic_cast<KWView *>(parent);
            if (!view) {
                parent = parent->parentWidget();
            }
        }
        if (view) {
            setCurrentView(view);
        }
        connect(m_controller.data(), &KoCanvasControllerProxyObject::canvasMousePositionChanged, this, &KWStatusBar::updateMousePosition);
    } else {
        m_mousePosLabel->setText(QString());
    }
}

void KWStatusBar::setCurrentView(KWView *view)
{
    if (view == nullptr) {
        m_currentView = nullptr;
        return;
    } else if (view == m_currentView) {
        return;
    } else if (view->canvasBase() == nullptr) {
        return;
    }

    if (m_currentView) {
        KoCanvasBase *const canvas = m_currentView->canvasBase();
        Q_ASSERT(canvas);
        KoCanvasResourceManager *resourceManager = canvas->resourceManager();
        Q_ASSERT(resourceManager);
        disconnect(resourceManager, &KoCanvasResourceManager::canvasResourceChanged, this, &KWStatusBar::canvasResourceChanged);
        QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
        if (zoomWidget) {
            m_statusbar->removeWidget(zoomWidget);
            disconnect(m_zoomAction, &QAction::toggled, this, &KWStatusBar::showZoom);
        }

        KWTextFrameSet *fs = m_currentView->kwdocument()->mainFrameSet();
        if (fs) {
            KoTextDocument doc(fs->document());
            KoTextEditor *editor = doc.textEditor();
            if (editor) {
                disconnect(editor, &KoTextEditor::cursorPositionChanged, this, &KWStatusBar::updateCursorPosition);
            }
        }
        disconnect(m_currentView.data(), &KWView::shownPagesChanged, this, &KWStatusBar::updatePageCount);
    }

    m_currentView = view;

    updatePageCount();
    updateCursorPosition();
    updatePageStyle();
    updatePageSize();

    if (m_currentView == nullptr)
        return;

    QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
    if (zoomWidget) {
        m_statusbar->addWidget(zoomWidget);
        connect(m_zoomAction, &QAction::toggled, this, &KWStatusBar::showZoom);
        zoomWidget->setVisible(m_currentView->kwdocument()->config().statusBarShowZoom());
    } else {
        createZoomWidget();
    }

    KoCanvasResourceManager *resourceManager = view->canvasBase()->resourceManager();
    Q_ASSERT(resourceManager);
    connect(resourceManager, &KoCanvasResourceManager::canvasResourceChanged, this, &KWStatusBar::canvasResourceChanged, Qt::QueuedConnection);

    KWTextFrameSet *fs = m_currentView->kwdocument()->mainFrameSet();
    if (fs) {
        KoTextDocument doc(fs->document());
        KoTextEditor *editor = doc.textEditor();
        if (editor) {
            connect(editor, &KoTextEditor::cursorPositionChanged, this, &KWStatusBar::updateCursorPosition, Qt::QueuedConnection);
        }
    }
    connect(m_currentView.data(), &KWView::shownPagesChanged, this, &KWStatusBar::updatePageCount);
}

void KWStatusBar::createZoomWidget()
{
    if (m_currentView) {
        KoZoomController *zoomController = m_currentView->zoomController();
        if (zoomController && !m_zoomWidgets.contains(m_currentView)) {
            QWidget *zoomWidget = zoomController->zoomAction()->createWidget(m_statusbar);
            m_zoomWidgets.insert(m_currentView, zoomWidget);
            m_statusbar->addWidget(zoomWidget);
            connect(m_zoomAction, &QAction::toggled, this, &KWStatusBar::showZoom);
            zoomWidget->setVisible(m_currentView->kwdocument()->config().statusBarShowZoom());
        }
    }
}

void KWStatusBar::showPage(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowPage(visible);
    m_pageLabel->setVisible(visible);
}

void KWStatusBar::showPageStyle(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowPageStyle(visible);
    m_pageStyleLabel->setVisible(visible);
}

void KWStatusBar::showPageSize(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowPageSize(visible);
    m_pageSizeLabel->setVisible(visible);
}

void KWStatusBar::showLineColumn(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowLineNumber(visible);
    m_lineLabel->setVisible(visible);
}

void KWStatusBar::showModified(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowModified(visible);
    m_modifiedLabel->setVisible(visible);
}

void KWStatusBar::showMouse(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowMouse(visible);
    m_mousePosLabel->setVisible(visible);
}

void KWStatusBar::showZoom(bool visible)
{
    Q_ASSERT(m_currentView);
    QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowZoom(visible);
    zoomWidget->setVisible(visible);
}

void KWStatusBar::removeView(QObject *object)
{
    KWView *view = static_cast<KWView *>(object);
    QWidget *widget = m_zoomWidgets.value(view);
    if (widget) {
        widget->deleteLater();
        m_zoomWidgets.remove(view);
    }
    if (view == m_currentView)
        m_currentView = nullptr;
}

// static
void KWStatusBar::addViewControls(QStatusBar *statusBar, KWView *view)
{
    /**
     * Life time of a KWStatusBar is tricky...
     * One main window has one QStatusBar.  But it can be re-used by different
     *  documents and thus by many different KWView instances.
     * So;  open a document in a window creates a KWView. That creates a KWStatusBar
     *      split the view creates a new KWView in the same mainwindow, this reuses
     *      the already existing KWStatusBar
     *      Create a new view (new MainWindow) also creates a new KWStatusBar
     *      Close all your views (deletes all KWViews) but not your Mainwindow will
     *      NOT destroy all KWStatusBar instance.  Note that QStatusBar is not
     *      destructed in that case either.
     */

    QVariant variant = statusBar->property(KWSTATUSBAR);
    if (variant.isValid()) { // already exists!
        KWStatusBar *decorator = static_cast<KWStatusBar *>(variant.value<void *>());
        if (decorator)
            decorator->connect(view, &QObject::destroyed, decorator, &KWStatusBar::removeView);
        return;
    }
    KWStatusBar *decorator = new KWStatusBar(statusBar, view);
    decorator->connect(view, &QObject::destroyed, decorator, &KWStatusBar::removeView);
    variant.setValue<void *>(decorator);
    statusBar->setProperty(KWSTATUSBAR, variant);
}
