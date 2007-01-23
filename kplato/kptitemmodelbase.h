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

#ifndef KPTITEMMODELBASE_H
#define KPTITEMMODELBASE_H

#include "kptglobal.h"

#include <QtGui>

namespace KPlato
{

class Part;
class Project;

class EnumDelegate : public QItemDelegate
{
    Q_OBJECT
    public:
        EnumDelegate(QObject *parent = 0);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class DurationDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DurationDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class DurationSpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    DurationSpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class SpinBoxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    SpinBoxDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class ItemModelBase : public QAbstractItemModel
{
    Q_OBJECT
public:
    ItemModelBase( Part *part, QObject *parent = 0 );
    ~ItemModelBase();

    Project *project() const { return m_project; }
    virtual void setProject( Project *project );
    virtual void setReadWrite( bool rw ) { m_readWrite = rw; }
    bool isReadWrite() { return m_readWrite; }

protected slots:
    virtual void slotLayoutToBeChanged();
    virtual void slotLayoutChanged();

protected:
    Part *m_part;
    Project *m_project;
    bool m_readWrite;
};

class TreeViewBase : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeViewBase( QWidget *parent );

    void setArrowKeyNavigation( bool on ) { m_arrowKeyNavigation = on; }
    bool arrowKeyNavigation() const { return m_arrowKeyNavigation; }

    int nextColumn( int col ) const;
    int previousColumn( int col ) const;

protected:
    void keyPressEvent(QKeyEvent *event);
    QItemSelectionModel::SelectionFlags selectionCommand(const QModelIndex &index, const QEvent *event) const;

protected:
    bool m_arrowKeyNavigation;
};


} // namespace KPlato

#endif
