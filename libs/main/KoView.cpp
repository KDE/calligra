/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoView.h"

// local directory
#include "KoShapeCollectionMenu.h"
#include "KoView_p.h"

#include "KoDockRegistry.h"
#include "KoDocument.h"
#include "KoMainWindow.h"
#include "KoPart.h"

#ifdef WITH_QTDBUS
#include "KoViewAdaptor.h"
#include <QDBusConnection>
#endif

#include "KoDockFactoryBase.h"
#include "KoDocumentInfo.h"
#include "KoGlobal.h"
#include "KoPageLayout.h"
#include "KoPrintJob.h"
#include "KoUndoStackAction.h"

#include <KoIcon.h>

#include <KActionCollection>
#include <KActionMenu>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <KoNetAccess.h>
#include <MainDebug.h>
#include <kselectaction.h>

#include <QApplication>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QImage>
#include <QList>
#include <QMimeData>
#include <QPointer>
#include <QPrintDialog>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QUrl>

// static
QString KoView::newObjectName()
{
    static int s_viewIFNumber = 0;
    QString name;
    name.setNum(s_viewIFNumber++);
    name.prepend("view_");
    return name;
}

class KoViewPrivate
{
public:
    KoViewPrivate()
    {
        tempActiveWidget = nullptr;
        documentDeleted = false;
        actionAuthor = nullptr;
    }
    ~KoViewPrivate() = default;

    QPointer<KoDocument> document; // our KoDocument
    QPointer<KoPart> part; // our part
    QWidget *tempActiveWidget;
    bool documentDeleted; // true when document gets deleted [can't use document==0
    // since this only happens in ~QObject, and views
    // get deleted by ~KoDocument].

    QStatusBar *statusBar;

    // Hmm sorry for polluting the private class with such a big inner class.
    // At the beginning it was a little struct :)
    class StatusBarItem
    {
    public:
        StatusBarItem() // for QValueList
            : m_widget(nullptr)
            , m_connected(false)
            , m_hidden(false)
        {
        }

        StatusBarItem(QWidget *widget, int stretch, bool permanent)
            : m_widget(widget)
            , m_stretch(stretch)
            , m_permanent(permanent)
            , m_connected(false)
            , m_hidden(false)
        {
        }

        bool operator==(const StatusBarItem &rhs) const
        {
            return m_widget == rhs.m_widget;
        }

        bool operator!=(const StatusBarItem &rhs) const
        {
            return m_widget != rhs.m_widget;
        }

        QWidget *widget() const
        {
            return m_widget;
        }

        void ensureItemShown(QStatusBar *sb)
        {
            Q_ASSERT(m_widget);
            if (!m_connected) {
                if (m_permanent)
                    sb->addPermanentWidget(m_widget, m_stretch);
                else
                    sb->addWidget(m_widget, m_stretch);

                if (!m_hidden)
                    m_widget->show();

                m_connected = true;
            }
        }
        void ensureItemHidden(QStatusBar *sb)
        {
            if (m_connected) {
                m_hidden = m_widget->isHidden();
                sb->removeWidget(m_widget);
                m_widget->hide();
                m_connected = false;
            }
        }

    private:
        QWidget *m_widget;
        int m_stretch;
        bool m_permanent;
        bool m_connected;
        bool m_hidden;
    };

    QList<StatusBarItem> statusBarItems; // Our statusbar items
    bool inOperation; // in the middle of an operation (no screen refreshing)?
    KSelectAction *actionAuthor; // Select action for author profile.
};

