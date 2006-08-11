/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#ifndef KCHART_PARAMS_H
#define KCHART_PARAMS_H


class KoXmlWriter;
class KoGenStyles;
class KoOasisLoadingContext;
class KoStore;
class KDChartParams;
class DCOPObject;


#include "kdchart/KDChartParams.h"


namespace KChart
{

class KChartPart;

class KChartParams : public KDChartParams
{
  public:
    typedef enum {
	// From KDChart
	NoType     = KDChartParams::NoType,
	Bar        = KDChartParams::Bar,
	Line       = KDChartParams::Line,
	Area       = KDChartParams::Area,
	Pie        = KDChartParams::Pie,
	HiLo       = KDChartParams::HiLo,
	Ring       = KDChartParams::Ring,
	Polar      = KDChartParams::Polar,
	BoxWhisker = KDChartParams::BoxWhisker

    } ChartType;

    // Data direction
    typedef  enum {
	DataRows    = 0,
	DataColumns = 1
    } DataDirection;


    KChartParams( KChartPart *_part );
    ~KChartParams();

    KChartPart * part() const { return m_part; }

    // Reimplementation of selected KDChartParams methods
    ChartType  chartType() const            { return m_chartType; }
    void       setChartType( ChartType _type ) {
	m_chartType = _type;
	KDChartParams::setChartType( (KDChartParams::ChartType) _type );
    }

    // Data in rows or columns.
    DataDirection  dataDirection() const    { return m_dataDirection; }
    void           setDataDirection( DataDirection _dir ) {
	m_dataDirection = _dir;
    }

    QString    chartTypeToString( ChartType _type) const;
    ChartType  stringToChartType( const QString& string );


    bool       firstRowAsLabel() const { return m_firstRowAsLabel; }
    void       setFirstRowAsLabel( bool _val );
    bool       firstColAsLabel() const { return m_firstColAsLabel; }
    void       setFirstColAsLabel( bool _val );


    // ----------------------------------------------------------------
    // BAR CHART EXTENSIONS TO SUPPORT OPENDOCUMENT

public slots:

    void setBarNumLines( int _numLines ) {
	m_barNumLines = _numLines;
	emit changed();
    }

    int barNumLines() const {
	return m_barNumLines;
    }

    // ----------------------------------------------------------------

 public:

    DCOPObject  *dcopObject();

    bool loadOasis( const QDomElement     &chartElem,
		    KoOasisLoadingContext &loadingContext,
                    QString               &errorMessage,
		    KoStore               *store );
    void saveOasis( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles ) const;

 private:
    bool loadOasisPlotarea( const QDomElement     &plotareaElem,
			    KoOasisLoadingContext &loadingContext,
			    QString               &errorMessage );
    bool loadOasisAxis( const QDomElement         &axisElem,
			KoOasisLoadingContext     &loadingContext,
			QString                   &errorMessage,
			KDChartAxisParams::AxisPos axisPos );
    void loadOasisFont( KoOasisLoadingContext& context, QFont& font, QColor& color );

    void saveOasisPlotArea( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles ) const;
    void saveOasisAxis( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles,
                        KDChartAxisParams::AxisPos axisPos, 
			const char* axisName ) const;
    QString saveOasisFont( KoGenStyles& mainStyles, const QFont& font, 
			   const QColor& color ) const;
    
 private:
    KChartPart    *m_part;

    // Info about the chart itself.
    ChartType      m_chartType;

    // Info about the data.
    DataDirection  m_dataDirection; // Rows or Columns
    bool           m_firstRowAsLabel;
    bool           m_firstColAsLabel;

    // Extensions to support OpenDocument
    int            m_barNumLines; // Number of lines in a bar chart.

    DCOPObject    *m_dcop;
};

}  //KChart namespace

#endif
