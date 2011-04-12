/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
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

#include "SvgTextHelper.h"
#include "SvgUtil.h"
#include "SvgGraphicContext.h"

#include <KoXmlReader.h>
#include <KDebug>

#include <math.h>

SvgTextHelper::SvgTextHelper()
    : m_textPosition(HUGE_VAL, HUGE_VAL)
{

}

QString SvgTextHelper::simplifyText(const QString &text, bool preserveWhiteSpace, bool hasSibling)
{
    // simplifies text according ot the svg specification
    QString simple = text;
    simple.remove('\n');
    simple.replace('\t', ' ');
    if (preserveWhiteSpace)
        return simple;

    QString stripped = simple.simplified();
    // preserve last whitespace character if we have a next sibling text range
    if (hasSibling && simple.endsWith(' '))
        stripped += QChar(' ');

    return stripped;
}

SvgTextHelper::OffsetType SvgTextHelper::xOffsetType() const
{
    if(m_absolutePosX.last().count())
        return Absolute;
    else if(m_relativePosX.last().count())
        return Relative;
    else
        return None;
}

SvgTextHelper::OffsetType SvgTextHelper::yOffsetType() const
{
    if(m_absolutePosY.last().count())
        return Absolute;
    else if(m_relativePosY.last().count())
        return Relative;
    else
        return None;
}

CharTransforms SvgTextHelper::xOffsets(int count) const
{
    switch(xOffsetType()) {
    case Absolute: {
        const QPointF origin = textPosition();
        CharTransforms offsets = collectValues(count, m_absolutePosX);
        const int offsetCount = offsets.count();
        for (int i = 0; i < offsetCount; ++i) {
            offsets[i] -= origin.x();
        }
        return offsets;
    }
    case Relative:
        return collectValues(count, m_relativePosX);
    default:
        return CharTransforms();
    }
}

CharTransforms SvgTextHelper::yOffsets(int count) const
{
    switch(yOffsetType()) {
    case Absolute: {
        const QPointF origin = textPosition();
        CharTransforms offsets = collectValues(count, m_absolutePosY);
        const int offsetCount = offsets.count();
        for (int i = 0; i < offsetCount; ++i) {
            offsets[i] -= origin.y();
        }
        return offsets;
    }
    case Relative:
        return collectValues(count, m_relativePosY);
    default:
        return CharTransforms();
    }
}

CharTransforms SvgTextHelper::rotations(int count) const
{
    return collectValues(count, m_rotations);
}

QPointF SvgTextHelper::textPosition() const
{
    qreal x = 0.0, y = 0.0;
    if (m_textPosition.x() != HUGE_VAL)
        x = m_textPosition.x();
    if (m_textPosition.y() != HUGE_VAL)
        y = m_textPosition.y();

    return QPointF(x, y);
}

void SvgTextHelper::pushCharacterTransforms(const KoXmlElement &element, SvgGraphicsContext *gc)
{
    parseList(element.attribute("x"), m_absolutePosX, gc, XLength);
    parseList(element.attribute("y"), m_absolutePosY, gc, YLength);
    parseList(element.attribute("dx"), m_relativePosX, gc, XLength);
    parseList(element.attribute("dy"), m_relativePosY, gc, YLength);
    parseList(element.attribute("rotate"), m_rotations, gc, Number);

    if (m_textPosition.x() == HUGE_VAL && m_absolutePosX.last().count()) {
        m_textPosition.setX(m_absolutePosX.last().first());
    }
    if (m_textPosition.y() == HUGE_VAL && m_absolutePosY.last().count()) {
        m_textPosition.setY(m_absolutePosY.last().first());
    }
}

void SvgTextHelper::popCharacterTransforms()
{
    m_absolutePosX.pop_back();
    m_absolutePosY.pop_back();
    m_relativePosX.pop_back();
    m_relativePosY.pop_back();
    m_rotations.pop_back();
}

void SvgTextHelper::stripCharacterTransforms(int count)
{
    m_absolutePosX.last() = m_absolutePosX.last().count() > count ? m_absolutePosX.last().mid(count) : CharTransforms();
    m_absolutePosY.last() = m_absolutePosY.last().count() > count ? m_absolutePosY.last().mid(count) : CharTransforms();
    m_relativePosX.last() = m_relativePosX.last().count() > count ? m_relativePosX.last().mid(count) : CharTransforms();
    m_relativePosY.last() = m_relativePosY.last().count() > count ? m_relativePosY.last().mid(count) : CharTransforms();
    m_rotations.last() = m_rotations.last().count() > count ? m_rotations.last().mid(count) : CharTransforms();
}

void SvgTextHelper::parseList(const QString &listString, CharTransformStack &stack, SvgGraphicsContext *gc, ValueType type)
{
    if (listString.isEmpty()) {
        stack.append(CharTransforms());
    } else {
        CharTransforms values;
        QStringList offsets = QString(listString).replace(',', ' ').simplified().split(' ');
        values.reserve(offsets.count());
        foreach(const QString &offset, offsets) {
            switch(type) {
            case Number:
                values.append(offset.toDouble());
                break;
            case XLength:
                values.append(SvgUtil::parseUnitX(gc, offset));
                break;
            case YLength:
                values.append(SvgUtil::parseUnitY(gc, offset));
                break;
            }
        }
        stack.append(values);
    }
}

CharTransforms SvgTextHelper::collectValues(int count, const CharTransformStack &stack) const
{
    // do we have enough values ?
    if (stack.last().count() >= count) {
        return stack.last().mid(0, count);
    } else {
        // collect values from ancestors
        CharTransforms collected = stack.last();
        QListIterator<CharTransforms> it(stack);
        it.toBack();
        while(it.hasPrevious()) {
            const CharTransforms &prev = it.previous();
            if (prev.count() > collected.count()) {
                int count = qMin(count, prev.count());
                for(int i = collected.count(); i < count; ++i) {
                    collected.append(prev[i]);
                }
            } else {
                break;
            }
        }
        return collected;
    }
}