KoView::KoView(KoPart *part, KoDocument *document, QWidget *parent)
    : QWidget(parent)
    , d(new KoViewPrivate)
{
    Q_ASSERT(document);
    Q_ASSERT(part);

    setObjectName(newObjectName());

#ifdef WITH_QTDBUS
    new KoViewAdaptor(this);
    QDBusConnection::sessionBus().registerObject('/' + objectName(), this);
#endif

    d->document = document;
    d->part = part;

    setFocusPolicy(Qt::StrongFocus);

    setupGlobalActions();

    d->statusBar = new QStatusBar(this);
    d->statusBar->setProperty("_breeze_statusbar_separator", true);
    d->statusBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(d->document.data(), &KoDocument::statusBarMessage, this, &KoView::slotActionStatusText);
    connect(d->document.data(), &KoDocument::clearStatusBarMessage, this, &KoView::slotClearStatusText);

    // add all plugins.
    const auto dockers = KoDockRegistry::instance()->keys();
    for (const QString &docker : dockers) {
        KoDockFactoryBase *factory = KoDockRegistry::instance()->value(docker);
        if (mainWindow()) {
            mainWindow()->createDockWidget(factory);
        }
    }

    actionCollection()->addAssociatedWidget(this);

    /**
     * WARNING: This code changes the context of global shortcuts
     *          only. All actions added later will have the default
     *          context, which is Qt::WindowShortcut!
     */
    const auto actions = actionCollection()->actions();
    for (QAction *action : actions) {
        action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }
}

KoView::~KoView()
{
    if (!d->documentDeleted) {
        if (d->document) {
            d->part->removeView(this);
        }
    }
    delete d;
}

void KoView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void KoView::dropEvent(QDropEvent *event)
{
    // we can drop a list of urls from, for instance dolphin
    QVector<QImage> images;

    if (event->mimeData()->hasImage()) {
        QImage image = event->mimeData()->imageData().value<QImage>();
        if (!image.isNull()) {
            // apparently hasImage() && imageData().value<QImage>().isNull()
            // can hold sometimes (Qt bug?).
            images << image;
        }
    } else if (event->mimeData()->hasUrls()) {
        const QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl &url : urls) {
            QImage image;
            // make sure we download the files before inserting them
            if (!url.isLocalFile()) {
                QString tmpFile;
                if (KIO::NetAccess::download(url, tmpFile, this)) {
                    image.load(tmpFile);
                    KIO::NetAccess::removeTempFile(tmpFile);
                } else {
                    KMessageBox::error(this, KIO::NetAccess::lastErrorString());
                }
            } else {
                image.load(url.toLocalFile());
            }
            if (!image.isNull()) {
                images << image;
            }
        }
    }

    if (!images.isEmpty()) {
        addImages(images, event->position().toPoint());
    }
}

void KoView::addImages(const QVector<QImage> &, const QPoint &)
{
    // override in your application
}

KoDocument *KoView::koDocument() const
{
    return d->document;
}

void KoView::setDocumentDeleted()
{
    d->documentDeleted = true;
}

void KoView::addStatusBarItem(QWidget *widget, int stretch, bool permanent)
{
    KoViewPrivate::StatusBarItem item(widget, stretch, permanent);
    QStatusBar *sb = statusBar();
    if (sb) {
        item.ensureItemShown(sb);
    }
    d->statusBarItems.append(item);
}

void KoView::removeStatusBarItem(QWidget *widget)
{
    QStatusBar *sb = statusBar();

    qsizetype itemCount = d->statusBarItems.count();
    for (qsizetype i = itemCount - 1; i >= 0; --i) {
        KoViewPrivate::StatusBarItem &sbItem = d->statusBarItems[i];
        if (sbItem.widget() == widget) {
            if (sb) {
                sbItem.ensureItemHidden(sb);
            }
            d->statusBarItems.removeOne(sbItem);
            break;
        }
    }
}

KoPrintJob *KoView::createPrintJob()
{
    warnMain << "Printing not implemented in this application";
    return nullptr;
}

KoPrintJob *KoView::createPdfPrintJob()
{
    return createPrintJob();
}

KoPageLayout KoView::pageLayout() const
{
    return koDocument()->pageLayout();
}

QPrintDialog *KoView::createPrintDialog(KoPrintJob *printJob, QWidget *parent)
{
    auto printDialog = new QPrintDialog(&printJob->printer(), parent);
    printDialog->setOptionTabs(printJob->createOptionWidgets());
    printDialog->setMinMax(printJob->printer().fromPage(), printJob->printer().toPage());
    printDialog->setOptions(printJob->printDialogOptions());
    return printDialog;
}

