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

#include <QtGui>
#include <KoPAPageBase.h>
#include <KoDocument.h>

#include <KoView.h>
#include <KoCanvasBase.h>
#include <KoDocumentInfo.h>
#include <kdemacros.h>
#include <KoCanvasController.h>
#include <KoZoomMode.h>
#include <KoZoomController.h>
#include <KoToolProxy.h>
#include <KoToolBase.h>
#include <KoResourceManager.h>
#include <KoToolManager.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoShapeUserData.h>
#include <KoTextShapeData.h>
#include <KoSelection.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAView.h>
#include <QProgressDialog>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

#include "flowlayout.h"
#include "previewwindow.h"

PreviewWindow::PreviewWindow(KoDocument *m_doc,KoView *m_view,int m_currentPage,QList <QPixmap> *thumbnailList)
{
    this->m_doc=m_doc;
    this->m_view=m_view;
    this->m_currentPage=m_currentPage;
    this->thumbnailList=thumbnailList;
        gridLayout = new QGridLayout(this);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        scrollArea = new QScrollArea(this);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 420,320));

        flowLayout = new FlowLayout(scrollAreaWidgetContents);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
        previewCount=0;

        scrollArea->resize(420,320);
        this->resize(420,320);
        this->show();
        moveFlag=false;
        toBeMovedPage=-1;
for(int i=0;i<thumbnailList->count();i++)
{
    showThumbnail();
}
}

void PreviewWindow::screenThumbnailClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString buttonName = clickedButton->objectName();
    int index=(clickedButton->text()).toInt();

    emit gotoPage(index);
    hide();
}

void PreviewWindow::showThumbnail()
{
    QPushButton *previewButton;

    if(previewCount<9)
        previewButton=new QPushButton(thumbnailList->at(previewCount),QString("0").append(QString::number(previewCount+1)),this);
    else
        previewButton=new QPushButton(thumbnailList->at(previewCount),QString::number(previewCount+1),this);

        previewButton->setFlat(true);

        previewButton->setMinimumSize(QSize(80, 70));
        previewButton->setIconSize(QSize(100, 70));
        flowLayout->addWidget(previewButton);
        connect(previewButton,SIGNAL(clicked()),this,SLOT(screenThumbnailClicked()));

        previewCount++;
}

void PreviewWindow::addThumbnail(QPixmap pix)
{
    QPushButton *previewButton;

    if(previewCount<9)
        previewButton=new QPushButton(pix,QString("0").append(QString::number(previewCount+1)),this);
    else
        previewButton=new QPushButton(pix,QString::number(previewCount+1),this);

        previewButton->setFlat(true);

        previewButton->setMinimumSize(QSize(80, 70));
        previewButton->setIconSize(QSize(100, 70));
        flowLayout->addWidget(previewButton);
        connect(previewButton,SIGNAL(clicked()),this,SLOT(screenThumbnailClicked()));

        previewCount++;
}

StoreButtonPreview::StoreButtonPreview(KoDocument *m_doc,KoView *m_view,QObject *parent) :
    QObject(parent),
    previewWindow(NULL)
{
    this->m_doc=m_doc;
    this->m_view=m_view;
    isPreviewDialogActive=false;
}

void StoreButtonPreview::showDialog(int m_currentPage)
{
    if(isPreviewDialogActive)
        return;
    isPreviewDialogActive=true;
    previewWindow=new PreviewWindow(m_doc,m_view,m_currentPage,&thumbnailList);
    QObject::connect(previewWindow,SIGNAL(gotoPage(int)),this,SIGNAL(gotoPage(int)));
    previewWindow->exec();
    isPreviewDialogActive=false;
}
void StoreButtonPreview::addThumbnail(long pageNumber)
{
    thumbnailList.append(QPixmap("/tmp/FreOfficeThumbnail/"+QString::number(pageNumber)+".png"));
    if(previewWindow!=NULL)
        previewWindow->addThumbnail(QPixmap("/tmp/FreOfficeThumbnail/"+QString::number(pageNumber)+".png"));
}


ThumbnailRetriever::ThumbnailRetriever(long pageCount,int viewNumber,QObject *parent) :
    QThread(parent)
{
    this->pageCount=pageCount;
    this->viewNumber=viewNumber;
}

void ThumbnailRetriever::run()
{
    QDir temp("/tmp/FreOfficeThumbnail");
    temp.mkdir("/tmp/FreOfficeThumbnail");
    int pageNumber=0;
    while(pageNumber<pageCount) {
        QDBusConnection bus = QDBusConnection::sessionBus();

        QDBusInterface *interface = new QDBusInterface("com.nokia.FreOffice", "/view_"+QString::number(viewNumber), "org.kde.koffice.presentation.view");

        QString m_notesHtml = (QDBusReply<QString>)interface->call("exportPageThumbnail", pageNumber,128,128,"/tmp/FreOfficeThumbnail/"+QString::number(pageNumber)+".png","PNG",-1);
        newThumbnail(pageNumber);
        pageNumber++;
    }
}

