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
#include "kdganttabstractgrid.h"
#include "kdganttabstractgrid_p.h"

using namespace KDGantt;

/*!\class KDGantt::AbstractGrid kdganttabstractgrid.h KDGanttAbstractGrid
 * \ingroup KDGantt
 * \brief Abstract baseclass for grids. A grid is used to convert between
 * QModelIndex'es and gantt chart values (doubles) and to paint the
 * background and header of the view.
 *
 * \see KDGantt::DateTimeGrid
 */

/*! Constructor. Creates an AbstractGrid with parent \a parent.
 * The QObject parent is not used for anything internally. */
AbstractGrid::AbstractGrid( QObject* parent )
    : QObject( parent ),
      _d( new Private )
{
}

/*! Destructor. Does nothing */
AbstractGrid::~AbstractGrid()
{
    delete _d;
}

#define d d_func()

/*! Sets the QAbstractItemModel used by this grid implementation.
 * This is called by the view, you should never need to call this
 * from client code.  */
void AbstractGrid::setModel( QAbstractItemModel* model )
{
    d->model = model;
}

/*!\returns The QAbstractItemModel used by this grid */
QAbstractItemModel* AbstractGrid::model() const
{
    return d->model;
}

/*! Sets the root index used by this grid implementation.
 * This is called by the view, you should never need to call this
 * from client code.  */
void AbstractGrid::setRootIndex( const QModelIndex& idx )
{
    d->root = idx;
}

/*!\returns the current root index for this grid */
QModelIndex AbstractGrid::rootIndex() const
{
    return d->root;
}

/*!\returns true if the startpoint is before the endpoint
 * of the constraint \a c.
 */
bool AbstractGrid::isSatisfiedConstraint( const Constraint& c ) const
{
    // First check if the data is valid,
    // TODO: review if true is the right choice
    if ( !c.startIndex().isValid() || !c.endIndex().isValid() ) return true;

    Span ss = mapToChart( c.startIndex() );
    Span es = mapToChart( c.endIndex() );
    return ( ss.end() <= es.start() );
}

/*!\fn virtual Span AbstractGrid::mapToChart( const QModelIndex& idx ) const
 * Implement this to map from the data in the model to the location of
 * the corresponding item in the view.
 */

/*!\fn virtual bool AbstractGrid::mapFromChart( const Span& span, const QModelIndex& idx, const QList<Constraint>& constraints ) const
 * Implement this to update the model data based on the location of the item. Check
 * against the \a constraints list to make sure no hard constraints are violated by
 * writing back to the model.
 * \returns true if the update succeeded.
 */

/*!\fn virtual void AbstractGrid::paintGrid( QPainter* painter, const QRectF& sceneRect, const QRectF& exposedRect, AbstractRowController* rowController=0, QWidget* widget=0 )
 *
 * Implement this to paint the background of the view -- typically
 * with some grid lines.
 * \param painter -- the QPainter to paint with.
 * \param sceneRect -- the total bounding rectangle of the scene.
 * \param exposedRect -- the rectangle that needs to be painted.
 * \param rowController -- the row controller used by the view -- may be 0.
 * \param widget -- the widget used by the view -- may be 0.
 */

/*!\fn virtual void AbstractGrid::paintHeader( QPainter* painter, const QRectF& headerRect, const QRectF& exposedRect, qreal offset, QWidget* widget=0 )
 *
 * Implement this to paint the header part of the view.
 * \param painter -- the QPainter to paint with.
 * \param headerRect -- the total rectangle occupied by the header.
 * \param exposedRect -- the rectangle that needs to be painted.
 * \param offset -- the horizontal scroll offset of the view.
 * \param widget -- the widget used by the view -- may be 0.
 */

#include "moc_kdganttabstractgrid.cpp"

