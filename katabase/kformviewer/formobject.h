/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#ifndef __formobject_h__
#define __formobject_h__

#include <qobject.h>
#include <qpushbutton.h>

class FormObject : public QObject
{
public:
 
  /**
   *  Constructor.
   *
   *  Initalizes a object of a form.
   *
   *  @param _posx  Horizontal position of the object.
   *  @param _posy  Vertical position of the object.
   *  @param _sizex Horizontal size of the object.
   *  @param _sizey Vertical size of the object.
   */
  FormObject( int _posx, int _posy, int _sizex, int _sizey )
  : m_posx( _posx ), m_posy( _posy ), m_sizex( _sizex ), m_sizey( _sizey ) { };
  
  /**
   *  Destructor.
   */
  ~FormObject() { };

  /**
   *  Moves an object of a form to a new position.
   *
   *  @param _posx  Horizontal position of the object.
   *  @param _posy  Vertical position of the object.
   */
  void move( int _posx, int _posy )
  { m_posx = _posx; m_posy = _posy; };

  /**
   *  Resizes an object of a form.
   *
   *  @param _sizex Horizontal size of the object.
   *  @param _sizey Vertical size of the object.
   */
  void resize( int _sizex, int _sizey )
  { m_sizex = _sizex; m_sizey = _sizey; };

  /**
   *  Creates a QT-object.
   *
   *  @param _parent Parent of the widget.
   *  @param _name   Internal QT-name of the widget.
   *
   *  @return A Pointer to a widget.
   */
  virtual QWidget* create( QWidget* _parent = 0L, const char* _name = 0L ) = 0;

protected:

  void setGeometry( QWidget* _widget )
  { _widget->setGeometry( m_posx, m_posy, m_sizex, m_sizey ); };

  int m_posx;
  int m_posy;
  int m_sizex;
  int m_sizey;
};

class FormButton : public FormObject
{
public:

  FormButton( int _posx, int _posy, int _sizex, int _sizey, QString _title )
  : FormObject( _posx, _posy, _sizex, _sizey ), m_title( _title ) { };

  ~FormButton() { };

  virtual QWidget* create( QWidget* _parent = 0L, const char* _name = 0L )
  {
    QPushButton* button = new QPushButton( _parent, _name );
    setGeometry( button );
    button->setText( m_title );
    return button;
  };

protected:

  QString m_title;
};

#endif  // __formobject_h__

