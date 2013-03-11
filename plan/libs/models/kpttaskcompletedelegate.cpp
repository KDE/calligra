/* This file is part of the KDE project
  Copyright (C) 2009 Dag Andersen <calligra-devel@kde.org>

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

#include "kpttaskcompletedelegate.h"

#include "kptnodeitemmodel.h"
#include "kptnode.h"

#include <kdebug.h>

#include <QModelIndex>
#include <QApplication>
#include <QToolTip>
#include <QStyleOptionViewItem>
#include <QStyle>
#include <QPainter>

namespace KPlato
{

//-----------------------------
TaskCompleteDelegate::TaskCompleteDelegate( QObject *parent )
 : ProgressBarDelegate( parent )
{
}

TaskCompleteDelegate::~TaskCompleteDelegate()
{
}

void TaskCompleteDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QModelIndex typeidx = index.model()->index( index.row(), NodeModel::NodeType, index.parent() );
    if ( ! typeidx.isValid() ) {
        kError()<<"Cannot find nodetype for index:"<<index;
        return;
    }
    int type = typeidx.data( Qt::EditRole ).toInt();
    if ( type == Node::Type_Task || type == Node::Type_Milestone ) {
        ProgressBarDelegate::paint( painter, option, index );
    } else {
        QStyle *style;
        QStyleOptionViewItemV4 opt = option;
        //initStyleOption( &opt, index );
        style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawPrimitive( QStyle::PE_PanelItemViewItem, &opt, painter );

        //kDebug(planDbg())<<"Draw something else, type="<<type<<index.model()->index( index.row(), NodeModel::NodeName, index.parent() ).data().toString();

        ItemDelegate::paint( painter, option, index );
    }
}

} //namespace KPlato

#include "kpttaskcompletedelegate.moc"
