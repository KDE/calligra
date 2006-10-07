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


#include "KDGanttSemiSizingControl.h"
#include <qpushbutton.h>
#include <qpointarray.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
/*!
  \class KDGanttSemiSizingControl KDGanttSemiSizingControl.h
  This class provides exactly one child widget with a button for
  minimizing and restoring. You can also specify a so-called minimize
  widget that will be shown in place of the child widget while the
  latter one is minimized. While the child widget is not minimized,
  the minimize widget will not be visible.

  If you add more than one child widget (besides the minimize widget),
  only the last one added will be visible.
*/


/*!
  Constructs an empty semi sizing control with horizontal
  orientation and the control arrow button on top of the controlled
  widget.

  \param parent the parent widget. This parameter is passed to the
  base class.
  \param name the internal widget name. This parameter is passed to
  the base class.
*/

KDGanttSemiSizingControl::KDGanttSemiSizingControl( QWidget* parent,
                                          const char* name ) :
    KDGanttSizingControl( parent, name ), _orient( Horizontal ), 
    _arrowPos( Before ), _minimizedWidget(0), _maximizedWidget(0)
{
    init();
}


/*!
  Constructs an empty semi sizing control with the specified
  orientation and the control arrow button either on top or left of
  the controlled widget (depending on the orientation).

  \param orientation the orientation of the splitter
  \param parent the parent widget. This parameter is passed to the
  base class.
  \param name the internal widget name. This parameter is passed to
  the base class.
*/

KDGanttSemiSizingControl::KDGanttSemiSizingControl( Orientation orientation,
                                          QWidget* parent,
                                          const char* name ) :
    KDGanttSizingControl( parent, name ), _orient( orientation ), 
    _arrowPos( Before ), _minimizedWidget(0), _maximizedWidget(0)
{
    init();
}


/*!
  Constructs an empty semi sizing control with the specified
  orientation and position of the control arrow button.

  \param arrowPosition specifies whether the control arrow button
  should appear before or after the controlled widget
  \param orientation the orientation of the splitter
  \param parent the parent widget. This parameter is passed to the
  base class.
  \param name the internal widget name. This parameter is passed to
  the base class.
*/

KDGanttSemiSizingControl::KDGanttSemiSizingControl( ArrowPosition arrowPosition,
                                          Orientation orientation,
                                          QWidget* parent,
                                          const char* name ) :
    KDGanttSizingControl( parent, name ), _orient( orientation ), 
    _arrowPos( arrowPosition ), _minimizedWidget(0), _maximizedWidget(0)
{
    init();
}


/*!
  Specifies the widget that should be shown while the child widget is
  minimized. This so-called minimize widget should be a child widget
  of the KDGanttSemiSizingControl.

  \param widget the minimize widget
  \sa minimizedWidget()
*/

void KDGanttSemiSizingControl::setMinimizedWidget( QWidget* widget )
{
    _minimizedWidget = widget;
    if( _minimizedWidget ) _minimizedWidget->hide();
    setup();
}


/*!
  Returns the widget that is shown while the child widget is
  minimized.

  \return the minimize widget
  \sa setMinimizedWidget()
*/

QWidget* KDGanttSemiSizingControl::minimizedWidget() const
{
    return _minimizedWidget;
}

/*!
  Specifies the widget that should be shown while the child widget is
  maximized. This so-called maximize widget should be a child widget
  of the KDGanttSemiSizingControl.

  \param widget the minimize widget
  \sa maximizedWidget()
*/

void KDGanttSemiSizingControl::setMaximizedWidget( QWidget* widget )
{
    _maximizedWidget = widget;
    //if( _maximizedWidget ) _maximizedWidget->show();
    setup();
}

/*!
  Returns the widget that is shown while the child widget is
  maximized.

  \return the maximize widget
  \sa setMaximizedWidget()
*/

QWidget* KDGanttSemiSizingControl::maximizedWidget() const
{
    return _maximizedWidget;
}



/*!
  Sets the orientation of the simple sizing control.

  \param orientation the new orientation
  \sa orientation()
*/

void KDGanttSemiSizingControl::setOrientation( Qt::Orientation orientation )
{
    if ( _orient != orientation ) {
        _orient = orientation;
        setup();
    }
}


/*!
  Returns the orientation of the simple sizing control.
  \return the orientation
  \sa setOrientation()
*/

Qt::Orientation KDGanttSemiSizingControl::orientation() const
{
    return _orient;
}


/*!
  Returns the position of the control arrow button.

  \param arrowPosition the position of the control arrow button
  \sa arrowPosition()
*/

void KDGanttSemiSizingControl::setArrowPosition( ArrowPosition arrowPosition )
{
    if ( _arrowPos != arrowPosition ) {
        _arrowPos = arrowPosition;
        setup();
    }
}


/*!
  Returns the position of the control arrow button.

  \return the position of the control arrow button
  \sa setArrowPosition()
*/

KDGanttSemiSizingControl::ArrowPosition KDGanttSemiSizingControl::arrowPosition() const
{
    return _arrowPos;
}


