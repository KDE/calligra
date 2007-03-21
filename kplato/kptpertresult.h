/* This file is part of the KDE project
  Copyright (C) 2007 Florian Piquemal <flotueur@yahoo.fr>
  Copyright (C) 2007 Alexis Ménard <darktears31@gmail.com>

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

#ifndef KPTPERTRESULT_H
#define KPTPERTRESULT_H

#include <kptviewbase.h>
#include <kptitemmodelbase.h>

#include <QList>
#include <QHeaderView>
#include <QObject>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QTreeWidget>

#include <QtGui>




#include "kptpart.h"
#include "kpttask.h"

#include <ui_kptpertresult.h>

#include "kptnode.h"


#include <kdebug.h>

class QTreeWidgetItem;

namespace KPlato
{

class View;
class Project;

class PertResult : public ViewBase
{
    Q_OBJECT
public:

    PertResult( Part *part, QWidget *parent );
    void draw( Project &project);

    QList<Node*> criticalPath();

    DateTime getStartEarlyDate(Node * currentNode);
    DateTime getFinishEarlyDate(Node * currentNode);
    DateTime getStartLateDate(Node * currentNode);
    DateTime getFinishLateDate(Node * currentNode);
    Duration getProjectFloat(Project &project);
    Duration getFreeMargin(Node * currentNode);
    Duration getTaskFloat(Node * currentNode);


private:
    Ui::PertResult widget;
    Node * m_node;
    Part * m_part;
    QList<Node *> m_criticalPath;
};

}  //KPlato namespace

#endif
