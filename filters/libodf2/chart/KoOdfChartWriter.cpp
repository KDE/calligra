/*
 *  Copyright (c) 2010 Sebastian Sauer <sebsauer@kdab.com>
 *  Copyright (c) 2010 Carlos Licea <carlos@kdab.com>
 *  Copyright (c) 2014 Inge Wallin <inge@lysator.liu.se>
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

// Own
#include "KoOdfChartWriter.h"

// libstdc++
#include <algorithm> // For std:find()

// Calligra
#include <KoStore.h>
#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoStoreDevice.h>
#include <KoGenStyles.h>
#include <KoGenStyle.h>

#include "Odf2Debug.h"
#include <Charting.h>
#include "NumberFormatParser.h"


// Print the content of generated content.xml to the console for debugging purpose
//#define CONTENTXML_DEBUG

using namespace KoChart;

KoOdfChartWriter::KoOdfChartWriter(KoChart::Chart* chart)
    : m_x(0)
    , m_y(0)
    , m_width(0)
    , m_height(0)
    , m_end_x(0)
    , m_end_y(0)
    , m_chart(chart)
    , sheetReplacement(true)
    , paletteIsSet(false)
{
    Q_ASSERT(m_chart);
    m_drawLayer = false;
}

KoOdfChartWriter::~KoOdfChartWriter()
{
}


// Takes a Excel cellrange and translates it into a ODF cellrange
QString KoOdfChartWriter::normalizeCellRange(QString range)
{
    if (range.startsWith('[') && range.endsWith(']')) {
        range.remove(0, 1).chop(1);
    }
    range.remove('$');

    const bool isPoint = !range.contains( ':' );
    QRegExp regEx(isPoint ? "(|.*\\.|.*\\!)([A-Z0-9]+)"
		          : "(|.*\\.|.*\\!)([A-Z]+[0-9]+)\\:(|.*\\.|.*\\!)([A-Z0-9]+)");
    if (regEx.indexIn(range) >= 0) {
        range.clear();
        QString sheetName = regEx.cap(1);
        if (sheetName.endsWith(QLatin1Char('.')) || sheetName.endsWith(QLatin1Char('!')))
            sheetName.chop(1);
        if (!sheetName.isEmpty())
            range = sheetName + '.';
        range += regEx.cap(2);
        if (!isPoint)
            range += ':' + regEx.cap(4);
    }

    return range;
}

QColor KoOdfChartWriter::tintColor(const QColor & color, qreal tintfactor)
{
    QColor retColor;
    const qreal  nonTindedPart = 1.0 - tintfactor;
    qreal luminance = 0.0;
    qreal sat = 0.0;
    qreal hue = 0.0;
    color.getHslF(&hue, &sat, &luminance);
    luminance = luminance * tintfactor + nonTindedPart;
    retColor.setHslF(hue, sat, luminance);
//     const int tintedColor = 255 * nonTindedPart;
//     retColor.setRed(tintedColor + tintfactor * color.red());
//     retColor.setGreen(tintedColor + tintfactor * color.green());
//     retColor.setBlue(tintedColor + tintfactor * color.blue());

    return retColor;
}

QColor KoOdfChartWriter::calculateColorFromGradientStop(const KoChart::Gradient::GradientStop& grad)
{
    QColor color = grad.knownColorValue;

    const int tintedColor = 255 * grad.tintVal / 100.0;
    const qreal  nonTindedPart = 1.0 - grad.tintVal / 100.0;
    color.setRed(tintedColor + nonTindedPart * color.red());
    color.setGreen(tintedColor + nonTindedPart * color.green());
    color.setBlue(tintedColor + nonTindedPart * color.blue());

    return color;
}

QString KoOdfChartWriter::generateGradientStyle(KoGenStyles& mainStyles,
						const KoChart::Gradient* grad)
{
    KoGenStyle gradStyle(KoGenStyle::GradientStyle);
    gradStyle.addAttribute("draw:style", "linear");

    QColor startColor = calculateColorFromGradientStop(grad->gradientStops.first());
    QColor endColor = calculateColorFromGradientStop(grad->gradientStops.last());
    
    gradStyle.addAttribute("draw:start-color", startColor.name());
    gradStyle.addAttribute("draw:end-color", endColor.name());
    gradStyle.addAttribute("draw:angle", QString::number(grad->angle));

    return mainStyles.insert(gradStyle, "ms_chart_gradient");
}

QColor KoOdfChartWriter::labelFontColor() const
{
    return QColor();
}

QString KoOdfChartWriter::genChartAreaStyle(KoGenStyle& style, KoGenStyles& styles,
					    KoGenStyles& mainStyles)
{
    if (chart()->m_fillGradient) {
        style.addProperty("draw:fill", "gradient", KoGenStyle::GraphicType);
        style.addProperty("draw:fill-gradient-name",
			  generateGradientStyle(mainStyles, chart()->m_fillGradient),
			  KoGenStyle::GraphicType);
    } else {
	style.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);

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

    return styles.insert(style, "ch");
}


QString KoOdfChartWriter::genChartAreaStyle(KoGenStyles& styles, KoGenStyles& mainStyles)
{
    KoGenStyle style(KoGenStyle::GraphicAutoStyle, "chart");

    return genChartAreaStyle(style, styles, mainStyles);
}


QString KoOdfChartWriter::genPlotAreaStyle(KoGenStyle& style, KoGenStyles& styles,
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

    return styles.insert(style, "ch");
}


void KoOdfChartWriter::addShapePropertyStyle(/*const*/ KoChart::Series* series, KoGenStyle& style,
					     KoGenStyles& /*mainStyles*/)
{
    Q_ASSERT(series);
    bool marker = false;
    KoChart::ScatterImpl* impl = dynamic_cast< KoChart::ScatterImpl* >(m_chart->m_impl);

    if (impl)
        marker = (impl->style == KoChart::ScatterImpl::Marker 
		  || impl->style == KoChart::ScatterImpl::LineMarker);

    if (series->spPr->lineFill.valid) {
        if (series->spPr->lineFill.type == KoChart::Fill::Solid) {
            style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
            style.addProperty("svg:stroke-color", series->spPr->lineFill.solidColor.name(),
			      KoGenStyle::GraphicType);
        }
        else if (series->spPr->lineFill.type == KoChart::Fill::None) {
            style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
	}
    }
    else if (  (paletteIsSet && m_chart->m_impl->name() != "scatter")
             || m_chart->m_showLines)
    {
        const int curSerNum = m_chart->m_series.indexOf(series);
        style.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);
        style.addProperty("svg:stroke-color", m_palette.at(24 + curSerNum).name(),
			  KoGenStyle::GraphicType);
    }
    else if (paletteIsSet && m_chart->m_impl->name() == "scatter")
        style.addProperty("draw:stroke", "none", KoGenStyle::GraphicType);
    if (series->spPr->areaFill.valid) {
        if (series->spPr->areaFill.type == KoChart::Fill::Solid) {
            style.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
            style.addProperty("draw:fill-color", series->spPr->areaFill.solidColor.name(),
			      KoGenStyle::GraphicType);
        }
        else if (series->spPr->areaFill.type == KoChart::Fill::None)
            style.addProperty("draw:fill", "none", KoGenStyle::GraphicType);
    }
    else if (paletteIsSet
	     && !(m_chart->m_markerType != KoChart::NoMarker || marker)
	     && series->m_markerType == KoChart::NoMarker)
    {
        const int curSerNum = m_chart->m_series.indexOf(series) % 8;
        style.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
        style.addProperty("draw:fill-color", m_palette.at(16 + curSerNum).name(),
			  KoGenStyle::GraphicType);
    }
}

