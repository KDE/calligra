/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org
 
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __widgetwrapper_h__
#define __widgetwrapper_h__

#include <qwidget.h>

class WidgetWrapper : public QWidget
{
  Q_OBJECT

public:

  enum SelectState { NoSelect, PrimarySelect, SecondarySelect };

  WidgetWrapper( QWidget* _widget, WFlags f = 0 );
  ~WidgetWrapper();

  SelectState selectState();
  virtual bool eventFilter( QObject* _obj, QEvent* _event );

protected:

//virtual void resizeEvent( QResizeEvent* _event );

public slots:

  void slotSelectPrimary();
  void slotSelectSecondary();
  void slotUnselect();
  void slotResizing( const QRect& _rect );

signals:

  void clicked( WidgetWrapper* );
  void clickedShift( WidgetWrapper* );
  void moveWidget( WidgetWrapper*, const QPoint& );

private:

  void addExtraChilds( const QColor& _color );
  void removeExtraChilds();
  void installChildEventFilter( QWidget* _widget );

  SelectState m_selectState;
  QWidget* m_widget;
};

#endif // __widgetwrapper_h__
