/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "AngleDialog.h"

// #include <QLabel>
// #include <QPushButton>
// #include <QSpinBox>
// #include <QVBoxLayout>

// #include <KLocalizedString>

// #include <KoCanvasBase.h>

// #include "Cell.h"
// #include "ui/Selection.h"
// #include "Sheet.h"

// #include "commands/StyleCommand.h"
// #include "commands/RowColumnManipulators.h"

using namespace Calligra::Sheets;

AngleDialog::AngleDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent)
{
    setCaption(i18n("Change Angle"));
    setModal(true);
    setButtons(Ok | Cancel | Default);

    m_selection = selection;

    QWidget *page = new QWidget();
    setMainWidget(page);

    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setMargin(0);
    QLabel *label = new QLabel(i18n("Angle:"), page);
    lay->addWidget(label);

    m_pAngle = new QSpinBox(page);
    m_pAngle->setRange(-90, 90);
    m_pAngle->setSingleStep(1);
    m_pAngle->setSuffix(" ");
    lay->addWidget(m_pAngle);

    QWidget* spacer = new QWidget(page);
    spacer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    lay->addWidget(spacer);

    m_pAngle->setFocus();

    connect(this, &KoDialog::okClicked, this, &AngleDialog::slotOk);
    connect(this, &KoDialog::defaultClicked, this, &AngleDialog::slotDefault);
    int angle = - Cell(m_selection->activeSheet(), m_selection->marker()).style().angle();
    m_pAngle->setValue(angle);
}

void AngleDialog::slotOk()
{
    KUndo2Command* macroCommand = new KUndo2Command(kundo2_i18n("Change Angle"));

    StyleCommand* manipulator = new StyleCommand(macroCommand);
    manipulator->setSheet(m_selection->activeSheet());
    manipulator->setAngle(-m_pAngle->value());
    manipulator->add(*m_selection);

    AdjustColumnRowManipulator* manipulator2 = new AdjustColumnRowManipulator(macroCommand);
    manipulator2->setSheet(m_selection->activeSheet());
    manipulator2->setAdjustColumn(true);
    manipulator2->setAdjustRow(true);
    manipulator2->add(*m_selection);

    m_selection->canvas()->addCommand(macroCommand);
    accept();
}

void AngleDialog::slotDefault()
{
    m_pAngle->setValue(0);
}
