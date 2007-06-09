/* This file is part of the KDE project
 * Copyright (C) 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SELECTBOOKMARKDIALOG_H
#define SELECTBOOKMARKDIALOG_H

#include "ui_SelectBookmark.h"

#include <QWidget>
#include <KDialog>

class SelectBookmark : public QWidget {
    Q_OBJECT
public:
    SelectBookmark(QList<QString> nameList, QWidget *parent = 0);
    QString bookmarkName();

signals:
    void bookmarkSelectionChanged(int currentRow);

private:
    Ui::SelectBookmark widget;
};

class SelectBookmarkDialog : public KDialog {
    Q_OBJECT
public:
    SelectBookmarkDialog(QList<QString> nameList, QWidget *parent = 0);
    QString selectedBookmarkName();

private slots:
    void selectionChanged(int currentRow);

private:
    SelectBookmark *ui;
};

#endif

