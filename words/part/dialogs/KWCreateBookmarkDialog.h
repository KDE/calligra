/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWCREATEBOOKMARKDIALOG_H
#define KWCREATEBOOKMARKDIALOG_H

#include <ui_KWCreateBookmark.h>

#include <KoDialog.h>
#include <QStringList>

class KWCreateBookmark : public QWidget
{
    Q_OBJECT
public:
    KWCreateBookmark(const QStringList &nameList, const QString &suggestedName, QWidget *parent = nullptr);
    QString bookmarkName();

Q_SIGNALS:
    void bookmarkNameChanged(const QString &name);

private:
    Ui::CreateBookmark widget;
};

class KWCreateBookmarkDialog : public KoDialog
{
    Q_OBJECT
public:
    KWCreateBookmarkDialog(const QStringList &nameList, const QString &suggestedName, QWidget *parent = nullptr);
    QString newBookmarkName();

private Q_SLOTS:
    void nameChanged(const QString &name);

private:
    KWCreateBookmark *ui;
    QStringList m_nameList;
};

#endif
