/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
                 1998 Erwin tratar <tratar@stud.uni-frankfurt.de>
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

#include "koChart.h"
#include <math.h>
#include <functional>

#include <kapp.h>


#ifndef OLDCODE
#include <qmessagebox.h>
#endif

const int scaleFactor[] = {
	1,
	2,
	3,
	5,
	7,
	10,
	20,
	30,
	50,
	70,
	0
};

const unsigned int MAX_SAUELEN = 24;
const unsigned int MAX_LINIEN_SECTIONS = 24;
const unsigned int MAX_KREIS_SECTIONS = 8;
const unsigned int MAX_FARBEN = 8;
const double KREIS_CUTOFF = 1.0;

#ifndef OLDCODE
#include "KChartAreaPainter.h"
#include "KChartBarsPainter.h"
#include "KChartLinesPainter.h"
#include "KoChartBarConfigDialog.h"
#include "KoChartAreaConfigDialog.h"
#include "KoChartPieConfigDialog.h"
#include "KoChartLinesConfigDialog.h"
#include "KoChartLinesPointsConfigDialog.h"
#include "KoChartPointsConfigDialog.h"
#include <klocale.h>
#endif

bool operator < ( const pair<double, string>& x, const pair<double, string>& y)
{
  return ( x.first > y.first );
};

/////////////////////////////////////////////////////////////////////////////
// KoChartView

KoChartView::KoChartView( QWidget *_parent ) : QWidget( _parent )
{
  //setBackgroundColor( white );
}

KoChartView::~KoChartView()
{
}

void KoChartView::resizeEvent( QResizeEvent * )
{
  update();
}

void KoChartView::paintEvent( QPaintEvent * )
{
  QPainter painter;
  painter.begin( this );

  m_diagramm.paint( painter, width(), height() );

  painter.end();
}


#ifndef OLDCODE
void KoChartView::mousePressEvent( QMouseEvent* ev )
{
	if( ev->button() != RightButton )
		QWidget::mousePressEvent( ev );
	else
		m_diagramm.config( this );
}
#endif

/////////////////////////////////////////////////////////////////////////////
// KoChart message handlers

const double pi = 3.1415926535;

static QPoint getPercentPoint( QRect& rcBound, double fPercent)
{
  QPoint pt( rcBound.x() + rcBound.width() / 2, rcBound.y() + rcBound.height() / 2 );

  double arc = (2*pi*fPercent)/100;
  pt.setX( pt.x() + static_cast<int>(sin(arc)*rcBound.width() / 2 ) );
  pt.setY( pt.y() - static_cast<int>(cos(arc)*rcBound.height() / 2 ) );

  return pt;
}

static double calcArc( QRect& rcBound, QPoint pt1, QPoint pt2 )
{
  QPoint ptCenter( rcBound.x() + rcBound.width() / 2, rcBound.y() + rcBound.height() / 2 );
  pt1 -= ptCenter;
  pt2 -= ptCenter;

  return acos( (pt1.x() * pt2.x() + pt1.y() * pt2.y() ) /
	       ( sqrt( pt1.x() * pt1.x() + pt1.y() * pt1.y() ) * sqrt( pt2.x() * pt2.x() + pt2.y() * pt2.y() ) ) );
}

static void drawPie( QPainter& _painter, const QRect& _rect, int _a, int _alen )
{
  _painter.drawPie( _rect, 90 * 16 - _a, - _alen );
}


#ifndef OLDCODE
void KoChart::config( QWidget* parent )
{
	switch( m_lastPainterType ) {
	case Bars:
		{
			KoChartBarConfigDialog* dlg = new
				KoChartBarConfigDialog( &_params, parent );
			dlg->exec();
			delete dlg;
			break;
		}
	case Area:
		{
			KoChartAreaConfigDialog* dlg = new
				KoChartAreaConfigDialog( &_params, parent );
			dlg->exec();
			delete dlg;
			break;
		}
	case Pie:
		{
			KoChartPieConfigDialog* dlg = new
				KoChartPieConfigDialog( &_params, parent );
			dlg->exec();
			delete dlg;
			break;
		}
	case Lines:
		{
			KoChartLinesConfigDialog* dlg = new
				KoChartLinesConfigDialog( &_params, parent );
			dlg->exec();
			delete dlg;
			break;
		}
	case LinesPoints:
		{
			KoChartLinesPointsConfigDialog* dlg = new
				KoChartLinesPointsConfigDialog( &_params, parent );
			dlg->exec();
			delete dlg;
			break;
		}
	case Points:
		{
			KoChartPointsConfigDialog* dlg = new
				KoChartPointsConfigDialog( &_params, parent );
			dlg->exec();
			delete dlg;
			break;
		}
	default:
		QMessageBox::warning( parent, "KDiagramm", "Sorry, there is not yet a configuration dialog for this type of chart", "OK" );
	}
}
#endif



