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

    DoubleListViewBase(QWidget *parent, bool description=false);

    //~DoubleListViewBase();

    KListView *masterListView() const { return m_masterList; }
    KListView *slaveListView() const { return m_slaveList; }
    
    void setNameHeader(QString text);
    void setTotalHeader(QString text);
    void setDescriptionHeader(QString text);
    void addSlaveColumn(QString text);
    virtual void print(KPrinter &printer);

    void calculate();
    void clearLists();
    virtual void createSlaveItems();
    void clearSlaveList();
    void setFormat(int fieldwidth=0, char fmt='f', int prec=0);
    void setMasterFormat(int fieldwidth=0, char fmt='f', int prec=0);
    void setSlaveFormat(int fieldwidth=0, char fmt='f', int prec=0);
    
    class MasterListItem;
    class SlaveListItem : public KListViewItem {
    public:
        SlaveListItem(MasterListItem *master, QListView *parent, QListViewItem *after, bool highlight=false);
        SlaveListItem(MasterListItem *master, QListViewItem *parent, QListViewItem *after, bool highlight=false);
        ~SlaveListItem();
        void masterItemDeleted() { m_masterItem = 0; }
        
        virtual void setColumn(int col, double value);
        virtual void clearColumn(int col);

        double value(int col) const { return m_valueMap[col]; }
        
        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
        
        void setFormat(int fieldwidth=0, char fmt='f', int prec=0);

    protected:
        MasterListItem *m_masterItem;
        double m_value;
        bool m_highlight;

        QMap<int, double> m_valueMap;
        
        int m_fieldwidth;
        char m_fmt;
        int m_prec;
    };

    class MasterListItem : public KListViewItem {
    public:
        MasterListItem(QListView *parent, bool highlight=false);
        MasterListItem(QListView *parent, QString text, bool highlight=false);
        MasterListItem(QListViewItem *parent, bool highlight=false);
        MasterListItem(QListViewItem *parent, QString text, bool highlight=false);
        ~MasterListItem();
        
        /// Creates slaveitems for myself and my children
        void createSlaveItems(QListView *lv, QListViewItem *after=0);
        void slaveItemDeleted();
        void setSlaveOpen(bool on);
        SlaveListItem *slaveItem() const { return m_slaveItem; }
        
        void setTotal(double tot);
        double calcTotal();
        void addToTotal(double v);
        void setSlaveItem(int col, double value);
        void clearColumn(int col);
        void calcSlaveItems();
        double calcSlaveItems(int col);

        virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);    

        void setFormat(int fieldwidth=0, char fmt='f', int prec=0);

    protected:
        SlaveListItem *m_slaveItem;
        double m_value;
        bool m_highlight;
        
        QMap<int, double> m_valueMap;
    
        int m_fieldwidth;
        char m_fmt;
        int m_prec;
    };

protected slots:
    void slotExpanded(QListViewItem* item);
    void slotCollapsed(QListViewItem* item);

private:

private:
    KListView *m_masterList;
    KListView *m_slaveList;
    
    int m_fieldwidth;
    char m_fmt;
    int m_prec;
};

}  //KPlato namespace

#endif
