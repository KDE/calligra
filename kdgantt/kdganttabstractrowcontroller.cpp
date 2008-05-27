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
#include "kdganttabstractrowcontroller.h"

using namespace KDGantt;

/*!\class KDGantt::AbstractRowController kdganttabstractrowcontroller.h KDGanttAbstractRowController
 * \ingroup KDGantt
 * \brief Abstract baseclass for row controllers. A row controller is used
 * by the GraphicsView to nagivate the model and to determine the
 * row geometries
 */

/*! Constructor. Does nothing */
AbstractRowController::AbstractRowController()
{
}

/*! Destructor. Does nothing */
AbstractRowController::~AbstractRowController()
{
}


/*!\fn virtual int AbstractRowController::headerHeight() const = 0
 * \returns The height of the header part of the view.
 *
 * Implement this to control how much space is reserved at
 * the top of the view for a header
 */

/*!\fn virtual bool AbstractRowController::isRowVisible( const QModelIndex& idx ) const
 * \returns true if the row containing index \a idx is visible
 * in the view.
 *
 * Implement this to allow KDGantt to optimise how items on
 * screen are created. It is not harmful to always return true here,
 * but the View will not perform optimally.
 */


/*!\fn virtual Span AbstractRowController::rowGeometry( const QModelIndex& idx ) const
 * \returns A Span consisting of the row offset and height for the row
 * containing \a idx. A simple implementation might look like
 *
 * \code
 * Span MyRowCtrlr::rowGeometry(const QModelIndex& idx)
 * {
 *      return Span(idx.row()*10,10);
 * }
 * \endcode
 */

/*!\fn virtual QModelIndex AbstractRowController::indexBelow( const QModelIndex& idx ) const
 *\returns The modelindex for the next row after \a idx.
 *
 *\see QTreeView::indexBelow
 */

/*!\fn virtual QModelIndex AbstractRowController::indexAbove( const QModelIndex& idx ) const
 *\returns The modelindex for the previous row before \a idx.
 *
 *\see QTreeView::indexAbove
 */

