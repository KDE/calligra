/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWCreateBookmarkDialog.h"

#include <KLocalizedString>

KWCreateBookmark::KWCreateBookmark(const QStringList &nameList, const QString &suggestedName, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.bookmarkName->setCompleter(nullptr);
    widget.bookmarkName->insertItems(0, nameList);
    widget.bookmarkName->setEditText(suggestedName);
    connect(widget.bookmarkName, &QComboBox::editTextChanged, this, &KWCreateBookmark::bookmarkNameChanged);
}

QString KWCreateBookmark::bookmarkName()
{
    return widget.bookmarkName->currentText();
}

KWCreateBookmarkDialog::KWCreateBookmarkDialog(const QStringList &nameList, const QString &suggestedName, QWidget *parent)
    : KoDialog(parent)
    , m_nameList(nameList)
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
    connect(ui, &KWCreateBookmark::bookmarkNameChanged, this, &KWCreateBookmarkDialog::nameChanged);
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