class AddShapeAction : public KActionMenu
{
public:
    explicit AddShapeAction(QObject *parent = nullptr)
        : KActionMenu(parent)
    {
        delete menu();
        setPopupMode(QToolButton::InstantPopup);
        setMenu(new KoShapeCollectionMenu);
        setProperty("isShortcutConfigurable", false);
        setIcon(QIcon::fromTheme("shape-choose-symbolic"));
        setText(i18nc("@action:intoolbar", "Add Shape"));
    }
};

void KoView::setupGlobalActions()
{
    QAction *undo = actionCollection()->addAction("edit_undo", new KoUndoStackAction(d->document->undoStack(), KoUndoStackAction::UNDO));
    QAction *redo = actionCollection()->addAction("edit_redo", new KoUndoStackAction(d->document->undoStack(), KoUndoStackAction::RED0));

    actionCollection()->setDefaultShortcut(undo, QKeySequence::Undo);
    actionCollection()->setDefaultShortcut(redo, QKeySequence::Redo);
    d->actionAuthor = new KSelectAction(koIcon("user-identity"), i18n("Active Author Profile"), this);
    connect(d->actionAuthor, &KSelectAction::textTriggered, this, &KoView::changeAuthorProfile);
    actionCollection()->addAction("settings_active_author", d->actionAuthor);

    actionCollection()->addAction("add_shape", new AddShapeAction(this));

    slotUpdateAuthorProfileActions();
}

void KoView::changeAuthorProfile(const QString &profileName)
{
    KConfigGroup appAuthorGroup(KSharedConfig::openConfig(), "Author");
    if (profileName.isEmpty()) {
        appAuthorGroup.writeEntry("active-profile", "");
    } else if (profileName == i18nc("choice for author profile", "Anonymous")) {
        appAuthorGroup.writeEntry("active-profile", "anonymous");
    } else {
        appAuthorGroup.writeEntry("active-profile", profileName);
    }
    appAuthorGroup.sync();
    d->document->documentInfo()->updateParameters();
}

KoMainWindow *KoView::mainWindow() const
{
    // It is possible (when embedded inside a Gemini window) that you have a KoMainWindow which
    // is not the top level window. The code below ensures you can still get access to it, even
    // in that case.
    auto mw = dynamic_cast<KoMainWindow *>(window());
    QWidget *parent = parentWidget();
    while (!mw && parent) {
        mw = dynamic_cast<KoMainWindow *>(parent);
        parent = parent->parentWidget();
        if (!parent) {
            break;
        }
    }
    return mw;
}

QStatusBar *KoView::statusBar() const
{
    return d->statusBar;
}

void KoView::slotActionStatusText(const QString &text) const
{
    auto sb = statusBar();
    if (sb) {
        sb->showMessage(text);
    }
}

void KoView::slotClearStatusText() const
{
    auto sb = statusBar();
    if (sb) {
        sb->clearMessage();
    }
}

void KoView::slotUpdateAuthorProfileActions()
{
    Q_ASSERT(d->actionAuthor);
    if (!d->actionAuthor) {
        return;
    }
    d->actionAuthor->clear();
    d->actionAuthor->addAction(i18n("Default Author Profile"));
    d->actionAuthor->addAction(i18nc("choice for author profile", "Anonymous"));

    KConfigGroup authorGroup(KoGlobal::calligraConfig(), "Author");
    const QStringList profiles = authorGroup.readEntry("profile-names", QStringList());
    for (const QString &profile : profiles) {
        d->actionAuthor->addAction(profile);
    }

    KConfigGroup appAuthorGroup(KSharedConfig::openConfig(), "Author");
    QString profileName = appAuthorGroup.readEntry("active-profile", "");
    if (profileName == "anonymous") {
        d->actionAuthor->setCurrentItem(1);
    } else if (profiles.contains(profileName)) {
        d->actionAuthor->setCurrentAction(profileName);
    } else {
        d->actionAuthor->setCurrentItem(0);
    }
}

QList<QAction *> KoView::createChangeUnitActions(bool addPixelUnit)
{
    auto unitActions = new UnitActionGroup(d->document, addPixelUnit, this);
    return unitActions->actions();
}

void KoView::guiActivateEvent(bool activated)
{
    Q_UNUSED(activated);
}
