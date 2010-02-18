/* This file is part of the KDE project
 * Copyright (C) 2006-2009 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef KWPAGESTYLE_P_H
#define KWPAGESTYLE_P_H

#include <QSharedData>
#include <QImage>
#include <QColor>
#include <KoText.h>
#include <KoParagraphStyle.h>
#include <KoPageLayout.h>
#include <KWord.h>
#include <KoShapeBackground.h>

// See parag 15.2 for all the ODF features of page-layout-properties
class KWPageStylePrivate : public QSharedData
{
public:
    KWPageStylePrivate() { clear(); }

    void clear();

    QString name;
    bool mainFrame;
    qreal headerDistance;
    qreal footerDistance;
    qreal endNoteDistance;
    KWord::HeaderFooterType headers, footers;

    // ODF 15.2 properties
    KoPageLayout pageLayout;

    KoColumns columns;

    /**
     * The style:writing mode attribute specifies the writing mode that should is used by all
     * paragraphs that appear on the page. See section 15.5.36 for details. The value page is not
     * allowed within page layouts.
     */
    KoText::Direction direction;

    qreal footNoteSeparatorLineWidth; ///< width of line; so more like 'thickness'
    int footNoteSeparatorLineLength; ///< It's a percentage of page.
    Qt::PenStyle footNoteSeparatorLineType; ///< foot note separate type
    KWord::FootNoteSeparatorLinePos footNoteSeparatorLinePos; ///< alignment in page
    qreal footNoteDistance; ///< KWord doesn't use the ODF mandated before sep and after sep values
    qreal footNoteDistanceBeforeSeparator;
    qreal footNoteDistanceAfterSeparator;

    // shadow (15.2.9)
    /**
     * The shadow attribute style:shadow specifies the shadow of the page. See section 15.5.28 for
     * detailed information on this attribute.
     */
    QString shadow; // In CSS, see 15.4.27 -- this used be parsed in KOffice 1.x

    // background (15.5.23, 15.5.24)
    /**
     * The background attribute fo:background-color and the background element
     * <style:background-image> specify the background properties of the page. See sections
     * 15.5.23 and 15.5.24 for detailed information on this attribute and element.
     */
    KoShapeBackground *background;

    // page number format (12.2)
    /**
     * The style:num-format, style:num-prefix and style:num-suffix attributes specify a
     * default number format for page styles, which is used to display page numbers within headers and
     * footers. See section 12.2 for detailed information on number format attributes.
     *
     * The style:num-format attribute can be empty. In this case, no page number will be displayed
     * by default.
     */
    QString pageNumberFormat;

    // paper tray (15.2.3)
    /**
     * The style:paper-tray-name attribute specifies the paper tray to use when printing the
     * document. The names assigned to the printer trays depend on the printer. If the value of this
     * attribute is default, the default tray specified in the printer configuration settings is used.
     */
    QString paperTrayName;

    // register-truth (15.2.12)
    /**
     * The style:register-truth-ref-style-name attribute references a paragraph style. The
     * line distance specified of the paragraph style is used as the reference line distance for
     * all paragraphs that have the register-truth feature enabled.
     */
    QString registerTruthStyleName;

    // first page number (15.2.15)
    /**
     * The style:first-page-number specifies the number of the first page of a text or graphical
     * document, or for the first page of a table within a spreadsheet document.
     * The value of this attribute can be an integer or continue. If the value is continue, the page
     * number is the preceding page number incremented by 1. The default first page number is 1.
     *
     * continue is represented by 0.
     */
    int firstPageNumber;

    // maximum footnote height (
    /**
     * The style:footnote-max-height attribute specifies the maximum amount of space on the
     * page that a footnote can occupy. The value of the attribute is a length, which determines the
     * maximum height of the footnote area.
     * If the value of this attribute is set to 0in, there is no limit to the amount of space that the footnote
     * can occupy.
     */
    QString maximumFootnoteHeight;



};

#endif
