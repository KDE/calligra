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

#include <kptviewbase.h>
#include <kptitemmodelbase.h>
#include <kptdocumentmodel.h>

#include <QTreeWidget>

#include "kptcontext.h"

class QPoint;


namespace KPlato
{

class View;

class DocumentTreeView : public DoubleTreeViewBase
{
    Q_OBJECT
public:
    DocumentTreeView( Part *part, QWidget *parent );

    DocumentItemModel *itemModel() const { return static_cast<DocumentItemModel*>( model() ); }

    Documents *documents() const { return itemModel()->documents(); }
    void setDocuments( Documents *docs ) { itemModel()->setDocuments( docs ); }

    Document *currentDocument() const;
    QList<Document*> selectedDocuments() const;

protected slots:
    void slotActivated( const QModelIndex index );

};

class DocumentsEditor : public ViewBase
{
    Q_OBJECT
public:
    DocumentsEditor( Part *part, QWidget *parent );
    
    void setupGui();
    virtual void draw( Documents &docs );
    virtual void draw();

    virtual void updateReadWrite( bool readwrite );

    virtual Document *currentDocument() const;
    
    /// Loads context info into this view. Reimplement.
    virtual bool loadContext( const KoXmlElement &/*context*/ );
    /// Save context info from this view. Reimplement.
    virtual void saveContext( QDomElement &/*context*/ ) const;
    
signals:
    void requestPopupMenu( const QString&, const QPoint& );
    void addDocument();
    void deleteDocumentList( QList<Document*> );
    
public slots:
    /// Activate/deactivate the gui
    virtual void setGuiActive( bool activate );

protected:
    void updateActionsEnabled(  bool on = true );

private slots:
    void slotContextMenuRequested( QModelIndex index, const QPoint& pos );
    void slotHeaderContextMenuRequested( const QPoint &pos );
    void slotOptions();
    
    void slotSelectionChanged( const QModelIndexList );
    void slotCurrentChanged( const QModelIndex& );
    void slotEnableActions( bool on );

    void slotAddDocument();
    void slotDeleteSelection();

private:
    DocumentTreeView *m_view;

    KAction *actionAddDocument;
    KAction *actionDeleteSelection;

    // View options context menu
    KAction *actionOptions;
};

}  //KPlato namespace

#endif
