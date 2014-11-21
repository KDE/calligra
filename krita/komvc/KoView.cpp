/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2007 Thomas Zander <zander@kde.org>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoView.h"

// local directory
#include "KoView_p.h"

#include "KoPart.h"
#include "KoDockRegistry.h"
#include "KoDocument.h"
#include "KoMainWindow.h"

#include "KoDockFactoryBase.h"
#include "KoUndoStackAction.h"
#include "KoPageLayout.h"
#include "KoPrintJob.h"
#include "KoDocumentInfo.h"

#include <KoIcon.h>

#include <kactioncollection.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kstatusbar.h>
#include <kdebug.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <ktemporaryfile.h>
#include <kselectaction.h>
#include <kconfiggroup.h>
#include <kdeprintdialog.h>
#include <QDockWidget>
#include <QToolBar>
#include <QApplication>
#include <QList>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QImage>
#include <QUrl>
#include <QPrintDialog>

//static
QString KoView::newObjectName()
{
    static int s_viewIFNumber = 0;
    QString name; name.setNum(s_viewIFNumber++); name.prepend("view_");
    return name;
}


class KoViewPrivate
{
public:
    KoViewPrivate()
        : undo(0)
        , redo(0)
    {
        tempActiveWidget = 0;
        documentDeleted = false;
    }

    ~KoViewPrivate() {
    }

    QPointer<KoDocument> document; // our KoDocument
    QPointer<KoPart> part; // our part
    QWidget *tempActiveWidget;
    bool documentDeleted; // true when document gets deleted [can't use document==0
    // since this only happens in ~QObject, and views
    // get deleted by ~KoDocument].


    // Hmm sorry for polluting the private class with such a big inner class.
    // At the beginning it was a little struct :)
    class StatusBarItem
    {
    public:
        StatusBarItem() // for QValueList
            : m_widget(0),
              m_connected(false),
              m_hidden(false) {}

        StatusBarItem(QWidget * widget, int stretch, bool permanent)
            : m_widget(widget),
              m_stretch(stretch),
              m_permanent(permanent),
              m_connected(false),
              m_hidden(false) {}

        bool operator==(const StatusBarItem& rhs) {
            return m_widget == rhs.m_widget;
        }

        bool operator!=(const StatusBarItem& rhs) {
            return m_widget != rhs.m_widget;
        }

        QWidget * widget() const {
            return m_widget;
        }

        void ensureItemShown(KStatusBar * sb) {
            Q_ASSERT(m_widget);
            if (!m_connected) {
                if (m_permanent)
                    sb->addPermanentWidget(m_widget, m_stretch);
                else
                    sb->addWidget(m_widget, m_stretch);

                if(!m_hidden)
                    m_widget->show();

                m_connected = true;
            }
        }
        void ensureItemHidden(KStatusBar * sb) {
            if (m_connected) {
                m_hidden = m_widget->isHidden();
                sb->removeWidget(m_widget);
                m_widget->hide();
                m_connected = false;
            }
        }
    private:
        QWidget * m_widget;
        int m_stretch;
        bool m_permanent;
        bool m_connected;
        bool m_hidden;

    };

    KoUndoStackAction *undo;
    KoUndoStackAction *redo;

    QList<StatusBarItem> statusBarItems; // Our statusbar items
    bool inOperation; //in the middle of an operation (no screen refreshing)?
};

KoView::KoView(KoPart *part, KoDocument *document, QWidget *parent)
        : QWidget(parent)
        , d(new KoViewPrivate)
{
    Q_ASSERT(document);
    Q_ASSERT(part);

    setObjectName(newObjectName());

    d->document = document;
    d->part = part;

    setFocusPolicy(Qt::StrongFocus);

    d->undo = new KoUndoStackAction(d->document->undoStack(), KoUndoStackAction::UNDO);
    d->redo = new KoUndoStackAction(d->document->undoStack(), KoUndoStackAction::RED0);


    KStatusBar * sb = statusBar();
    if (sb) { // No statusbar in e.g. konqueror
        connect(d->document, SIGNAL(statusBarMessage(const QString&)),
                this, SLOT(slotActionStatusText(const QString&)));
        connect(d->document, SIGNAL(clearStatusBarMessage()),
                this, SLOT(slotClearStatusText()));
    }

}

KoView::~KoView()
{
    d->part->removeView(this);
    delete d;
}

