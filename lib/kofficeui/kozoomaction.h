/* This file is part of the KDE libraries
    Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef kozoomaction_h
#define kozoomaction_h

#include <kaction.h>

class KoZoomAction : public KSelectAction
{
Q_OBJECT

public:

  KoZoomAction( const QString& text, const QIconSet& pix, 
    const KShortcut& cut = KShortcut(), QObject* parent = 0, const char* name = 0 );
      
  KoZoomAction( const QString& text, const QString& pix, 
    const KShortcut& cut = KShortcut(), QObject* parent = 0, const char* name = 0 );
      
public slots:

  void setZoom( const QString& zoom );
  
  void setZoom( int zoom );

protected slots:

  void activated( const QString& text );  
  
signals:

  void zoomChanged( const QString& zoom );
  
protected:

  void init();
  
};


#endif // kozoomaction_h