QString KoOdfChartWriter::genPlotAreaStyle(KoGenStyles& styles, KoGenStyles& mainStyles)
{
    KoGenStyle style(KoGenStyle::ChartAutoStyle/*, "chart"*/);
    return genPlotAreaStyle(style, styles, mainStyles);
}

QString KoOdfChartWriter::replaceSheet(const QString &originalString,
				       const QString &replacementSheet)
{
    QStringList split = originalString.split(QLatin1Char('!'));
    split[0] = replacementSheet;
    return split.join(QString::fromLatin1("!"));
}

void KoOdfChartWriter::set2003ColorPalette(QList < QColor > palette)
{
    m_palette = palette;
    paletteIsSet = true;
}

QString KoOdfChartWriter::markerType(KoChart::MarkerType type, int currentSeriesNumber)
{
    QString markerName;
    switch(type) {
        case NoMarker:
            break;
        case AutoMarker: { // auto marker type
            const int resNum = currentSeriesNumber % 3;
            if (resNum == 0)
                markerName = "square";
            else if (resNum == 1)
                markerName = "diamond";
            else if (resNum == 2)
                markerName = "circle";
        } break;
        case SquareMarker:
            markerName = "square";
            break;
        case DiamondMarker:
            markerName = "diamond";
            break;
        case StarMarker:
            markerName = "star";
            break;
        case TriangleMarker:
            markerName = "arrow-up";
            break;
        case DotMarker:
            markerName = "dot";
            break;
        case PlusMarker:
            markerName = "plus";
            break;
        case SymbolXMarker:
            markerName = "x";
            break;
        case CircleMarker:
            markerName = "circle";
            break;
        case DashMarker:
            markerName = "horizontal-bar";
            break;
    }

    return markerName;
}


// ----------------------------------------------------------------
//                 The actual saving code


bool KoOdfChartWriter::saveIndex(KoXmlWriter* xmlWriter)
{
    if (!chart() || m_href.isEmpty())
        return false;

    // This because for presesentations the frame is done in read_graphicFrame
    if (!m_drawLayer) {
        xmlWriter->startElement("draw:frame");
        // used in opendocumentpresentation for layers
        //if (m_drawLayer)
        //    xmlWriter->addAttribute("draw:layer", "layout");

        // used in opendocumentspreadsheet to reference cells
        if (!m_endCellAddress.isEmpty()) {
            xmlWriter->addAttribute("table:end-cell-address", m_endCellAddress);
            xmlWriter->addAttributePt("table:end-x", m_end_x);
            xmlWriter->addAttributePt("table:end-y", m_end_y);
        }

        xmlWriter->addAttributePt("svg:x", m_x);
        xmlWriter->addAttributePt("svg:y", m_y);
        if (m_width > 0)
            xmlWriter->addAttributePt("svg:width", m_width);
        if (m_height > 0)
            xmlWriter->addAttributePt("svg:height", m_height);
    }
    //xmlWriter->addAttribute("draw:z-index", "0");
    xmlWriter->startElement("draw:object");
    //TODO don't show on e.g. presenter
    if (!m_notifyOnUpdateOfRanges.isEmpty())
        xmlWriter->addAttribute("draw:notify-on-update-of-ranges", m_notifyOnUpdateOfRanges);

    xmlWriter->addAttribute("xlink:href", "./" + m_href);
    xmlWriter->addAttribute("xlink:type", "simple");
    xmlWriter->addAttribute("xlink:show", "embed");
    xmlWriter->addAttribute("xlink:actuate", "onLoad");

    xmlWriter->endElement(); // draw:object
    if (!m_drawLayer) {
        xmlWriter->endElement(); // draw:frame
    }
    return true;
}

