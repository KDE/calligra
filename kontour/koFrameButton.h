/*
 *
 *  Copyright (c) 2000 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef __ko_framebutton_h__
#define __ko_framebutton_h__

#include <qlabel.h>
class QPixmap;
class QString;

class KoFrameButton : public QLabel
{
  Q_OBJECT

 public:
  KoFrameButton( QWidget* parent = 0, const char* name = 0 );
  KoFrameButton( const QString& text, QWidget* parent = 0, const char* name = 0 );
  KoFrameButton( const QPixmap& pixmap, QWidget* parent = 0, const char* name = 0 );

  bool isOn() { return m_active; }
  bool isToggleButton() { return m_toggle; }
  void setOn(bool);
  void setToggleButton(bool);

  QString text() { return m_text; }
  virtual void setText ( const QString& );

 signals:
  void clicked();
  void clicked(const QString&);

 protected:
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );

 private:
  bool     m_active;
  bool     m_toggle;
  QString  m_text;
};

#endif
