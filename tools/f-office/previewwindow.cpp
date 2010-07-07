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

#include "flowlayout.h"
#include "previewwindow.h"

PreviewWindow::PreviewWindow()
{
        gridLayout = new QGridLayout(this);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        scrollArea = new QScrollArea(this);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 380, 280));

        flowLayout = new FlowLayout(scrollAreaWidgetContents);

        scrollArea->setWidget(scrollAreaWidgetContents);

        gridLayout->addWidget(scrollArea, 0, 0, 1, 1);
        previewCount=0;
}

void PreviewWindow::screenThumbnailClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString buttonName = clickedButton->objectName();
    emit gotoPage(buttonName.toInt());
    hide();
}

void PreviewWindow::showThumbnail(QPixmap pageThumbnail)
{
    if(previewCount<9)
        previewScreen[previewCount]=new QPushButton(QIcon(pageThumbnail),QString("0").append(QString::number(previewCount+1)));
    else
        previewScreen[previewCount]=new QPushButton(QIcon(pageThumbnail),QString::number(previewCount+1));

        previewScreen[previewCount]->setFlat(true);

        previewScreen[previewCount]->setMinimumSize(QSize(80, 70));
        previewScreen[previewCount]->setIconSize(QSize(100, 70));
        flowLayout->addWidget(previewScreen[previewCount]);
        connect(previewScreen[previewCount],SIGNAL(clicked()),this,SLOT(screenThumbnailClicked()));

        previewScreen[previewCount]->setObjectName(QString::fromUtf8(QString::number(previewCount+1).toAscii().data()));
        previewCount++;
}
