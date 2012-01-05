/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2011 KoGmbh <cbo@kogmbh.com>
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

#include "KWFrameDialog.h"

#include "KWShapeConfigFactory.h"
#include "KWFrameConnectSelector.h"
#include "KWRunAroundProperties.h"
#include "KWGeneralFrameProperties.h"
#include "KWAnchoringProperties.h"
#include "frames/KWFrame.h"

KWFrameDialog::KWFrameDialog(const QList<KWFrame*> &frames, KWDocument *document, QWidget *parent)
        : KPageDialog(parent),
        m_frameConnectSelector(0)
{
    m_state = new FrameConfigSharedState(document);
    setFaceType(Tabbed);
    m_generalFrameProperties = new KWGeneralFrameProperties(m_state);
    addPage(m_generalFrameProperties, i18n("General"));
    m_anchoringProperties = new KWAnchoringProperties(m_state);
    addPage(m_anchoringProperties, i18n("Smart Positioning"));
    m_runAroundProperties = new KWRunAroundProperties(m_state);
    addPage(m_runAroundProperties, i18n("Text Run Around"));

    if (frames.count() == 1) {
        m_frameConnectSelector = new KWFrameConnectSelector(m_state);
        KWFrame *frame = frames.first();
        m_state->setKeepAspectRatio(frame->shape()->keepAspectRatio());
        if (m_frameConnectSelector->open(frame))
            addPage(m_frameConnectSelector, i18n("Connect Text Frames"));
        else {
            delete m_frameConnectSelector;
            m_frameConnectSelector = 0;
        }
    }

    m_generalFrameProperties->open(frames);
    m_anchoringProperties->open(frames);
    m_runAroundProperties->open(frames);

    connect(this, SIGNAL(okClicked()), this, SLOT(okClicked()));
    connect(this, SIGNAL(cancelClicked()), this, SLOT(cancelClicked()));
}

KWFrameDialog::~KWFrameDialog()
{
}

void KWFrameDialog::okClicked()
{
    if (m_frameConnectSelector)
        m_frameConnectSelector->save();
    
    m_generalFrameProperties->save();
    m_anchoringProperties->save();
    m_runAroundProperties->save();
}

void KWFrameDialog::cancelClicked()
{
}

// static
QList<KoShapeConfigFactoryBase *> KWFrameDialog::panels(KWDocument *doc)
{
    QList<KoShapeConfigFactoryBase *> answer;
    FrameConfigSharedState *state = new FrameConfigSharedState(doc);
    answer.append(new KWFrameConnectSelectorFactory(state));
    answer.append(new KWRunAroundPropertiesFactory(state));
    answer.append(new KWGeneralFramePropertiesFactory(state));
    return answer;
}

