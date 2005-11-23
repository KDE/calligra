/* This file is part of the KDE project
   Copyright (C) 2005 Dag Andersen <kplato@kde.org>

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

#ifndef KPTDOUBLELISTVIEWBASE_H
#define KPTDOUBLELISTVIEWBASE_H

#include <qsplitter.h>
#include <klistview.h>

class QListViewItem;

class KListView;
class KListViewItem;
class KPrinter;

namespace KPlato
{

class View;
class Project;

/**
 * The class DoubleListViewBase provides a double listview
 * where the right listview (the slave) containes columns of
 * double values and the left listview (the master) is the 'item' listview
 * and also provides for a sum total column of the values in the slave listview.
 * This makes it possible to scroll the slave listview and still see the values
 * in the master listview.
 */
class DoubleListViewBase : public QSplitter
{
    Q_OBJECT
public:

    DoubleListViewBase(Project &project, View *view, QWidget *parent, bool description=false);

    //~DoubleListViewBase();

    virtual View *mainView() { return m_mainview; }
    virtual void print(KPrinter &printer);

    class MasterDLVItem;
    class SlaveDLVItem : public KListViewItem {
    public:
        SlaveDLVItem(MasterDLVItem *master, QListView *parent, QListViewItem *after, bool highlight=false);
        SlaveDLVItem(MasterDLVItem *master, QListViewItem *parent, QListViewItem *after, bool highlight=false);
        ~SlaveDLVItem();
        void masterItemDeleted() { m_masterItem = 0; }
        
        void setColumn(int col, double value);
        void clearColumn(int col);

        double value(int col) const { return m_valueMap[col]; }
        
        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
    private:
        MasterDLVItem *m_masterItem;
        double m_value;
        bool m_highlight;

        QMap<int, double> m_valueMap;
        
    };

    class MasterDLVItem : public KListViewItem {
    public:
        MasterDLVItem(QListView *parent, bool highlight=false);
        MasterDLVItem(QListViewItem *parent, QString text, bool highlight=false);
        MasterDLVItem(QListViewItem *parent, QString text, QString text2, bool highlight=false);
        ~MasterDLVItem();
        
        /// Creates slaveitems for myself and my children
        void createSlaveItems(QListView *lv, QListViewItem *after=0);
        void slaveItemDeleted();
        void setSlaveOpen(bool on);
        SlaveDLVItem *slaveItem() const { return m_slaveItem; }
        
        void setTotal(double tot);
        double calcTotal();
        void addToTotal(double v);
        void setSlaveItem(int col, double value);
        void clearColumn(int col);
        void calcSlaveItems();
        double calcSlaveItems(int col);

        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);    

    private:
        SlaveDLVItem *m_slaveItem;
        double m_value;
        bool m_highlight;
        
        QMap<int, double> m_valueMap;
    };

protected slots:
    void slotExpanded(QListViewItem* item);
    void slotCollapsed(QListViewItem* item);

private:
    void createSlaveItems();
    void clearSlaveList();

private:
    Project &m_project;
    View *m_mainview;

    KListView *m_masterList;
    KListView *m_slaveList;
};

}  //KPlato namespace

#endif
