/* This file is part of the KDE project
  Copyright (C) 2006 - 2007 Dag Andersen <kplato@kde.org>

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

#ifndef KPTDOCUMENTSEDITOR_H
#define KPTDOCUMENTSEDITOR_H

#include "kplatoui_export.h"

#include <kptviewbase.h>
#include <kptitemmodelbase.h>
#include <kptdocumentmodel.h>



class QPoint;

class KoDocument;

namespace KPlato
{

class KPLATOUI_EXPORT DocumentTreeView : public TreeViewBase
{
    Q_OBJECT
public:
    DocumentTreeView( QWidget *parent );

    DocumentItemModel *model() const { return static_cast<DocumentItemModel*>( TreeViewBase::model() ); }

    Documents *documents() const { return model()->documents(); }
    void setDocuments( Documents *docs ) { model()->setDocuments( docs ); }

    Document *currentDocument() const;
    QList<Document*> selectedDocuments() const;
    
    QModelIndexList selectedRows() const;
    
    using QTreeView::selectionChanged;
signals:
    void selectionChanged( const QModelIndexList& );
    
protected slots:
    void slotSelectionChanged( const QItemSelection &selected );
};

class KPLATOUI_EXPORT DocumentsEditor : public ViewBase
{
    Q_OBJECT
public:
    DocumentsEditor( KoDocument *part, QWidget *parent );
    
    void setupGui();
    using ViewBase::draw;
    virtual void draw( Documents &docs );
    virtual void draw();

    DocumentItemModel *model() const { return m_view->model(); }
    
    virtual void updateReadWrite( bool readwrite );

    virtual Document *currentDocument() const;
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
    DocumentTreeView *view() const { return m_view; }
    
signals:
    void addDocument();
    void deleteDocumentList( QList<Document*> );
    void editDocument( Document *doc );
    void viewDocument( Document *doc );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected slots:
    virtual void slotOptions();

protected:
    void updateActionsEnabled(  bool on = true );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( bool on );

    void slotEditDocument();
    void slotViewDocument();
    void slotAddDocument();
    void slotDeleteSelection();

private:
    DocumentTreeView *m_view;

    KAction *actionEditDocument;
    KAction *actionViewDocument;
    KAction *actionAddDocument;
    KAction *actionDeleteSelection;

};

}  //KPlato namespace

#endif
