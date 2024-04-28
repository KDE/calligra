/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999-2004 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ShowDialog.h"

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

#include <KLocalizedString>

#include "../ui/Selection.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "engine/Damages.h"

// commands
#include "../commands/SheetCommands.h"

using namespace Calligra::Sheets;

ShowDialog::ShowDialog(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , m_selection(selection)
{
    setCaption(i18n("Show Sheet"));
    setModal(true);
    setButtons(Ok | Cancel);
    setObjectName(QLatin1String("ShowDialog"));

    QWidget *page = new QWidget(this);
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);
    lay1->setContentsMargins({});

    QLabel *label = new QLabel(i18n("Select hidden sheets to show:"), page);
    lay1->addWidget(label);

    m_listWidget = new QListWidget(page);
    lay1->addWidget(m_listWidget);

    m_listWidget->setSelectionMode(QListWidget::MultiSelection);
    QStringList tabsList = m_selection->activeSheet()->fullMap()->hiddenSheets();
    m_listWidget->addItems(tabsList);
    if (!m_listWidget->count())
        enableButtonOk(false);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &ShowDialog::accept);
    resize(200, 150);
    setFocus();
}

void ShowDialog::accept()
{
    const QList<QListWidgetItem *> items = m_listWidget->selectedItems();

    if (items.count() == 0) {
        return;
    }

    Map *const map = m_selection->activeSheet()->fullMap();
    KUndo2Command *macroCommand = new KUndo2Command(kundo2_i18n("Show Sheet"));
    for (int i = 0; i < items.count(); ++i) {
        Sheet *sheet = dynamic_cast<Sheet *>(map->findSheet(items[i]->text()));
        if (!sheet)
            continue;
        new ShowSheetCommand(sheet, macroCommand);
    }
    map->addCommand(macroCommand);
    KoDialog::accept();
}
