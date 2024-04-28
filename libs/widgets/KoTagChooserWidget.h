/*
 *    This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *    SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *    SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *    SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTAGCHOOSERWIDGET_H
#define KOTAGCHOOSERWIDGET_H

#include "kowidgets_export.h"
#include <QWidget>

class KOWIDGETS_EXPORT KoTagChooserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KoTagChooserWidget(QWidget *parent);
    ~KoTagChooserWidget() override;
    void setCurrentIndex(int index);
    int findIndexOf(const QString &tagName);
    void insertItem(const QString &tagName);
    void insertItemAt(int index, const QString &tag);
    QString currentlySelectedTag();
    QStringList allTags();
    bool selectedTagIsReadOnly();
    void removeItem(const QString &item);
    void addItems(QStringList tagNames);
    void addReadOnlyItem(const QString &tagName);
    void clear();
    void setUndeletionCandidate(const QString &tag);
    void showTagToolButton(bool show);

Q_SIGNALS:
    void newTagRequested(const QString &tagname);
    void tagDeletionRequested(const QString &tagname);
    void tagRenamingRequested(const QString &oldTagname, const QString &newTagname);
    void tagUndeletionRequested(const QString &tagname);
    void tagUndeletionListPurgeRequested();
    void popupMenuAboutToShow();
    void tagChosen(const QString &tag);

private Q_SLOTS:
    void slotTagRenamingRequested(const QString &newName);
    void tagOptionsContextMenuAboutToShow();
    void contextDeleteCurrentTag();

private:
    /// pimpl because chooser will most likely get upgraded at some point
    class Private;
    Private *const d;
};
;

#endif // KOTAGCHOOSERWIDGET_H
