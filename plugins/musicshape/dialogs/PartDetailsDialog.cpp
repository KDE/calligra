/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "PartDetailsDialog.h"

#include "../MusicTool.h"

#include "../core/Part.h"

#include "../commands/ChangePartDetailsCommand.h"

using namespace MusicCore;

PartDetailsDialog::PartDetailsDialog(Part *part, QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Part details"));
    QWidget *w = new QWidget(this);
    widget.setupUi(w);
    setMainWidget(w);

    widget.nameEdit->setText(part->name());
    widget.shortNameEdit->setText(part->shortName());
    widget.staffCount->setValue(part->staffCount());
}

void PartDetailsDialog::showDialog(MusicTool *tool, Part *part, QWidget *parent)
{
    PartDetailsDialog dlg(part, parent);
    if (dlg.exec() == QDialog::Accepted) {
        tool->addCommand(
            new ChangePartDetailsCommand(tool->shape(), part, dlg.widget.nameEdit->text(), dlg.widget.shortNameEdit->text(), dlg.widget.staffCount->value()));
    }
}
