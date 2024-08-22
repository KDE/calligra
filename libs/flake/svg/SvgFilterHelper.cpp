/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgFilterHelper.h"
#include "SvgUtil.h"

SvgFilterHelper::SvgFilterHelper()
    : m_filterUnits(ObjectBoundingBox) // default as per svg spec
    , m_primitiveUnits(UserSpaceOnUse) // default as per svg spec
    , m_position(-0.1, -0.1) // default as per svg spec
    , m_size(1.2, 1.2) // default as per svg spec
{
}

SvgFilterHelper::~SvgFilterHelper() = default;

void SvgFilterHelper::setFilterUnits(Units filterUnits)
{
    m_filterUnits = filterUnits;
}

SvgFilterHelper::Units SvgFilterHelper::filterUnits() const
{
    return m_filterUnits;
}

void SvgFilterHelper::setPrimitiveUnits(Units primitiveUnits)
{
    m_primitiveUnits = primitiveUnits;
}

SvgFilterHelper::Units SvgFilterHelper::primitiveUnits() const
{
    return m_primitiveUnits;
}

void SvgFilterHelper::setPosition(const QPointF &position)
{
    m_position = position;
}

QPointF SvgFilterHelper::position(const QRectF &objectBound) const
{
    if (m_filterUnits == UserSpaceOnUse) {
        return m_position;
    } else {
        return SvgUtil::objectToUserSpace(m_position, objectBound);
    }
}

void SvgFilterHelper::setSize(const QSizeF &size)
{
    m_size = size;
}

QSizeF SvgFilterHelper::size(const QRectF &objectBound) const
{
    if (m_filterUnits == UserSpaceOnUse) {
        return m_size;
    } else {
        return SvgUtil::objectToUserSpace(m_size, objectBound);
    }
}

void SvgFilterHelper::setContent(const KoXmlElement &content)
{
    m_filterContent = content;
}

KoXmlElement SvgFilterHelper::content() const
{
    return m_filterContent;
}
