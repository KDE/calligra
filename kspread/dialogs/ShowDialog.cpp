/* This file is part of the KDE project
   Copyright (C) 1999-2004 Laurent Montel <montel@kde.org>
             (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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
#include "ShowDialog.h"

#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>

#include <klocale.h>

#include "Damages.h"
#include "Map.h"
#include "ui/Selection.h"
#include "Sheet.h"

// commands
#include "commands/SheetCommands.h"

using namespace KSpread;

ShowDialog::ShowDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , m_selection(selection)
{
    setCaption(i18n("Show Sheet"));
    setModal(true);
    setButtons(Ok | Cancel);
    setObjectName("ShowDialog");

    QWidget *page = new QWidget(this);
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);
    lay1->setMargin(0);
    lay1->setSpacing(spacingHint());

    QLabel *label = new QLabel(i18n("Select hidden sheets to show:"), page);
    lay1->addWidget(label);

    m_listWidget = new QListWidget(page);
    lay1->addWidget(m_listWidget);

    m_listWidget->setSelectionMode(QListWidget::MultiSelection);
    QString text;
    QStringList::Iterator it;
    QStringList tabsList = m_selection->activeSheet()->map()->hiddenSheets();
    m_listWidget->addItems(tabsList);
    if (!m_listWidget->count())
        enableButtonOk(false);
    connect(m_listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this, SLOT(accept()));
    resize(200, 150);
    setFocus();
}

void ShowDialog::accept()
{
    const QList<QListWidgetItem *> items = m_listWidget->selectedItems();

    if (items.count() == 0) {
        return;
    }

    Map *const map = m_selection->activeSheet()->map();
    Sheet *sheet;
    QUndoCommand* macroCommand = new QUndoCommand(i18n("Show Sheet"));
    for (int i = 0; i < items.count(); ++i) {
        sheet = map->findSheet(items[i]->text());
        if (!sheet)
            continue;
        new ShowSheetCommand(sheet, macroCommand);
    }
    map->addCommand(macroCommand);
    // Just repaint everything visible; no need to invalidate the visual cache.
    map->addDamage(new SheetDamage(m_selection->activeSheet(), SheetDamage::ContentChanged));
    KDialog::accept();
}

#include "ShowDialog.moc"
