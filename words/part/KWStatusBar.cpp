/* This file is part of the KDE project
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2008-2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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
 * Boston, MA 02110-1301, USA
 */

#include "KWStatusBar.h"
#include "KWView.h"
#include "KWDocument.h"
#include "frames/KWTextFrameSet.h"

#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoCanvasControllerWidget.h>
#include <KoZoomController.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>

#include <QStackedWidget>
#include <QLabel>
#include <QStatusBar>
#include <QLineEdit>
#include <QIntValidator>
#include <QToolButton>
#include <QTimer>
#include <QAction>

#include <ksqueezedtextlabel.h>
#include <klocalizedstring.h>

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
    KWStatusBarBaseItem(QWidget *parent = 0) : QStackedWidget(parent), m_widget(0)
    {
#ifdef Q_WS_MAC
        setAttribute(Qt::WA_MacMiniSize, true);
#endif
        m_label = new QLabel(this);
        addWidget(m_label);
    }
protected:
    void enterEvent(QEvent*) override
    {
        setCurrentIndex(1);
    }
    void leaveEvent(QEvent*) override
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
    KWStatusBarEditItem(QWidget *parent = 0) : KWStatusBarBaseItem(parent)
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
    KWStatusBarButtonItem(QWidget *parent = 0) : KWStatusBarBaseItem(parent)
    {
        m_button = new QToolButton(this);
        m_widget = m_button;
        addWidget(m_widget);
    }
};

