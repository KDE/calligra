/* This file is part of the KDE project
   Copyright 1998-2016 The Calligra Team <calligra-devel@kde.org>
   Copyright 2016 Tomas Mecir <mecirt@gmail.com>
   Copyright 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2005-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2002 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#include "SheetsOdf.h"
#include "SheetsOdfPrivate.h"

#include <kcodecs.h>

#include <KoDocumentInfo.h>
#include <KoGenStyles.h>
#include <KoProgressUpdater.h>
#include <KoShape.h>
#include <KoShapeRegistry.h>
#include "KoStore.h"
#include <KoStyleStack.h>
#include "KoUnit.h"
#include <KoUpdater.h>
#include <KoXmlNS.h>
#include <KoXmlWriter.h>

#include "CellStorage.h"
#include "Condition.h"
#include "DocBase.h"
#include "Formula.h"
#include "HeaderFooter.h"
#include "LoadingInfo.h"
#include "Map.h"
#include "PrintSettings.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "ShapeApplicationData.h"
#include "StyleManager.h"
#include "StyleStorage.h"
#include "Validity.h"

// This file contains functionality to load/save a Sheet

namespace Calligra {
namespace Sheets {

class Cell;

template<typename T> class IntervalMap
{
public:
    IntervalMap() {}
    // from and to are inclusive, assumes no overlapping ranges
    // even though no checks are done
    void insert(int from, int to, const T& data) {
        m_data.insert(to, qMakePair(from, data));
    }
    T get(int idx) const {
        typename QMap<int, QPair<int, T> >::ConstIterator it = m_data.lowerBound(idx);
        if (it != m_data.end() && it.value().first <= idx) {
            return it.value().second;
        }
        return T();
    }
private:
    QMap<int, QPair<int, T> > m_data;
};


namespace Odf {
    // Sheet loading - helper functions
    /**
     * Inserts the styles contained in \p styleRegions into the style storage.
     * Looks automatic styles up in the map of preloaded automatic styles,
     * \p autoStyles , and custom styles in the StyleManager.
     * The region is restricted to \p usedArea .
     */
    void loadSheetInsertStyles(Sheet *sheet, const Styles& autoStyles,
                             const QHash<QString, QRegion>& styleRegions,
                             const QHash<QString, Conditions>& conditionalStyles,
                             const QRect& usedArea,
                             QList<QPair<QRegion, Style> >& outStyleRegions,
                             QList<QPair<QRegion, Conditions> >& outConditionalStyles);

    bool loadStyleFormat(Sheet *sheet, KoXmlElement *style);
    void loadMasterLayoutPage(Sheet *sheet, KoStyleStack &styleStack);
    void loadRowNodes(Sheet *sheet, const KoXmlElement& parent,
                            int& rowIndex,
                            int& maxColumn,
                            OdfLoadingContext& tableContext,
                            QHash<QString, QRegion>& rowStyleRegions,
                            QHash<QString, QRegion>& cellStyleRegions,
                            const IntervalMap<QString>& columnStyles,
                            const Styles& autoStyles,
                            QList<ShapeLoadingData>& shapeData);
    void loadColumnNodes(Sheet *sheet, const KoXmlElement& parent,
                            int& indexCol,
                            int& maxColumn,
                            KoOdfLoadingContext& odfContext,
                            QHash<QString, QRegion>& columnStyleRegions,
                            IntervalMap<QString>& columnStyles);
    bool loadColumnFormat(Sheet *sheet, const KoXmlElement& column,
                             const KoOdfStylesReader& stylesReader, int & indexCol,
                             QHash<QString, QRegion>& columnStyleRegions, IntervalMap<QString>& columnStyles);
    int loadRowFormat(Sheet *sheet, const KoXmlElement& row, int &rowIndex,
                          OdfLoadingContext& tableContext,
                          QHash<QString, QRegion>& rowStyleRegions,
                          QHash<QString, QRegion>& cellStyleRegions,
                          const IntervalMap<QString>& columnStyles,
                          const Styles& autoStyles,
                          QList<ShapeLoadingData>& shapeData);
    QString getPart(const KoXmlNode & part);
    void replaceMacro(QString & text, const QString & old, const QString & newS);

    // Sheet saving - helper functions
    QString saveSheetStyleName(Sheet *sheet, KoGenStyles &mainStyles);
    void saveColRowCell(Sheet *sheet, int maxCols, int maxRows, OdfSavingContext& tableContext);
    void saveCells(Sheet *sheet, int row, int maxCols, OdfSavingContext& tableContext);
    void saveHeaderFooter(Sheet *sheet, KoXmlWriter &xmlWriter);
    void saveBackgroundImage(Sheet *sheet, KoXmlWriter& xmlWriter);
    void addText(const QString & text, KoXmlWriter & writer);
    void convertPart(Sheet *sheet, const QString & part, KoXmlWriter & xmlWriter);
    bool compareRows(Sheet *sheet, int row1, int row2, int maxCols, OdfSavingContext& tableContext);
    QString savePageLayout(PrintSettings *settings, KoGenStyles &mainStyles, bool formulas, bool zeros);
}

// *************** Loading *****************

