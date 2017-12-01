/* This file is part of the KDE project
  Copyright (C) 22017 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/


#ifndef ResourceAllocationView_h
#define ResourceAllocationView_h

#include <QTreeView>
#include <QPersistentModelIndex>

class KoDocument;

class QAction;

namespace KPlato
{

class Resource;

// class to use in resources docker
class ResourceAllocationView : public QTreeView
{
    Q_OBJECT
public:
    ResourceAllocationView(KoDocument *doc, QWidget *parent = 0);

    QList<Resource*> selectedResources() const;

public Q_SLOTS:
    void setSelectedTasks(const QItemSelection &selected, const QItemSelection &deselected);

protected Q_SLOTS:
    void slotAllocate();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    KoDocument *m_doc;
    QList<QPersistentModelIndex> m_tasks;

    QAction *m_allocateAction;
};


} // namespace KPlato

#endif
