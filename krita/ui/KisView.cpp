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

#include "KisView.h"

// local directory
#include "KisView_p.h"

#include "KisPart.h"
#include "KoDockRegistry.h"
#include "KisDocument.h"
#include "KisMainWindow.h"

#include "KoDockFactoryBase.h"
#include "KisUndoStackAction.h"
#include "KoPageLayout.h"
#include "KisPrintJob.h"
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
QString KisView::newObjectName()
{
    static int s_viewIFNumber = 0;
    QString name; name.setNum(s_viewIFNumber++); name.prepend("view_");
    return name;
}


class KisViewPrivate
{
public:
    KisViewPrivate()
        : undo(0)
        , redo(0)
    {
        tempActiveWidget = 0;
        documentDeleted = false;
    }

    ~KisViewPrivate() {
    }

    QPointer<KisDocument> document; // our KisDocument
    QPointer<KisPart> part; // our part
    QWidget *tempActiveWidget;
    bool documentDeleted; // true when document gets deleted [can't use document==0
    // since this only happens in ~QObject, and views
    // get deleted by ~KisDocument].


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

    KisUndoStackAction *undo;
    KisUndoStackAction *redo;

    QList<StatusBarItem> statusBarItems; // Our statusbar items
    bool inOperation; //in the middle of an operation (no screen refreshing)?
};

KisView::KisView(KisPart *part, KisDocument *document, QWidget *parent)
        : QWidget(parent)
        , d(new KisViewPrivate)
{
    Q_ASSERT(document);
    Q_ASSERT(part);

    setObjectName(newObjectName());

    d->document = document;
    d->part = part;

    setFocusPolicy(Qt::StrongFocus);

    d->undo = new KisUndoStackAction(d->document->undoStack(), KisUndoStackAction::UNDO);
    d->redo = new KisUndoStackAction(d->document->undoStack(), KisUndoStackAction::RED0);


    KStatusBar * sb = statusBar();
    if (sb) { // No statusbar in e.g. konqueror
        connect(d->document, SIGNAL(statusBarMessage(const QString&)),
                this, SLOT(slotActionStatusText(const QString&)));
        connect(d->document, SIGNAL(clearStatusBarMessage()),
                this, SLOT(slotClearStatusText()));
    }

}

KisView::~KisView()
{
    d->part->removeView(this);
    delete d;
}

KAction *KisView::undoAction() const
{
    return d->undo;
}

KAction *KisView::redoAction() const
{
    return d->redo;
}

void KisView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasImage()
            || event->mimeData()->hasUrls()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void KisView::dropEvent(QDropEvent *event)
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


void KisView::addImages(const QList<QImage> &, const QPoint &)
{
    // override in your application
}

KisDocument *KisView::document() const
{
    return d->document;
}

void KisView::setDocument(KisDocument *document)
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

void KisView::setDocumentDeleted()
{
    d->documentDeleted = true;
}

void KisView::addStatusBarItem(QWidget * widget, int stretch, bool permanent)
{
    KisViewPrivate::StatusBarItem item(widget, stretch, permanent);
    KStatusBar * sb = statusBar();
    if (sb) {
        item.ensureItemShown(sb);
    }
    d->statusBarItems.append(item);
}

void KisView::removeStatusBarItem(QWidget *widget)
{
    KStatusBar *sb = statusBar();

    int itemCount = d->statusBarItems.count();
    for (int i = itemCount-1; i >= 0; --i) {
        KisViewPrivate::StatusBarItem &sbItem = d->statusBarItems[i];
        if (sbItem.widget() == widget) {
            if (sb) {
                sbItem.ensureItemHidden(sb);
            }
            d->statusBarItems.removeOne(sbItem);
            break;
        }
    }
}


KisPrintJob * KisView::createPdfPrintJob()
{
    return createPrintJob();
}

KoPageLayout KisView::pageLayout() const
{
    return document()->pageLayout();
}

QPrintDialog *KisView::createPrintDialog(KisPrintJob *printJob, QWidget *parent)
{
    QPrintDialog *printDialog = KdePrint::createPrintDialog(&printJob->printer(),
                                printJob->createOptionWidgets(), parent);
    printDialog->setMinMax(printJob->printer().fromPage(), printJob->printer().toPage());
    printDialog->setEnabledOptions(printJob->printDialogOptions());
    return printDialog;
}


KisMainWindow * KisView::mainWindow() const
{
    return dynamic_cast<KisMainWindow *>(window());
}

KStatusBar * KisView::statusBar() const
{
    KisMainWindow *mw = mainWindow();
    return mw ? mw->statusBar() : 0;
}

void KisView::slotActionStatusText(const QString &text)
{
    KStatusBar *sb = statusBar();
    if (sb)
        sb->showMessage(text);
}

void KisView::slotClearStatusText()
{
    KStatusBar *sb = statusBar();
    if (sb)
        sb->clearMessage();
}

QList<QAction*> KisView::createChangeUnitActions(bool addPixelUnit)
{
    UnitActionGroup* unitActions = new UnitActionGroup(d->document, addPixelUnit, this);
    return unitActions->actions();
}

void KisView::guiActivateEvent(bool activated)
{
    Q_UNUSED(activated);
}


#include <KisView_p.moc>
#include <KisView.moc>
