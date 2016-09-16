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

#include <QSplitter>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMap>
#include <QAction>

class KReportDesignerSectionDetailGroup;
class KReportDesigner;

class KToolBar;

class QDomElement;
class QActionGroup;

namespace KPlato
{

class ReportData;


class GroupSectionEditor : public QObject
{
    Q_OBJECT
public:
    explicit GroupSectionEditor(QObject *parent);

    void setupUi( QWidget *widget );
    void clear();
    void setData( KReportDesigner *designer, ReportData *rd );

protected Q_SLOTS:
    void slotSelectionChanged(const QItemSelection &sel );
    void slotAddRow();
    void slotRemoveRows();
    void slotMoveRowUp();
    void slotMoveRowDown();

private:
    Ui::ReportGroupSectionsWidget gsw;
    KReportDesigner *designer;
    ReportData *reportdata;
    QStandardItemModel model;

    class Item : public QStandardItem
    {
    public:
        explicit Item(KReportDesignerSectionDetailGroup *g) : QStandardItem(), group( g ) {}
        KReportDesignerSectionDetailGroup *group;

        QStringList names;
        QStringList keys;
    };

    class ColumnItem : public Item
    {
    public:
        explicit ColumnItem(KReportDesignerSectionDetailGroup *g);
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class SortItem : public Item
    {
    public:
        explicit SortItem(KReportDesignerSectionDetailGroup *g);
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class HeaderItem : public Item
    {
    public:
        explicit HeaderItem(KReportDesignerSectionDetailGroup *g);
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class FooterItem : public Item
    {
    public:
        explicit FooterItem(KReportDesignerSectionDetailGroup *g);
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
    class PageBreakItem : public Item
    {
    public:
        explicit PageBreakItem(KReportDesignerSectionDetailGroup *g);
        QVariant data( int role = Qt::DisplayRole ) const;
        void setData( const QVariant &value, int role = Qt::EditRole );
    };
};


} // namespace KPlato

#endif
