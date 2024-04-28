/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Ganesh Paramasivam <ganesh@crystalfab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __ACCEPT_REJECT_CHANGE_DIALOG_H__
#define __ACCEPT_REJECT_CHANGE_DIALOG_H__

#include <ui_AcceptRejectChangeDialog.h>

class KoChangeTracker;

class AcceptRejectChangeDialog : public QDialog
{
    Q_OBJECT

public:
    typedef enum { eDialogCancelled = 0, eChangeAccepted, eChangeRejected } AcceptRejectResult;

    AcceptRejectChangeDialog(KoChangeTracker *changeTracker, int changeId);
    ~AcceptRejectChangeDialog();

private:
    Ui::AcceptRejectChangeDialog ui;

private Q_SLOTS:
    void changeAccepted();
    void changeRejected();
    void dialogCancelled();
};

#endif