void KoChart::paint( QPainter& painter, int width, int height )
{
  switch( m_diaType )
  {
  case DT_KREIS_ODER_SAEULEN:
    {
      if( m_table.data.size() > 8 )
	drawDiagrammSaeulen( painter, width, height );
      else
	drawDiagrammKreis( painter, width, height );
    }
  break;
  case DT_SAEULEN:
    {
      drawDiagrammSaeulen( painter, width, height );
    }
  break;
  case DT_KREIS:
    {
      drawDiagrammKreis( painter, width, height );
    }
  break;
  case DT_LINIEN:
    {
      drawDiagrammLinien( painter, width, height );
    }
  case DT_AREA:
    {
      drawDiagrammLinien( painter, width, height );
    }
  break;
  }
}


#ifndef OLDCODE
void KoChart::setupPainter( QPainter& painter )
{
	painter.setPen( _params._fgcolor );
	painter.setPen( _params._bgcolor );
	//painter.eraseRect( painter.window() );

	// should be configurable later...
	_params._transparency = true;
	painter.setBackgroundMode( _params._transparency ? Qt::TransparentMode :
							   Qt::OpaqueMode );
}
#endif



// Sauelendiagramm
///////////////////////////////////////////////////////////////////////
void KoChart::drawDiagrammSaeulen( QPainter& painter, int _width, int _height )
{
#ifndef OLDCODE
	if( m_lastPainterType != Bars ) {
		delete _chartpainter;
		_chartpainter = new KChartBarsPainter( &_params );
		m_lastPainterType = Bars;
	}
	setupPainter( painter );
	_chartpainter->setWidthHeight(_width,_height);
	_chartpainter->paintChart( &painter, &m_table );
#else
  // Farben
  enum { FRONT, TOP, RIGHT };

  // Farben
  QColor brushPie[MAX_FARBEN];
  brushPie[0].setRgb( 153,153,255 );
  brushPie[1].setRgb( 153, 51,102 );
  brushPie[2].setRgb( 255,104,104 );
  brushPie[3].setRgb( 104,104,255);
  brushPie[4].setRgb( 102,  0,102 );
  brushPie[5].setRgb( 255,128,128 );
  brushPie[6].setRgb( 0,102,204 );
  brushPie[7].setRgb( 0,204,255 );

  // Font Groesse
  QRect sizeText = painter.boundingRect( 0, 0, _width, _height, AlignLeft, "0" );

  // Find maximal value
  double maxValue = 0.0;
  line_t::iterator iter;
  // Fuer jede Zeile berechnen
  for( data_t::iterator lit = m_table.data.begin(); lit != m_table.data.end(); ++lit )
  {
    for( iter = lit->begin(); iter != lit->end(); ++iter )
    {
      maxValue = max(maxValue, (*iter) );	
    }
  }

  // Laenge der horizintalen Beschreibung
  int max_xdesc_len = 0;
  const char *s;
  for( s = m_table.xDesc.first(); s != 0L; s = m_table.xDesc.next() )
  {
    QRect tmp = painter.boundingRect( 0, 0, _width, _height, AlignLeft, s );
    max_xdesc_len = (int)max((double)max_xdesc_len, sqrt(0.5) * tmp.width() );
  }

  if( maxValue == 0.0 )
    return;

  // in welchem Bereich liegt maxValue ?
  double dScale = 0.0;
  switch( m_dataType )
    {
    case DAT_NUMMER:
    case DAT_GEB:
      {
	KatDouble( maxValue, dScale );
      }
    break;
    case DAT_DAUER:
      {
	KatTime( maxValue, dScale );
     }
    break;
    }

  int nSteps = 0;
  double dStep = 0.0;

  if( maxValue <= 0.2 * dScale)
  {
    dStep	= dScale * 0.02;
    nSteps	= (int)ceil(maxValue/dStep);
  }
  else if( maxValue <= 0.5 * dScale)
  {
    dStep	= dScale * 0.05;
    nSteps	= (int)ceil(maxValue/dStep);
  }
  else
  {
    dStep	= dScale * 0.1;
    nSteps	= (int)ceil(maxValue/dStep);
  }

  QRect rcDraw( 0, 0, _width, _height );

  rcDraw.setTop( rcDraw.top() + sizeText.height() );
  rcDraw.setBottom( rcDraw.bottom() - max_xdesc_len - sizeText.height() - sizeText.height() );

  // Backplane
  int nBackPlane = (rcDraw.height() * 95) / 100;
  int nOffY = rcDraw.height() - nBackPlane;
  int nOffX = nOffY;

  int nStep = ( rcDraw.height() - nOffY ) / nSteps;
  int nSaeulen = min( (int)MAX_SAUELEN, (int)m_table.xDesc.count() );

  // Laenge der vertikalen Beschriftung
  int sizeKat = 0;
  QString strFoo;
  for( int i = 0; i * nStep <= rcDraw.height() - nOffY; i++ )
  {
    strFoo.sprintf( "%f", dStep * i );
    while( strFoo.right(1) == "0" )
      strFoo.truncate( strFoo.length() - 1 );
    if ( strFoo.right(1) == "." )
      strFoo.truncate( strFoo.length() - 1 );

    QRect tmp = painter.boundingRect( 0, 0, _width, _height, AlignLeft, strFoo );
    if ( tmp.width() > sizeKat )
      sizeKat = tmp.width();
  }

  // Hintergrund zeichnen
  rcDraw.setLeft( rcDraw.left() + sizeKat + sizeText.width() + 10 );
  rcDraw.setRight( rcDraw.right() - max(0, max_xdesc_len - sizeText.height()) - sizeText.height() );
  int nSection = ( rcDraw.width() - nOffX ) / nSaeulen;

  QRect rcBack( rcDraw.left() + nOffX, rcDraw.top(), rcDraw.right() - rcDraw.left() - nOffX,
		rcDraw.bottom() - nOffY - rcDraw.top() );

  QPoint leftPane[4];
  leftPane[0] = QPoint( rcDraw.left(), rcDraw.top() + nOffY);
  leftPane[1] = QPoint( rcDraw.left(), rcDraw.bottom() );
  leftPane[2] = QPoint( rcDraw.left() + nOffX, rcDraw.bottom() - nOffY );
  leftPane[3] = QPoint( rcDraw.left() + nOffX, rcDraw.top() );
  QPointArray poly( 4 );
  for( int k = 0; k < 4; k++ )
    poly.setPoint( k, leftPane[k] );

  painter.setPen( lightGray );
  painter.drawPolygon( poly );
  painter.drawRect(rcBack);

  leftPane[0] = QPoint( rcDraw.left(), rcDraw.bottom() );
  leftPane[1] = QPoint( rcDraw.left() + nOffX, rcDraw.bottom() - nOffY);
  leftPane[2] = QPoint( rcDraw.right(), rcDraw.bottom() - nOffY );
  leftPane[3] = QPoint( rcDraw.right() - nOffX, rcDraw.bottom() );

  for( int k = 0; k < 4; k++ )
    poly.setPoint( k, leftPane[k] );

  painter.setPen( darkGray );
  painter.drawPolygon( poly );

  QString strStep;
  for( int i = 0; i * nStep <= rcDraw.height() - nOffY; i++ )
  {
    painter.drawLine( rcDraw.left() - 10, rcDraw.bottom() - i * nStep,
		      rcDraw.left(), rcDraw.bottom() - i * nStep );
    painter.drawLine( rcDraw.left(), rcDraw.bottom() - i * nStep,
		      rcDraw.left() + nOffX, rcDraw.bottom() - i * nStep - nOffY );
    painter.drawLine( rcDraw.left() + nOffX, rcDraw.bottom() - i * nStep - nOffY,
		      rcDraw.right(), rcDraw.bottom() - i  * nStep - nOffY );

    // Beschriftung
    strStep.sprintf("%f", dStep * i );
    while( strStep.right(1) == "0" )
      strStep.truncate( strStep.length() - 1 );
    if ( strStep.right(1) == "." )
      strStep.truncate( strStep.length() - 1 );
    QRect tmp( 0, rcDraw.bottom() - i * nStep - painter.fontMetrics().height() / 2, rcDraw.left() - 10, _height );
    painter.drawText( tmp, AlignRight, strStep );
  }

  for( int i = 0; i < nSaeulen; i++)
  {
    painter.drawLine( rcDraw.left() + i * nSection, rcDraw.bottom(), rcDraw.left() + i * nSection, rcDraw.bottom() + 10 );
  }

  QPoint ptBackOffset( (nOffX*6)/10, -(nOffY*6)/10);
  QPoint ptFrontOffset( (nOffX*2)/10, -(nOffY*2)/10);

  unsigned int line = 0;
  // Fuer jede Zeile berechnen
  for( data_t::iterator lit = m_table.data.begin(); lit != m_table.data.end(); ++lit )
  {
    unsigned int nCount = 0;
    for( iter = lit->begin(); iter != lit->end() && nCount < MAX_SAUELEN; ++iter, ++nCount )
    {
      double fHeight = (*iter);

      fHeight *= (static_cast<double>(nStep)/dStep);
      QRect rcSaeule(rcDraw.left() + nCount * nSection + nSection / 2 - ( nSection * 3 ) / 10,
		     rcDraw.bottom(),
		     2 * ( nSection * 3 ) / 10 + 1, -(int)fHeight - 1 );
      rcSaeule.moveBy( ptFrontOffset.x(), ptFrontOffset.y() );

      int w = rcSaeule.width() / m_table.data.size();

      // Right
      leftPane[0]	= QPoint( rcSaeule.left() + (line+1)*w, rcSaeule.top() );
      leftPane[1]	= QPoint( rcSaeule.left() + (line+1)*w, rcSaeule.bottom() );
      leftPane[2]	= leftPane[1] + ptBackOffset;
      leftPane[3]	= leftPane[0] + ptBackOffset;
      for( int k = 0; k < 4; k++ )
	poly.setPoint( k, leftPane[k] );

      QColor col = brushPie[ line % MAX_FARBEN ];
      col.setRgb( (int)((double)col.red()/1.98), (int)((double)col.green()/1.98), (int)((double)col.blue()/1.98) );
      painter.setPen( col );
      painter.setBrush( col );
      painter.drawPolygon( poly );

      // Top
      leftPane[0]	= QPoint( rcSaeule.left() + (line+1)*w, rcSaeule.bottom() );
      leftPane[1]	= QPoint( rcSaeule.left() + line*w, rcSaeule.bottom() );
      leftPane[2]	= leftPane[1] + ptBackOffset;
      leftPane[3]	= leftPane[0] + ptBackOffset;
      for( int k = 0; k < 4; k++ )
	poly.setPoint( k, leftPane[k] );

      col = brushPie[ line % MAX_FARBEN ];
      col.setRgb( (int)((double)col.red()/1.33), (int)((double)col.green()/1.33), (int)((double)col.blue()/1.33) );
      painter.setPen( col );
      painter.setBrush( col );
      painter.drawPolygon( poly );

      // Front
      painter.setPen( brushPie[ line % MAX_FARBEN ] );
      rcSaeule.setBottom( rcSaeule.bottom() - 1 );
      rcSaeule.setRight( rcSaeule.left() + (line+1) * w );
      rcSaeule.setLeft( rcSaeule.left() + line * w );
      painter.fillRect( rcSaeule, brushPie[ line % MAX_FARBEN ] );
    }

    line++;
  }

  // Label
  unsigned int nCount = 0;
  s = m_table.xDesc.first();
  for( ; s != 0L && nCount < MAX_SAUELEN; s = m_table.xDesc.next(),  ++nCount )
  {
      painter.setPen( black );
      QRect tmp = painter.boundingRect( 0, 0, _width, _height, AlignLeft, s );
      painter.drawText( rcDraw.left() + nCount * nSection + nSection / 2 - tmp.width()/2,
			rcDraw.bottom() + painter.fontMetrics().ascent(), s );
  }
#endif
}


