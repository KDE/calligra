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

#ifndef __diagramm_h__
#define __diagramm_h__

#include <map>
#include <list>
#include <string>
#include <vector>
#include <qstrlist.h>

typedef vector<double> line_t;
typedef vector<line_t> data_t;

struct table_t
{
  QStrList xDesc;
  QStrList yDesc;
  data_t data;
};

#ifndef OLDCODE
#include "KoDiagrammParameters.h"
class KChartPainter;
#endif

#include <qcolor.h>
#include <qrect.h>
#include <qpoint.h>
#include <qwidget.h>
#include <qstring.h>
#include <qpainter.h>

class KoDiagramm
{
public:
#ifndef OLDCODE
	KoDiagramm() {
		m_lastPainterType = NoneYet;
		_chartpainter = 0;
	}
#endif

  enum dia_type { DT_KREIS, DT_SAEULEN,	DT_KREIS_ODER_SAEULEN, DT_LINIEN, DT_AREA };
  enum data_type { DAT_NUMMER, DAT_GEB,	DAT_DAUER };

  void setData( table_t& data, const char* szTitle, data_type datType = DAT_NUMMER,
		dia_type diaType = DT_KREIS_ODER_SAEULEN);
  void clearData();

  void paint( QPainter& painter, int width, int height );

#ifndef OLDCODE
	void config( QWidget* parent );
#endif

protected:  
  void drawDiagrammKreis( QPainter& painter, int _width, int _height );
  void drawDiagrammLinien( QPainter& painter, int _width, int _height );
  void drawDiagrammSaeulen( QPainter& painter, int _width, int _height );
#ifndef OLDCODE
	void setupPainter( QPainter& painter );
#endif

  /* void FormatAnzahl ( double val, QString& out );
  void FormatGebuehr ( double val, QString& out );
  void FormatDauer ( double val, QString& out );
  */
  void KatDouble ( double val, double& out );
  void KatTime ( double val, double& out );

  table_t m_table;
  QString m_curTitle;
  dia_type m_diaType;
  data_type m_dataType;
#ifndef OLDCODE
	KChartType m_lastPainterType;
	KChartPainter* _chartpainter;
	KoDiagrammParameters _params;
#endif
};

class KoDiagrammView : public QWidget
{
  Q_OBJECT
public:
  KoDiagrammView( QWidget* _parent );
  virtual ~KoDiagrammView();

  KoDiagramm& diagramm() { return m_diagramm; }

  void paintEvent( QPaintEvent *_ev );
  void resizeEvent( QResizeEvent *_ve );
#ifndef OLDCODE
	void mousePressEvent( QMouseEvent* ev );
#endif
  
protected:
  KoDiagramm m_diagramm;
};

#endif
