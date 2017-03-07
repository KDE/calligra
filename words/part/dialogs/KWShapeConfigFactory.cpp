/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
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

#include "KWShapeConfigFactory.h"
#include "KWRunAroundProperties.h"
#include "KWFrameConnectSelector.h"
#include "KWAnchoringProperties.h"
#include <KWCanvas.h>
#include <frames/KWFrame.h>
#include <frames/KWFrameSet.h>

#include <klocalizedstring.h>

KoShapeConfigWidgetBase *KWFrameConnectSelectorFactory::createConfigWidget(KoShape *shape)
{
    KWFrameConnectSelector *widget = new KWFrameConnectSelector(m_state);
    widget->open(shape);
    return widget;
}

QString KWFrameConnectSelectorFactory::name() const
{
    return i18n("Connect Text Frames");
}

bool KWFrameConnectSelectorFactory::showForShapeId(const QString &id) const
{
    return id == TextShape_SHAPEID;
}


KoShapeConfigWidgetBase *KWAnchoringFactory::createConfigWidget(KoShape *shape)
{
    KWAnchoringProperties *widget = new KWAnchoringProperties(m_state);
    widget->open(shape);
    return widget;
}

QString KWAnchoringFactory::name() const
{
    return i18n("Geometry");
}

bool KWAnchoringFactory::showForShapeId(const QString &id) const
{
    return id == TextShape_SHAPEID;
}


KoShapeConfigWidgetBase *KWRunAroundPropertiesFactory::createConfigWidget(KoShape *shape)
{
    KWRunAroundProperties *widget = new KWRunAroundProperties(m_state);
    widget->open(shape);
    return widget;
}

QString KWRunAroundPropertiesFactory::name() const
{
    return i18n("Text Run Around");
}

bool KWRunAroundPropertiesFactory::showForShapeId(const QString &id) const
{
    return id == TextShape_SHAPEID;
}

FrameConfigSharedState::FrameConfigSharedState(KWDocument *document)
        : QObject(document),
        m_refcount(0),
        m_protectAspectRatio(false),
        m_document(document)
{
}

FrameConfigSharedState::~FrameConfigSharedState()
{
}

void FrameConfigSharedState::removeUser()
{
    m_refcount--;
    Q_ASSERT(m_refcount >= 0);
}

void FrameConfigSharedState::addUser()
{
    ++m_refcount;
}

void FrameConfigSharedState::setKeepAspectRatio(bool on)
{
    if (m_protectAspectRatio == on)
        return;
    m_protectAspectRatio = on;
    emit keepAspectRatioChanged(on);
}
