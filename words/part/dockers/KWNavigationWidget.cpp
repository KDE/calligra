/* This file is part of the KDE project
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
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

#include "KWNavigationWidget.h"

#include "Words.h"
#include "KWDocument.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include <KWView.h>
#include <KoCanvasResourceManager.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoShapeController.h>
#include <KoIcon.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>

#include <QStack>
#include <QTextLayout>
#include <QTextDocument>
#include <QTextBlock>
#include <QTimer>

KWNavigationWidget::KWNavigationWidget(QWidget *parent)
    : QWidget(parent)
    , m_document(0)
    , m_canvas(0)
{
    m_model = new QStandardItemModel(this);
    m_timer = new QTimer(this);
    initUi();
    initLayout();

    //use to refresh navigator
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateData())); // FIXME: better idea ?

    //TODO: some configuration??
}

KWNavigationWidget::~KWNavigationWidget()
{
    m_timer->stop();
}

void KWNavigationWidget::initUi()
{
    m_treeView = new QTreeView;
    m_treeView->setModel(m_model);
    m_treeView->setItemsExpandable(false);
    connect(m_treeView, SIGNAL(clicked(QModelIndex)), this, SLOT(headerClicked(QModelIndex)));
}

void KWNavigationWidget::initLayout()
{
    QHBoxLayout *mainBox = new QHBoxLayout(this);
    mainBox->addWidget(m_treeView);

    setLayout(mainBox); // FIXME: Is this necessary?
}

void KWNavigationWidget::headerClicked(QModelIndex idx)
{
    QTextDocument *doc = static_cast<QTextDocument *>(
        m_model->itemFromIndex(idx)->data(Qt::UserRole + 2).value<void *>());

    int position = m_model->itemFromIndex(idx)->data(Qt::UserRole + 1).toInt();

    KoTextDocument(doc).textEditor()->setPosition(position); // placing cursor
    m_canvas->view()->setFocus(); // passing focus

    KoTextDocumentLayout *l = qobject_cast<KoTextDocumentLayout *>(
        m_document->mainFrameSet()->document()->documentLayout());
    KoTextLayoutRootArea *a = l->rootAreaForPosition(position);
    m_canvas->view()->goToPage(*(static_cast<KWPage *>(a->page()))); // showing needed page
}

void KWNavigationWidget::updateData()
{
    if (!isVisible()) {
        return;
    }

    m_model->clear();
    m_model->setColumnCount(2);

    QStringList head;
    head << "Header" << "Page number";
    m_model->setHorizontalHeaderLabels(head);

    QStack< QPair<QStandardItem *, int> > curChain;
    curChain.push(QPair<QStandardItem *, int>(m_model->invisibleRootItem(), 0));

    foreach (KWFrameSet *fs, m_document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*>(fs);
        if (tfs == 0) continue;

        tfs->wordsDocument();
        QTextDocument *doc = tfs->document();
        QTextBlock block = doc->begin();
        while (block.isValid()) {
            int blockLevel = block.blockFormat().intProperty(KoParagraphStyle::OutlineLevel);

            if (!blockLevel) {
                block = block.next();
                continue;
            }

            QStandardItem *item = new QStandardItem(block.text());
            item->setData(block.position(), Qt::UserRole + 1);
            item->setData(qVariantFromValue((void *)doc), Qt::UserRole + 2);
            QList< QStandardItem *> buf;

            KoTextDocumentLayout *l = qobject_cast<KoTextDocumentLayout *>(
                m_document->mainFrameSet()->document()->documentLayout());
            KoTextLayoutRootArea *a = l->rootAreaForPosition(block.position());

            buf.append(item);
            buf.append(new QStandardItem(QString::number(a->page()->visiblePageNumber())));

            while (curChain.top().second >= blockLevel) {
                curChain.pop();
            }

            curChain.top().first->appendRow(buf);
            curChain.push(QPair<QStandardItem *, int>(item, blockLevel));

            block = block.next();
        }
    }
    m_treeView->expandAll();
    m_treeView->resizeColumnToContents(0);
}

void KWNavigationWidget::setCanvas(KWCanvas* canvas)
{
    m_document = canvas->document();
    m_canvas = canvas;
    m_timer->start(2500);
}

void KWNavigationWidget::unsetCanvas()
{
    m_timer->stop();
    m_document = 0;
}