// Kreisdiagramm
///////////////////////////////////////////////////////////////////////

void KoChart::drawDiagrammKreis( QPainter &painter, int _width, int _height )
{
  // Exception
  if ( m_table.xDesc.count() < 2 )
    return;
  if ( m_table.yDesc.count() < 1 )
    return;

  QColor brushPie[MAX_KREIS_SECTIONS];
  brushPie[0].setRgb( 153,153,255 );
  brushPie[1].setRgb( 153, 51,102 );
  brushPie[2].setRgb( 255,104,104 );
  brushPie[3].setRgb( 104,104,255);
  brushPie[4].setRgb( 102,  0,102 );
  brushPie[5].setRgb( 255,128,128 );
  brushPie[6].setRgb( 0,102,204 );
  brushPie[7].setRgb( 0,204,255 );

  // Font-Groessen
  QRect sizeText = painter.boundingRect( 0, 0, _width, _height, Qt::AlignLeft, "0" );
  int iOff = max(sizeText.height(), sizeText.width() );
  cerr << "iOff = " << iOff << endl;

  // Wie sollen die Kreise auf das Fenster verteilt werden ?
  int x_kreise = m_table.yDesc.count();
  int y_kreise = 1;
  int optimum = 0;
  // Wieviele Zeilen ? Einfach probieren ....
  for( unsigned int i = 1; i <= m_table.yDesc.count(); i++ )
  {
    // Kreise pro Zeile
    int k = (int)ceil( (double)m_table.yDesc.count()/(double)i );
    int w = _width / k;
    int h = _height / i;
    int m = min( w * 2 / 3, h );
    if ( m > optimum )
    {
      optimum = m;
      x_kreise = k;
      y_kreise = i;
    }
  }

  int nx = 0;
  int ny = 0;
  const char* ydesc = m_table.yDesc.first();
  // Ein Kreis pro Datenzeile
  for( data_t::iterator lit = m_table.data.begin(); lit != m_table.data.end() && ydesc != 0L; ++lit, nx++, ydesc = m_table.yDesc.next() )
  {
    // Offset des Kreises von links oben
    if ( nx == x_kreise )
    {
      nx = 0;
      ny++;
    }
    int x_offset = nx * _width / x_kreise;
    int y_offset = ny * _height / y_kreise;

    painter.save();
    painter.translate( x_offset, y_offset );

    // Legende
    QRect sizeTextMaxTmp = painter.boundingRect( 0, 0, _width, _height, Qt::AlignLeft, "Rest" );
    int sizeTextMax = sizeTextMaxTmp.width();
    int sizeValue = 0;
    unsigned int nCount = 0;

    const char* s = m_table.xDesc.first();
    for( line_t::iterator iter = lit->begin(); nCount < MAX_KREIS_SECTIONS && iter != lit->end() && s != 0L;
	 ++iter, ++nCount, s = m_table.xDesc.next() )
    {
      QRect tmp = painter.boundingRect( 0, 0, _width, _height, Qt::AlignLeft, s );
      sizeTextMax = max( sizeTextMax , tmp.width() );

      QString strValue;
      strValue.sprintf( "%f", (*iter) );
      while( strValue.right(1) == "0" )
	strValue.truncate( strValue.length() - 1 );
      if ( strValue.right(1) == "." )
	strValue.truncate( strValue.length() - 1 );
	
      tmp = painter.boundingRect( 0, 0, _width, _height, Qt::AlignLeft, strValue );
      sizeValue = max( sizeValue, tmp.width() );
    }
    sizeValue	+= iOff;
    sizeTextMax += sizeValue;

    // Legendensymbol
    sizeTextMax += 2*iOff;

    // Platz für Legende abziehen
    sizeTextMax = min( sizeTextMax, _width/x_kreise / 3 );

    QRect rcDraw( 0, 0, _width/x_kreise, _height/y_kreise );
    rcDraw.setWidth( rcDraw.width() - sizeTextMax - iOff );

    // Hoehe fuer die Diagramm-Unterschift
    int y_desc_height = 0;
    if ( m_table.yDesc.count() > 1 )
    {
      y_desc_height = 2 * painter.fontMetrics().height();
    }
    rcDraw.setBottom( rcDraw.bottom() - y_desc_height );
		
    if( rcDraw.height() > rcDraw.width() )
      rcDraw.setBottom( rcDraw.top() + rcDraw.width() );
    else
      rcDraw.setRight( rcDraw.left() + rcDraw.height() );

    // Diagramm Unterschift anzeigen
    if ( m_table.yDesc.count() > 1 )
      painter.drawText( rcDraw.x(), rcDraw.bottom() + y_desc_height / 4, rcDraw.width(), y_desc_height * 3 / 4, Qt::AlignCenter, ydesc );


    QRect rcLegend( rcDraw.right() + iOff, rcDraw.top(), rcDraw.right() + sizeTextMax, rcDraw.bottom() );

    QRect rcBound( rcDraw );
    rcBound.setBottom( rcBound.bottom() - iOff/2 );
    rcBound.setRight( rcBound.right() - iOff/2 );
    rcBound.setLeft( rcBound.left() + iOff/2 );
    rcBound.setTop( rcBound.top() + iOff/2 );

    rcDraw.setBottom( rcBound.bottom() - 2*iOff );
    rcDraw.setRight( rcBound.right() - 2*iOff );
    rcDraw.setLeft( rcBound.left() + 2*iOff );
    rcDraw.setTop( rcBound.top() + 2*iOff );

    // Leerer Kreis
    painter.setBrush( Qt::white );
    painter.drawEllipse( rcDraw );

    double fLast = 0;
    double fRealLast = 0;

    double sum = 0.0;
    for( line_t::iterator iter = lit->begin(); iter != lit->end(); sum += (*iter), ++iter);

    nCount = 0;
    s = m_table.xDesc.first();
    for( line_t::iterator iter = lit->begin(); nCount < MAX_KREIS_SECTIONS && iter != lit->end() && s != 0L ; nCount++, s = m_table.xDesc.next() )
    {
      double fRealValue = (*iter);
      double fValue = ( (*iter) * 100 ) / sum;
      QString strLegend = s;

      cerr << "Entry: " << strLegend << endl;

      QPoint ptLegend( rcLegend.left(), rcLegend.top() + (3 * nCount * iOff) / 2 );

      // 3% Cutoff
      if( fValue < KREIS_CUTOFF && iter != lit->end() )
      {
	QPoint center( rcDraw.x() + rcDraw.width() / 2, rcDraw.y() + rcDraw.height() / 2 );
	if( getPercentPoint( rcDraw, fLast ).x() < center.x() )
	{
	  // Rest auffüllen
	  painter.setBrush( Qt::white );
	  // TODO pDC->Pie(rcDraw, getPercentPoint(rcDraw,0), getPercentPoint(rcDraw,fLast));
	}

	// : Bezeichnung
	painter.setPen( Qt::black );
	QString strValue;

	// Rest berechnen
	while( iter != lit->end() )
        {
	  fRealValue += (*iter);
	  ++iter;
	}

	if( fRealValue > 0.0 )
        {
	  // Format.raise(fRealValue, strValue);
	  strValue.sprintf( "%d", fRealValue );
	
	  painter.drawText( ptLegend.x() + 2*iOff, ptLegend.y() + painter.fontMetrics().ascent(), strValue );
	  painter.drawText( ptLegend.x() + 2*iOff + sizeValue, ptLegend.y() + painter.fontMetrics().ascent(), i18n( "Rest") );
	}

	break;
      }
      else
      {
	painter.setBrush( brushPie[ nCount % MAX_KREIS_SECTIONS ] );
	painter.setPen( brushPie[ nCount % MAX_KREIS_SECTIONS ] );
	
	QString strText;
	strText.sprintf( "%d", nCount + 1 );
	
	if ( ( nCount == 0 && fValue >= 99.0 ) ||
	     calcArc( rcDraw, getPercentPoint( rcDraw, fLast + fValue ), getPercentPoint( rcDraw, fLast ) ) > 0.0 )
        {
	  drawPie( painter, rcDraw, (int)( fRealLast / sum * 360 * 16 ), (int)( fRealValue / sum * 360 * 16 ) );
	
	  // Datenlinie
	  painter.drawLine( getPercentPoint( rcDraw, fLast + fValue / 2 ), getPercentPoint( rcBound, fLast + fValue / 2 ) );

	  QPoint ptText( getPercentPoint( rcBound, fLast + fValue / 2 ) );
	
	  painter.setBrush( Qt::black );
	  painter.setPen( Qt::black );
	
	  QRect tmp( ptText.x() - iOff/2, ptText.y() - iOff/2, iOff, iOff );
	  painter.drawEllipse( tmp );
	
	  QRect tmp2( ptLegend.x(), ptLegend.y(), iOff, iOff );
	  painter.drawEllipse( tmp2 );
	
	  painter.setBrush( Qt::white );
	  painter.setPen( Qt::white );
	
	  QRect tmp3 = painter.boundingRect( 0, 0, _width, _height, Qt::AlignLeft, strText );
	
	  painter.drawText( ptText.x() + ( iOff - tmp3.width() )/2 - iOff/2,
			    ptText.y() + painter.fontMetrics().ascent() +
			    ( iOff - painter.fontMetrics().height() ) / 2 - iOff/2, strText );
	  // Legende
	  cerr << "ptLegend.y() + xxx = " << ptLegend.y() + painter.fontMetrics().ascent() << endl;
	  painter.drawText( ptLegend.x() + iOff/2 - sizeText.width() / 2, ptLegend.y() + painter.fontMetrics().ascent(), strText );
	}

	// Legende
	painter.setBrush( Qt::black );
	painter.setPen( Qt::black );
	
	QString strValue;
	// TODO Format.raise(fRealValue, strValue);
	strValue.sprintf( "%f", fRealValue );
	while( strValue.right(1) == "0" )
	  strValue.truncate( strValue.length() - 1 );
	if ( strValue.right(1) == "." )
	  strValue.truncate( strValue.length() - 1 );
	cerr << "Val= " << fRealValue << ":" << strValue << endl;
	
	painter.drawText( ptLegend.x() + 2*iOff, ptLegend.y() + painter.fontMetrics().ascent(), strValue );
	painter.drawText( ptLegend.x() + 2*iOff + sizeValue, ptLegend.y() + painter.fontMetrics().ascent(), strLegend );
	
	fLast += fValue;
	fRealLast += fRealValue;
      }

      ++iter;
    }

    painter.restore();
  }
}


