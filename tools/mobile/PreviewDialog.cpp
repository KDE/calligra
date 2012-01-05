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
#ifndef Q_OS_ANDROID
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#endif

#include "FlowLayout.h"
#include "PreviewDialog.h"

PreviewWindow::PreviewWindow(KoDocument *doc, KoView *view, int currentPage, QList <QPixmap> *thumbnailList)
{
    m_doc = doc;
    m_view = view;
    m_currentPage = currentPage;
    m_thumbnailList = thumbnailList;
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
    resize(420,320);
    show();
    moveFlag=false;
    toBeMovedPage=-1;
    for(int i=0;i<m_thumbnailList->count();i++)
    {
        showThumbnail();
    }
    QPushButton *insertSlide = new QPushButton("New Slide");
    gridLayout->addWidget(insertSlide);
    connect(insertSlide, SIGNAL(clicked()), this, SLOT(newSlide()));
}

void PreviewWindow::screenThumbnailClicked()
{
    PreviewButton *clickedButton = qobject_cast<PreviewButton *>(sender());
    QString buttonName = clickedButton->objectName();
    int index=clickedButton->getSlideNumber();

    if(!moveFlag || index==toBeMovedPage)
    {
      emit goToPage(clickedButton->getSlideNumber()+1);
      hide();
    }
    else
    {
        KoPADocument* padoc = qobject_cast<KoPADocument*>(m_doc);
        KoPAPageBase* page = padoc->pageByIndex(toBeMovedPage, false);

        padoc->takePage(page);

        if ( !page )
            return;

        padoc->insertPage((KoPAPageBase*)  page ,index);

        QList<KoPAPageBase*> pages = padoc->pages();

        pages.insert( index, (KoPAPageBase *)page );

        accept();

        m_thumbnailList->move(toBeMovedPage,index);


        if(m_currentPage==m_doc->pageCount()) {
            emit goToPage(m_currentPage-1);
            emit goToPage(m_currentPage);
        }
        else {
        emit goToPage(m_currentPage+1);
        emit goToPage(m_currentPage);
      }
    }
}

void PreviewWindow::showThumbnail()
{
    PreviewButton *previewButton;

    if(previewCount<9)
        previewButton=new PreviewButton(
            m_thumbnailList->at(previewCount),QString("0").append(QString::number(previewCount+1)),this);
    else
        previewButton=new PreviewButton(m_thumbnailList->at(previewCount),QString::number(previewCount+1),this);

        previewButton->setFlat(true);

        previewButton->setMinimumSize(QSize(80, 70));
        previewButton->setIconSize(QSize(100, 70));
        flowLayout->addWidget(previewButton);
        connect(previewButton,SIGNAL(clicked()),this,SLOT(screenThumbnailClicked()));
        connect(previewButton,SIGNAL(newSlide()),this,SLOT(newSlide()));
        connect(previewButton,SIGNAL(moveSlide()),this,SLOT(moveSlide()));
        connect(previewButton,SIGNAL(deleteSlide()),this,SLOT(deleteSlide()));
        previewButton->setSlideNumber(previewCount);
        previewCount++;
}

void PreviewWindow::addThumbnail(QPixmap pix)
{
    PreviewButton *previewButton;

    if(previewCount<9)
        previewButton=new PreviewButton(pix,QString("0").append(QString::number(previewCount+1)),this);
    else
        previewButton=new PreviewButton(pix,QString::number(previewCount+1),this);

        previewButton->setFlat(true);

        previewButton->setMinimumSize(QSize(80, 70));
        previewButton->setIconSize(QSize(100, 70));
        flowLayout->addWidget(previewButton);
        connect(previewButton,SIGNAL(clicked()),this,SLOT(screenThumbnailClicked()));
        connect(previewButton,SIGNAL(newSlide()),this,SLOT(newSlide()));
        connect(previewButton,SIGNAL(moveSlide()),this,SLOT(moveSlide()));
        connect(previewButton,SIGNAL(deleteSlide()),this,SLOT(deleteSlide()));
        previewButton->setSlideNumber(previewCount);
        previewCount++;
}

void PreviewWindow::moveSlide()
{
    moveFlag=true;
    PreviewButton *clickedButton = qobject_cast<PreviewButton *>(sender());
    clickedButton->setCheckable(true);
    clickedButton->setChecked(true);
    toBeMovedPage=clickedButton->getSlideNumber();
}

void PreviewWindow::deleteSlide()
{
    if(m_doc->pageCount()==1)
        return;

    PreviewButton *clickedButton = qobject_cast<PreviewButton *>(sender());

    int index=clickedButton->getSlideNumber();
    KoPADocument* padoc = qobject_cast<KoPADocument*>(m_doc);
    KoPAPageBase* page = padoc->pageByIndex(index, false);

    padoc->takePage(page);
    previewScreenButton.removeAt(index);
    m_thumbnailList->removeAt(index);
    flowLayout->removeWidget(clickedButton);
    flowLayout->update();
    repaint();
    accept();
    repaint();
    if(m_currentPage==m_doc->pageCount()) {
        emit goToPage(m_currentPage-1);
        emit goToPage(m_currentPage);
    }
    else {
    emit goToPage(m_currentPage+1);
    emit goToPage(m_currentPage);
  }
}

void PreviewWindow::newSlide()
{
    PreviewButton *clickedButton = qobject_cast<PreviewButton *>(sender());
    int index=clickedButton->getSlideNumber()+1;

    emit goToPage(clickedButton->getSlideNumber()+1);

    if (m_view) {
        QAction* action = ((KXMLGUIClient*)m_view)->action("page_insertpage");
        if (action) {
            action->activate(QAction::Trigger);
        }
    }

    KoPADocument* padoc = qobject_cast<KoPADocument*>(m_doc);
    KoPAPageBase* papage = padoc->pageByIndex(index, false);
    m_thumbnailList->insert(index,papage->thumbnail());

    accept();
}

StoreButtonPreview::StoreButtonPreview(KoDocument *doc, KoView *view, QWidget *parent)
    : QWidget(parent),
    m_doc(doc),
    m_view(view),
    m_previewWindow(0),
    m_isPreviewDialogActive(false),
    m_value(0)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(addThumbnail()));
    //timer->setSingleShot(true);
    m_timer.start(100);
}

void StoreButtonPreview::showDialog(int currentPage)
{
    if (m_isPreviewDialogActive)
        return;
    m_isPreviewDialogActive = true;
    m_previewWindow = new PreviewWindow(m_doc, m_view, currentPage, &m_thumbnailList);
    connect(m_previewWindow, SIGNAL(goToPage(int)), this, SIGNAL(goToPage(int)));
    m_previewWindow->exec();
    m_isPreviewDialogActive = false;
}

void StoreButtonPreview::addThumbnail()
{
    if (!m_doc)
        return;

    KoPADocument* padoc = qobject_cast<KoPADocument*>(m_doc);
    KoPAPageBase* papage = padoc->pageByIndex(m_value, false);
    if (!papage)
        return;
    m_thumbnailList.append(papage->thumbnail());
    if (m_previewWindow)
        m_previewWindow->addThumbnail(m_thumbnailList.last());

    if (m_value < m_doc->pageCount()) {
        m_value++;
        m_timer.start(10);
    }
    if (m_value == m_doc->pageCount()){
        m_timer.stop();
        disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(addThumbnail()));
        m_value = 0;
    }
}

