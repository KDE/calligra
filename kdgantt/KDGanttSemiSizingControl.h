/* -*- Mode: C++ -*-
   $Id$
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


#ifndef KDGANTTSEMISIZINGCONTROL_H
#define KDGANTTSEMISIZINGCONTROL_H

#include "KDGanttSizingControl.h"
#include <qlayout.h>
class QPushButton;
class QBoxLayout;

class KDGanttSemiSizingControl : public KDGanttSizingControl
{
    Q_PROPERTY( ArrowPosition arrowPosition READ arrowPosition WRITE setArrowPosition )
    Q_ENUMS( ArrowPosition )
    Q_OBJECT

public:
    enum ArrowPosition { Before, After };

    KDGanttSemiSizingControl( QWidget* parent = 0, const char* name = 0 );
    KDGanttSemiSizingControl( Orientation orientation, QWidget* parent = 0,
                           const char* name = 0 );
    KDGanttSemiSizingControl( ArrowPosition arrowPosition,
                         Orientation orientation, QWidget* parent = 0,
                         const char* name = 0 );

    void setMinimizedWidget( QWidget* widget );
    void setMaximizedWidget( QWidget* widget );
    QWidget* minimizedWidget() const;
    QWidget* maximizedWidget() const;

    void setOrientation( Qt::Orientation orientation );
    Qt::Orientation orientation() const;

    void setArrowPosition( ArrowPosition arrowPosition );
    ArrowPosition arrowPosition() const;

public slots:
    virtual void minimize( bool minimize );
    virtual void restore( bool restore );

protected:
    void setup();
    void init();
    enum Direction {Left, Right, Up, Down };
    QPixmap pixmap( Direction );

private:
    Orientation _orient;
    ArrowPosition _arrowPos;
    QWidget* _minimizedWidget;
    QWidget* _maximizedWidget;
    QBoxLayout* _layout;
    QPushButton* _but;
};


#endif
