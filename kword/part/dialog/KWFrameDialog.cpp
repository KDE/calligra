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

#include "KWFrameDialog.h"

#include "KWShapeConfigFactory.h"
#include "KWFrameConnectSelector.h"
#include "KWFrameRunaroundProperties.h"
#include "KWGeneralFrameProperties.h"
#include "frame/KWFrame.h"

#include "KoShapeGeometry.h"

KWFrameDialog::KWFrameDialog (const QList<KWFrame*> &frames, KWDocument *document, QWidget *parent)
    : KPageDialog(parent),
    m_frameConnectSelector(0),
    m_frameGeometry(0)
{
    m_state = new FrameConfigSharedState(document);
    setFaceType(Tabbed);
    m_generalFrameProperties = new KWGeneralFrameProperties(m_state);
    addPage(m_generalFrameProperties, i18n("Options"));
    m_frameRunaroundProperties = new KWFrameRunaroundProperties(m_state);
    addPage(m_frameRunaroundProperties, i18n("Text Run Around"));

    if(frames.count() == 1) {
        m_frameConnectSelector = new KWFrameConnectSelector(m_state);
        KWFrame *frame = frames.first();
        if(m_frameConnectSelector->open(frame))
            addPage(m_frameConnectSelector, i18n("Connect Text Frames"));
        else {
            delete m_frameConnectSelector;
            m_frameConnectSelector = 0;
        }
        m_frameGeometry = new KoShapeGeometry();
        m_frameGeometry->open(frame->shape());
        addPage(m_frameGeometry, i18n("Geometry"));
    }

    m_generalFrameProperties->open(frames);
    m_frameRunaroundProperties->open(frames);

    connect(this, SIGNAL( okClicked() ), this, SLOT( okClicked() ));
    connect(this, SIGNAL( cancelClicked() ), this, SLOT( cancelClicked() ));
}

KWFrameDialog::~KWFrameDialog() {
}

void KWFrameDialog::okClicked() {
    if(m_frameConnectSelector)
        m_frameConnectSelector->save();
    m_generalFrameProperties->save();
    m_frameRunaroundProperties->save();
    if(m_frameGeometry)
        m_frameGeometry->save();
}

void KWFrameDialog::cancelClicked() {
    if(m_frameGeometry)
        m_frameGeometry->cancel();
}

// static
QList<KoShapeConfigFactory *> KWFrameDialog::panels(KWDocument *doc) {
    QList<KoShapeConfigFactory *> answer;
    FrameConfigSharedState *state = new FrameConfigSharedState(doc);
    answer.append(new KWFrameConnectSelectorFactory(state));
    answer.append(new KWFrameGeometryFactory(state));
    answer.append(new KWFrameRunaroundPropertiesFactory(state));
    answer.append(new KWGeneralFramePropertiesFactory(state));
    return answer;
}

#include "KWFrameDialog.moc"
