/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2001-2003 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ListDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include <KLocalizedString>
#include <KMessageBox>
#include <ktextedit.h>

#include "engine/Localization.h"

using namespace Calligra::Sheets;

class ListDialog::Private
{
public:
    QListWidget *list;
    KTextEdit *textEdit;
    QPushButton *addButton;
    QPushButton *cancelButton;
    QPushButton *removeButton;
    QPushButton *newButton;
    QPushButton *modifyButton;
    QPushButton *copyButton;
    bool changed;
};

static const int numBuiltinLists = 4;

ListDialog::ListDialog(QWidget *parent)
    : ActionDialog(parent)
    , d(new Private)
{
    setCaption(i18n("Custom Lists"));
    setButtonText(Apply, i18n("Save Changes"));

    QWidget *page = new QWidget(this);
    setMainWidget(page);

    QGridLayout *grid1 = new QGridLayout(page);

    QLabel *lab = new QLabel(page);
    lab->setText(i18n("List:"));
    grid1->addWidget(lab, 0, 0);

    d->list = new QListWidget(page);
    grid1->addWidget(d->list, 1, 0, 7, 1);

    lab = new QLabel(page);
    lab->setText(i18n("Entry:"));
    grid1->addWidget(lab, 0, 1);

    d->textEdit = new KTextEdit(page);
    grid1->addWidget(d->textEdit, 1, 1, 7, 1);

    d->addButton = new QPushButton(i18n("Add"), page);
    d->addButton->setEnabled(false);
    grid1->addWidget(d->addButton, 1, 2);

    d->cancelButton = new QPushButton(i18n("Cancel"), page);
    d->cancelButton->setEnabled(false);
    grid1->addWidget(d->cancelButton, 2, 2);

    d->newButton = new QPushButton(i18n("New"), page);
    grid1->addWidget(d->newButton, 3, 2);

    d->removeButton = new QPushButton(i18n("Remove"), page);
    grid1->addWidget(d->removeButton, 4, 2);

    d->modifyButton = new QPushButton(i18n("Modify"), page);
    grid1->addWidget(d->modifyButton, 5, 2);

    d->copyButton = new QPushButton(i18n("Copy"), page);
    grid1->addWidget(d->copyButton, 6, 2);

    connect(d->addButton, &QAbstractButton::clicked, this, &ListDialog::slotAdd);
    connect(d->cancelButton, &QAbstractButton::clicked, this, &ListDialog::slotCancel);
    connect(d->newButton, &QAbstractButton::clicked, this, &ListDialog::slotNew);
    connect(d->removeButton, &QAbstractButton::clicked, this, &ListDialog::slotRemove);
    connect(d->modifyButton, &QAbstractButton::clicked, this, &ListDialog::slotModify);
    connect(d->copyButton, &QAbstractButton::clicked, this, &ListDialog::slotCopy);
    connect(d->list, &QListWidget::itemDoubleClicked, this, &ListDialog::slotDoubleClicked);
    connect(d->list, &QListWidget::currentRowChanged, this, &ListDialog::slotCurrentRowChanged);

    d->textEdit->setEnabled(false);
    d->modifyButton->setEnabled(false);
    slotCurrentRowChanged(0);
    resize(600, 250);
    d->changed = false;
}

ListDialog::~ListDialog()
{
    delete d;
}

void ListDialog::slotCurrentRowChanged(int row)
{
    // we can't remove the first built-in items
    const bool state = row >= numBuiltinLists;
    d->removeButton->setEnabled(state);
    d->copyButton->setEnabled(row >= 0);
    d->textEdit->setEnabled(false);
    d->textEdit->clear();
}

void ListDialog::setCustomLists(const QStringList &list, Localization *locale)
{
    QStringList lst;

    QString e;
    for (int month = 1; month <= 12; ++month) {
        e += locale->monthName(month);
        if (month < 12)
            e += ", ";
    }
    lst << e;
    e = QString();
    for (int month = 1; month <= 12; ++month) {
        e += locale->monthName(month, false);
        if (month < 12)
            e += ", ";
    }
    lst << e;

    e = QString();
    for (int day = 1; day <= 7; ++day) {
        e += locale->dayName(day);
        if (day < 7)
            e += ", ";
    }
    lst << e;
    e = QString();
    for (int day = 1; day <= 7; ++day) {
        e += locale->dayName(day, false);
        if (day < 7)
            e += ", ";
    }
    lst << e;

    QString tmp;
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it) {
        if ((*it) != "\\") {
            tmp += (*it) + ", ";
        } else if (it != list.begin()) {
            tmp = tmp.left(tmp.length() - 2);
            lst.append(tmp);
            tmp.clear();
        }
    }
    d->list->addItems(lst);
}

