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

#include <koFrame.h>
#include <koDocument.h>
#include <koPrintExt.h>
#include <koPageLayoutDia.h>

#include "kimage.h"
#include "kimage_view.h"

#define MIME_TYPE "application/x-kimage"
#define EDITOR "IDL:KImage/Document:1.0"

class KImageDoc : public QObject,
                  virtual public KoDocument,
                  virtual public KoPrintExt,
                  virtual public KImage::Document_skel
{
  Q_OBJECT

public:
  KImageDoc();
  ~KImageDoc();

  virtual bool save( ostream&, const char* _format );
  virtual bool completeSaving( KOStore::Store_ptr _store );
  virtual bool hasToWriteMultipart() { return true; }
  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr _store );
  virtual void cleanUp();
  virtual void removeView( KImageView* _view );

  // C++
  virtual KImageView* createImageView( QWidget* _parent = 0 );

  virtual bool initDoc();
  virtual KOffice::MainWindow_ptr createMainWindow();

  /**
   * Wrapper for @ref #createImageView
   */
  virtual OpenParts::View_ptr createView();
  virtual void viewList( OpenParts::Document::ViewList & _list );
  virtual QCString mimeType() { return MIME_TYPE; }
  virtual bool isModified() { return m_bModified; }
  virtual int viewCount();
  virtual void setModified( bool _c );
  virtual bool isEmpty();
  virtual void print( QPaintDevice* _dev );
  virtual void draw( QPaintDevice* _dev, long int _width, long int _height, float _scale );
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
   * Changes the paper layout and repaints the currently displayed KSpreadTable.
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
  bool openDocument( const QString & _filename, const char* _format = 0L );
  bool saveDocument( const QString & _filename, const char* _format = 0L );

  /**
   * Returns the image, that is stored in the document.
   * 
   * @result The returned image.
   */
  const QImage& image();

  /**
   * Transforms the image with the given matrix.
   * 
   * @param The matrix to transform the image.
   */
  void transformImage( const QWMatrix& matrix );
  
signals:
  // Document signals
  void sigUpdateView();
  
protected:
  virtual bool completeLoading( KOStore::Store_ptr /* _store */ );

  /**
   * Indicates whether an image is loaded or not.
   */
  bool m_bEmpty;

  /**
   * List of views, that are connectet to the document.
   */
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
