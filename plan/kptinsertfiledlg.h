/* This file is part of the KDE project
   Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTINSERTFILEDIALOG_H
#define KPTINSERTFILEDIALOG_H

#include "ui_kptinsertfilepanel.h"

#include <kdialog.h>

#include <QWidget>

class KoView;

namespace KPlato
{

class Project;
class Node;
class InsertFilePanel;

class InsertFileDialog : public KDialog
{
    Q_OBJECT
public:
    InsertFileDialog( Project &project, Node *currentNode, QWidget *parent=0 );

    KUrl url() const;
    Node *parentNode() const;
    Node *afterNode() const;

private:
    InsertFilePanel *m_panel;
};


class InsertFilePanel : public QWidget
{
    Q_OBJECT
public:
    InsertFilePanel( Project &project, Node *currentNode, QWidget *parent );

    KUrl url() const;
    Node *parentNode() const;
    Node *afterNode() const;

    Ui::InsertFilePanel ui;

signals:
    void enableButtonOk( bool );

protected slots:
    void changed( const QString& );

    void slotOpenFileDialog( KUrlRequester * );

private:
    Project &m_project;
    Node *m_node;
};


} //KPlato namespace

#endif
