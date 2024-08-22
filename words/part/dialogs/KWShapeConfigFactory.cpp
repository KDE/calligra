/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWShapeConfigFactory.h"
#include "KWAnchoringProperties.h"
#include "KWFrameConnectSelector.h"
#include "KWRunAroundProperties.h"
#include <KWCanvas.h>
#include <frames/KWFrame.h>
#include <frames/KWFrameSet.h>

#include <KLocalizedString>

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
    : QObject(document)
    , m_refcount(0)
    , m_protectAspectRatio(false)
    , m_document(document)
{
}

FrameConfigSharedState::~FrameConfigSharedState() = default;

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
    Q_EMIT keepAspectRatioChanged(on);
}
