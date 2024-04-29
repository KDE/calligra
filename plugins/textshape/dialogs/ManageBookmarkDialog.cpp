/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ManageBookmarkDialog.h"

#include <KMessageBox>
#include <QInputDialog>

static QString lastBookMarkItem;

ManageBookmark::ManageBookmark(const QList<QString> &nameList, KoTextEditor *editor, QWidget *parent)
    : QWidget(parent)
    , m_editor(editor)
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

    connect(widget.bookmarkList, &QListWidget::currentRowChanged, this, &ManageBookmark::selectionChanged);
    connect(widget.buttonRename, &QAbstractButton::clicked, this, &ManageBookmark::slotBookmarkRename);
    connect(widget.buttonDelete, &QAbstractButton::clicked, this, &ManageBookmark::slotBookmarkDelete);
    connect(widget.buttonInsert, &QAbstractButton::clicked, this, &ManageBookmark::slotBookmarkInsert);
    connect(widget.bookmarkList, &QListWidget::itemActivated, this, &ManageBookmark::slotBookmarkItemActivated);
    selectionChanged(bookmarkRow());
}

QString ManageBookmark::bookmarkName() const
{
    const QListWidgetItem *item = widget.bookmarkList->currentItem();
    return item ? item->text() : QString();
}

int ManageBookmark::bookmarkRow() const
{
    return widget.bookmarkList->currentRow();
}

void ManageBookmark::selectionChanged(int currentRow)
{
    widget.buttonRename->setEnabled(currentRow != -1);
    widget.buttonDelete->setEnabled(currentRow != -1);
    Q_EMIT bookmarkSelectionChanged(currentRow);
}

void ManageBookmark::slotBookmarkRename()
{
    bool ok = 0;
    QListWidgetItem *item = widget.bookmarkList->currentItem();
    Q_ASSERT(item);
    QString curName = item->text();
    QString newName = item->text();
    while (true) {
        newName =
            QInputDialog::getText(parentWidget(), i18n("Rename Bookmark"), i18n("Please provide a new name for the bookmark"), QLineEdit::Normal, newName, &ok);
        if (curName != newName && ok) {
            QList<QListWidgetItem *> items = widget.bookmarkList->findItems(newName, Qt::MatchExactly);
            if (items.count() > 0) {
                KMessageBox::error(parentWidget(), i18n("A bookmark with the name \"%1\" already exists.", newName));
                continue;
            }
            item->setText(newName);
            Q_EMIT bookmarkNameChanged(curName, newName);
        }
        break;
    }
}

void ManageBookmark::slotBookmarkDelete()
{
    int currentRow = widget.bookmarkList->currentRow();
    Q_ASSERT(currentRow >= 0);
    QListWidgetItem *deletedItem = widget.bookmarkList->takeItem(currentRow);
    QString deletedName = deletedItem->text();
    Q_EMIT bookmarkItemDeleted(deletedName);
    delete deletedItem;
}

void ManageBookmark::slotBookmarkItemActivated(QListWidgetItem *item)
{
    Q_ASSERT(item);
    lastBookMarkItem = item->text();
    Q_EMIT bookmarkItemDoubleClicked(item);
}

void ManageBookmark::slotBookmarkInsert()
{
    QString bookmarkName;
    bool ok = 0;
    while (true) {
        bookmarkName = QInputDialog::getText(parentWidget(),
                                             i18n("Insert Bookmark"),
                                             i18n("Please provide a name for the bookmark"),
                                             QLineEdit::Normal,
                                             bookmarkName,
                                             &ok);
        if (ok) {
            QList<QListWidgetItem *> items = widget.bookmarkList->findItems(bookmarkName, Qt::MatchExactly);
            if (items.count() > 0) {
                KMessageBox::error(parentWidget(), i18n("A bookmark with the name \"%1\" already exists.", bookmarkName));
                continue;
            } else {
                m_editor->addBookmark(bookmarkName);
                widget.bookmarkList->insertItem(widget.bookmarkList->count(), bookmarkName);
            }
        }
        break;
    }
}

ManageBookmarkDialog::ManageBookmarkDialog(const QList<QString> &nameList, KoTextEditor *editor, QWidget *parent)
    : KoDialog(parent)
{
    ui = new ManageBookmark(nameList, editor, this);
    setMainWidget(ui);
    setCaption(i18n("Manage Bookmarks"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    showButtonSeparator(true);
    connect(ui, &ManageBookmark::bookmarkSelectionChanged, this, &ManageBookmarkDialog::selectionChanged);
    connect(ui, &ManageBookmark::bookmarkNameChanged, this, &ManageBookmarkDialog::nameChanged);
    connect(ui, &ManageBookmark::bookmarkItemDeleted, this, &ManageBookmarkDialog::bookmarkDeleted);
    connect(ui, &ManageBookmark::bookmarkItemDoubleClicked, this, &ManageBookmarkDialog::bookmarkDoubleClicked);
    selectionChanged(ui->bookmarkRow());
}

QString ManageBookmarkDialog::selectedBookmarkName()
{
    return ui->bookmarkName();
}

void ManageBookmarkDialog::selectionChanged(int currentRow)
{
    enableButtonOk(currentRow != -1);
}

void ManageBookmarkDialog::bookmarkDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    accept();
}
