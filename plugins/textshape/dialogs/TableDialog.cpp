/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 KO GmbH <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TableDialog.h"

#include <KLocalizedString>

TableDialog::TableDialog(QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Insert Table"));
    setButtons(KoDialog::Ok | KoDialog::Cancel);
    showButtonSeparator(true);
    QWidget *form = new QWidget;
    widget.setupUi(form);
    setMainWidget(form);

    /* disable stuff not done yet */
    widget.groupPhysical->setVisible(false);
}

int TableDialog::columns()
{
    return widget.intColumns->value();
}

int TableDialog::rows()
{
    return widget.intRows->value();
}
