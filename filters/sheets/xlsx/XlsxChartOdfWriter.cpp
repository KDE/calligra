/*
 *  Copyright (c) 2010 Sebastian Sauer <sebsauer@kdab.com>
 *  Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
 *  Copyright (c) 2014 Inge Wallin <inge@lysator.liu.se>
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

// Own
#include "XlsxChartOdfWriter.h"

// Calligra
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoStoreDevice.h>
#include <KoGenStyles.h>
#include <KoGenStyle.h>


#include <Charting.h>
#include "NumberFormatParser.h"
#include <MsooXmlTheme.h>

#include <algorithm> // For std:find()

// Print the content of generated content.xml to the console for debugging purpose
//#define CONTENTXML_DEBUG

using namespace KoChart;

XlsxChartOdfWriter::XlsxChartOdfWriter(KoChart::Chart* chart, const MSOOXML::DrawingMLTheme* const theme)
    : KoOdfChartWriter(chart)
    , m_theme(theme)
{
    Q_ASSERT(m_chart);
    m_drawLayer = false;
}

XlsxChartOdfWriter::~XlsxChartOdfWriter()
{
}


QColor XlsxChartOdfWriter::calculateColorFromGradientStop(const KoChart::Gradient::GradientStop& grad)
{
    QColor color = grad.knownColorValue;

    if (!grad.referenceColor.isEmpty())
        color = m_theme->colorScheme.value(grad.referenceColor)->value();

    const int tintedColor = 255 * grad.tintVal / 100.0;
    const qreal  nonTindedPart = 1.0 - grad.tintVal / 100.0;
    color.setRed(tintedColor + nonTindedPart * color.red());
    color.setGreen(tintedColor + nonTindedPart * color.green());
    color.setBlue(tintedColor + nonTindedPart * color.blue());

    return color;
}

QColor XlsxChartOdfWriter::labelFontColor() const
{
    bool useTheme = !chart()->m_areaFormat && m_theme;
    if (useTheme) {
        // The following assumes that we just need to invert the in
        // genChartAreaStyle used font-color for the axis. It's not clear yet
        // (means any documentation in the specs is missing) if that is really
        // the correct thing to do.
        const MSOOXML::DrawingMLColorScheme& colorScheme = m_theme->colorScheme;
        switch(chart()->m_style) {
            case(33):
            case(34):
            case(35):
            case(36):
            case(37):
            case(38):
            case(39):
            case(40): {
                return colorScheme.value("dk1")->value();
            } break;

            case(41):
            case(42):
            case(43):
            case(44):
            case(45):
            case(46):
            case(47):
            case(48): {
                return colorScheme.value("lt1")->value();
            } break;

            default:
                break;
        }
    }

    return QColor();
}

QString XlsxChartOdfWriter::genChartAreaStyle(KoGenStyle& style, KoGenStyles& styles,
					      KoGenStyles& mainStyles)
{
    if (chart()->m_fillGradient) {
        style.addProperty("draw:fill", "gradient", KoGenStyle::GraphicType);
        style.addProperty("draw:fill-gradient-name",
			  generateGradientStyle(mainStyles, chart()->m_fillGradient),
			  KoGenStyle::GraphicType);
    } else {
        style.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
        bool useTheme = !chart()->m_areaFormat && m_theme;
        if (useTheme) {
            const MSOOXML::DrawingMLColorScheme& colorScheme = m_theme->colorScheme;
            switch(chart()->m_style) {
                case(33):
                case(34):
                case(35):
                case(36):
                case(37):
                case(38):
                case(39):
                case(40): {
                    style.addProperty("draw:fill-color", colorScheme.value("lt1")->value().name(),
				      KoGenStyle::GraphicType);
                } break;
                case(41):
                case(42):
                case(43):
                case(44):
                case(45):
                case(46):
                case(47):
                case(48): {
                    style.addProperty("draw:fill-color", colorScheme.value("dk1")->value().name(),
				      KoGenStyle::GraphicType);
                } break;

                default: {
                    useTheme = false;
                } break;
            }
        }

        if (!useTheme) {
            QColor color;
            if (chart()->m_areaFormat
		&& chart()->m_areaFormat->m_fill
		&& chart()->m_areaFormat->m_foreground.isValid())
	    {
                color = chart()->m_areaFormat->m_foreground;
	    }
            else
                color = QColor("#FFFFFF");
            style.addProperty("draw:fill-color", color.name(), KoGenStyle::GraphicType);

            if (color.alpha() < 255)
                style.addProperty("draw:opacity",
				  QString("%1%").arg(chart()->m_areaFormat->m_foreground.alphaF()
						     * 100.0),
				  KoGenStyle::GraphicType);
        }
    }

    return styles.insert(style, "ch");
}


QString XlsxChartOdfWriter::genPlotAreaStyle(KoGenStyle& style, KoGenStyles& styles,
					     KoGenStyles& mainStyles)
{
    KoChart::AreaFormat *areaFormat = ((chart()->m_plotArea
					&& chart()->m_plotArea->m_areaFormat
					&& chart()->m_plotArea->m_areaFormat->m_fill)
				       ? chart()->m_plotArea->m_areaFormat
				       : chart()->m_areaFormat);
    if (chart()->m_plotAreaFillGradient) {
        style.addProperty("draw:fill", "gradient", KoGenStyle::GraphicType);
        style.addProperty("draw:fill-gradient-name",
			  generateGradientStyle(mainStyles, chart()->m_plotAreaFillGradient),
			  KoGenStyle::GraphicType);
    } else {
        style.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
        bool useTheme = !areaFormat && m_theme;
        if (useTheme) {
            const MSOOXML::DrawingMLColorScheme& colorScheme = m_theme->colorScheme;
            switch(chart()->m_style) {
                case(33):
                case(34): {
                    style.addProperty("draw:fill-color",
				      tintColor(colorScheme.value("dk1")->value(), 0.2).name(),
				      KoGenStyle::GraphicType);
                } break;
                case(35):
                case(36):
                case(37):
                case(38):
                case(39):
                case(40): {
                    QString prop = QString::fromLatin1("accent%1").arg(chart()->m_style - 34);
                    style.addProperty("draw:fill-color",
				      colorScheme.value("dk1")->value().name(),
				      KoGenStyle::GraphicType);
                } break;
                case(41):
                case(42):
                case(43):
                case(44):
                case(45):
                case(46):
                case(47):
                case(48): {
                    style.addProperty("draw:fill-color",
				      tintColor(colorScheme.value("dk1")->value(), 0.95).name(),
				      KoGenStyle::GraphicType);
                } break;

                default: {
                    useTheme = false;
                } break;
            }
        }

        if (!useTheme) {
            QColor color;
            if (areaFormat && areaFormat->m_foreground.isValid())
                color = areaFormat->m_foreground;
            else
                color = QColor(paletteIsSet ? "#C0C0C0" : "#FFFFFF");
            style.addProperty("draw:fill-color", color.name(), KoGenStyle::GraphicType);

            if (color.alpha() < 255)
                style.addProperty("draw:opacity",
				  QString("%1%").arg(areaFormat->m_foreground.alphaF() * 100.0),
				  KoGenStyle::GraphicType);
        }
    }

    return styles.insert(style, "ch");
}


// ----------------------------------------------------------------
//                 The actual saving code


// We don't have to do anything special here in the case of Xlsx.


// ----------------------------------------------------------------
//                   Some helper functions


void XlsxChartOdfWriter::addDataThemeToStyle(KoGenStyle& style, int dataNumber, int maxNumData,
					     bool strokes)
{
    if (!m_theme) return;

    const int patternOneIndexes[] = { 1, 9, 17, 25, 33 };
    const int patternTwoIndexes[] = { 42, 34, 26, 18, 10, 2 };
    const int patternFourIndexes[] = { 41 };
    
    const int fadepatternOne[] = { 3, 11, 19, 27, 35, 43 };
    const int fadepatternTwo[] = { 4, 12, 20, 28, 36, 44 };
    const int fadepatternThree[] = { 5, 13, 21, 29, 37, 45 };
    const int fadepatternFour[] = { 6, 14, 22, 30, 38, 46 };
    const int fadepatternFive[] = { 7, 15, 23, 31, 39, 47 };
    const int fadepatternSix[] = { 8, 16, 24, 32, 40, 48 };
    QVector< const int* > fadePatterns; fadePatterns << fadepatternOne << fadepatternTwo
						     << fadepatternThree << fadepatternFour
						     << fadepatternFive << fadepatternSix;

    const MSOOXML::DrawingMLColorScheme& colorScheme = m_theme->colorScheme;
    const int rounds = dataNumber / 6;
    const int maxRounds = maxNumData / 6 + 1;
    QColor seriesColor;
    if (std::find(patternTwoIndexes, patternTwoIndexes + 6, chart()->m_style)
	!= patternTwoIndexes + 6)
    {
        const QString themeColorString = QString::fromLatin1("accent%1").arg((dataNumber % 6) + 1);
        const qreal tintFactor = 1.0 - (rounds / maxRounds * 2);
        MSOOXML::DrawingMLColorSchemeItemBase *colorSchemeItem = colorScheme.value(themeColorString);
        if (colorSchemeItem) {
            seriesColor = colorSchemeItem->value();
            if (rounds > 1)
                seriesColor = tintColor(seriesColor, tintFactor);
        }
    }
    else if (std::find(patternOneIndexes, patternOneIndexes + 5, chart()->m_style)
	     != patternOneIndexes + 5)
    {
        const QString themeColorString = QString::fromLatin1("dk1");
        MSOOXML::DrawingMLColorSchemeItemBase *colorSchemeItem = colorScheme.value(themeColorString);
        if (colorSchemeItem) {
            seriesColor = colorSchemeItem->value();
            const qreal tintVals[] = { 0.885, 0.55, 0.78, 0.925, 0.7, 0.3 };
            seriesColor = tintColor(seriesColor, tintVals[ dataNumber % 6 ]);
            const qreal tintFactor = 1.0 - (rounds / maxRounds * 2);
            if (rounds > 1)
                seriesColor = tintColor(seriesColor, tintFactor);
        }
    }
    else if (std::find(patternFourIndexes, patternFourIndexes + 1, chart()->m_style)
	     != patternFourIndexes + 1)
    {
        const QString themeColorString = QString::fromLatin1("dk1");
        MSOOXML::DrawingMLColorSchemeItemBase *colorSchemeItem = colorScheme.value(themeColorString);
        if (colorSchemeItem) {
            seriesColor = colorSchemeItem->value();
            const qreal tintVals[] = { 0.885, 0.55, 0.78, 0.925, 0.7, 0.3 };
            seriesColor = tintColor(seriesColor, tintVals[ dataNumber % 6 ]);
            const qreal tintFactor = 1.0 - (rounds / maxRounds * 2);
            if (rounds > 1)
                seriesColor = tintColor(seriesColor, tintFactor);
        }
    }
    else {
        for (int i = 0; i < fadePatterns.count(); ++i) {
            if (std::find(fadePatterns[ i ], fadePatterns[ i ] + 6, chart()->m_style)
		!= fadePatterns[ i ] + 6)
	    {
                const QString themeColorString = QString::fromLatin1("accent%1").arg(i + 1);
                MSOOXML::DrawingMLColorSchemeItemBase *colorSchemeItem = colorScheme.value(themeColorString);
                if (colorSchemeItem) {
                    seriesColor = colorSchemeItem->value();
                    qreal fadeValue = calculateFade(dataNumber, maxNumData) / 100.0;
                    if (fadeValue > 0.0)
                        seriesColor = tintColor(seriesColor, 1 - fadeValue);
                    else
                        seriesColor = shadeColor(seriesColor, 1 + fadeValue);
                }
            }
        }
    }

    if (seriesColor.isValid()) {
        style.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
        style.addProperty("draw:fill-color", seriesColor.name(), KoGenStyle::GraphicType);

        if (strokes) {
            style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
            style.addProperty("svg:stroke-color", seriesColor.name(), KoGenStyle::GraphicType);
        } else {
            style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
        }
    }
}
