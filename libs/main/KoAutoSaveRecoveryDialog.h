/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Boudewijn Rempt <boud@valdyas.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KOAUTOSAVERECOVERYDIALOG_H
#define KOAUTOSAVERECOVERYDIALOG_H

#include <KoDialog.h>
#include <QModelIndex>
#include <QStringList>

class QListView;
struct FileItem;

Q_DECLARE_METATYPE(QModelIndex)

class KoAutoSaveRecoveryDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit KoAutoSaveRecoveryDialog(const QStringList &filenames, QWidget *parent = nullptr);

    QStringList recoverableFiles();
    void slotDeleteAll();

private:
    QListView *m_listView;

    class FileItemModel;
    FileItemModel *m_model;
};

#endif // KOAUTOSAVERECOVERYDIALOG_H
