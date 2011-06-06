/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
 *
 * Contact: Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "ConfirmationDialog.h"

#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

#include <KLocale>

ConfirmationDialog::ConfirmationDialog(QWidget * parent)
 : QDialog(parent)
{
    m_yes = new QPushButton(i18n("Save"),this);
    Q_CHECK_PTR(m_yes);
    m_no = new QPushButton(i18n("Discard"),this);
    Q_CHECK_PTR(m_no);
    m_cancel = new QPushButton(i18n("Cancel"),this);
    Q_CHECK_PTR(m_cancel);
    QLabel *message = new QLabel(i18n("Document is modified. Do you want to save it before closing?"),this);
    Q_CHECK_PTR(message);

    QGridLayout *confirmationdialoglayout = new QGridLayout(this);
    Q_CHECK_PTR(confirmationdialoglayout);
    confirmationdialoglayout->addWidget(message,0,0,1,3,Qt::AlignCenter);
    confirmationdialoglayout->addWidget(m_yes,1,0);
    confirmationdialoglayout->addWidget(m_no,1,1);
    confirmationdialoglayout->addWidget(m_cancel,1,2);

    connect(m_no, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(m_yes, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(m_cancel, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

ConfirmationDialog::~ConfirmationDialog()
{
}

void ConfirmationDialog::buttonClicked()
{
    if (sender() == m_yes) {
        done(QMessageBox::Save);
    }
    else if (sender() == m_no) {
        done(QMessageBox::Discard);
    }
    else if (sender() == m_cancel) {
        done(QMessageBox::Cancel);
    }
}
