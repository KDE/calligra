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

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>

#include <KLocalizedString>

using namespace Calligra::Sheets;

AngleDialog::AngleDialog(QWidget* parent, int angle)
        : KoDialog(parent)
{
    setCaption(i18n("Change Angle"));
    setModal(true);
    setButtons(Ok | Cancel | Default);

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
    m_pAngle->setValue(angle);
}

void AngleDialog::slotOk()
{
    accept();
}

int AngleDialog::angle()
{
    return m_pAngle->value();
}

void AngleDialog::slotDefault()
{
    m_pAngle->setValue(0);
}
