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

#ifndef KPTVIEWLISTCOMMAND_H
#define KPTVIEWLISTCOMMAND_H

#include "kplato_export.h"
#include "kptcommand.h"

class QString;
class QTreeWidgetItem;
/**
 * @file
 * This file includes undo/redo commands for embedding documents
 */

/// The main namespace
namespace KPlato
{

class ViewListWidget;
class ViewListItem;

class KPLATO_EXPORT InsertEmbeddedDocumentCmd : public NamedCommand
{
public:
    InsertEmbeddedDocumentCmd( ViewListWidget *list, ViewListItem *item, QTreeWidgetItem *parent, const QString& name = QString() );
    ~InsertEmbeddedDocumentCmd();
    void execute();
    void unexecute();
private:
    ViewListWidget *m_list;
    QTreeWidgetItem *m_parent;
    ViewListItem *m_item;
    int m_index;
    bool m_mine;
};

class KPLATO_EXPORT DeleteEmbeddedDocumentCmd : public NamedCommand
{
public:
    DeleteEmbeddedDocumentCmd( ViewListWidget *list, ViewListItem *item, const QString& name = QString() );
    ~DeleteEmbeddedDocumentCmd();
    void execute();
    void unexecute();
private:
    ViewListWidget *m_list;
    QTreeWidgetItem *m_parent;
    ViewListItem *m_item;
    int m_index;
    bool m_mine;
};


}  //KPlato namespace

#endif
