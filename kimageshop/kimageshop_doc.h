/*
 *  kimageshop_doc.h - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
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

#ifndef __kimageshop_doc_h__
#define __kimageshop_doc_h__

#include <qlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qimage.h>

#include <koFrame.h>
#include <koDocument.h>
#include <koPrintExt.h>
#include <koPageLayoutDia.h>

#include "kimageshop.h"
#include "kimageshop_view.h"
#include "canvas.h"

#define MIME_TYPE "application/x-kimageshop"
#define EDITOR "IDL:KImageShop/Document:1.0"

class KImageShopDoc : public Canvas,
		      virtual public KoDocument,
		      virtual public KoPrintExt,
		      virtual public KImageShop::Document_skel
{
  Q_OBJECT
    
 public:
  KImageShopDoc(int w = 510, int h = 510);
  ~KImageShopDoc();

  virtual bool save( ostream&, const char* _format );
  virtual bool completeSaving( KOStore::Store_ptr _store );
  virtual bool hasToWriteMultipart() { return true; }
  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr _store );
  virtual void cleanUp();
  virtual void removeView( KImageShopView* _view );
  
  // C++
  virtual KImageShopView* createImageView( QWidget* _parent = 0 );
  
  virtual CORBA::Boolean initDoc();
  virtual KOffice::MainWindow_ptr createMainWindow();
  
  /**
   * Wrapper for @ref #createImageView
   */
  virtual OpenParts::View_ptr createView();
  virtual void viewList( OpenParts::Document::ViewList*& _list );
  virtual char* mimeType();
  virtual CORBA::Boolean isModified();
  virtual int viewCount();
  virtual void setModified( bool _c );
  virtual bool isEmpty();
  virtual void print( QPaintDevice* _dev );
  virtual void draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height, CORBA::Float _scale );
  void paperLayoutDlg();
  
  /**
   * @return the printable width of the paper in millimeters.
   */
  float printableWidth();

  /**
   * @return the printable height of the paper in millimeters.
   */
  float printableHeight();
  float paperHeight();
  float paperWidth();
  
  /**
   * @return the left border in millimeters
   */
  float leftBorder();
  
  /**
   * @return the right border in millimeters
   */
  float rightBorder();
  
  /**
   * @return the top border in millimeters
   */
  float topBorder();
  
  /**
   * @return the bottom border in millimeters
   */
  float bottomBorder();
  
  /**
   * @return the orientation of the paper.
   */
  KoOrientation orientation();
  
  /**
   * @return the paper format.
   */
  KoFormat paperFormat();
  
  /**
   * Changes the paper layout and repaints the currently displayed View
   */
  void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBoder, KoFormat _paper, KoOrientation orientation );
  void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBorder, const char * _paper, const char* _orientation );
  void setHeadFootLine( const char* _headl, const char* _headm, const char* _headr,	const char* _footl, const char* _footm, const char* _footr );
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
  bool openDocument( const char* _filename, const char* _format = 0L );
  bool saveDocument( const char* _filename, const char* _format = 0L );

public slots:
 void slotUpdateViews(const QRect &area);

signals:
  // Document signals
  void sigUpdateView(const QRect &area);
  
protected:
  virtual bool completeLoading( KOStore::Store_ptr /* _store */ );

  /**
   * Indicates whether an image is loaded or not.
   */
  bool m_bEmpty;

  /**
   * List of views, that are connectet to the document.
   */
  QList<KImageShopView> m_lstViews;

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

  /**
   * The text in the left field of the headline.
   */
  QString m_headLeft;

  /**
   * The text in the right field of the headline.
   */
  QString m_headRight;

  /**
   * The text in the middle field of the headline.
   */
  QString m_headMid;

  /**
   * The text in the left field of the footline.
   */
  QString m_footLeft;

  /**
   * The text in the right field of the footline.
   */
  QString m_footRight;

  /**
   * The text in the middle field of the footline.
   */
  QString m_footMid;

  /**
   * The image, that is stored in the document.
   */
  QImage m_image;

  /**
   * The image format of the image in the document.
   */

// TODO
// make this private
public:
  QString m_strImageFormat;
  bool m_executeCommand;
};

#endif
