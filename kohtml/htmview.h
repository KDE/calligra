/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#ifndef __htmview_h_
#define __htmview_h_

#include <qpainter.h>
#include <qlist.h>
#include <iostream.h>

#include <khtmlview.h>
#include <khtmlsavedpage.h>

class KoHTMLDoc;

class KMyHTMLView : public KHTMLView
{
  Q_OBJECT
public:
  KMyHTMLView( KoHTMLDoc *doc, QWidget *parent = 0L, const char *name = 0L, int flags = 0,
                    KMyHTMLView *parent_view = 0L );
  ~KMyHTMLView();

  void draw( QPainter *painter, int width, int height );

  void drawWidget( QWidget *widget );

  virtual void begin( const char *url, int dx, int dy );
  
  virtual KHTMLView *newView( QWidget *parent, const char *name = 0L, int flags = 0L );
  
  virtual void openURL( const char *url );
  virtual void openURL( const char *url, bool reload );
  
  void feedDocumentData( const char *data, bool eof );
  
  void stop();
  
  void childFinished( KMyHTMLView *child_view );

  void save( ostream &out );

  bool isChild( KMyHTMLView *child );
  
public slots:
  void redirectURL( int /*id*/, const char *url ) { m_strURL = url; }
    
protected slots:
  void documentFinished( KHTMLView *view );  
  void requestImage( const char *url );
  void cancelImage( const char *url );

protected:
  QString m_strDocument;  
      
private:

  struct FrameChild
  {
    FrameChild( KMyHTMLView *view )
    {
      m_pView = view;
      m_bDone = false;
    }
    
    KMyHTMLView *m_pView;
    bool m_bDone;
  };

  QPixmap *m_pPixmap;
  KoHTMLDoc *m_pDoc;
  QList<FrameChild> m_lstChildren;
  bool m_bParsing;
  bool m_bReload;
  bool m_bDone;
  QString m_strURL;
  KMyHTMLView *m_pParent;
};

#endif
