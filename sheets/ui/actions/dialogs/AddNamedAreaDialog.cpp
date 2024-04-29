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
#include <QLineEdit>
#include <QVBoxLayout>

#include <KLocalizedString>

using namespace Calligra::Sheets;

AddNamedAreaDialog::AddNamedAreaDialog(QWidget *parent)
    : ActionDialog(parent)
{
    setButtonText(Apply, i18n("Add Named Area"));
    setCaption(i18n("Add Named Area"));
    setObjectName(QLatin1String("AddNamedAreaDialog"));

    QWidget *widget = new QWidget();
    setMainWidget(widget);

    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *label = new QLabel(i18n("Enter the area name:"), widget);
    layout->addWidget(label);

    m_areaName = new QLineEdit(widget);
    m_areaName->setFocus();
    m_areaName->setMinimumWidth(m_areaName->sizeHint().width() * 3);
    layout->addWidget(m_areaName);

    enableButtonOk(!m_areaName->text().isEmpty());

    connect(m_areaName, &QLineEdit::textChanged, this, &AddNamedAreaDialog::slotAreaNameChanged);
}

void AddNamedAreaDialog::slotAreaNameChanged(const QString &name)
{
    enableButtonOk(!name.isEmpty());
}

QString AddNamedAreaDialog::areaName() const
{
    return m_areaName->text();
}

void AddNamedAreaDialog::onApply()
{
    Q_EMIT addArea(areaName());
}
