/* This file is part of the KDE project
 * Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
 * Copyright (C) 2011 Boudewijn Rempt <boud@valdyas.org>
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

#include "KWRdfDockerTree.h"

#include <WordsDebug.h>

#include <QMimeDatabase>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>

#include <KoDocumentRdf.h>
#include <KoRdfSemanticItemRegistry.h>

#include "KWCanvas.h"

#include "KoRdfSemanticTreeWidgetItem.h"
#include <KoTextEditor.h>
#include <KoToolProxy.h>

KWRdfDockerTree::KWRdfDockerTree(QWidget *parent)
        : QTreeWidget(parent),
        m_rdf(0),
        m_canvas(0)
{
    //kDebug(30015) << "KWRdfDockerTree()";
    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true);
}

void KWRdfDockerTree::setDocumentRdf(KoDocumentRdf *rdf)
{
    m_rdf = rdf;
}

void KWRdfDockerTree::setCanvas(KoCanvasBase *canvas)
{
    if (m_canvas) {
        m_canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }
    m_canvas = canvas;
}

void KWRdfDockerTree::dragEnterEvent(QDragEnterEvent *event)
{
    //kDebug(30015) << "dragEnterEvent() mime format:" << event->mimeData()->formats();
    if (KoRdfSemanticItemRegistry::instance()->canCreateSemanticItemFromMimeData(event->mimeData()) ||
        event->mimeData()->hasFormat("text/uri-list")) {
        event->accept();
    }
}

void KWRdfDockerTree::dragMoveEvent(QDragMoveEvent *e)
{
    //kDebug(30015) << "accepting...";
    e->accept();
}

bool KWRdfDockerTree::dropMimeData(QTreeWidgetItem *parent, int index,
        const QMimeData *data, Qt::DropAction action)
{
    Q_UNUSED(parent);
    Q_UNUSED(index);
    Q_UNUSED(action);
    //kDebug(30015) << "KWRdfDockerTree::dropMimeData() mime format:" << data->formats();

    if (data->hasFormat("text/directory")) {
        QByteArray ba = data->data("text/directory");
        //kDebug(30015) << "text/directory:" << ba;
    } else if (data->hasFormat("text/uri-list")) {
        QByteArray urilist = data->data("text/uri-list");
        //kDebug(30015) << "uri-list:" << urilist;
        QTextStream ss(&urilist, QIODevice::ReadOnly);
        while (!ss.atEnd()) {
            QString fileName = ss.readLine();
            //kDebug(30015) << "fileName:" << fileName;
            if (fileName.startsWith(QLatin1String("file:"))) {
                fileName.remove(0, 5);
            }
            QFile f(fileName);
            f.open(QIODevice::ReadOnly);
            const QByteArray ba = f.readAll();
            // Try to work out what ba contains....
            // TODO: rather pass QFile here instead of loading ourselves
            const QString mt =  QMimeDatabase().mimeTypeForData(ba).name();
            QMimeData linkedData;
            linkedData.setData(mt, ba);
            //kDebug(30015) << "MIME Type:" << mt;
            KoRdfSemanticItemRegistry::instance()->createSemanticItemFromMimeData(&linkedData, m_canvas, m_rdf, m_rdf);
        }
    } else {
        KoRdfSemanticItemRegistry::instance()->createSemanticItemFromMimeData(data, m_canvas, m_rdf, m_rdf);
    }
    return true;
}

void KWRdfDockerTree::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if (!currentItem())
        return;
    QTreeWidgetItem *baseitem = currentItem();
    if (KoRdfSemanticTreeWidgetItem *item = dynamic_cast<KoRdfSemanticTreeWidgetItem*>(baseitem)) {
        QMimeData *mimeData = new QMimeData;
        item->semanticItem()->exportToMime(mimeData);
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->start(Qt::CopyAction);
    }
}

QStringList KWRdfDockerTree::mimeTypes() const
{
    //kDebug(30015) << "default mt:" << QTreeWidget::mimeTypes();
    return QStringList();
}

QMimeData *KWRdfDockerTree::mimeData(const QList<QTreeWidgetItem *> items) const
{
    Q_UNUSED(items)
    //kDebug(30015) << "getting mime data, itemlist.sz:" << items.size();
    QByteArray ba;
    QMimeData *md = new QMimeData;
    md->setData("foo/bar", ba);
    return md;
}

