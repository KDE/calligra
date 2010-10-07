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
#include "KWGeneralFrameProperties.h"
#include "KWFrameRunaroundProperties.h"
#include "KWFrameConnectSelector.h"
#include "KWFrameGeometry.h"
#include <KWCanvas.h>
#include <frames/KWFrame.h>
#include <frames/KWFrameSet.h>

#include <klocale.h>

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


KoShapeConfigWidgetBase *KWFrameGeometryFactory::createConfigWidget(KoShape *shape)
{
    KWFrameGeometry *widget = new KWFrameGeometry(m_state);
    widget->open(shape);
    return widget;
}

QString KWFrameGeometryFactory::name() const
{
    return i18n("Geometry");
}

bool KWFrameGeometryFactory::showForShapeId(const QString &id) const
{
    return id == TextShape_SHAPEID;
}


KoShapeConfigWidgetBase *KWFrameRunaroundPropertiesFactory::createConfigWidget(KoShape *shape)
{
    KWFrameRunaroundProperties *widget = new KWFrameRunaroundProperties(m_state);
    widget->open(shape);
    return widget;
}

QString KWFrameRunaroundPropertiesFactory::name() const
{
    return i18n("Text Run Around");
}

bool KWFrameRunaroundPropertiesFactory::showForShapeId(const QString &id) const
{
    return id == TextShape_SHAPEID;
}


KoShapeConfigWidgetBase *KWGeneralFramePropertiesFactory::createConfigWidget(KoShape *shape)
{
    KWGeneralFrameProperties *panel = new KWGeneralFrameProperties(m_state);
    panel->open(shape);
    return panel;
}

QString KWGeneralFramePropertiesFactory::name() const
{
    return i18n("Options");
}

bool KWGeneralFramePropertiesFactory::showForShapeId(const QString &id) const
{
    return id == TextShape_SHAPEID;
}


FrameConfigSharedState::FrameConfigSharedState(KWDocument *document)
        : QObject(document),
        m_refcount(0),
        m_deleteFrame(false),
        m_protectAspectRatio(false),
        m_frame(0),
        m_document(document)
{
}

FrameConfigSharedState::~FrameConfigSharedState()
{
    if (m_deleteFrame)
        delete m_frame;
}

void FrameConfigSharedState::removeUser()
{
    m_refcount--;
    Q_ASSERT(m_refcount >= 0);
    if (m_refcount == 0 && m_frame) {
        if (m_deleteFrame)
            delete m_frame;
        m_frame = 0;
    }
}

void FrameConfigSharedState::addUser()
{
    ++m_refcount;
}

KWFrame *FrameConfigSharedState::createFrame(KoShape *shape)
{
    if (m_frame == 0) {
        KWFrameSet *fs = new KWFrameSet();
        m_frame = new KWFrame(shape, fs);
        m_document->addFrameSet(fs);
        m_deleteFrame = false;
    }
    return m_frame;
}

void FrameConfigSharedState::setKeepAspectRatio(bool on)
{
    if (m_protectAspectRatio == on)
        return;
    m_protectAspectRatio = on;
    emit keepAspectRatioChanged(on);
}

void FrameConfigSharedState::setFrame(KWFrame *frame)
{
    m_deleteFrame = true;
    m_frame = frame;
}

#include <KWShapeConfigFactory.moc>
