/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWSELECTBOOKMARKDIALOG_H
#define KWSELECTBOOKMARKDIALOG_H

#include <ui_KWSelectBookmark.h>

#include <KoDialog.h>

class KWSelectBookmark : public QWidget
{
    Q_OBJECT
public:
    explicit KWSelectBookmark(const QStringList &nameList, QWidget *parent = nullptr);
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
    explicit KWSelectBookmarkDialog(const QStringList &nameList, QWidget *parent = nullptr);
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