bool Odf::loadSheet(Sheet *sheet, const KoXmlElement& sheetElement, OdfLoadingContext& tableContext, const Styles& autoStyles, const QHash<QString, Conditions>& conditionalStyles)
{
    QPointer<KoUpdater> updater;
    if (sheet->doc() && sheet->doc()->progressUpdater()) {
        updater = sheet->doc()->progressUpdater()->startSubtask(1,
                                                     "Calligra::Sheets::Odf::loadSheet");
        updater->setProgress(0);
    }

    KoOdfLoadingContext& odfContext = tableContext.odfContext;
    if (sheetElement.hasAttributeNS(KoXmlNS::table, "style-name")) {
        QString stylename = sheetElement.attributeNS(KoXmlNS::table, "style-name", QString());
        //debugSheetsODF<<" style of table :"<<stylename;
        const KoXmlElement *style = odfContext.stylesReader().findStyle(stylename, "table");
        Q_ASSERT(style);
        //debugSheetsODF<<" style :"<<style;
        if (style) {
            KoXmlElement properties(KoXml::namedItemNS(*style, KoXmlNS::style, "table-properties"));
            if (!properties.isNull()) {
                if (properties.hasAttributeNS(KoXmlNS::table, "display")) {
                    bool visible = (properties.attributeNS(KoXmlNS::table, "display", QString()) == "true" ? true : false);
                    sheet->setHidden(!visible);
                }
            }
            if (style->hasAttributeNS(KoXmlNS::style, "master-page-name")) {
                QString masterPageStyleName = style->attributeNS(KoXmlNS::style, "master-page-name", QString());
                //debugSheets<<"style->attribute( style:master-page-name ) :"<<masterPageStyleName;
                KoXmlElement *masterStyle = odfContext.stylesReader().masterPages()[masterPageStyleName];
                //debugSheets<<"stylesReader.styles()[masterPageStyleName] :"<<masterStyle;
                if (masterStyle) {
                    loadStyleFormat(sheet, masterStyle);
                    if (masterStyle->hasAttributeNS(KoXmlNS::style, "page-layout-name")) {
                        QString masterPageLayoutStyleName = masterStyle->attributeNS(KoXmlNS::style, "page-layout-name", QString());
                        //debugSheetsODF<<"masterPageLayoutStyleName :"<<masterPageLayoutStyleName;
                        const KoXmlElement *masterLayoutStyle = odfContext.stylesReader().findStyle(masterPageLayoutStyleName);
                        if (masterLayoutStyle) {
                            //debugSheetsODF<<"masterLayoutStyle :"<<masterLayoutStyle;
                            KoStyleStack styleStack;
                            styleStack.setTypeProperties("page-layout");
                            styleStack.push(*masterLayoutStyle);
                            loadMasterLayoutPage(sheet, styleStack);
                        }
                    }
                }
            }

            if (style->hasChildNodes() ) {
                KoXmlElement element;
                forEachElement(element, properties) {
                    if (element.nodeName() == "style:background-image") {
                        QString imagePath = element.attributeNS(KoXmlNS::xlink, "href");
                        KoStore* store = tableContext.odfContext.store();
                        if (store->hasFile(imagePath)) {
                            QByteArray data;
                            store->extractFile(imagePath, data);
                            QImage image = QImage::fromData(data);

                            if( image.isNull() ) {
                                continue;
                            }

                            sheet->setBackgroundImage(image);

                            Sheet::BackgroundImageProperties bgProperties;
                            if( element.hasAttribute("draw:opacity") ) {
                                QString opacity = element.attribute("draw:opacity", "");
                                if( opacity.endsWith(QLatin1Char('%')) ) {
                                    opacity.chop(1);
                                }
                                bool ok;
                                float opacityFloat = opacity.toFloat( &ok );
                                if( ok ) {
                                    bgProperties.opacity = opacityFloat;
                                }
                            }
                            //TODO
                            //if( element.hasAttribute("style:filterName") ) {
                            //}
                            if( element.hasAttribute("style:position") ) {
                                const QString positionAttribute = element.attribute("style:position","");
                                const QStringList positionList = positionAttribute.split(' ', QString::SkipEmptyParts);
                                if( positionList.size() == 1) {
                                    const QString position = positionList.at(0);
                                    if( position == "left" ) {
                                        bgProperties.horizontalPosition = Sheet::BackgroundImageProperties::Left;
                                    }
                                    if( position == "center" ) {
                                        //NOTE the standard is too vague to know what center alone means, we assume that it means both centered
                                        bgProperties.horizontalPosition = Sheet::BackgroundImageProperties::HorizontalCenter;
                                        bgProperties.verticalPosition = Sheet::BackgroundImageProperties::VerticalCenter;
                                    }
                                    if( position == "right" ) {
                                        bgProperties.horizontalPosition = Sheet::BackgroundImageProperties::Right;
                                    }
                                    if( position == "top" ) {
                                        bgProperties.verticalPosition = Sheet::BackgroundImageProperties::Top;
                                    }
                                    if( position == "bottom" ) {
                                        bgProperties.verticalPosition = Sheet::BackgroundImageProperties::Bottom;
                                    }
                                }
                                else if (positionList.size() == 2) {
                                    const QString verticalPosition = positionList.at(0);
                                    const QString horizontalPosition = positionList.at(1);
                                    if( horizontalPosition == "left" ) {
                                        bgProperties.horizontalPosition = Sheet::BackgroundImageProperties::Left;
                                    }
                                    if( horizontalPosition == "center" ) {
                                        bgProperties.horizontalPosition = Sheet::BackgroundImageProperties::HorizontalCenter;
                                    }
                                    if( horizontalPosition == "right" ) {
                                        bgProperties.horizontalPosition = Sheet::BackgroundImageProperties::Right;
                                    }
                                    if( verticalPosition == "top" ) {
                                        bgProperties.verticalPosition = Sheet::BackgroundImageProperties::Top;
                                    }
                                    if( verticalPosition == "center" ) {
                                        bgProperties.verticalPosition = Sheet::BackgroundImageProperties::VerticalCenter;
                                    }
                                    if( verticalPosition == "bottom" ) {
                                        bgProperties.verticalPosition = Sheet::BackgroundImageProperties::Bottom;
                                    }
                                }
                            }
                            if( element.hasAttribute("style:repeat") ) {
                                const QString repeat = element.attribute("style:repeat");
                                if( repeat == "no-repeat" ) {
                                    bgProperties.repeat = Sheet::BackgroundImageProperties::NoRepeat;
                                }
                                if( repeat == "repeat" ) {
                                    bgProperties.repeat = Sheet::BackgroundImageProperties::Repeat;
                                }
                                if( repeat == "stretch" ) {
                                    bgProperties.repeat = Sheet::BackgroundImageProperties::Stretch;
                                }
                            }
                            sheet->setBackgroundImageProperties(bgProperties);
                        }
                    }
                }

            }
        }
    }

    // Cell style regions
    QHash<QString, QRegion> cellStyleRegions;
    // Cell style regions (row defaults)
    QHash<QString, QRegion> rowStyleRegions;
    // Cell style regions (column defaults)
    QHash<QString, QRegion> columnStyleRegions;
    IntervalMap<QString> columnStyles;

    // List of shapes that need to have their size recalculated after loading is complete
    QList<ShapeLoadingData> shapeData;

    int rowIndex = 1;
    int indexCol = 1;
    int maxColumn = 1;
    KoXmlNode rowNode = sheetElement.firstChild();
    // Some spreadsheet programs may support more rows than
    // Calligra Sheets so limit the number of repeated rows.
    // FIXME POSSIBLE DATA LOSS!

    // First load all style information for rows, columns and cells
    while (!rowNode.isNull() && rowIndex <= KS_rowMax) {
        //debugSheetsODF << " rowIndex :" << rowIndex << " indexCol :" << indexCol;
        KoXmlElement rowElement = rowNode.toElement();
        if (!rowElement.isNull()) {
            // slightly faster
            KoXml::load(rowElement);

            //debugSheetsODF << " Odf::loadSheet rowElement.tagName() :" << rowElement.localName();
            if (rowElement.namespaceURI() == KoXmlNS::table) {
                if (rowElement.localName() == "table-header-columns") {
                    // NOTE Handle header cols as ordinary ones
                    //      as long as they're not supported.
                    loadColumnNodes(sheet, rowElement, indexCol, maxColumn, odfContext, columnStyleRegions, columnStyles);
                } else if (rowElement.localName() == "table-column-group") {
                    loadColumnNodes(sheet, rowElement, indexCol, maxColumn, odfContext, columnStyleRegions, columnStyles);
                } else if (rowElement.localName() == "table-column" && indexCol <= KS_colMax) {
                    //debugSheetsODF << " table-column found : index column before" << indexCol;
                    loadColumnFormat(sheet, rowElement, odfContext.stylesReader(), indexCol, columnStyleRegions, columnStyles);
                    //debugSheetsODF << " table-column found : index column after" << indexCol;
                    maxColumn = qMax(maxColumn, indexCol - 1);
                } else if (rowElement.localName() == "table-header-rows") {
                    // NOTE Handle header rows as ordinary ones
                    //      as long as they're not supported.
                    loadRowNodes(sheet, rowElement, rowIndex, maxColumn, tableContext, rowStyleRegions, cellStyleRegions, columnStyles, autoStyles, shapeData);
                } else if (rowElement.localName() == "table-row-group") {
                    loadRowNodes(sheet, rowElement, rowIndex, maxColumn, tableContext, rowStyleRegions, cellStyleRegions, columnStyles, autoStyles, shapeData);
                } else if (rowElement.localName() == "table-row") {
                    //debugSheetsODF << " table-row found :index row before" << rowIndex;
                    int columnMaximal = loadRowFormat(sheet, rowElement, rowIndex, tableContext,
                                  rowStyleRegions, cellStyleRegions, columnStyles, autoStyles, shapeData);
                    // allow the row to define more columns then defined via table-column
                    maxColumn = qMax(maxColumn, columnMaximal);
                    //debugSheetsODF << " table-row found :index row after" << rowIndex;
                } else if (rowElement.localName() == "shapes") {
                    // OpenDocument v1.1, 8.3.4 Shapes:
                    // The <table:shapes> element contains all graphic shapes
                    // with an anchor on the table this element is a child of.
                    KoShapeLoadingContext* shapeLoadingContext = tableContext.shapeContext;
                    KoXmlElement element;
                    forEachElement(element, rowElement) {
                        if (element.namespaceURI() != KoXmlNS::draw)
                            continue;
                        loadSheetObject(sheet, element, *shapeLoadingContext);
                    }
                }
            }

            // don't need it anymore
            KoXml::unload(rowElement);
        }

        rowNode = rowNode.nextSibling();

        int count = sheet->map()->increaseLoadedRowsCounter();
        if (updater && count >= 0) updater->setProgress(count);
    }

    // now recalculate the size for embedded shapes that had sizes specified relative to a bottom-right corner cell
    foreach (const ShapeLoadingData& sd, shapeData) {
        // subtract offset because the accumulated width and height we calculate below starts
        // at the top-left corner of this cell, but the shape can have an offset to that corner
        QSizeF size = QSizeF( sd.endPoint.x() - sd.offset.x(), sd.endPoint.y() - sd.offset.y());
        for (int col = sd.startCell.x(); col < sd.endCell.firstRange().left(); ++col)
            size += QSizeF(sheet->columnFormat(col)->width(), 0.0);
        if (sd.endCell.firstRange().top() > sd.startCell.y())
            size += QSizeF(0.0, sheet->rowFormats()->totalRowHeight(sd.startCell.y(), sd.endCell.firstRange().top() - 1));
        sd.shape->setSize(size);
    }

    QList<QPair<QRegion, Style> > styleRegions;
    QList<QPair<QRegion, Conditions> > conditionRegions;
    // insert the styles into the storage (column defaults)
    debugSheetsODF << "Inserting column default cell styles ...";
    loadSheetInsertStyles(sheet, autoStyles, columnStyleRegions, conditionalStyles,
                        QRect(1, 1, maxColumn, rowIndex - 1), styleRegions, conditionRegions);
    // insert the styles into the storage (row defaults)
    debugSheetsODF << "Inserting row default cell styles ...";
    loadSheetInsertStyles(sheet, autoStyles, rowStyleRegions, conditionalStyles,
                        QRect(1, 1, maxColumn, rowIndex - 1), styleRegions, conditionRegions);
    // insert the styles into the storage
    debugSheetsODF << "Inserting cell styles ...";
    loadSheetInsertStyles(sheet, autoStyles, cellStyleRegions, conditionalStyles,
                        QRect(1, 1, maxColumn, rowIndex - 1), styleRegions, conditionRegions);

    sheet->cellStorage()->loadStyles(styleRegions);
    sheet->cellStorage()->loadConditions(conditionRegions);

    if (sheetElement.hasAttributeNS(KoXmlNS::table, "print-ranges")) {
        // e.g.: Sheet4.A1:Sheet4.E28
        QString range = sheetElement.attributeNS(KoXmlNS::table, "print-ranges", QString());
        Region region(loadRegion(range));
        if (!region.firstSheet() || sheet->sheetName() == region.firstSheet()->sheetName())
            sheet->printSettings()->setPrintRegion(region);
    }

    if (sheetElement.attributeNS(KoXmlNS::table, "protected", QString()) == "true") {
        loadProtection(sheet, sheetElement);
    }
    return true;
}

void Odf::loadSheetObject(Sheet *sheet, const KoXmlElement& element, KoShapeLoadingContext& shapeContext)
{
    KoShape* shape = KoShapeRegistry::instance()->createShapeFromOdf(element, shapeContext);
    if (!shape)
        return;
    sheet->addShape(shape);
    dynamic_cast<ShapeApplicationData*>(shape->applicationData())->setAnchoredToCell(false);
}

