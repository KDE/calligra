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

#ifndef __kimage_doc_h__
#define __kimage_doc_h__

#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qimage.h>

//#include <koFrame.h>
#include <koStore.h>
#include <koDocument.h>
//#include <koPrintExt.h>
#include <koPageLayoutDia.h>

//#include "kimage.h"
#include "kimage_view.h"

#define MIME_TYPE "application/x-kimage"

class KImageDocument : public KoDocument
{
  Q_OBJECT

public:
  KImageDocument( KoDocument* parent = 0, const char* name = 0 );
  ~KImageDocument();

  // IDL

  // document
  virtual View* createView( QWidget* parent = 0, const char* name = 0 );
  virtual Shell* createShell();
  virtual QString configFile() const;
  virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );
  virtual bool initDoc();
  virtual QCString mimeType() const;
  
  float printableHeight();
  float paperHeight();
  float paperWidth();
  float leftBorder();
  float rightBorder();
  float topBorder();
  float bottomBorder();
  //KoOrientation orientation();
  //KoFormat paperFormat();
  //void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBoder, KoFormat _paper, KoOrientation orientation );
  //void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder, const char * _paper, const char* _orientation );
  //void setHeadFootLine( const char* _headl, const char* _headm, const char* _headr,	const char* _footl, const char* _footm, const char* _footr );
  QString headLeft( int _p, const char* _t );
  QString headMid( int _p, const char* _t );
  QString headRight( int _p, const char* _t );
  QString footLeft( int _p, const char* _t );
  QString footMid( int _p, const char* _t );
  QString footRight( int _p, const char* _t );
  QString completeHeading( const char* _data, int _page, const char* _table );
  QString headLeft();
  QString headMid();
  QString headRight();
  QString footLeft();
  QString footMid();
  QString footRight();
  void calcPaperSize();
  QString orientationString();
  QString paperFormatString();
  bool openDocument( const QString & _filename, const char* _format = 0L );
  bool saveDocument( const QString & _filename, const char* _format = 0L );
  const QImage& image();
  void transformImage( const QWMatrix& matrix );
  
signals:
  // Document signals
  void sigUpdateView();
  
protected:
  //virtual bool completeLoading( KOStore::Store_ptr /* _store */ );
  bool m_bEmpty;
  QList<KImageView> m_lstViews;
  KoOrientation m_orientation;
  KoFormat m_paperFormat;
  float m_paperWidth;
  float m_paperHeight;    
  float m_leftBorder;
  float m_rightBorder;
  float m_topBorder;
  float m_bottomBorder;
  QString m_headLeft;
  QString m_headRight;
  QString m_headMid;
  QString m_footLeft;
  QString m_footRight;
  QString m_footMid;
  QImage m_image;

// FIXME : make this private
public:
  QString m_strImageFormat;
};

#endif
