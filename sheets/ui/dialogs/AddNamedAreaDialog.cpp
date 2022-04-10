/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1999-2001 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "AddNamedAreaDialog.h"

#include <QLabel>
#include <QVBoxLayout>

#include <klineedit.h>
#include <kmessagebox.h>

#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"
#include "core/Sheet.h"
#include "../Selection.h"

#include "../commands/NamedAreaCommand.h"

using namespace Calligra::Sheets;

AddNamedAreaDialog::AddNamedAreaDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent)
        , m_selection(selection)
{
    setButtons(Ok | Cancel);
    setCaption(i18n("Add Named Area"));
    setModal(true);
    setObjectName(QLatin1String("AddNamedAreaDialog"));

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

    connect(this, &KoDialog::okClicked, this, &AddNamedAreaDialog::slotOk);
    connect(m_areaName, &QLineEdit::textChanged,
            this, &AddNamedAreaDialog::slotAreaNameChanged);
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
        command->setText(kundo2_i18n("Replace Named Area"));
    } else
        command = new NamedAreaCommand();
    command->setSheet(m_selection->activeSheet());
    command->setAreaName(name);
    command->add(region);
    command->execute(m_selection->canvas());
}
