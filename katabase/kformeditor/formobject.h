/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>
 
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

class FormObject : public QObject
{
public:

  enum Type { Label, Button, LineEdit, ListBox, Pixmap, ChechButton, Radiobutton };
  
  /**
   *  Constructor.
   */
  FormObject();
 
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
  FormObject( Type _type, QString _name, int _posx, int _posy, int _sizex, int _sizey );
  FormObject( Type _type, QString _name, int _posx, int _posy, int _sizex, int _sizey, QString _text );

  /**
   *  copy-Constructor.
   */
  FormObject( const FormObject& _to_copy );
  
  /**
   *  Destructor.
   */
  ~FormObject();

  /**
   *  Moves an object of a form to a new position.
   *
   *  @param _posx  Horizontal position of the object.
   *  @param _posy  Vertical position of the object.
   */
  void move( int _posx, int _posy );

  /**
   *  Resizes an object of a form.
   *
   *  @param _sizex Horizontal size of the object.
   *  @param _sizey Vertical size of the object.
   */
  void resize( int _sizex, int _sizey );

  int posx() { return m_posx; };
  int posy() { return m_posy; };

  /**
   *  Creates a QT-object.
   *
   *  @param _parent Parent of the widget.
   *  @param _name   Internal QT-name of the widget.
   *
   *  @return A Pointer to a widget.
   */
  QWidget* create( QWidget* _parent = 0L );

  Type type();
  QString action();
  void setAction( QString _action );

protected:

  void setGeometry( QWidget* _widget );

  Type m_type;
  QString m_name;
  int m_posx;
  int m_posy;
  int m_sizex;
  int m_sizey;
  QString m_text;
  QString m_action;
};

#endif  // __formobject_h__

