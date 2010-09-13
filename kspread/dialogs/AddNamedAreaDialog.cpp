/* This file is part of the KDE project
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2001 Laurent Montel <montel@kde.org>
   Copyright 1998-1999 Torben Weis <weis@kde.org>

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
#include "AddNamedAreaDialog.h"

#include <QLabel>
#include <QVBoxLayout>

#include <klineedit.h>
#include <kmessagebox.h>

#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "ui/Selection.h"
#include "Sheet.h"

#include "commands/NamedAreaCommand.h"

using namespace KSpread;

AddNamedAreaDialog::AddNamedAreaDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , m_selection(selection)
{
    setButtons(Ok | Cancel);
    setCaption(i18n("Add Named Area"));
    setModal(true);
    setObjectName("AddNamedAreaDialog");

    QWidget* widget = new QWidget();
    setMainWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout(widget);

    QLabel* label = new QLabel(i18n("Enter the area name:"), widget);
    layout->addWidget(label);

    m_areaName = new KLineEdit(widget);
    m_areaName->setFocus();
    m_areaName->setMinimumWidth(m_areaName->sizeHint().width()* 3);
    layout->addWidget(m_areaName);

    enableButtonOk(!m_areaName->text().isEmpty());

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(m_areaName, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAreaNameChanged(const QString&)));
}

void AddNamedAreaDialog::slotAreaNameChanged(const QString& name)
{
    enableButtonOk(!name.isEmpty());
}

void AddNamedAreaDialog::slotOk()
{
    if (m_areaName->text().isEmpty())
        return;

    const QString name = m_areaName->text();
    const Region region(m_selection->lastRange(), m_selection->lastSheet());
    if (m_selection->activeSheet()->map()->namedAreaManager()->namedArea(name) == region)
        return; // nothing to do

    NamedAreaCommand* command = 0;
    if (m_selection->activeSheet()->map()->namedAreaManager()->contains(name)) {
        const QString question = i18n("The named area '%1' already exists.\n"
                                      "Do you want to replace it?", name);
        int result = KMessageBox::warningContinueCancel(this, question, i18n("Replace Named Area"),
                     KStandardGuiItem::overwrite());
        if (result == KMessageBox::Cancel)
            return;

        command = new NamedAreaCommand();
        command->setText(i18n("Replace Named Area"));
    } else
        command = new NamedAreaCommand();
    command->setSheet(m_selection->activeSheet());
    command->setAreaName(name);
    command->add(region);
    command->execute(m_selection->canvas());
}

#include "AddNamedAreaDialog.moc"
