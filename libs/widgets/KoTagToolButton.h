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

#ifndef KOTAGTOOLBUTTON_H
#define KOTAGTOOLBUTTON_H

#include <QWidget>

class KoTagToolButton : public QWidget
{
    Q_OBJECT

private:
    explicit KoTagToolButton(QWidget *parent = nullptr);
    ~KoTagToolButton() override;
    void readOnlyMode(bool activate);
    void setUndeletionCandidate(const QString &deletedTagName);

Q_SIGNALS:
    void newTagRequested(const QString &tagname);
    void renamingOfCurrentTagRequested(const QString &tagname);
    void deletionOfCurrentTagRequested();
    void undeletionOfTagRequested(const QString &tagname);
    void purgingOfTagUndeleteListRequested();
    void popupMenuAboutToShow();

private Q_SLOTS:
    void onTagUndeleteClicked();

private:
    class Private;
    Private *const d;
    friend class KoTagChooserWidget;
};

#endif // KOTAGTOOLBUTTON_H