void Odf::loadRowNodes(Sheet *sheet, const KoXmlElement& parent,
                            int& rowIndex,
                            int& maxColumn,
                            OdfLoadingContext& tableContext,
                            QHash<QString, QRegion>& rowStyleRegions,
                            QHash<QString, QRegion>& cellStyleRegions,
                            const IntervalMap<QString>& columnStyles,
                            const Styles& autoStyles,
                            QList<ShapeLoadingData>& shapeData
                            )
{
    KoXmlNode node = parent.firstChild();
    while (!node.isNull()) {
        KoXmlElement elem = node.toElement();
        if (!elem.isNull() && elem.namespaceURI() == KoXmlNS::table) {
            if (elem.localName() == "table-row") {
                int columnMaximal = loadRowFormat(sheet, elem, rowIndex, tableContext,
                                                        rowStyleRegions, cellStyleRegions,
                                                        columnStyles, autoStyles, shapeData);
                // allow the row to define more columns then defined via table-column
                maxColumn = qMax(maxColumn, columnMaximal);
            } else if (elem.localName() == "table-row-group") {
                loadRowNodes(sheet, elem, rowIndex, maxColumn, tableContext, rowStyleRegions, cellStyleRegions, columnStyles, autoStyles, shapeData);
            }
        }
        node = node.nextSibling();
    }
}

void Odf::loadColumnNodes(Sheet *sheet, const KoXmlElement& parent,
                            int& indexCol,
                            int& maxColumn,
                            KoOdfLoadingContext& odfContext,
                            QHash<QString, QRegion>& columnStyleRegions,
                            IntervalMap<QString>& columnStyles
                            )
{
    KoXmlNode node = parent.firstChild();
    while (!node.isNull()) {
        KoXmlElement elem = node.toElement();
        if (!elem.isNull() && elem.namespaceURI() == KoXmlNS::table) {
            if (elem.localName() == "table-column") {
                loadColumnFormat(sheet, elem, odfContext.stylesReader(), indexCol, columnStyleRegions, columnStyles);
                maxColumn = qMax(maxColumn, indexCol - 1);
            } else if (elem.localName() == "table-column-group") {
                loadColumnNodes(sheet, elem, indexCol, maxColumn, odfContext, columnStyleRegions, columnStyles);
            }
        }
        node = node.nextSibling();
    }
}


void Odf::loadSheetInsertStyles(Sheet *sheet, const Styles& autoStyles,
                             const QHash<QString, QRegion>& styleRegions,
                             const QHash<QString, Conditions>& conditionalStyles,
                             const QRect& usedArea,
                             QList<QPair<QRegion, Style> >& outStyleRegions,
                             QList<QPair<QRegion, Conditions> >& outConditionalStyles)
{
    const QList<QString> styleNames = styleRegions.keys();
    for (int i = 0; i < styleNames.count(); ++i) {
        if (!autoStyles.contains(styleNames[i]) && !sheet->map()->styleManager()->style(styleNames[i])) {
            warnSheetsODF << "\t" << styleNames[i] << " not used";
            continue;
        }
        const bool hasConditions = conditionalStyles.contains(styleNames[i]);
        const QRegion styleRegion = styleRegions[styleNames[i]] & QRegion(usedArea);
        if (hasConditions)
            outConditionalStyles.append(qMakePair(styleRegion, conditionalStyles[styleNames[i]]));
        if (autoStyles.contains(styleNames[i])) {
            //debugSheetsODF << "\tautomatic:" << styleNames[i] << " at" << styleRegion.rectCount() << "rects";
            Style style;
            style.setDefault(); // "overwrite" existing style
            style.merge(autoStyles[styleNames[i]]);
            outStyleRegions.append(qMakePair(styleRegion, style));
        } else {
            const CustomStyle* namedStyle = sheet->map()->styleManager()->style(styleNames[i]);
            //debugSheetsODF << "\tcustom:" << namedStyle->name() << " at" << styleRegion.rectCount() << "rects";
            Style style;
            style.setDefault(); // "overwrite" existing style
            style.merge(*namedStyle);
            outStyleRegions.append(qMakePair(styleRegion, style));
        }
    }
}

void Odf::replaceMacro(QString & text, const QString & old, const QString & newS)
{
    int n = text.indexOf(old);
    if (n != -1)
        text = text.replace(n, old.length(), newS);
}

QString Odf::getPart(const KoXmlNode & part)
{
    QString result;
    KoXmlElement e = KoXml::namedItemNS(part, KoXmlNS::text, "p");
    while (!e.isNull()) {
        QString text = e.text();

        KoXmlElement macro = KoXml::namedItemNS(e, KoXmlNS::text, "time");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<time>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "date");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<date>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "page-number");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<page>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "page-count");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<pages>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "sheet-name");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<sheet>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "title");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<name>");

        macro = KoXml::namedItemNS(e, KoXmlNS::text, "file-name");
        if (!macro.isNull())
            replaceMacro(text, macro.text(), "<file>");

        //add support for multi line into kspread
        if (!result.isEmpty())
            result += '\n';
        result += text;
        e = e.nextSibling().toElement();
    }

    return result;
}


bool Odf::loadStyleFormat(Sheet *sheet, KoXmlElement *style)
{
    QString hleft, hmiddle, hright;
    QString fleft, fmiddle, fright;
    KoXmlNode header = KoXml::namedItemNS(*style, KoXmlNS::style, "header");

    if (!header.isNull()) {
        debugSheetsODF << "Header exists";
        KoXmlNode part = KoXml::namedItemNS(header, KoXmlNS::style, "region-left");
        if (!part.isNull()) {
            hleft = getPart(part);
            debugSheetsODF << "Header left:" << hleft;
        } else
            debugSheetsODF << "Style:region:left doesn't exist!";
        part = KoXml::namedItemNS(header, KoXmlNS::style, "region-center");
        if (!part.isNull()) {
            hmiddle = getPart(part);
            debugSheetsODF << "Header middle:" << hmiddle;
        }
        part = KoXml::namedItemNS(header, KoXmlNS::style, "region-right");
        if (!part.isNull()) {
            hright = getPart(part);
            debugSheetsODF << "Header right:" << hright;
        }
        //If Header doesn't have region tag add it to Left
        hleft.append(getPart(header));
    }
    //TODO implement it under kspread
    KoXmlNode headerleft = KoXml::namedItemNS(*style, KoXmlNS::style, "header-left");
    if (!headerleft.isNull()) {
        KoXmlElement e = headerleft.toElement();
        if (e.hasAttributeNS(KoXmlNS::style, "display"))
            debugSheetsODF << "header.hasAttribute( style:display ) :" << e.hasAttributeNS(KoXmlNS::style, "display");
        else
            debugSheetsODF << "header left doesn't has attribute  style:display";
    }
    //TODO implement it under kspread
    KoXmlNode footerleft = KoXml::namedItemNS(*style, KoXmlNS::style, "footer-left");
    if (!footerleft.isNull()) {
        KoXmlElement e = footerleft.toElement();
        if (e.hasAttributeNS(KoXmlNS::style, "display"))
            debugSheetsODF << "footer.hasAttribute( style:display ) :" << e.hasAttributeNS(KoXmlNS::style, "display");
        else
            debugSheetsODF << "footer left doesn't has attribute  style:display";
    }

    KoXmlNode footer = KoXml::namedItemNS(*style, KoXmlNS::style, "footer");

    if (!footer.isNull()) {
        KoXmlNode part = KoXml::namedItemNS(footer, KoXmlNS::style, "region-left");
        if (!part.isNull()) {
            fleft = getPart(part);
            debugSheetsODF << "Footer left:" << fleft;
        }
        part = KoXml::namedItemNS(footer, KoXmlNS::style, "region-center");
        if (!part.isNull()) {
            fmiddle = getPart(part);
            debugSheetsODF << "Footer middle:" << fmiddle;
        }
        part = KoXml::namedItemNS(footer, KoXmlNS::style, "region-right");
        if (!part.isNull()) {
            fright = getPart(part);
            debugSheetsODF << "Footer right:" << fright;
        }
        //If Footer doesn't have region tag add it to Left
        fleft.append(getPart(footer));
    }

    sheet->print()->headerFooter()->setHeadFootLine(hleft, hmiddle, hright,
            fleft, fmiddle, fright);
    return true;
}