QStringList ListDialog::customLists()
{
    QStringList result;
    result.append("\\");

    // don't save the first built-in lines
    for (int i = numBuiltinLists - 1; i < d->list->count(); ++i) {
        QStringList tmp = d->list->item(i)->text().split(", ", Qt::SkipEmptyParts);
        if (!tmp.isEmpty()) {
            result += tmp;
            result += "\\";
        }
    }
    return result;
}

void ListDialog::slotDoubleClicked()
{
    // we can't modify the first built-in items
    if (d->list->currentRow() < numBuiltinLists) {
        return;
    }
    const QStringList result = d->list->currentItem()->text().split(", ", Qt::SkipEmptyParts);
    d->textEdit->setText(result.join(QChar('\n')));
    d->textEdit->setEnabled(true);
    d->modifyButton->setEnabled(true);
}

void ListDialog::slotAdd()
{
    d->addButton->setEnabled(false);
    d->cancelButton->setEnabled(false);
    d->newButton->setEnabled(true);
    d->list->setEnabled(true);
    const QStringList tmp = d->textEdit->toPlainText().split(QChar('\n'), Qt::SkipEmptyParts);
    if (!tmp.isEmpty()) {
        d->list->addItem(tmp.join(", "));
    }

    d->textEdit->setText("");
    d->textEdit->setEnabled(false);
    d->textEdit->setFocus();
    slotCurrentRowChanged(0);
    d->changed = true;
}

void ListDialog::slotCancel()
{
    d->textEdit->setText("");
    slotAdd();
}

void ListDialog::slotNew()
{
    d->addButton->setEnabled(true);
    d->cancelButton->setEnabled(true);
    d->newButton->setEnabled(false);
    d->removeButton->setEnabled(false);
    d->modifyButton->setEnabled(false);
    d->copyButton->setEnabled(false);
    d->list->setEnabled(false);
    d->textEdit->setText("");
    d->textEdit->setEnabled(true);
    d->textEdit->setFocus();
}

void ListDialog::slotRemove()
{
    if (!d->list->isEnabled() || d->list->currentRow() == -1) {
        return;
    }
    // don't remove the first built-in items
    if (d->list->currentRow() < numBuiltinLists) {
        return;
    }
    int ret = KMessageBox::warningContinueCancel(this, i18n("Do you really want to remove this list?"), i18n("Remove List"), KStandardGuiItem::del());
    if (ret == Cancel) { // response = No
        return;
    }
    delete d->list->takeItem(d->list->currentRow());
    d->textEdit->setEnabled(false);
    d->textEdit->setText("");
    if (d->list->count() <= numBuiltinLists) {
        d->removeButton->setEnabled(false);
    }
    d->changed = true;
}

bool ListDialog::changed()
{
    return d->changed;
}

void ListDialog::onApply()
{
    if (!d->textEdit->toPlainText().isEmpty()) {
        int ret = KMessageBox::warningTwoActions(this,
                                                 i18n("Entry area is not empty.\nDo you want to continue?"),
                                                 {},
                                                 KStandardGuiItem::cont(),
                                                 KStandardGuiItem::cancel());
        if (ret == KMessageBox::SecondaryAction)
            return;
    }

    Q_EMIT saveChanges(customLists());
}

void ListDialog::slotModify()
{
    // you can modify list but not the first built-in items
    if (d->list->currentRow() >= numBuiltinLists && !d->textEdit->toPlainText().isEmpty()) {
        const QString tmp = d->textEdit->toPlainText().split(QChar('\n'), Qt::SkipEmptyParts).join(", ");
        d->list->insertItem(d->list->currentRow(), tmp);
        delete d->list->takeItem(d->list->currentRow());

        d->textEdit->setText("");
        d->changed = true;
    }
    d->textEdit->setEnabled(false);
    d->modifyButton->setEnabled(false);
}

void ListDialog::slotCopy()
{
    if (d->list->currentRow() != -1) {
        d->list->addItem(d->list->currentItem()->text());
    }
}
