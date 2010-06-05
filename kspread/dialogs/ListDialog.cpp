/* This file is part of the KDE project
   Copyright (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
             (C) 2001-2003 Laurent Montel <montel@kde.org>
             (C) 1998, 1999 Torben Weis <weis@kde.org>

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
#include "ListDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>

#include <kcomponentdata.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kmessagebox.h>
#include <KTextEdit>

#include "commands/AutoFillCommand.h"
#include "Localization.h"

using namespace KSpread;

class ListDialog::Private
{
public:
    KSharedConfigPtr config;

    QListWidget* list;
    KTextEdit* textEdit;
    QPushButton* addButton;
    QPushButton* cancelButton;
    QPushButton* removeButton;
    QPushButton* newButton;
    QPushButton* modifyButton;
    QPushButton* copyButton;
    bool changed;
};

static const int numBuiltinLists = 4;

ListDialog::ListDialog(QWidget* parent)
        : KDialog(parent)
        , d(new Private)
{
    setCaption(i18n("Custom Lists"));
    setButtons(Ok | Cancel);
    setModal(true);

    QWidget* page = new QWidget(this);
    setMainWidget(page);

    QGridLayout *grid1 = new QGridLayout(page);
    grid1->setMargin(KDialog::marginHint());
    grid1->setSpacing(KDialog::spacingHint());

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

    connect(d->addButton, SIGNAL(clicked()), this, SLOT(slotAdd()));
    connect(d->cancelButton, SIGNAL(clicked()), this, SLOT(slotCancel()));
    connect(d->newButton, SIGNAL(clicked()), this, SLOT(slotNew()));
    connect(d->removeButton, SIGNAL(clicked()), this, SLOT(slotRemove()));
    connect(d->modifyButton, SIGNAL(clicked()), this, SLOT(slotModify()));
    connect(d->copyButton, SIGNAL(clicked()), this, SLOT(slotCopy()));
    connect(d->list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotDoubleClicked()));
    connect(d->list, SIGNAL(currentRowChanged(int)), this, SLOT(slotCurrentRowChanged(int)));
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));

    init();
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
    //we can't remove the first built-in items
    const bool state = row >= numBuiltinLists;
    d->removeButton->setEnabled(state);
    d->copyButton->setEnabled(row >= 0);
    d->textEdit->setEnabled(false);
    d->textEdit->clear();
}

void ListDialog::init()
{
    QString month;
    month += i18n("January") + ", ";
    month += i18n("February") + ", ";
    month += i18n("March") + ", ";
    month += i18n("April") + ", ";
    month += i18n("May") + ", ";
    month += i18n("June") + ", ";
    month += i18n("July") + ", ";
    month += i18n("August") + ", ";
    month += i18n("September") + ", ";
    month += i18n("October") + ", ";
    month += i18n("November") + ", ";
    month += i18n("December");
    QStringList lst;
    lst.append(month);

    QString smonth;
    smonth += i18n("Jan") + ", ";
    smonth += i18n("Feb") + ", ";
    smonth += i18n("Mar") + ", ";
    smonth += i18n("Apr") + ", ";
    smonth += i18n("May") + ", ";
    smonth += i18n("Jun") + ", ";
    smonth += i18n("Jul") + ", ";
    smonth += i18n("Aug") + ", ";
    smonth += i18n("Sep") + ", ";
    smonth += i18n("Oct") + ", ";
    smonth += i18n("Nov") + ", ";
    smonth += i18n("Dec");
    lst.append(smonth);

    QString day = i18n("Monday") + ", ";
    day += i18n("Tuesday") + ", ";
    day += i18n("Wednesday") + ", ";
    day += i18n("Thursday") + ", ";
    day += i18n("Friday") + ", ";
    day += i18n("Saturday") + ", ";
    day += i18n("Sunday");
    lst.append(day);

    QString sday = i18n("Mon") + ", ";
    sday += i18n("Tue") + ", ";
    sday += i18n("Wed") + ", ";
    sday += i18n("Thu") + ", ";
    sday += i18n("Fri") + ", ";
    sday += i18n("Sat") + ", ";
    sday += i18n("Sun");
    lst.append(sday);

    d->config = KGlobal::activeComponent().config();
    const QStringList other = d->config->group("Parameters").readEntry("Other list", QStringList());
    QString tmp;
    for (QStringList::ConstIterator it = other.begin(); it != other.end();++it) {
        if ((*it) != "\\") {
            tmp += (*it) + ", ";
        } else if (it != other.begin()) {
            tmp = tmp.left(tmp.length() - 2);
            lst.append(tmp);
            tmp = "";
        }
    }
    d->list->addItems(lst);
}

void ListDialog::slotDoubleClicked()
{
    //we can't modify the first built-in items
    if (d->list->currentRow() < numBuiltinLists) {
        return;
    }
    const QStringList result = d->list->currentItem()->text().split(", ", QString::SkipEmptyParts);
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
    const QStringList tmp = d->textEdit->toPlainText().split(QChar('\n'), QString::SkipEmptyParts);
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
    //don't remove the first built-in items
    if (d->list->currentRow() < numBuiltinLists) {
        return;
    }
    int ret = KMessageBox::warningContinueCancel(this,
              i18n("Do you really want to remove this list?"),
              i18n("Remove List"), KStandardGuiItem::del());
    if (ret == Cancel) { // reponse = No
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

void ListDialog::slotOk()
{
    if (!d->textEdit->toPlainText().isEmpty()) {
        int ret = KMessageBox::warningYesNo(this, i18n("Entry area is not empty.\nDo you want to continue?"));
        if (ret == 4) { // reponse = No
            return;
        }
    }
    if (d->changed) {
        QStringList result;
        result.append("\\");

        //don't save the first built-in lines
        for (int i = numBuiltinLists - 1; i < d->list->count(); ++i) {
            QStringList tmp = d->list->item(i)->text().split(", ", QString::SkipEmptyParts);
            if (!tmp.isEmpty()) {
                result += tmp;
                result += "\\";
            }
        }
        d->config->group("Parameters").writeEntry("Other list", result);
        //todo refresh AutoFillCommand::other
        // I don't know how to do for the moment
        if (AutoFillCommand::other != 0) {
            delete(AutoFillCommand::other);
            AutoFillCommand::other = 0;
        }
    }
    accept();
}

void ListDialog::slotModify()
{
    //you can modify list but not the first built-in items
    if (d->list->currentRow() >= numBuiltinLists && !d->textEdit->toPlainText().isEmpty()) {
        const QString tmp = d->textEdit->toPlainText().split(QChar('\n'), QString::SkipEmptyParts).join(", ");
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

#include "ListDialog.moc"
