/* This file is part of the KDE project

   Copyright 2007-2008 Johannes Simon <johannes.simon@gmail.com>

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

#ifndef KCHART_DATASET_H
#define KCHART_DATASET_H

// Local
#include "ChartShape.h"

namespace KChart {

class DataSet
{
public:
    DataSet( PlotArea *plotArea );
	~DataSet();
	
	// Getter methods
	QString title() const;
	ChartType chartType() const;
	Axis *attachedAxis() const;

    QString xValueCellRange() const;
    QString yValueCellRange() const;
    QString widthCellRange() const;
    
    ProxyModel *model() const;
	
    bool showValues() const;
    bool showLabels() const;
    
    QPen pen() const;
    QColor color() const;
    
	bool showMeanValue() const;
	QPen meanValuePen() const;
	
	bool showLowerErrorIndicator() const;
	bool showUpperErrorIndicator() const;
	QPen errorIndicatorPen() const;
	ErrorCategory errorCategory() const;
	double errorPercentage() const;
	double errorMargin() const;
	double lowerErrorLimit() const;
	double upperErrorLimit() const;
	
	// Setter methods
	void setChartType( ChartType type );
	void setAttachedAxis( Axis *axis );
	
	void setShowValues( bool showValues );
	void setShowLabels( bool showLabels );
	
	void setPen( const QPen &pen );
	void setColor( const QColor &color );
	
	void setShowMeanValue( bool b );
	void setMeanValuePen( const QPen &pen );

	void setShowLowerErrorIndicator( bool b );
	void setShowUpperErrorIndicator( bool b );
	void setShowErrorIndicators( bool lower, bool upper );
	void setErrorIndicatorPen( const QPen &pen );
	void setErrorCategory( ErrorCategory category );
	void setErrorPercentage( double percentage );
	void setErrorMargin( double margin );
	void setLowerErrorLimit( double limit );
	void setUpperErrorLimit( double limit );
	
private:
    class Private;
    Private *const d;
};

} // Namespace KChart

#endif // KCHART_DATASET_H
