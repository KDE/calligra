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

#ifndef KPLATO_DOCUMENTSPANEL_H
#define KPLATO_DOCUMENTSPANEL_H

#include "kplatoui_export.h"

#include "ui_kptdocumentspanel.h"

#include "kptdocuments.h"

#include <QWidget>
#include <kundo2qstack.h>
#include <QModelIndex>

namespace KPlato
{

class Node;
class DocumentItemModel;
class MacroCommand;
class DocumentTreeView;

class KPLATOUI_EXPORT DocumentsPanel : public QWidget
{
    Q_OBJECT
public:
    explicit DocumentsPanel( Node &node, QWidget *parent = 0 );
    ~DocumentsPanel() {}
    
    MacroCommand *buildCommand();
    
    Ui::DocumentsPanel widget;

    DocumentItemModel* model() const;
    Document *selectedDocument() const;
    
signals:
    void changed();
    
protected slots:
    void slotAddUrl();
    void slotChangeUrl();
    void slotRemoveUrl();
    void slotViewUrl();
    
    void dataChanged( const QModelIndex& );
    
    void slotSelectionChanged( const QModelIndexList& );
    void currentChanged( const QModelIndex &index );
    
private:
    Node &m_node;
    Documents m_docs;
    enum State { Unmodified = 1, Modified = 2, Added = 4, Removed = 8 };
    QMap<Document*, State> m_state;
    QMap<Document*, KUrl> m_orgurl;
    KUndo2QStack m_cmds;
    DocumentTreeView *m_view;
};

}

#endif
