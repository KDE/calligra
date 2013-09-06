/* This file is part of the KDE project
 * Copyright (C)  2006 Thomas Zander <zander@kde.org>
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

#include "OdfDefines.h"

#include <KoUnit.h>
#include <KoCharacterStyle.h>

#include <klocale.h>

using namespace KoOdf;

#if 0
QStringList KoOdf::underlineTypeList()
{
    QStringList lst;
    lst << i18nc("Underline Style", "None");
    lst << i18nc("Underline Style", "Single");
    lst << i18nc("Underline Style", "Double");
    return lst;
}
#endif
#if 0
QStringList KoOdf::underlineStyleList()
{
    QStringList lst;
    lst << "_________";  // solid
    lst << "___ ___ __"; // dash
    lst << "_ _ _ _ _ _"; // dot
    lst << "___ _ ___ _"; // dash_dot
    lst << "___ _ _ ___"; // dash_dot_dot
    lst << "~~~~~~~"; // wavy lines
    return lst;
}
#endif


KoOdf::Tab::Tab()
    : position(0.)
    , type(QTextOption::LeftTab)
    , leaderType(KoOdf::NoLineType)
    , leaderStyle(KoOdf::NoLineStyle)
    , leaderWeight(KoOdf::AutoLineWeight)
    , leaderWidth(0)
{
}

bool KoOdf::Tab::operator==(const Tab &other) const
{
    return other.position == position &&
           other.type == type &&
           other.delimiter == delimiter &&
           other.leaderStyle == leaderStyle &&
           other.leaderColor == leaderColor &&
           other.leaderText == leaderText ;
}

Qt::Alignment KoOdf::alignmentFromString(const QString &align)
{
    Qt::Alignment alignment = Qt::AlignLeft;
    if (align == "left")
        alignment = Qt::AlignLeft | Qt::AlignAbsolute;
    else if (align == "right")
        alignment = Qt::AlignRight | Qt::AlignAbsolute;
    else if (align == "start")
        alignment = Qt::AlignLeading;
    else if (align == "end")
        alignment = Qt::AlignTrailing;
    else if (align == "center")
        alignment = Qt::AlignHCenter;
    else if (align == "justify")
        alignment = Qt::AlignJustify;
    else if (align == "margins") // in tables this is effectively the same as justify
        alignment = Qt::AlignJustify;
    return alignment;
}

QString KoOdf::alignmentToString(Qt::Alignment alignment)
{
    QString align;

    alignment &= Qt::AlignHorizontal_Mask;
    if (alignment == (Qt::AlignLeft | Qt::AlignAbsolute))
        align = "left";
    else if (alignment == (Qt::AlignRight | Qt::AlignAbsolute))
        align = "right";
    else if (alignment == Qt::AlignLeading)
        align = "start";
    else if (alignment == Qt::AlignTrailing)
        align = "end";
    else if (alignment == Qt::AlignHCenter)
        align = "center";
    else if (alignment == Qt::AlignJustify)
        align = "justify";
    return align;
}

Qt::Alignment KoOdf::valignmentFromString(const QString &align)
{
    Qt::Alignment alignment = Qt::AlignTop;
    if (align == "top")
        alignment = Qt::AlignTop;
    else if (align == "middle")
        alignment = Qt::AlignVCenter;
    else if (align == "bottom")
        alignment = Qt::AlignBottom;
    return alignment;
}

QString KoOdf::valignmentToString(Qt::Alignment alignment)
{
    QString align;
    alignment &= Qt::AlignVertical_Mask;
    if (alignment == (Qt::AlignTop))
        align = "top";
    else if (alignment == Qt::AlignVCenter)
        align = "middle";
    else if (alignment == Qt::AlignBottom)
        align = "bottom";
    else
        align = "automatic";
    return align;
}

KoOdf::TextDirection KoOdf::textDirectionFromString(const QString &writingMode)
{
    // LTR is lr-tb. RTL is rl-tb
    if (writingMode == "lr" || writingMode == "lr-tb")
        return KoOdf::LeftRightTopBottom;
    if (writingMode == "rl" || writingMode == "rl-tb")
        return KoOdf::RightLeftTopBottom;
    if (writingMode == "tb" || writingMode == "tb-rl")
        return KoOdf::TopBottomRightLeft;
    if (writingMode == "tb-lr")
        return KoOdf::TopBottomLeftRight;
    if (writingMode == "page")
        return KoOdf::InheritDirection;
    return KoOdf::AutoDirection;
}

QString KoOdf::textDirectionToString(KoOdf::TextDirection direction)
{
    if (direction == KoOdf::LeftRightTopBottom)
        return "lr";
    if (direction == KoOdf::RightLeftTopBottom)
        return "rl";
    if (direction == KoOdf::TopBottomRightLeft)
        return "tb-rl";
    if (direction == KoOdf::TopBottomLeftRight)
        return "tb-lr";
    if (direction == KoOdf::InheritDirection)
        return "page";

    return "auto";
}

KoOdf::KoTextBreakProperty KoOdf::textBreakFromString(const QString& textBreak)
{
    if (textBreak == "page")
        return KoOdf::PageBreak;
    if (textBreak == "column")
        return KoOdf::ColumnBreak;
    return KoOdf::NoBreak;
}

QString KoOdf::textBreakToString(KoOdf::KoTextBreakProperty textBreak)
{
    if (textBreak == KoOdf::PageBreak)
        return "page";
    if (textBreak == KoOdf::ColumnBreak)
        return "column";
    return "auto";
}

QTextLength KoOdf::parseLength(const QString &length)
{
    if (length.contains('%')) {
        QString lengthValue = length.left(length.indexOf('%'));
        bool ok = false;
        qreal realLength = lengthValue.toDouble(&ok);
        if (ok)
            return QTextLength(QTextLength::PercentageLength, realLength);
        else
            return QTextLength(QTextLength::PercentageLength, 0);
    }
    else {
        return QTextLength(QTextLength::FixedLength, KoUnit::parseValue(length));
    }
}