void Odf::loadMasterLayoutPage(Sheet *sheet, KoStyleStack &styleStack)
{
    KoPageLayout pageLayout;

    if (styleStack.hasProperty(KoXmlNS::fo, "page-width")) {
        pageLayout.width = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "page-width"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "page-height")) {
        pageLayout.height = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "page-height"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-top")) {
        pageLayout.topMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-top"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-bottom")) {
        pageLayout.bottomMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-bottom"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-left")) {
        pageLayout.leftMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-left"));
    }
    if (styleStack.hasProperty(KoXmlNS::fo, "margin-right")) {
        pageLayout.rightMargin = KoUnit::parseValue(styleStack.property(KoXmlNS::fo, "margin-right"));
    }
    /* set sheet's direction to RTL if sheet name is an RTL string */
    Qt::LayoutDirection ldir = sheet->sheetName().isRightToLeft() ? Qt::RightToLeft : Qt::LeftToRight;
    if (styleStack.hasProperty(KoXmlNS::style, "writing-mode")) {
        debugSheetsODF << "styleStack.hasAttribute( style:writing-mode ) :" << styleStack.hasProperty(KoXmlNS::style, "writing-mode");
        const QString writingMode = styleStack.property(KoXmlNS::style, "writing-mode");
        if (writingMode == "lr-tb") {
            ldir = Qt::LeftToRight;
        } else if (writingMode == "rl-tb") {
            ldir = Qt::RightToLeft;
        }
        //TODO
        //<value>lr-tb</value>
        //<value>rl-tb</value>
        //<value>tb-rl</value>
        //<value>tb-lr</value>
        //<value>lr</value>
        //<value>rl</value>
        //<value>tb</value>
        //<value>page</value>
    }
    sheet->setLayoutDirection(ldir);

    if (styleStack.hasProperty(KoXmlNS::style, "print-orientation")) {
        pageLayout.orientation = (styleStack.property(KoXmlNS::style, "print-orientation") == "landscape")
                                 ? KoPageFormat::Landscape : KoPageFormat::Portrait;
    }
    if (styleStack.hasProperty(KoXmlNS::style, "num-format")) {
        //not implemented into kspread
        //These attributes specify the numbering style to use.
        //If a numbering style is not specified, the numbering style is inherited from
        //the page style. See section 6.7.8 for information on these attributes
        debugSheetsODF << " num-format :" << styleStack.property(KoXmlNS::style, "num-format");

    }
    if (styleStack.hasProperty(KoXmlNS::fo, "background-color")) {
        //TODO
        debugSheetsODF << " fo:background-color :" << styleStack.property(KoXmlNS::fo, "background-color");
    }
    if (styleStack.hasProperty(KoXmlNS::style, "print")) {
        //todo parsing
        QString str = styleStack.property(KoXmlNS::style, "print");
        debugSheetsODF << " style:print :" << str;

        if (str.contains("headers")) {
            //TODO implement it into kspread
        }
        if (str.contains("grid")) {
            sheet->print()->settings()->setPrintGrid(true);
        }
        if (str.contains("annotations")) {
            //TODO it's not implemented
        }
        if (str.contains("objects")) {
            //TODO it's not implemented
        }
        if (str.contains("charts")) {
            //TODO it's not implemented
        }
        if (str.contains("drawings")) {
            //TODO it's not implemented
        }
        if (str.contains("formulas")) {
            sheet->setShowFormula(true);
        }
        if (str.contains("zero-values")) {
            //TODO it's not implemented
        }
    }
    if (styleStack.hasProperty(KoXmlNS::style, "table-centering")) {
        QString str = styleStack.property(KoXmlNS::style, "table-centering");
        //TODO not implemented into kspread
        debugSheetsODF << " styleStack.attribute( style:table-centering ) :" << str;
#if 0
        if (str == "horizontal") {
        } else if (str == "vertical") {
        } else if (str == "both") {
        } else if (str == "none") {
        } else
            debugSheetsODF << " table-centering unknown :" << str;
#endif
    }
    sheet->print()->settings()->setPageLayout(pageLayout);
}

bool Odf::loadColumnFormat(Sheet *sheet, const KoXmlElement& column,
                             const KoOdfStylesReader& stylesReader, int & indexCol,
                             QHash<QString, QRegion>& columnStyleRegions, IntervalMap<QString>& columnStyles)
{
//   debugSheetsODF<<"bool Odf::loadColumnFormat(const KoXmlElement& column, const KoOdfStylesReader& stylesReader, unsigned int & indexCol ) index Col :"<<indexCol;

    bool isNonDefaultColumn = false;

    int number = 1;
    if (column.hasAttributeNS(KoXmlNS::table, "number-columns-repeated")) {
        bool ok = true;
        int n = column.attributeNS(KoXmlNS::table, "number-columns-repeated", QString()).toInt(&ok);
        if (ok)
            // Some spreadsheet programs may support more rows than Calligra Sheets so
            // limit the number of repeated rows.
            // FIXME POSSIBLE DATA LOSS!
            number = qMin(n, KS_colMax - indexCol + 1);
        //debugSheetsODF << "Repeated:" << number;
    }

    if (column.hasAttributeNS(KoXmlNS::table, "default-cell-style-name")) {
        const QString styleName = column.attributeNS(KoXmlNS::table, "default-cell-style-name", QString());
        if (!styleName.isEmpty()) {
            columnStyleRegions[styleName] += QRect(indexCol, 1, number, KS_rowMax);
            columnStyles.insert(indexCol, indexCol+number-1, styleName);
        }
    }

    enum { Visible, Collapsed, Filtered } visibility = Visible;
    if (column.hasAttributeNS(KoXmlNS::table, "visibility")) {
        const QString string = column.attributeNS(KoXmlNS::table, "visibility", "visible");
        if (string == "collapse")
            visibility = Collapsed;
        else if (string == "filter")
            visibility = Filtered;
        isNonDefaultColumn = true;
    }

    KoStyleStack styleStack;
    if (column.hasAttributeNS(KoXmlNS::table, "style-name")) {
        QString str = column.attributeNS(KoXmlNS::table, "style-name", QString());
        const KoXmlElement *style = stylesReader.findStyle(str, "table-column");
        if (style) {
            styleStack.push(*style);
            isNonDefaultColumn = true;
        }
    }
    styleStack.setTypeProperties("table-column"); //style for column

    double width = -1.0;
    if (styleStack.hasProperty(KoXmlNS::style, "column-width")) {
        width = KoUnit::parseValue(styleStack.property(KoXmlNS::style, "column-width") , -1.0);
        //debugSheetsODF << " style:column-width : width :" << width;
        isNonDefaultColumn = true;
    }

    bool insertPageBreak = false;
    if (styleStack.hasProperty(KoXmlNS::fo, "break-before")) {
        QString str = styleStack.property(KoXmlNS::fo, "break-before");
        if (str == "page") {
            insertPageBreak = true;
        } else {
            // debugSheetsODF << " str :" << str;
        }
        isNonDefaultColumn = true;
    } else if (styleStack.hasProperty(KoXmlNS::fo, "break-after")) {
        // TODO
    }

    // If it's a default column, we can return here.
    // This saves the iteration, which can be caused by column cell default styles,
    // but which are not inserted here.
    if (!isNonDefaultColumn) {
        indexCol += number;
        return true;
    }

    for (int i = 0; i < number; ++i) {
        //debugSheetsODF << " insert new column: pos :" << indexCol << " width :" << width << " hidden ?" << visibility;

        if (isNonDefaultColumn) {
            ColumnFormat* cf = sheet->nonDefaultColumnFormat(indexCol);

            if (width != -1.0)   //safe
                cf->setWidth(width);
            if (insertPageBreak) {
                cf->setPageBreak(true);
            }
            if (visibility == Collapsed)
                cf->setHidden(true);
            else if (visibility == Filtered)
                cf->setFiltered(true);

            cf->setPageBreak(insertPageBreak);
        }
        ++indexCol;
    }
//     debugSheetsODF<<" after index column !!!!!!!!!!!!!!!!!! :"<<indexCol;
    return true;
}

