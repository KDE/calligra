/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "CommentDialog.h"

#include <QPushButton>
#include <QVBoxLayout>

#include <klocale.h>
#include <KTextEdit>

//KSpread includes
#include "Cell.h"
#include "ui/Selection.h"
#include "Sheet.h"

#include "commands/CommentCommand.h"

using namespace KSpread;

CommentDialog::CommentDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
{
    setCaption(i18n("Cell Comment"));
    setModal(true);
    setButtons(Ok | Cancel);

    m_selection = selection;

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);
    lay1->setMargin(KDialog::marginHint());
    lay1->setSpacing(KDialog::spacingHint());

    multiLine = new KTextEdit(page);
    lay1->addWidget(multiLine);

    multiLine->setFocus();

    const QString comment = Cell(m_selection->activeSheet(), m_selection->marker()).comment();
    if (!comment.isEmpty())
        multiLine->setText(comment);

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(multiLine, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));

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
    command->setText(i18n("Add Comment"));
    command->setComment(multiLine->toPlainText().trimmed());
    command->add(*m_selection);
    command->execute(m_selection->canvas());
    accept();
}

#include "CommentDialog.moc"
