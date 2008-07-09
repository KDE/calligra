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
#ifndef KDGANTTGRAPHICSVIEW_P_H
#define KDGANTTGRAPHICSVIEW_P_H

#include "kdganttgraphicsview.h"
#include "kdganttgraphicsscene.h"
#include "kdganttdatetimegrid.h"

#include <QPointer>

namespace KDGantt {
    class HeaderWidget : public QWidget {
        Q_OBJECT
    public:
        explicit HeaderWidget( GraphicsView* parent );
        virtual ~HeaderWidget();

        GraphicsView* view() const { return qobject_cast<GraphicsView*>( parent() );}

    public Q_SLOTS:
        void scrollTo( int );
    protected:
        /*reimp*/ void paintEvent( QPaintEvent* ev );
        /*reimp*/ void contextMenuEvent( QContextMenuEvent* ev );
    private:
        qreal m_offset;
    };

    class GraphicsView::Private {
        Q_DISABLE_COPY( Private )
    public:
        explicit Private(GraphicsView* _q);

        void updateHeaderGeometry();

        void slotGridChanged();
        void slotHorizontalScrollValueChanged( int val );

        /* slots for QAbstractItemModel signals */
        void slotColumnsInserted( const QModelIndex& parent,  int start, int end );
        void slotColumnsRemoved( const QModelIndex& parent,  int start, int end );
        void slotDataChanged( const QModelIndex& topLeft, const QModelIndex& bottomRight );
        void slotLayoutChanged();
        void slotModelReset();
        void slotRowsInserted( const QModelIndex& parent,  int start, int end );
        void slotRowsAboutToBeRemoved( const QModelIndex& parent,  int start, int end );
        void slotRowsRemoved( const QModelIndex& parent,  int start, int end );

        void slotItemClicked( const QModelIndex& idx );
        void slotItemDoubleClicked( const QModelIndex& idx );


        GraphicsView* q;
        AbstractRowController* rowcontroller;
        HeaderWidget headerwidget;
        GraphicsScene scene;
    };
}

#endif /* KDGANTTGRAPHICSVIEW_P_H */

