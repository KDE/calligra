/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWSelectBookmarkDialog.h"

#include <KMessageBox>

#include <QInputDialog>

static QString lastBookMarkItem;

KWSelectBookmark::KWSelectBookmark(const QStringList &nameList, QWidget *parent)
    : QWidget(parent)
    , parentWidget(parent)
{
    widget.setupUi(this);
    widget.bookmarkList->addItems(nameList);
    widget.bookmarkList->setFocus(Qt::ActiveWindowFocusReason);

    const int count = widget.bookmarkList->count();
    if (count > 0) {
        int row = 0;
        if (!lastBookMarkItem.isNull()) {
            QList<QListWidgetItem *> items = widget.bookmarkList->findItems(lastBookMarkItem, Qt::MatchExactly);
            if (items.count() > 0)
                row = widget.bookmarkList->row(items[0]);
        }
        widget.bookmarkList->setCurrentRow(row);
    }

    connect(widget.bookmarkList, &QListWidget::currentRowChanged, this, &KWSelectBookmark::selectionChanged);
    connect(widget.buttonRename, &QAbstractButton::clicked, this, &KWSelectBookmark::slotBookmarkRename);
    connect(widget.buttonDelete, &QAbstractButton::clicked, this, &KWSelectBookmark::slotBookmarkDelete);
    connect(widget.bookmarkList, &QListWidget::itemActivated, this, &KWSelectBookmark::slotBookmarkItemActivated);
    selectionChanged(bookmarkRow());
}

QString KWSelectBookmark::bookmarkName() const
{
    const QListWidgetItem *item = widget.bookmarkList->currentItem();
    return item ? item->text() : QString();
}

int KWSelectBookmark::bookmarkRow() const
{
    return widget.bookmarkList->currentRow();
}

void KWSelectBookmark::selectionChanged(int currentRow)
{
    widget.buttonRename->setEnabled(currentRow != -1);
    widget.buttonDelete->setEnabled(currentRow != -1);
    Q_EMIT bookmarkSelectionChanged(currentRow);
}

void KWSelectBookmark::slotBookmarkRename()
{
    QListWidgetItem *item = widget.bookmarkList->currentItem();
    Q_ASSERT(item);
    QString curName = item->text();
    QString newName = item->text();
    while (true) {
        newName = QInputDialog::getText(parentWidget, i18n("Rename Bookmark"), i18n("Please provide a new name for the bookmark"), QLineEdit::Normal, newName);
        if (curName != newName && !newName.isNull()) {
            if (newName.isEmpty())
                continue;
            QList<QListWidgetItem *> items = widget.bookmarkList->findItems(newName, Qt::MatchExactly);
            if (items.count() > 0) {
                KMessageBox::error(parentWidget, i18n("A bookmark with the name \"%1\" already exists.", newName));
                continue;
            }
            item->setText(newName);
            Q_EMIT bookmarkNameChanged(curName, newName);
        }
        break;
    }
}

void KWSelectBookmark::slotBookmarkDelete()
{
    int currentRow = widget.bookmarkList->currentRow();
    Q_ASSERT(currentRow >= 0);
    QListWidgetItem *deletedItem = widget.bookmarkList->takeItem(currentRow);
    QString deletedName = deletedItem->text();
    Q_EMIT bookmarkItemDeleted(deletedName);
    delete deletedItem;
}

void KWSelectBookmark::slotBookmarkItemActivated(QListWidgetItem *item)
{
    Q_ASSERT(item);
    lastBookMarkItem = item->text();
    Q_EMIT bookmarkItemDoubleClicked(item);
}

KWSelectBookmarkDialog::KWSelectBookmarkDialog(const QStringList &nameList, QWidget *parent)
    : KoDialog(parent)
{
    ui = new KWSelectBookmark(nameList, this);
    setMainWidget(ui);
    setCaption(i18n("Select Bookmark"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    showButtonSeparator(true);
    connect(ui, &KWSelectBookmark::bookmarkSelectionChanged, this, &KWSelectBookmarkDialog::selectionChanged);
    connect(ui, &KWSelectBookmark::bookmarkNameChanged, this, &KWSelectBookmarkDialog::nameChanged);
    connect(ui, &KWSelectBookmark::bookmarkItemDeleted, this, &KWSelectBookmarkDialog::bookmarkDeleted);
    connect(ui, &KWSelectBookmark::bookmarkItemDoubleClicked, this, &KWSelectBookmarkDialog::bookmarkDoubleClicked);
    selectionChanged(ui->bookmarkRow());
}

QString KWSelectBookmarkDialog::selectedBookmarkName()
{
    return ui->bookmarkName();
}

void KWSelectBookmarkDialog::selectionChanged(int currentRow)
{
    enableButtonOk(currentRow != -1);
}

void KWSelectBookmarkDialog::bookmarkDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    accept();
}