bool KoOdfChartWriter::saveContent(KoStore* store, KoXmlWriter* manifestWriter)
{
    if (!chart() || !chart()->m_impl || m_href.isEmpty())
        return false;
    
    KoGenStyles styles;
    KoGenStyles mainStyles;

    store->pushDirectory();
    store->enterDirectory(m_href);

    KoOdfWriteStore s(store);
    KoXmlWriter* bodyWriter = s.bodyWriter();
    KoXmlWriter* contentWriter = s.contentWriter();
    Q_ASSERT(bodyWriter && contentWriter);

    bodyWriter->startElement("office:body");
    bodyWriter->startElement("office:chart");

    //<chart:chart chart:class="chart:circle"
    //             svg:width="8cm" svg:height="7cm"
    //             chart:style-name="ch1">
    bodyWriter->startElement("chart:chart");

    if (!chart()->m_impl->name().isEmpty()) {
        bodyWriter->addAttribute("chart:class", "chart:" + chart()->m_impl->name());
    }

    if (m_width > 0) {
        bodyWriter->addAttributePt("svg:width", m_width);
    }
    if (m_height > 0) {
        bodyWriter->addAttributePt("svg:height", m_height);
    }
    
    bodyWriter->addAttribute("chart:style-name", genChartAreaStyle(styles, mainStyles));

    // <chart:title svg:x="5.618cm" svg:y="0.14cm" chart:style-name="ch2">
    //     <text:p>PIE CHART</text:p>
    // </chart:title>
    if (!chart()->m_title.isEmpty()) {
        bodyWriter->startElement("chart:title");

        /* TODO we can't determine this because by default we need to center the title,
        in order to center it we need to know the textbox size, and to do that we need
        the used font metrics.

        Also, for now, the default implementation of KChart centers
        the title, so we get close to the expected behavior. We ignore any offset though.

        Nonetheless, the formula should be something like this:
        const int widht = m_width/2 - textWidth/2 + sprcToPt(t->m_x1, vertical);
        const int height = m_height/2 - textHeight/2 + sprcToPt(t->m_y1, horizontal);
        bodyWriter->addAttributePt("svg:x", width);
        bodyWriter->addAttributePt("svg:y", height);
        */

        // NOTE: Don't load width or height, the record MUST be ignored and
        //       determined by the application
        // see [MS-XLS] p. 362

        bodyWriter->startElement("text:p");
        bodyWriter->addTextNode(chart()->m_title);
        bodyWriter->endElement(); // text:p
        bodyWriter->endElement(); // chart:title
    }

    // Legend
    if (chart()->m_legend) {
        bodyWriter->startElement("chart:legend");
        bodyWriter->addAttribute("chart:legend-position", "end");

        KoGenStyle legendstyle(KoGenStyle::ChartAutoStyle, "chart");

        QColor labelColor = labelFontColor();
        if (labelColor.isValid())
            legendstyle.addProperty("fo:font-color", labelColor.name(), KoGenStyle::TextType);

        bodyWriter->addAttribute("chart:style-name", styles.insert(legendstyle, "lg"));

        bodyWriter->endElement(); // chart:legend
    }

    // <chart:plot-area chart:style-name="ch3"
    //                  table:cell-range-address="Sheet1.C2:Sheet1.E2"
    //                  svg:x="0.16cm" svg:y="0.14cm">
    bodyWriter->startElement("chart:plot-area");

    if (chart()->m_is3d) {
        //bodyWriter->addAttribute("dr3d:transform", "matrix (0.893670830886674 0.102940425033731 -0.436755898547686 -0.437131441492021 0.419523087196176 -0.795560483036015 0.101333848646097 0.901888933407692 0.419914042293545 0cm 0cm 0cm)");
        //bodyWriter->addAttribute("dr3d:vrp", "(12684.722548717 7388.35827488833 17691.2795565958)");
        //bodyWriter->addAttribute("dr3d:vpn", "(0.416199821709347 0.173649045905254 0.892537795986984)");
        //bodyWriter->addAttribute("dr3d:vup", "(-0.0733876362771618 0.984807599917971 -0.157379306090273)");
        //bodyWriter->addAttribute("dr3d:projection", "parallel");
        //bodyWriter->addAttribute("dr3d:distance", "4.2cm");
        //bodyWriter->addAttribute("dr3d:focal-length", "8cm");
        //bodyWriter->addAttribute("dr3d:shadow-slant", "0");
        //bodyWriter->addAttribute("dr3d:shade-mode", "flat");
        //bodyWriter->addAttribute("dr3d:ambient-color", "#b3b3b3");
        //bodyWriter->addAttribute("dr3d:lighting-mode", "true");
    }

    KoGenStyle chartstyle(KoGenStyle::ChartAutoStyle, "chart");
    //chartstyle.addProperty("chart:connect-bars", "false");
    //chartstyle.addProperty("chart:include-hidden-cells", "false");
    chartstyle.addProperty("chart:auto-position", "true");
    chartstyle.addProperty("chart:auto-size", "true");
    chartstyle.addProperty("chart:angle-offset", chart()->m_angleOffset);

    //chartstyle.addProperty("chart:series-source", "rows");
    //chartstyle.addProperty("chart:sort-by-x-values", "false");
    //chartstyle.addProperty("chart:right-angled-axes", "true");
    if (chart()->m_is3d) {
        chartstyle.addProperty("chart:three-dimensional", "true");
    }
    //chartstyle.addProperty("chart:angle-offset", "90");
    //chartstyle.addProperty("chart:series-source", "rows");
    //chartstyle.addProperty("chart:right-angled-axes", "false");
    if (chart()->m_transpose) {
        chartstyle.addProperty("chart:vertical", "true");
    }
    if (chart()->m_stacked) {
        chartstyle.addProperty("chart:stacked", "true");
    }
    if (chart()->m_f100) {
        chartstyle.addProperty("chart:percentage", "true");
    }
    bodyWriter->addAttribute("chart:style-name", genPlotAreaStyle(chartstyle, styles, mainStyles));

    QString verticalCellRangeAddress = chart()->m_verticalCellRangeAddress;
// FIXME microsoft treats the regions from this area in a different order, so don't use it or x and y values will be switched
//     if (!chart()->m_cellRangeAddress.isEmpty()) {
//         if (sheetReplacement)
//             bodyWriter->addAttribute("table:cell-range-address", replaceSheet(normalizeCellRange(m_cellRangeAddress), QString::fromLatin1("local"))); //"Sheet1.C2:Sheet1.E5");
//         else
//             bodyWriter->addAttribute("table:cell-range-address", normalizeCellRange(m_cellRangeAddress)); //"Sheet1.C2:Sheet1.E5");
//     }

    /*FIXME
    if (verticalCellRangeAddress.isEmpty()) {
        // only add the chart:data-source-has-labels if no chart:categories with a table:cell-range-address was defined within an axis.
        bodyWriter->addAttribute("chart:data-source-has-labels", "both");
    }
    */

    //bodyWriter->addAttribute("svg:x", "0.16cm"); //FIXME
    //bodyWriter->addAttribute("svg:y", "0.14cm"); //FIXME
    //bodyWriter->addAttribute("svg:width", "6.712cm"); //FIXME
    //bodyWriter->addAttribute("svg:height", "6.58cm"); //FIXME

    const bool definesCategories = chart()->m_impl->name() != "scatter"; // scatter charts are using domains
    int countXAxis = 0;
    int countYAxis = 0;
    foreach (KoChart::Axis* axis, chart()->m_axes) {
        //TODO handle series-axis
        if (axis->m_type == KoChart::Axis::SeriesAxis) continue;

        bodyWriter->startElement("chart:axis");

        KoGenStyle axisstyle(KoGenStyle::ChartAutoStyle, "chart");

        if (axis->m_reversed)
            axisstyle.addProperty("chart:reverse-direction", "true", KoGenStyle::ChartType);

        //FIXME this hits an infinite-looping bug in kdchart it seems... maybe fixed with a newer version
//         if (axis->m_logarithmic)
//             axisstyle.addProperty("chart:logarithmic", "true", KoGenStyle::ChartType);

        if (!axis->m_autoMinimum)
            axisstyle.addProperty("chart:minimum", QString::number(axis->m_minimum, 'f'),
				  KoGenStyle::ChartType);
        if (!axis->m_autoMaximum)
            axisstyle.addProperty("chart:maximum", QString::number(axis->m_maximum, 'f'),
				  KoGenStyle::ChartType);

        axisstyle.addProperty("fo:font-size", QString("%0pt").arg(chart()->m_textSize),
			      KoGenStyle::TextType);

        QColor labelColor = labelFontColor();
        if (labelColor.isValid())
            axisstyle.addProperty("fo:font-color", labelColor.name(), KoGenStyle::TextType);

        if (!axis->m_numberFormat.isEmpty()) {
            const KoGenStyle style = NumberFormatParser::parse(axis->m_numberFormat, &styles);
            axisstyle.addAttribute("style:data-style-name", styles.insert(style, "ds"));
        }

        bodyWriter->addAttribute("chart:style-name", styles.insert(axisstyle, "ch"));

        switch(axis->m_type) {
            case KoChart::Axis::VerticalValueAxis:
                bodyWriter->addAttribute("chart:dimension", "y");
                bodyWriter->addAttribute("chart:name", QString("y%1").arg(++countYAxis));
                break;
            case KoChart::Axis::HorizontalValueAxis:
                bodyWriter->addAttribute("chart:dimension", "x");
                bodyWriter->addAttribute("chart:name", QString("x%1").arg(++countXAxis));
                if (countXAxis == 1 && definesCategories && !verticalCellRangeAddress.isEmpty()) {
                    bodyWriter->startElement("chart:categories");
                    if (sheetReplacement)
                        verticalCellRangeAddress
			    = normalizeCellRange(replaceSheet(verticalCellRangeAddress,
							      QString::fromLatin1("local")));
                    else
                        verticalCellRangeAddress = normalizeCellRange(verticalCellRangeAddress);
                    bodyWriter->addAttribute("table:cell-range-address", verticalCellRangeAddress); //"Sheet1.C2:Sheet1.E2");
                    bodyWriter->endElement();
                }
                break;
            default: break;
        }

        if (axis->m_majorGridlines.m_format.m_style != KoChart::LineFormat::None) {
            bodyWriter->startElement("chart:grid");
            bodyWriter->addAttribute("chart:class", "major");
            bodyWriter->endElement(); // chart:grid
        }

        if (axis->m_minorGridlines.m_format.m_style != KoChart::LineFormat::None) {
            bodyWriter->startElement("chart:grid");
            bodyWriter->addAttribute("chart:class", "minor");
            bodyWriter->endElement(); // chart:grid
        }
        bodyWriter->endElement(); // chart:axis
    }

    // Add at least one x-axis.
    if (countXAxis == 0) {
        bodyWriter->startElement("chart:axis");
        bodyWriter->addAttribute("chart:dimension", "x");
        bodyWriter->addAttribute("chart:name", "primary-x");

        if (definesCategories && !verticalCellRangeAddress.isEmpty()) {
            bodyWriter->startElement("chart:categories");
            if (sheetReplacement)
                verticalCellRangeAddress
		    = normalizeCellRange(replaceSheet(verticalCellRangeAddress,
						      QString::fromLatin1("local")));
            else
                verticalCellRangeAddress = normalizeCellRange(verticalCellRangeAddress);

            bodyWriter->addAttribute("table:cell-range-address", verticalCellRangeAddress);
            bodyWriter->endElement();
        }

        bodyWriter->endElement(); // chart:axis
    }

    // Add at least one y-axis.
    if (countYAxis == 0) {
        bodyWriter->startElement("chart:axis");
        bodyWriter->addAttribute("chart:dimension", "y");
        bodyWriter->addAttribute("chart:name", "primary-y");
        bodyWriter->endElement(); // chart:axis
    }

    //<chart:axis chart:dimension="x" chart:name="primary-x" chart:style-name="ch4"/>
    //<chart:axis chart:dimension="y" chart:name="primary-y" chart:style-name="ch5"><chart:grid chart:style-name="ch6" chart:class="major"/></chart:axis>

    // NOTE: The XLS format specifies that if an explodeFactor that is > 100
    //       is found, we should find the biggest and make it 100, then scale
    //       all the other factors accordingly.
    // see 2.4.195 PieFormat
    int maxExplode = 100;
    foreach (KoChart::Series* series, chart()->m_series) {
        foreach (KoChart::Format* f, series->m_datasetFormat) {
            if (KoChart::PieFormat* pieformat = dynamic_cast<KoChart::PieFormat*>(f)) {
                if (pieformat->m_pcExplode > 0) {
                    maxExplode = qMax(maxExplode, pieformat->m_pcExplode);
                }
            }
        }
    }

    // Area diagrams are special in that Excel displays the areas in another
    // order than OpenOffice.org and Calligra Sheets. To make sure the same areas are
    // visible we do the same as OpenOffice.org does and reverse the order.
    if (chart()->m_impl->name() == "area") {
        for (int i = chart()->m_series.count() - 1; i >= 0; --i) {
            chart()->m_series.append(chart()->m_series.takeAt(i));
        }
    }

    // Save the series.
    if (!saveSeries(styles, mainStyles, bodyWriter, maxExplode))
	return false;

    bodyWriter->startElement("chart:wall");
    bodyWriter->endElement(); // chart:wall

    bodyWriter->startElement("chart:floor");
    bodyWriter->endElement(); // chart:floor

    bodyWriter->endElement(); // chart:plot-area

    writeInternalTable(bodyWriter);

    bodyWriter->endElement(); // chart:chart
    bodyWriter->endElement(); // office:chart
    bodyWriter->endElement(); // office:body

#ifdef CONTENTXML_DEBUG
    debugOdf2 << bodyWriter->toString();
#endif

    styles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, contentWriter);
    s.closeContentWriter();

    if (store->open("styles.xml")) {
        KoStoreDevice dev(store);
        KoXmlWriter* stylesWriter = new KoXmlWriter(&dev);

        stylesWriter->startDocument("office:document-styles");
        stylesWriter->startElement("office:document-styles");
        stylesWriter->addAttribute("xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0");
        stylesWriter->addAttribute("xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0");
        stylesWriter->addAttribute("xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0");
        stylesWriter->addAttribute("xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0");
        stylesWriter->addAttribute("xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0");
        stylesWriter->addAttribute("xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0");
        stylesWriter->addAttribute("xmlns:svg", "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0");
        stylesWriter->addAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
        stylesWriter->addAttribute("xmlns:chart", "urn:oasis:names:tc:opendocument:xmlns:chart:1.0");
        stylesWriter->addAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
        stylesWriter->addAttribute("xmlns:meta", "urn:oasis:names:tc:opendocument:xmlns:meta:1.0");
        stylesWriter->addAttribute("xmlns:number", "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0");
        stylesWriter->addAttribute("xmlns:dr3d", "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0");
        stylesWriter->addAttribute("xmlns:math", "http://www.w3.org/1998/Math/MathML");
        stylesWriter->addAttribute("xmlns:of", "urn:oasis:names:tc:opendocument:xmlns:of:1.2");
        stylesWriter->addAttribute("office:version", "1.2");
        mainStyles.saveOdfStyles(KoGenStyles::MasterStyles, stylesWriter);
        mainStyles.saveOdfStyles(KoGenStyles::DocumentStyles, stylesWriter); // office:style
        mainStyles.saveOdfStyles(KoGenStyles::DocumentAutomaticStyles, stylesWriter); // office:automatic-styles
        stylesWriter->endElement();  // office:document-styles
        stylesWriter->endDocument();

        delete stylesWriter;
        store->close();
    }

    manifestWriter->addManifestEntry(m_href + QLatin1Char('/'), "application/vnd.oasis.opendocument.chart");
    manifestWriter->addManifestEntry(QString("%1/styles.xml").arg(m_href), "text/xml");
    manifestWriter->addManifestEntry(QString("%1/content.xml").arg(m_href), "text/xml");

    store->popDirectory();
    return true;
}

