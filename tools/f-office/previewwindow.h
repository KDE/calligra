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

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QScrollArea>
#include "flowlayout.h"

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class PreviewWindow : public QDialog
{
    Q_OBJECT

public:
    PreviewWindow();
public:
   /*!
    * Preview images of the slides will be shown on the pushbutton
    */
    QPushButton *previewScreen[100];
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
   /*!
    * Its a Layout created for showing the previews in a sequential manner
    */
    FlowLayout *flowLayout;
    QGridLayout *gridLayout;
   /*!
    * stores the number of preview its showing in the
    */
    int previewCount;
public slots:
   /*!
    * slot to service the click of any thumbnail
    */
    void screenThumbnailClicked();
   /*!
    * it adds thumbnail and shows it in the scroll area
    */
    void showThumbnail(QPixmap pageThumbnail);
signals:
   /*!
    * This signal is emitted whenever the thumbnail is clicked
    */
    void gotoPage(int page);
};

#endif
