/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SubtotalDialog.h"

#include "ui_SubtotalWidget.h"
#include "ui_SubtotalsDetailsWidget.h"

// KF5
#include <KMessageBox>

// Sheets
#include "core/Cell.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

class SubtotalDialog::Private
{
public:
    Selection *selection;
    Ui::SubtotalsWidget mainWidget;
    Ui::SubtotalsDetailsWidget detailsWidget;
    bool m_removeExisting;
};

SubtotalDialog::SubtotalDialog(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , d(new Private)
{
    d->selection = selection;
    d->m_removeExisting = false;

    setCaption(i18n("Subtotals"));
    setButtons(Ok | Cancel | Details | User1);
    setButtonGuiItem(User1, KGuiItem(i18n("Remove All")));

    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);

    widget = new QWidget(this);
    d->detailsWidget.setupUi(widget);
    setDetailsWidget(widget);

    fillColumnBoxes();
    fillFunctionBox();
    connect(this, &KoDialog::user1Clicked, this, &SubtotalDialog::slotUser1);
}

SubtotalDialog::~SubtotalDialog()
{
    delete d;
}

void SubtotalDialog::fillColumnBoxes()
{
    const QRect range = d->selection->lastRange();

    int index = 0;
    int r = range.right();
    for (int i = range.left(); i <= r; ++i) {
        QString text = i18n("Column '%1' ", CellBase::columnName(i));

        d->mainWidget.m_columnBox->insertItem(index++, text);

        QListWidgetItem *item = new QListWidgetItem(text);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        d->mainWidget.m_columnList->addItem(item);
    }
}

void SubtotalDialog::fillFunctionBox()
{
    QStringList lst;
    lst << i18n("Average");
    lst << i18n("Count");
    lst << i18n("CountA");
    lst << i18n("Max");
    lst << i18n("Min");
    lst << i18n("Product");
    lst << i18n("StDev");
    lst << i18n("StDevP");
    lst << i18n("Sum");
    lst << i18n("Var");
    lst << i18n("VarP");
    d->mainWidget.m_functionBox->insertItems(0, lst);
}

bool SubtotalDialog::removeExisting() const
{
    return d->m_removeExisting;
}

bool SubtotalDialog::summaryBelow() const
{
    return d->detailsWidget.m_summaryBelow->isChecked();
}

bool SubtotalDialog::summaryOnly() const
{
    return d->detailsWidget.m_summaryOnly->isChecked();
}

bool SubtotalDialog::ignoreEmpty() const
{
    return d->detailsWidget.m_IgnoreBox->isChecked();
}

int SubtotalDialog::funcCode() const
{
    return d->mainWidget.m_functionBox->currentIndex() + 1;
}

QList<int> SubtotalDialog::columns() const
{
    QList<int> res;
    QRect range = d->selection->lastRange();
    for (int i = 0; i < d->mainWidget.m_columnList->count(); ++i) {
        QListWidgetItem *item = d->mainWidget.m_columnList->item(i);
        if (item->checkState() == Qt::Checked)
            res.append(range.left() + i);
    }
    return res;
}

int SubtotalDialog::primaryColumn() const
{
    QRect range = d->selection->lastRange();
    return range.left() + d->mainWidget.m_columnBox->currentIndex();
}

void SubtotalDialog::accept()
{
    QList<int> cols = columns();
    if (cols.isEmpty()) {
        KMessageBox::error(this, i18n("You need to select at least one column for adding subtotals."));
        return;
    }

    KoDialog::accept();
}

void SubtotalDialog::slotUser1()
{
    d->m_removeExisting = true;
    KoDialog::accept();
}