// FIXME: We should probably create a KoOdfChartWriterContext out of these
//        parameters when we add more similar saving functions later.
bool KoOdfChartWriter::saveSeries(KoGenStyles &styles, KoGenStyles &mainStyles,
				  KoXmlWriter* bodyWriter, int maxExplode)
{
    int curSerNum = 0;
    bool lines = true;
    bool marker = false;
    Q_FOREACH (KoChart::Series* series, chart()->m_series) {
        lines = true;
        if (chart()->m_impl->name() == "scatter" && !paletteIsSet) {
            KoChart::ScatterImpl* impl = static_cast< KoChart::ScatterImpl* >(chart()->m_impl);
            lines = (impl->style == KoChart::ScatterImpl::Line
		     || impl->style == KoChart::ScatterImpl::LineMarker);
            marker = (impl->style == KoChart::ScatterImpl::Marker
		      || impl->style == KoChart::ScatterImpl::LineMarker);
        }
        const bool noLineFill = ((series->spPr != 0)
				 && series->spPr->lineFill.type == KoChart::Fill::None);
        lines = lines && !noLineFill;
        lines = lines || m_chart->m_showLines;
        
	// <chart:series chart:style-name="ch7"
	//               chart:values-cell-range-address="Sheet1.C2:Sheet1.E2"
	//               chart:class="chart:circle">
        bodyWriter->startElement("chart:series");
        KoGenStyle seriesstyle(KoGenStyle::GraphicAutoStyle, "chart");
        if (series->spPr)
            addShapePropertyStyle(series, seriesstyle, mainStyles);
        else if (lines && paletteIsSet) {
            lines = false;
            seriesstyle.addProperty("draw:stroke", "solid", KoGenStyle::GraphicType);      
            seriesstyle.addProperty("svg:stroke-color", m_palette.at(24 + curSerNum).name(),
				    KoGenStyle::GraphicType);
        }

        if (paletteIsSet
	    && m_chart->m_impl->name() != "ring"
	    && m_chart->m_impl->name() != "circle")
	{
            if (series->m_markerType == KoChart::NoMarker
		&& m_chart->m_markerType == KoChart::NoMarker
		&& !marker)
            {
		seriesstyle.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
		seriesstyle.addProperty("draw:fill-color", m_palette.at(16 + curSerNum).name(),
					KoGenStyle::GraphicType);
            }
        }

        if (series->m_markerType != KoChart::NoMarker) {
            QString markerName = markerType(series->m_markerType, curSerNum);
            if (!markerName.isEmpty()) {
                seriesstyle.addProperty("chart:symbol-type", "named-symbol", KoGenStyle::ChartType);
                seriesstyle.addProperty("chart:symbol-name", markerName, KoGenStyle::ChartType);
            }
        }
        else if (m_chart->m_markerType != KoChart::NoMarker || marker) {
            QString markerName = markerType(m_chart->m_markerType == KoChart::NoMarker
					    ? KoChart::AutoMarker
					    : m_chart->m_markerType, curSerNum);
            if (!markerName.isEmpty()) {
                seriesstyle.addProperty("chart:symbol-type", "named-symbol", KoGenStyle::ChartType);
                seriesstyle.addProperty("chart:symbol-name", markerName, KoGenStyle::ChartType);
            }
        }

        if (chart()->m_impl->name() != "circle" && chart()->m_impl->name() != "ring")
            addDataThemeToStyle(seriesstyle, curSerNum, chart()->m_series.count(), lines);
        //seriesstyle.addProperty("draw:stroke", "solid");
        //seriesstyle.addProperty("draw:fill-color", "#ff0000");

        foreach (KoChart::Format* f, series->m_datasetFormat) {
            if (KoChart::PieFormat* pieformat = dynamic_cast<KoChart::PieFormat*>(f)) {
                if (pieformat->m_pcExplode > 0) {
                    // Note that 100.0/maxExplode will yield 1.0 most of the
                    // time, that's why do that division first
                    const int pcExplode = (int)((float)pieformat->m_pcExplode * (100.0 / (float)maxExplode));
                    seriesstyle.addProperty("chart:pie-offset", pcExplode, KoGenStyle::ChartType);
                }
            }
        }

        if (series->m_showDataLabelValues && series->m_showDataLabelPercent) {
            seriesstyle.addProperty("chart:data-label-number", "value-and-percentage",
				    KoGenStyle::ChartType);
        } else if (series->m_showDataLabelValues) {
            seriesstyle.addProperty("chart:data-label-number", "value", KoGenStyle::ChartType);
        } else if (series->m_showDataLabelPercent) {
            seriesstyle.addProperty("chart:data-label-number", "percentage", KoGenStyle::ChartType);
        }

        if (series->m_showDataLabelCategory) {
            seriesstyle.addProperty("chart:data-label-text", "true", KoGenStyle::ChartType);
        }
        //seriesstyle.addProperty("chart:data-label-symbol", "true", KoGenStyle::ChartType);

        if (!series->m_numberFormat.isEmpty()) {
            const KoGenStyle style = NumberFormatParser::parse(series->m_numberFormat, &styles);
            seriesstyle.addAttribute("style:data-style-name", styles.insert(style, "ds"));
        }

        bodyWriter->addAttribute("chart:style-name", styles.insert(seriesstyle, "ch"));

        // ODF does not support custom labels so we depend on the
        // SeriesLegendOrTrendlineName being defined and to point to a valid
        // cell to be able to display custom labels.
        if (series->m_datasetValue.contains(KoChart::Value::SeriesLegendOrTrendlineName)) {
            KoChart::Value* v = series->m_datasetValue[KoChart::Value::SeriesLegendOrTrendlineName];
            if (!v->m_formula.isEmpty()) {
                bodyWriter->addAttribute("chart:label-cell-address",
					 (v->m_type == KoChart::Value::CellRange
					  ? normalizeCellRange(v->m_formula)
					  : v->m_formula));
            }
        }

        if (!series->m_labelCell.isEmpty()) {
            QString labelAddress = series->m_labelCell;
            if (sheetReplacement)
                labelAddress = normalizeCellRange(replaceSheet(labelAddress,
							       QString::fromLatin1("local")));
            else
                labelAddress = normalizeCellRange(labelAddress);
            bodyWriter->addAttribute("chart:label-cell-address", labelAddress);
        }

        QString valuesCellRangeAddress;
        if (sheetReplacement)
            valuesCellRangeAddress
		= normalizeCellRange(replaceSheet(series->m_valuesCellRangeAddress,
						  QString::fromLatin1("local")));
        else
            valuesCellRangeAddress = normalizeCellRange(series->m_valuesCellRangeAddress);
        
        if (!valuesCellRangeAddress.isEmpty()) {
	    // "Sheet1.C2:Sheet1.E2";
            bodyWriter->addAttribute("chart:values-cell-range-address", valuesCellRangeAddress);
        }
        else if (!series->m_domainValuesCellRangeAddress.isEmpty()) {
	    // "Sheet1.C2:Sheet1.E2";
            bodyWriter->addAttribute("chart:values-cell-range-address",
				     series->m_domainValuesCellRangeAddress.last());
        }

        bodyWriter->addAttribute("chart:class", "chart:" + chart()->m_impl->name());

//         if (chart()->m_impl->name() == "scatter") {
//             bodyWriter->startElement("chart:domain");
//             bodyWriter->addAttribute("table:cell-range-address", verticalCellRangeAddress); //"Sheet1.C2:Sheet1.E5");
//             bodyWriter->endElement();
//         } else if (chart()->m_impl->name() == "bubble") {

            QString domainRange;
            Q_FOREACH (const QString& curRange, series->m_domainValuesCellRangeAddress) {
                bodyWriter->startElement("chart:domain");
                if (sheetReplacement)
                    domainRange = normalizeCellRange(replaceSheet(curRange, QString::fromLatin1("local")));
                else
                    domainRange = normalizeCellRange(curRange);
                if (!domainRange.isEmpty())
                    bodyWriter->addAttribute("table:cell-range-address", domainRange);
                bodyWriter->endElement();
            }
//             if (series->m_domainValuesCellRangeAddress.count() == 1){
//                 bodyWriter->startElement("chart:domain");
//                 bodyWriter->addAttribute("table:cell-range-address", series->m_domainValuesCellRangeAddress.last()); //"Sheet1.C2:Sheet1.E5");
//                 bodyWriter->endElement();
//             }
//             if (series->m_domainValuesCellRangeAddress.isEmpty()){
//                 bodyWriter->startElement("chart:domain");
//                 bodyWriter->addAttribute("table:cell-range-address", series->m_valuesCellRangeAddress); //"Sheet1.C2:Sheet1.E5");
//                 bodyWriter->endElement();
//                 bodyWriter->startElement("chart:domain");
//                 bodyWriter->addAttribute("table:cell-range-address", series->m_valuesCellRangeAddress); //"Sheet1.C2:Sheet1.E5");
//                 bodyWriter->endElement();
//             }
//         }

        for (int j = 0; j < series->m_countYValues; ++j) {
            bodyWriter->startElement("chart:data-point");
            KoGenStyle gs(KoGenStyle::GraphicAutoStyle, "chart");

            if (chart()->m_impl->name() == "circle" || chart()->m_impl->name() == "ring") {
                QColor fillColor;
                if (j < series->m_dataPoints.count()) {
                    KoChart::DataPoint *dataPoint = series->m_dataPoints[j];
                    if (dataPoint->m_areaFormat) {
                        fillColor = dataPoint->m_areaFormat->m_foreground;
                    }
                }

                if (fillColor.isValid()) {
                    gs.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
                    gs.addProperty("draw:fill-color", fillColor.name(), KoGenStyle::GraphicType);
                }
                else if (series->m_markerType == KoChart::NoMarker
			 && m_chart->m_markerType == KoChart::NoMarker
			 && !marker)
		{
                    if (paletteIsSet) {
                        gs.addProperty("draw:fill", "solid", KoGenStyle::GraphicType);
                        gs.addProperty("draw:fill-color", m_palette.at(16 + j).name(),
				       KoGenStyle::GraphicType);
                    }
                    else {
                        addDataThemeToStyle(gs, j, series->m_countYValues, lines);
                    }
                }
            }/*
            else
            {
                addSeriesThemeToStyle(gs, curSerNum, chart()->m_series.count());
            }*/

            //gs.addProperty("chart:solid-type", "cuboid", KoGenStyle::ChartType);
            //gs.addProperty("draw:fill-color",j==0?"#004586":j==1?"#ff420e":"#ffd320",
	    //               KoGenStyle::GraphicType);
            bodyWriter->addAttribute("chart:style-name", styles.insert(gs, "ch"));

            Q_FOREACH (KoChart::Text* t, series->m_texts) {
                bodyWriter->startElement("chart:data-label");
                bodyWriter->startElement("text:p");
                bodyWriter->addTextNode(t->m_text);
                bodyWriter->endElement();
                bodyWriter->endElement();
            }

            bodyWriter->endElement();
        }
        
        ++curSerNum;
        bodyWriter->endElement(); // chart:series
    }

    return true;
}


