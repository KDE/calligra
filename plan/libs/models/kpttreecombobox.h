/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTTREECOMBOBOX_H
#define KPTTREECOMBOBOX_H

#include "kplatomodels_export.h"

#include <KComboBox>

#include <QAbstractItemView>
#include <QEvent>

class QModelIndex;
class QPersistentModelIndex;
class QPaintEvent;
class QTreeView;
class QAbstractItemModel;

namespace KPlato
{

class KPLATOMODELS_EXPORT TreeComboBox : public KComboBox
{
    Q_OBJECT
public:
    TreeComboBox( QWidget *parent = 0 );
    
    QTreeView *view() const;
    void setModel( QAbstractItemModel *model );
    QAbstractItemModel *model() const;
    
    QList<QPersistentModelIndex> currentIndexes() const { return m_currentIndexes; }

    void setSelectionMode( QAbstractItemView::SelectionMode mode );

    void showPopup();

signals:
    void changed();
    
public slots:
    void setCurrentIndexes( const QModelIndexList &lst );
    void setCurrentIndexes( const QList<QPersistentModelIndex> &lst );

protected:
    void paintEvent( QPaintEvent *event );
    
protected slots:
    void updateCurrentIndexes( const QModelIndexList &lst );
    void slotSelectionChanged();

private:
    void updateView();
    
private:
    QAbstractItemView::SelectionMode m_selectionmode;
    QList<int> m_showcolumns;
    bool m_showheader;
    QList<QPersistentModelIndex> m_currentIndexes;
};


} //namespace KPlato

#endif

