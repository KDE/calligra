/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Arjun Asthana <arjun@iiitd.com>
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

#include "CollabDialog.h"
#include "ui_CollabDialog.h"

CollabDialog::CollabDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CollabDialog),
    server(false),
    client(false)
{
    ui->setupUi(this);
    ui->collabInfo->setVisible(false);
    ui->buttonBox->setVisible(false);

    connect(ui->serverButton, SIGNAL(clicked()), this, SLOT(serverClicked()));
    connect(ui->clientButton, SIGNAL(clicked()), this, SLOT(clientClicked()));
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
}

void CollabDialog::serverClicked() {
    server = true;

    ui->sessionType->setVisible(false);

    ui->collabInfo->setVisible(true);
    ui->buttonBox->setVisible(true);

    ui->collabAddr->setVisible(false);
    ui->collabAddrLabel->setVisible(false);
}

void CollabDialog::clientClicked() {
    client = true;

    ui->sessionType->setVisible(false);

    ui->collabInfo->setVisible(true);
    ui->buttonBox->setVisible(true);
}

void CollabDialog::buttonClicked(QAbstractButton *button) {
    if(QString::compare(button->text(), "\"Done\""))
        emit accepted();
}

QString CollabDialog::getNick() {
    return ui->collabNick->text();
}

QHostAddress CollabDialog::getAddress() {
    return QHostAddress(ui->collabAddr->text());
}

quint16 CollabDialog::getPort() {
    return ui->collabPort->text().toInt();
}

CollabDialog::~CollabDialog()
{
    delete ui;
}

void CollabDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