/*!
  \enum KDGanttSemiSizingControl::ArrowPosition

  This enum is used for specifying whether the control arrow button
  should appear before (on top of, left of) or after (below, right of)
  the controlled widget.
*/

void KDGanttSemiSizingControl::init()
{
    _but = new QPushButton( this );
    _but->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
    connect( _but, SIGNAL( clicked() ), this, SLOT(changeState()) );
    _layout = 0;
    QWhatsThis::add( _but, "Click on this button to show the \nlegend at the bottom of the widget");
    QToolTip::add( _but, "Show / hide legend");


}

void KDGanttSemiSizingControl::setup()
{
    //-------------------------------------------------- Setup layout
    delete _layout;
    QBoxLayout* butLayout; // _layout will delete me

    if ( _orient == Horizontal || isMinimized() )
        _layout = new QHBoxLayout( this );
    else
        _layout = new QVBoxLayout( this );

    if ( _orient == Vertical && !isMinimized() )
        butLayout = new QHBoxLayout( _layout );
    else
        butLayout = new QVBoxLayout( _layout );



    //---------------------------------------- Set the arrow on the button
    if ( !isMinimized() ) {
        _but->setPixmap( pixmap( Down ) );
    }
    else {
        if ( _arrowPos == Before ) {
            _but->setPixmap( pixmap( Right ) );
        }
        else {
            _but->setPixmap( pixmap( Left ) );
        }
    }

    //------------------------------ Setup the button at the correct possition
    if ( _arrowPos == After && _orient == Vertical && !isMinimized() ) {
        butLayout->addStretch( 1 );
        butLayout->addWidget( _but, 0, Qt::AlignLeft );
    }
    else {
        butLayout->addWidget( _but, 0, Qt::AlignRight  );
        butLayout->addStretch( 1 );
    }

    // Set widget in the correct possition
    QWidget* widget;
    /* ************************** old code ***************
       if ( isMinimized() )
       widget = _minimizedWidget;
       else
       widget = _maximizedWidget;
       if( widget ) {
       if ( _arrowPos == Before  || _orient == Vertical && !isMinimized() )
       _layout->addWidget( widget, 1 );
       else
       _layout->insertWidget( 0, widget, 1 );
	}
     ************************************************** */
    // hack for the usage in KDGantt as pop-up legend widget
    // for this purpose,
    // the _maximizedWidget must be a child of the parent of this widget

    if ( isMinimized() ) {
       widget = _minimizedWidget;
       if( widget ) {
	 if ( _arrowPos == Before  || _orient == Vertical && !isMinimized() )
	   _layout->addWidget( widget, 1 );
	 else
	   _layout->insertWidget( 0, widget, 1 );
       }
    }
    else {
      if ( _arrowPos == Before  || _orient == Vertical && !isMinimized() )
	_layout->addStretch( 1 );
      else
	_layout->insertStretch( 0, 1 );
      widget = _maximizedWidget;
      // the following is only the special case
      // arrowPos == Before  and  _orient == Vertical
      //widget->move( 0+x(), _but->height()+y());
    }
}


/*!
  Restores or minimizes the child widget. \a minimize() does exactly the
  opposite to this method.

  \param restore true to restore, false to minimize
  \sa minimize()
*/

void KDGanttSemiSizingControl::restore( bool restore )
{
    if ( ! restore ) {
        minimize( true );
    }
    else {
        if( _maximizedWidget ) _maximizedWidget->show();
        if( _minimizedWidget ) _minimizedWidget->hide();
        KDGanttSizingControl::restore( restore );
        setup();
    }
}

/*!
  Restores or minimizes the child widget. \a restore() does exactly the
  opposite to this method.

  \param minimize true to minimize, false to restore
  \sa restore()

*/

void KDGanttSemiSizingControl::minimize( bool minimize )
{
    if ( ! minimize ) {
        restore( true );
    }
    else {
        if( _minimizedWidget ) _minimizedWidget->show();
	if( _maximizedWidget ) _maximizedWidget->hide();
        KDGanttSizingControl::minimize( minimize );
        setup();
    }
}

QPixmap KDGanttSemiSizingControl::pixmap( Direction direction ) {
    int s = 10;
    QPixmap pix( s, s );
    pix.fill( blue );

    QPointArray arr;
    switch ( direction ) {
    case Up:    arr.setPoints( 3,   0, s-1,   s-1, s-1,   0, s/2   ); ;break;
    case Down:  arr.setPoints( 3,   0, 0,     s-1, 0,     s/2, s-1 ); break;
    case Left:  arr.setPoints( 3,   s-1, 0,   s-1, s-1,   0, s/2   ); break;
    case Right: arr.setPoints( 3,   0,0,      s-1, s/2,   0, s-1   ); break;
    }

    QPainter p( &pix );
    p.setPen( black );
    p.setBrush( colorGroup().button() );
    p.drawPolygon( arr );
    QBitmap bit( s, s );
    bit.fill( color0 );

    QPainter p2( &bit );
    p2.setPen( color1 );
    p2.setBrush( color1 );
    p2.drawPolygon( arr );
    pix.setMask( bit );
    return pix;
}

#ifndef KDGANTT_MASTER_CVS
#include "KDGanttSemiSizingControl.moc"
#endif
