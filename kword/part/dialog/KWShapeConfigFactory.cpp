/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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
#include "KWFrameConnectSelector.h"
#include <KWCanvas.h>
#include <frame/KWFrame.h>
#include <frame/KWFrameSet.h>

#include <KoTextShape.h>

#include <klocale.h>

KoShapeConfigWidgetBase *KWFrameConnectSelectorFactory::createConfigWidget(KoCanvasBase *canvas, KoShape *shape) {
    KWFrameConnectSelector *widget = new KWFrameConnectSelector(m_state);
    widget->open(shape);
    return widget;
}

QString KWFrameConnectSelectorFactory::name() const {
    return i18n("Connect Text Frames");
}

bool KWFrameConnectSelectorFactory::showForShapeId(const QString &id) const {
    return id == KoTextShape_SHAPEID;
}


KoShapeConfigWidgetBase *KWFrameGeometryFactory::createConfigWidget(KoCanvasBase *canvas, KoShape *shape) {
    return 0;
}

QString KWFrameGeometryFactory::name() const {
    return i18n("Geometry");
}


KoShapeConfigWidgetBase *KWFrameRunaroundPropertiesFactory::createConfigWidget(KoCanvasBase *canvas, KoShape *shape) {
    return 0;
}

QString KWFrameRunaroundPropertiesFactory::name() const {
    return i18n("Text Run Around");
}


KoShapeConfigWidgetBase *KWGeneralFramePropertiesFactory::createConfigWidget(KoCanvasBase *canvas, KoShape *shape) {
    KWGeneralFrameProperties *panel = new KWGeneralFrameProperties(m_state);
    panel->open(shape);
    return panel;
}

QString KWGeneralFramePropertiesFactory::name() const {
    return i18n("Options");
}


FrameConfigSharedState::FrameConfigSharedState(KWDocument *document)
    : m_refcount(0),
    m_frame(0),
    m_document(document)
{
}

FrameConfigSharedState::~FrameConfigSharedState() {
    delete m_frame;
}

void FrameConfigSharedState::removeUser() {
    m_refcount--;
    if(m_refcount == 0 && m_frame) {
        delete m_frame;
        m_frame = 0;
    }
}

KWFrame *FrameConfigSharedState::createFrame(KoShape *shape) {
    if(m_frame == 0) {
        KWFrameSet *fs = new KWFrameSet();
        m_frame = new KWFrame(shape, fs);
        m_document->addFrameSet(fs);
    }
    return m_frame;
}
