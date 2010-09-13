/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002 Ariya Hidayat <ariya@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2002 Laurent Montel <montel@kde.org>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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
#include "NamedAreaDialog.h"

// Qt
#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QVBoxLayout>

// KDE
#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klistwidget.h>
#include <kmessagebox.h>
#include <KStandardGuiItem>

#include <KoCanvasBase.h>

// KSpread
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include "Util.h"

#include "commands/NamedAreaCommand.h"

using namespace KSpread;

NamedAreaDialog::NamedAreaDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , m_selection(selection)
{
    setButtons(KDialog::Ok | KDialog::Close | KDialog::User1 | KDialog::User2 | KDialog::User3);
    setButtonsOrientation(Qt::Vertical);
    setButtonText(KDialog::Ok, i18n("&Select"));
    setButtonText(KDialog::User1, i18n("&Remove"));
    setButtonText(KDialog::User2, i18n("&Edit..."));
    setButtonText(KDialog::User3, i18n("&New..."));
    setCaption(i18n("Named Areas"));
    setModal(true);
    setObjectName("NamedAreaDialog");

    QWidget* widget = new QWidget(this);
    setMainWidget(widget);

    QVBoxLayout *vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setMargin(0);
    vboxLayout->setSpacing(KDialog::spacingHint());

    m_list = new KListWidget(this);
    m_list->setSortingEnabled(true);
    vboxLayout->addWidget(m_list);

    m_rangeName = new QLabel(this);
    m_rangeName->setText(i18n("Area: %1", QString()));
    vboxLayout->addWidget(m_rangeName);

    const QList<QString> namedAreas = m_selection->activeSheet()->map()->namedAreaManager()->areaNames();
    for (int i = 0; i < namedAreas.count(); ++i)
        m_list->addItem(namedAreas[i]);

    if (m_list->count() == 0) {
        enableButtonOk(false);
        enableButton(KDialog::User1, false);
        enableButton(KDialog::User2, false);
        m_list->setCurrentRow(0);
    }

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(slotClose()));
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotRemove()));
    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotEdit()));
    connect(this, SIGNAL(user3Clicked()), this, SLOT(slotNew()));
    connect(m_list, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(slotOk()));
    connect(m_list, SIGNAL(currentTextChanged(const QString&)),
            this, SLOT(displayAreaValues(const QString&)));

    if (m_list->count() > 0)
        m_list->setCurrentItem(m_list->item(0));

    m_list->setFocus();
}

void NamedAreaDialog::displayAreaValues(QString const & areaName)
{
    const QString regionName = m_selection->activeSheet()->map()->namedAreaManager()->namedArea(areaName).name();
    m_rangeName->setText(i18n("Area: %1", regionName));
}

void NamedAreaDialog::slotOk()
{
    if (m_list->count() > 0) {
        QListWidgetItem* item = m_list->currentItem();
        Region region = m_selection->activeSheet()->map()->namedAreaManager()->namedArea(item->text());
        Sheet* sheet = m_selection->activeSheet()->map()->namedAreaManager()->sheet(item->text());
        if (!sheet || !region.isValid()) {
            return;
        }

        if (sheet && sheet != m_selection->activeSheet())
            m_selection->emitVisibleSheetRequested(sheet);
        m_selection->initialize(region);
    }

    m_selection->emitModified();
    accept();
}

void NamedAreaDialog::slotClose()
{
    reject();
}

void NamedAreaDialog::slotNew()
{
    QPointer<EditNamedAreaDialog> dialog = new EditNamedAreaDialog(this, m_selection);
    dialog->setCaption(i18n("New Named Area"));
    dialog->setRegion(*m_selection);
    dialog->exec();
    if (dialog->result() == Rejected)
        return;
    if (dialog->areaName().isEmpty())
        return;

    m_list->addItem(dialog->areaName());
    QList<QListWidgetItem*> items = m_list->findItems(dialog->areaName(),
                                    Qt::MatchExactly | Qt::MatchCaseSensitive);
    m_list->setCurrentItem(items.first());
    displayAreaValues(dialog->areaName());
    delete dialog;

    enableButtonOk(true);
    enableButton(KDialog::User1, true);
    enableButton(KDialog::User2, true);
}

void NamedAreaDialog::slotEdit()
{
    QListWidgetItem* item = m_list->currentItem();
    if (item->text().isEmpty())
        return;

    QPointer<EditNamedAreaDialog> dialog = new EditNamedAreaDialog(this, m_selection);
    dialog->setCaption(i18n("Edit Named Area"));
    dialog->setAreaName(item->text());
    dialog->exec();
    if (dialog->result() == Rejected)
        return;

    item->setText(dialog->areaName());
    displayAreaValues(dialog->areaName());
    delete dialog;
}