int Odf::loadRowFormat(Sheet *sheet, const KoXmlElement& row, int &rowIndex,
                          OdfLoadingContext& tableContext,
                          QHash<QString, QRegion>& rowStyleRegions,
                          QHash<QString, QRegion>& cellStyleRegions,
                          const IntervalMap<QString>& columnStyles,
                          const Styles& autoStyles,
                          QList<ShapeLoadingData>& shapeData)
{
    static const QString sStyleName             = QString::fromLatin1("style-name");
    static const QString sNumberRowsRepeated    = QString::fromLatin1("number-rows-repeated");
    static const QString sDefaultCellStyleName  = QString::fromLatin1("default-cell-style-name");
    static const QString sVisibility            = QString::fromLatin1("visibility");
    static const QString sVisible               = QString::fromLatin1("visible");
    static const QString sCollapse              = QString::fromLatin1("collapse");
    static const QString sFilter                = QString::fromLatin1("filter");
    static const QString sPage                  = QString::fromLatin1("page");
    static const QString sTableCell             = QString::fromLatin1("table-cell");
    static const QString sCoveredTableCell      = QString::fromLatin1("covered-table-cell");
    static const QString sNumberColumnsRepeated = QString::fromLatin1("number-columns-repeated");

//    debugSheetsODF<<"Odf::loadRowFormat( const KoXmlElement& row, int &rowIndex,const KoOdfStylesReader& stylesReader, bool isLast )***********";
    KoOdfLoadingContext& odfContext = tableContext.odfContext;
    bool isNonDefaultRow = false;

    KoStyleStack styleStack;
    if (row.hasAttributeNS(KoXmlNS::table, sStyleName)) {
        QString str = row.attributeNS(KoXmlNS::table, sStyleName, QString());
        const KoXmlElement *style = odfContext.stylesReader().findStyle(str, "table-row");
        if (style) {
            styleStack.push(*style);
            isNonDefaultRow = true;
        }
    }
    styleStack.setTypeProperties("table-row");

    int number = 1;
    if (row.hasAttributeNS(KoXmlNS::table, sNumberRowsRepeated)) {
        bool ok = true;
        int n = row.attributeNS(KoXmlNS::table, sNumberRowsRepeated, QString()).toInt(&ok);
        if (ok)
            // Some spreadsheet programs may support more rows than Calligra Sheets so
            // limit the number of repeated rows.
            // FIXME POSSIBLE DATA LOSS!
            number = qMin(n, KS_rowMax - rowIndex + 1);
    }

    QString rowCellStyleName;
    if (row.hasAttributeNS(KoXmlNS::table, sDefaultCellStyleName)) {
        rowCellStyleName = row.attributeNS(KoXmlNS::table, sDefaultCellStyleName, QString());
        if (!rowCellStyleName.isEmpty()) {
            rowStyleRegions[rowCellStyleName] += QRect(1, rowIndex, KS_colMax, number);
        }
    }

    double height = -1.0;
    if (styleStack.hasProperty(KoXmlNS::style, "row-height")) {
        height = KoUnit::parseValue(styleStack.property(KoXmlNS::style, "row-height") , -1.0);
        //    debugSheetsODF<<" properties style:row-height : height :"<<height;
        isNonDefaultRow = true;
    }

    enum { Visible, Collapsed, Filtered } visibility = Visible;
    if (row.hasAttributeNS(KoXmlNS::table, sVisibility)) {
        const QString string = row.attributeNS(KoXmlNS::table, sVisibility, sVisible);
        if (string == sCollapse)
            visibility = Collapsed;
        else if (string == sFilter)
            visibility = Filtered;
        isNonDefaultRow = true;
    }

    bool insertPageBreak = false;
    if (styleStack.hasProperty(KoXmlNS::fo, "break-before")) {
        QString str = styleStack.property(KoXmlNS::fo, "break-before");
        if (str == sPage) {
            insertPageBreak = true;
        }
        //  else
        //      debugSheetsODF<<" str :"<<str;
        isNonDefaultRow = true;
    } else if (styleStack.hasProperty(KoXmlNS::fo, "break-after")) {
        // TODO
    }

//     debugSheetsODF<<" create non defaultrow format :"<<rowIndex<<" repeate :"<<number<<" height :"<<height;
    if (isNonDefaultRow) {
        if (height != -1.0)
            sheet->rowFormats()->setRowHeight(rowIndex, rowIndex + number - 1, height);
        sheet->rowFormats()->setPageBreak(rowIndex, rowIndex + number - 1, insertPageBreak);
        if (visibility == Collapsed)
            sheet->rowFormats()->setHidden(rowIndex, rowIndex + number - 1, true);
        else if (visibility == Filtered)
            sheet->rowFormats()->setFiltered(rowIndex, rowIndex + number - 1, true);
    }

    int columnIndex = 1;
    int columnMaximal = 0;
    const int endRow = qMin(rowIndex + number - 1, KS_rowMax);

    KoXmlElement cellElement;
    forEachElement(cellElement, row) {
        if (cellElement.namespaceURI() != KoXmlNS::table)
            continue;
        if (cellElement.localName() != sTableCell && cellElement.localName() != sCoveredTableCell)
            continue;


        bool ok = false;
        const int n = cellElement.attributeNS(KoXmlNS::table, sNumberColumnsRepeated, QString()).toInt(&ok);
        // Some spreadsheet programs may support more columns than
        // Calligra Sheets so limit the number of repeated columns.
        const int numberColumns = ok ? qMin(n, KS_colMax - columnIndex + 1) : 1;
        columnMaximal = qMax(numberColumns, columnMaximal);

        // Styles are inserted at the end of the loading process, so check the XML directly here.
        const QString styleName = cellElement.attributeNS(KoXmlNS::table , sStyleName, QString());
        if (!styleName.isEmpty())
            cellStyleRegions[styleName] += QRect(columnIndex, rowIndex, numberColumns, number);

        // figure out exact cell style for loading of cell content
        QString cellStyleName = styleName;
        if (cellStyleName.isEmpty())
            cellStyleName = rowCellStyleName;
        if (cellStyleName.isEmpty())
            cellStyleName = columnStyles.get(columnIndex);

        Cell cell(sheet, columnIndex, rowIndex);
        loadCell(&cell, cellElement, tableContext, autoStyles, cellStyleName, shapeData);

        if (!cell.comment().isEmpty())
            sheet->cellStorage()->setComment(Region(columnIndex, rowIndex, numberColumns, number, sheet), cell.comment());
        if (!cell.conditions().isEmpty())
            sheet->cellStorage()->setConditions(Region(columnIndex, rowIndex, numberColumns, number, sheet), cell.conditions());
        if (!cell.validity().isEmpty())
            sheet->cellStorage()->setValidity(Region(columnIndex, rowIndex, numberColumns, number, sheet), cell.validity());

        if (!cell.hasDefaultContent()) {
            // Row-wise filling of PointStorages is faster than column-wise filling.
            QSharedPointer<QTextDocument> richText = cell.richText();
            for (int r = rowIndex; r <= endRow; ++r) {
                for (int c = 0; c < numberColumns; ++c) {
                    Cell target(sheet, columnIndex + c, r);
                    target.setFormula(cell.formula());
                    target.setUserInput(cell.userInput());
                    target.setRichText(richText);
                    target.setValue(cell.value());
                    if (cell.doesMergeCells()) {
                        target.mergeCells(columnIndex + c, r, cell.mergedXCells(), cell.mergedYCells());
                    }
                }
            }
        }
        columnIndex += numberColumns;
    }

    sheet->cellStorage()->setRowsRepeated(rowIndex, number);

    rowIndex += number;
    return columnMaximal;
}


// *************** Saving *****************

bool Odf::saveSheet(Sheet *sheet, OdfSavingContext& tableContext)
{
    KoXmlWriter & xmlWriter = tableContext.shapeContext.xmlWriter();
    KoGenStyles & mainStyles = tableContext.shapeContext.mainStyles();
    xmlWriter.startElement("table:table");
    xmlWriter.addAttribute("table:name", sheet->sheetName());
    xmlWriter.addAttribute("table:style-name", saveSheetStyleName(sheet, mainStyles));
    QByteArray pwd;
    sheet->password(pwd);
    if (!pwd.isNull()) {
        xmlWriter.addAttribute("table:protected", "true");
        QByteArray str = KCodecs::base64Encode(pwd);
        // FIXME Stefan: see OpenDocument spec, ch. 17.3 Encryption
        xmlWriter.addAttribute("table:protection-key", QString(str));
    }
    QRect _printRange = sheet->printSettings()->printRegion().lastRange();
    if (!_printRange.isNull() &&_printRange != (QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax)))) {
        const Region region(_printRange, sheet);
        if (region.isValid()) {
            debugSheetsODF << region;
            xmlWriter.addAttribute("table:print-ranges", saveRegion(region.name()));
        }
    }

    // flake
    // Create a dict of cell anchored shapes with the cell as key.
    int sheetAnchoredCount = 0;
    foreach(KoShape* shape, sheet->shapes()) {
        if (dynamic_cast<ShapeApplicationData*>(shape->applicationData())->isAnchoredToCell()) {
            qreal dummy;
            const QPointF position = shape->position();
            const int col = sheet->leftColumn(position.x(), dummy);
            const int row = sheet->topRow(position.y(), dummy);
            tableContext.insertCellAnchoredShape(sheet, row, col, shape);
        } else {
            sheetAnchoredCount++;
        }
    }

    // flake
    // Save the remaining shapes, those that are anchored in the page.
    if (sheetAnchoredCount) {
        xmlWriter.startElement("table:shapes");
        foreach(KoShape* shape, sheet->shapes()) {
            if (dynamic_cast<ShapeApplicationData*>(shape->applicationData())->isAnchoredToCell())
                continue;
            shape->saveOdf(tableContext.shapeContext);
        }
        xmlWriter.endElement();
    }

    const QRect usedArea = sheet->usedArea();
    saveColRowCell(sheet, usedArea.width(), usedArea.height(), tableContext);

    xmlWriter.endElement();
    return true;
}

QString Odf::saveSheetStyleName(Sheet *sheet, KoGenStyles &mainStyles)
{
    KoGenStyle pageStyle(KoGenStyle::TableAutoStyle, "table"/*FIXME I don't know if name is sheet*/);

    KoGenStyle pageMaster(KoGenStyle::MasterPageStyle);
    const QString pageLayoutName = savePageLayout(sheet->printSettings(), mainStyles,
                                   sheet->getShowFormula(),
                                   !sheet->getHideZero());
    pageMaster.addAttribute("style:page-layout-name", pageLayoutName);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter elementWriter(&buffer);    // TODO pass indentation level
    saveHeaderFooter(sheet, elementWriter);

    QString elementContents = QString::fromUtf8(buffer.buffer(), buffer.buffer().size());
    pageMaster.addChildElement("headerfooter", elementContents);
    pageStyle.addAttribute("style:master-page-name", mainStyles.insert(pageMaster, "Standard"));

    pageStyle.addProperty("table:display", !sheet->isHidden());

    if( !sheet->backgroundImage().isNull() ) {
        QBuffer bgBuffer;
        bgBuffer.open(QIODevice::WriteOnly);
        KoXmlWriter bgWriter(&bgBuffer); //TODO pass indentation level
        saveBackgroundImage(sheet, bgWriter);

        const QString bgContent = QString::fromUtf8(bgBuffer.buffer(), bgBuffer.size());
        pageMaster.addChildElement("backgroundImage", bgContent);
    }

    return mainStyles.insert(pageStyle, "ta");
}

