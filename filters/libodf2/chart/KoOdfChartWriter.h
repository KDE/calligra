/*
 *  Copyright (c) 2010 Sebastian Sauer <sebsauer@kdab.com>
 *  Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
 *  Copyright (c) 2014-2015 Inge Wallin <inge@lysator.liu.se>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KOODFCHARTWRITER_H
#define KOODFCHARTWRITER_H

#include <Charting.h>

#include "koodf2_export.h"

class KoStore;
class KoXmlWriter;
class KoGenStyles;
class KoGenStyle;


class KOODF2_EXPORT KoOdfChartWriter
{
public:
    explicit KoOdfChartWriter(KoChart::Chart* chart);
    virtual ~KoOdfChartWriter();

    KoChart::Chart* chart() const { return m_chart; }

    void setSheetReplacement(bool val);

    void set2003ColorPalette(QList< QColor > palette);
    QString markerType(KoChart::MarkerType type, int currentSeriesNumber);

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
    qreal m_end_x, m_end_y; //in pt

    // Saving of content
            bool saveIndex(KoXmlWriter* xmlWriter);
    virtual bool saveContent(KoStore* store, KoXmlWriter* manifestWriter);
    virtual bool saveSeries(KoGenStyles &styles, KoGenStyles &mainStyles, KoXmlWriter* bodyWriter,
			    int maxExplode);

    // helper functions
    qreal calculateFade(int index, int maxIndex);
    QColor shadeColor(const QColor& col, qreal factor);

protected:
    KoChart::Chart* m_chart;

    QString normalizeCellRange(QString range);
    QString toPtString(int number);
    enum Orientation {
        vertical,
        horizontal
    };
    float sprcToPt(int sprc, Orientation orientation);
    QColor tintColor(const QColor & color, qreal tintfactor);
    QString replaceSheet(const QString &originalString, const QString &replacementSheet);
    bool sheetReplacement;

    // Style generation
            QString genChartAreaStyle(KoGenStyles& styles, KoGenStyles& mainStyles);
    virtual QString genChartAreaStyle(KoGenStyle& style, KoGenStyles& styles,
				      KoGenStyles& mainStyles);
            QString genPlotAreaStyle(KoGenStyles& styles, KoGenStyles& mainStyles);
    virtual QString genPlotAreaStyle(KoGenStyle& style, KoGenStyles& styles,
				     KoGenStyles& mainStyles);
            void addShapePropertyStyle(/*const*/ KoChart::Series* series,
				       KoGenStyle& style, KoGenStyles& mainStyles);
    virtual void addDataThemeToStyle(KoGenStyle& style,
				     int dataNumber, int maxNumData = 1, bool strokes = true);

    QString generateGradientStyle(KoGenStyles& mainStyles, const KoChart::Gradient* grad);

    // Color functions
    virtual QColor calculateColorFromGradientStop(const KoChart::Gradient::GradientStop& grad);
    virtual QColor labelFontColor() const;
    void writeInternalTable(KoXmlWriter* bodyWriter);

    // MS Office related stuff
    QList<QColor> m_palette;
    // tells if a 2003 color palette has been set
    bool paletteIsSet;
};

#endif  // KOODFCHARTWRITER_H
