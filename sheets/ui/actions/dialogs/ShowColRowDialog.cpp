/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ShowColRowDialog.h"

// Qt
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

// KF5
#include <KLocalizedString>

using namespace Calligra::Sheets;

ShowColRowDialog::ShowColRowDialog(QWidget *parent, bool rows)
    : KoDialog(parent)
{
    setModal(true);
    setButtons(Ok | Cancel);

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    QLabel *label = new QLabel(page);

    if (rows) {
        setWindowTitle(i18n("Show Rows"));
        label->setText(i18n("Select hidden rows to show:"));
    } else {
        setWindowTitle(i18n("Show Columns"));
        label->setText(i18n("Select hidden columns to show:"));
    }

    list = new QListWidget(page);

    lay1->addWidget(label);
    lay1->addWidget(list);

    // selection multiple
    list->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(this, &KoDialog::okClicked, this, &ShowColRowDialog::slotOk);
    connect(list, &QListWidget::itemDoubleClicked, this, &ShowColRowDialog::slotDoubleClicked);
    resize(200, 150);
    setFocus();
}

void ShowColRowDialog::setList(const QVector<QString> &captions)
{
    list->clear();
    for (QString item : captions)
        list->addItem(item);
    if (list->count())
        enableButtonOk(true);
    else
        enableButtonOk(false);
}

QVector<int> ShowColRowDialog::selectedIndexes() const
{
    QVector<int> res;
    for (unsigned int i = 0; i < (unsigned int)list->count(); i++)
        if (list->item(i)->isSelected())
            res.append(i);
    return res;
}

void ShowColRowDialog::slotDoubleClicked(QListWidgetItem *)
{
    slotOk();
}

void ShowColRowDialog::slotOk()
{
    accept();
}