void Odf::saveColRowCell(Sheet *sheet, int maxCols, int maxRows, OdfSavingContext& tableContext)
{
    debugSheetsODF << "Odf::saveColRowCell:" << sheet->sheetName();

    KoXmlWriter & xmlWriter = tableContext.shapeContext.xmlWriter();
    KoGenStyles & mainStyles = tableContext.shapeContext.mainStyles();

    // calculate the column/row default cell styles
    int maxMaxRows = maxRows; // includes the max row a column default style occupies
    // also extends the maximum column/row to include column/row styles
    sheet->styleStorage()->saveCreateDefaultStyles(maxCols, maxMaxRows, tableContext.columnDefaultStyles, tableContext.rowDefaultStyles);
    if (tableContext.rowDefaultStyles.count() != 0)
        maxRows = qMax(maxRows, (--tableContext.rowDefaultStyles.constEnd()).key());
    // Take the actual used area into account so we also catch shapes that are
    // anchored after any content.
    QRect r = sheet->usedArea(false);
    maxRows = qMax(maxRows, r.bottom());
    maxCols = qMax(maxCols, r.right());
    // OpenDocument needs at least one cell per sheet.
    maxCols = qMin(KS_colMax, qMax(1, maxCols));
    maxRows = qMin(KS_rowMax, qMax(1, maxRows));
    maxMaxRows = maxMaxRows;
    debugSheetsODF << "\t Sheet dimension:" << maxCols << " x" << maxRows;

    // saving the columns
    //
    int i = 1;
    while (i <= maxCols) {
        const ColumnFormat* column = sheet->columnFormat(i);
//         debugSheetsODF << "Odf::saveColRowCell: first col loop:"
//                       << "i:" << i
//                       << "column:" << (column ? column->column() : 0)
//                       << "default:" << (column ? column->isDefault() : false);

        //style default layout for column
        const Style style = tableContext.columnDefaultStyles.value(i);

        int j = i;
        int count = 1;

        while (j <= maxCols) {
            const ColumnFormat* nextColumn = sheet->nextColumn(j);
            const int nextColumnIndex = nextColumn ? nextColumn->column() : 0;
            const QMap<int, Style>::iterator nextColumnDefaultStyle = tableContext.columnDefaultStyles.upperBound(j);
            const int nextStyleColumnIndex = nextColumnDefaultStyle == tableContext.columnDefaultStyles.end()
                                             ? 0 : nextColumnDefaultStyle.key();
            // j becomes the index of the adjacent column
            ++j;

//           debugSheetsODF <<"Odf::saveColRowCell: second col loop:"
//                         << "j:" << j
//                         << "next column:" << (nextColumn ? nextColumn->column() : 0)
//                         << "next styled column:" << nextStyleColumnIndex;

            // no next or not the adjacent column?
            if ((!nextColumn && !nextStyleColumnIndex) ||
                    (nextColumnIndex != j && nextStyleColumnIndex != j)) {
                // if the origin column was a default column,
                if (column->isDefault() && style.isDefault()) {
                    // we count the default columns
                    if (!nextColumn && !nextStyleColumnIndex)
                        count = maxCols - i + 1;
                    else if (nextColumn && (!nextStyleColumnIndex || nextColumn->column() <= nextStyleColumnIndex))
                        count = nextColumn->column() - i;
                    else
                        count = nextStyleColumnIndex - i;
                }
                // otherwise we just stop here to process the adjacent
                // column in the next iteration of the outer loop
                break;
            }

            // stop, if the next column differs from the current one
            if ((nextColumn && (*column != *nextColumn)) || (!nextColumn && !column->isDefault()))
                break;
            if (style != tableContext.columnDefaultStyles.value(j))
                break;
            ++count;
        }

        xmlWriter.startElement("table:table-column");
        if (!column->isDefault()) {
            KoGenStyle currentColumnStyle(KoGenStyle::TableColumnAutoStyle, "table-column");
            currentColumnStyle.addPropertyPt("style:column-width", column->width());
            if (column->hasPageBreak()) {
                currentColumnStyle.addProperty("fo:break-before", "page");
            }
            xmlWriter.addAttribute("table:style-name", mainStyles.insert(currentColumnStyle, "co"));
        }
        if (!column->isDefault() || !style.isDefault()) {
            if (!style.isDefault()) {
                KoGenStyle currentDefaultCellStyle; // the type is determined in saveOdfStyle
                const QString name = saveStyle(&style, currentDefaultCellStyle, mainStyles,
                                                   sheet->map()->styleManager());
                xmlWriter.addAttribute("table:default-cell-style-name", name);
            }

            if (column->isHidden())
                xmlWriter.addAttribute("table:visibility", "collapse");
            else if (column->isFiltered())
                xmlWriter.addAttribute("table:visibility", "filter");
        }
        if (count > 1)
            xmlWriter.addAttribute("table:number-columns-repeated", count);
        xmlWriter.endElement();

        debugSheetsODF << "Odf::saveColRowCell: column" << i
        << "repeated" << count - 1 << "time(s)";

        i += count;
    }

    // saving the rows and the cells
    // we have to loop through all rows of the used area
    for (i = 1; i <= maxRows; ++i) {
        // default cell style for row
        const Style style = tableContext.rowDefaultStyles.value(i);

        xmlWriter.startElement("table:table-row");

        const bool rowIsDefault = sheet->rowFormats()->isDefaultRow(i);
        if (!rowIsDefault) {
            KoGenStyle currentRowStyle(KoGenStyle::TableRowAutoStyle, "table-row");
            currentRowStyle.addPropertyPt("style:row-height", sheet->rowFormats()->rowHeight(i));
            if (sheet->rowFormats()->hasPageBreak(i)) {
                currentRowStyle.addProperty("fo:break-before", "page");
            }
            xmlWriter.addAttribute("table:style-name", mainStyles.insert(currentRowStyle, "ro"));
        }

        // We cannot use cellStorage()->rowRepeat(i) here cause the RowRepeatStorage only knows
        // about the content but not about the shapes anchored to a cell. So, we need to check
        // for them here to be sure to catch them even when the content in the cell is repeated.
        int repeated = 1;
        // empty row?
        if (!sheet->cellStorage()->firstInRow(i) && !tableContext.rowHasCellAnchoredShapes(sheet, i)) { // row is empty
//             debugSheetsODF <<"Odf::saveColRowCell: first row loop:"
//                           << " i: " << i
//                           << " row: " << row->row();
            int j = i + 1;

            // search for
            //   next non-empty row
            // or
            //   next row with different Format
            while (j <= maxRows && !sheet->cellStorage()->firstInRow(j) && !tableContext.rowHasCellAnchoredShapes(sheet, j)) {
//               debugSheetsODF <<"Odf::saveColRowCell: second row loop:"
//                         << " j: " << j
//                         << " row: " << nextRow->row();

                // if the reference row has the default row format
                if (rowIsDefault && style.isDefault()) {
                    // if the next is not default, stop here
                    if (!sheet->rowFormats()->isDefaultRow(j) || !tableContext.rowDefaultStyles.value(j).isDefault())
                        break;
                    // otherwise, jump to the next
                    ++j;
                    continue;
                }

                // stop, if the next row differs from the current one
                if (!sheet->rowFormats()->rowsAreEqual(i, j))
                    break;
                if (style != tableContext.rowDefaultStyles.value(j))
                    break;
                // otherwise, process the next
                ++j;
            }
            repeated = j - i;

            if (repeated > 1)
                xmlWriter.addAttribute("table:number-rows-repeated", repeated);
            if (!style.isDefault()) {
                KoGenStyle currentDefaultCellStyle; // the type is determined in saveCellStyle
                const QString name = saveStyle(&style, currentDefaultCellStyle, mainStyles,
                                                   sheet->map()->styleManager());
                xmlWriter.addAttribute("table:default-cell-style-name", name);
            }
            if (sheet->rowFormats()->isHidden(i))   // never true for the default row
                xmlWriter.addAttribute("table:visibility", "collapse");
            else if (sheet->rowFormats()->isFiltered(i)) // never true for the default row
                xmlWriter.addAttribute("table:visibility", "filter");

            // NOTE Stefan: Even if paragraph 8.1 states, that rows may be empty, the
            //              RelaxNG schema does not allow that.
            xmlWriter.startElement("table:table-cell");
            // Fill the row with empty cells, if there's a row default cell style.
            if (!style.isDefault())
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(maxCols));
            // Fill the row with empty cells up to the last column with a default cell style.
            else if (!tableContext.columnDefaultStyles.isEmpty()) {
                const int col = (--tableContext.columnDefaultStyles.constEnd()).key();
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(col));
            }
            xmlWriter.endElement();

            debugSheetsODF << "Odf::saveColRowCell: empty row" << i
            << "repeated" << repeated << "time(s)";

            // copy the index for the next row to process
            i = j - 1; /*it's already incremented in the for loop*/
        } else { // row is not empty
            if (!style.isDefault()) {
                KoGenStyle currentDefaultCellStyle; // the type is determined in saveCellStyle
                const QString name = saveStyle(&style, currentDefaultCellStyle, mainStyles,
                                                   sheet->map()->styleManager());
                xmlWriter.addAttribute("table:default-cell-style-name", name);
            }
            if (sheet->rowFormats()->isHidden(i))   // never true for the default row
                xmlWriter.addAttribute("table:visibility", "collapse");
            else if (sheet->rowFormats()->isFiltered(i)) // never true for the default row
                xmlWriter.addAttribute("table:visibility", "filter");

            int j = i + 1;
            while (j <= maxRows && compareRows(sheet, i, j, maxCols, tableContext)) {
                j++;
                repeated++;
            }
            repeated = j - i;
            if (repeated > 1) {
                debugSheetsODF << "Odf::saveColRowCell: NON-empty row" << i
                << "repeated" << repeated << "times";

                xmlWriter.addAttribute("table:number-rows-repeated", repeated);
            }

            saveCells(sheet, i, maxCols, tableContext);

            // copy the index for the next row to process
            i = j - 1; /*it's already incremented in the for loop*/
        }
        xmlWriter.endElement();
    }

    // Fill in rows with empty cells, if there's a column default cell style.
    if (!tableContext.columnDefaultStyles.isEmpty()) {
        if (maxMaxRows > maxRows) {
            xmlWriter.startElement("table:table-row");
            if (maxMaxRows > maxRows + 1)
                xmlWriter.addAttribute("table:number-rows-repeated", maxMaxRows - maxRows);
            xmlWriter.startElement("table:table-cell");
            const int col = qMin(maxCols, (--tableContext.columnDefaultStyles.constEnd()).key());
            xmlWriter.addAttribute("table:number-columns-repeated", QString::number(col));
            xmlWriter.endElement();
            xmlWriter.endElement();
        }
    }
}