// Linendiagramm
///////////////////////////////////////////////////////////////////////
void KoChart::drawDiagrammLinien( QPainter& painter, int _width, int _height )
{
#ifndef OLDCODE
	if( m_diaType == DT_LINIEN ) {
		if( m_lastPainterType != Lines ) {
			delete _chartpainter;
			_chartpainter = new KChartLinesPainter( &_params );
			m_lastPainterType = Lines;
		}
		setupPainter( painter );
		_chartpainter->setWidthHeight(_width,_height);
		_chartpainter->paintChart( &painter, &m_table );
	} else if( m_diaType == DT_AREA ) {
		if( m_lastPainterType != Area ) {
			delete _chartpainter;
			_chartpainter = new KChartAreaPainter( &_params );
			m_lastPainterType = Area;
		}
		setupPainter( painter );
		_chartpainter->setWidthHeight(_width,_height);
		_chartpainter->paintChart( &painter, &m_table );
	}
#else
  // Exception
  if ( m_table.xDesc.count() < 2 )
    return;
  if ( m_table.yDesc.count() < 1 )
    return;

  // Farben
  QColor brushPie[MAX_KREIS_SECTIONS];
  brushPie[0].setRgb( 153,153,255 );
  brushPie[1].setRgb( 153, 51,102 );
  brushPie[2].setRgb( 255,104,104 );
  brushPie[3].setRgb( 104,104,255);
  brushPie[4].setRgb( 102,  0,102 );
  brushPie[5].setRgb( 255,128,128 );
  brushPie[6].setRgb( 0,102,204 );
  brushPie[7].setRgb( 0,204,255 );

  table_t table = m_table;

  // Konsolidierung
  if( table.xDesc.count() > MAX_LINIEN_SECTIONS )
  {
    // Konsolidierungsgröße
    int nKonsolidierung = ( table.xDesc.count() / MAX_LINIEN_SECTIONS ) + 1;
		
    // Fuer jede Zeile berechnen
    for( data_t::iterator lit = table.data.begin(); lit != table.data.end(); ++lit )
    {
      line_t newLine;
      int nKonsolStep = nKonsolidierung;
      for( line_t::iterator iter = lit->begin(); iter != lit->end(); ++iter )
      {
	line_t::reverse_iterator riter = newLine.rbegin();
	if( nKonsolStep == nKonsolidierung )
	{
	  // Neu anlegen
	  newLine.push_back( *iter);
	}
	else
        {
	  (*riter) += (*iter);
	}

	if(--nKonsolStep == 0)
	  nKonsolStep = nKonsolidierung;
      }

      *lit = newLine;
    }

    int nKonsolStep = nKonsolidierung;
    QStrList dest;
    const char *s;
    for( s = table.xDesc.first(); s != 0L; s = table.xDesc.next() )
    {
	if( nKonsolStep == nKonsolidierung )
	{
	  // Neu anlegen
	  dest.append( s );
	}
	
	if(--nKonsolStep == 0)
	  nKonsolStep = nKonsolidierung;
    }
    table.xDesc = dest;
  }

  QRect sizeText = painter.boundingRect( 0, 0, _width, _height, AlignLeft, "0" );
  int iOff = max(sizeText.height(), sizeText.width() );

  // Find longest description in yDesc
  int max_ydesc_len = 0;
  const char *s;
  for( s = table.yDesc.first(); s != 0L; s = table.yDesc.next() )
  {
    QRect tmp = painter.boundingRect( 0, 0, _width, _height, AlignLeft, s );
    max_ydesc_len = (int)max((double)max_ydesc_len, sqrt(0.5) * tmp.width() );
  }

  // Position & Size der Legende
  QRect legende( _width - 2*iOff - max_ydesc_len - iOff, iOff, 2*iOff + max_ydesc_len + iOff, _height - 2*iOff );
  if ( table.yDesc.count() == 1 )
    legende.setCoords( 0, 0, 0, 0 );

  // Find longest description in xDesc
  int max_xdesc_len = 0;
  for( s = table.xDesc.first(); s != 0L; s = table.xDesc.next() )
  {
    QRect tmp = painter.boundingRect( 0, 0, _width, _height, AlignLeft, s );
    max_xdesc_len = (int)max((double)max_xdesc_len, sqrt(0.5) * tmp.width() );
  }

  // Find maximal value
  double maxValue = 0.0;
  line_t::iterator iter;
  // Fuer jede Zeile berechnen
  for( data_t::iterator lit = table.data.begin(); lit != table.data.end(); ++lit )
  {
    for( iter = lit->begin(); iter != lit->end(); ++iter )
    {
      maxValue = max(maxValue, (*iter) );	
    }
  }

  // Exception
  if( maxValue == 0.0 )
    return;

  // in welchem Bereich liegt maxValue ? => Skalierung berechnen
  double dScale = 0.0;
  switch( m_dataType )
    {
    case DAT_NUMMER:
    case DAT_GEB:
      {
	KatDouble( maxValue, dScale );
      }
    break;
    case DAT_DAUER:
      {
	KatTime( maxValue, dScale );
     }
    break;
    }

  int nSteps = 0;
  double dStep = 0.0;

  if( maxValue <= 0.2 * dScale)
  {
    dStep = dScale * 0.02;
    nSteps = (int)ceil(maxValue/dStep);
  }
  else if( maxValue <= 0.5 * dScale )
  {
    dStep = dScale * 0.05;
    nSteps = (int)ceil(maxValue/dStep);
  }
  else
  {
    dStep = dScale * 0.1;
    nSteps = (int)ceil(maxValue/dStep);
  }

  // Hoehe des zu zeichnenden Diagramms
  QRect rcDraw( 0, 0, _width, _height );
  rcDraw.setTop( rcDraw.top() + sizeText.height() );
  rcDraw.setBottom( rcDraw.bottom() - max_xdesc_len - sizeText.height() - sizeText.height() );

  // Groesse eines Schrittes
  int nStep = rcDraw.height() / nSteps;

  // Maximal Laenge der verticalen Beschriftung ermitteln
  int sizeKat = 0;
  QString strFoo;
  for(int i = 0; i * nStep <= rcDraw.height(); i++ )
  {
    strFoo.sprintf( "%f", dStep * i );
    while( strFoo.right(1) == "0" )
      strFoo.truncate( strFoo.length() - 1 );
    if ( strFoo.right(1) == "." )
      strFoo.truncate( strFoo.length() - 1 );

    QRect tmp = painter.boundingRect( 0, 0, _width, _height, AlignLeft, strFoo );
    if ( tmp.width() > sizeKat )
      sizeKat = tmp.width();
  }

  // Breite des zu zeichnenden Diagramms
  rcDraw.setLeft( rcDraw.left() + sizeKat + sizeText.height() );
  rcDraw.setRight( rcDraw.right() - max(0, max_xdesc_len - sizeText.height() ) - sizeText.height() - legende.width() );

  // Exceptions
  if ( rcDraw.width() <= 0 || rcDraw.height() <= 0 )
    return;

  // Hintergrund zeichnen
  painter.setPen( lightGray );
  painter.setBrush( white );
  painter.drawRect( rcDraw );

  // Skalierung einzeichnen
  QString strStep;
  for(int i = 0; i * nStep <= rcDraw.height(); i++ )
  {
    painter.setPen( lightGray );
    painter.moveTo( rcDraw.left() - 10, rcDraw.bottom() - i * nStep );
    painter.lineTo( rcDraw.right(), rcDraw.bottom() - i * nStep );

    // Beschriftung
    strStep.sprintf( "%f", dStep * i );
    while( strStep.right(1) == "0" )
      strStep.truncate( strStep.length() - 1 );
    if ( strStep.right(1) == "." )
      strStep.truncate( strStep.length() - 1 );

    painter.setPen( black );
    QRect tmp( 0, rcDraw.bottom() - i * nStep, rcDraw.left() - 5, _height );
    painter.drawText( tmp, AlignRight, strStep );
  }

  // Strichmarkierungen horizontal zeichnen
  int nSaeulen	= min( MAX_LINIEN_SECTIONS, table.xDesc.count() ) - 1;
  int nSection	= rcDraw.width() / nSaeulen;

  painter.setPen( lightGray );
  for( int i = 0; i <= nSaeulen; i++ )
  {
    painter.moveTo( rcDraw.left() + i * nSection, rcDraw.bottom() );
    painter.lineTo( rcDraw.left() + i * nSection, rcDraw.bottom() + 10 );
  }

  // Horizontale Beschriftung einzeichnen
  unsigned int nCount = 0;
  painter.setPen( black );
  for( s = table.xDesc.first(); s != 0L && nCount < MAX_LINIEN_SECTIONS; s = table.xDesc.next(), ++nCount )
  {
    // Label
    painter.drawText( rcDraw.left() + nCount * nSection, rcDraw.bottom() + painter.fontMetrics().ascent() + 12, s );
  }

  int line = 0;
  // Linien zeichnen
  for( data_t::iterator lit = table.data.begin(); lit != table.data.end(); ++lit )
  {
    nCount = 0;
    list<QPoint> poly;
    // Startpunkt links unten
    if ( m_diaType == DT_AREA )
      poly.push_back( QPoint( rcDraw.left(), rcDraw.bottom() ) );

    for( iter = lit->begin(); iter != lit->end() && nCount < MAX_LINIEN_SECTIONS; ++iter, ++nCount )
    {
      double fHeight = *iter;
      fHeight *= (static_cast<double>(nStep)/dStep);

      poly.push_back( QPoint( rcDraw.left() + nCount * nSection, rcDraw.bottom() - (int)fHeight) );
    }

    // Endpunkt
    if ( m_diaType == DT_AREA )
      poly.push_back( QPoint( rcDraw.left() + ( nCount - 1 ) * nSection, rcDraw.bottom() ) );

    // Zeichnen
    QPointArray arr( poly.size() );
    list<QPoint>::iterator it = poly.begin();
    int i = 0;
    for( ; it != poly.end(); ++it )
      arr.setPoint( i++, *it );

    painter.setPen( brushPie[ line % MAX_KREIS_SECTIONS ] );
    painter.setBrush( brushPie[ line % MAX_KREIS_SECTIONS ] );
    if ( m_diaType == DT_AREA )
      painter.drawPolygon( arr );
    else
      painter.drawPolyline( arr );
    line++;
  }

  if ( table.yDesc.count() == 1 )
    return;

  line = 0;
  int y = 0;
  // Legende zeichnen
  for( s = table.yDesc.first(); s != 0L; s = table.yDesc.next() )
  {
    painter.setPen( brushPie[ line % MAX_KREIS_SECTIONS ] );
    painter.setBrush( brushPie[ line % MAX_KREIS_SECTIONS ] );
    QRect tmp( legende.x(), y + legende.y(), iOff, iOff );
    painter.drawEllipse( tmp );

    painter.setPen( black );
    painter.drawText( legende.x() + 2*iOff, legende.y() + y + painter.fontMetrics().ascent(), s );

    line++;
    y += painter.fontMetrics().height() * 3 / 2;
  }
#endif
}


