/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
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

#ifndef KWSELECTBOOKMARKDIALOG_H
#define KWSELECTBOOKMARKDIALOG_H

#include <ui_KWSelectBookmark.h>

#include <KoDialog.h>

class KWSelectBookmark : public QWidget
{
    Q_OBJECT
public:
    explicit KWSelectBookmark(const QStringList &nameList, QWidget *parent = 0);
    QString bookmarkName() const;
    int bookmarkRow() const;

Q_SIGNALS:
    void bookmarkSelectionChanged(int currentRow);
    void bookmarkNameChanged(const QString &oldName, const QString &newName);
    void bookmarkItemDeleted(const QString &deletedName);
    void bookmarkItemDoubleClicked(QListWidgetItem *item);

private Q_SLOTS:
    void selectionChanged(int currentRow);
    void slotBookmarkRename();
    void slotBookmarkDelete();
    void slotBookmarkItemActivated(QListWidgetItem *item);

private:
    Ui::SelectBookmark widget;
    QWidget *parentWidget;
};

class KWSelectBookmarkDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit KWSelectBookmarkDialog(const QStringList &nameList, QWidget *parent = 0);
    QString selectedBookmarkName();

Q_SIGNALS:
    void nameChanged(const QString &oldName, const QString &newName);
    void bookmarkDeleted(const QString &deletedName);

private Q_SLOTS:
    void selectionChanged(int currentRow);
    void bookmarkDoubleClicked(QListWidgetItem *item);

private:
    KWSelectBookmark *ui;
};

#endif

