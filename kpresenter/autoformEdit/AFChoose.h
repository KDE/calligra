// -*- Mode: c++-mode; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef AFCHOOSE_H
#define AFCHOOSE_H

#include <qtabdialog.h>
#include <qptrlist.h>
#include <QMap>
#include <qfileinfo.h>

class QLabel;
class KIconCanvas;

class AFChoose : public QTabDialog
{
    Q_OBJECT

public:

    // constructor - destructor
    AFChoose(QWidget *parent=0, const QString &caption=QString::null, const char *name=0);
    ~AFChoose();

private:

    // structure of a group
    struct Group
    {
        QFileInfo dir;
        QString name;
        QWidget *tab;
        KIconCanvas *loadWid;
        QLabel *label;
        QMap<QString, QString> entries;
    };

    // set groups
    void getGroups();

    // setup tabs
    void setupTabs();

    // ********** variables **********

    // list of groups and a pointer to a group
    QPtrList<Group> groupList;
    Group *grpPtr;

private slots:

    // name changed
    void nameChanged(QString);
    void tabChanged(QWidget *);

    // autoform chosen
    void chosen();

    void cancelClicked();

    void slotDoubleClick();
protected:
    void closeEvent ( QCloseEvent * );

signals:

    //autoform chosen
    void formChosen(const QString &);
    void afchooseCanceled();
};

#endif //AFCHOOSE_H
