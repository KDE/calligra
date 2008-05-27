/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Gantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Gantt licenses may use this file in
 ** accordance with the KD Gantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdgantt for
 **   information about KD Gantt Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef KDGANTTLEGEND_H
#define KDGANTTLEGEND_H

#include <QAbstractItemView>

#include "kdganttglobal.h"
#include "kdganttstyleoptionganttitem.h"

namespace KDGantt 
{
    class KDGANTT_EXPORT Legend : public QAbstractItemView
    {
        Q_OBJECT
        KDGANTT_DECLARE_PRIVATE_BASE_POLYMORPHIC( Legend )
    public:
        explicit Legend( QWidget* parent = 0 );
        virtual ~Legend();

        /*reimp*/ QModelIndex indexAt( const QPoint& point ) const;
        /*reimp*/ QRect visualRect( const QModelIndex& index ) const;

        /*reimp*/ void scrollTo( const QModelIndex&, ScrollHint = EnsureVisible ){}

        /*reimp*/ QSize sizeHint() const;
        /*reimp*/ QSize minimumSizeHint() const;

        /*reimp*/ void setModel( QAbstractItemModel* model );

    protected:
        virtual QRect drawItem( QPainter* painter, const QModelIndex& index, const QPoint& pos = QPoint() ) const;
        virtual QSize measureItem( const QModelIndex& index, bool recursive = true ) const;
        virtual StyleOptionGanttItem getStyleOption( const QModelIndex& index ) const;

        /*reimp*/ void paintEvent( QPaintEvent* event );

        /*reimp*/ int horizontalOffset() const { return 0; }
        /*reimp*/ bool isIndexHidden( const QModelIndex& ) const { return false; }
        /*reimp*/ QModelIndex moveCursor( CursorAction, Qt::KeyboardModifiers ) { return QModelIndex(); }
        /*reimp*/ void setSelection( const QRect&, QItemSelectionModel::SelectionFlags ){}
        /*reimp*/ int verticalOffset() const { return 0; }
        /*reimp*/ QRegion visualRegionForSelection( const QItemSelection& ) const { return QRegion(); }

    protected Q_SLOTS:
        virtual void modelDataChanged();
    };
}

#endif
