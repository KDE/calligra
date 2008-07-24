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

#ifndef KPLATO_VIEWLIST_H
#define KPLATO_VIEWLIST_H

#include "kplato_export.h"

#include <QTreeWidget>

#include <KoQueryTrader.h>

class QDomElement;

class KoDocument;
class KoView;

namespace KPlato
{

class View;
class ViewBase;
class ViewListItem;
class ViewListWidget;

class Part;
class DocumentChild;
class Context;

class KPLATO_EXPORT ViewListItem : public QTreeWidgetItem
{
    public:
        enum ItemType { ItemType_Category = Type, ItemType_SubView = UserType, ItemType_ChildDocument };

        enum DataRole { DataRole_View = Qt::UserRole, DataRole_Document, DataRole_ChildDocument };

        ViewListItem( const QString &tag, const QStringList &strings, int type = ItemType_Category );
        ViewListItem( QTreeWidget *parent, const QString &tag, const QStringList &strings, int type = ItemType_Category );
        ViewListItem( QTreeWidgetItem *parent, const QString &tag, const QStringList &strings, int type = ItemType_Category );
        void setView( KoView *view );
        KoView *view() const;
        void setDocumentChild( DocumentChild *child );
        DocumentChild *documentChild() const;
        void setDocument( KoDocument *doc );
        KoDocument *document() const;

        QString tag() const { return m_tag; }
        void save( QDomElement &element ) const;

        void setReadWrite( bool rw );
        
    private:
        QString m_tag;
};

class KPLATO_EXPORT ViewListTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ViewListTreeWidget( QWidget *parent );
    ViewListItem *findCategory( const QString &cat );

    /// Return the category of @p view
    ViewListItem *category( const KoView *view ) const;
    
    void save( QDomElement &element ) const;

protected:
    void drawRow( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    virtual void mousePressEvent ( QMouseEvent *event );

signals:
    void activated( QTreeWidgetItem* );

private slots:
    void handleMousePress( QTreeWidgetItem *item );
};

class KPLATO_EXPORT ViewListWidget : public QWidget
{
Q_OBJECT
public:
    ViewListWidget( Part *part, QWidget *parent );//QString name, KXmlGuiWindow *parent );
    ~ViewListWidget();

    /// Set read/write permission on all views.
    void setReadWrite( bool rw ); 
    /// Add a category if it does not already exist
    ViewListItem *addCategory( const QString &tag, const QString& name );
    /// Return a list of all categories
    QList<ViewListItem*> categories() const;

    /// Return the category with @p tag
    ViewListItem *findCategory( const QString &tag ) const;
    /// Return the category of @p view
    ViewListItem *category( const KoView *view ) const;
    
    /// Create a unique tag
    QString uniqueTag( const QString &seed ) const;
    /// Add a sub-view
    ViewListItem *addView(QTreeWidgetItem *category, const QString &tag, const QString& name, KoView *view, KoDocument *doc, const QString& icon = QString() );
    /// Create a embedded child document view (callers resposibility to add to the list)
    ViewListItem *createChildDocumentView( const QString &tag, const QString& name, KoView *view, DocumentChild *ch, const QString& icon = QString() );

    void setSelected( QTreeWidgetItem *item );
    KoView *findView( const QString &tag ) const;
    ViewListItem *findItem( const QString &tag ) const;
    ViewListItem *findItem( const QString &tag, QTreeWidgetItem* parent ) const;
    ViewListItem *findItem( const QWidget *view, QTreeWidgetItem* parent = 0 ) const;

    /// Remove @p item, don't emit signal
    int removeViewListItem( ViewListItem *item );
    /// Add @p item to @p parent at @p index, don't emit signal
    void addViewListItem( ViewListItem *item, QTreeWidgetItem *parent, int index );
    
    /// Remove @p item, emit signal
    int takeViewListItem( ViewListItem *item );
    /// Add @p item to @p parent at @ index, emit signal
    void insertViewListItem( ViewListItem *item, QTreeWidgetItem *parent, int index );

    void save( QDomElement &element ) const;

signals:
    void activated( ViewListItem*, ViewListItem* );
    void createView();
    void createKofficeDocument( KoDocumentEntry &entry );
    void viewListItemRemoved( ViewListItem *item );
    void viewListItemInserted( ViewListItem *item );

protected slots:
    void slotActivated( QTreeWidgetItem *item, QTreeWidgetItem *prev );
    void slotItemChanged( QTreeWidgetItem *item, int col );
    void renameCategory();
    void slotCreatePart();
    void slotAddView();
    void slotRemoveCategory();
    void slotRemoveView();
    void slotEditViewTitle();
    void slotEditDocumentTitle();
    void slotRemoveDocument();
    void slotConfigureItem();
    
protected:
    virtual void contextMenuEvent ( QContextMenuEvent *event );

private:
    void setupContextMenus();

private:
    Part *m_part;
    ViewListTreeWidget *m_viewlist;
    QList<KoDocumentEntry> m_lstEntries;
    KoDocumentEntry m_documentEntry;

    ViewListItem *m_contextitem;
    QList<QAction*> m_editcategory;
    QList<QAction*> m_editview;
    QList<QAction*> m_addview;
    QList<QAction*> m_editdocument;
    QList<QAction*> m_adddocument;
};

} //Kplato namespace

#endif
