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

// #include <QPushButton>
// #include <QVBoxLayout>

// #include <KLocalizedString>
// #include <ktextedit.h>

// Sheets
// #include "Cell.h"
// #include "ui/Selection.h"
// #include "Sheet.h"

// #include "commands/CommentCommand.h"

using namespace Calligra::Sheets;

CommentDialog::CommentDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent)
{
    setCaption(i18n("Cell Comment"));
    setModal(true);
    setButtons(Ok | Cancel);

    m_selection = selection;

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    multiLine = new KTextEdit(page);
    lay1->addWidget(multiLine);

    multiLine->setFocus();

    const QString comment = Cell(m_selection->activeSheet(), m_selection->marker()).comment();
    if (!comment.isEmpty())
        multiLine->setText(comment);

    connect(this, &KoDialog::okClicked, this, &CommentDialog::slotOk);
    connect(multiLine, &QTextEdit::textChanged, this, &CommentDialog::slotTextChanged);

    slotTextChanged();
    resize(400, height());
}

void CommentDialog::slotTextChanged()
{
    enableButtonOk(!multiLine->toPlainText().isEmpty());
}

void CommentDialog::slotOk()
{
    CommentCommand* command = new CommentCommand();
    command->setSheet(m_selection->activeSheet());
    command->setText(kundo2_i18n("Add Comment"));
    command->setComment(multiLine->toPlainText().trimmed());
    command->add(*m_selection);
    command->execute(m_selection->canvas());
    accept();
}
