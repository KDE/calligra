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

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include <KLocalizedString>

using namespace Calligra::Sheets;

GotoDialog::GotoDialog(QWidget *parent, const QList<QString> &choices)
    : ActionDialog(parent)
{
    setCaption(i18n("Goto Cell"));
    setObjectName(QLatin1String("GotoDialog"));
    setButtonText(Apply, i18n("Goto Cell"));

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    QLabel *label = new QLabel(i18n("Enter cell:"), page);
    lay1->addWidget(label);

    m_nameCell = new QComboBox(page);
    lay1->addWidget(m_nameCell);

    m_nameCell->addItems(choices);
    m_nameCell->setCurrentText(QString());
    m_nameCell->setEditable(true);

    connect(m_nameCell, &QComboBox::editTextChanged, this, &GotoDialog::textChanged);

    resize(QSize(320, 50).expandedTo(minimumSizeHint()));
}

void GotoDialog::textChanged(const QString &_text)
{
    enableButtonApply(!_text.isEmpty());
}

QWidget *GotoDialog::defaultWidget()
{
    return m_nameCell;
}

void GotoDialog::onApply()
{
    QString region = m_nameCell->currentText();
    Q_EMIT gotoCell(region);
    m_nameCell->lineEdit()->selectAll();
    m_nameCell->setFocus();
}
