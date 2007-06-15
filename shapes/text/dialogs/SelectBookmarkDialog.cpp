/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
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

#include "SelectBookmarkDialog.h"

#include <kinputdialog.h>

SelectBookmark::SelectBookmark(QList<QString> nameList, QWidget *parent)
    : QWidget(parent),
    parentWidget(parent)
{
    widget.setupUi(this);
    widget.bookmarkList->addItems(nameList);
    connect( widget.bookmarkList, SIGNAL( currentRowChanged(int) ), this, SIGNAL( bookmarkSelectionChanged(int) ) );
    connect( widget.buttonRename, SIGNAL( clicked() ), this, SLOT( slotBookmarkRename() ) );
    connect( widget.buttonDelete, SIGNAL( clicked() ), this, SLOT( slotBookmarkDelete() ) );
    connect( widget.bookmarkList, SIGNAL( itemDoubleClicked(QListWidgetItem *) ),
            this, SIGNAL( bookmarkItemDoubleClicked(QListWidgetItem *) ) );
}

QString SelectBookmark::bookmarkName()
{
    return widget.bookmarkList->currentItem()->text();
}

void SelectBookmark::slotBookmarkRename()
{
    QString curName = widget.bookmarkList->currentItem()->text();

    QString newName = KInputDialog::getText( "Rename Bookmark",
                                             "Please provide a new name for the bookmark",
                                             widget.bookmarkList->currentItem()->text(),
                                             0,
                                             parentWidget);
    if (!newName.isNull()) {
        if (newName != curName) {
            widget.bookmarkList->currentItem()->setText( newName );
            emit bookmarkNameChanged( curName, newName );
        }
    }
}

void SelectBookmark::slotBookmarkDelete()
{
    int currentRow = widget.bookmarkList->currentRow();
    QListWidgetItem *deletedItem = widget.bookmarkList->takeItem(currentRow);
    QString deletedName = deletedItem->text();
    emit bookmarkItemDeleted( deletedName );
    delete deletedItem;
}

SelectBookmarkDialog::SelectBookmarkDialog(QList<QString> nameList, QWidget *parent)
    : KDialog(parent)
{
    ui = new SelectBookmark( nameList, this );
    setMainWidget( ui );
    setCaption(i18n("Select Bookmark") );
    setModal( true );
    setButtons( Ok|Cancel );
    setDefaultButton( Ok );
    enableButtonOk( false );
    showButtonSeparator( true );
    connect( ui, SIGNAL( bookmarkSelectionChanged(int) ), this, SLOT( selectionChanged(int) ) );
    connect( ui, SIGNAL( bookmarkNameChanged(const QString &, const QString &) ),
            this, SIGNAL( nameChanged(const QString &, const QString &) ) );
    connect( ui, SIGNAL( bookmarkItemDeleted(const QString &) ),
            this, SIGNAL( bookmarkDeleted(const QString &)) );
    connect( ui, SIGNAL( bookmarkItemDoubleClicked(QListWidgetItem *)),
            this, SLOT( bookmarkDoubleClicked(QListWidgetItem *) ) );
}

QString SelectBookmarkDialog::selectedBookmarkName()
{
    return ui->bookmarkName();
}

void SelectBookmarkDialog::selectionChanged(int currentRow)
{
    if (currentRow != -1)
        enableButtonOk( true );
    else
        enableButtonOk( false );
}

void SelectBookmarkDialog::bookmarkDoubleClicked(QListWidgetItem *item) {
    Q_UNUSED(item);
    accept();
}

#include <SelectBookmarkDialog.moc>

