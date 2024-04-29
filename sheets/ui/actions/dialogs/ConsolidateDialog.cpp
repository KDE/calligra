/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000-2001 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ConsolidateDialog.h"

// KF5
#include <KMessageBox>

#include <KoIcon.h>

#include "core/Sheet.h"
#include "engine/MapBase.h"
#include "ui/Selection.h"

#include "ui_ConsolidateDetailsWidget.h"
#include "ui_ConsolidateWidget.h"

using namespace Calligra::Sheets;

class ConsolidateDialog::Private
{
public:
    Selection *selection;
    Ui::ConsolidateWidget mainWidget;
    Ui::ConsolidateDetailsWidget detailsWidget;
};

ConsolidateDialog::ConsolidateDialog(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , d(new Private)
{
    d->selection = selection;

    setCaption(i18n("Consolidate"));
    setModal(false);
    setButtons(Ok | Cancel | Details);
    enableButton(Ok, false);

    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);

    widget = new QWidget(this);
    d->detailsWidget.setupUi(widget);
    setDetailsWidget(widget);

    // ui refinements Designer is not capable of
    d->mainWidget.m_addButton->setIcon(koIcon("list-add"));
    d->mainWidget.m_removeButton->setIcon(koIcon("list-remove"));

    d->mainWidget.m_function->addItem(i18n("Sum"), "SUM");
    d->mainWidget.m_function->addItem(i18n("Average"), "AVERAGE");
    d->mainWidget.m_function->addItem(i18n("Count"), "COUNT");
    d->mainWidget.m_function->addItem(i18n("Max"), "MAX");
    d->mainWidget.m_function->addItem(i18n("Min"), "MIN");
    d->mainWidget.m_function->addItem(i18n("Product"), "PRODUCT");
    d->mainWidget.m_function->addItem(i18n("Standard Deviation"), "STDEV");
    d->mainWidget.m_function->addItem(i18n("Variance"), "VAR");

    connect(d->mainWidget.m_addButton, &QAbstractButton::clicked, this, &ConsolidateDialog::slotAdd);
    connect(d->mainWidget.m_removeButton, &QAbstractButton::clicked, this, &ConsolidateDialog::slotRemove);
    connect(d->mainWidget.m_sourceRange, &KLineEdit::returnPressed, this, &ConsolidateDialog::slotReturnPressed);

    connect(d->selection, &Selection::changed, this, &ConsolidateDialog::slotSelectionChanged);
}

ConsolidateDialog::~ConsolidateDialog()
{
    delete d;
}

QList<Region> ConsolidateDialog::sourceRegions() const
{
    Sheet *const destinationSheet = d->selection->activeSheet();
    QList<Region> ranges;
    MapBase *map = d->selection->activeSheet()->map();
    for (int i = 0; i < d->mainWidget.m_sourceRanges->count(); ++i) {
        const QString address = d->mainWidget.m_sourceRanges->item(i)->text();
        const Region region = map->regionFromName(address, destinationSheet);
        if (region.isValid())
            ranges.append(region);
    }
    return ranges;
}

Region ConsolidateDialog::targetRegion() const
{
    return Region(d->selection->lastRange(), d->selection->activeSheet());
}

QString ConsolidateDialog::function() const
{
    int index = d->mainWidget.m_function->currentIndex();
    return d->mainWidget.m_function->itemData(index).toString();
}

bool ConsolidateDialog::rowHeaders() const
{
    return d->detailsWidget.m_rowHeader->isChecked();
}

bool ConsolidateDialog::colHeaders() const
{
    return d->detailsWidget.m_columnHeader->isChecked();
}

void ConsolidateDialog::accept()
{
    // Some sanity checking.

    if (d->mainWidget.m_sourceRanges->count() == 0) {
        KMessageBox::error(this, i18n("You have to define at least one source range."));
        return;
    }
    int mincols = 1, minrows = 1;
    if (colHeaders())
        mincols = 2;
    if (rowHeaders())
        minrows = 2;
    MapBase *map = d->selection->activeSheet()->map();
    Region tg = targetRegion();

    for (int i = 0; i < d->mainWidget.m_sourceRanges->count(); ++i) {
        const QString address = d->mainWidget.m_sourceRanges->item(i)->text();
        const Region region = map->regionFromName(address, tg.firstSheet());
        if (!region.isValid()) {
            KMessageBox::error(this, i18n("%1 is not a valid cell range.", address));
            return;
        }
        const QRect firstRange = region.firstRange();
        if ((firstRange.width() < mincols) || (firstRange.height() < minrows)) {
            KMessageBox::error(this, i18n("The range\n%1\nis too small", region.name()));
            return;
        }
        if (region.isAllSelected() || region.isColumnOrRowSelected()) {
            KMessageBox::error(this, i18n("The range\n%1\nis too large", region.name()));
            return;
        }

        if ((tg.firstSheet() == region.firstSheet()) && tg.firstRange().intersects(region.firstRange())) {
            KMessageBox::error(this, i18n("The source range intersects the destination range."));
            return;
        }
    }

    Q_EMIT applyChange();
    KoDialog::accept();
}

void ConsolidateDialog::slotAdd()
{
    slotReturnPressed();
}

void ConsolidateDialog::slotRemove()
{
    int i = d->mainWidget.m_sourceRanges->currentRow();
    if (i < 0)
        return;

    delete d->mainWidget.m_sourceRanges->takeItem(i);

    if (d->mainWidget.m_sourceRanges->count() == 0)
        enableButton(Ok, false);
}

void ConsolidateDialog::slotSelectionChanged()
{
    if (!d->selection->isValid()) {
        d->mainWidget.m_sourceRange->setText("");
        return;
    }

    QString area = d->selection->name();
    d->mainWidget.m_sourceRange->setText(area);
    d->mainWidget.m_sourceRange->setSelection(0, area.length());
}

void ConsolidateDialog::slotReturnPressed()
{
    QString txt = d->mainWidget.m_sourceRange->text();

    const Region r = d->selection->activeSheet()->map()->regionFromName(txt, d->selection->activeSheet());
    if (!r.isValid()) {
        KMessageBox::error(this, i18n("The range\n%1\n is malformed", txt));
        return;
    }

    if (!txt.isEmpty()) {
        d->mainWidget.m_sourceRanges->addItem(txt);
        enableButton(Ok, true);
    }
}
