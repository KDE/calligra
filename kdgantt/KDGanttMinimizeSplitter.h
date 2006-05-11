/* -*- Mode: C++ -*-
   $Id$
*/

/****************************************************************************
 ** Copyright (C)  2001-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDGantt library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDGantt licenses may use this file in
 ** accordance with the KDGantt Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
 **   information about KDGantt Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 ** As a special exception, permission is given to link this program
 ** with any edition of Qt, and distribute the resulting executable,
 ** without including the source code for Qt in the source distribution.
 **
 **********************************************************************/

#ifndef KDGANTTMINIMIZESPLITTER_H
#define KDGANTTMINIMIZESPLITTER_H

#ifndef QT_H
#include "qframe.h"
#include "qvaluelist.h"
#endif // QT_H

#ifndef QT_NO_SPLITTER

class QSplitterData;
class QSplitterLayoutStruct;

class KDGanttMinimizeSplitter : public QFrame
{
    Q_OBJECT
    Q_ENUMS( Direction )
    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    Q_PROPERTY( Direction minimizeDirection READ minimizeDirection WRITE setMinimizeDirection )

public:
    enum ResizeMode { Stretch, KeepSize, FollowSizeHint };
    enum Direction { Left, Right, Up, Down };

    KDGanttMinimizeSplitter( QWidget* parent=0, const char* name=0 );
    KDGanttMinimizeSplitter( Orientation, QWidget* parent=0, const char* name=0 );
    ~KDGanttMinimizeSplitter();

    virtual void setOrientation( Orientation );
    Orientation orientation() const { return orient; }

    void setMinimizeDirection( Direction );
    Direction minimizeDirection() const;

#if QT_VERSION >= 300
    virtual void setResizeMode( QWidget *w, ResizeMode );
    virtual void setOpaqueResize( bool = TRUE );
    bool opaqueResize() const;

    void moveToFirst( QWidget * );
    void moveToLast( QWidget * );

    void refresh() { recalc( TRUE ); }
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    QValueList<int> sizes() const;
    void setSizes( QValueList<int> );

    void expandPos( int id, int* min, int* max );
protected:
    void childEvent( QChildEvent * );

    bool event( QEvent * );
    void resizeEvent( QResizeEvent * );

    int idAfter( QWidget* ) const;

    void moveSplitter( QCOORD pos, int id );
    virtual void drawSplitter( QPainter*, QCOORD x, QCOORD y,
			       QCOORD w, QCOORD h );
    void styleChange( QStyle& );
    int adjustPos( int , int );
    virtual void setRubberband( int );
    void getRange( int id, int*, int* );

private:
    void init();
    void recalc( bool update = FALSE );
    void doResize();
    void storeSizes();
    void processChildEvents();
    QSplitterLayoutStruct *addWidget( QWidget*, bool first = FALSE );
    void recalcId();
    void moveBefore( int pos, int id, bool upLeft );
    void moveAfter( int pos, int id, bool upLeft );
    void setG( QWidget *w, int p, int s, bool isSplitter = FALSE );

    QCOORD pick( const QPoint &p ) const
    { return orient == Horizontal ? p.x() : p.y(); }
    QCOORD pick( const QSize &s ) const
    { return orient == Horizontal ? s.width() : s.height(); }

    QCOORD trans( const QPoint &p ) const
    { return orient == Vertical ? p.x() : p.y(); }
    QCOORD trans( const QSize &s ) const
    { return orient == Vertical ? s.width() : s.height(); }

    QSplitterData *data;
#endif

private:
    Orientation orient;
    Direction _direction;
#ifndef DOXYGEN_SKIP_INTERNAL
    friend class KDGanttSplitterHandle;
#endif
private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    KDGanttMinimizeSplitter( const KDGanttMinimizeSplitter & );
    KDGanttMinimizeSplitter& operator=( const KDGanttMinimizeSplitter & );
#endif
};

#ifndef DOXYGEN_SKIP_INTERNAL
// This class was continued from a verbatim copy of the
// QSplitterHandle pertaining to the Qt Enterprise License and the
// GPL. It has only been renamed to KDGanttSplitterHandler in order to
// avoid a symbol clash on some platforms.
class KDGanttSplitterHandle : public QWidget
{
    Q_OBJECT
#if QT_VERSION >= 300
public:
    KDGanttSplitterHandle( Qt::Orientation o,
		       KDGanttMinimizeSplitter *parent, const char* name=0 );
    void setOrientation( Qt::Orientation o );
    Qt::Orientation orientation() const { return orient; }

    bool opaque() const { return s->opaqueResize(); }

    QSize sizeHint() const;

    int id() const { return myId; } // data->list.at(id())->wid == this
    void setId( int i ) { myId = i; }

protected:
    QValueList<QPointArray> buttonRegions();
    void paintEvent( QPaintEvent * );
    void mouseMoveEvent( QMouseEvent * );
    void mousePressEvent( QMouseEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    int onButton( const QPoint& p );
    void updateCursor( const QPoint& p );

private:
    Qt::Orientation orient;
    bool opaq;
    int myId;

    KDGanttMinimizeSplitter *s;
    int _activeButton;
    bool _collapsed;
    int _origPos;
#endif
};
#endif

#endif // QT_NO_SPLITTER

#endif // KDGANTTMINIMIZESPLITTER_H