void NamedAreaDialog::slotRemove()
{
    const QString question = i18n("Do you really want to remove this named area?");
    int result = KMessageBox::warningContinueCancel(this, question, i18n("Remove Named Area"),
                 KStandardGuiItem::del());
    if (result == KMessageBox::Cancel)
        return;

    QListWidgetItem* item = m_list->currentItem();

    NamedAreaCommand* command = new NamedAreaCommand();
    command->setAreaName(item->text());
    command->setReverse(true);
    command->setSheet(m_selection->activeSheet());
    if (!command->execute(m_selection->canvas())) {
        delete command;
        return;
    }
    m_list->takeItem(m_list->row(item));

    if (m_list->count() == 0) {
        enableButtonOk(false);
        enableButton(KDialog::User1, false);
        enableButton(KDialog::User2, false);
        displayAreaValues(QString());
    } else
        displayAreaValues(m_list->currentItem()->text());
}



EditNamedAreaDialog::EditNamedAreaDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
        , m_selection(selection)
{
    setButtons(Ok | Cancel);
    setModal(true);
    setObjectName("EditNamedAreaDialog");
    enableButtonOk(false);

    QWidget *page = new QWidget();
    setMainWidget(page);

    QGridLayout * gridLayout = new QGridLayout(page);
    gridLayout->setMargin(KDialog::marginHint());
    gridLayout->setSpacing(KDialog::spacingHint());

    QLabel * textLabel4 = new QLabel(page);
    textLabel4->setText(i18n("Cells:"));
    gridLayout->addWidget(textLabel4, 2, 0);

    m_cellRange = new KLineEdit(page);
    gridLayout->addWidget(m_cellRange, 2, 1);

    QLabel * textLabel1 = new QLabel(page);
    textLabel1->setText(i18n("Sheet:"));
    gridLayout->addWidget(textLabel1, 1, 0);

    m_sheets = new KComboBox(page);
    gridLayout->addWidget(m_sheets, 1, 1);

    QLabel * textLabel2 = new QLabel(page);
    textLabel2->setText(i18n("Area name:"));
    gridLayout->addWidget(textLabel2, 0, 0);

    m_areaNameEdit = new KLineEdit(page);
    gridLayout->addWidget(m_areaNameEdit, 0, 1);

    const QList<Sheet*> sheetList = m_selection->activeSheet()->map()->sheetList();
    for (int i = 0; i < sheetList.count(); ++i) {
        Sheet* sheet = sheetList.at(i);
        if (!sheet)
            continue;
        m_sheets->insertItem(i, sheet->sheetName());
    }

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(m_areaNameEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotAreaNameModified(const QString&)));
}

EditNamedAreaDialog::~EditNamedAreaDialog()
{
}

QString EditNamedAreaDialog::areaName() const
{
    return m_areaNameEdit->text();
}

void EditNamedAreaDialog::setAreaName(const QString& name)
{
    m_initialAreaName = name;
    m_areaNameEdit->setText(name);
    Sheet* sheet = m_selection->activeSheet()->map()->namedAreaManager()->sheet(name);
    const QString tmpName = m_selection->activeSheet()->map()->namedAreaManager()->namedArea(name).name(sheet);
    m_cellRange->setText(tmpName);
}

void EditNamedAreaDialog::setRegion(const Region& region)
{
    Sheet* sheet = region.firstSheet();
    m_sheets->setCurrentIndex(m_sheets->findText(sheet->sheetName()));
    m_cellRange->setText(region.name(sheet));
}

void EditNamedAreaDialog::slotOk()
{
    if (m_areaNameEdit->text().isEmpty())
        return;
    Sheet* sheet = m_selection->activeSheet()->map()->sheet(m_sheets->currentIndex());
    Region region(m_cellRange->text(), m_selection->activeSheet()->map(), sheet);
    if (!region.isValid())
        return;

    QUndoCommand* macroCommand = 0;
    if (!m_initialAreaName.isEmpty() && m_initialAreaName != m_areaNameEdit->text()) {
        macroCommand = new QUndoCommand(i18n("Replace Named Area"));
        // remove the old named area
        NamedAreaCommand* command = new NamedAreaCommand(macroCommand);
        command->setAreaName(m_initialAreaName);
        command->setReverse(true);
        command->setSheet(sheet);
        command->add(region);
    }

    // insert the new named area
    NamedAreaCommand* command = new NamedAreaCommand(macroCommand);
    command->setAreaName(m_areaNameEdit->text());
    command->setSheet(sheet);
    command->add(region);

    m_selection->canvas()->addCommand(macroCommand ? macroCommand : command);

    accept();
}

void EditNamedAreaDialog::slotAreaNameModified(const QString& name)
{
    enableButtonOk(!name.isEmpty());
}

#include "NamedAreaDialog.moc"
