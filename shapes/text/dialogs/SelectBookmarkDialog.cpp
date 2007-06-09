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

SelectBookmark::SelectBookmark(QList<QString> nameList, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.bookmarkList->addItems(nameList);
    connect( widget.bookmarkList, SIGNAL( currentRowChanged(int) ), this, SIGNAL( bookmarkSelectionChanged(int) ) );
}

QString SelectBookmark::bookmarkName()
{
    return widget.bookmarkList->currentItem()->text();
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
    // TODO close dialog if one item is double-clicked
    // TODO Rename and Delete
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

#include <SelectBookmarkDialog.moc>

