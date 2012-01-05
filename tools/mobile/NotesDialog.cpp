/*
 * This file is part of Maemo 5 Office UI for Calligra
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
#ifndef Q_OS_ANDROID
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#endif
#include <QProcess>
#include <QWidget>
#include <QToolButton>
#include <QScrollArea>

#include <KoPAPageBase.h>
#include<unistd.h>

NotesDialog::NotesDialog(KoDocument *m_doc,int viewNumber,QList <QPixmap> thumbnailList,QWidget *parent)
    : QDialog(parent)
{
    this->thumbnailList<<thumbnailList;
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


        gridLayout->addWidget(pageNotesTextEdit,0,0,4,10);

        scrollArea = new QScrollArea(this);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 20,10));

        verticalLayout = new QVBoxLayout(scrollAreaWidgetContents);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(slidePreview,0,11,1,1);
        currentSlideNumber=new QLabel();//("            Slide ");
        gridLayout->addWidget(currentSlideNumber,1,11,1,1);
        gridLayout->addWidget(scrollArea,2,11,2,2);
        for(int i=0;i<thumbnailList.count();i++)
        {
            QToolButton *button=new QToolButton();
            previewButtonList.append(button);
            button->setIcon(QIcon(thumbnailList.at(i).scaled(160,120)));
            button->setIconSize(QSize(160,120));
            verticalLayout->addWidget(button);
            button->setMinimumHeight(160);
            button->setMaximumHeight(120);
            button->setMinimumWidth(160);
            button->setMaximumWidth(120);
            connect(button,SIGNAL(clicked()),this,SLOT(previewClicked()));
        }
        slidePreview->setPixmap(thumbnailList.at(0).scaled(10,10));

        gridLayout->addWidget(pushPrev,3,11,1,2);
        gridLayout->addWidget(pushNext,3,12,1,2);

        this->setLayout(gridLayout);

        setWindowTitle(QApplication::translate("NotesDialog", "NotesDialog", 0, QApplication::UnicodeUTF8));

        connect(pushNext,SIGNAL(clicked()),this,SLOT(nextButtonClicked()));
        connect(pushPrev,SIGNAL(clicked()),this,SLOT(preButtonClicked()));
        pageNotesTextEdit->setReadOnly(true);
        this->resize(620,820);
        currentPage=0;
        slidePreview->hide();
}

void NotesDialog::showNotesDialog(int page)
{
    if(page<0 || page>m_doc->pageCount())
        return;
    if(currentPage>=0 && currentPage<m_doc->pageCount() && currentPage<thumbnailList.count()){
        previewButtonList.at(currentPage)->setChecked(false);
        previewButtonList.at(currentPage)->setCheckable(false);
    }
    this->currentPage=page-1;

    slidePreview->setPixmap(thumbnailList.at(currentPage).scaled(220,150));

#ifndef Q_OS_ANDROID
    QDBusConnection bus = QDBusConnection::sessionBus();
qDebug()<<"view number="<<viewNumber<<"\n";
    QDBusInterface *interface = new QDBusInterface("com.nokia.CalligraMobile-"+QString::number(getpid()), "/view_"+QString::number(viewNumber), "org.kde.calligra.presentation.view");

    QString m_notesHtml = (QDBusReply<QString>)interface->call("pageNotes", currentPage, "html");
    pageNotesTextEdit->setHtml(m_notesHtml);
#endif

    currentPage=page-1;
    previewButtonList.at(currentPage)->setCheckable(true);
    previewButtonList.at(currentPage)->setChecked(true);
    currentSlideNumber->setText("    Current Slide "+QString::number(currentPage+1));
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
void NotesDialog::previewClicked()
{
    QToolButton *clickedButton = qobject_cast<QToolButton *>(sender());
if(!clickedButton)
    return;
    clickedButton->setCheckable(true);
    clickedButton->setChecked(true);
    if(currentPage==previewButtonList.indexOf(clickedButton)){
        return;
    }
    previewButtonList.at(currentPage)->setChecked(false);
    previewButtonList.at(currentPage)->setCheckable(false);
    currentPage=previewButtonList.indexOf(clickedButton);
    showNotesDialog(currentPage+1);
    emit gotoPage(currentPage+1);
}