KWStatusBar::KWStatusBar(QStatusBar *statusBar, KWView *view)
    : QObject(statusBar),
    m_statusbar(statusBar),
    m_controller(0),
    m_currentPageNumber(0)
{
    KWDocument *document = view->kwdocument();
    m_statusbar->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_pageLabel = new KWStatusBarEditItem();
    m_pageLabel->setFixedWidth(QFontMetrics(m_pageLabel->m_label->font()).width(i18nPageRange.subs("9999").subs("9999").subs("9999").toString()));
    m_statusbar->addWidget(m_pageLabel);
    m_pageLabel->setVisible(document->config().statusBarShowPage());
    connect(m_pageLabel->m_edit, SIGNAL(returnPressed()), this, SLOT(gotoPage()));
    connect(document, SIGNAL(pageSetupChanged()), this, SLOT(updatePageCount()));

    QAction *action = new QAction(i18n("Page Number"), this);
    action->setObjectName("pages_current_total");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowPage());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showPage(bool)));

    m_lineLabel = new KWStatusBarEditItem();
    m_lineLabel->setFixedWidth(QFontMetrics(m_lineLabel->m_label->font()).width(i18nLine.subs("999999").toString()));
    m_statusbar->addWidget(m_lineLabel);
    connect(m_lineLabel->m_edit, SIGNAL(returnPressed()), this, SLOT(gotoLine()));
    m_lineLabel->setVisible(document->config().statusBarShowLineNumber());

    action = new QAction(i18n("Line Number"), this);
    action->setObjectName("textcursor_position");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowLineNumber());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showLineColumn(bool)));

    m_pageStyleLabel = new KWStatusBarButtonItem();
    QFontMetrics psfm(m_pageStyleLabel->m_label->font());
    m_pageStyleLabel->setFixedWidth(psfm.width(I18N_NOOP("Standard")) * 2.5);
    m_pageStyleLabel->m_button->setMinimumHeight(psfm.height());
    m_pageStyleLabel->m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_statusbar->addWidget(m_pageStyleLabel);
    connect(m_pageStyleLabel->m_button, SIGNAL(clicked()), this, SLOT(showPageStyle()));
    connect(document, SIGNAL(pageSetupChanged()), this, SLOT(updatePageStyle()));
    m_pageStyleLabel->setVisible(document->config().statusBarShowPageStyle());

    action = new QAction(i18n("Page Style"), this);
    action->setObjectName("pagestyle_current_name");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowPageStyle());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showPageStyle(bool)));

    m_pageSizeLabel = new QLabel();
    m_pageSizeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pageSizeLabel->setMinimumWidth(QFontMetrics(m_pageSizeLabel->font()).width("99999x99999"));
    m_statusbar->addWidget(m_pageSizeLabel);
    m_pageSizeLabel->setVisible(document->config().statusBarShowPageSize());
    connect(document, SIGNAL(pageSetupChanged()), this, SLOT(updatePageSize()));

    action = new QAction(i18n("Page Size"), this);
    action->setObjectName("pagestyle_current_size");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowPageSize());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showPageSize(bool)));

    m_modifiedLabel = new QLabel(m_statusbar);
    m_modifiedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFontMetrics modfm(m_modifiedLabel->font());
    m_modifiedLabel->setMinimumWidth(qMax(modfm.width(i18nModified.toString()), modfm.width(i18nSaved.toString())));
    m_statusbar->addWidget(m_modifiedLabel);
    m_modifiedLabel->setVisible(document->config().statusBarShowModified());
    connect(document, SIGNAL(modified(bool)), this, SLOT(setModified(bool)));

    action = new QAction(i18n("Saved/Modified"), this);
    action->setObjectName("doc_save_state");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowModified());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showModified(bool)));

    m_mousePosLabel = new QLabel(m_statusbar);
    m_mousePosLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_mousePosLabel->setMinimumWidth(QFontMetrics(m_mousePosLabel->font()).width("99999:99999"));
    m_statusbar->addWidget(m_mousePosLabel);
    m_mousePosLabel->setVisible(document->config().statusBarShowMouse());

    action = new QAction(i18n("Mouse Cursor X:Y"), this);
    action->setObjectName("mousecursor_position");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowMouse());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showMouse(bool)));

    m_statusLabel = new KSqueezedTextLabel(m_statusbar);
    m_statusLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_statusbar->addWidget(m_statusLabel, 1);
    connect(m_statusbar, SIGNAL(messageChanged(QString)), this, SLOT(setText(QString)));
    connect(KoToolManager::instance(), SIGNAL(changedStatusText(QString)),
            this, SLOT(setText(QString)));

    m_zoomAction = new QAction(i18n("Zoom Controller"), this);
    m_zoomAction->setObjectName("zoom_controller");
    m_zoomAction->setCheckable(true);
    m_zoomAction->setChecked(document->config().statusBarShowZoom());
    m_statusbar->addAction(m_zoomAction);

    updateCurrentTool(0);
    setCurrentView(view);
    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*,int)),
            this, SLOT(updateCurrentTool(KoCanvasController*)));
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
            m_pageLabel->m_label->setText(i18nPageRange.subs(m_currentView->minPageNumber()).subs(m_currentView->maxPageNumber()).subs(m_currentView->kwdocument()->pageCount()).toString());
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
    QString name = (page.isValid() && page.pageStyle().isValid() 
		    ? page.pageStyle().displayName()
		    : QString());
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
    KWTextFrameSet *fs = m_currentView ? m_currentView->kwdocument()->mainFrameSet() : 0;
    KoTextEditor *editor = fs ? KoTextDocument(fs->document()).textEditor() : 0;
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
    KoTextDocumentLayout *lay = dynamic_cast<KoTextDocumentLayout*>(fs->document()->documentLayout());
    Q_ASSERT(lay);
    KoTextLayoutRootArea *area = lay->rootAreaForPosition(block.position());
    if (!area)
        return;
    gotoPage(area->page()->pageNumber());
    //m_currentView->canvasBase()->ensureVisible(block.layout()->boundingRect());
}

void KWStatusBar::updateMousePosition(const QPoint &pos)
{
    if (m_mousePosLabel->isVisible())
        m_mousePosLabel->setText(QString("%1:%2").arg(pos.x()).arg(pos.y()));
}

void KWStatusBar::canvasResourceChanged(int key, const QVariant &value)
{
    Q_UNUSED(value);
    if (key ==  KoCanvasResourceManager::CurrentPage) {
        updateCursorPosition();
        updatePageStyle();
        updatePageSize();
    }
}

void KWStatusBar::updateCurrentTool(KoCanvasController *canvasController)
{
    KoCanvasControllerWidget *widget = dynamic_cast<KoCanvasControllerWidget*>(canvasController);
    if (!widget) {
        return;
    }
    QWidget *root = m_statusbar->window();
    if (root && !root->isAncestorOf(widget))
        return; // ignore tool changes in other mainWindows

    if (m_controller) {
        disconnect(m_controller, SIGNAL(canvasMousePositionChanged(QPoint)),
                this, SLOT(updateMousePosition(QPoint)));
    }
    m_controller = canvasController->proxyObject;
    if (canvasController) {
        // find KWView parent of the canvas controller widget
        KWView *view = 0;
        QWidget *parent = widget->parentWidget();
        while (view == 0 && parent != 0) {
            view = dynamic_cast<KWView*>(parent);
            if (!view) {
                parent = parent->parentWidget();
            }
        }
        if (view) {
            setCurrentView(view);
        }
        connect(m_controller, SIGNAL(canvasMousePositionChanged(QPoint)), this,
                SLOT(updateMousePosition(QPoint)));
    } else {
        m_mousePosLabel->setText(QString());
    }
}

