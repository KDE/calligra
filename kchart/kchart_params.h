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
	BoxWhisker = KDChartParams::BoxWhisker,

	// Only in KChart
	BarLines
    } ChartType;

    // Data direction
    typedef  enum {
	DataRows    = 0,
	DataColumns = 1
    } DataDirection;


    KChartParams( KChartPart *_part );
    ~KChartParams();

    // Reimplementation of selected KDChartParams methods
    // FIXME: Enhance for BarLines
    ChartType  chartType() const            { return m_chartType; }
    void       setChartType( ChartType _type ) {
	m_chartType = _type;
	if ( _type != BarLines )
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
    // BARLINES CHART-SPECIFIC

    enum BarlinesChartSubType {
	BarlinesNormal,
	BarlinesStacked,
	BarlinesPercent
    };


public slots:
    void setBarlinesChartSubType( BarlinesChartSubType _barlinesChartSubType ) {
	m_barlinesChartSubType = _barlinesChartSubType;
	emit changed();
    }

    BarlinesChartSubType barlinesChartSubType() const {
	return m_barlinesChartSubType;
    }

    void setBarlinesNumLines( int _numLines ) {
	m_barlinesNumLines = _numLines;
	emit changed();
    }

    int barlinesNumLines() const {
	return m_barlinesNumLines;
    }


    static QString barlinesChartSubTypeToString( BarlinesChartSubType type );
    static BarChartSubType stringToBarlinesChartSubType( const QString& string );

 private:
    BarlinesChartSubType  m_barlinesChartSubType;
    int                   m_barlinesNumLines;

    // ----------------------------------------------------------------

 public:

    DCOPObject  *dcopObject();

    bool loadOasis( const QDomElement     &chartElem,
		    KoOasisLoadingContext &loadingContext,
                    QString               &errorMessage,
		    KoStore               *store );
    void saveOasis( KoXmlWriter* bodyWriter, KoGenStyles& mainStyles );

 private:
    bool loadOasisPlotarea( const QDomElement     &plotareaElem,
			    KoOasisLoadingContext &loadingContext,
			    QString               &errorMessage,
			    KoStore               *store );

 private:
    KChartPart    *m_part;

    // The chart itself.
    ChartType      m_chartType;
    DataDirection  m_dataDirection; // Rows or Columns

    bool           m_firstRowAsLabel;
    bool           m_firstColAsLabel;

    DCOPObject    *m_dcop;
};

}  //KChart namespace

#endif
