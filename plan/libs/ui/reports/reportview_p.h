/* This file is part of the KDE project
  Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATOREPORTVIEW_P_H
#define KPLATOREPORTVIEW_P_H


#include "kplatoui_export.h"

#include "ui_reportgroupsectionswidget.h"

#include "KoReportDesigner.h"

#include <QSplitter>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMap>
#include <QAction>

class KoReportData;
class ORPreRender;
class ORODocument;
class ReportSectionDetailGroup;
class ReportViewPageSelect;
class RecordNavigator;
class ScriptAdaptor;
class KoReportDesigner;

class KToolBar;

class QScrollArea;
class QDomElement;
class QActionGroup;

namespace KPlato
{

class ReportData;

class ReportDesignPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ReportDesignPanel( QWidget *parent = 0 );

    ReportDesignPanel( const QDomElement &element, const QList<ReportData*> &models, QWidget *parent = 0 );
    
    QDomDocument document() const;
    
    KoReportDesigner *m_designer;
    KoProperty::EditorView *m_propertyeditor;
    ReportSourceEditor *m_sourceeditor;
    bool m_modified;
    QActionGroup *m_actionGroup;

    QStandardItemModel *createSourceModel( QObject *parent = 0 ) const;

signals:
    void insertItem( const QString &name );

public slots:
    void slotPropertySetChanged();
    void slotInsertAction();
    
    void setReportData( const QString &tag );
    
    void setModified() { m_modified = true; }
    void slotItemInserted(const QString &item);
    
protected:
    ReportData *createReportData( const QString &type );
    void populateToolbar( KToolBar *tb );

private:
    QList<ReportData*> m_reportdatamodels;
};

class GroupSectionEditor : public QObject
{
    Q_OBJECT
public:
    GroupSectionEditor( QObject *parent );

    void setupUi( QWidget *widget );
    void clear();
    void setData( KoReportDesigner *designer, ReportData *rd );

protected slots:
    void slotSelectionChanged(const QItemSelection &sel );
    void slotAddRow();
    void slotRemoveRows();
    void slotMoveRowUp();
    void slotMoveRowDown();

private:
    Ui::ReportGroupSectionsWidget gsw;
    KoReportDesigner *designer;
    ReportData *reportdata;
    QStandardItemModel model;

    class Item : public QStandardItem
    {
    public:
        Item( ReportSectionDetailGroup *g ) : QStandardItem(), group( g ) {}
        ReportSectionDetailGroup *group;

        QStringList names;
        QStringList keys;
    };

    class ColumnItem : public Item
    {
    public:
        ColumnItem( ReportSectionDetailGroup *g );
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class SortItem : public Item
    {
    public:
        SortItem( ReportSectionDetailGroup *g );
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class HeaderItem : public Item
    {
    public:
        HeaderItem( ReportSectionDetailGroup *g );
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class FooterItem : public Item
    {
    public:
        FooterItem( ReportSectionDetailGroup *g );
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class PageBreakItem : public Item
    {
    public:
        PageBreakItem( ReportSectionDetailGroup *g );
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
};


} // namespace KPlato

#endif
