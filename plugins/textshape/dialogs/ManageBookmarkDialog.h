/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2013 Aman Madaan <madaan.amanmadaan@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef MANAGEBOOKMARKDIALOG_H
#define MANAGEBOOKMARKDIALOG_H
#include <KoTextEditor.h>
#include <ui_ManageBookmark.h>

#include <KoDialog.h>
#include <QWidget>

class ManageBookmark : public QWidget
{
    Q_OBJECT
public:
    explicit ManageBookmark(const QList<QString> &nameList, KoTextEditor *editor, QWidget *parent = 0);
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
    void slotBookmarkInsert();
    void slotBookmarkItemActivated(QListWidgetItem *item);

private:
    Ui::ManageBookmark widget;
    KoTextEditor *m_editor;
};

class ManageBookmarkDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit ManageBookmarkDialog(const QList<QString> &nameList, KoTextEditor *editor, QWidget *parent = 0);
    QString selectedBookmarkName();

Q_SIGNALS:
    void nameChanged(const QString &oldName, const QString &newName);
    void bookmarkDeleted(const QString &deletedName);

private Q_SLOTS:
    void selectionChanged(int currentRow);
    void bookmarkDoubleClicked(QListWidgetItem *item);

private:
    ManageBookmark *ui;
};

#endif
