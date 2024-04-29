/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "NamedAreaDialog.h"

// Qt
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>

// KF5
#include <KComboBox>
#include <KMessageBox>
#include <klineedit.h>
#include <kstandardguiitem.h>

#include <KoCanvasBase.h>

// Sheets
#include "core/Sheet.h"
#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"
#include "ui/Selection.h"

#include "ui/commands/NamedAreaCommand.h"

using namespace Calligra::Sheets;

NamedAreaDialog::NamedAreaDialog(QWidget *parent, Selection *selection)
    : ActionDialog(parent)
    , m_selection(selection)
{
    setButtons(Close);
    setCaption(i18n("Named Areas"));

    // *** SPLITTER ***
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);
    splitter->setHandleWidth(5);
    setMainWidget(splitter);

    // *** LEFT ***
    QWidget *leftWidget = new QWidget(splitter);
    splitter->addWidget(leftWidget);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins({});

    QVBoxLayout *vboxLayout = new QVBoxLayout();

    m_list = new QListWidget(this);
    m_list->setSortingEnabled(false);
    vboxLayout->addWidget(m_list);

    m_rangeName = new QLabel(this);
    m_rangeName->setText(i18n("Area: %1", QString()));
    vboxLayout->addWidget(m_rangeName);

    leftLayout->addLayout(vboxLayout);

    // *** RIGHT ***
    QFrame *rightWidget = new QFrame(splitter);
    rightWidget->setFrameStyle(QFrame::StyledPanel);
    splitter->addWidget(rightWidget);

    QGridLayout *gridLayout = new QGridLayout(rightWidget);

    QLabel *textLabel4 = new QLabel(rightWidget);
    textLabel4->setText(i18n("Cells:"));
    gridLayout->addWidget(textLabel4, 2, 0);

    m_cellRange = new KLineEdit(rightWidget);
    gridLayout->addWidget(m_cellRange, 2, 1);

    QLabel *textLabel1 = new QLabel(rightWidget);
    textLabel1->setText(i18n("Sheet:"));
    gridLayout->addWidget(textLabel1, 1, 0);

    m_sheets = new KComboBox(rightWidget);
    gridLayout->addWidget(m_sheets, 1, 1);

    QLabel *textLabel2 = new QLabel(rightWidget);
    textLabel2->setText(i18n("Area name:"));
    gridLayout->addWidget(textLabel2, 0, 0);

    m_areaNameEdit = new KLineEdit(rightWidget);
    gridLayout->addWidget(m_areaNameEdit, 0, 1);

    m_removeButton = new QPushButton(i18n("&Remove"), rightWidget);
    gridLayout->addWidget(m_removeButton, 4, 0);
    m_saveButton = new QPushButton(i18n("Save"), rightWidget);
    gridLayout->addWidget(m_saveButton, 4, 1);

    gridLayout->setRowStretch(4, 1);

    connect(m_removeButton, &QAbstractButton::clicked, this, &NamedAreaDialog::slotRemove);
    connect(m_saveButton, &QAbstractButton::clicked, this, &NamedAreaDialog::slotSave);
    connect(m_list, &QListWidget::itemActivated, this, &NamedAreaDialog::slotActivated);

    m_list->setFocus();

    fillData();
}

void NamedAreaDialog::fillData()
{
    QListWidgetItem *item = m_list->currentItem();
    QString cur = item ? item->text() : QString();

    MapBase *map = m_selection->activeSheet()->map();
    m_list->clear();
    QList<QString> namedAreas = map->namedAreaManager()->areaNames();
    namedAreas.sort();
    for (int i = 0; i < namedAreas.count(); ++i)
        m_list->addItem(namedAreas[i]);
    m_list->addItem(i18n("(new area)"));

    if (m_list->count() == 1) {
        m_removeButton->setEnabled(false);
    } else {
        m_removeButton->setEnabled(true);
        QList<QListWidgetItem *> items = m_list->findItems(cur, Qt::MatchFixedString);
        int idx = 0;
        if (items.size())
            idx = m_list->row(items.front());
        m_list->setCurrentRow(idx);
    }

    m_sheets->clear();
    const QList<SheetBase *> sheetList = map->sheetList();
    for (int i = 0; i < sheetList.count(); ++i) {
        SheetBase *sheet = sheetList.at(i);
        if (!sheet)
            continue;
        m_sheets->insertItem(i, sheet->sheetName());
    }

    slotActivated();
}

void NamedAreaDialog::slotActivated()
{
    m_rangeName->setText(QString());
    m_areaNameEdit->setText(QString());
    m_sheets->setCurrentIndex(0);
    m_cellRange->setText(QString());
    if (m_list->count() <= 1)
        return;

    QListWidgetItem *item = m_list->currentItem();
    QString name = item->text();
    NamedAreaManager *manager = m_selection->activeSheet()->map()->namedAreaManager();
    Region region = manager->namedArea(name);
    SheetBase *sheet = manager->sheet(name);

    if (!sheet || !region.isValid())
        return;
    Sheet *fullSheet = dynamic_cast<Sheet *>(sheet);

    m_rangeName->setText(i18n("Area: %1", region.name(sheet)));

    // fill in the editor
    m_areaNameEdit->setText(name);
    m_sheets->setCurrentIndex(m_sheets->findText(sheet->sheetName()));
    m_cellRange->setText(region.name(sheet));

    Q_EMIT requestSelection(region, fullSheet);
}

void NamedAreaDialog::onSelectionChanged(Selection *)
{
    m_cellRange->setText(m_selection->name(m_selection->activeSheet()));
}

void NamedAreaDialog::slotRemove()
{
    const QString question = i18n("Do you really want to remove this named area?");
    int result = KMessageBox::warningContinueCancel(this, question, i18n("Remove Named Area"), KStandardGuiItem::del());
    if (result == KMessageBox::Cancel)
        return;

    QListWidgetItem *item = m_list->currentItem();

    NamedAreaCommand *command = new NamedAreaCommand();
    command->setAreaName(item->text());
    command->setRemove(true);
    command->setSheet(m_selection->activeSheet());
    command->execute(m_selection->canvas());
    fillData();
}

void NamedAreaDialog::slotSave()
{
    QString name = m_areaNameEdit->text();
    QString regName = m_cellRange->text();
    MapBase *map = m_selection->activeSheet()->map();
    SheetBase *baseSheet = map->sheet(m_sheets->currentIndex());
    Sheet *sheet = dynamic_cast<Sheet *>(baseSheet);

    if (!name.length())
        return;
    Region region = map->regionFromName(regName, baseSheet);
    if (!region.isValid()) {
        KMessageBox::error(this, i18n("The provided region is not valid."));
        return;
    }

    NamedAreaCommand *command = new NamedAreaCommand();
    command->setSheet(sheet);
    command->add(region);

    // New one?
    if (m_list->currentRow() == m_list->count() - 1) {
        command->setAreaName(name);
    } else {
        QListWidgetItem *item = m_list->currentItem();
        QString origName = item->text();
        command->setAreaName(origName);
        command->setNewAreaName(name);
    }
    command->execute(m_selection->canvas());

    fillData();
}
