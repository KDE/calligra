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

#ifndef KWCREATEBOOKMARKDIALOG_H
#define KWCREATEBOOKMARKDIALOG_H

#include <ui_KWCreateBookmark.h>

#include <QStringList>
#include <KoDialog.h>

class KWCreateBookmark : public QWidget
{
    Q_OBJECT
public:
    KWCreateBookmark(const QStringList &nameList, const QString &suggestedName, QWidget *parent = 0);
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
    KWCreateBookmarkDialog(const QStringList &nameList, const QString &suggestedName, QWidget *parent = 0);
    QString newBookmarkName();

private Q_SLOTS:
    void nameChanged(const QString &name);

private:
    KWCreateBookmark *ui;
    QStringList m_nameList;
};

#endif