void KoChart::clearData()
{
  m_table.data.clear();
  m_table.xDesc.clear();
  m_table.yDesc.clear();
  m_curTitle = "";
}


void KoChart::setData( table_t& data, const char* szTitle,
			data_type datType /*DAT_NUMMER*/, dia_type diaType /*=DT_KREIS_ODER_SAEULEN*/)
{
  m_table = data;

  m_curTitle = szTitle;
  m_diaType = diaType;
  m_dataType = datType;
}

/*
void KoChart::FormatAnzahl( double val, CString& out )
{
	out.Format("%d", (int)val);
}

void KoChart::FormatGebuehr( double val, CString& out )
{
	COleCurrencyExt cur( val );
	out = cur.FormatWithSymbol(2);
}

void KoChart::FormatDauer( double val, CString& out)
{
	COleDateTimeSpan date(0,0,0,(int)val );
	out.Format("%0d:%02d:%02d",
		date.GetDays()*24 +	date.GetHours(),
		date.GetMinutes(),
		date.GetSeconds()
	);
}
*/

void KoChart::KatDouble( double val, double& out )
{
  out = pow(10,(int) ceil(log10(val)) );
}

void KoChart::KatTime( double val, double& out )
{
  if( val <= 30)	// 1/2 Minute
    out = 30;
  else if( val <= 60)	// 1 Minute
    out = 60;
  else if( val <= 60*30)	// 1/2 Stunde
    out = 60*30;
  else if( val <= 60*60)	// 1 Stunde
    out = 60*60;
  else
  {
    double dTage = val/(60*60*24);
    out = ceil(pow(10, ceil(log10(dTage)) )) * 60*60*24;
  }
}


#include "koChart.moc"
