/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999-2003 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2003 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "GotoDialog.h"

#include <QLabel>
#include <QVBoxLayout>

#include <kcombobox.h>

#include "Cell.h"
#include "Localization.h"
#include "Map.h"
#include "NamedAreaManager.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include "Util.h"

using namespace Calligra::Sheets;

GotoDialog::GotoDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent)
{
    setCaption(i18n("Goto Cell"));
    setObjectName(QLatin1String("GotoDialog"));
    setModal(true);
    setButtons(Ok | Cancel);

    m_selection = selection;
    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    QLabel *label = new QLabel(i18n("Enter cell:"), page);
    lay1->addWidget(label);

    m_nameCell = new KComboBox(page);
    m_nameCell->setEditable(true);
    lay1->addWidget(m_nameCell);

    const Sheet* sheet = m_selection->activeSheet();
    if (sheet && selection) {
        Cell cell(sheet, selection->cursor());
        m_nameCell->addItem(cell.name());
        m_nameCell->addItem(cell.fullName());
    }
    NamedAreaManager *manager = m_selection->activeSheet()->map()->namedAreaManager();
    m_nameCell->addItems(manager->areaNames());
    m_nameCell->setFocus();

    connect(this, &KoDialog::okClicked, this, &GotoDialog::slotOk);
    connect(m_nameCell, &QComboBox::editTextChanged,
            this, &GotoDialog::textChanged);

    resize(QSize(320, 50).expandedTo(minimumSizeHint()));
}

void GotoDialog::textChanged(const QString &_text)
{
    enableButtonOk(!_text.isEmpty());
}

void GotoDialog::slotOk()
{
    QString tmp_upper = m_nameCell->currentText();
    Region region(tmp_upper, m_selection->activeSheet()->map(), m_selection->activeSheet());
    if (region.isValid()) {
        if (region.firstSheet() != m_selection->activeSheet())
            m_selection->emitVisibleSheetRequested(region.firstSheet());
        m_selection->initialize(region);
        accept();
    } else {
        m_nameCell->setItemText(m_nameCell->currentIndex(), "");
    }
}
