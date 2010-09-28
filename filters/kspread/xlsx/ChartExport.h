/*
 *  Copyright (c) 2010 Sebastian Sauer <sebsauer@kdab.com>
 *  Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef CHARTEXPORT_H
#define CHARTEXPORT_H

#include <Charting.h>

//#include <QtCore/QString>
//#include <QtCore/QStringList>
//#include <QtCore/QRect>
//#include <QtCore/QMap>
//#include <QtGui/QColor>

class KoStore;
class KoXmlWriter;
class KoGenStyles;
class KoGenStyle;

namespace MSOOXML
{
    class DrawingMLTheme;
}


class ChartExport
{
public:
    explicit ChartExport(Charting::Chart* chart, const MSOOXML::DrawingMLTheme* const contextWithThemeInformation = NULL);
    ~ChartExport();
    Charting::Chart* chart() const { return m_chart; }
    void setSheetReplacement( bool val );

    bool m_drawLayer;
    QString m_href;
    QString m_cellRangeAddress;
    QString m_endCellAddress;
    QString m_notifyOnUpdateOfRanges;
#if 0
    /// anchored to sheet
    QString m_sheetName;
    /// anchored to cell
    //unsigned long m_colL, m_rwT;
#endif
    qreal m_x, m_y, m_width, m_height; //in pt

    bool saveIndex(KoXmlWriter* xmlWriter);
    bool saveContent(KoStore* store, KoXmlWriter* manifestWriter);

private:
    QString toPtString( int number );
    enum Orientation {
        vertical,
        horizontal
    };
    float sprcToPt( int sprc, Orientation orientation );
    Charting::Chart* m_chart;
    const MSOOXML::DrawingMLTheme* m_theme;
    bool sheetReplacement;
    QString genChartAreaStyle( const int styleID, KoGenStyles& styles, KoGenStyles& mainStyles );
    QString genChartAreaStyle( const int styleID, KoGenStyle& style, KoGenStyles& styles, KoGenStyles& mainStyles );
    QString genPlotAreaStyle( const int styleID, KoGenStyles& styles, KoGenStyles& mainStyles );
    QString genPlotAreaStyle( const int styleID, KoGenStyle& style, KoGenStyles& styles, KoGenStyles& mainStyles );
    void addDataThemeToStyle( const int styleID, KoGenStyle& style, int dataNumber, int maxNumData = 1);
    QString generateGradientStyle( KoGenStyles& mainStyles, const Charting::Gradient* grad );
    QColor calculateColorFromGradientStop( const Charting::Gradient::GradientStop& grad );
    void writeInternalTable ( KoXmlWriter* bodyWriter );
};

#endif