// ----------------------------------------------------------------
//                   Some helper functions


// Calculate fade factor as suggested in msoo xml reference page 4161
qreal KoOdfChartWriter::calculateFade(int index, int maxIndex)
{
    return -70.0 + 140.0 * ((double) index / ((double) maxIndex + 1.0));
}

QColor KoOdfChartWriter::shadeColor(const QColor& col, qreal factor)
{
    QColor result = col;
    qreal luminance = 0.0;
    qreal hue = 0.0;
    qreal sat = 0.0;
    result.getHslF(&hue, &sat, &luminance);
    luminance *= factor;
    result.setHslF(hue, sat, luminance);
    return result;
}

void KoOdfChartWriter::addDataThemeToStyle(KoGenStyle& style, int dataNumber, int maxNumData,
					   bool strokes)
{
    // FIXME: This is only relevant to themes, so remove this function after
    //        we are done with saveContent().
    Q_UNUSED(style);
    Q_UNUSED(dataNumber);
    Q_UNUSED(maxNumData);
    Q_UNUSED(strokes);
}


float KoOdfChartWriter::sprcToPt(int sprc, Orientation orientation )
{
    if (orientation & vertical)
        return (float)sprc * ( (float)m_width / 4000.0);

    return (float)sprc * ( (float)m_height / 4000.0);
}

