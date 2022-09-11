/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CommentDialog.h"

#include <QVBoxLayout>
#include <ktextedit.h>
#include <KLocalizedString>


using namespace Calligra::Sheets;

CommentDialog::CommentDialog(QWidget* parent)
        : KoDialog(parent)
{
    setCaption(i18n("Cell Comment"));
    setModal(true);
    setButtons(Ok | Cancel);

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    multiLine = new KTextEdit(page);
    lay1->addWidget(multiLine);

    multiLine->setFocus();

    connect(this, &KoDialog::okClicked, this, &CommentDialog::slotOk);
    connect(multiLine, &QTextEdit::textChanged, this, &CommentDialog::slotTextChanged);

    slotTextChanged();
    resize(400, height());
}

void CommentDialog::setComment(const QString &comment)
{
    multiLine->setText(comment);
}

QString CommentDialog::comment() const
{
    return multiLine->toPlainText().trimmed();
}

void CommentDialog::slotTextChanged()
{
    enableButtonOk(!multiLine->toPlainText().isEmpty());
}

void CommentDialog::slotOk()
{
    accept();
}