void Odf::saveCells(Sheet *sheet, int row, int maxCols, OdfSavingContext& tableContext)
{
    KoXmlWriter & xmlWriter = tableContext.shapeContext.xmlWriter();

    int i = 1;
    Cell cell(sheet, i, row);
    Cell nextCell = sheet->cellStorage()->nextInRow(i, row);
    // handle situations where the row contains shapes and nothing else
    if (cell.isDefault() && nextCell.isNull()) {
        int nextShape = tableContext.nextAnchoredShape(sheet, row, i);
        if (nextShape)
            nextCell = Cell(sheet, nextShape, row);
    }
    // while
    //   the current cell is not a default one
    // or
    //   we have a further cell in this row
    do {
//         debugSheetsODF <<"Odf::saveCells:"
//                       << " i: " << i
//                       << " column: " << cell.column() << endl;

        int repeated = 1;
        int column = i;
        saveCell(&cell, repeated, tableContext);
        i += repeated;
        // stop if we reached the end column
        if (i > maxCols || nextCell.isNull())
            break;

        cell = Cell(sheet, i, row);
        // if we have a shape anchored to an empty cell, ensure that the cell gets also processed
        int nextShape = tableContext.nextAnchoredShape(sheet, row, column);
        if (nextShape && ((nextShape < i) || cell.isDefault())) {
            cell = Cell(sheet, nextShape, row);
            i = nextShape;
        }

        nextCell = sheet->cellStorage()->nextInRow(i, row);
    } while (!cell.isDefault() || tableContext.cellHasAnchoredShapes(sheet, cell.row(), cell.column()) || !nextCell.isNull());

    // Fill the row with empty cells, if there's a row default cell style.
    if (tableContext.rowDefaultStyles.contains(row)) {
        if (maxCols >= i) {
            xmlWriter.startElement("table:table-cell");
            if (maxCols > i)
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(maxCols - i + 1));
            xmlWriter.endElement();
        }
    }
    // Fill the row with empty cells up to the last column with a default cell style.
    else if (!tableContext.columnDefaultStyles.isEmpty()) {
        const int col = (--tableContext.columnDefaultStyles.constEnd()).key();
        if (col >= i) {
            xmlWriter.startElement("table:table-cell");
            if (col > i)
                xmlWriter.addAttribute("table:number-columns-repeated", QString::number(col - i + 1));
            xmlWriter.endElement();
        }
    }
}

void Odf::saveBackgroundImage(Sheet *sheet, KoXmlWriter& xmlWriter)
{
    const Sheet::BackgroundImageProperties& properties = sheet->backgroundImageProperties();
    xmlWriter.startElement("style:backgroundImage");

    //xmlWriter.addAttribute("xlink:href", fileName);
    xmlWriter.addAttribute("xlink:type", "simple");
    xmlWriter.addAttribute("xlink:show", "embed");
    xmlWriter.addAttribute("xlink:actuate", "onLoad");

    QString opacity = QString("%1%").arg(properties.opacity);
    xmlWriter.addAttribute("draw:opacity", opacity);

    QString position;
    if(properties.horizontalPosition == Sheet::BackgroundImageProperties::Left) {
        position += "left";
    }
    else if(properties.horizontalPosition == Sheet::BackgroundImageProperties::HorizontalCenter) {
        position += "center";
    }
    else if(properties.horizontalPosition == Sheet::BackgroundImageProperties::Right) {
        position += "right";
    }

    position += ' ';

    if(properties.verticalPosition == Sheet::BackgroundImageProperties::Top) {
        position += "top";
    }
    else if(properties.verticalPosition == Sheet::BackgroundImageProperties::VerticalCenter) {
        position += "center";
    }
    else if(properties.verticalPosition == Sheet::BackgroundImageProperties::Bottom) {
        position += "right";
    }
    xmlWriter.addAttribute("style:position", position);

    QString repeat;
    if(properties.repeat == Sheet::BackgroundImageProperties::NoRepeat) {
        repeat = "no-repeat";
    }
    else if(properties.repeat == Sheet::BackgroundImageProperties::Repeat) {
        repeat = "repeat";
    }
    else if(properties.repeat == Sheet::BackgroundImageProperties::Stretch) {
        repeat = "stretch";
    }
    xmlWriter.addAttribute("style:repeat", repeat);

    xmlWriter.endElement();
}

void Odf::addText(const QString & text, KoXmlWriter & writer)
{
    if (!text.isEmpty())
        writer.addTextNode(text);
}

void Odf::convertPart(Sheet *sheet, const QString & part, KoXmlWriter & xmlWriter)
{
    QString text;
    QString var;

    bool inVar = false;
    uint i = 0;
    uint l = part.length();
    while (i < l) {
        if (inVar || part[i] == '<') {
            inVar = true;
            var += part[i];
            if (part[i] == '>') {
                inVar = false;
                if (var == "<page>") {
                    addText(text, xmlWriter);
                    xmlWriter.startElement("text:page-number");
                    xmlWriter.addTextNode("1");
                    xmlWriter.endElement();
                } else if (var == "<pages>") {
                    addText(text, xmlWriter);
                    xmlWriter.startElement("text:page-count");
                    xmlWriter.addTextNode("99");   //TODO I think that it can be different from 99
                    xmlWriter.endElement();
                } else if (var == "<date>") {
                    addText(text, xmlWriter);
                    //text:p><text:date style:data-style-name="N2" text:date-value="2005-10-02">02/10/2005</text:date>, <text:time>10:20:12</text:time></text:p> "add style" => create new style
#if 0 //FIXME
                    KoXmlElement t = dd.createElement("text:date");
                    t.setAttribute("text:date-value", "0-00-00");
                    // todo: "style:data-style-name", "N2"
                    t.appendChild(dd.createTextNode(QDate::currentDate().toString()));
                    parent.appendChild(t);
#endif
                } else if (var == "<time>") {
                    addText(text, xmlWriter);

                    xmlWriter.startElement("text:time");
                    xmlWriter.addTextNode(QTime::currentTime().toString());
                    xmlWriter.endElement();
                } else if (var == "<file>") { // filepath + name
                    addText(text, xmlWriter);
                    xmlWriter.startElement("text:file-name");
                    xmlWriter.addAttribute("text:display", "full");
                    xmlWriter.addTextNode("???");
                    xmlWriter.endElement();
                } else if (var == "<name>") { // filename
                    addText(text, xmlWriter);

                    xmlWriter.startElement("text:title");
                    xmlWriter.addTextNode("???");
                    xmlWriter.endElement();
                } else if (var == "<author>") {
                    DocBase* sdoc = sheet->doc();
                    KoDocumentInfo* docInfo = sdoc->documentInfo();

                    text += docInfo->authorInfo("creator");
                    addText(text, xmlWriter);
                } else if (var == "<email>") {
                    DocBase* sdoc = sheet->doc();
                    KoDocumentInfo* docInfo = sdoc->documentInfo();

                    text += docInfo->authorInfo("email");
                    addText(text, xmlWriter);

                } else if (var == "<org>") {
                    DocBase* sdoc = sheet->doc();
                    KoDocumentInfo* docInfo    = sdoc->documentInfo();

                    text += docInfo->authorInfo("company");
                    addText(text, xmlWriter);

                } else if (var == "<sheet>") {
                    addText(text, xmlWriter);

                    xmlWriter.startElement("text:sheet-name");
                    xmlWriter.addTextNode("???");
                    xmlWriter.endElement();
                } else {
                    // no known variable:
                    text += var;
                    addText(text, xmlWriter);
                }

                text.clear();
                var.clear();
            }
        } else {
            text += part[i];
        }
        ++i;
    }
    if (!text.isEmpty() || !var.isEmpty()) {
        //we don't have var at the end =>store it
        addText(text + var, xmlWriter);
    }
    debugSheetsODF << " text end :" << text << " var :" << var;
}

