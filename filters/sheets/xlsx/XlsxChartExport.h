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

#ifndef XLSXCHARTEXPORT_H
#define XLSXCHARTEXPORT_H

#include <ChartExport.h>


class KoStore;
class KoXmlWriter;
class KoGenStyles;
class KoGenStyle;

namespace MSOOXML
{
    class DrawingMLTheme;
}


class XlsxChartExport : public ChartExport
{
public:
    explicit XlsxChartExport(KoChart::Chart* chart,
			     const MSOOXML::DrawingMLTheme* const contextWithThemeInfo = NULL);
    ~XlsxChartExport();


private:

    // All of these are virtual functions called from other functions (mainly
    // saveContent()) in the parent class.

    QString genChartAreaStyle(KoGenStyle& style, KoGenStyles& styles, KoGenStyles& mainStyles);
    QString genPlotAreaStyle(KoGenStyle& style, KoGenStyles& styles, KoGenStyles& mainStyles);
    void addDataThemeToStyle(KoGenStyle& style,
			     int dataNumber, int maxNumData = 1, bool strokes = true);

    QColor calculateColorFromGradientStop(const KoChart::Gradient::GradientStop& grad);
    QColor labelFontColor() const;

private:
    const MSOOXML::DrawingMLTheme* m_theme;
};

#endif  // XLSXCHARTEXPORT_H