KAction *KoView::undoAction() const
{
    return d->undo;
}

KAction *KoView::redoAction() const
{
    return d->redo;
}

void KoView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasImage()
            || event->mimeData()->hasUrls()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void KoView::dropEvent(QDropEvent *event)
{
    // we can drop a list of urls from, for instance dolphin
    QList<QImage> images;

    if (event->mimeData()->hasImage()) {
        QImage image = event->mimeData()->imageData().value<QImage>();
        if (!image.isNull()) {
            // apparently hasImage() && imageData().value<QImage>().isNull()
            // can hold sometimes (Qt bug?).
            images << image;
        }
    }
    else if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        foreach (const QUrl &url, urls) {
            QImage image;
            KUrl kurl(url);
            // make sure we download the files before inserting them
            if (!kurl.isLocalFile()) {
                QString tmpFile;
                if( KIO::NetAccess::download(kurl, tmpFile, this)) {
                    image.load(tmpFile);
                    KIO::NetAccess::removeTempFile(tmpFile);
                } else {
                    KMessageBox::error(this, KIO::NetAccess::lastErrorString());
                }
            }
            else {
                image.load(kurl.toLocalFile());
            }
            if (!image.isNull()) {
                images << image;
            }
        }
    }

    if (!images.isEmpty()) {
        addImages(images, event->pos());
    }
}


void KoView::addImages(const QList<QImage> &, const QPoint &)
{
    // override in your application
}

KoDocument *KoView::document() const
{
    return d->document;
}

void KoView::setDocument(KoDocument *document)
{
    d->document->disconnect(this);
    d->document = document;
    KStatusBar *sb = statusBar();
    if (sb) { // No statusbar in e.g. konqueror
        connect(d->document, SIGNAL(statusBarMessage(const QString&)),
                this, SLOT(slotActionStatusText(const QString&)));
        connect(d->document, SIGNAL(clearStatusBarMessage()),
                this, SLOT(slotClearStatusText()));
    }
}

void KoView::setDocumentDeleted()
{
    d->documentDeleted = true;
}

void KoView::addStatusBarItem(QWidget * widget, int stretch, bool permanent)
{
    KoViewPrivate::StatusBarItem item(widget, stretch, permanent);
    KStatusBar * sb = statusBar();
    if (sb) {
        item.ensureItemShown(sb);
    }
    d->statusBarItems.append(item);
}

void KoView::removeStatusBarItem(QWidget *widget)
{
    KStatusBar *sb = statusBar();

    int itemCount = d->statusBarItems.count();
    for (int i = itemCount-1; i >= 0; --i) {
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

KoPrintJob * KoView::createPrintJob()
{
    kWarning(30003) << "Printing not implemented in this application";
    return 0;
}

KoPrintJob * KoView::createPdfPrintJob()
{
    return createPrintJob();
}

KoPageLayout KoView::pageLayout() const
{
    return document()->pageLayout();
}

QPrintDialog *KoView::createPrintDialog(KoPrintJob *printJob, QWidget *parent)
{
    QPrintDialog *printDialog = KdePrint::createPrintDialog(&printJob->printer(),
                                printJob->createOptionWidgets(), parent);
    printDialog->setMinMax(printJob->printer().fromPage(), printJob->printer().toPage());
    printDialog->setEnabledOptions(printJob->printDialogOptions());
    return printDialog;
}


KoMainWindow * KoView::mainWindow() const
{
    return dynamic_cast<KoMainWindow *>(window());
}

KStatusBar * KoView::statusBar() const
{
    KoMainWindow *mw = mainWindow();
    return mw ? mw->statusBar() : 0;
}

void KoView::slotActionStatusText(const QString &text)
{
    KStatusBar *sb = statusBar();
    if (sb)
        sb->showMessage(text);
}

void KoView::slotClearStatusText()
{
    KStatusBar *sb = statusBar();
    if (sb)
        sb->clearMessage();
}

QList<QAction*> KoView::createChangeUnitActions(bool addPixelUnit)
{
    UnitActionGroup* unitActions = new UnitActionGroup(d->document, addPixelUnit, this);
    return unitActions->actions();
}

void KoView::guiActivateEvent(bool activated)
{
    Q_UNUSED(activated);
}


#include <KoView_p.moc>
#include <KoView.moc>