void KWStatusBar::setCurrentView(KWView *view)
{
    if (view == 0) {
        m_currentView = 0;
        return;
    } else if (view == m_currentView) {
        return;
    } else if (view->canvasBase() == 0 ) {
        return;
    }

    if (m_currentView) {
        KoCanvasBase *const canvas =  m_currentView->canvasBase();
        Q_ASSERT(canvas);
        KoCanvasResourceManager *resourceManager = canvas->resourceManager();
        Q_ASSERT(resourceManager);
        disconnect(resourceManager, SIGNAL(canvasResourceChanged(int,QVariant)),
            this, SLOT(canvasResourceChanged(int,QVariant)));
        QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
        if (zoomWidget) {
            m_statusbar->removeWidget(zoomWidget);
            disconnect(m_zoomAction, SIGNAL(toggled(bool)), this, SLOT(showZoom(bool)));
        }

        KWTextFrameSet *fs = m_currentView->kwdocument()->mainFrameSet();
        if (fs) {
            KoTextDocument doc(fs->document());
            KoTextEditor *editor = doc.textEditor();
            if (editor) {
                disconnect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursorPosition()));
            }
        }
        disconnect(m_currentView, SIGNAL(shownPagesChanged()), this, SLOT(updatePageCount()));
    }

    m_currentView = view;

    updatePageCount();
    updateCursorPosition();
    updatePageStyle();
    updatePageSize();

    if (m_currentView == 0)
        return;

    QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
    if (zoomWidget) {
        m_statusbar->addWidget(zoomWidget);
        connect(m_zoomAction, SIGNAL(toggled(bool)), this, SLOT(showZoom(bool)));
        zoomWidget->setVisible(m_currentView->kwdocument()->config().statusBarShowZoom());
    } else {
        createZoomWidget();
    }

    KoCanvasResourceManager *resourceManager = view->canvasBase()->resourceManager();
    Q_ASSERT(resourceManager);
    connect(resourceManager, SIGNAL(canvasResourceChanged(int,QVariant)), this, SLOT(canvasResourceChanged(int,QVariant)), Qt::QueuedConnection);

    KWTextFrameSet *fs = m_currentView->kwdocument()->mainFrameSet();
    if (fs) {
        KoTextDocument doc(fs->document());
        KoTextEditor *editor = doc.textEditor();
        if (editor) {
            connect(editor, SIGNAL(cursorPositionChanged()), this, SLOT(updateCursorPosition()), Qt::QueuedConnection);
        }
    }
    connect(m_currentView, SIGNAL(shownPagesChanged()), this, SLOT(updatePageCount()));
}

void KWStatusBar::createZoomWidget()
{
    if (m_currentView) {
        KoZoomController *zoomController = m_currentView->zoomController();
        if (zoomController && !m_zoomWidgets.contains(m_currentView)) {
            QWidget *zoomWidget = zoomController->zoomAction()->createWidget(m_statusbar);
            m_zoomWidgets.insert(m_currentView, zoomWidget);
            m_statusbar->addWidget(zoomWidget);
            connect(m_zoomAction, SIGNAL(toggled(bool)), this, SLOT(showZoom(bool)));
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
    KWView *view = static_cast<KWView*>(object);
    QWidget *widget = m_zoomWidgets.value(view);
    if (widget) {
        widget->deleteLater();
        m_zoomWidgets.remove(view);
    }
    if (view == m_currentView)
        m_currentView = 0;
}

//static
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
        KWStatusBar *decorator = static_cast<KWStatusBar*>(variant.value<void*>());
        if (decorator)
            decorator->connect(view, SIGNAL(destroyed(QObject*)), SLOT(removeView(QObject*)));
        return;
    }
    KWStatusBar *decorator = new KWStatusBar(statusBar, view);
    decorator->connect(view, SIGNAL(destroyed(QObject*)), SLOT(removeView(QObject*)));
    variant.setValue<void*>(decorator);
    statusBar->setProperty(KWSTATUSBAR, variant);
}
