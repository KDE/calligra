/* This file is part of the KDE project
 * Copyright (C)  2006, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C)  2008 Girish Ramakrishnan <girish@forwardbias.in>
 * Copyright (C)  2011 Pierre Ducroquet <pinaraf@pinaraf.info>
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
#ifndef ODFDEFINES_H
#define ODFDEFINES_H

#include <QChar>
#include <QColor>
#include <QTextCharFormat>
#include <QString>


#include "koodf_export.h"

/**
 * Generic namespace of the Calligra ODF library for helper methods and data.
 */
namespace KoOdf
{

// ----------------------------------------------------------------
//                         Text properties


/// list of possible line type : no line, single line, double line
enum LineType {
    NoLineType,
    SingleLine,
    DoubleLine
};

/// list of possible line style.
enum LineStyle {
    NoLineStyle = Qt::NoPen,
    SolidLine = Qt::SolidLine,
    DottedLine = Qt::DotLine,
    DashLine = Qt::DashLine,
    DotDashLine = Qt::DashDotLine,
    DotDotDashLine = Qt::DashDotDotLine,
    LongDashLine,
    WaveLine
};

enum LineWeight {
    AutoLineWeight,
    NormalLineWeight,
    BoldLineWeight,
    ThinLineWeight,
    DashLineWeight, // ## ??what the heck does this mean??
    MediumLineWeight,
    ThickLineWeight,
    PercentLineWeight,
    LengthLineWeight
};


/// Text in the objects will be positioned according to the direction.
enum TextDirection {
    AutoDirection,      ///< Take the direction from the text.
    LeftRightTopBottom, ///< Text layout for most western languages
    RightLeftTopBottom, ///< Text layout for languages like Hebrew
    TopBottomRightLeft,  ///< Vertical text layout.
    TopBottomLeftRight,  ///< Vertical text layout. ?
    InheritDirection    ///< Direction is unspecified and should come from the container
};
/// convert the string version of directions (as specified in XSL and ODF) to the Direction enum
KOODF_EXPORT TextDirection textDirectionFromString(const QString &direction);
/// convert the TextDirection enum to the string version of directions (as specified in XSL and ODF)
KOODF_EXPORT QString textDirectionToString(TextDirection direction);


// ----------------------------------------------------------------
//                     Paragraph properties


/// There are several possible text breaks
enum KoTextBreakProperty {
    NoBreak = 0,     ///< No text break
    ColumnBreak,     ///< Column break
    PageBreak        ///< Page break
};
/// convert the string version of text break (as specified in ODF) to the KoTextBreakProperty enum
KOODF_EXPORT KoTextBreakProperty textBreakFromString(const QString &textBreak);
/// convert the KoTextBreakProperty enum to the string version of text break (as specified in ODF)
KOODF_EXPORT QString textBreakToString(KoTextBreakProperty textBreak);



/// For paragraphs each tab definition is represented by this struct.
//
// FIXME: Should be called TabData?
struct KOODF_EXPORT Tab
{
    Tab();

    bool operator==(const Tab &tab) const;

    qreal position;    ///< distance in ps-points from the edge of the text-shape
    QTextOption::TabType type;       ///< Determine which type is used.
    QChar delimiter;    ///< If type is DelimitorTab; tab until this char was found in the text.
    LineType leaderType; // none/single/double
    LineStyle leaderStyle; // solid/dotted/dash/...
    LineWeight leaderWeight; // auto/bold/thin/length/percentage/...
    qreal leaderWidth; // the width value if length/percentage
    QColor leaderColor; ///< if color is valid, then use this instead of the (current) text color
    QString leaderText;   ///< character to print as the leader (filler of the tabbed space)
};



KOODF_EXPORT Qt::Alignment alignmentFromString(const QString &align);
KOODF_EXPORT QString alignmentToString(Qt::Alignment align);
KOODF_EXPORT Qt::Alignment valignmentFromString(const QString &align);
KOODF_EXPORT QString valignmentToString(Qt::Alignment align);

///@TODO: move to KoUnit ?
KOODF_EXPORT QTextLength parseLength (const QString &length);

}


#endif