void Odf::saveHeaderFooter(Sheet *sheet, KoXmlWriter &xmlWriter)
{
    HeaderFooter *hf = sheet->print()->headerFooter();
    QString headerLeft = hf->headLeft();
    QString headerCenter = hf->headMid();
    QString headerRight = hf->headRight();

    QString footerLeft = hf->footLeft();
    QString footerCenter = hf->footMid();
    QString footerRight =hf->footRight();

    xmlWriter.startElement("style:header");
    if ((!headerLeft.isEmpty())
            || (!headerCenter.isEmpty())
            || (!headerRight.isEmpty())) {
        xmlWriter.startElement("style:region-left");
        xmlWriter.startElement("text:p");
        convertPart(sheet, headerLeft, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement("style:region-center");
        xmlWriter.startElement("text:p");
        convertPart(sheet, headerCenter, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement("style:region-right");
        xmlWriter.startElement("text:p");
        convertPart(sheet, headerRight, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();
    } else {
        xmlWriter.startElement("text:p");

        xmlWriter.startElement("text:sheet-name");
        xmlWriter.addTextNode("???");
        xmlWriter.endElement();

        xmlWriter.endElement();
    }
    xmlWriter.endElement();


    xmlWriter.startElement("style:footer");
    if ((!footerLeft.isEmpty())
            || (!footerCenter.isEmpty())
            || (!footerRight.isEmpty())) {
        xmlWriter.startElement("style:region-left");
        xmlWriter.startElement("text:p");
        convertPart(sheet, footerLeft, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement(); //style:region-left

        xmlWriter.startElement("style:region-center");
        xmlWriter.startElement("text:p");
        convertPart(sheet, footerCenter, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();

        xmlWriter.startElement("style:region-right");
        xmlWriter.startElement("text:p");
        convertPart(sheet, footerRight, xmlWriter);
        xmlWriter.endElement();
        xmlWriter.endElement();
    } else {
        xmlWriter.startElement("text:p");

        xmlWriter.startElement("text:sheet-name");
        xmlWriter.addTextNode("Page ");   // ???
        xmlWriter.endElement();

        xmlWriter.startElement("text:page-number");
        xmlWriter.addTextNode("1");   // ???
        xmlWriter.endElement();

        xmlWriter.endElement();
    }
    xmlWriter.endElement();
}

inline int compareCellInRow(const Cell &cell1, const Cell &cell2, int maxCols)
{
    if (cell1.isNull() != cell2.isNull())
        return 0;
    if (cell1.isNull())
        return 2;
    if (maxCols >= 0 && cell1.column() > maxCols)
        return 3;
    if (cell1.column() != cell2.column())
        return 0;
    if (!cell1.compareData(cell2))
        return 0;
    return 1;
}

inline bool compareCellsInRows(CellStorage *cellStorage, int row1, int row2, int maxCols)
{
    Cell cell1 = cellStorage->firstInRow(row1);
    Cell cell2 = cellStorage->firstInRow(row2);
    while (true) {
        int r = compareCellInRow(cell1, cell2, maxCols);
        if (r == 0)
            return false;
        if (r != 1)
            break;
        cell1 = cellStorage->nextInRow(cell1.column(), cell1.row());
        cell2 = cellStorage->nextInRow(cell2.column(), cell2.row());
    }
    return true;
}

bool Odf::compareRows(Sheet *sheet, int row1, int row2, int maxCols, OdfSavingContext& tableContext)
{
#if 0
    if (!sheet->rowFormats()->rowsAreEqual(row1, row2)) {
        return false;
    }
    if (tableContext.rowHasCellAnchoredShapes(sheet, row1) != tableContext.rowHasCellAnchoredShapes(sheet, row2)) {
        return false;
    }
    return compareCellsInRows(sheet->cellStorage(), row1, row2, maxCols);
#else
    Q_UNUSED(maxCols);

    // Optimized comparison by using the RowRepeatStorage to compare the content
    // rather then an expensive loop like compareCellsInRows.
    int row1repeated = sheet->cellStorage()->rowRepeat(row1);
    Q_ASSERT( row2 > row1 );
    if (row2 - row1 >= row1repeated) {
        return false;
    }

    // The RowRepeatStorage does not take to-cell anchored shapes into account
    // so we need to check for them explicit.
    if (tableContext.rowHasCellAnchoredShapes(sheet, row1) != tableContext.rowHasCellAnchoredShapes(sheet, row2)) {
        return false;
    }

    // Some sanity-checks to be sure our RowRepeatStorage works as expected.
    Q_ASSERT_X( sheet->rowFormats()->rowsAreEqual(row1, row2), __FUNCTION__, QString("Bug in RowRepeatStorage").toLocal8Bit() );
    //Q_ASSERT_X( compareCellsInRows(sheet->cellStorage(), row1, row2, maxCols), __FUNCTION__, QString("Bug in RowRepeatStorage").toLocal8Bit() );
    Q_ASSERT_X( compareCellInRow(sheet->cellStorage()->lastInRow(row1), sheet->cellStorage()->lastInRow(row2), -1), __FUNCTION__, QString("Bug in RowRepeatStorage").toLocal8Bit() );

    // If we reached that point then the both rows are equal.
    return true;
#endif
}






// *************** Settings *****************

void Odf::loadSheetSettings(Sheet *sheet, const KoOasisSettings::NamedMap &settings)
{
    // Find the entry in the map that applies to this sheet (by name)
    KoOasisSettings::Items items = settings.entry(sheet->sheetName());
    if (items.isNull())
        return;
    sheet->setHideZero(!items.parseConfigItemBool("ShowZeroValues"));
    sheet->setShowGrid(items.parseConfigItemBool("ShowGrid"));
    sheet->setFirstLetterUpper(items.parseConfigItemBool("FirstLetterUpper"));

    int cursorX = qMin(KS_colMax, qMax(1, items.parseConfigItemInt("CursorPositionX") + 1));
    int cursorY = qMin(KS_rowMax, qMax(1, items.parseConfigItemInt("CursorPositionY") + 1));
    sheet->map()->loadingInfo()->setCursorPosition(sheet, QPoint(cursorX, cursorY));

    double offsetX = items.parseConfigItemDouble("xOffset");
    double offsetY = items.parseConfigItemDouble("yOffset");
    sheet->map()->loadingInfo()->setScrollingOffset(sheet, QPointF(offsetX, offsetY));

    sheet->setShowFormulaIndicator(items.parseConfigItemBool("ShowFormulaIndicator"));
    sheet->setShowCommentIndicator(items.parseConfigItemBool("ShowCommentIndicator"));
    sheet->setShowPageOutline(items.parseConfigItemBool("ShowPageOutline"));
    sheet->setLcMode(items.parseConfigItemBool("lcmode"));
    sheet->setAutoCalculationEnabled(items.parseConfigItemBool("autoCalc"));
    sheet->setShowColumnNumber(items.parseConfigItemBool("ShowColumnNumber"));
}

void Odf::saveSheetSettings(Sheet *sheet, KoXmlWriter &settingsWriter)
{
    //not into each page into oo spec
    settingsWriter.addConfigItem("ShowZeroValues", !sheet->getHideZero());
    settingsWriter.addConfigItem("ShowGrid", sheet->getShowGrid());
    //not define into oo spec
    settingsWriter.addConfigItem("FirstLetterUpper", sheet->getFirstLetterUpper());
    settingsWriter.addConfigItem("ShowFormulaIndicator", sheet->getShowFormulaIndicator());
    settingsWriter.addConfigItem("ShowCommentIndicator", sheet->getShowCommentIndicator());
    settingsWriter.addConfigItem("ShowPageOutline", sheet->isShowPageOutline());
    settingsWriter.addConfigItem("lcmode", sheet->getLcMode());
    settingsWriter.addConfigItem("autoCalc", sheet->isAutoCalculationEnabled());
    settingsWriter.addConfigItem("ShowColumnNumber", sheet->getShowColumnNumber());
}

QString Odf::savePageLayout(PrintSettings *settings, KoGenStyles &mainStyles, bool formulas, bool zeros)
{
    // Create a page layout style.
    // 15.2.1 Page Size
    // 15.2.4 Print Orientation
    // 15.2.5 Margins
    KoGenStyle pageLayout = settings->pageLayout().saveOdf();

    // 15.2.13 Print
    QString printParameter;
    if (settings->printHeaders()) {
        printParameter = "headers ";
    }
    if (settings->printGrid()) {
        printParameter += "grid ";
    }
    /*    if (settings->printComments()) {
            printParameter += "annotations ";
        }*/
    if (settings->printObjects()) {
        printParameter += "objects ";
    }
    if (settings->printCharts()) {
        printParameter += "charts ";
    }
    /*    if (settings->printDrawings()) {
            printParameter += "drawings ";
        }*/
    if (formulas) {
        printParameter += "formulas ";
    }
    if (zeros) {
        printParameter += "zero-values ";
    }
    if (!printParameter.isEmpty()) {
        printParameter += "drawings"; //default print style attributes in OO
        pageLayout.addProperty("style:print", printParameter);
    }

    // 15.2.14 Print Page Order
    const QString pageOrder = (settings->pageOrder() == PrintSettings::LeftToRight) ? "ltr" : "ttb";
    pageLayout.addProperty("style:print-page-order", pageOrder);

    // 15.2.16 Scale
    // FIXME handle cases where only one direction is limited
    if (settings->pageLimits().width() > 0 && settings->pageLimits().height() > 0) {
        const int pages = settings->pageLimits().width() * settings->pageLimits().height();
        pageLayout.addProperty("style:scale-to-pages", pages);
    } else if (settings->zoom() != 1.0) {
        pageLayout.addProperty("style:scale-to", qRound(settings->zoom() * 100)); // in %
    }

    // 15.2.17 Table Centering
    if (settings->centerHorizontally() && settings->centerVertically()) {
        pageLayout.addProperty("style:table-centering", "both");
    } else if (settings->centerHorizontally()) {
        pageLayout.addProperty("style:table-centering", "horizontal");
    } else if (settings->centerVertically()) {
        pageLayout.addProperty("style:table-centering", "vertical");
    } else {
        pageLayout.addProperty("style:table-centering", "none");
    }

    // this is called from Sheet::saveOdfSheetStyleName for writing the SytleMaster so
    // the style has to be in the styles.xml file and only there
    pageLayout.setAutoStyleInStylesDotXml(true);

    return mainStyles.insert(pageLayout, "pm");
}





}  // Sheets
}  // Calligra

