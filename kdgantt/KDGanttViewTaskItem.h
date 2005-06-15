/* -*- Mode: C++ -*-
   $Id$
   KDGantt - a multi-platform charting engine
*/

/****************************************************************************
 ** Copyright (C)  2002-2004 Klarälvdalens Datakonsult AB.  All rights reserved.
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


#ifndef KDGANTTVIEWTASKITEM_H
#define KDGANTTVIEWTASKITEM_H

#include "KDGanttViewItem.h"
class KDCanvasRectangle;
class QBrush;

class KDGanttViewTaskItem : public KDGanttViewItem
{
public:
    KDGanttViewTaskItem( KDGanttView* view,
                            const QString& lvtext = QString::null,
                            const QString& name = QString::null );
    KDGanttViewTaskItem( KDGanttViewItem* parent,
                            const QString& lvtext = QString::null,
                            const QString& name = QString::null );
    KDGanttViewTaskItem( KDGanttView* view, KDGanttViewItem* after,
                            const QString& lvtext = QString::null,
                            const QString& name = QString::null );
    KDGanttViewTaskItem( KDGanttViewItem* parent, KDGanttViewItem* after,
                            const QString& lvtext = QString::null,
                            const QString& name = QString::null );
    virtual ~KDGanttViewTaskItem();

    void setStartTime( const QDateTime& start );
    void setEndTime( const QDateTime& end );

private:
    void showItem( bool show = true, int coordY = 0 );
    void initItem();
    void hideMe();
    QBrush myBrush, undefinedBrush;
    bool _showUndefinedBrush;
};

#endif
