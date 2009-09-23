/* This file is part of the KDE project
  Copyright (C) 2008 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTGANTTITEMDELEGATE_H
#define KPTGANTTITEMDELEGATE_H

#include "kplatoui_export.h"

#include <kdganttglobal.h>

#include "kdganttitemdelegate.h"

#include <QBrush>

namespace KDGantt
{
    class StyleOptionGanttItem;
    class Constraint;
}

class QPainter;
class QModelIndex;


namespace KPlato
{

class KPLATOUI_EXPORT GanttItemDelegate : public KDGantt::ItemDelegate
{
    Q_OBJECT
public:
    GanttItemDelegate( QObject *parent = 0 );

//    virtual QString toolTip( const QModelIndex& idx ) const;
    virtual KDGantt::Span itemBoundingSpan( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const;
    virtual void paintGanttItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx );
    
    virtual void paintConstraintItem( QPainter* p, const QStyleOptionGraphicsItem& opt, const QPointF& start, const QPointF& end, const KDGantt::Constraint &constraint );

    QVariant data( const QModelIndex& idx, int column, int role = Qt::DisplayRole ) const;
    QString itemText( const QModelIndex& idx, int type ) const;
    int itemFloatWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const;
    int itemNegativeFloatWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const;

    bool hasStartConstraint( const QModelIndex& idx ) const;
    bool hasEndConstraint( const QModelIndex& idx ) const;
    int itemStartConstraintWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const;
    int itemEndConstraintWidth( const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx ) const;

    bool showResources;
    bool showTaskName;
    bool showTaskLinks;
    bool showProgress;
    bool showPositiveFloat;
    bool showNegativeFloat;
    bool showCriticalPath;
    bool showCriticalTasks;
    bool showAppointments;
    bool showNoInformation;
    bool showTimeConstraint;

protected:
    void paintSpecialItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx, int typ );

private:
    Q_DISABLE_COPY(GanttItemDelegate)
    QBrush m_criticalBrush;

};

class KPLATOUI_EXPORT ResourceGanttItemDelegate : public KDGantt::ItemDelegate
{
    Q_OBJECT
public:
    ResourceGanttItemDelegate( QObject *parent = 0 );

    QVariant data( const QModelIndex& idx, int column, int role = Qt::DisplayRole ) const;

    virtual void paintGanttItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx );

protected:
    void paintResourceItem( QPainter* painter, const KDGantt::StyleOptionGanttItem& opt, const QModelIndex& idx );

private:
    Q_DISABLE_COPY(ResourceGanttItemDelegate)
    QBrush m_overloadBrush;
    QBrush m_underloadBrush;

};

} // namespace KPlato

#endif
