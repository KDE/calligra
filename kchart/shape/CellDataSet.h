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

#ifndef KCHART_CELLDATASET_H
#define KCHART_CELLDATASET_H

// Local
#include "ChartShape.h"
#include "DataSet.h"

namespace KChart {

class CellDataSet : public DataSet
{
public:
	CellDataSet( ProxyModel *model );
    ~CellDataSet();
    
    QVariant xData( int index ) const;
    QVariant yData( int index ) const;
    QVariant customData( int index ) const;
    QVariant categoryData( int index ) const;
    QVariant labelData() const;

    CellRegion xDataRegion() const;
    CellRegion yDataRegion() const;
    CellRegion customDataRegion() const;
    CellRegion categoryDataRegion() const;
    CellRegion labelDataRegion() const;
    // TODO: Region for custom colors

    QString xDataRegionString() const;
    QString yDataRegionString() const;
    QString customDataRegionString() const;
    QString categoryDataRegionString() const;
    QString labelDataRegionString() const;

    void setXDataRegion( const CellRegion &region );
    void setYDataRegion( const CellRegion &region );
    void setCustomDataRegion( const CellRegion &region );
    void setCategoryDataRegion( const CellRegion &region );
    void setLabelDataRegion( const CellRegion &region );

    void setXDataRegionString( const QString &region );
    void setYDataRegionString( const QString &region );
    void setCustomDataRegionString( const QString &region );
    void setCategoryDataRegionString( const QString &region );
    void setLabelDataRegionString( const QString &region );

    // Called by the proxy model
    void yDataChanged( const QRect &region ) const;
    void xDataChanged( const QRect &region ) const;
    void customDataChanged( const QRect &region ) const;
    void labelDataChanged( const QRect &region ) const;
    void categoryDataChanged( const QRect &region ) const;

    int size() const;
};

} // Namespace KChart

#endif // KCHART_CELLDATASET_H

