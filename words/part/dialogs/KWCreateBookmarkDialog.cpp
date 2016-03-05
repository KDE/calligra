/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
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

#include "KWCreateBookmarkDialog.h"

#include <klocalizedstring.h>

KWCreateBookmark::KWCreateBookmark(const QStringList &nameList, const QString &suggestedName, QWidget *parent)
        : QWidget(parent)
{
    widget.setupUi(this);
    widget.bookmarkName->setCompleter(0);
    widget.bookmarkName->insertItems(0, nameList);
    widget.bookmarkName->setEditText(suggestedName);
    connect(widget.bookmarkName, SIGNAL(editTextChanged(QString)), this, SIGNAL(bookmarkNameChanged(QString)));
}

QString KWCreateBookmark::bookmarkName()
{
    return widget.bookmarkName->currentText();
}

KWCreateBookmarkDialog::KWCreateBookmarkDialog(const QStringList &nameList, const QString &suggestedName, QWidget *parent)
        : KoDialog(parent),
        m_nameList(nameList)
{
    if (suggestedName.isEmpty() || m_nameList.contains(suggestedName)) {
        enableButtonOk(false);
        ui = new KWCreateBookmark(nameList, "", this);
    } else {
        ui = new KWCreateBookmark(nameList, suggestedName, this);
    }
    setMainWidget(ui);
    setCaption(i18n("Create New Bookmark"));
    setModal(true);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);
    showButtonSeparator(true);
    connect(ui, SIGNAL(bookmarkNameChanged(QString)), this, SLOT(nameChanged(QString)));
}

QString KWCreateBookmarkDialog::newBookmarkName()
{
    return ui->bookmarkName();
}

void KWCreateBookmarkDialog::nameChanged(const QString &bookmarkName)
{
    if (!m_nameList.contains(bookmarkName))
        enableButtonOk(!bookmarkName.isEmpty());
    else
        enableButtonOk(false);
}
