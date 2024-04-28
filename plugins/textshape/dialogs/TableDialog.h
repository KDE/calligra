/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <ui_TableDialog.h>

#include <KoDialog.h>

class TableDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit TableDialog(QWidget *parent);

    int columns();
    int rows();

Q_SIGNALS:

private:
    Ui::TableForm widget;
};

#endif
