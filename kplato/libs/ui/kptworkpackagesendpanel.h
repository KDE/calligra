/* This file is part of the KDE project
   Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTWORKPACKAGESENDPANEL_H
#define KPTWORKPACKAGESENDPANEL_H

#include "kplatoui_export.h"

#include "ui_kptworkpackagesendpanel.h"

#include <QWidget>
#include <QMap>

class QPushButton;

namespace KPlato
{

class Resource;
class Node;
class TaskWorkPackageView;

class KPLATOUI_EXPORT WorkPackageSendPanel : public QWidget, public Ui_WorkPackageSendPanel
{
    Q_OBJECT
public:
    explicit WorkPackageSendPanel( const QList<Node*> &tasks, QWidget *parent=0 );

signals:
    void sendWorkpackages( QList<Node*>&, Resource* );

protected slots:
    void slotSendClicked();

protected:
    QMap<Resource*, QList<Node*> > m_resMap;
    QMap<QPushButton*, Resource*> m_pbMap;
};

} //KPlato namespace

#endif // KPTWORKPACKAGESENDPANEL_H
