/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 KoGmbh <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWFrameDialog.h"

#include <kundo2command.h>

#include "KWAnchoringProperties.h"
#include "KWCanvas.h"
#include "KWFrameConnectSelector.h"
#include "KWRunAroundProperties.h"
#include "KWShapeConfigFactory.h"
#include "frames/KWFrameSet.h"

KWFrameDialog::KWFrameDialog(const QList<KoShape *> &shapes, KWDocument *document, KWCanvas *canvas)
    : KPageDialog(canvas)
    , m_frameConnectSelector(nullptr)
    , m_canvas(canvas)
{
    m_state = new FrameConfigSharedState(document);
    setFaceType(Tabbed);

    m_anchoringProperties = new KWAnchoringProperties(m_state);
    if (m_anchoringProperties->open(shapes))
        addPage(m_anchoringProperties, i18n("Smart Positioning"));

    m_runAroundProperties = new KWRunAroundProperties(m_state);
    if (m_runAroundProperties->open(shapes))
        addPage(m_runAroundProperties, i18n("Text Run Around"));

    if (shapes.count() == 1) {
        m_frameConnectSelector = new KWFrameConnectSelector(m_state);
        KoShape *shape = shapes.first();
        m_state->setKeepAspectRatio(shape->keepAspectRatio());
        if (m_frameConnectSelector->canOpen(shape)) {
            m_frameConnectSelector->open(shape);
            addPage(m_frameConnectSelector, i18n("Connect Text Frames"));
        } else {
            delete m_frameConnectSelector;
            m_frameConnectSelector = nullptr;
        }
    }

    connect(this, &QDialog::accepted, this, &KWFrameDialog::okClicked);
    connect(this, &QDialog::rejected, this, &KWFrameDialog::cancelClicked);
}

KWFrameDialog::~KWFrameDialog() = default;

void KWFrameDialog::okClicked()
{
    if (m_frameConnectSelector)
        m_frameConnectSelector->save();

    // create the master command
    class MasterCommand : public KUndo2Command
    {
    public:
        MasterCommand(KWAnchoringProperties *anchoringProperties, KWCanvas *canvas)
            : KUndo2Command(kundo2_i18n("Change Shape Properties"))
            , m_anchoringProperties(anchoringProperties)
            , m_first(true)
            , m_canvas(canvas)
        {
        }

        void redo() override
        {
            if (m_first) {
                m_first = false;
                KUndo2Command::redo();
                m_anchoringProperties->save(this, m_canvas);
            } else {
                KUndo2Command::redo();
            }
        }
        KWAnchoringProperties *m_anchoringProperties;
        bool m_first;
        KWCanvas *m_canvas;
    };

    MasterCommand *macro = new MasterCommand(m_anchoringProperties, m_canvas);

    // these we can just add as children as they don't deal with kotexteditor
    m_runAroundProperties->save(macro);

    m_canvas->addCommand(macro);
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
    return answer;
}
