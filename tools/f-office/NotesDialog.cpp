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

#include "NotesDialog.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QProcess>
#include <QWidget>
#include <KoPAPageBase.h>

NotesDialog::NotesDialog(KoDocument *m_doc,int viewNumber,QWidget *parent)
    : QDialog(parent)
{
    this->viewNumber=viewNumber;
        this->m_doc=m_doc;
        this->resize(486, 330);
        gridLayout = new QGridLayout(this);
        gridLayout->setSpacing(6);
        gridLayout->setMargin(11);
        slidePreview = new QLabel(this);
        //slidePreview->setObjectName(QString::fromUtf8("slidePreview"));

        pageNotesTextEdit = new QTextEdit();

        pushNext = new QPushButton();
        pushPrev = new QPushButton();
        pushNext->raise();


        pushNext->setIcon(QIcon(QPixmap("/usr/share/icons/hicolor/48x48/hildon/general_forward.png")));
        pushNext->resize(64,64);
        pushNext->setStyleSheet("background-color: black; \
                                     border: none; \
                                     border-radius: 5px;");

        pushPrev->setIcon(QIcon(QPixmap("/usr/share/icons/hicolor/48x48/hildon/general_back.png")));
        pushPrev->resize(64,64);
        pushPrev->setStyleSheet("background-color: black; \
                                     border: none; \
                                     border-radius: 5px;");


        gridLayout->addWidget(pageNotesTextEdit,0,0,4,8);
        gridLayout->addWidget(slidePreview,0,8,2,4);
        gridLayout->addWidget(pushPrev,3,8,1,2);
        gridLayout->addWidget(pushNext,3,10,1,2);

        this->setLayout(gridLayout);

        setWindowTitle(QApplication::translate("NotesDialog", "NotesDialog", 0, QApplication::UnicodeUTF8));

        connect(pushNext,SIGNAL(clicked()),this,SLOT(nextButtonClicked()));
        connect(pushPrev,SIGNAL(clicked()),this,SLOT(preButtonClicked()));
        pageNotesTextEdit->setReadOnly(true);
}

void NotesDialog::showNotesDialog(int page)
{
    if(page<0 || page>m_doc->pageCount())
        return;

    this->currentPage=page-1;
    qDebug()<<"current page="<<currentPage;
    KoPADocument* padoc = qobject_cast<KoPADocument*>(m_doc);
    KoPAPageBase* papage = padoc->pageByIndex(currentPage, false);

    slidePreview->setPixmap(papage->thumbnail().scaled(220,150));

    QDBusConnection bus = QDBusConnection::sessionBus();
qDebug()<<"view number="<<viewNumber<<"\n";
    QDBusInterface *interface = new QDBusInterface("com.nokia.FreOffice", "/view_"+QString::number(viewNumber), "org.kde.koffice.presentation.view");

    QString m_notesHtml = (QDBusReply<QString>)interface->call("pageNotes", currentPage, "html");
    pageNotesTextEdit->setHtml(m_notesHtml);
}
void NotesDialog::preButtonClicked()
{
    if(currentPage>0)
    {
        showNotesDialog(currentPage);
        emit moveSlide(false);
    }
}
void NotesDialog::nextButtonClicked()
{
    if(currentPage<m_doc->pageCount())
    {
        showNotesDialog(currentPage+2);
        emit moveSlide(true);
    }
}