void KoOdfChartWriter::writeInternalTable(KoXmlWriter* bodyWriter)
{
    Q_ASSERT( bodyWriter );
    bodyWriter->startElement("table:table");
        bodyWriter->addAttribute( "table:name", "local" );

        bodyWriter->startElement( "table:table-header-columns" );
            bodyWriter->startElement( "table:table-column" );
            bodyWriter->endElement();
        bodyWriter->endElement();

        bodyWriter->startElement( "table:table-columns" );
            bodyWriter->startElement( "table:table-column" );
            bodyWriter->endElement();
        bodyWriter->endElement();

        bodyWriter->startElement( "table:table-rows" );

        const int rowCount = chart()->m_internalTable.maxRow();
        for (int r = 1; r <= rowCount; ++r) {
            bodyWriter->startElement("table:table-row");
            const int columnCount = chart()->m_internalTable.maxCellsInRow(r);
            for (int c = 1; c <= columnCount; ++c) {
                bodyWriter->startElement("table:table-cell");
                if (Cell* cell = chart()->m_internalTable.cell(c, r, false)) {
                    //debugOdf2 << "cell->m_value " << cell->m_value;
                    if (!cell->m_value.isEmpty()) {
                        if (!cell->m_valueType.isEmpty()) {
                            bodyWriter->addAttribute("office:value-type", cell->m_valueType);
                            if (cell->m_valueType == "string") {
                                bodyWriter->addAttribute("office:string-value", cell->m_value);
                            } else if (cell->m_valueType == "boolean") {
                                bodyWriter->addAttribute("office:boolean-value", cell->m_value);
                            } else if (cell->m_valueType == "date") {
                                bodyWriter->addAttribute("office:date-value", cell->m_value);
                            } else if (cell->m_valueType == "time") {
                                bodyWriter->addAttribute("office:time-value", cell->m_value);
                            } else { // float, percentage and currency including fraction and scientific
                                bodyWriter->addAttribute("office:value", cell->m_value);
                            }
                        }

                        bodyWriter->startElement("text:p");
                        bodyWriter->addTextNode( cell->m_value );
                        bodyWriter->endElement(); // text:p
                    }
                }
                bodyWriter->endElement(); // table:table-cell
            }
            bodyWriter->endElement(); // table:table-row
        }
        bodyWriter->endElement(); // table:table-rows
    bodyWriter->endElement(); // table:table
}

void KoOdfChartWriter::setSheetReplacement( bool val )
{
    sheetReplacement = val;
}
