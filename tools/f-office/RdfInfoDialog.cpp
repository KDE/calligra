/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sugnan Prabhu S <sugnan.prabhu@gmail.com>
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
 */

#include "RdfInfoDialog.h"
#include <Soprano/Soprano>
#include <Soprano/Statement>
#include <Soprano/Node>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDesktopServices>
#include <QDebug>
#include <QTreeWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QProcess>
#include <KoStore.h>
Soprano::Model *ram_model;

RdfInfoDialog::RdfInfoDialog(QWidget *parent) :
    QDialog(parent)
{
    this->resize(423, 123);
    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);

    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);

    gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

    this->setWindowTitle("RDF Information");
}

void RdfInfoDialog::setData(struct Details details)
{
    this->details.homePage=details.homePage;
    this->details.phone=details.phone;

    if(!details.name.isEmpty()){
        QString name=details.name;
        if(!details.nick.isEmpty())
            name+=" ("+details.nick+")";
        QPushButton *tempButton=new QPushButton(name);
        tempButton->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_default_avatar.png"));
        tempButton->setObjectName(QString::fromUtf8("nameButton"));
        detailsButton.append(tempButton);
        verticalLayout->addWidget(tempButton);
    }

    if(!details.phone.isEmpty()){
        QPushButton *tempButton=new QPushButton(details.phone);
        tempButton->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_call.png"));
        tempButton->setObjectName(QString::fromUtf8("phoneButton"));
        detailsButton.append(tempButton);
        verticalLayout->addWidget(tempButton);
        connect(tempButton,SIGNAL(clicked()),this,SLOT(doSomething()));
    }

    if(!details.homePage.isEmpty()){
        QPushButton *tempButton=new QPushButton(details.homePage);
        tempButton->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_web.png"));
        tempButton->setObjectName(QString::fromUtf8("homepageButton"));
        detailsButton.append(tempButton);
        verticalLayout->addWidget(tempButton);
        connect(tempButton,SIGNAL(clicked()),this,SLOT(doSomething()));
    }

    if(!details.location.isEmpty()){
        QPushButton *tempButton=new QPushButton(details.location);
        tempButton->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/tasklaunch_map.png"));
        tempButton->setObjectName(QString::fromUtf8("locationButton"));
        detailsButton.append(tempButton);
        verticalLayout->addWidget(tempButton);
    }

    if(!details.summary.isEmpty()){
        QPushButton *tempButton=new QPushButton(details.summary);
        tempButton->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_notes.png"));
        tempButton->setObjectName(QString::fromUtf8("summaryButton"));
        detailsButton.append(tempButton);
        verticalLayout->addWidget(tempButton);
    }

    if(!details.taskStart.isEmpty()){
        details.taskStart=details.taskStart.replace("T","  ");
        QPushButton *tempButton=new QPushButton(details.taskStart);
        tempButton->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_clock.png"));
        tempButton->setObjectName(QString::fromUtf8("taskStartButton"));
        detailsButton.append(tempButton);
        verticalLayout->addWidget(tempButton);
    }

    if(!details.taskEnd.isEmpty()){
        details.taskEnd=details.taskEnd.replace("T","  ");
        QPushButton *tempButton=new QPushButton(details.taskEnd);
        tempButton->setIcon(QIcon("/usr/share/icons/hicolor/64x64/hildon/general_clock.png"));
        tempButton->setObjectName(QString::fromUtf8("taskEndButton"));
        detailsButton.append(tempButton);
        verticalLayout->addWidget(tempButton);
    }
}

void RdfInfoDialog::doSomething()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if(clickedButton->objectName()=="phoneButton"){

        QRegExp reg("[ ]+");
        QString number=details.phone;
        number=number.replace(reg,"");
        reg.setPattern("[^+ * # 0-9]");
        number=number.replace(reg,"");
        details.phone=number;

        QDBusConnection bus = QDBusConnection::sessionBus();
        QDBusInterface *interface = new QDBusInterface("com.nokia.csd.Call", "/com/nokia/csd/call", "com.nokia.csd.Call");
        interface->call("CreateWith", this->details.phone, uint(0));
        this->hide();
        QProcess::startDetached(QString("dbus-send --system --dest=com.nokia.csd.Call --type=method_call --print-reply /com/nokia/csd/call com.nokia.csd.Call.CreateWith string:\""+details.phone+"\" uint32:0").toAscii().data());
        this->accept();
    }
    if(clickedButton->objectName()=="homepageButton"){
        this->hide();
        QDesktopServices::openUrl(details.homePage);
        this->accept();
    }
}

