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

class KImageDoc;

#include <koFrame.h>
#include <koDocument.h>
#include <koPrintExt.h>

#include <iostream.h>
#include <qlist.h>

#include <qobject.h>
#include <qrect.h>
#include <qstring.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qimage.h>

#include "kimage.h"
#include "kimage_view.h"

#include <koPageLayoutDia.h>


#define MIME_TYPE "application/x-kimage"
#define EDITOR "IDL:KImage/Document:1.0"

/*
 */
class KImageDoc : public QObject,
		     virtual public KoDocument,
		     virtual public KoPrintExt,
		     virtual public KImage::Document_skel
{
  Q_OBJECT
public:
  // C++
  KImageDoc();
  ~KImageDoc();

  // C++
  virtual bool save( ostream&, const char *_format );
  virtual bool completeSaving( KOStore::Store_ptr _store );
  virtual bool hasToWriteMultipart() { return true; }
  
  // C++
  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr _store );
  
  virtual void cleanUp();

  virtual void removeView( KImageView* _view );

  // C++
  virtual KImageView* createImageView();

  // IDL
  virtual CORBA::Boolean init();

  virtual KOffice::MainWindow_ptr createMainWindow();
  
  /**
   * Wrapper for @ref #createImageView
   */
  virtual OpenParts::View_ptr createView();
  
  virtual void viewList( OpenParts::Document::ViewList*& _list );
  
  virtual char* mimeType() { return CORBA::string_dup( MIME_TYPE ); }
  
  virtual CORBA::Boolean isModified() { return m_bModified; }

  // C++
  virtual int viewCount();
  
  // C++
  virtual void setModified( bool _c ) { m_bModified = _c; if ( _c ) m_bEmpty = false; }
  virtual bool isEmpty() { return m_bEmpty; }

  // C++
  virtual void print( QPaintDevice* _dev );
  virtual void draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
		     CORBA::Float _scale );

  // C++
  void paperLayoutDlg();

  /**
   * @return the printable width of the paper in millimeters.
   */
  float printableWidth() { return m_paperWidth - m_leftBorder - m_rightBorder; }  
  /**
   * @return the printable height of the paper in millimeters.
   */
  float printableHeight() { return m_paperHeight - m_topBorder - m_bottomBorder; }
  
  float paperHeight() { return m_paperHeight; }
  float paperWidth() { return m_paperWidth; }
  
  /**
   * @return the left border in millimeters
   */
  float leftBorder() { return m_leftBorder; }
  /**
   * @return the right border in millimeters
   */
  float rightBorder() { return m_rightBorder; }
  /**
   * @return the top border in millimeters
   */
  float topBorder() { return m_topBorder; }
  /**
   * @return the bottom border in millimeters
   */
  float bottomBorder() { return m_bottomBorder; }
  
  /**
   * @return the orientation of the paper.
   */
  KoOrientation orientation() { return m_orientation; }
  /**
   * @return the paper format.
   */
  KoFormat paperFormat() { return m_paperFormat; }
  
  /**
   * Changes the paper layout and repaints the currently displayed KSpreadTable.
   */
  void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBoder,
		       KoFormat _paper, KoOrientation orientation );
  void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder,
		       const char * _paper, const char* _orientation );
  
  void setHeadFootLine( const char *_headl, const char *_headm, const char *_headr,
			const char *_footl, const char *_footm, const char *_footr );

  QString headLeft( int _p, const char *_t  ) { if ( m_headLeft.isNull() ) return "";
  return completeHeading( m_headLeft.data(), _p, _t ); }
  QString headMid( int _p, const char *_t ) { if ( m_headMid.isNull() ) return "";
  return completeHeading( m_headMid.data(), _p, _t ); }
  QString headRight( int _p, const char *_t ) { if ( m_headRight.isNull() ) return "";
  return completeHeading( m_headRight.data(), _p, _t ); }
  QString footLeft( int _p, const char *_t ) { if ( m_footLeft.isNull() ) return "";
  return completeHeading( m_footLeft.data(), _p, _t ); }
  QString footMid( int _p, const char *_t ) { if ( m_footMid.isNull() ) return "";
  return completeHeading( m_footMid.data(), _p, _t ); }
  QString footRight( int _p, const char *_t ) { if ( m_footRight.isNull() ) return "";
  return completeHeading( m_footRight.data(), _p, _t ); }

  QString completeHeading( const char *_data, int _page, const char *_table );

  QString headLeft() { if ( m_headLeft.isNull() ) return ""; return m_headLeft.data(); }
  QString headMid() { if ( m_headMid.isNull() ) return ""; return m_headMid.data(); }
  QString headRight() { if ( m_headRight.isNull() ) return ""; return m_headRight.data(); }
  QString footLeft() { if ( m_footLeft.isNull() ) return ""; return m_footLeft.data(); }
  QString footMid() { if ( m_footMid.isNull() ) return ""; return m_footMid.data(); }
  QString footRight() { if ( m_footRight.isNull() ) return ""; return m_footRight.data(); }
  
  void calcPaperSize();

  QString orientationString();
  QString paperFormatString();
  
  bool openDocument( const char *_filename, const char *_format = 0L );
  bool saveDocument( const char *_filename, const char *_format = 0L );

  const QImage& image() { return m_image; }
  int xOffset() { return m_iXOffset; }
  int yOffset() { return m_iYOffset; }
  
signals:
  // Document signals
  void sig_updateView();
  
protected:
  virtual bool completeLoading( KOStore::Store_ptr /* _store */ );
  
  bool m_bEmpty;

  QList<KImageView> m_lstViews;

  /**
   * The orientation of the paper.
   */
  KoOrientation m_orientation;
  /**
   * Tells about the currently seleced paper size.
   */
  KoFormat m_paperFormat;
  
  /**
   * The paper width in millimeters. Dont change this value, it is calculated by
   * @ref #calcPaperSize from the value @ref #m_paperFormat.
   */
  float m_paperWidth;
  /**
   * The paper height in millimeters. Dont change this value, it is calculated by
   * @ref #calcPaperSize from the value @ref #m_paperFormat.
   */
  float m_paperHeight;    
  /**
   * The left border in millimeters.
   */
  float m_leftBorder;
  /**
   * The right border in millimeters.
   */
  float m_rightBorder;
  /**
   * The top border in millimeters.
   */
  float m_topBorder;
  /**
   * The right border in millimeters.
   */
  float m_bottomBorder;
  
  QString m_headLeft;
  QString m_headRight;
  QString m_headMid;
  QString m_footLeft;
  QString m_footRight;
  QString m_footMid;

  QImage m_image;
  QString m_strImageFormat;
  int m_iXOffset;
  int m_iYOffset;
};

#endif
