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

#ifndef NOTESDIALOG_H
#define NOTESDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QTextEdit>
#include <KoDocument.h>
#include <KoPADocument.h>

class QToolButton;
class QScrollArea;

class NotesDialog : public QDialog
{
    Q_OBJECT

public:
    NotesDialog(KoDocument *m_doc,int viewNumber,QList <QPixmap> thumbnailList,QWidget *parent = 0);

public:
        QGridLayout *gridLayout;
        /*!
         * Pointer to preview of the current slide
         */
        QLabel *slidePreview;
        /*!
         * Pointer to editor showing the notes of the current slide
         */
        QTextEdit *pageNotesTextEdit;
        /*!
         * Pointer to next button
         */
        QPushButton *pushNext;
        /*!
         * Pointer to previous button
         */
        QPushButton *pushPrev;
        int currentPage;
        KoDocument *m_doc;
        int viewNumber;
        QList <QPixmap> thumbnailList;
        QScrollArea *scrollArea;
        QWidget *scrollAreaWidgetContents;
        QVBoxLayout *verticalLayout;
        QList <QToolButton *> previewButtonList;
        QLabel *currentSlideNumber;
public slots:
        /*!
          * functions which gets and shows the notes for a particular page number
          */
        void showNotesDialog(int page);
        /*!
         * slot which handles the click of the next side button
         */
        void preButtonClicked();
        /*!
         * slot which handles the click of the previous side button
         */
        void nextButtonClicked();
        void previewClicked();
signals:
        /*!
          * signal emitted when slide is changes which is caught in the mainwindow to update the change in slide number
          */
        void moveSlide(bool flag);
        /*!
         * This signal is emitted whenever the thumbnail is clicked
         */
         void gotoPage(int page);
};

#endif // NOTESDIALOG_H
