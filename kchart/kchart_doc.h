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

#ifndef __kchart_doc_h__
#define __kchart_doc_h__

class KChartDoc;

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


#include "kchart.h"
#include "kchart_view.h"
#include "koChart.h"
#include "chart.h"

#include <koPageLayoutDia.h>


#define MIME_TYPE "application/x-kchart"
#define EDITOR "IDL:KChart/Document:1.0"

/*
 */
class KChartDoc : public QObject,
		     virtual public KoDocument,
		     virtual public KoPrintExt,
		     virtual public Chart::SimpleChart_skel,
		     virtual public KChart::Document_skel
{
  Q_OBJECT
public:
  // C++
  KChartDoc();
  ~KChartDoc();

  // C++
  virtual bool save( ostream&, const char *_format );

  // C++
  // virtual bool loadChildren( KOStore::Store_ptr _store ) { return m_pMap->loadChildren( _store ); }
  virtual bool loadXML( KOMLParser&, KOStore::Store_ptr _store );

  virtual void cleanUp();

  virtual void removeView( KChartView* _view );

  // C++
  virtual KChartView* createDiagrammView(QWidget *_parent = 0);

  // IDL
  virtual bool initDoc();

  /**
   * Wrapper for @ref #createDiagrammView
   */
  virtual OpenParts::View_ptr createView();

  virtual void viewList( OpenParts::Document::ViewList & _list );

  virtual QCString mimeType() { return MIME_TYPE; }

  virtual bool isModified() { return m_bModified; }

  virtual bool hasToWriteMultipart() { return false; }

  virtual KOffice::MainWindow_ptr createMainWindow();

  // C++
  virtual int viewCount();

  // C++
  virtual void setModified( bool _c ) { m_bModified = _c; if ( _c ) m_bEmpty = false; }
  virtual bool isEmpty() { return m_bEmpty; }

  // C++
  table_t& data() { return m_table; }

  // C++
  virtual void editData();

  // C++
  void configChart();

  // IDL
  virtual void showWizard();
  virtual void fill( const Chart::Range& range, const Chart::Matrix& matrix );

  // C++
  virtual void print( QPaintDevice* _dev );
  virtual void draw( QPaintDevice* _dev, long int _width, long int _height,
		     float _scale );

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

  KoChart::dia_type diaType() { return m_type; }
  void setDiaType( KoChart::dia_type _type );

  void calcPaperSize();

signals:
  // Document signals
  void sig_updateView();

protected:
  virtual bool completeLoading( KOStore::Store_ptr /* _store */ );

  table_t m_table;
  bool m_bEmpty;
  KoChart::dia_type m_type;

  QList<KChartView> m_lstViews;

  Chart::Range m_range;

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
};

#endif
