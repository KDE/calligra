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

#ifndef KPTITEMVIEWSETTUP_H
#define KPTITEMVIEWSETTUP_H

#include "ui_kptitemviewsettings.h"

#include <QListWidgetItem>
#include <QWidget>

#include <kdialog.h>
#include <kpagedialog.h>

namespace KPlato
{

class DoubleTreeViewBase;
class TreeViewBase;

class ItemViewSettup : public QWidget, public Ui::ItemViewSettings
{
    Q_OBJECT
public:
    explicit ItemViewSettup( TreeViewBase *view, bool includeColumn0, QWidget *parent = 0 );
    
    class Item : public QListWidgetItem
    {
    public:
        Item( int column, const QString &text );
        int column() const;
        bool operator<( const QListWidgetItem & other ) const;
    private:
        int m_column;
    };
    
signals:
    void enableButtonOk( bool );
    
public slots:
    void slotChanged();
    void slotOk();
    void setDefault();
    
private:
    TreeViewBase *m_view;
    bool m_includeColumn0;
};

class ItemViewSettupDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit ItemViewSettupDialog( TreeViewBase *view, bool includeColumn0 = false, QWidget *parent = 0 );

    KPageWidgetItem *insertWidget( int before, QWidget *widget, const QString &name, const QString &header );

private:
    QList<KPageWidgetItem*> m_pageList;
    ItemViewSettup *m_panel;
};

class SplitItemViewSettupDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit SplitItemViewSettupDialog( DoubleTreeViewBase *view, QWidget *parent = 0 );

    KPageWidgetItem *insertWidget( int before, QWidget *widget, const QString &name, const QString &header );
    
private:
    QList<KPageWidgetItem*> m_pageList;
    ItemViewSettup *m_page1;
    ItemViewSettup *m_page2;
};

} //namespace KPlato

#endif
