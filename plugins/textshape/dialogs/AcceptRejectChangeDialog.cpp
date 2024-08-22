/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "AcceptRejectChangeDialog.h"
#include <KoChangeTracker.h>
#include <KoChangeTrackerElement.h>

AcceptRejectChangeDialog::AcceptRejectChangeDialog(KoChangeTracker *changeTracker, int changeId)
{
    ui.setupUi(this);
    ui.authorNameLineEdit->setText(changeTracker->elementById(changeId)->getCreator());
    ui.dateLineEdit->setText(changeTracker->elementById(changeId)->getDate());
    KoGenChange::Type changeType = changeTracker->elementById(changeId)->getChangeType();

    if (changeType == KoGenChange::InsertChange) {
        ui.changeTypeLineEdit->setText(QString("Insertion"));
    } else if (changeType == KoGenChange::FormatChange) {
        ui.changeTypeLineEdit->setText(QString("Formatting"));
    } else {
        ui.changeTypeLineEdit->setText(QString("Deletion"));
    }

    connect(ui.acceptButton, &QAbstractButton::released, this, &AcceptRejectChangeDialog::changeAccepted);
    connect(ui.rejectButton, &QAbstractButton::released, this, &AcceptRejectChangeDialog::changeRejected);
    connect(ui.cancelButton, &QAbstractButton::released, this, &AcceptRejectChangeDialog::dialogCancelled);
}

AcceptRejectChangeDialog::~AcceptRejectChangeDialog() = default;

void AcceptRejectChangeDialog::changeAccepted()
{
    this->done(AcceptRejectChangeDialog::eChangeAccepted);
}

void AcceptRejectChangeDialog::changeRejected()
{
    this->done(AcceptRejectChangeDialog::eChangeRejected);
}

void AcceptRejectChangeDialog::dialogCancelled()
{
    this->done(AcceptRejectChangeDialog::eDialogCancelled);
}
