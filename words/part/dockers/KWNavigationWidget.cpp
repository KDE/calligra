/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWNavigationWidget.h"

#include "KWDocument.h"
#include "Words.h"
#include "frames/KWFrame.h"
#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include <KWView.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>
#include <KoTextLayoutRootArea.h>

#include <QHeaderView>
#include <QStack>
#include <QTextBlock>
#include <QTextDocument>

KWNavigationWidget::KWNavigationWidget(QWidget *parent)
    : QWidget(parent)
    , m_document(nullptr)
    , m_canvas(nullptr)
    , m_layout(nullptr)
    , m_updateTimer(new QTimer(this))
{
    m_model = new QStandardItemModel(this);

    m_updateTimer->setSingleShot(true);

    initUi();
    initLayout();
}

KWNavigationWidget::~KWNavigationWidget() = default;

void KWNavigationWidget::initUi()
{
    m_treeView = new QTreeView;
    m_treeView->setModel(m_model);
    m_treeView->setItemsExpandable(false);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setSelectionMode(QAbstractItemView::NoSelection);

    connect(m_treeView, &QAbstractItemView::clicked, this, &KWNavigationWidget::navigationClicked);
}

void KWNavigationWidget::initLayout()
{
    QHBoxLayout *mainBox = new QHBoxLayout(this);
    mainBox->addWidget(m_treeView);

    m_treeView->header()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setLayout(mainBox); // FIXME: Is this necessary?
}

void KWNavigationWidget::navigationClicked(const QModelIndex &idx)
{
    if (idx.column() == 0) {
        QTextDocument *doc = static_cast<QTextDocument *>(m_model->itemFromIndex(idx)->data(Qt::UserRole + 2).value<void *>());

        int position = m_model->itemFromIndex(idx)->data(Qt::UserRole + 1).toInt();

        KoTextDocument(doc).textEditor()->setPosition(position); // placing cursor
        m_canvas->view()->setFocus(); // passing focus

        KoTextLayoutRootArea *a = m_layout->rootAreaForPosition(position);
        m_canvas->view()->goToPage(*(static_cast<KWPage *>(a->page()))); // showing needed page
    }
}

void KWNavigationWidget::updateData()
{
    if (!isVisible()) {
        return;
    }

    // don't refresh too often
    if (m_updateTimer->isActive()) {
        return;
    }

    m_model->clear();

    QStringList head;
    head << i18n("Section") << i18n("Page #");
    m_model->setHorizontalHeaderLabels(head);
    m_model->setColumnCount(2);

    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_treeView->header()->setStretchLastSection(false);

    QStack<QPair<QStandardItem *, int>> curChain;
    curChain.push(QPair<QStandardItem *, int>(m_model->invisibleRootItem(), 0));

    foreach (KWFrameSet *fs, m_document->frameSets()) {
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet *>(fs);
        if (tfs == nullptr)
            continue;

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
            item->setData(QVariant::fromValue((void *)doc), Qt::UserRole + 2);
            QList<QStandardItem *> buf;

            KoTextLayoutRootArea *a = m_layout->rootAreaForPosition(block.position());

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
    m_treeView->resizeColumnToContents(1);

    m_updateTimer->start(300);
}

void KWNavigationWidget::setCanvas(KWCanvas *canvas)
{
    if (!canvas)
        return;
    m_document = canvas->document();
    if (m_layout) {
        disconnect(m_layout, &KoTextDocumentLayout::finishedLayout, this, &KWNavigationWidget::updateData);
    }
    if (m_document->mainFrameSet()) {
        m_layout = qobject_cast<KoTextDocumentLayout *>(m_document->mainFrameSet()->document()->documentLayout());
        connect(m_layout, &KoTextDocumentLayout::finishedLayout, this, &KWNavigationWidget::updateData);
    } else {
        m_layout = nullptr;
    }
    m_canvas = canvas;
}

void KWNavigationWidget::unsetCanvas()
{
    m_document = nullptr;
    if (m_layout) {
        disconnect(m_layout, &KoTextDocumentLayout::finishedLayout, this, &KWNavigationWidget::updateData);
    }
    m_layout = nullptr;
}
