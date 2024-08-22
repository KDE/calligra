/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SvgClipPathHelper.h"

SvgClipPathHelper::SvgClipPathHelper()
    : m_clipPathUnits(UserSpaceOnUse) // default as per svg spec
{
}

SvgClipPathHelper::~SvgClipPathHelper() = default;

void SvgClipPathHelper::setClipPathUnits(Units clipPathUnits)
{
    m_clipPathUnits = clipPathUnits;
}

SvgClipPathHelper::Units SvgClipPathHelper::clipPathUnits() const
{
    return m_clipPathUnits;
}

void SvgClipPathHelper::setContent(const KoXmlElement &content)
{
    m_content = content;
}

KoXmlElement SvgClipPathHelper::content() const
{
    return m_content;
}
